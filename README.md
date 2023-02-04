# Endless Sky Mining Complete Fork

This is a fork of endless sky with new features implemented not available in the
base game.  See download page for list of changes from upstream vanilla game.

# Steam Deck Guidance

This fork provides dedicated support for Steam Deck.

- [A steam deck guide to controls][sdg]
- [How to install this fork on Steam Deck](launch-es.sh) with automatic updates.

[sdg]: https://steamcommunity.com/sharedfiles/filedetails/?id=2882293103

# Installing the game


Download: [mining-complete version with recommended
plugins][complete-with-plugins].

This will launch with all plugins disabled by default.  You can enable plugins
in-game from plugins menu in preferences.

## Building from source
Most development is done on Linux and Windows, using CMake ([build instructions](readme-cmake.md)) to compile the project. For those wishing to use an IDE, project files are provided for [Code::Blocks](https://www.codeblocks.org/) to simplify the project setup, and other IDEs are supported through their respective CMake integration. [SCons](https://scons.org/) was the primary build tool up until 0.9.16, and some files and information continue to be available for it.
For full installation instructions, consult the [Build Instructions](https://github.com/endless-sky/endless-sky/blob/master/readme-developer.md) readme.

Learn more in the [player manual][manual].

See also [upstream endless sky README][es].  If you want to contribute, then
contribute to the upstream project.

[complete-with-plugins]: https://github.com/samrocketman/endless-sky/releases/tag/continuous-with-plugins
[complete]: https://github.com/samrocketman/endless-sky/releases/tag/continuous
[es]: https://github.com/endless-sky/endless-sky
[manual]: https://github.com/endless-sky/endless-sky/wiki/PlayersManual
