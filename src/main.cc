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
#include <cmath>
#include <random>

#include "BearLibTerminal.h"

struct Color {
    int r, g, b;
    Color(int r, int g, int b) : r(r), g(g), b(b) {};
};

Color RIVER_COLOR(4,69,143);
Color BG_COLOR(91,135,20);
Color BEACH_COLOR(166,157,12);

class River {
 protected:
  int length;
  float min_width = 15;
  float max_width = 40;
  float min_travel = 40;
  float max_travel = 80;
  int num_periods = 3;
  std::vector<float> width;
  std::vector<float> shape;
  std::mt19937 rng;
  
  std::vector<float> RandomSignal(int n, float y_min, float y_max, float min_period, float max_period, int num_periods);
 public:
  River(int length);
  float GetVelocity(int x, int y);
  bool isBeach(int x, int y);
};

River::River(int length) : length(length) {
  width.resize(length);
  shape.resize(length);
  rng.seed(std::random_device()());
  
  // Create the river
  width = RandomSignal(length, min_width, max_width, length/2.0, length*2.0, num_periods);
  shape = RandomSignal(length, min_travel, max_travel, length/3.0, length*2.0, num_periods);
};

std::vector<float> River::RandomSignal(int n, float y_min, float y_max, 
                                       float min_period, float max_period, 
                                       int num_periods) {
  const float pi = std::atan(1)*4;
  float min_log = std::log(min_period);
  float max_log = std::log(max_period);
  std::vector<float> periods(num_periods);
  std::vector<float> shifts(num_periods);
  
  std::uniform_real_distribution<> dist(0,1);
  periods[0] = std::exp(dist(rng)*0.25*(max_log - min_log) + min_log);
  periods[1] = std::exp(dist(rng)*(max_log - min_log) + min_log);
  periods[2] = std::exp((0.75 + dist(rng)*0.25)*(max_log - min_log) + min_log);
  for (int i = 0; i<num_periods; i++)
    shifts[i] = dist(rng)*2.0*pi;
    
  std::vector<float> signal(n);
  for (int i = 0; i<n; i++) {
    signal[i] = 0.5*(y_max + y_min);
    for (int j=0; j<num_periods; j++) {
      signal[i] += 0.5*(y_max-y_min)/num_periods * std::sin(2.0*pi*i/periods[j] + shifts[j]);
    }
  }
  
  return signal;
}

float River::GetVelocity(int x, int y) {
  int i = (int)x;
  float rescaled = (y - shape[i])/(width[i]/2.0);
  if (rescaled < 1.0 && rescaled > -1.0) {
    return 1.0 - rescaled*rescaled;
  } else {
    return 0.0;
  }
};

bool River::isBeach(int x, int y) {
  int i = (int)x;
  float rescaled = std::abs(y - shape[i])/(width[i]/2.0);
  if (rescaled > 1.0 && rescaled < 1.2) {
    return true;
  } else {
    return false;
  }
};

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
  for (int term_x=panel.tl_corner.x; term_x < panel.br_corner.x; term_x++) {
    for (int term_y=panel.tl_corner.y; term_y < panel.br_corner.y; term_y++) {
      int game_x = term_x + camera->x - panel.width/2;
      int game_y = height - (term_y + height-camera->y - panel.height/2);
        if (inBounds(game_x,game_y)) {
          if (tiles[game_x + game_y*width].vel > 0) {
            terminal_print(term_x, term_y, "[bkcolor=azure]~");
          } else if (river->isBeach(game_x, game_y)) {
            terminal_print(term_x, term_y, "[bkcolor=sand].");
          }
        }
    }
  }
};

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

Engine::Engine() {
	terminal_open();
	// Terminal settings
	terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, minimum-size=80x24; font: graphics/VeraMono.ttf, size=16x16");
	terminal_bkcolor(color_from_argb(255, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b));
	terminal_set("palette.sand = #a69d7b");
	
	// Initialize engine state
	width = terminal_state(TK_WIDTH);
    height = terminal_state(TK_HEIGHT);
    status = OPEN;
    
    // Initialize members
    map = new Map(MAP_WIDTH, MAP_HEIGHT);
	map_panel.Update(0, 0, width-1, height-1);
	camera = new Position(width/2, height/2);
    player = new Actor(width/2, height/2, (int)'@');
};

Engine::~Engine() {
	terminal_close();
};

void Engine::ProcessInput() {
    while (terminal_has_input()) {
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
	}
};

void Engine::Render() {
	terminal_clear();
	map->Render(map_panel, camera);
	terminal_put(player->x - camera->x + map_panel.width/2,
	             player->y - camera->y + map_panel.height/2, player->symbol);
	terminal_printf(map_panel.width/2-5, 0, "X: %d  Y: %d", player->x, player->y);
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
