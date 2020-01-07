/**
 *  Copyright (C) 2020 Wmbat
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

#ifndef LUCIOLE_VK_DEVICE_PHYSICAL_DEVICE_SELECTION_INTERFACE_HPP
#define LUCIOLE_VK_DEVICE_PHYSICAL_DEVICE_SELECTION_INTERFACE_HPP

#include <luciole/vk/core.hpp>

#include <vector>

namespace core
{
   class physical_device_selection_interface
   {
   protected:
      physical_device_selection_interface( ) = default;
      virtual ~physical_device_selection_interface( ) = default;

   public:
      virtual VkPhysicalDevice select_physical_device( std::vector<VkPhysicalDevice> const& physical_devices ) const = 0;
   };
}

#endif // LUCIOLE_VK_DEVICE_PHYSICAL_DEVICE_SELECTION_INTERFACE_HPP
