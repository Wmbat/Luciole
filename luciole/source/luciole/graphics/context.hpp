/*
 * @author wmbat@protonmail.com
 *
 * Copyright (C) 2018-2019 Wmbat
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * You should have received a copy of the GNU General Public License
 * GNU General Public License for more details.
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../context.hpp"

#include "../window/base_window.hpp"

namespace lcl::gfx
{
    class context : public core::context
    {
    public:
        LUCIOLE_API context( ) = default;
        LUCIOLE_API context( base_window* p_wnd, const std::string& app_name, std::uint32_t app_version );
        LUCIOLE_API virtual ~context( );

    private:
        void create_surface( base_window* p_wnd ) noexcept;
        void destroy_surface( ) noexcept;

        virtual void pick_gpu( ) noexcept;
        virtual int rate_gpu( const VkPhysicalDevice gpu ) noexcept;

    private:
        VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    };
} // namespace lcl::gfx