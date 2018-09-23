#ifndef VULKAN_PROJECT_FILE_IO_H
#define VULKAN_PROJECT_FILE_IO_H

#include <stdexcept>
#include <fstream>

namespace utilities
{
    inline const std::string read_from_file( const std::string& filepath )
    {
        std::ifstream file( filepath );
        std::string str;

        if( !file.tellg() < 0 )
            throw std::runtime_error{ "Error loading file at location: " + filepath + "." };
        else if( !file.good() )
            throw std::runtime_error{ "Error reading file: " + filepath + "." };

        char c;
        while( file.get( c ) )
        {
            str.push_back( c );
        }

        return str;
    }

    inline const std::string read_from_binary_file( const std::string& filepath )
    {
        std::ifstream file( filepath , std::ios::binary );
        std::string str;

        if( !file.tellg() < 0 )
            throw std::runtime_error{ "Error loading file at location: " + filepath + "." };
        else if( !file.good() )
            throw std::runtime_error{ "Error reading file: " + filepath + "." };

        char c;
        while( file.get( c ) )
        {
            str.push_back( c );
        }

        return str;
    }

    inline const void write_to_file( const std::string& filepath, const std::string& data )
    {
        std::ofstream file( filepath );

        if( !file.good() )
            throw std::runtime_error{ "Error finding file: " + filepath + "." };

        file << data;
    }
}

#endif //VULKAN_PROJECT_FILE_IO_H
