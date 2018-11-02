//  Copyright (C) 2018 Wmbat
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  You should have received a copy of the GNU General Public License
//  GNU General Public License for more details.
//  along with this program. If not, see <http://www.gnu.org/licenses/>.

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( location = 0 ) in vec4 frag_colour;

layout( location = 0 ) out vec4 out_colour;

void main( )
{
    out_colour = frag_colour;
}