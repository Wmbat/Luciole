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

#ifndef TWE_ENTRY_POINT_H
#define TWE_ENTRY_POINT_H

extern TWE::application* TWE::create_application( );

#if defined( TWE_PLATFORM_WINDOWS )

#else
int main( int args, char** argv )
{
    TWE::log::init( );
    
    auto app = TWE::create_application( );
    app->run();
    
    delete app;
}
#endif

#endif //TWE_ENTRY_POINT_H
