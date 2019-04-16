#ifndef LUCIOLE_VULKAN_UNIFORM_BUFFER_HPP
#define LUCIOLE_VULKAN_UNIFORM_BUFFER_HPP

#include "vulkan.hpp"
#include "memory_allocator.hpp"

namespace lcl::vulkan
{
    class uniform_buffers
    {
    public:
        struct create_info
        {
            create_info( 
                const VmaAllocator allocator = VK_NULL_HANDLE, 
                std::uint32_t queue_family_index_count = 0, 
                std::uint32_t* p_queue_family_indices = nullptr, 
                std::uint32_t buffer_size = 0, 
                std::uint32_t buffer_count = 0 ); 

            create_info& set_vma_allocator( const VmaAllocator allocator ) noexcept;
            create_info& set_queue_family_index_count( std::uint32_t index_count ) noexcept;
            create_info& set_p_queue_family_indices( std::uint32_t* p_indices ) noexcept;
            create_info& set_buffer_size( std::uint32_t buffer_size ) noexcept;
            create_info& set_buffer_count( std::uint32_t buffer_count ) noexcept;

            VmaAllocator allocator_;

            uint32_t queue_family_index_count_;
    		uint32_t* p_queue_family_indices_;

            std::uint32_t buffer_size_;
            std::uint32_t buffer_count_;
        };      

    public:
        uniform_buffers( ) = default;
        uniform_buffers( const create_info& create_info );
        uniform_buffers( const uniform_buffers& other ) = delete;
        uniform_buffers( uniform_buffers&& other );
        ~uniform_buffers( );

        uniform_buffers& operator=( const uniform_buffers& other ) = delete;
        uniform_buffers& operator=( uniform_buffers&& other );

    private:
        VmaAllocator allocator_;

        std::vector<vk::Buffer> buffers_;
        std::vector<VmaAllocation> memory_allocations_;
    };
}

#endif // LUCIOLE_VULKAN_UNIFORM_BUFFER_HPP