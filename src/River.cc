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

#include <cmath>

#include "Engine.h"

River::River(int length) : length(length) {
  width.resize(length);
  shape.resize(length);
  angle.resize(length);
  mean_velocity.resize(length);

  // Create the river
  width = RandomSignal(length, min_width, max_width, 200, length*2.0, num_periods);
  shape = RandomSignal(length, min_travel, max_travel, 150, length*2.0, num_periods);
  
  // Create the angle of the river
  angle[0] = std::tan(shape[1] - shape[0]);
  for (int i=0; i<length-1; i++) {
    angle[i] = std::tan((shape[i+1] - shape[i-1])/2);
  };
  angle[length-1] = std::tan(shape[length-1] - shape[length-2]);
  
  // Create the speed of the river
  max_velocity = 0.0;
  float C = 2.00;    // constant on curve fit
  float p = -0.395;  // power on curve fit
  float Q = 40;      // m^3/s volumetric flowrate
  for (int i=0; i<length; i++) {
    mean_velocity[i] = C*std::pow(width[i]/Q, p);
    if (1.5*mean_velocity[i] > max_velocity)
        max_velocity = 1.5*mean_velocity[i];
  };
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
  periods[0] = std::exp(dist(engine.rng)*0.25*(max_log - min_log) + min_log);
  periods[1] = std::exp(dist(engine.rng)*(max_log - min_log) + min_log);
  periods[2] = std::exp((0.75 + dist(engine.rng)*0.25)*(max_log - min_log) + min_log);
  for (int i = 0; i<num_periods; i++)
    shifts[i] = dist(engine.rng)*2.0*pi;

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
  float rescaled = (y - shape[x])/(width[x]/2.0);
  float peak_velocity = 1.5*mean_velocity[x];
  if (rescaled < 1.0 && rescaled > -1.0) {
    return (1.0 - rescaled*rescaled)*peak_velocity;
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

int River::GetPlayerStart(int x) {
    return shape[x] + width[x]/2 + 2;
};


