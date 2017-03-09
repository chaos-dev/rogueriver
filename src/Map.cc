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
#include "Color.h"

Map::Map(int width, int height)
    : width(width), height(height), 
      water_color(4,69,143), beach_color(166,157, 123), bg_color(91,135,20) {
  tiles.resize(height*width);
  river = new River(width);
  for (int x=0; x<width; x++) {
    for (int y=0; y<height; y++) {
      float vel = river->GetVelocity(x,y);
      tiles[x + y*width].vel = vel;
      if (vel > 0) {
        tiles[x + y*width].color = water_color*vel + beach_color*(1.0-vel);
      } else if (river->isBeach(x, y)) {
        tiles[x + y*width].color = beach_color;
      } else {
        tiles[x + y*width].color = bg_color;
      }
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
  
  color_t corner_colors[4];
  for (int term_x=panel.tl_corner.x; term_x < panel.br_corner.x; term_x++) {
    for (int term_y=panel.tl_corner.y; term_y < panel.br_corner.y; term_y++) {
      int game_x = term_x/2 + camera->x - panel.width/4;
      int game_y = height - (term_y + height-camera->y - panel.height/2);
      if (inBounds(game_x, game_y)) {
        for (int corner = 0; corner<4; corner++)
          corner_colors[corner] = tiles[game_x + game_y*width].color.Convert();
        terminal_put_ext(term_x, term_y, 0, 0, 0x2588, corner_colors);
      }
    }
  }
};


