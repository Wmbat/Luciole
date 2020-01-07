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

#include <luciole/core/context.hpp>
#include <luciole/core/physical_device_selection_interface.hpp>
#include <luciole/ui/window.hpp>
#include <luciole/utils/logger.hpp>

#include <map>

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback( VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
   VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data )
{
   if ( message_type != 0 )
   {
   }

   if ( p_user_data != nullptr )
   {
   }

   if ( message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT )
   {
      spdlog::info( "{0}", p_callback_data->pMessage );
   }
   else if ( message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
   {
      spdlog::warn( "{0}", p_callback_data->pMessage );
   }
   else if ( message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT )
   {
      spdlog::error( "{0}", p_callback_data->pMessage );
   }

   return VK_FALSE;
}

namespace core
{
   context::context( ) :
      p_logger( nullptr ), instance( VK_NULL_HANDLE ), debug_messenger( VK_NULL_HANDLE ), physical_device( VK_NULL_HANDLE ),
      logical_device( VK_NULL_HANDLE )
   {
      init_volk( p_logger );
   }

   context::context( ui::window const& window, logger* p_logger ) :
      p_logger( p_logger ), instance( VK_NULL_HANDLE ), debug_messenger( VK_NULL_HANDLE ), physical_device( VK_NULL_HANDLE ),
      logical_device( VK_NULL_HANDLE )
   {
      init_volk( p_logger );

      std::uint32_t api_version = 0;
      vkEnumerateInstanceVersion( &api_version );

      if ( p_logger )
      {
         p_logger->info( "[{0}] Vulkan API version: {1}.{2}.{3}", __FUNCTION__, VK_VERSION_MAJOR( api_version ),
            VK_VERSION_MINOR( api_version ), VK_VERSION_PATCH( api_version ) );
      }

      auto app_info = VkApplicationInfo{};
      app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      app_info.pNext = nullptr;
      app_info.apiVersion = api_version;
      app_info.applicationVersion = VK_MAKE_VERSION( 0, 0, 0 );
      app_info.pApplicationName = nullptr;
      app_info.engineVersion = VK_MAKE_VERSION( 0, 0, 0 );
      app_info.pEngineName = "Luciole";

      auto instance_create_info = VkInstanceCreateInfo{};
      instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      instance_create_info.pNext = nullptr;
      instance_create_info.flags = 0;
      instance_create_info.pApplicationInfo = &app_info;

      // Get the validation layers
      if constexpr ( vk::enable_debug_layers )
      {
         std::uint32_t layer_count = 0;
         vkEnumerateInstanceLayerProperties( &layer_count, nullptr );
         std::vector<VkLayerProperties> layer_properties( layer_count );
         vkEnumerateInstanceLayerProperties( &layer_count, layer_properties.data( ) );

         std::array<char const*, 1> validation_layers = {"VK_LAYER_KHRONOS_validation"};

         bool has_khronos_validation = false;
         for ( auto const& property : layer_properties )
         {
            if ( strcmp( validation_layers[0], property.layerName ) == 0 )
            {
               has_khronos_validation = true;
            }
         }

         if ( has_khronos_validation )
         {
            instance_create_info.enabledLayerCount = layer_count;
            instance_create_info.ppEnabledLayerNames = validation_layers.data( );
         }
         else
         {
            if ( p_logger )
            {
               p_logger->warn(
                  "[{0}] \"{1}\" not found. No validation layer support will be provided.", __FUNCTION__, validation_layers[0] );
            }

            instance_create_info.enabledLayerCount = 0;
            instance_create_info.ppEnabledLayerNames = nullptr;
         }
      }
      else
      {
         instance_create_info.enabledLayerCount = 0;
         instance_create_info.ppEnabledLayerNames = nullptr;
      }

      std::vector<char const*> instance_extensions;

      // Get the instance extensions
      std::uint32_t instance_extension_count = 0;
      vkEnumerateInstanceExtensionProperties( nullptr, &instance_extension_count, nullptr );
      std::vector<VkExtensionProperties> instance_extensions_properties( instance_extension_count );
      vkEnumerateInstanceExtensionProperties( nullptr, &instance_extension_count, instance_extensions_properties.data( ) );

      instance_extensions.resize( instance_extension_count );

      bool has_surface_extension = false;
      bool has_xcb_extension = false;
      for ( auto const& property : instance_extensions_properties )
      {
         if ( strcmp( "VK_KHR_surface", property.extensionName ) == 0 )
         {
            has_surface_extension = true;
         }

         if ( strcmp( "VK_KHR_xcb_surface", property.extensionName ) == 0 )
         {
            has_xcb_extension = true;
         }

         instance_extensions.emplace_back( property.extensionName );

         if ( p_logger )
         {
            p_logger->info( "[{0}] Instance extension \"{1}\" enabled", __FUNCTION__, property.extensionName );
         }
      }

      instance_create_info.enabledExtensionCount = instance_extension_count;
      instance_create_info.ppEnabledExtensionNames = instance_extensions.data( );

      auto instance_res = vkCreateInstance( &instance_create_info, nullptr, &instance );
      if ( instance_res != VK_SUCCESS )
      {
         if ( p_logger )
         {
            p_logger->error( "[{0}] Failed to create vulkan instance: {1}", __FUNCTION__, instance_res );
         }
      }
      else
      {
         if ( p_logger )
         {
            p_logger->info( "[{0}] Vulkan instance created: 0x{1:x}", __FUNCTION__, reinterpret_cast<std::uintptr_t>( &instance ) );
         }
      }

      if constexpr ( vk::enable_debug_layers )
      {
         VkDebugUtilsMessengerCreateInfoEXT create_info = {};
         create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
         create_info.pNext = nullptr;
         create_info.flags = 0;
         create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
         create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
         create_info.pfnUserCallback = debug_callback;
         create_info.pUserData = nullptr;

         auto debug_messenger_res = vkCreateDebugUtilsMessengerEXT( instance, &create_info, nullptr, &debug_messenger );
         if ( debug_messenger_res != VK_SUCCESS )
         {
            if ( p_logger )
            {
               p_logger->error( "[{0}] Failed to create vulkan debug utils messenger: {1}", __FUNCTION__, debug_messenger_res );
               p_logger->flush( );
            }
         }
         else
         {
            if ( p_logger )
            {
               p_logger->info( "[{0}] Vulkan debug utils messenger created: 0x{1:x}", __FUNCTION__,
                  reinterpret_cast<std::uintptr_t>( &debug_messenger ) );
            }
         }
      }

      core::uint32 physical_device_count = 0;
      vkEnumeratePhysicalDevices( instance, &physical_device_count, nullptr );
      available_physical_devices.resize( physical_device_count );
      vkEnumeratePhysicalDevices( instance, &physical_device_count, available_physical_devices.data( ) );

      if ( available_physical_devices.size( ) == 0 )
      {
         if ( p_logger )
         {
            p_logger->error( "[{0}] Failed to find a usable graphics card for rendering", __FUNCTION__ );
         }
      }
   }

   context::~context( )
   {
      if constexpr ( vk::enable_debug_layers )
      {
         if ( debug_messenger != VK_NULL_HANDLE )
         {
            vkDestroyDebugUtilsMessengerEXT( instance, debug_messenger, nullptr );
            debug_messenger = VK_NULL_HANDLE;
         }
      }

      if ( instance != VK_NULL_HANDLE )
      {
         vkDestroyInstance( instance, nullptr );
         instance = VK_NULL_HANDLE;
      }
   }

   void context::select_best_physical_device( physical_device_selection_interface const* p_selection )
   {
      assert( p_selection == nullptr && "p_selection is nullptr" );

      physical_device = p_selection->select_physical_device( available_physical_devices );
   }

   void context::init_volk( logger* p_logger ) const
   {
      if ( !VOLK_IS_INITIALIZED )
      {
         auto volk_result = volkInitialize( );
         if ( volk_result != VK_SUCCESS )
         {
            if ( p_logger )
            {
               p_logger->error( "[{0}] Failed to initialize volk: {1}", __FUNCTION__, volk_result );
            }
         }
         else
         {
            if ( p_logger )
            {
               p_logger->info( "[{0}] Volk initialization was successful", __FUNCTION__ );
            }
         }
      }
   }
} // namespace vk
