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

#include <luciole/vk/descriptor_pool.hpp>

namespace vk
{
   descriptor_pool::descriptor_pool( create_info_t const& create_info )
      :
      p_context_( create_info.value_.p_context )
   {
      std::vector<VkDescriptorPoolSize> sizes;
      sizes.reserve( create_info.value_.pool_sizes.size() );

      for( auto const& size : create_info.value_.pool_sizes )
      {
         VkDescriptorPoolSize const pool_size
         {
            .type = static_cast<VkDescriptorType>( size.type ),
            .descriptorCount = static_cast<std::uint32_t>( size.descriptor_count )
         };
      } 

      VkDescriptorPoolCreateInfo const pool_create_info
      {
         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
         .pNext = nullptr,
         .flags = 0,
         .maxSets = create_info.value_.max_num_sets,
         .poolSizeCount = static_cast<std::uint32_t>( sizes.size( ) ),
         .pPoolSizes = sizes.data( )
      };

      auto const res = p_context_->create_descriptor_pool(
         vk::descriptor_pool_create_info_t( pool_create_info )
      );

      if ( auto const* p_val = std::get_if<VkDescriptorPool>( &res ) )
      {
         pool_ = *p_val;
      }

      // TODO: Log the potential error.
   }
   descriptor_pool::descriptor_pool( descriptor_pool&& rhs )
   {
      *this = std::move( rhs );
   }
   
   descriptor_pool::~descriptor_pool( )
   {
      if ( pool_ != VK_NULL_HANDLE )
      {
         pool_ = p_context_->destroy_descriptor_pool( vk::descriptor_pool_t( pool_ ) );
      } 
   }
   
   descriptor_pool& descriptor_pool::operator=( descriptor_pool&& rhs )
   {
      if ( this != &rhs )
      {
         p_context_ = rhs.p_context_;
         rhs.p_context_ = nullptr;

         pool_ = rhs.pool_;
         rhs.pool_ = VK_NULL_HANDLE;
      }
   }
} // namespace vk
