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

#ifndef LUCIOLE_MEMORY_POOL_HPP
#define LUCIOLE_MEMORY_POOL_HPP

#include <cstdint>
#include <list>

/*

constexpr std::uint32_t cache_line = 64;

template<std::size_t block_count = 64>
class memory_pool
{
private:
    struct block
    {
        alignas( cache_line ) char memory[cache_line];
    };

public:
    template<typename type>
    type* allocate( std::size_t count )
    {
        return new ( data_->memory ) type[count];
    }

    template<typename type>
    void deallocate( type* p_type )
    {
        
    }

public:
    block* data_ = new block( );

    std::vector<std::size_t> offsets_;
};
 */

#endif //LUCIOLE_MEMORY_POOL_HPP