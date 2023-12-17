/* Playlist.cpp
Copyright (c) 2022 by RisingLeaf

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Playlist.h"

#include "DataNode.h"
#include "GameData.h"
#include "PlayerInfo.h"
#include "System.h"

#include <algorithm>

using namespace std;

namespace {
	const Track *currentTrack = nullptr;
}



Playlist::Playlist(const DataNode &node)
{
	Load(node);
}



void Playlist::Load(const DataNode &node)
{
	// All tracks need a name.
	if(node.Size() < 2)
	{
		node.PrintTrace("Error: No name specified for playlist:");
		return;
	}

	if(!name.empty())
	{
		node.PrintTrace("Error: Duplicate definition of playlist:");
		return;
	}
	name = node.Token(1);

	for(const DataNode &child : node)
	{
		if(child.Token(0) == "to")
		{
			if(child.Token(1) == "play")
				toPlay.Load(child);
		}
		else if(child.Token(0) == "location")
			location.Load(child);
		else if(child.Token(0) == "priority" && child.Size() >= 2)
			priority = child.Value(1);
		else if(child.Token(0) == "weight" && child.Size() >= 2)
			weight = max<int>(1, child.Value(1));
		else if(child.Token(0) == "tracks")
		{
			std::vector<std::string> progressionStyles = {"random", "linear", "pick"};
			bool validProgressionStyle = (child.Size() >= 2) ?
					count(progressionStyles.begin(), progressionStyles.end(), child.Token(1)) :
					false;
			if(validProgressionStyle)
				progressionStyle = child.Token(1);
			else
			{
				if(child.Size() >= 2)
					child.PrintTrace("Warning: \"" + child.Token(1) + "\" is not a valid progression style so using linear:");
				progressionStyle = "linear";
			}
			for(const auto &grand : child)
			{
				int trackWeight = (grand.Size() >= 2) ? max<int>(1, grand.Value(1)) : 1;
				tracks.emplace_back(trackWeight, GameData::Tracks().Get(grand.Token(0)));
			}
		}
	}
}



void Playlist::Activate() const
{
	// Linear should always get the first track in the list when activating.
	if(progressionStyle == "linear" && tracks.size() > 0)
		currentTrack = *tracks.begin();
	else
		currentTrack = tracks.Get();
}



const Track *Playlist::GetCurrentTrack() const
{
	if(progressionStyle == "linear")
	{
		const Track * tmpTrack = currentTrack;
		auto it = find(tracks.begin(), tracks.end(), tmpTrack);
		++it;
		if(it == tracks.end())
			it = tracks.begin();
		currentTrack = *it;
		return tmpTrack;
	}
	else if(progressionStyle == "pick")
		return currentTrack;

	// Asuming the progression style is "random".
	return tracks.Get();
}



bool Playlist::MatchingConditions(PlayerInfo &player) const
{
	if(player.GetPlanet())
		if(!location.Matches(player.GetPlanet()))
			return false;
	return toPlay.Test(player.Conditions()) && location.Matches(player.GetSystem());
}



int Playlist::Priority() const
{
	return priority;
}



int Playlist::Weight() const
{
	return weight;
}
