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

#ifndef INCLUDE_ACTOR_H_
#define INCLUDE_ACTOR_H_

class Actor;
#include "Ai.h"
#include "Color.h"
#include "Destructible.h"
#include "Attacker.h"

class Actor {
 private:
  
 public:
  int x, y;
  int symbol;
  const char* name;
  bool blocks;
  Color color;
  Ai* ai;
  Destructible* destructible;
  Attacker* attacker;
  
  Actor(int x, int y, int symbol, const char* name);
  ~Actor();
  void Update();
  void ProcessInput(int key, bool shift);
  float GetDistance(int cx, int cy) const;
};

#endif /* INCLUDE_ACTOR_H_ */
