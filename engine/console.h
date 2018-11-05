/*
 *
 */

#ifndef VULKAN_PROJECT_CONSOLE_H
#define VULKAN_PROJECT_CONSOLE_H

#include <string>
#include <iostream>

namespace TWE
{
    class console
    {
    public:
        enum class message_priority
        {
            error,
            warning,
            status,
            normal
        };

    public:
        template<typename T>
        static T input( )
        {
            T input;

            std::cin >> input;

            return input;
        }

        static void log( const std::string& str, const message_priority priority = message_priority::status );
        static void new_line( );

        static void flush( );

    private:
        static char buffer_[4096];
        static std::uint32_t buffer_index_;
    };
}

#endif //VULKAN_PROJECT_CONSOLE_H
