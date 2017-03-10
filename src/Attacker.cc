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


#include <algorithm>    // std::min
#include <exception>
#include <random>

#include "Actor.h"
#include "Engine.h"

Attacker::Attacker() {
};

/** Allows initialization with combat attributes.
 */
Attacker::Attacker(int attack, int dodge, int mean_damage)
    : attack(attack), dodge(dodge), mean_damage(mean_damage) {
};

/** This is the core functionality behind all attacks (melee, ranged, etc.)
 *
 * @param owner - A pointer to the actor who is attacking.
 * @param target - A pointer to the actor being attacked.
 * @param temp_damager - A raw pointer pointing to the damage object used.
 *   This function does not take ownership of the damager object.
 * @param mod - An integer representing a bonus applied to both the attack and
 *   the damage.
 */
void Attacker::Attack(Actor *owner, Actor *target, int mod) {

    std::normal_distribution<float> dist(owner->attacker->attack-3,
                                         (owner->attacker->attack-3.0)/3);
	int attack_roll = std::max(0,(int)dist(engine.rng));
#ifndef NDEBUG
        engine.gui->log->Print("[color=grey]The attack roll was: %d / %d", attack_roll, owner->attacker->attack);
#endif
	int damage = 0;
	bool hits = false;
	bool penetrates = false;

    hits = owner->attacker->DoesItHit(attack_roll, mod, target);
	if (hits) {
		damage = owner->attacker->GetDamage(owner->attacker->mean_damage, 0, target);
		if (damage > 0) penetrates = true;
	    damage = std::min(target->destructible->hp,damage);
    }
    
	owner->attacker->Message(hits,penetrates,damage,owner,target);	//Display a message.
	//Taking damage must happen after the message is displayed. Otherwise the
	//  messages about leveling up, killing, etc. happen before the attack
	//  messages. This would also lead to the killing blow reading
	//  "[name] deals 4 damage to [corpse]"
	if (target->destructible)
	    damage = target->destructible->takeDamage(target, damage);
    
};

/** Checks to see if a particular attack successfully hits the target.
 *
 * Landing a hit and penetrating armor are two separate things.  This function
 * only addresses hitting the target.
 *
 * @param dice - An integer representing the combat "roll", plus modifiers
 * @param target - A pointer to the actor being attacked
 * @return True if the attack successfully hits the target
 */
bool Attacker::DoesItHit(int attack_roll, int mod, Actor *target) {
	if (target->attacker) {
	    std::normal_distribution<float> dist(target->attacker->dodge-3,
                                             (target->attacker->dodge-3.0)/3);
        int dodge_roll = std::max(0,(int)dist(engine.rng));
#ifndef NDEBUG
        engine.gui->log->Print("[color=grey]The dodge roll was: %d / %d", dodge_roll, target->attacker->dodge);
#endif
        if (attack_roll > 10 + dodge_roll + mod) {
            return true;
        } else {
            return false;
        };
	} else {
		return true;
	};
};

void Attacker::Message(bool hits, bool penetrates, int damage, Actor *owner, Actor *target) {
	if (target->destructible) {
		if ( penetrates ) {
			engine.gui->log->Print("%s deals %d damage to %s.", 
			                       owner->name, damage, target->name);
		} else if ( !hits ) {
			engine.gui->log->Print("%s misses %s",
			                       owner->name, target->name);
		} else if ( !penetrates ) {
			engine.gui->log->Print("%s's blow bounces off %s's armor.",
			                       owner->name, target->name);
		};
	} else {
		engine.gui->log->Print("%s attacks %s in vain.",
		                       owner->name,target->name);
	};
};

int Attacker::GetDamage(int mean_damage, int mod, Actor* target) {
    std::normal_distribution<float> dist(mean_damage, mean_damage/3);
    int damage = (int)std::max(0, (int)dist(engine.rng));
    #ifndef NDEBUG
        engine.gui->log->Print("[color=grey]The damage roll was: %d / %d", damage, mean_damage);
    #endif
    if (target->destructible)
        damage -= target->destructible->armor;
    return damage;
};
