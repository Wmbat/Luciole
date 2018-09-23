#include "game.h"

game::game( engine::window& wnd )
    :
    wnd_( wnd ),
    renderer_( wnd_, "Test", VK_MAKE_VERSION( 0, 0, 1 ) )
{

}

void game::run( )
{
    while( wnd_.is_open() )
    {
        wnd_.poll_events();
    }
}
