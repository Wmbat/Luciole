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

#ifndef ENGINE_ATOMIC_QUEUE_HPP
#define ENGINE_ATOMIC_QUEUE_HPP

#include <atomic>
#include <type_traits>

template<typename T>
class atomic_queue
{
    static_assert( std::is_move_constructible<T>::value, "Template type must be a movable type" );
public:
    atomic_queue( ) = default;
    atomic_queue( const atomic_queue& rhs ) = delete;
    atomic_queue( atomic_queue&& rhs )
    {
    
    }
    
    atomic_queue& operator=( const atomic_queue& rhs ) = delete;
    atomic_queue& operator=( atomic_queue&& rhs )
    {
    
    }
    
    bool push( T&& data )
    {
        for( ;; )
        {
            size_t tail_pos = tail_.load( std::memory_order_acquire );
            auto& node = buffer_[tail_pos & buffer_mask_];
            
            const auto seq = node.sequence_.load( std::memory_order_acquire );
            const auto difference = ( intptr_t ) seq - ( intptr_t ) tail_pos;
            
            if ( difference == 0 )
            {
                if ( tail_.compare_exchange_weak( tail_pos, tail_pos + 1, std::memory_order_relaxed))
                {
                    node.data = std::forward<T>(data);
            
                    node.sequence.store( tail_pos + 1, std::memory_order_release);
            
                    return true;
                }
            }
            else if ( difference < 0)
            {
                return false;
            }
        }
    }
    
    
private:
    struct node
    {
        std::atomic<std::size_t> sequence_;
        T data_;
    };
    
private:
    alignas( cache_line ) std::vector<node> buffer_;
    alignas( cache_line ) std::size_t buffer_mask_;
    alignas( cache_line ) std::atomic<std::size_t> tail_;
    alignas( cache_line ) std::atomic<std::size_t> head_;
};

#endif //ENGINE_ATOMIC_QUEUE_HPP
