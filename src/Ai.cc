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

#include "Ai.h"

#include <cmath>
#include <iostream>
#include <map>
#include <string>

#include "Actor.h"
#include "Engine.h"

MonsterAi::MonsterAi() : active(false) {
}

/** Checks to see if a monster should be updated/considered this turn.
 *
 * If a monster is inactive, out of range, etc., we don't need to have them
 * move each turn.  This function checks to see if the monster is active.
 *
 * @param owner - The monster to be considered.
 * @return True indicates that the monster should be updated/considered.
 */
bool MonsterAi::isActive(Actor *owner) {
  if (owner->destructible && owner->destructible->isDead()) {
    return false;
  }
  if (active) {
    return true;
  } else if (std::sqrt(std::pow(float(engine.player->x-owner->x),2) +
                       std::pow(float(engine.player->y-owner->y),2)) < 100.0) {
    active = true;
    return true;
  } else {
    return false;
  }
};

void MonsterAi::ProcessInput(Actor *owner, int key, bool shift) {
  // Monsters don't process input.
};

/** Allows the AI to perform actions for a monster during its turn.
 *
 * @param owner - The monster to update.
 */
void MonsterAi::Update(Actor *owner) {
  if (isActive(owner)) {
    moveOrAttack(owner, engine.player->x,engine.player->y);
  }
}

/** This wraps together two possible actions: moving a monster and attacking.
 *
 * For simplicity, this function just directs the monster to a cell.  If
 * the player is at that cell, it will attack the player.  If not, it will try
 * to move to that cell.
 *
 * @param owner - The monster to move or attack with.
 * @param targetx - The target cell to move to.
 * @param targety
 */
void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety) {
  for (int i=0; i<owner->speed; i++) {
      int dx = targetx - owner->x;
      int dy = targety - owner->y;
      int stepdx = (dx > 0 ? 1:-1);
      int stepdy = (dy > 0 ? 1:-1);
      float distance=sqrtf(dx*dx+dy*dy);
      if ( distance >= 2 ) {
        //TODO: The movement here is really simplistic...
        dx = (int)(round(dx/distance));
        dy = (int)(round(dy/distance));
        if (engine.map->CanWalk(owner->x+dx,owner->y+dy) && 
            (!engine.map->isWater(owner->x+dx,owner->y+dy) || 
            owner->can_fly)) {
          owner->x += dx;
          owner->y += dy;
        } else if ( engine.map->CanWalk(owner->x+stepdx,owner->y) && 
                   (!engine.map->isWater(owner->x+stepdx,owner->y) || 
                   owner->can_fly)) {
          owner->x += stepdx;
        } else if (engine.map->CanWalk(owner->x,owner->y+stepdy) && 
                   (!engine.map->isWater(owner->x,owner->y+stepdy) || 
                   owner->can_fly)) {
          owner->y += stepdy;
        }
      } else if ( owner->attacker && i<(owner->speed)) {
        owner->attacker->Attack(owner,engine.player,0);
        break;
      }
   }
}


PlayerAi::PlayerAi() : dx(0), dy(0), move(false), running(false){
}

/** Checks to see if the player should be considered for effects, etc.
 *
 * @param owner - A pointer to the player character.
 * @return True if the player character should be considered.
 */
bool PlayerAi::isActive(Actor *owner) {
  return true;
};

/** This function allows the player to move each turn.
 *
 * Two main things happen: the character has the chance to level up and any
 * keystrokes are recorded then acted on.  The actual functions for actions
 * are contained elsewhere.
 *
 * @param owner - A pointer to the player character.
 */
void PlayerAi::Update(Actor *owner) {
  // Check to see if the player is dead:
  if ( owner->destructible && owner->destructible->isDead() ) {
        return;
  }
  
  if (engine.game_status == Engine::AIMING) {
    if (owner->attacker->UpdateFiring(owner)) {
        dx = 0; dy = 0;
        move = true;
    }
  };

  // Process any movement
  if (move) {
    if (moveOrAttack(owner, owner->x+dx,owner->y+dy))
      engine.game_status = Engine::NEW_TURN;
    engine.camera->x = owner->x; engine.camera->y = owner->y;
    dx = 0; dy = 0;
    move = false;
  }
}

void PlayerAi::ProcessInput(Actor *owner, int key, bool shift) {
  if (key == TK_UP || key == TK_KP_8 || (key == TK_K && !shift)) {
    dy = 1; move = true;
  } else if (key == TK_DOWN || key == TK_KP_2 || (key == TK_J && !shift)) {
    dy = -1; move = true;
  } else if (key == TK_LEFT || key == TK_KP_4 || (key == TK_H && !shift)) {
    dx = -1; move = true;
  } else if (key == TK_RIGHT || key == TK_KP_6 || (key == TK_L && !shift)) {
    dx = 1; move = true;
  } else if (key == TK_KP_1 || (key == TK_B && !shift)) {
    dx = -1; dy = -1; move = true;
  } else if (key == TK_KP_3 || (key == TK_N && !shift)){
    dx = 1; dy = -1; move = true;
  } else if (key == TK_KP_7 || (key == TK_Y && !shift)){
    dx = -1; dy = 1; move = true;
  } else if (key == TK_KP_9 || (key == TK_U && !shift)){
    dx = 1; dy = 1; move = true;
  } else if (key == TK_KP_5 || (key == TK_PERIOD && !shift)) {
    dx = 0; dy = 0; move = true;
  } else if (key == TK_F && !shift) {
    engine.game_status = Engine::AIMING;
    engine.gui->log->Print("Click any square to aim, or press spacebar to cancel.");
  } else if (key == TK_R && !shift) {
    if (running) {
      running = false;
    } else {
      running = true;
    }
  }
  if (running) {
    dx *= 2;
    dy *= 2;
  }
}

/** This function handles the movement and "bump to attack" actions.
 *
 * @param owner - The player character
 * @param targetx - The x coordinate where the character wants to move
 * @param targety - The y coordinate where the character wants to move
 * @return True if the player moved to that square, false under any other
 *          action, including trying to walk into walls.
 */
bool PlayerAi::moveOrAttack(Actor *owner, int targetx,int targety) {

  if ( engine.map->isWall(targetx,targety) ) return false;
  
  // look for living actors to attack
  bool attacking = false;
  for (Actor* actor : engine.actors) {
    if ( actor->destructible && !actor->destructible->isDead()
        && actor->x == targetx && actor->y == targety 
        && actor != engine.player) {
      owner->attacker->Attack(owner, actor, 0);
      attacking = true;
      targetx = owner->x;
      targety = owner->y;
      break;
    }
  }

  bool on_raft = ((engine.player->x == engine.raft->x) && 
                  (engine.player->y == engine.raft->y));
  bool target_is_raft = ((targetx == engine.raft->x) && 
                         (targety == engine.raft->y));
  if (engine.map->isWater(targetx, targety) && !on_raft && !attacking && !target_is_raft) {
    engine.gui->log->Print("[color=flame]You need to board a raft to sail down the river.");
    return false;
  }

  
  // I cheat a little here to give the player a favorable rounding
  int temp_x = owner->x; int temp_y = owner->y;
  if (owner->x == targetx) {
    owner->x=targetx + std::round(engine.map->GetUVelocity(owner->x, owner->y));
  } else {
    owner->x=targetx + std::trunc(engine.map->GetUVelocity(owner->x, owner->y));
  }
  if (owner->y == targety) {
    owner->y=targety + std::round(engine.map->GetVVelocity(owner->x, owner->y));
  } else {
    owner->y=targety + std::trunc(engine.map->GetVVelocity(owner->x, owner->y));
  }
  
  bool moved = (owner->x != temp_x || owner->y != temp_y);
  if ((targetx != owner->x || targety != owner->y) && !moved && !attacking)
      engine.gui->log->Print("You fight the current, but make no progress.");
  
  // look for corpses or items
  if (moved) {
      for (Actor* actor : engine.actors) {
        //bool corpseOrItem=(actor->destructible && actor->destructible->isDead())
	    //		    || actor->pickable;
	    bool corpse_or_item = (actor->destructible && actor->destructible->isDead());
        if ( corpse_or_item && actor->x == owner->x && actor->y == owner->y ) {
          engine.gui->log->Print("There's a %s here.",actor->words->name);
        }
      }
  }
  
  if (on_raft && engine.map->isWater(targetx, targety)) {
    engine.raft->x = owner->x; engine.raft->y = owner->y;
  }
  
  return true;
  
}
