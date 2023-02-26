/*
 *  main.cc -- GoL Lab -- GUI with various options and view controls
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

#include "simulation.h"
#include "gui.h"
#include "config.h"
#include <iostream>
#include <gtkmm/application.h>
#include <cstdlib>
#include <string>
#include <cstdio>

//===========================================

int main(int argc, char * argv[]) {
	std::string filename("");
	if (argc == 2) {
		if (std::string(argv[1]).find_first_of("-") != 0) {
			filename = argv[1];
			if (!read_file(filename)) filename = "";
		}
	}
	argc = 1;
	set_default_frame();

	srand((unsigned) time(0));

	auto app = Gtk::Application::create(argc, argv, "com.github.clacassa.GoL-Lab");
	SimulationWindow window(app, filename);

	if (filename == "") window.set_title(PROGRAM_NAME);
	else {
		filename = filename_from_filepath(filename);
		window.set_title(filename + "  -  " + PROGRAM_NAME);
	}

	return app->run(window);
}
