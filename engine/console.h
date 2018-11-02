/*
 *
 */

#ifndef VULKAN_PROJECT_CONSOLE_H
#define VULKAN_PROJECT_CONSOLE_H

#include <string>

namespace engine
{
    class console
    {
    public:
        enum class message_priority
        {
            error,
            warning,
            status
        };

    public:
        static void log( const std::string& str, const message_priority priority = message_priority::status );

        static void flush( );

    private:
        static char buffer_[4096];
        static std::uint32_t buffer_index_;
    };
}

#endif //VULKAN_PROJECT_CONSOLE_H
