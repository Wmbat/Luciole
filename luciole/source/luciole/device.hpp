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
         * @brief - 
         * 
         * @param [in] gpu -
         * @param [in] surface - 
         * @param [in] desired_extensions - 
         * @return true -
         * @return false -
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
