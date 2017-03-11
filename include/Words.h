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

#include <string>
 
struct Words {
  const char* name;
  const char* Name;
  const char* corpse;
  const char* possessive;
  std::string weapon;
  std::string armor;
  Words(const char* name, const char* Name, const char* corpse, 
        const char* possessive, const char* weapon, const char* armor)
    : name(name), Name(Name), corpse(corpse), possessive(possessive), 
      weapon(weapon), armor(armor) {};
};
