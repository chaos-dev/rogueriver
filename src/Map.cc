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
        tiles[x + y*width].color = water_color*(vel/(river->mean_velocity[x]*1.5)) + 
                                   beach_color*(1.0-(vel/(river->mean_velocity[x]*1.5)));
      } else if (river->isBeach(x, y)) {
        tiles[x + y*width].color = beach_color;
      } else if (river->isBeach(x,y-1) || river->isBeach(x,y+1)) {
        tiles[x + y*width].color = beach_color*0.5 + bg_color*0.5;
      } else {
        tiles[x + y*width].color = bg_color;
      }
    }
  }
  
  // Rocks have to go first, so they're on the bottom.
  PlaceRocks();
  
  // Push the raft above the rocks
  for (int i=0; i<engine.actors.size(); i++) {
    if (engine.actors[i] == engine.raft) {
      engine.actors.erase(engine.actors.begin()+i);
      engine.actors.push_back(engine.raft);
      break;
    };
  }
  
  PlaceMonsters();
  
  PlaceItems();

  // Push the player back to the top.
  for (int i=0; i<engine.actors.size(); i++) {
      if (engine.actors[i] == engine.player) {
      engine.actors.erase(engine.actors.begin()+i);
      engine.actors.push_back(engine.player);
      break;
    };
  }
};

void Map::PlaceMonsters() {
  Position player = GetPlayerStart();
  
  std::uniform_real_distribution<> dist(0,1);
  int num_enemies = 75-engine.level*5;
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
  
  if (engine.level == 5) {
    int x = 5;
    int y = (int)(dist(engine.rng)*100+200);
    
    Actor* Thanatos = CreateMonster(MonsterType::THANATOS, x, y);
    engine.actors.push_back(Thanatos);
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
    num_armor = (int)(dist(engine.rng)*3+1);
    num_weapons = (int)(dist(engine.rng)*3+1);
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
        if (engine.level == 4) {
          Actor* chimera = CreateMonster(MonsterType::CHIMERA, x, y);
          engine.actors.push_back(chimera);
        };
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
        if (engine.level == 4) {
          Actor* cerberus = CreateMonster(MonsterType::CERBERUS, x, y);
          engine.actors.push_back(cerberus);
        };
        num_armor--;
    };
  };
};

void Map::PlaceRocks() {
  for (Rock rock : river->rocks) {
    //if (rock.x == engine.raft->x && rock.y == engine.raft->y) continue;
    if (rock.width == 1) {
      Actor* actor = new Actor(rock.x, rock.y, '*', rock_color,1);
       actor->words = new Words("rock","Rock"," "," "," "," ");
       actor->blocks = false;
      engine.actors.push_back(actor);
    } else {
      Actor* actor;
      for (int i=0; i<rock.width; i++) {
       Actor* actor = new Actor(rock.x, rock.y+i, '*', rock_color,1);
       actor->words = new Words("rock","Rock"," "," "," "," ");
       actor->blocks = false;
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
      rock_color.Update(91,96,87);
      break;
    case 2:
      water_color.Update(23,61,64);
      beach_color.Update(127,128,132);
      bg_color.Update(83,108,76);
      rock_color.Update(109,89,89);
      break;
    case 3:
      water_color.Update(23,61,64);
      beach_color.Update(117,122,100);
      bg_color.Update(71,51,40);
      rock_color.Update(75,66,55);
      break;
    case 4:
      water_color.Update(23,61,64);
      beach_color.Update(107,83,49);
      bg_color.Update(50,36,23);
      rock_color.Update(50,36,23);
      break;
    case 5:
      water_color.Update(92,10,12);
      bg_color.Update(24,12,14);
      beach_color.Update(59,64,60);
      rock_color.Update(24,12,14);
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

bool Map::isRock(int x, int y) const {
  for (Rock rock : river->rocks) {
    if (rock.x == x && rock.y == y) return true;
    if (rock.x == x && rock.y+1 == y && rock.width == 2) return true;
  };
  return false;
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
      if ( roll < 50 ) {
        Actor* centaur = CreateMonster(MonsterType::CENTAUR, x, y);
        engine.actors.push_back(centaur);
      } else {
        Actor* skeleton = CreateMonster(MonsterType::SKELETON, x, y);
        engine.actors.push_back(skeleton);
      }
      break;
    case 3:
      if ( roll < 70 ) {
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
      if ( roll < 30 ) {
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
      monster = new Actor(x,y,'g',Color(241,224,197),1);
      switch (roll%4) {
        case 0:
          monster->words = new Words("the ghost","The ghost","dead ghost","his","javelin","shadowy form");
          monster->attacker = new Attacker(9,6,6,32); 
          break;
        case 1:
          monster->words = new Words("the ghost","The ghost","dead ghost","his","sling","shadowy form");
          monster->attacker = new Attacker(9,6,3,12);
          break;
        case 2:
          monster->words = new Words("the ghost","The ghost","dead ghost","his","spear","shadowy form");
          monster->attacker = new Attacker(9,6,5,1);
          break;
        default:
          monster->words = new Words("the ghost","The ghost","dead ghost","his","sword","shadowy form");
          monster->attacker = new Attacker(9,6,5,1);
        break;
      }
      if (roll%2 == 0) monster->words->possessive = "her";
      monster->destructible = new GhostDestructible(1,0);
      monster->ai = new MonsterAi();
      monster->can_fly = true;
      return monster;
      
    case SKELETON:
      monster = new Actor(x,y,'s',Color(241,224,197),1);
      monster->words = new Words("the skeleton","The skeleton","pile of bones","his","sword","bones");
      if (roll%2 == 0) monster->words->possessive = "her";
      monster->destructible = new MonsterDestructible(12,0);
      monster->attacker = new Attacker(15,15,11,1);
      monster->ai = new MonsterAi();
      return monster;
      
    case GHOUL:
      monster = new Actor(x,y,'g',Color(161,195,73),1);
      monster->words = new Words("the ghoul","The ghoul","pile of bones","his","acidic vomit","flesh");
      if (roll%2 == 0) monster->words->possessive = "her";
      monster->destructible = new MonsterDestructible(19,0);
      monster->attacker = new Attacker(20,12,6,12);
      monster->ai = new MonsterAi();
      return monster;
    
    case CENTAUR:
      monster = new Actor(x,y,'c',Color(213,160,33),2);
      monster->words = new Words("the centaur","The centaur","dead centaur","his","arrow","skin");
      monster->destructible = new MonsterDestructible(16,0);
      monster->attacker = new Attacker(15,9,5,40);
      monster->ai = new MonsterAi();
      return monster;
       
    case HARPY:
      monster = new Actor(x,y,'h',Color(213,160,33),2);
      monster->words = new Words("the harpy","The harpy","dead harpy","her","claws","thick skin");
      monster->destructible = new MonsterDestructible(21,0);
      monster->can_fly = true;
      monster->attacker = new Attacker(14,9,12,0);
      monster->ai = new MonsterAi();
      return monster;
      
    case STYMP:
      monster = new Actor(x,y,'v',Color(213,137,54),4);
      monster->words = new Words("the stymphalian bird","The stymphalian bird","dead stymphalian bird","his","bronze beak","metal feathers");
      monster->destructible = new MonsterDestructible(26,6);
      monster->can_fly = true;
      monster->attacker = new Attacker(15,9,15,1);
      monster->ai = new MonsterAi();
      return monster;
      
    case GIANT:
      monster = new Actor(x,y,'G',Color(130,115,92),2); 
      monster->words = new Words("the giant","The giant","dead giant","his","boulder","fur coat");
      monster->destructible = new MonsterDestructible(32,2);
      monster->attacker = new Attacker(15,3,25,12);
      monster->ai = new MonsterAi();
      return monster;
    
    case CYCLOPS:
      monster = new Actor(x,y,'O',Color(86,54,53),1);
      monster->words = new Words("the cyclops","The cyclops","dead cyclops","his","massive club","skin");
      monster->destructible = new MonsterDestructible(26,0);
      monster->attacker = new Attacker(7,3,20,1);
      monster->ai = new MonsterAi();
      return monster;
      
    case MANTICORE:
      monster = new Actor(x,y,'M',Color(0,0,0),3);
      monster->words = new Words("the manticore","The manticore","dead manticore","the","spines shot from his tail","thick hide");
      monster->destructible = new MonsterDestructible(22,3);
      monster->attacker = new Attacker(15,11,9,15);
      monster->can_fly = true;
      monster->ai = new MonsterAi();
      return monster;
    case DRAGON:
      monster = new Actor(x,y,'D',Color(164,66,0),1);
      monster->words = new Words("the dragon","The dragon","dead dragon","her","fiery breath","scales");
      monster->destructible = new MonsterDestructible(30,12);
      monster->attacker = new Attacker(20,6,18,40); 
      monster->can_fly = true;
      monster->ai = new MonsterAi();
      return monster;
    case CERBERUS:
      monster = new Actor(x,y,'3',Color(255,255,255),2);
      monster->words = new Words("Cerberus","Cerberus","Cerberus's corpse","his","teeth","thick hide");
      monster->destructible = new MonsterDestructible(32,6);
      monster->attacker = new Attacker(15,11,9,1);
      monster->ai = new MonsterAi();
      return monster;
    case CHIMERA:
      monster = new Actor(x,y,'C',Color(255,255,255),2);
      monster->words = new Words("the chimera","The chimera","the chimera's corpse","his","fiery breath","thick hide");
      monster->destructible = new MonsterDestructible(32,6);
      monster->attacker = new Attacker(17,13,12,40);
      monster->ai = new MonsterAi();
      return monster;
    case THANATOS:
      monster = new Actor(x,y,'T',Color(255,255,255),2);
      monster->words = new Words("Thanatos","Thanatos","the corpse of Thanatos","his","sword of death","skin");
      monster->destructible = new MonsterDestructible(100,100);
      monster->attacker = new Attacker(20,20,100,1); 
      monster->can_fly = true;
      monster->ai = new MonsterAi();
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
      item = new Actor(x,y,')',Color(141,59,114),1);
      item->blocks = false;
      item->words = new Words("short bow","Short bow"," ", " ", "arrow"," ");
      item->item = new Item(8,150,0);
      return item;
      
    case JAVELIN:
      item = new Actor(x,y,'/',Color(157,203,186),1);
      item->blocks = false;
      item->words = new Words("set of javelins", "Set of javelins", " ", " ", "javelin"," ");
      item->item = new Item(10,35,0);
      return item;
    
    case LONGBOW:
      item = new Actor(x,y,'}',Color(242,163,89),1);
      item->blocks = false;
      item->words = new Words("longbow","Longbow"," ", " ", "arrows"," ");
      item->item = new Item(12,150,0);
      return item;
      
    case ARTEMIS:
      item = new Actor(x,y,'}',Color(83,216,251),1);
      item->blocks = false;
      item->words = new Words("Artemis's bow","Artemis's bow"," ", " ", "arrow"," ");
      item->item = new Item(25,200,0);
      return item;
      
    case LEATHER:
      item = new Actor(x,y,'a',Color(220,191,133),1);
      item->blocks = false;
      item->words = new Words("leather armor","Leather Armor"," ", " ", " "," ");
      item->item = new Item(0,0,3);
      return item;
    
    case BRONZE:
      item = new Actor(x,y,'a',Color(225,176,126),1);
      item->blocks = false;
      item->words = new Words("bronze breastplate and helmet","Bronze breatplate and helmet"," ", " ", " "," ");
      item->item = new Item(0,0,6);
      return item;
      
    case ADAMANT:
      item = new Actor(x,y,'a',Color(61,163,93),1); 
      item->blocks = false;
      item->words = new Words("adamant breastplate and helmet","Adamant breatplate and helmet"," ", " ", " "," ");
      item->item = new Item(0,0,10);
      return item;
      
    case ACHILLES:
      item = new Actor(x,y,'a',Color(102,195,255),1);
      item->blocks = false;
      item->words = new Words("armor of Achilles","Armor of Achilles"," ", " ", " "," ");
      item->item = new Item(0,0,100);
      return item;
  }
  return nullptr;

};
