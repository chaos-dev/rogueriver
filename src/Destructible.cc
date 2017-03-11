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

#include "Destructible.h"

#include "Ai.h"
#include "Actor.h"
#include "Gui.h"
#include "Engine.h"

Destructible::Destructible(int maxHp, int armor) :
	maxHp(maxHp),hp(maxHp),armor(armor) {
}

int Destructible::takeDamage(Actor *owner, int damage) {
	if ( damage > 0 ) {
		hp -= damage;
		if ( hp <= 0 ) {
			die(owner);
		}
	} else {
		damage=0;
	}
	return damage;
}

int Destructible::heal(float amount) {
	hp += amount;
	if ( hp > maxHp ) {
		amount -= hp-maxHp;
		hp=maxHp;
	}
	return amount;
}

void Destructible::die(Actor *owner) {
	// transform the actor into a corpse!
	owner->symbol='%';
	owner->color=Color(240,0,0);
	owner->words->name=owner->words->corpse;
	owner->blocks=false;

	// make sure corpses are drawn before living actors
	for (unsigned int i=0; i<engine.actors.size(); i++) {
	    if (engine.actors[i] == owner) engine.actors.erase(engine.actors.begin()+i);
	}
	engine.actors.push_front(owner);
}

MonsterDestructible::MonsterDestructible(int maxHp, int armor) :
	Destructible(maxHp,armor) {
}

void MonsterDestructible::die(Actor *owner) {
	// transform it into a nasty corpse! it doesn't block, can't be
	// attacked and doesn't move
	engine.gui->log->Print("%s dies!", owner->words->Name);
	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(int maxHp, int armor) :
	Destructible(maxHp,armor) {
}

void PlayerDestructible::die(Actor *owner) {
	engine.gui->log->Print("[color=red]You died!");
	Destructible::die(owner);
	// make sure your corpse is on top
	for (unsigned int i=0; i<engine.actors.size(); i++) {
	    if (engine.actors[i] == owner) engine.actors.erase(engine.actors.begin()+i);
	}
	engine.actors.push_back(owner);
	engine.game_status=Engine::DEFEAT;
}
