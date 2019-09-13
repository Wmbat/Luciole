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

#ifndef LUCIOLE_THREAD_POOL_HPP
#define LUCIOLE_THREAD_POOL_HPP

/* INCLUDES */
#include "../strong_types.hpp"

#include <wmbats_bazaar/delegate.hpp>

#include <vector>
#include <queue>
#include <thread>

/**
 * @brief 
 * 
 */
class thread_pool
{
public:
    using task = bzr::delegate<void( )>;

public:
    thread_pool( );
    explicit thread_pool( count32_t thread_count );

private:
    std::vector<std::queue<task>> task_queues_;
    std::vector<std::thread> threads_; 
};

#endif // LUCIOLE_THREAD_POOL_HPP