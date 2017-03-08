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

#ifndef INCLUDE_ENGINE_H_
#define INCLUDE_ENGINE_H_

#include "Map.h"
#include "Actor.h"

class Engine {
 protected:
  const int MAP_WIDTH = 500;
  const int MAP_HEIGHT = 120;
  int width, height;
  Map* map;
  Panel map_panel;
  Position* camera;
  const int symbol = 0x2588;
  enum Status {OPEN,
               CLOSED} status;
  Actor* player;

  void ProcessInput();
  void Update();
  void Render();
 public:
  Engine();
  ~Engine();
  void Run();
};



#endif /* INCLUDE_ENGINE_H_ */
