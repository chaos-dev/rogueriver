/*
 * Common.hpp
 *
 *  Created on: Nov 27, 2013
 *      Author: cfyz
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_

#include "BearLibTerminal.h"
#include <stdint.h>
#include <sstream>

uint64_t GetTime();

template<typename T> std::string to_string(const T& value)
{
	std::ostringstream ss;
	ss << value;
	return ss.str();
}

#endif /* COMMON_HPP_ */
