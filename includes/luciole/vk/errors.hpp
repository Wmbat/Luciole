/*
 *  Copyright (C) 2018-2019 Wmbat
 *
 *  wmbat@protonmail.com
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

#ifndef LUCIOLE_VULKAN_ERRORS_HPP
#define LUCIOLE_VULKAN_ERRORS_HPP

#include <string>

namespace vk
{
    class error
    {
    public:
        enum class type
        {
            e_none,
            e_suboptimal,
            e_out_of_host_memory,
            e_out_of_device_memory,
            e_initialization_failed,
            e_incompatible_driver,
            e_no_physical_device_found,
            e_no_suitable_physical_devices,
            e_device_lost,
            e_surface_lost,
            e_out_of_date,
            e_native_window_in_use,
            e_too_many_objects,
            e_invalid_queue,
            e_command_pool_creation_failed,
            e_count
        };
    
    public:
        inline static std::string_view to_string( type t )
        {
            return string_type[static_cast<std::size_t>( t )];
        }

    private:
        static constexpr char const* string_type[] =
        {
            "out of host memory",
            "out of device memory",
            "initialization failed",
            "incompatible driver",
            "no physical device found",
            "no suitable physical devices",
            "device lost",
            "surface lost",
            "native window in use",
            "too many objects"
        };
    };
}

#endif // LUCIOLE_VULKAN_ERROR_HPP