/* PlayerHailPanel.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef PLAYER_HAIL_PANEL_H_
#define PLAYER_HAIL_PANEL_H_

#include "Angle.h"
#include "HailPanel.h"
#include "Point.h"

#include <cstdint>
#include <memory>
#include <string>

class Government;
class Planet;
class PlayerInfo;
class Ship;
class Sprite;
class StellarObject;



// This panel is shown when you hail a ship or planet. It allows you to ask for
// assistance from friendly ships, to bribe hostile ships to leave you alone, or
// to bribe a planet to allow you to land there.
class PlayerHailPanel : public HailPanel {
public:
	PlayerHailPanel(PlayerInfo &player, const std::shared_ptr<Ship> &ship,
		std::function<void(const Government *)> bribeCallback);
	PlayerHailPanel(PlayerInfo &player, const StellarObject *object);

	virtual void Draw() override;


protected:
	// Only override the ones you need; the default action is to return false.
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;


private:
	void SetBribe(double scale);


private:
	PlayerInfo &player;
	std::shared_ptr<Ship> ship = nullptr;
	std::function<void(const Government *)> bribeCallback = nullptr;
	const Planet *planet = nullptr;
	const Sprite *sprite = nullptr;
	Angle facing;

	int64_t bribe = 0;
	const Government *bribed = nullptr;
	bool playerNeedsHelp = false;
	bool canAssistPlayer = true;
	bool canGiveFuel = false;
	bool canRepair = false;
	bool hasLanguage = true;
	bool requestedToBribeShip = false;
};



#endif
