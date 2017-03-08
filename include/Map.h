/**
 *  \brief
 *
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

#ifndef INCLUDE_MAP_H_
#define INCLUDE_MAP_H_

#include "River.h"

struct Position {
    int x, y;
    Position() : x(0), y(0) {};
    Position(int x, int y) : x(x), y(y) {};
};

struct Panel {
    Position tl_corner;
    Position br_corner;
    int width, height;
    Panel(Position tl_corner, Position br_corner)
        : tl_corner(tl_corner), br_corner(br_corner) {};
    Panel() {};
    void Update(int x1, int y1, int x2, int y2) {
        tl_corner.x = x1; tl_corner.y = y1;
        br_corner.x = x2; br_corner.y = y2;
        width = x2 - x1 + 1;
        height = y2 - y1 + 1;
    };
};

struct Tile {
    bool canWalk;
    float vel;
    Tile() : canWalk(true), vel(0.0) {};
};

class Map {
 protected:
  std::vector<Tile> tiles;
  void SetWall(int x, int y);
  bool inBounds(int x, int y) const;
  River* river;
 public:
  int width, height;
  Position camera;

  Map(int width, int height);
  bool isWall(int x, int y) const;
  void Render(Panel panel, Position* camera) const;
};



#endif /* INCLUDE_MAP_H_ */