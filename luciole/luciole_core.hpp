/*
 *  Copyright (C) 2018-2019 Wmbat
 *
 *  wmbat@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  You should have received a copy of the GNU General Public License
 *  GNU General Public License for more details.
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef LUCIOLE_LUCIOLE_CORE_HPP
#define LUCIOLE_LUCIOLE_CORE_HPP

#include <cstdint>

static constexpr uint32_t kilobyte = 1024;
static constexpr uint32_t megabyte = kilobyte * kilobyte;
    
constexpr unsigned long long operator "" _kg( unsigned long long size ) { return size * kilobyte; }
constexpr unsigned long long operator "" _mb( unsigned long long size ) { return size * megabyte; }

#endif //LUCIOLE_LUCIOLE_CORE_HPP