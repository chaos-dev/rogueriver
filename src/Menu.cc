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
 
#include "Menu.h"

#include <iostream>

#include "BearLibTerminal.h"

#include "Engine.h"
 
Menu::~Menu() {
	clear();
}

void Menu::clear() {
	items.clear();
}

void Menu::addItem(MenuItemCode code, const char *label) {
	MenuItem *item=new MenuItem();
	item->code=code;
	item->label=label;
	items.push_back(item);
}

const int PAUSE_MENU_WIDTH=16;
const int PAUSE_MENU_HEIGHT=11;
Menu::MenuItemCode Menu::pick(DisplayMode mode) {
	int selectedItem=0;
	int menux,menuy;
	if (mode == PAUSE) {
		menux=terminal_state(TK_WIDTH)/2-PAUSE_MENU_WIDTH/2;
		menuy=terminal_state(TK_HEIGHT)/2-PAUSE_MENU_HEIGHT/2;
		
		// Print out a frame
		for (int i=0; i<12; i++) {
		  terminal_layer(i);
      terminal_bkcolor("black");
      terminal_clear_area(menux, menuy, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT);
      terminal_bkcolor("darkest gray");
      terminal_clear_area(menux+2, menuy+1, PAUSE_MENU_WIDTH-4, PAUSE_MENU_HEIGHT-2);
    };
		
		menux+=3;
		menuy+=2;
	} else {
		menux=10;
		menuy=terminal_state(TK_HEIGHT)/3;
	  terminal_layer(Engine::MAP);
	  terminal_clear();
	  terminal_bkcolor("darkest gray");
    terminal_clear_area(4, 2, terminal_state(TK_WIDTH)-8, terminal_state(TK_HEIGHT)-4);
    terminal_print(menux,menuy-4,"[color=crimson]Rogue River:\nObol of Charon");
	  terminal_set("U+E200: graphics/menu_background.jpg, resize=700x500");
	  terminal_put(35, 6, 0xE200); // Background
	}
	terminal_refresh();
  
  terminal_layer(Engine::PAUSE_MENU);
  bool exit = false;
  while (!exit) {
  	int currentItem=0;
  	for (MenuItem* item : items) {
			if ( currentItem == selectedItem ) {
				terminal_color(color_from_name("dark orange"));
			} else {
				terminal_color(color_from_name("light grey"));
			}
			terminal_print(menux,menuy+currentItem*3,item->label);
			currentItem++;
		}
		terminal_refresh();
		
		// Read input (This BLOCKS all other controls)
    int key = terminal_read();
    if (key == TK_UP) {
  		selectedItem--; 
			if (selectedItem < 0) {
				selectedItem=items.size()-1;
			}
    } else if (key == TK_DOWN) {
      selectedItem = (selectedItem + 1) % items.size(); 
    } else if (key == TK_ENTER) {
      return items.at(selectedItem)->code;
    } else if (key == TK_CLOSE || key == TK_ESCAPE) {
      exit = true;
    }
  };
	return NONE;
}
