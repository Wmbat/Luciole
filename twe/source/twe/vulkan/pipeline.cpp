
#include "pipeline.hpp"
#include "../utilities/vk_utils.hpp"

namespace twe
{
    pipeline_cache::pipeline_cache( const vk::Device& device )
    {
    
    }
    
    ////////////////
    
    pipeline_layout::pipeline_layout( const vk::Device& device, const shader::id vert_id, const shader::id frag_id )
        :
        vert_id_( vert_id ),
        frag_id_( frag_id )
    {
        auto create_info = vk::PipelineLayoutCreateInfo( );
        
        layout_ = device.createPipelineLayoutUnique( create_info );
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
    
    pipeline::pipeline( vk::UniquePipeline&& pipeline, const type type, const pipeline_layout::id layout_id )
        :
        pipeline_( std::move( pipeline ) ),
        type_( type ),
        layout_id_( layout_id )
    {
    
    }
    pipeline::pipeline( pipeline&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    
    pipeline& pipeline::operator=( pipeline&& rhs ) noexcept
    {
        if( this != &rhs )
        {
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
        return pipeline_.get();
    }
}
