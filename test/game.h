
#ifndef VULKAN_PROJECT_GAME_H
#define VULKAN_PROJECT_GAME_H

#include "../engine/renderer.h"
#include "../engine/window.h"

class game
{
public:
    explicit game( engine::window& wnd );

    void run( );

private:
    engine::window& wnd_;
    engine::renderer renderer_;
};


#endif //VULKAN_PROJECT_GAME_H
