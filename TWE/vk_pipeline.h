
#ifndef TWE_VK_PIPELINE_H
#define TWE_VK_PIPELINE_H

#include "vk_utils.h"
#include "vk_shader.h"

namespace TWE
{
    class vk_pipeline
    {
    public:
        using id = std::uint32_t;
        
        enum class type
        {
            graphics,
            computer,
            ray_tracing
        };
    
        struct create_info
        {
            vk_shader::id vert_shader_id_ = 0;
            vk_shader::id frag_shader_id_ = 0;
        
            std::vector<std::string> pipelines_filepath_;
        
            vk::Extent2D swapchain_extent_;
        };
        
    private:
        vk::Pipeline pipeline_;
        type type_;
    };
}

#endif //TWE_VK_PIPELINE_H
