/*!
 *  Copyright (C) 2018 Wmbat
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

#ifndef TWE_MESSAGE_H
#define TWE_MESSAGE_H

#include <vector>

#include "delegate.hpp"

namespace twe
{
    template<class C>
    class message_handler
    {
    public:
        void add_callback( const delegate<void( C )>& callback )
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
        std::vector<delegate<void( C )>> callbacks_;
    };
}

#endif //TWE_MESSAGE_H
