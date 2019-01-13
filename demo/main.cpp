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

#include <twe/twe.hpp>

class player
{
public:
    void on_key_event( const twe::key_event& event )
    {
    
    }
    void on_mouse_button( const twe::mouse_button_event& event )
    {
        // std::cout << "Mouse button pressed: " << static_cast<uint32_t>( event.code_ ) << std::endl;
    }
};

class demo : public twe::application
{
public:
    demo( const std::string& title )
        :
        twe::application( title )
    {
        p_renderer_->set_clear_colour( glm::vec4( 48.f, 10.f, 36.f, 1.f ) );
        
        vert_id_ = p_renderer_->create_shader<twe::shader_type::vertex>( "../../demo/resources/shaders/vert.spv", "main" );
        frag_id_ = p_renderer_->create_shader<twe::shader_type::fragment>( "../../demo/resources/shaders/frag.spv", "main" );
        
        std::string triangle_pipeline = "../../demo/resources/triangle_pipeline.json";
        std::string wireframe_triangle_pipeline = "../../demo/resources/wireframe_triangle_pipeline.json";
        
        pipeline_ids_.emplace_back( p_renderer_->create_pipeline<twe::pipeline_type::graphics>( triangle_pipeline, vert_id_, frag_id_ ) );
        pipeline_ids_.emplace_back( p_renderer_->create_pipeline<twe::pipeline_type::graphics>( wireframe_triangle_pipeline, vert_id_, frag_id_ ) );
        
        p_renderer_->set_pipeline( pipeline_ids_[0] );
        
        p_wnd_->set_event_callback( twe::key_event_delg( player_, &player::on_key_event ) );
        p_wnd_->set_event_callback( twe::mouse_button_event_delg( player_, &player::on_mouse_button ) );
        
        p_wnd_->set_event_callback( twe::mouse_button_event_delg( [this]( const twe::mouse_button_event& event )
            {
                if ( event.code_ == twe::mouse::button::l_button )
                    p_renderer_->switch_pipeline ( pipeline_ids_[1] );
    
                if ( event.code_ == twe::mouse::button::r_button )
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

std::unique_ptr<twe::application> twe::create_application( )
{
    auto p_demo = std::make_unique<demo>( "Demo" );
    
    return std::move( p_demo );
}