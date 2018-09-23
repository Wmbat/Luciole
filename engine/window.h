
#ifndef VULKAN_PROJECT_WINDOW_H
#define VULKAN_PROJECT_WINDOW_H

#if NDEBUG
static constexpr bool enable_validation_layers = false;
#else
static constexpr bool enable_validation_layers = true;
#endif

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

namespace engine
{
    class window
    {
    public:
        window( ) = default;
        window( uint32_t width, uint32_t height, const std::string& title );
        window( const window& other ) = delete;
        window( window&& other ) noexcept;
        ~window( );

        bool is_open( );

        void poll_events( );

        std::vector<const char*> get_required_extensions( ) const noexcept;
        std::pair<VkBool32, VkSurfaceKHR> create_surface( const VkInstance& instance ) const noexcept;

        const uint32_t get_width( ) const noexcept;
        const uint32_t get_height( ) const noexcept;

        window& operator=( const window& other ) = delete;
        window& operator=( window&& other ) noexcept;

    private:
        GLFWwindow* p_glfw_window_ = nullptr;

        std::string title_;
        uint32_t width_ = 0;
        uint32_t height_ = 0;
    };
}

#endif //VULKAN_PROJECT_WINDOW_H
