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

/* INCLUDES */
#include <luciole/luciole_core.hpp>
#include <luciole/utils/strong_types.hpp>
#include <luciole/vk/core.hpp>

#include <array>
#include <string>

namespace vk
{
   struct [[nodiscard]] error
   {
   public:
      /**
       * @brief Enum containing all error codes.
       */
      enum class type : std::size_t
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
      }; // enum class type

      using type_t = strong_type<type, error>;

   public:
      /**
       * @brief Default Constructor.
       */
      error( ) = default;
      /**
       * @brief Explicit constructor that takes
       * a VkResult and converts it into an error
       * type.
       *
       * @param result The VkResult to convert.
       */
      explicit error( result_t result ) noexcept;
      /**
       * @brief explicit constructor.
       */
      explicit error( type_t type ) noexcept;

      /**
       * @brief Converts the error type into a string.
       *
       * @return The string equivalent to the error type.
       */
      [[nodiscard]] std::string const& to_string( ) const PURE;

      /**
       * @brief Check if the error type is
       * type::e_none
       */
      [[nodiscard]] bool is_error( ) const noexcept PURE;

      type get_type( ) const noexcept PURE;

   private:
      type to_type( VkResult result )
      {
         switch ( result )
         {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
               return type::e_out_of_host_memory;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
               return type::e_out_of_device_memory;
            case VK_ERROR_INITIALIZATION_FAILED:
               return type::e_initialization_failed;
            case VK_ERROR_INCOMPATIBLE_DRIVER:
               return type::e_incompatible_driver;
            case VK_ERROR_DEVICE_LOST:
               return type::e_device_lost;
            case VK_ERROR_SURFACE_LOST_KHR:
               return type::e_surface_lost;
            default:
               return type::e_none;
         }
      }

   private:
      type err_code;

      inline static std::string const string_type[] = {"none", "suboptimal", "out of host memory", "out of device memory",
         "initialization failed", "incompatible driver", "no physical device found", "no suitable physical devices", "device lost",
         "surface lost", "native window in use", "too many objects", "invalid queue", "command pool creation failed"};
   }; // class error
} // namespace vk

#endif // LUCIOLE_VULKAN_ERROR_HPP
