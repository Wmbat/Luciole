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

#ifndef LUCIOLE_APPLICATION_HPP
#define LUCIOLE_APPLICATION_HPP

#include <memory>
#include <unordered_map>

#include <vulkan/vulkan.h>
 
#include "graphics/renderer.hpp"

#include "window/window.hpp"

#include "strong_types.hpp"

#include "context.hpp"

class application
{
public:
    application( );
    ~application( );

    void run( );

private: 
    std::unique_ptr<window> p_wnd_;

    context context_;
    renderer renderer_;
};

#endif // LUCIOLE_APPLICATION_HPP