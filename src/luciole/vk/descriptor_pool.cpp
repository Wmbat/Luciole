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
   descriptor_pool::descriptor_pool( create_info_t const& create_info ) : p_context( create_info.value( ).p_context )
   {
      std::vector<VkDescriptorPoolSize> sizes;
      sizes.reserve( create_info.value( ).pool_sizes.size( ) );

      for ( auto const& size : create_info.value( ).pool_sizes )
      {
         auto pool_size = VkDescriptorPoolSize{};
         pool_size.type = static_cast<VkDescriptorType>( size.type );
         pool_size.descriptorCount = static_cast<std::uint32_t>( size.descriptor_count );
      }

      auto pool_create_info = VkDescriptorPoolCreateInfo{};
      pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      pool_create_info.pNext = nullptr;
      pool_create_info.flags = 0;
      pool_create_info.maxSets = create_info.value( ).max_num_sets;
      pool_create_info.poolSizeCount = static_cast<std::uint32_t>( sizes.size( ) );
      pool_create_info.pPoolSizes = sizes.data( );

      auto const res = p_context->create_descriptor_pool( vk::descriptor_pool_create_info_t( pool_create_info ) );

      if ( auto const* p_val = std::get_if<VkDescriptorPool>( &res ) )
      {
         pool = *p_val;
      }

      // TODO: Log the potential error.
   }
   descriptor_pool::descriptor_pool( descriptor_pool&& rhs ) { *this = std::move( rhs ); }

   descriptor_pool::~descriptor_pool( )
   {
      if ( pool != VK_NULL_HANDLE )
      {
         pool = p_context->destroy_descriptor_pool( vk::descriptor_pool_t( pool ) );
      }
   }

   descriptor_pool& descriptor_pool::operator=( descriptor_pool&& rhs )
   {
      if ( this != &rhs )
      {
         p_context = rhs.p_context;
         rhs.p_context = nullptr;

         pool = rhs.pool;
         rhs.pool = VK_NULL_HANDLE;
      }

      return *this;
   }
} // namespace vk
