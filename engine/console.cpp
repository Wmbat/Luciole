/*
 *
 */

#include <iostream>

#include "console.h"

namespace TWE
{
    char console::buffer_[4096];
    std::uint32_t console::buffer_index_ = 0;

    void console::log( const std::string &str, const console::message_priority priority )
    {
        if( buffer_index_ + str.length() > 4096 )
        {
            flush();
        }

        if( priority == message_priority::status )
        {
            buffer_[buffer_index_] = '-';
            buffer_[buffer_index_ + 1] = '-';

            buffer_index_ += 2;

            for( const char c : str )
            {
                buffer_[buffer_index_] = c;
                ++buffer_index_;
            }
        }
        else if( priority == message_priority::warning )
        {
            for( const char c : str )
            {
                buffer_[buffer_index_] = c;
                ++buffer_index_;
            }
        }
        else
        {
            std::cerr << str << std::endl;
        }
    }

    void console::flush( )
    {
        buffer_[buffer_index_] = '\0';
        ++buffer_index_;

        std::cout << buffer_ << std::endl;

        for( auto i = 0; i <= buffer_index_; ++i )
        {
            buffer_[i] = '\0';
        }

        buffer_index_ = 0;
    }
}

