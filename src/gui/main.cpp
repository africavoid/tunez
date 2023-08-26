#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Value_Slider.H>

#include "window.h"

int main (int argc, char *argv[])
{
	Window *window = new Window; 
 	Taskbar *taskbar = new Taskbar;
	Volume *volume = new Volume;

	window->create_window();
	taskbar->draw_taskbar();
	taskbar->draw_header();
	volume->draw_volume();
	
	window->end_session(argc, argv);
	return Fl::run();
}
