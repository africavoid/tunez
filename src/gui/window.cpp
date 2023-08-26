#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Value_Slider.H>

#include "window.h"

class Window {
	public:
		Fl_Window *window;

		void create_window (void)
		{
			window = new Fl_Window(500, 500);
		}

		void end_session (int argc, char *argv[])
		{
			window->end();
			window->show(argc, argv);
		}
};

class Volume : public Window 
{
	public:
		enum {
			VOLUME_BAR_WIDTH = 25,
			VOLUME_BAR_HEIGHT = 250,
		};

		const char *vol_label = "vol";

		Fl_Value_Slider *slider;

		void draw_volume (void)
		{
			slider = new Fl_Value_Slider(7, 100, VOLUME_BAR_WIDTH, VOLUME_BAR_HEIGHT, vol_label);
		}
};

class Taskbar : public Window
{
	public:
		enum {
			BUTTON_WIDTH = 100,
			BUTTON_HEIGHT = 25,
		};

		struct MAP {
			const char *label;
			void (*fn)(Fl_Widget *w, void*);
		};
	
		/* a function map for the task bar buttons */
		struct MAP map[5] = {
			{ "Play", NULL },
			{ "Pause", NULL },
			{ "Files", NULL },
			{ "Info", NULL },
			{ "Quit", quit },
		};

		Fl_Box *header;
		Fl_Button *button[5];
	
		static void quit(Fl_Widget *w, void*)
		{
			exit(0);
		}

		void draw_header (void)
		{
			header = new Fl_Box(0, 0, 500, 25, "TUNEZ");
			header->box(FL_UP_BOX);
			header->labelsize(14);
			header->labelfont(FL_BOLD);
			header->color(0xab2b2800);
		}
	
		/* draws the taskbar on the screen */
		void draw_taskbar (void)
		{
			for (size_t i = 0, x = 0; i < 5; i++)
			{
				button[i] = new Fl_Button(x, 25, BUTTON_WIDTH, BUTTON_HEIGHT, map[i].label);
				button[i]->type(FL_NORMAL_BUTTON);
				button[i]->labelsize(14);
				button[i]->when(FL_WHEN_RELEASE);
				button[i]->color(0xab2b2800);

				/* function which will run when pressed */
				button[i]->callback((Fl_Callback*) map[i].fn);
				x+=BUTTON_WIDTH;
			}
		}
};
