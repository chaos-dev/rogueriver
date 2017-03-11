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

#include "Actor.h"

#include <cmath>

#include "Ai.h"

/** Initializes an actor object.
 *
 * @param x - The x position of the actor
 * @param y - The y position of the actor
 * @param symbol - An integer representing the ASCII number for the actor's symbol
 * @param name - A character array with the name of the actor
 */
Actor::Actor(int x, int y, int symbol, Color color, int speed) :
             x(x),y(y),symbol(symbol),ai(nullptr), item(nullptr),
             destructible(nullptr), attacker(nullptr), words(nullptr),
             blocks(true), color(color), speed(speed), can_fly(false) {
};

Actor::~Actor() {
  if ( ai ) delete ai;
  if ( destructible ) delete destructible;
  if ( attacker ) delete attacker;
  if ( words ) delete words;
};

void Actor::Update() {
  if ( ai ) ai->Update(this);
}

void Actor::ProcessInput(int key, bool shift) {
  if ( ai ) ai->ProcessInput(this, key, shift);
}

/** Finds the distance between the actor and a target cell
 *
 * @param[in] cx - The x coordinate of the target cell
 * @param[in] cy - The y coordinate of the target cell
 */
float Actor::GetDistance(int cx, int cy) const {
  int dx=x-cx;
  int dy=y-cy;
  return std::sqrt(dx*dx+dy*dy);
}
