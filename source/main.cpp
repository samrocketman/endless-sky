/* main.cpp
Copyright (c) 2014 by Michael Zahniser

Main function for Endless Sky, a space exploration and combat RPG.

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Audio.h"
#include "Command.h"
#include "Conversation.h"
#include "ConversationPanel.h"
#include "DataFile.h"
#include "DataNode.h"
#include "DataWriter.h"
#include "Dialog.h"
#include "Files.h"
#include "text/Font.h"
#include "FrameTimer.h"
#include "GameData.h"
#include "GameLoadingPanel.h"
#include "GameWindow.h"
#include "Hardpoint.h"
#include "Logger.h"
#include "MenuPanel.h"
#include "Panel.h"
#include "PlayerInfo.h"
#include "Plugins.h"
#include "Preferences.h"
#include "PrintData.h"
#include "Screen.h"
#include "SpriteSet.h"
#include "SpriteShader.h"
#include "Test.h"
#include "TestContext.h"
#include "UI.h"

#include <chrono>
#include <iostream>
#include <map>
#include <thread>

#include <cassert>
#include <future>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#endif

namespace {
	// The delay in frames when debugging the integration tests.
	constexpr int UI_DELAY = 60;
}

using namespace std;

void PrintHelp();
void PrintVersion();
void GameLoop(PlayerInfo &player, const Conversation &conversation, const string &testToRun, bool debugMode);
Conversation LoadConversation();
void PrintTestsTable();
#ifdef _WIN32
void InitConsole();
#endif



// Entry point for the EndlessSky executable
int main(int argc, char *argv[])
{
	// Handle command-line arguments
#ifdef _WIN32
	if(argc > 1)
		InitConsole();
#endif
	Conversation conversation;
	bool debugMode = false;
	bool loadOnly = false;
	bool printTests = false;
	bool printData = false;
	bool noTestMute = false;
	string testToRunName = "";

	// Ensure that we log errors to the errors.txt file.
	Logger::SetLogErrorCallback([](const string &errorMessage) { Files::LogErrorToFile(errorMessage); });

	for(const char *const *it = argv + 1; *it; ++it)
	{
		string arg = *it;
		if(arg == "-h" || arg == "--help")
		{
			PrintHelp();
			return 0;
		}
		else if(arg == "-v" || arg == "--version")
		{
			PrintVersion();
			return 0;
		}
		else if(arg == "-t" || arg == "--talk")
			conversation = LoadConversation();
		else if(arg == "-d" || arg == "--debug")
			debugMode = true;
		else if(arg == "-p" || arg == "--parse-save")
			loadOnly = true;
		else if(arg == "--test" && *++it)
			testToRunName = *it;
		else if(arg == "--tests")
			printTests = true;
		else if(arg == "--nomute")
			noTestMute = true;
	}
	printData = PrintData::IsPrintDataArgument(argv);
	Files::Init(argv);

	try {
		// Load plugin preferences before game data if any.
		Plugins::LoadSettings();

		// Begin loading the game data.
		bool isConsoleOnly = loadOnly || printTests || printData;
		future<void> dataLoading = GameData::BeginLoad(isConsoleOnly, debugMode);

		// If we are not using the UI, or performing some automated task, we should load
		// all data now. (Sprites and sounds can safely be deferred.)
		if(isConsoleOnly || !testToRunName.empty())
			dataLoading.wait();

		if(!testToRunName.empty() && !GameData::Tests().Has(testToRunName))
		{
			Logger::LogError("Test \"" + testToRunName + "\" not found.");
			return 1;
		}

		if(printData)
		{
			PrintData::Print(argv);
			return 0;
		}
		if(printTests)
		{
			PrintTestsTable();
			return 0;
		}

		PlayerInfo player;
		if(loadOnly)
		{
			// Set the game's initial internal state.
			GameData::FinishLoading();

			// Reference check the universe, as known to the player. If no player found,
			// then check the default state of the universe.
			if(!player.LoadRecent())
				GameData::CheckReferences();
			cout << "Parse completed." << endl;
			return 0;
		}
		assert(!isConsoleOnly && "Attempting to use UI when only data was loaded!");

		// On Windows, make sure that the sleep timer has at least 1 ms resolution
		// to avoid irregular frame rates.
#ifdef _WIN32
		timeBeginPeriod(1);
#endif

		Preferences::Load();

		// Load global conditions:
		DataFile globalConditions(Files::Config() + "global conditions.txt");
		for(const DataNode &node : globalConditions)
			if(node.Token(0) == "conditions")
				GameData::GlobalConditions().Load(node);

		if(!GameWindow::Init())
			return 1;

		GameData::LoadShaders(!GameWindow::HasSwizzle());

		// Show something other than a blank window.
		GameWindow::Step();

		Audio::Init(GameData::Sources());

		if(!testToRunName.empty() && !noTestMute)
		{
			Audio::SetVolume(0);
		}

		// This is the main loop where all the action begins.
		GameLoop(player, conversation, testToRunName, debugMode);
	}
	catch(Test::known_failure_tag)
	{
		// This is not an error. Simply exit succesfully.
	}
	catch(const runtime_error &error)
	{
		Audio::Quit();
		bool doPopUp = testToRunName.empty();
		GameWindow::ExitWithError(error.what(), doPopUp);
		return 1;
	}

	// Remember the window state and preferences if quitting normally.
	Preferences::Set("maximized", GameWindow::IsMaximized());
	Preferences::Set("fullscreen", GameWindow::IsFullscreen());
	Screen::SetRaw(GameWindow::Width(), GameWindow::Height());
	Preferences::Save();
	Plugins::Save();

	Audio::Quit();
	GameWindow::Quit();

	return 0;
}



void GameLoop(PlayerInfo &player, const Conversation &conversation, const string &testToRunName, bool debugMode)
{
	// gamePanels is used for the main panel where you fly your spaceship.
	// All other game content related dialogs are placed on top of the gamePanels.
	// If there are both menuPanels and gamePanels, then the menuPanels take
	// priority over the gamePanels. The gamePanels will not be shown until
	// the stack of menuPanels is empty.
	UI gamePanels;

	// menuPanels is used for the panels related to pilot creation, preferences,
	// game loading and game saving.
	UI menuPanels;

	// Whether the game data is done loading. This is used to trigger any
	// tests to run.
	bool dataFinishedLoading = false;
	menuPanels.Push(new GameLoadingPanel(player, conversation, gamePanels, dataFinishedLoading));

	bool showCursor = true;
	int cursorTime = 0;
	int frameRate = 60;
	FrameTimer timer(frameRate);
	bool isPaused = false;
	bool isFastForward = false;
	int testDebugUIDelay = UI_DELAY;

	// If fast forwarding, keep track of whether the current frame should be drawn.
	int skipFrame = 0;

	// Limit how quickly full-screen mode can be toggled.
	int toggleTimeout = 0;

	// Data to track progress of testing if/when a test is running.
	TestContext testContext;
	if(!testToRunName.empty())
		testContext = TestContext(GameData::Tests().Get(testToRunName));

	// IsDone becomes true when the game is quit.
	while(!menuPanels.IsDone())
	{
		if(toggleTimeout)
			--toggleTimeout;
		chrono::steady_clock::time_point start = chrono::steady_clock::now();

		// Handle any events that occurred in this frame.
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			UI &activeUI = (menuPanels.IsEmpty() ? gamePanels : menuPanels);

			// If the mouse moves, reset the cursor movement timeout.
			if(event.type == SDL_MOUSEMOTION)
				cursorTime = 0;

			if(debugMode && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKQUOTE)
			{
				isPaused = !isPaused;
			}
			else if(event.type == SDL_KEYDOWN && menuPanels.IsEmpty()
					&& Command(event.key.keysym.sym).Has(Command::MENU)
					&& !gamePanels.IsEmpty() && gamePanels.Top()->IsInterruptible())
			{
				// User pressed the Menu key.
				menuPanels.Push(shared_ptr<Panel>(
					new MenuPanel(player, gamePanels)));
			}
			else if(event.type == SDL_QUIT)
			{
				menuPanels.Quit();
			}
			else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				// The window has been resized. Adjust the raw screen size
				// and the OpenGL viewport to match.
				GameWindow::AdjustViewport();
			}
			else if(activeUI.Handle(event))
			{
				// The UI handled the event.
			}
			else if(event.type == SDL_KEYDOWN && !toggleTimeout
					&& (Command(event.key.keysym.sym).Has(Command::FULLSCREEN)
					|| (event.key.keysym.sym == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))))
			{
				toggleTimeout = 30;
				Preferences::ToggleScreenMode();
			}
			else if(event.type == SDL_KEYDOWN && !event.key.repeat
					&& (Command(event.key.keysym.sym).Has(Command::FASTFORWARD)))
			{
				isFastForward = !isFastForward;
			}
		}
		SDL_Keymod mod = SDL_GetModState();
		Font::ShowUnderlines(mod & KMOD_ALT);

		// In full-screen mode, hide the cursor if inactive for ten seconds,
		// but only if the player is flying around in the main view.
		bool inFlight = (menuPanels.IsEmpty() && gamePanels.Root() == gamePanels.Top());
		++cursorTime;
		bool shouldShowCursor = (!GameWindow::IsFullscreen() || cursorTime < 600 || !inFlight);
		if(shouldShowCursor != showCursor)
		{
			showCursor = shouldShowCursor;
			SDL_ShowCursor(showCursor);
		}

		// Switch off fast-forward if the player is not in flight or flight-related screen
		// (for example when the boarding dialog shows up or when the player lands). The player
		// can switch fast-forward on again when flight is resumed.
		bool allowFastForward = !gamePanels.IsEmpty() && gamePanels.Top()->AllowsFastForward();
		if(Preferences::Has("Interrupt fast-forward") && !inFlight && isFastForward && !allowFastForward)
			isFastForward = false;

		// Tell all the panels to step forward, then draw them.
		((!isPaused && menuPanels.IsEmpty()) ? gamePanels : menuPanels).StepAll();

		// All manual events and processing done. Handle any test inputs and events if we have any.
		const Test *runningTest = testContext.CurrentTest();
		if(runningTest && dataFinishedLoading)
		{
			// When flying around, all test processing must be handled in the
			// thread-safe section of Engine. When not flying around (and when no
			// Engine exists), then it is safe to execute the tests from here.
			auto mainPanel = gamePanels.Root();
			if(!isPaused && inFlight && menuPanels.IsEmpty() && mainPanel)
				mainPanel->SetTestContext(testContext);
			else if(debugMode && testDebugUIDelay > 0)
				--testDebugUIDelay;
			else
			{
				// The command will be ignored, since we only support commands
				// from within the engine at the moment.
				Command ignored;
				runningTest->Step(testContext, player, ignored);
				// Reset the visual delay.
				testDebugUIDelay = UI_DELAY;
			}
			// Skip drawing 29 out of every 30 in-flight frames during testing to speedup testing (unless debug mode is set).
			// We don't skip UI-frames to ensure we test the UI code more.
			if(inFlight && !debugMode)
			{
				skipFrame = (skipFrame + 1) % 30;
				if(skipFrame)
					continue;
			}
			else
				skipFrame = 0;
		}
		// Caps lock slows the frame rate in debug mode.
		// Slowing eases in and out over a couple of frames.
		else if((mod & KMOD_CAPS) && inFlight && debugMode)
		{
			if(frameRate > 10)
			{
				frameRate = max(frameRate - 5, 10);
				timer.SetFrameRate(frameRate);
			}
		}
		else
		{
			if(frameRate < 60)
			{
				frameRate = min(frameRate + 5, 60);
				timer.SetFrameRate(frameRate);
			}

			if(isFastForward && inFlight)
			{
				skipFrame = (skipFrame + 1) % 3;
				if(skipFrame)
					continue;
			}
		}

		Audio::Step();

		// Events in this frame may have cleared out the menu, in which case
		// we should draw the game panels instead:
		(menuPanels.IsEmpty() ? gamePanels : menuPanels).DrawAll();
		if(isFastForward)
			SpriteShader::Draw(SpriteSet::Get("ui/fast forward"), Screen::TopLeft() + Point(10., 10.));

		GameWindow::Step();

		// When we perform automated testing, then we run the game by default as quickly as possible.
		// Except when debug-mode is set.
		if(!testContext.CurrentTest() || debugMode)
			timer.Wait();

		// If the player ended this frame in-game, count the elapsed time as played time.
		if(menuPanels.IsEmpty())
			player.AddPlayTime(chrono::steady_clock::now() - start);
	}

	// If player quit while landed on a planet, save the game if there are changes.
	if(player.GetPlanet() && gamePanels.CanSave())
		player.Save();
}



void PrintHelp()
{
	cerr << endl;
	cerr << "Command line options:" << endl;
	cerr << "    -h, --help: print this help message." << endl;
	cerr << "    -v, --version: print version information." << endl;
	cerr << "    -t, --talk: read and display a conversation from STDIN." << endl;
	cerr << "    -r, --resources <path>: load resources from given directory." << endl;
	cerr << "    -c, --config <path>: save user's files to given directory." << endl;
	cerr << "    -d, --debug: turn on debugging features (e.g. Caps Lock slows down instead of speeds up)." << endl;
	cerr << "    -p, --parse-save: load the most recent saved game and inspect it for content errors." << endl;
	cerr << "    --tests: print table of available tests, then exit." << endl;
	cerr << "    --test <name>: run given test from resources directory." << endl;
	cerr << "    --nomute: don't mute the game while running tests." << endl;
	PrintData::Help();
	cerr << endl;
	cerr << "Report bugs to: <https://github.com/endless-sky/endless-sky/issues>" << endl;
	cerr << "Home page: <https://endless-sky.github.io>" << endl;
	cerr << endl;
}



void PrintVersion()
{
	cerr << endl;
	cerr << "Endless Sky ver. 0.10.1-alpha" << endl;
	cerr << "License GPLv3+: GNU GPL version 3 or later: <https://gnu.org/licenses/gpl.html>" << endl;
	cerr << "This is free software: you are free to change and redistribute it." << endl;
	cerr << "There is NO WARRANTY, to the extent permitted by law." << endl;
	cerr << endl;
	cerr << GameWindow::SDLVersions() << endl;
	cerr << endl;
}



Conversation LoadConversation()
{
	Conversation conversation;
	DataFile file(cin);
	for(const DataNode &node : file)
		if(node.Token(0) == "conversation")
		{
			conversation.Load(node);
			break;
		}

	map<string, string> subs = {
		{"<bunks>", "[N]"},
		{"<cargo>", "[N tons of Commodity]"},
		{"<commodity>", "[Commodity]"},
		{"<date>", "[Day Mon Year]"},
		{"<day>", "[The Nth of Month]"},
		{"<destination>", "[Planet in the Star system]"},
		{"<fare>", "[N passengers]"},
		{"<first>", "[First]"},
		{"<last>", "[Last]"},
		{"<origin>", "[Origin Planet]"},
		{"<passengers>", "[your passengers]"},
		{"<planet>", "[Planet]"},
		{"<ship>", "[Ship]"},
		{"<system>", "[Star]"},
		{"<tons>", "[N tons]"}
	};
	return conversation.Instantiate(subs);
}



// This prints out the list of tests that are available and their status
// (active/missing feature/known failure)..
void PrintTestsTable()
{
	for(auto &it : GameData::Tests())
		if(it.second.GetStatus() != Test::Status::PARTIAL
				&& it.second.GetStatus() != Test::Status::BROKEN)
			cout << it.second.Name() << '\n';
	cout.flush();
}



#ifdef _WIN32
void InitConsole()
{
	const int UNINITIALIZED = -2;
	bool redirectStdout = _fileno(stdout) == UNINITIALIZED;
	bool redirectStderr = _fileno(stderr) == UNINITIALIZED;
	bool redirectStdin = _fileno(stdin) == UNINITIALIZED;

	// Bail if stdin, stdout, and stderr are already initialized (e.g. writing to a file)
	if(!redirectStdout && !redirectStderr && !redirectStdin)
		return;

	// Bail if we fail to attach to the console
	if(!AttachConsole(ATTACH_PARENT_PROCESS) && !AllocConsole())
		return;

	// Perform console redirection.
	if(redirectStdout)
	{
		FILE *fstdout = nullptr;
		freopen_s(&fstdout, "CONOUT$", "w", stdout);
		if(fstdout)
			setvbuf(stdout, nullptr, _IOFBF, 4096);
	}
	if(redirectStderr)
	{
		FILE *fstderr = nullptr;
		freopen_s(&fstderr, "CONOUT$", "w", stderr);
		if(fstderr)
			setvbuf(stderr, nullptr, _IOLBF, 1024);
	}
	if(redirectStdin)
	{
		FILE *fstdin = nullptr;
		freopen_s(&fstdin, "CONIN$", "r", stdin);
		if(fstdin)
			setvbuf(stdin, nullptr, _IONBF, 0);
	}
}
#endif
