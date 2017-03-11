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

#ifndef INCLUDE_RIVER_H_
#define INCLUDE_RIVER_H_

#include <vector>
#include <cmath>
#include <random>

class River {
 protected:
  int length;
  float min_width = 15;
  float max_width = 40;
  float min_travel = 30;
  float max_travel = 90;
  int num_periods = 3;
  std::vector<float> width;
  std::vector<float> shape;
  std::vector<float> mean_velocity;
  
  std::vector<float> RandomSignal(int n, float y_min, float y_max, float min_period, float max_period, int num_periods);
 public:
  River(int length);
  std::vector<float> angle;
  float GetVelocity(int x, int y);
  bool isBeach(int x, int y);
  int GetPlayerStart(int x);
  float max_velocity;
};

#endif /* INCLUDE_RIVER_H_ */
