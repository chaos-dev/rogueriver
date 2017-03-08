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

#include "Map.h"

#include "BearLibTerminal.h"

Map::Map(int width, int height) : width(width), height(height) {
  tiles.resize(height*width);
  river = new River(width);
  for (int x=0; x<width; x++) {
    for (int y=0; y<height; y++) {
     tiles[x+y*width].vel = river->GetVelocity(x,y);
    }
  }
};

bool Map::isWall(int x, int y) const {
  if (inBounds(x,y)) {
    return !tiles.at(x+y*width).canWalk;
  } else {
    return false;
  }
};

void Map::SetWall(int x, int y) {
  if (inBounds(x,y)) tiles.at(x+y*width).canWalk = false;
};

bool Map::inBounds(int x, int y) const {
    return ((x >= 0) && (x < width) && (y >= 0) && (y < height));
}

void Map::Render(Panel panel, Position* camera) const {
  color_t water_color;
  float vel;
  for (int term_x=panel.tl_corner.x; term_x < panel.br_corner.x; term_x++) {
    for (int term_y=panel.tl_corner.y; term_y < panel.br_corner.y; term_y++) {
      int game_x = term_x + camera->x - panel.width/2;
      int game_y = height - (term_y + height-camera->y - panel.height/2);
      if (inBounds(game_x,game_y)) {
          vel = tiles[game_x + game_y*width].vel;
          if (vel > 0) {
            water_color = color_from_name("water")*vel -
                          color_from_name("beach")*(1.0 - vel);
            terminal_print(term_x, term_y, "[bkcolor=water]~");
          } else if (river->isBeach(game_x, game_y)) {
            terminal_print(term_x, term_y, "[bkcolor=beach].");
          } else {
            terminal_print(term_x, term_y, "[bkcolor=bg] ");
          }
      } else if (inBounds(game_x, 0)) {
        // I set y to '0' so that the y direction is never empty
        terminal_print(term_x, term_y, "[bkcolor=bg] ");
      }
    }
  }
};


