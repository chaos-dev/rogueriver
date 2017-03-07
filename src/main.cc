/**
 *  brief: Explore procedurally generated rivers in the Greek Underworld
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

#include <vector>
#include "stdlib.h"
#include "time.h"
#include "BearLibTerminal.h"

struct Tile {
    bool canWalk;
    Tile() : canWalk(true) {};
};

class Map {
 protected:
  std::vector<Tile> tiles;
  void SetWall(int x, int y);
 public:
  int width, height;
  Map(int width, int height);
  bool isWall(int x, int y) const;
  void Render() const;
};

Map::Map(int width, int height) : width(width), height(height) {
  tiles.resize(width*height);
  srand(time(NULL));
  for (int i; i<30; i++) {
    int wall_x = rand()*width;
    int wall_y = rand()*height;
    SetWall(wall_x, wall_y);
  };
};

bool Map::isWall(int x, int y) const {
    return !tiles.at(x+y*width).canWalk;
};

void Map::SetWall(int x, int y) {
  tiles.at(x+y*width).canWalk = false;
};

void Map::Render() const {
  for (int x=0; x < width; x++) {
    for (int y=0; y < height; y++) {
      terminal_put(x, y, isWall(x,y)? (int)'#': (int)'.');
    }
  }
};

class Engine {
 protected:
  int width, height;
  Map* map;
  const int symbol = 0x2588;
  enum Status {OPEN, 
               CLOSED} status;
  void ProcessInput();
  void Update();
  void Render();
 public:
  Engine();
  ~Engine();
  void Run();
};

Engine::Engine() {
	terminal_open();
	terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, minimum-size=80x24");
	width = terminal_state(TK_WIDTH);
    height = terminal_state(TK_HEIGHT);
    status = OPEN;
    map = new Map(width, height);
};

Engine::~Engine() {
	terminal_close();
};

void Engine::ProcessInput() {
	int key = terminal_read();

	if (key == TK_CLOSE || key == TK_ESCAPE) {
		status = CLOSED;
	}
};

void Engine::Render() {
	terminal_clear();
	for (int x=0; x<width; x++) {
		terminal_put(x, 0, x%2? symbol: (int)'#');
		terminal_put(x, height-1, x%2? symbol: (int)'#');
	}
	for (int y=0; y<height; y++) {
		terminal_put(0, y, y%2? symbol: (int)'#');
		terminal_put(width-1, y, y%2? symbol: (int)'#');
	}
	map->Render();
	terminal_refresh();
};

void Engine::Update() {
	width = terminal_state(TK_WIDTH);
	height = terminal_state(TK_HEIGHT);
};

void Engine::Run() {
	while (status == OPEN) {
        Update();
        Render();
        ProcessInput();
	}
};

int main()
{
   Engine engine;
   engine.Run();
	
   return 0;
}
