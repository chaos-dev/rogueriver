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
      } else if (river->isBeach(x,y-1) || river->isBeach(x,y+1)) {
        tiles[x + y*width].color = beach_color*0.5 + bg_color*0.5;
      } else {
        tiles[x + y*width].color = bg_color;
      }
    }
  }
  
  PlaceMonsters();
  
  PlaceItems();
  
  PlaceRocks();
};

void Map::PlaceMonsters() {
  Position player = GetPlayerStart();
  
  std::uniform_real_distribution<> dist(0,1);
  int num_enemies = (int)(dist(engine.rng)*20);
  while (num_enemies > 0) {
    int x = (int)(dist(engine.rng)*width);
    int y = (int)(dist(engine.rng)*height);
    
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
}

void Map::PlaceItems() {
  Position player = GetPlayerStart();

  std::uniform_real_distribution<> dist(0,1);
  int num_armor; int num_weapons;
  if (engine.level == 4) {
    num_armor = 1; num_weapons = 1;
  } else if (engine.level == 5) {
    num_armor = 0; num_weapons = 0;
  } else {
    num_armor = (int)(dist(engine.rng)*2+1);
    num_weapons = (int)(dist(engine.rng)*2+1);
  }

  if (engine.level > 3) num_weapons = 1;
  while (num_weapons > 0) {
    int x = (int)(dist(engine.rng)*width);
    int y = (int)(dist(engine.rng)*height);
    if (!isBeach(x,y)) continue;
    
    if (x < player.x || x > (engine.map->width - engine.NEXT_LEVEL_POINT)) continue;
    if ((player.x-x)*(player.x-x)+(player.y-y)*(player.y-y) < 900) continue;
    
    if (CanWalk(x,y)) {
        AddWeapon(x,y);
        num_weapons--;
    };
  };
  
  while (num_armor > 0) {
    int x = (int)(dist(engine.rng)*width);
    int y = (int)(dist(engine.rng)*height);
    if (!isBeach(x,y)) continue;
    
    if (x < player.x || x > (engine.map->width - engine.NEXT_LEVEL_POINT)) continue;
    if ((player.x-x)*(player.x-x)+(player.y-y)*(player.y-y) < 900) continue;
    
    if (CanWalk(x,y)) {
        AddArmor(x,y);
        num_armor--;
    };
  };
};

void Map::PlaceRocks() {
  for (Rock rock : river->rocks) {
    if (rock.width == 1) {
      Actor* actor = new Actor(rock.x, rock.y, '*', Color(137,136,131),1);
       actor->words = new Words("rock","Rock"," "," "," "," ");
      engine.actors.push_back(actor);
    } else {
      Actor* actor;
      for (int i=0; i<rock.width; i++) {
       Actor* actor = new Actor(rock.x, rock.y+i, '*', Color(137,136,131),1);
       actor->words = new Words("rock","Rock"," "," "," "," ");
       engine.actors.push_back(actor);
      };
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
  std::uniform_int_distribution<> dist(0,100);
  int roll = dist(engine.rng);
  switch (engine.level) {
    case 1:
      if ( roll < 90 ) {
        Actor* ghost = CreateMonster(MonsterType::GHOST, x, y);
        engine.actors.push_back(ghost);
      } else {
        Actor* cyclops = CreateMonster(MonsterType::CYCLOPS, x, y);
        engine.actors.push_back(cyclops);
      }
      break;
    case 2:
      if ( roll < 70 ) {
        Actor* centaur = CreateMonster(MonsterType::CENTAUR, x, y);
        engine.actors.push_back(centaur);
      } else {
        Actor* skeleton = CreateMonster(MonsterType::SKELETON, x, y);
        engine.actors.push_back(skeleton);
      }
      break;
    case 3:
      if ( roll < 80 ) {
        Actor* ghoul = CreateMonster(MonsterType::GHOUL, x, y);
        engine.actors.push_back(ghoul);
      } else {
        Actor* harpy = CreateMonster(MonsterType::HARPY, x, y);
        engine.actors.push_back(harpy);
      }
      break;
    case 4:
      if ( roll < 70 ) {
        Actor* giant = CreateMonster(MonsterType::GIANT, x, y);
        engine.actors.push_back(giant);
      } else {
        Actor* manticore = CreateMonster(MonsterType::MANTICORE, x, y);
        engine.actors.push_back(manticore);
      }
      break;
    case 5:
      if ( roll < 70 ) {
        Actor* giant = CreateMonster(MonsterType::DRAGON, x, y);
        engine.actors.push_back(giant);
      } else {
        Actor* stymp = CreateMonster(MonsterType::STYMP, x, y);
        engine.actors.push_back(stymp);
      }
      break;
    default:
      break;
  };
};

Actor* Map::CreateMonster(Map::MonsterType monster_type, int x, int y) {
  std::uniform_int_distribution<> dist(0,100);
  int roll = dist(engine.rng);
  Actor* monster = nullptr;
  switch (monster_type) {
    case GHOST:
      monster = new Actor(x,y,'g',Color(198,198,198),1);
      switch (roll%4) {
        case 0:
          monster->words = new Words("the ghost","The ghost","dead ghost","his","javelin","shadowy form");
          monster->attacker = new Attacker(6,6,6,32);
          break;
        case 1:
          monster->words = new Words("the ghost","The ghost","dead ghost","his","sling","shadowy form");
          monster->attacker = new Attacker(6,6,3,12);
          break;
        case 2:
          monster->words = new Words("the ghost","The ghost","dead ghost","his","spear","shadowy form");
          monster->attacker = new Attacker(6,6,5,1);
          break;
        default:
          monster->words = new Words("the ghost","The ghost","dead ghost","his","sword","shadowy form");
          monster->attacker = new Attacker(6,6,5,1);
        break;
      }
      if (roll%2 == 0) monster->words->possessive = "her";
      monster->destructible = new GhostDestructible(1,0);
      monster->ai = new MonsterAi();
      monster->can_fly = true;
      return monster;
      
    case SKELETON:
      monster = new Actor(x,y,'s',Color(240,240,240),1);
      monster->words = new Words("the skeleton","The skeleton","pile of bones","his","sword","bones");
      if (roll%2 == 0) monster->words->possessive = "her";
      monster->destructible = new MonsterDestructible(12,0);
      monster->attacker = new Attacker(6,6,11,1);
      monster->ai = new MonsterAi();
      return monster;
      
    case GHOUL:
      monster = new Actor(x,y,'g',Color(240,240,240),1);
      monster->words = new Words("the ghoul","The ghoul","pile of bones","his","acidic blood","bones");
      if (roll%2 == 0) monster->words->possessive = "her";
      monster->destructible = new MonsterDestructible(19,0);
      monster->attacker = new Attacker(7,12,9,12);
      monster->ai = new MonsterAi();
      return monster;
    
    case CENTAUR:
      monster = new Actor(x,y,'c',Color(240,240,240),2);
      monster->words = new Words("the centaur","The centaur","dead centaur","his","arrow","skin");
      monster->destructible = new MonsterDestructible(16,0);
      monster->attacker = new Attacker(6,9,14,150);
      monster->ai = new MonsterAi();
      return monster;
      
    case HARPY:
      monster = new Actor(x,y,'h',Color(240,240,240),3);
      monster->words = new Words("the harpy","The harpy","dead harpy","her","claws","skin");
      monster->destructible = new MonsterDestructible(26,0);
      monster->can_fly = true;
      monster->attacker = new Attacker(14,15,12,1);
      monster->ai = new MonsterAi();
      return monster;
      
    case STYMP:
      monster = new Actor(x,y,'v',Color(240,240,240),4);
      monster->words = new Words("the stymphalian bird","The stymphalian bird","dead stymphalian bird","his","bronze beak","metal feathers");
      monster->destructible = new MonsterDestructible(21,6);
      monster->can_fly = true;
      monster->attacker = new Attacker(15,9,15,1);
      monster->ai = new MonsterAi();
      return monster;
      
    case GIANT:
      monster = new Actor(x,y,'G',Color(240,240,240),2);
      monster->words = new Words("the giant","The giant","dead giant","his","boulder","fur coat");
      monster->destructible = new MonsterDestructible(32,2);
      monster->attacker = new Attacker(10,3,25,12);
      monster->ai = new MonsterAi();
      return monster;
    
    case CYCLOPS:
      monster = new Actor(x,y,'C',Color(240,240,240),1);
      monster->words = new Words("the cyclops","The cyclops","dead cyclops","his","massive club","skin");
      monster->destructible = new MonsterDestructible(26,0);
      monster->attacker = new Attacker(7,3,20,1);
      monster->ai = new MonsterAi();
      return monster;
      
    case MANTICORE:
      monster = new Actor(x,y,'M',Color(240,240,240),3);
      monster->words = new Words("the manticore","The manticore","dead manticore","the","spines shot from his tail","thick hide");
      monster->destructible = new MonsterDestructible(22,3);
      monster->attacker = new Attacker(15,11,9,15);
      monster->can_fly = true;
      monster->ai = new MonsterAi();
      return monster;
    case DRAGON:
      monster = new Actor(x,y,'D',Color(240,240,240),1);
      monster->words = new Words("the dragon","The dragon","dead dragon","her","fiery breath","scales");
      monster->destructible = new MonsterDestructible(30,9);
      monster->attacker = new Attacker(30,21,18,40);
      monster->can_fly = true;
      monster->ai = new MonsterAi();
      return monster;
      
    case NESSUS:
      return nullptr;
    case CHIMERA:
      return nullptr;
    case CAUCUS:
      return nullptr;
    case FURY:
      return nullptr;
    case CHARYBDIS:
      return nullptr;
    case CERBERUS:
      return nullptr;
  }
  return monster;
};

void Map::AddArmor(int x, int y) {
  Actor* armor = nullptr;
  std::uniform_int_distribution<> dist(0,100);
  int roll = dist(engine.rng);
  switch (engine.level) {
    case 1:
      armor = CreateItem(ItemType::LEATHER,x,y);
      engine.actors.push_back(armor);
      break;
    case 2:
      armor = CreateItem(ItemType::BRONZE,x,y);
      engine.actors.push_back(armor);
      break;
    case 3:
      armor = CreateItem(ItemType::ADAMANT,x,y);
      engine.actors.push_back(armor);
      break;
    case 4:
      armor = CreateItem(ItemType::ACHILLES,x,y);
      engine.actors.push_back(armor);
      break;
    default:
      break;
    };
};

void Map::AddWeapon(int x, int y) {
  Actor* weapon;
  std::uniform_int_distribution<> dist(0,100);
  int roll = dist(engine.rng);
  switch (engine.level) {
    case 1:
      weapon = CreateItem(ItemType::SHORTBOW,x,y);
      engine.actors.push_back(weapon);
      break;
    case 2:
      weapon = CreateItem(ItemType::JAVELIN,x,y);
      engine.actors.push_back(weapon);
      break;
    case 3:
      weapon = CreateItem(ItemType::LONGBOW,x,y);
      engine.actors.push_back(weapon);
      break;
    case 4:
      weapon = CreateItem(ItemType::ARTEMIS,x,y);
      engine.actors.push_back(weapon);
      break;
    default:
      break;
    };
};

Actor* Map::CreateItem(ItemType item_type, int x, int y) {
  Actor* item = nullptr;
  switch (item_type) {
    case SHORTBOW:
      item = new Actor(x,y,')',Color(240,240,240),1);
      item->words = new Words("short bow","Short bow"," ", " ", "arrows"," ");
      item->item = new Item(5,150,0);
      return item;
      
    case JAVELIN:
      item = new Actor(x,y,'/',Color(240,240,240),1);
      item->words = new Words("set of javelins", "Set of javelins", " ", " ", "javelin"," ");
      item->item = new Item(7,35,0);
      return item;
    
    case LONGBOW:
      item = new Actor(x,y,'}',Color(240,240,240),1);
      item->words = new Words("longbow","Longbow"," ", " ", "arrows"," ");
      item->item = new Item(9,150,0);
      return item;
      
    case ARTEMIS:
      item = new Actor(x,y,'}',Color(240,0,0),1);
      item->words = new Words("Artemis's bow","Artemis's bow"," ", " ", "arrows"," ");
      item->item = new Item(20,200,0);
      return item;
      
    case LEATHER:
      item = new Actor(x,y,'a',Color(240,240,240),1);
      item->words = new Words("leather armor","Leather Armor"," ", " ", " "," ");
      item->item = new Item(0,0,3);
      return item;
    
    case BRONZE:
      item = new Actor(x,y,'a',Color(240,240,240),1);
      item->words = new Words("bronze breastplate and helmet","Bronze breatplate and helmet"," ", " ", " "," ");
      item->item = new Item(0,0,6);
      return item;
      
    case ADAMANT:
      item = new Actor(x,y,'a',Color(240,240,240),1);
      item->words = new Words("adamant breastplate and helmet","Adamant breatplate and helmet"," ", " ", " "," ");
      item->item = new Item(0,0,10);
      return item;
      
    case ACHILLES:
      item = new Actor(x,y,'a',Color(240,240,240),1);
      item->words = new Words("armor of Achilles","Armor of Achilles"," ", " ", " "," ");
      item->item = new Item(0,0,500);
      return item;
  }
  return nullptr;

};
