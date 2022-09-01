/*
 *  graphic_gui.h -- GameofLife -- GUI with various options and view controls
 *  Copyright (C) 2022 Cyprien Lacassagne

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef GRAPHIC_GUI_H_INCLUDED
#define GRAPHIC_GUI_H_INCLUDED

#include <vector>
#include <gtkmm/drawingarea.h>

#include "graphic.h"

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr);

#endif
