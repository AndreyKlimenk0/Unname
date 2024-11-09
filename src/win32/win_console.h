#ifndef CONSOLE_H
#define CONSOLE_H

#include "win_helpers.h"

void append_text_to_console_buffer(const char *text, bool move_to_next_line);
bool create_console(HINSTANCE hinstance);

#endif
