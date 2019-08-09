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

#ifndef LUCIOLE_UTILITIES_MESSAGE_HPP
#define LUCIOLE_UTILITIES_MESSAGE_HPP

#include <vector>

#include <wmbats_bazaar/delegate.hpp>

template<class C>
class message_handler
{
public:
    void add_callback( const bzr::delegate<void( C )>& callback )
    {
        callbacks_.push_back( callback );
    }
        
    void send_message( const C& message )
    {
        for( auto& delegate : callbacks_ )
        {
            delegate( message );
        }
    }
    
private:
    std::vector<bzr::delegate<void( C )>> callbacks_;
};

#endif //LUCIOLE_UTILITIES_MESSAGE_HPP
