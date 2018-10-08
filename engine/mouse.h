
#ifndef VULKAN_PROJECT_MOUSE_H
#define VULKAN_PROJECT_MOUSE_H

#include <bitset>
#include <cstdint>
#include <cstddef>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace engine
{
    class mouse
    {
    public:
        enum class type : std::uint32_t
        {
            pressed,
            released,

            invalid
        };

        struct cursor_event
        {
            double x_pos_ = 0.0;
            double y_pos_ = 0.0;
        };

        struct button_event
        {
            std::int32_t button_id_ = 0;
            type type_ = type::invalid;
        };

    public:
        mouse( );

        bool is_button_pressed( int button_id ) const noexcept;

        glm::dvec2 cursor_pos( ) noexcept;

        void push_button_event( const button_event& event );
        void push_cursor_event( const cursor_event& event );

    private:
        void pop_button_event( );
        void pop_cursor_event( );

    private:
        static constexpr std::uint8_t MAX_BUTTON_BUFFER_SIZE_ = 8;
        static constexpr std::uint8_t MAX_CURSOR_BUFFER_SIZE_ = 8;

        std::bitset<GLFW_MOUSE_BUTTON_LAST> button_state_;

        button_event button_buffer_[MAX_BUTTON_BUFFER_SIZE_];
        size_t num_button_buffer_;

        cursor_event cursor_buffer_[MAX_CURSOR_BUFFER_SIZE_];
        size_t num_cursor_buffer_;

        size_t button_head_;
        size_t button_tail_;

        size_t cursor_head_;
        size_t cursor_tail_;
    };
}

#endif //VULKAN_PROJECT_MOUSE_H
