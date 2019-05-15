#ifndef LUCIOLE_DEVICE_HPP
#define LUCIOLE_DEVICE_HPP

#include "vulkan/volk/volk.h"

#include "instance.hpp"
#include "surface.hpp"

namespace lcl::vulkan
{
    struct device
    {
    public:
        device( ) = default;
        device( const instance& instance, const surface& surface );

    private:
        /**
         * @brief - Check if a graphics card is suitable for rendering
         * 
         * @param [in] gpu - The graphics card to check for rendering support.
         * @param [in] surface - The Surface to use for rendering.
         * @return - true if the graphics card is suitable for rendering.
         * @return - false if the graphics card does not meet the requirements for rendering.
         */
        bool is_gpu_suitable( const VkPhysicalDevice& gpu, const surface& surface ) const;

    public:
        VkPhysicalDevice gpu_;
        VkDevice handle_;

        VkQueue graphics_queue_;
        VkQueue transfer_queue_;

        device_extensions extensions_;
    };
}

#endif // LUCIOLE_DEVICE_HPP
