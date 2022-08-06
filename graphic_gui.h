#ifndef GRAPHIC_GUI_H_INCLUDED
#define GRAPHIC_GUI_H_INCLUDED

#include <vector>
#include <gtkmm/drawingarea.h>

#include "graphic.h"

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr);

#endif