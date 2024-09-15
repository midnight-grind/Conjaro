#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_utils.h"
#include <unistd.h>

int main()
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

	if (!has_colors())
	{
		endwin();
		printf("Your terminal does not support color.\n");
		return 1;
	}

	start_color();

	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	attron(COLOR_PAIR(1));

	printw("Hello, ncurses!Hello, ncurses!Hello, ncurses!Hello, ncurses!Hello, ncurses!Hello, ncurses!Hello, ncurses! Hello, ncurses!Hello, ncurses!Hello, ncurses!Hello, ncurses!");
	move(0,0);
	refresh();
	attroff(A_BOLD);
	refresh();


	main_loop();
	endwin();

	printf("conjaro session ended\n");

	return 0;
}

// int process_character(char** buffer, int* num_chars, char new_character)
// {

// 	add_character(&buffer, &num_chars, (char) new_character);
// }

int main_loop()
{
	int num_chars = 1;
	char *buffer = (char*) malloc((num_chars+1) *sizeof(char));

	if (buffer == NULL)
	{
		endwin();
		printf("not enough memory for text buffer");
		return 1;
	}
	buffer[0] = '\0';  // Initialize the buffer with an empty string

	// the better way to do this is:
	// 1: determine which element on the buffer the cursor is
	// 2: convert to a cursor coordinate
	// 3: check if cursor is on screen
	struct Cursor cursor = {0, 0};
	adjust_cursor(cursor.x, cursor.y, &cursor);

	// getyx(stdscr, cursor.y, cursor.x);

	int new_character;
	while ((new_character = getch()) != 27)
	{
		if (new_character == KEY_LEFT)
		{
			if ((&cursor)->x != 0 || (&cursor)->y != 0) // stop cursor from going off left side of screen
				adjust_cursor((&cursor)->x - 1, (&cursor)->y, &cursor);
		}
		else if (new_character == KEY_RIGHT)
		{
			adjust_cursor((&cursor)->x + 1, (&cursor)->y, &cursor);
		}
		else if (new_character == KEY_UP)
		{
			if ((&cursor)->y != 0)
				adjust_cursor((&cursor)->x, (&cursor)->y - 1, &cursor);
		}
		else if (new_character == KEY_DOWN)
		{
			adjust_cursor((&cursor)->x, (&cursor)->y + 1, &cursor);
		}
		else
		{
			add_character_to_buffer(&buffer, &num_chars, new_character);
		}
	}

	return 0;
}

int adjust_cursor(int x, int y, struct Cursor *cursor)
{
	cursor->x = x;
	cursor->y = y;
	move(20, 0);
	printw("cursor location: %d, %d", cursor->x, cursor->y);
	move(cursor->y, cursor->x);

	return 0;
}

// displays the contents of the string buffer to the terminal
int buffer_to_window(char **buffer)
{
	printw("%s", *buffer);

	return 0;
}

int add_character_to_buffer(char **buffer, int *num_chars, char new_character)
{
	int used_length = strlen(*buffer);

	// resizing buffer failed
	if (resize_buffer(buffer, num_chars) != 0)
	{
		return 1;
	}

	// add new character and null terminator
	(*buffer)[used_length] = new_character;
	(*buffer)[used_length + 1] = '\0';

	return 0;
}

// gives you element position of cursor on the string buffer
// given an X, Y position of the ncurses window
int cursor_loc_to_buffer_loc(struct Cursor *cursor, char **buffer)
{
	int buffer_index = 0;

	int count = 0;
	for (int line=0; line<cursor->y; line++) // loop through each line
	{
		int j;
		while ((*buffer)[j] != '\0') // find how many newline characters before cursor
		{
			count++;

			if ((*buffer)[j] == '\n')
			{
				break;
			}

			j++;
		}
	}

	return count + cursor->x;
}

// gives you X, Y coordinates of cursor on window
// given an element position on the string buffer
int buffer_loc_to_cursor_loc(int index, char **buffer)
{
	struct Cursor ret_cursor = {0, 0};

	// count how many newline characters between start of buffer and index (non-inclusive)
	int newline_count = 0;
	int x_count = 0;
	for (int i=0; i<index; i++)
	{
		x_count ++;

		if ((*buffer)[i] == '\n')
		{
			newline_count ++;
			x_count = 0;
		}
	}

	ret_cursor.y = newline_count;
	ret_cursor.x = x_count;
}

bool cursor_on_screen(struct Cursor *cursor)
{
	// get screen dimensions
	int screen_size_x, screen_size_y;
	getmaxyx(stdscr, screen_size_y, screen_size_x);

	if (cursor->x < 0 || cursor->x >= screen_size_x)
		return false;
	
	if (cursor->y < 0 || cursor->y >= screen_size_y)
		return false;

	return true;
}

// prints the contents of the buffer to the ncurses window
int display_buffer(char **buffer)
{
	// get screen dimensions
	int screen_size_x, screen_size_y;
	getmaxyx(stdscr, screen_size_y, screen_size_x);

	// cursor starts at top left of screen
	struct Cursor cursor = {0, 0};

	int buffer_length = strlen(*buffer);


	int buffer_index = 0;
	while(buffer_index < buffer_length)
	{
		// write characters to screen until newline or screen width is reached
		for (int i=0; i < screen_size_x && buffer_index < buffer_length; i++)
		{
			if ((*buffer)[buffer_index] == '\n') // end current line if newline is found
			{
				cursor.x = 0;
				cursor.y ++;
				buffer_index ++; // move to character after the next newline found
				break; // move on to next iteration of while loop with new line
			}

			adjust_cursor(cursor.x, cursor.y, &cursor);
			printw("%c", (*buffer)[buffer_index]);

			cursor.x ++;
			buffer_index ++;
		}

		// after finishing a full line or encountering a newline, move to next line
		if (cursor.x >= screen_size_x || (buffer_index < buffer_length && (*buffer)[buffer_index] == '\n'))
		{
			cursor.x = 0;
			cursor.y ++;
		}

		// skip any extra characters until the next newline is found since they won't fit on screen
		while(buffer_index < buffer_length && (*buffer)[buffer_index] != '\n')
		{
			buffer_index ++; // move past characters that should be skipped on this line
		}

		// if we reach a newline, move past it to the next character
		if (buffer_index < buffer_length && (*buffer)[buffer_index] == '\n')
		{
			buffer_index ++;
		}
	}

	return 0;
}

// chatGPT generated function
int resize_buffer(char **buffer, int *num_chars)
{
	// Check if there's enough space for the new character and the null terminator
    int used_length = strlen(*buffer); // Currently used length
    if (used_length + 1 >= *num_chars)
	{
        // Not enough space, so realloc more space (let's double the size)
        int new_num_chars = *num_chars * 2;
        *buffer = (char *) realloc(*buffer, new_num_chars * sizeof(char));

        if (*buffer == NULL)
		{
            printf("text buffer memory reallocation failed\n");
            return 1;
        }

     	*num_chars = new_num_chars;

    }
}
