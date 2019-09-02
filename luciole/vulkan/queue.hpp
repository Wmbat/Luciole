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

#ifndef LUCIOLE_VULKAN_QUEUE_HPP
#define LUCIOLE_VULKAN_QUEUE_HPP

#include "errors.hpp"

#include "../luciole_core.hpp"

#include "../utilities/enum_operators.hpp"

class queue
{
private:
    struct param { };
    struct index_param { };
    struct family_index_param { };

public:
    enum class flag
    {
        e_none      = 0,
        e_graphics  = 1 << 1,
        e_transfer  = 1 << 2,
        e_compute   = 1 << 3
    };

    using flag_t = strong_type<flag, param>;
    using index_t = strong_type<std::uint32_t, index_param>;
    using family_index_t = strong_type<std::uint32_t, family_index_param>;

public:
    queue( ) = default;
    queue( vk::device_t device, family_index_t family_index, index_t index );
    queue( queue const& rhs ) = delete;
    queue( queue && rhs );

    queue& operator=( queue const& rhs ) = delete;
    queue& operator=( queue && rhs );

    [[nodiscard]] vk::error::type submit( vk::submit_info_t info, vk::fence_t fence ) const noexcept;
    [[nodiscard]] vk::error::type present( vk::present_info_t info ) const noexcept;

    [[nodiscard]] std::uint32_t get_family_index( ) const noexcept;

private:
    VkQueue handle_  = VK_NULL_HANDLE;
    std::uint32_t family_index_ = 0;
    std::uint32_t index_ = 0;
};

ENABLE_BITMASK_OPERATORS( queue::flag );

#endif // LUCIOLE_VULKAN_QUEUE_HPP