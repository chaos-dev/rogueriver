/**
 *  \brief  
 *  Copyright (C) 2017  Chaos-Dev
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDE_MENU_H_
#define INCLUDE_MENU_H_

#include <vector>
 
class Menu {
public :
	enum MenuItemCode {
		NONE,
		NEW_GAME,
		RESUME,
		EXIT
	};
	enum DisplayMode {
		MAIN,
		PAUSE
	};
	~Menu();
	void clear();
	void addItem(MenuItemCode code, const char *label);
	MenuItemCode pick(DisplayMode mode=MAIN);
protected :
	struct MenuItem {
		MenuItemCode code;
		const char *label;
	};
	std::vector<MenuItem*> items;
};

#endif
