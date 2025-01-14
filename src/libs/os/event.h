#ifndef EVENT_H
#define EVENT_H

#include "input.h"
#include "../number_types.h"
#include "../structures/queue.h"

enum Event_Type {
	EVENT_TYPE_MOUSE,
	EVENT_TYPE_MOUSE_WHEEL,
	EVENT_TYPE_KEY,
	EVENT_TYPE_CHAR,
	EVENT_TYPE_DOUBLE_CLICK,
};

struct Event {
	Event() {};
	Event_Type type;

	union {
		s32 mouse_wheel_delta;
		char char_key;
		Key_Info key_info;
		Mouse_Info mouse_info;
	};

	bool is_key_up(Key key);
	bool is_key_down(Key key);
};

void pump_events();
void push_event(Event_Type type, int first_value, int second_value);
void run_event_loop();
void clear_event_queue();

bool were_key_events();
bool was_click(Key key);
bool was_key_just_pressed(Key key);
bool was_key_just_released(Key key);
bool was_double_click(Key key);

Queue<Event> *get_event_queue();

#endif

