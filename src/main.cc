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

class Actor {
 public:
  int x, y;
  int symbol;
  Actor(int x, int y, int symbol) : x(x), y(y), symbol(symbol) {};
};

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
    Tile() : canWalk(true) {};
};

class Map {
 protected:
  std::vector<Tile> tiles;
  void SetWall(int x, int y);
  bool inBounds(int x, int y) const;
 public:
  int width, height;
  Position camera;

  Map(int width, int height);
  bool isWall(int x, int y) const;
  void Render(Panel panel, Position* camera) const;
};

Map::Map(int width, int height) : width(width), height(height) {
  tiles.resize(height*width);
  SetWall(20,20);
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
  for (int term_x=panel.tl_corner.x; term_x < panel.br_corner.x; term_x++) {
    for (int term_y=panel.tl_corner.y; term_y < panel.br_corner.y; term_y++) {
      int game_x = term_x + camera->x - panel.width/2;
      int game_y = height - (term_y + height-camera->y - panel.height/2);
        if (inBounds(game_x,game_y))
            terminal_put(term_x, term_y, 
                         isWall(game_x, game_y)? (int)'#': (int)'.');
    }
  }
};

class Engine {
 protected:
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

Engine::Engine() {
	terminal_open();
	terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, minimum-size=80x24");
	width = terminal_state(TK_WIDTH);
    height = terminal_state(TK_HEIGHT);
    status = OPEN;
    map = new Map(width, height);
	map_panel.Update(0, 0, width-1, height-1);
	camera = new Position(width/2, height/2);
    player = new Actor(width/2, height/2, (int)'@');
};

Engine::~Engine() {
	terminal_close();
};

void Engine::ProcessInput() {
	int key = terminal_read();

	if (key == TK_CLOSE || key == TK_ESCAPE) {
		status = CLOSED;
	} else if (key == TK_UP) {
	    camera->y += 1; player->y += 1;
	} else if (key == TK_DOWN) {
	    camera->y -= 1; player->y -= 1;
	} else if (key == TK_LEFT) {
	    camera->x -= 1; player->x -= 1;
	} else if (key == TK_RIGHT) {
	    camera->x += 1; player->x += 1;
	}
};

void Engine::Render() {
	terminal_clear();
	map->Render(map_panel, camera);
	terminal_put(player->x - camera->x + map_panel.width/2,
	             player->y - camera->y + map_panel.height/2, player->symbol);
	for (int x=0; x<width; x++) {
		terminal_put(x, 0, x%2? symbol: (int)'#');
		terminal_put(x, height-1, x%2? symbol: (int)'#');
	}
	for (int y=0; y<height; y++) {
		terminal_put(0, y, y%2? symbol: (int)'#');
		terminal_put(width-1, y, y%2? symbol: (int)'#');
	}
	terminal_refresh();
};

void Engine::Update() {
	width = terminal_state(TK_WIDTH);
	height = terminal_state(TK_HEIGHT);
	map_panel.Update(0, 0, width-1, height-1);
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
