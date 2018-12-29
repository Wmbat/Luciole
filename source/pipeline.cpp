
#include "pipeline.h"
#include "vk_utils.h"

namespace TWE
{
    pipeline_cache::pipeline_cache( const vk::Device& device )
    {
    
    }
    
    ////////////////
    
    pipeline_layout::pipeline_layout( const vk::Device& device )
    {
        auto create_info = vk::PipelineLayoutCreateInfo( );
        
        auto res = device.createPipelineLayoutUnique( create_info );
        check_vk_result( res.result, "failed to create pipeline layout" );
        
        layout_.swap( res.value );
    }
    
    pipeline_layout::pipeline_layout( pipeline_layout&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    
    pipeline_layout& pipeline_layout::operator=( pipeline_layout&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            layout_ = std::move( rhs.layout_ );
        }
        
        return *this;
    }
    
    const vk::PipelineLayout& pipeline_layout::get( ) const noexcept
    {
        return layout_.get();
    }
    
    /////////////
    
    pipeline::pipeline( vk::Device* p_device, vk::Pipeline&& pipeline, const type type,
        const pipeline_layout::id layout_id )
        :
        p_device_( p_device ),
        pipeline_( std::move( pipeline ) ),
        type_( type ),
        layout_id_( layout_id )
    {
    
    }
    pipeline::pipeline( pipeline&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    pipeline::~pipeline( )
    {
        if( p_device_ != nullptr )
            p_device_->destroyPipeline( pipeline_ );
    }
    
    pipeline& pipeline::operator=( pipeline&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            p_device_ = rhs.p_device_;
            rhs.p_device_ = nullptr;
            
            pipeline_ = std::move( rhs.pipeline_ );
            
            type_ = rhs.type_;
            rhs.type_ = type{ };
    
            layout_id_ = rhs.layout_id_;
            rhs.layout_id_ = 0;
            
            cache_id_ = rhs.cache_id_;
            rhs.cache_id_ = 0;
        }
        
        return *this;
    }
    
    vk::PipelineBindPoint pipeline::get_bind_point( ) const
    {
        if( type_ == type::graphics )
        {
            return vk::PipelineBindPoint::eGraphics;
        }
        else if( type_ == type::compute )
        {
            return vk::PipelineBindPoint::eCompute;
        }
        else
        {
            return vk::PipelineBindPoint::eRayTracingNV;
        }
    }
    
    const vk::Pipeline& pipeline::get( ) const
    {
        return pipeline_;
    }
}
