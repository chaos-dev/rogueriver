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
 
#ifndef INCLUDE_ATTACKER_H_
#define INCLUDE_ATTACKER_H_

class Actor;

class Attacker {
protected:
    int attack;
    int dodge;
    int mean_damage;
	bool DoesItHit(int dice, int mod, Actor *target);
	int GetDamage(int mean_damage, int mod, Actor* target);
	void Message(bool hits, bool penetrates, int damage, Actor *owner, Actor *target);

public :
	Attacker();
	Attacker(int attack, int dodge, int mean_damage);
	void Attack(Actor *owner, Actor *target, int mod);
};
#endif // INCLUDE_ATTACKER_H_
