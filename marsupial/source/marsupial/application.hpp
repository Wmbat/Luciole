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


#ifndef MARSUPIAL_APPLICATION_HPP
#define MARSUPIAL_APPLICATION_HPP

#include <memory>

#include "marsupial_core.hpp"
#include "window/base_window.hpp"
#include "graphics/renderer.hpp"

namespace marsupial
{
    class application
    {
    public:
        MARSUPIAL_API application ( const std::string& title );
        virtual ~application ( ) = default;
        
        virtual void run( ) = 0;

    protected:
        std::unique_ptr<base_window> p_wnd_;
        std::unique_ptr<renderer> p_renderer_;
    };
    
    /**
     * should be defined by the Client.
     */
    std::unique_ptr<application> create_application( );
}

#endif //MARSUPIAL_APPLICATION_HPP