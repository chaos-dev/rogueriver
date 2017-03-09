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
 
#ifndef INCLUDE_COLOR_H_
#define INCLUDE_COLOR_H_

#include "BearLibTerminal.h"

struct Color {
    int r,g,b;
    
    Color() : r(0), g(0), b(0) {};
    Color(int r, int g, int b) : r(r), g(g), b(b) {};
    
    color_t Convert() const {
        return color_from_argb(255, r, g, b);
    };
    
    Color operator+(const Color& rhs) {
        Color color;
        color.r = this->r + rhs.r;
        color.g = this->g + rhs.g;
        color.b = this->b + rhs.b;
        return color;
    };
    
    Color operator-(const Color& rhs) {
        Color color;
        color.r = this->r - rhs.r;
        color.g = this->g - rhs.g;
        color.b = this->b - rhs.b;
        return color;
    };
    
    Color operator*(const float& scalar) {
        Color color;
        color.r = this->r * scalar;
        color.g = this->g * scalar;
        color.b = this->b * scalar;
        return color;
    };
};

#endif /* INCLUDE_COLOR_H_ */
