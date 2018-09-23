#include <iostream>
#include <string>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "../engine/window.h"

#include "game.h"

int main( )
{
    try
    {
        engine::window wnd( 1080, 720, "Test" );

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

    return 0;
}