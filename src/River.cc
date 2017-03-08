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

#include "River.h"

River::River(int length) : length(length) {
  width.resize(length);
  shape.resize(length);
  rng.seed(std::random_device()());

  // Create the river
  width = RandomSignal(length, min_width, max_width, length/2.0, length*2.0, num_periods);
  shape = RandomSignal(length, min_travel, max_travel, length/3.0, length*2.0, num_periods);
};

std::vector<float> River::RandomSignal(int n, float y_min, float y_max,
                                       float min_period, float max_period,
                                       int num_periods) {
  const float pi = std::atan(1)*4;
  float min_log = std::log(min_period);
  float max_log = std::log(max_period);
  std::vector<float> periods(num_periods);
  std::vector<float> shifts(num_periods);

  std::uniform_real_distribution<> dist(0,1);
  periods[0] = std::exp(dist(rng)*0.25*(max_log - min_log) + min_log);
  periods[1] = std::exp(dist(rng)*(max_log - min_log) + min_log);
  periods[2] = std::exp((0.75 + dist(rng)*0.25)*(max_log - min_log) + min_log);
  for (int i = 0; i<num_periods; i++)
    shifts[i] = dist(rng)*2.0*pi;

  std::vector<float> signal(n);
  for (int i = 0; i<n; i++) {
    signal[i] = 0.5*(y_max + y_min);
    for (int j=0; j<num_periods; j++) {
      signal[i] += 0.5*(y_max-y_min)/num_periods * std::sin(2.0*pi*i/periods[j] + shifts[j]);
    }
  }

  return signal;
}

float River::GetVelocity(int x, int y) {
  int i = (int)x;
  float rescaled = (y - shape[i])/(width[i]/2.0);
  if (rescaled < 1.0 && rescaled > -1.0) {
    return 1.0 - rescaled*rescaled;
  } else {
    return 0.0;
  }
};

bool River::isBeach(int x, int y) {
  int i = (int)x;
  float rescaled = std::abs(y - shape[i])/(width[i]/2.0);
  if (rescaled > 1.0 && rescaled < 1.2) {
    return true;
  } else {
    return false;
  }
};


