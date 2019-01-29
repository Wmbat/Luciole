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

#ifndef MARSUPIAL_ENTRY_POINT_HPP
#define MARSUPIAL_ENTRY_POINT_HPP

#include <marsupial/application.hpp>

extern std::unique_ptr<marsupial::application> marsupial::create_application( );

#if defined( MARSUPIAL_PLATFORM_WINDOWS )
int main( int args, char** argv )
{
    marsupial::log::init ( );

    auto app = marsupial::create_application ( );
    app->run ( );
    
    return 0;
}
#else
int main( int args, char** argv )
{
    marsupial::log::init( );
    
    auto app = marsupial::create_application( );
    app->run();
    
    return 0;
}
#endif

#endif //MARSUPIAL_ENTRY_POINT_HPP
