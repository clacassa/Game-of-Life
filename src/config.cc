/*
 * config.cc
 * This file is part of GoL Lab, a simulator of Conway's game of life.
 *
 * Copyright (C) 2022 - Cyprien Lacassagne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#ifdef _WIN32
#  include <windows.h>
#  include <Shlwapi.h>
#  include <Shlobj.h>
#  include <io.h>
#  define access _access_s
#elif __linux__
#  include <limits.h>
#  include <unistd.h>
#endif

std::string working_dir() {
#ifdef _WIN32
	char path[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, path, MAX_PATH);

    std::string executable_path(path);
    char* exe_path = new char[executable_path.length()];
    strcpy(exe_path, executable_path.c_str());
    PathRemoveFileSpecA(exe_path);
    std::string directory = std::string(exe_path);
    size_t pos(directory.find_last_of('\\'));
    return std::string(directory.substr(0, pos+1));
#else
#  ifndef __linux__
#    define PATH_MAX 1024
#  endif
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string path(result, (count > 0) ? count : 0);
    size_t pos = path.find_last_of('/');
    path = path.substr(0, pos);
    pos = path.find_last_of('/');
    path = path.substr(0, pos+1);
    return path;
#endif /* _WIN32 */
}
