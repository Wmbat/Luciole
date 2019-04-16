#include "uniform_buffers.hpp"

namespace lcl::vulkan
{
    uniform_buffers::create_info::create_info( 
        const VmaAllocator allocator, 
        std::uint32_t queue_family_index_count, 
        std::uint32_t* p_queue_family_indices, 
        std::uint32_t buffer_size, 
        std::uint32_t buffer_count )
        :
        allocator_( allocator ),
        queue_family_index_count_( queue_family_index_count ),
        p_queue_family_indices_( p_queue_family_indices ),
        buffer_size_( buffer_size ),
        buffer_count_( buffer_count )
    {   }
    
    uniform_buffers::create_info& uniform_buffers::create_info::set_vma_allocator( const VmaAllocator allocator ) noexcept
    {
        allocator_ = allocator;
        return *this;
    }
    
    uniform_buffers::create_info& uniform_buffers::create_info::set_queue_family_index_count( std::uint32_t queue_index_count ) noexcept
    {
        queue_family_index_count_ = queue_index_count;
        return *this;
    }
    
    uniform_buffers::create_info& uniform_buffers::create_info::set_p_queue_family_indices( std::uint32_t* p_queue_family_indices ) noexcept
    {
        p_queue_family_indices_ = p_queue_family_indices;
        return *this;
    }
    
    uniform_buffers::create_info& uniform_buffers::create_info::set_buffer_size( std::uint32_t buffer_size ) noexcept
    {
        buffer_size_ = buffer_size;
        return *this;
    }
    
    uniform_buffers::create_info& uniform_buffers::create_info::set_buffer_count( std::uint32_t buffer_count ) noexcept
    {
        buffer_count_ = buffer_count;
        return *this;
    }
    
    uniform_buffers::uniform_buffers( const create_info& create_info )
        :
        allocator_( create_info.allocator_ )
    {
        buffers_.resize( create_info.buffer_count_ );
        memory_allocations_.resize( create_info.buffer_count_ );
    
        auto sharing_mode = vk::SharingMode{ };
    
    	if ( create_info.queue_family_index_count_ == 1 )
    	{
    		sharing_mode = vk::SharingMode::eExclusive;
    	}
    	else if ( create_info.queue_family_index_count_ > 1 )
    	{
    		if ( create_info.p_queue_family_indices_[0] == create_info.p_queue_family_indices_[1] )
    		{
    			sharing_mode = vk::SharingMode::eExclusive;
    		}
    		else
    		{
    			sharing_mode = vk::SharingMode::eConcurrent;
    		}
    	}
    
        for( std::uint32_t i = 0; i < create_info.buffer_count_; ++i )
        {
            const auto buffer_create_info = vk::BufferCreateInfo( )
                .setSize( create_info.buffer_size_ )
                .setUsage( vk::BufferUsageFlagBits::eUniformBuffer )
                .setSharingMode( sharing_mode )
                .setQueueFamilyIndexCount( create_info.queue_family_index_count_ )
                .setPQueueFamilyIndices( create_info.p_queue_family_indices_ );
    
            VmaAllocationCreateInfo allocation_info = { };
            allocation_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        
            vmaCreateBuffer( allocator_,
                reinterpret_cast<const VkBufferCreateInfo*>( &buffer_create_info ), &allocation_info,
                reinterpret_cast<VkBuffer*>( &buffers_[i] ), &memory_allocations_[i], nullptr );
        }
    }
    uniform_buffers::uniform_buffers( uniform_buffers&& other )
    {
        *this = std::move( other );
    }
    uniform_buffers::~uniform_buffers( )
    {
        if ( allocator_ )
        {
            for( size_t i = 0; i < buffers_.size( ); ++i )
            {
                vmaDestroyBuffer( allocator_, buffers_[i], memory_allocations_[i] );
            }
        }
    }
    
    uniform_buffers& uniform_buffers::operator=( uniform_buffers&& other )
    {
        if ( this != &other )
        {
            if ( allocator_ && !buffers_.empty( ) )
            {
                for( size_t i = 0; i < buffers_.size( ); ++i )
                {
                    vmaDestroyBuffer( allocator_, buffers_[i], memory_allocations_[i] );
                }
            }
    
            allocator_ = other.allocator_;
            other.allocator_ = VK_NULL_HANDLE;
    
            buffers_.resize( other.buffers_.size( ) );
    
            for( size_t i = 0; i < buffers_.size( ); ++i )
            {
                buffers_[i] = other.buffers_[i];
            }
        }
    }
}
