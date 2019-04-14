/*
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

#include <iostream>

#include <luciole/luciole.hpp>

#include <luciole/window/xcb_window.hpp>

int main( int args, char** argv )
{
    lcl::xcb_window window( "Demo" );
    lcl::renderer renderer( &window, "Demo", 0 );
    renderer.set_clear_colour( glm::vec4( 48.f, 10.f, 36.f, 1.f ) );

    const std::uint32_t vert_id = renderer.create_vertex_shader( "resources/shaders/vert.spv" );
    const std::uint32_t frag_id = renderer.create_fragment_shader( "resources/shaders/frag.spv" );

    const auto pipeline_id = renderer.create_graphics_pipeline( "resources/triangle_pipeline.json", vert_id, frag_id );

    renderer.set_pipeline( pipeline_id );

    auto time_point = std::chrono::steady_clock::now( );
    float max_dt = 1.0f / 20.0f;
        
    while( window.is_open() )
    {
        window.poll_events();
            
        float dt;
        {
            const auto new_time_point = std::chrono::steady_clock::now( );
            dt = std::chrono::duration<float>( new_time_point - time_point ).count( );
            time_point = new_time_point;
        }
            // dt = std::min( dt, max_dt );
            
            // std::cout << "dt: " << 1/dt << '\n';
            
        renderer.draw_frame( );
    }
    
    return 0;
}

class player
{
public:
    void on_key_event( const lcl::key_event& event )
    {
    
    }
    void on_mouse_button( const lcl::mouse_button_event& event )
    {
        // std::cout << "Mouse button pressed: " << static_cast<uint32_t>( event.code_ ) << std::endl;
    }
};

/*
class demo : public lcl::application
{
public:
    demo( const std::string& title )
        :
        lcl::application( title )
    {
        p_renderer_->set_clear_colour( glm::vec4( 48.f, 10.f, 36.f, 1.f ) );
        
        vert_id_ = p_renderer_->create_shader<lcl::shader_type::vertex>( "../../demo/resources/shaders/vert.spv", "main" );
        frag_id_ = p_renderer_->create_shader<lcl::shader_type::fragment>( "../../demo/resources/shaders/frag.spv", "main" );
        
        std::string triangle_pipeline = "../../demo/resources/triangle_pipeline.json";
        std::string wireframe_triangle_pipeline = "../../demo/resources/wireframe_triangle_pipeline.json";
        
        pipeline_ids_.emplace_back( p_renderer_->create_pipeline<lcl::pipeline_type::graphics>( triangle_pipeline, vert_id_, frag_id_ ) );
        
        p_renderer_->set_pipeline( pipeline_ids_[0] );
        
        p_wnd_->set_event_callback( lcl::key_event_delg( player_, &player::on_key_event ) );
        p_wnd_->set_event_callback( lcl::mouse_button_event_delg( player_, &player::on_mouse_button ) );
        
        p_wnd_->set_event_callback( lcl::mouse_button_event_delg( [this]( const lcl::mouse_button_event& event )
            {
                if ( event.code_ == lcl::mouse::button::l_button )
                    p_renderer_->switch_pipeline ( pipeline_ids_[1] );
    
                if ( event.code_ == lcl::mouse::button::r_button )
                    p_renderer_->switch_pipeline ( pipeline_ids_[0] );
            } ) );
    }
    ~demo( ) override
    {
    
    }
    
    void run( ) override
    {
        auto time_point = std::chrono::steady_clock::now( );
        float max_dt = 1.0f / 20.0f;
        
        while( p_wnd_->is_open() )
        {
            p_wnd_->poll_events();
            
            float dt;
            {
                const auto new_time_point = std::chrono::steady_clock::now( );
                dt = std::chrono::duration<float>( new_time_point - time_point ).count( );
                time_point = new_time_point;
            }
            // dt = std::min( dt, max_dt );
            
            // std::cout << "dt: " << 1/dt << '\n';
            
            p_renderer_->draw_frame( );
        }
    }

private:
    player player_;
    
    float time_passed_ = 0;
    
    uint32_t vert_id_ = 0;
    uint32_t frag_id_ = 0;
    
    std::vector<uint32_t> pipeline_ids_;
};

std::unique_ptr<lcl::application> lcl::create_application( )
{
    auto p_demo = std::make_unique<demo>( "Demo" );
    
    return std::move( p_demo );
}
*/