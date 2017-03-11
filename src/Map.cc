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

#include <cmath>
#include <random>

#include "BearLibTerminal.h"
#include "Color.h"
#include "Actor.h"
#include "Engine.h"

Map::Map(int width, int height)
    : width(width), height(height) {
};

Map::~Map() {
    delete river;
    tiles.clear();
};

void Map::Init(bool withActors) {
  SetColors();
  tiles.resize(height*width);
  river = new River(width);
  for (int x=0; x<width; x++) {
    for (int y=0; y<height; y++) {
      float vel = river->GetVelocity(x,y);
      tiles[x + y*width].vel = vel;
      tiles[x + y*width].u = vel*std::cos(river->angle[x]);
      tiles[x + y*width].v = vel*std::sin(river->angle[x]);
      if (vel > 0) {
        tiles[x + y*width].color = water_color*(vel/river->max_velocity) + 
                                   beach_color*(1.0-(vel/river->max_velocity));
      } else if (river->isBeach(x, y)) {
        tiles[x + y*width].color = beach_color;
      } else {
        tiles[x + y*width].color = bg_color;
      }
    }
  }
  std::uniform_real_distribution<> dist(0,1);
  int num_enemies = (int)dist(engine.rng)*20;
  while (num_enemies > 0) {
    int x = (int)(dist(engine.rng)*width);
    int y = (int)(dist(engine.rng)*height);
    
    Position player = GetPlayerStart();
    if ((player.x-x)*(player.x-x)+(player.y-y)*(player.y-y) < 900) continue;
    
    if (CanWalk(x,y)) {
        AddMonster(x,y);
        Actor* new_monster = engine.actors.back();
        if (isWater(x,y) && !new_monster->can_fly) {
            engine.actors.pop_back();
            delete new_monster;
        } else {
            num_enemies--;
        }
    };
  };
};

void Map::SetColors() {
  switch (engine.level) {
    case 1:
      water_color.Update(4,69,143);
      beach_color.Update(166,157,123);
      bg_color.Update(91,135,20);
      break;
    case 2:
      water_color.Update(23,61,64);
      beach_color.Update(127,128,132);
      bg_color.Update(83,108,76);
      break;
    case 3:
      water_color.Update(23,61,64);
      beach_color.Update(117,122,100);
      bg_color.Update(71,51,40);
      break;
    case 4:
      water_color.Update(23,61,64);
      beach_color.Update(107,83,49);
      bg_color.Update(50,36,23);
      break;
    case 5:
      water_color.Update(92,10,12);
      bg_color.Update(24,12,14);
      beach_color.Update(59,64,60);
      break;
    default:
      break;
  }
};

bool Map::isWall(int x, int y) const {
  if (inBounds(x,y)) {
    return !tiles.at(x+y*width).canWalk;
  } else {
    return false;
  }
};

bool Map::isWater(int x, int y) const {
  if (inBounds(x,y)) {
    return (tiles.at(x+y*width).vel > 1e-6);
  } else {
    return false;
  }
};

bool Map::isBeach(int x, int y) const {
    return river->isBeach(x,y);
};

void Map::SetWall(int x, int y) {
  if (inBounds(x,y)) tiles.at(x+y*width).canWalk = false;
};

bool Map::inBounds(int x, int y) const {
    return ((x >= 0) && (x < width) && (y >= 0) && (y < height));
}

void Map::Render(Panel panel, Position* camera) const {
  color_t corner_colors[4];
  Color color;
  for (int term_x=panel.tl_corner.x; term_x < panel.br_corner.x; term_x++) {
    for (int term_y=panel.tl_corner.y; term_y < panel.br_corner.y; term_y++) {
      int game_x = term_x/2 + camera->x - panel.width/4;
      int game_y = height - (term_y + height-camera->y - panel.height/2);
      if (inBounds(game_x, game_y)) {
        for (int corner = 0; corner<4; corner++) {
          color = tiles[game_x + game_y*width].color;
          if (engine.game_status == Engine::AIMING) {
            if (engine.player->GetDistance(game_x, game_y) <= 
                engine.player->attacker->max_range)
              color = color*float(.9) + Color(255,255,255)*float(.1);
          }
          corner_colors[corner] = color.Convert();
        }
        terminal_put_ext(term_x, term_y, 0, 0, 0x2588, corner_colors);
      }
    }
  }
};

Position Map::GetPlayerStart() const {
    Position position;
    position.x = 50;
    position.y = river->GetPlayerStart(50);
    return position;
};

float Map::GetUVelocity(int x, int y) const {
    if (inBounds(x,y)) {
        return tiles[x+y*width].u;
    } else {
        return 0.0;
    };
};

float Map::GetVVelocity(int x, int y) const{
    if (inBounds(x,y)) {
        return tiles[x+y*width].v;
    } else {
        return 0.0;
    };
};

bool Map::CanWalk(int x, int y) const {
  if (isWall(x,y)) {
    // this is a wall
    return false;
  }
  for (Actor* actor : engine.actors) {
    if ( actor->blocks && actor->x == x && actor->y == y ) {
      return false;
    }
  }
  return true;
}

void Map::AddMonster(int x, int y) {
   std::uniform_real_distribution<> dist(0,100);
   float roll = dist(engine.rng);
   if ( roll < 70 ) {
        Actor *centaur = new Actor(x,y,'c',Color(240,240,240),1);
        centaur->words = new Words("the centaur","The centaur","dead centaur","his","arrow","skin");
        centaur->destructible = new MonsterDestructible(16,0);
        centaur->attacker = new Attacker(6,9,14,150);
        centaur->ai = new MonsterAi();
        engine.actors.push_back(centaur);
    } else {
        Actor *harpy = new Actor(x,y,'h',Color(240,240,240),3);
        harpy->words = new Words("the harpy","The harpy","dead harpy","her","claws","skin");
        harpy->destructible = new MonsterDestructible(26,0);
        harpy->can_fly = true;
        harpy->attacker = new Attacker(14,15,12,0);
        harpy->ai = new MonsterAi();
        engine.actors.push_back(harpy);
    }
};

void Map::AddItem(int x, int y) {
};
