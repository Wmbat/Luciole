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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "window/base_window.h"

#include "TWE_core.h"

namespace TWE
{
    class application
    {
    public:
        TWE_API application ( );
        virtual TWE_API ~application ( );

        void run ( );

    private:
        std::unique_ptr<base_window> p_wnd_;
    };
}

#endif //TWE_APPLICATION_H