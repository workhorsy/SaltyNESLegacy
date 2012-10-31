/*
C++NES Copyright (c) 2012 Matthew Brennan Jones <mattjones@workhorsy.org>
vNES Copyright (c) 2006-2011 Jamie Sanders

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nes_cpp.h"

	bool Parameters::scale = false;
	bool Parameters::sound = true;
	bool Parameters::stereo = false;
	bool Parameters::scanlines = false;
	bool Parameters::fps = false;
	bool Parameters::timeemulation = true;
	bool Parameters::showsoundbuffer = false;
	string Parameters::p1_up = "VK_UP";
	string Parameters::p1_down = "VK_DOWN";
	string Parameters::p1_left = "VK_LEFT";
	string Parameters::p1_right = "VK_RIGHT";
	string Parameters::p1_a = "VK_Z";
	string Parameters::p1_b = "VK_X";
	string Parameters::p1_start = "VK_ENTER";
	string Parameters::p1_select = "VK_CONTROL";
	
	string Parameters::p2_up = "VK_NUMPAD8";
	string Parameters::p2_down = "VK_NUMPAD2";
	string Parameters::p2_left = "VK_NUMPAD4";
	string Parameters::p2_right = "VK_NUMPAD6";
	string Parameters::p2_a = "VK_NUMPAD7";
	string Parameters::p2_b = "VK_NUMPAD9";
	string Parameters::p2_start = "VK_NUMPAD1";
	string Parameters::p2_select = "VK_NUMPAD3";