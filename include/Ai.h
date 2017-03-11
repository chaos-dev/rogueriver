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
 
#ifndef INCLUDE_AI_H_
#define INCLUDE_AI_H_

class Actor;

/** This is an abstract object that allows players and monsters to take turns.
 *
 *  This base class is extended to create a player and a monster type of AI.
 */
class Ai {
public :
    ~Ai() {};
	virtual void Update(Actor *owner)=0;
	virtual void ProcessInput(Actor *owner, int key, bool shift)=0;
	virtual bool isActive(Actor *owner) = 0;
protected :
	enum AiType {
		MONSTER, PLAYER
	};
};

class MonsterAi : public Ai {
public :
	MonsterAi();
	void Update(Actor *owner);
	void ProcessInput(Actor *owner, int key, bool shift);
	bool isActive(Actor *owner);
protected :
  bool active; // Is the monster active?
  void moveOrAttack(Actor *owner, int targetx, int targety);
};

class PlayerAi : public Ai {
public :
	PlayerAi();
  void Update(Actor *owner);
  void ProcessInput(Actor *owner, int key, bool shift);
	bool isActive(Actor *owner);
protected :
	int dx, dy; // Amount requested to move this next turn
	bool move;  // Is the player moving?
	bool running; // Is the player running?
	bool fire; // Is the player going to fire a weapon?
	bool moveOrAttack(Actor *owner, int targetx, int targety);
};
#endif // INCLUDE_AI_H_
