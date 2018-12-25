/*!
 *  Copyright (C) 2018 Wmbat
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

#include <vk_pipeline_manager.h>

#include "vk_pipeline_manager.h"

#include "utilities/file_io.h"

namespace TWE
{
    uint32_t vk_pipeline_manager::create_pipeline( const vk_pipeline_manager::pipeline_create_info& create_info )
    {
        const auto id = ++pipeline_id_count_;
        
        return 0;
    }
}
