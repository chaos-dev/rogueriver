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

#ifndef INCLUDE_DESTRUCTIBLE_H_
#define INCLUDE_DESTRUCTIBLE_H_

class Actor;

class Destructible {
public :
	int maxHp; // maximum health points
	int hp; // current health points
	int armor; // strength of their armor

	Destructible(int maxHp, int armor);
	inline bool isDead() { return hp <= 0; }
	int takeDamage(Actor *owner, int damage);
	int heal(float amount);
	virtual void die(Actor *owner);
protected :
	enum DestructibleType {
		MONSTER,PLAYER
	};
};

class MonsterDestructible : public Destructible {
public :
	MonsterDestructible(int maxHp, int armor);
	void die(Actor *owner);
};

class PlayerDestructible : public Destructible {
public :
	PlayerDestructible(int maxHp, int armor);
	void die(Actor *owner);
};
#endif // INCLUDE_DESTRUCTIBLE_H_
