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

#ifndef VULKAN_PROJECT_FILE_IO_H
#define VULKAN_PROJECT_FILE_IO_H

#include <stdexcept>
#include <fstream>
#include <vector>

#include "basic_error.hpp"


// TODO: Update this whole file to be better and comment.
namespace twe
{
    inline const std::string read_from_file( const std::string& filepath )
    {
        std::ifstream file( filepath );
        std::string str;

        if( !file.is_open() )
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
        std::ifstream file( filepath, std::ios::binary );
        std::string str;

        if( !file.is_open() )
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
        std::ofstream file( filepath, std::ios::binary );

        if( !file.good() )
            throw std::runtime_error{ "Error finding file: " + filepath + "." };

        file << data;
    }
}

#endif //VULKAN_PROJECT_FILE_IO_H
