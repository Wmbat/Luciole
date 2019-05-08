#ifndef LUCIOLE_DEVICE_HPP
#define LUCIOLE_DEVICE_HPP

#include "vulkan/vulkan.hpp"

#include "instance.hpp"
#include "surface.hpp"

namespace lcl::vulkan
{
    struct device
    {
    public:
        device( ) = default;
        device( const instance& instance, const surface& surface, const std::vector<extension>& desired_extensions );

    private:
        /**
         * @brief - Check if a graphics card is suitable for rendering and has support for the users
         * required extensions.
         * 
         * @param [in] gpu - The graphics card to check for rendering support.
         * @param [in] surface - The Surface to use for rendering.
         * @param [in] desired_extensions - The extensions the user wants to have enabled if supported
         * @return - true if the graphics card is suitable for rendering.
         * @return - false if the graphics card does not meet the requirements for rendering.
         */
        bool is_gpu_suitable( const vk::PhysicalDevice& gpu, const surface& surface, const std::vector<extension>& desired_extensions ) const;

    public:
        vk::PhysicalDevice gpu_;
        vk::UniqueDevice p_device_;

        vk::Queue graphics_queue_;
        vk::Queue transfer_queue_;

        device_extensions extensions_;
    };
}

#endif // LUCIOLE_DEVICE_HPP
