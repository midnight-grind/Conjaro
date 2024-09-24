int handle_up_arrow(int buffer_index, int *newline_indexes)
{
    int range = find_range(buffer_index, newline_indexes);
    
    // example [5, 11, 14, 22]
    int previous_newline_index = find_previous_newline_index(buffer_index, newline_indexes);
}

int handle_down_arrow(int buffer_index, int *newline_indexes)
{
    int range = find_range(buffer_index, newline_indexes);

    int next_newline_index = find_next_newline_index(buffer_index, newline_indexes);
}

int find_range(int buffer_index, int *newline_indexes)
{

    // case 1: buffer_index falls on or before the first newline, cannot go up
    if (buffer_index <= newline_indexes[0])
        return -1;

    // case 2: buffer_index falls after the last newline
    if (buffer_index > newline_indexes[strlen(newline_indexes) - 1])
        return buffer_index - newline_indexes[strlen(newline_indexes) - 1];

    // case 3: buffer_index falls on a new line, go up/down another line
    for (int i=0; i<strlen(newline_indexes); i++)
        if (buffer_index == i)
            return 0;

    // case 4: buffer_index falls somewhere in the middle
    for (int i=0; i<strlen(newline_indexes); i++)
        if (buffer_index > newline_indexes[i])
            return buffer_index - i; // tells you how far into the next new line we should move
}

int find_previous_newline_index(int buffer_index, int *newline_indexes)
{
    if (buffer_index != 0)
        return buffer_index - 1;
}

int find_next_newline_index(int buffer_index, int *newline_indexes)
{
    if (buffer_index != len(newline_indexes))
        return buffer_index + 1;
}

// notes:
// -can do binary search on case 4 to save complexity
// -case 3 and 4 can be combined with case 3 logic
