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

#include <iostream>
#include <string>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#include "../engine/window.h"
#include "../engine/console.h"

#include "game.h"

int main( )
{
    try
    {
        engine::window wnd( "Test" );

        try
        {
            game game( wnd );

            try
            {
                game.run();
            }
            catch( const std::runtime_error& e )
            {
                std::cerr << "Error caught at application runtime:\n\t" << e.what() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "Major fuck up caught at application runtime!" << std::endl;
            }
        }
        catch( const std::runtime_error& e )
        {
            std::cerr << "Error caught at application creation:\n\t" << e.what() << std::endl;
        }
        catch( ... )
        {
            std::cerr << "Major fuck up caught at application creation!" << std::endl;
        }
    }
    catch( const std::runtime_error& e )
    {
        std::cerr << "Error cought at window creation:\n\t" << e.what() << std::endl;
    }
    catch( ... )
    {
        std::cerr << "Major fuck up caught at window creation!" << std::endl;
    }

    engine::console::flush();

    return 0;
}