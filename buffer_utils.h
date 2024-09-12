// buffer_utils.h
#ifndef BUFFER_UTILS_H
#define BUFFER_UTILS_H

struct Cursor {
	int x;
	int y;
};

int resize_buffer(char** buffer, int* num_chars);
int add_character_to_buffer(char** buffer, int* num_chars, char new_character);
int process_character(char** buffer, int* num_chars, char new_character);
int main_loop();
int adjust_cursor(int x, int y, struct Cursor *cursor);

#endif
