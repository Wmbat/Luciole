/**
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

#ifndef LUCIOLE_VK_DESCRIPTOR_POOL_HPP
#define LUCIOLE_VK_DESCRIPTOR_POOL_HPP

#include <luciole/context.hpp> 
#include <luciole/vk/core.hpp>

namespace vk
{
   enum class descriptor_type
   {
      sampler                    = 0,
      combined_image_sampler     = 1,
      sampled_image              = 2,
      storage_image              = 3,
      uniform_texel_buffer       = 4,
      storage_texel_buffer       = 5,
      uniform_buffer             = 6,
      storage_buffer             = 7,
      uniform_buffer_dynamic     = 8,
      storage_buffer_dynamic     = 9,
      input_attachment           = 10,
   }; 
      
   class descriptor_pool
   {
   public:
      struct size
      {   
         descriptor_type type;
         std::uint32_t descriptor_count;
      }; // struct size
         
      struct create_info
      {
         context const* p_context; 
        
         std::vector<size> pool_sizes;

         std::uint32_t max_num_sets;   
      }; // struct create_info

      using create_info_t = strong_type<create_info const&>;

   public:
      descriptor_pool( ) = default;
      descriptor_pool( create_info_t const& create_info );
      descriptor_pool( descriptor_pool const& rhs ) = delete;
      descriptor_pool( descriptor_pool&& rhs );
      ~descriptor_pool( );
   
      descriptor_pool& operator=( descriptor_pool const& rhs ) = delete;
      descriptor_pool& operator=( descriptor_pool&& rhs );
   
   private:
      context const* p_context_ = nullptr;

      VkDescriptorPool pool_ = VK_NULL_HANDLE;
   }; // class descriptor_pool
} // namespace vk

#endif // LUCIOLE_VK_DESCRIPTOR_POOL_HPP
