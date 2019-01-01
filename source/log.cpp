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

#include "log.h"

namespace twe
{
    std::shared_ptr<spdlog::logger> log::core_logger_;

    void log::init( )
    {
        core_logger_ = spdlog::stdout_color_mt( "TWE_logger" );
        core_logger_->set_pattern( "%^[%T] %n: %v%$" );
        core_logger_->set_level( spdlog::level::trace );
        
        core_info( "spdlog -> TWE_logger Initialized." );
    }
}

