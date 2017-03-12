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

#include <deque>
#include <random>

#include "Map.h"
#include "Actor.h"
#include "Ai.h"
#include "Gui.h"

class Engine {
 protected:
  const int MAP_WIDTH = 800;
  const int MAP_HEIGHT = 500;
  const int SIDEBAR_WIDTH = 40;
  int width, height;
  Panel map_panel;
  const int symbol = 0x2588;
  enum Status {OPEN,
               CLOSED} status;

  void ProcessInput();
  void Update();
  void UpdateMouse();
  void Render();
  void RenderActor(Actor* actor);
  bool PickATile(int key, int *x, int *y, int max_range);

 public:
  const int NEXT_LEVEL_POINT = 40;
  int level;
  Actor* player;
  Actor* raft;
  Map* map;
  Position* camera;
  Position* mouse;
  Gui* gui;
  std::deque<Actor*> actors;
  std::mt19937 rng;  // Random number generator
  enum GameStatus {
    STARTUP,
    IDLE,
    NEW_TURN,
    AIMING,
    VICTORY,
    DEFEAT
  } game_status;

  Engine();
  ~Engine();
  void Init();
  void Run();
  void Term();
  void NextLevel();
  void Load(bool pause=false);
  bool CursorOnMap();
};

extern Engine engine;

#endif /* INCLUDE_ENGINE_H_ */
