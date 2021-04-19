#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BORDER_SYMBOL '#'

typedef struct {
	int width;
	int height;
	char **data;
} map;

void map_print(map *m)
{
	int row_lim = m->height;
	int col_lim = m->width;

	for (int i = 0; i < row_lim; ++i) {
		for (int j = 0; j < col_lim; ++j) {
			putchar(m->data[i][j]);
		}

		putchar('\n');
	}
}

int get_element(int *arr, int width, int row, int col)
{
	return arr[(row * width) + col];
}

void set_element(int *arr, int width, int row, int col, int val)
{
	arr[(row * width) + col] = val;
} 

// Checks if a given cell (row, col) can be included in DFS 
int DFS_is_safe(map *map, int row, int col, int *visited) 
{ 
	// Return true if:
	return (row >= 0) && (row < map->height)      // row num is in range
		&& (col >= 0) && (col < map->width)       // col num is in range
		&& (map->data[row][col] != BORDER_SYMBOL) // the cell is not a border
		&& get_element(visited, map->width, row, col) == 0;   // it was not visited before
} 

// Performs DFS for a 2D array. It only considers the 4 neighbours:
// up, down, left and right as adjacent vertices.
void DFS(map *map, int curr_row, int curr_col, int *visited) 
{ 
	// These arrays are used to get row and column numbers of the 4 neighbours of a given cell
	static int row_offset[] = {-1,  0, 0, 1}; 
	static int col_offset[] = { 0, -1, 1, 0}; 

	// Mark this cell as visited 
	set_element(visited, map->width, curr_row, curr_col, 1);

	// Recur for all connected neighbours 
	for (int i = 0; i < 4; ++i) 
		if (DFS_is_safe(map, curr_row + row_offset[i], curr_col + col_offset[i], visited)) 
			DFS(map, curr_row + row_offset[i], curr_col + col_offset[i], visited); 
}

int map_count_regions(map *map)
{
	int *visited = (int *) calloc(map->height * map->width, sizeof(int)); 

	int row_lim = map->height;
	int col_lim = map->width;

	int region_count = 0; 
	for (int i = 0; i < row_lim; ++i) {
		for (int j = 0; j < col_lim; ++j) {
			if (map->data[i][j] != BORDER_SYMBOL && !get_element(visited, map->width, i, j)) {
				DFS(map, i, j, visited);
				++region_count;
			}
		}
	}

    return region_count; 
}

// Reads a line from the file pointer. Returns the line and its length via pass-by-reference
// Returns 0 if it has successfully read a line.
// Returns -1 if it could not (EOF).
// Returns -2 if the line consists of nothing but a newline ('\n').
int get_line(char **lineptr, size_t *len, FILE *filep)
{
	int return_val = 0; // success initially

	// Copy a line from the file into the buffer
	int    line_len = 128;
	size_t curr_length = 0;

	char *buff = (char*) malloc(line_len * sizeof(char)); // line buffer
	char *pc = buff;

	while (1) {
		char ch = fgetc(filep);

		if (ch == '\n') {
			return_val = -2;
			break;
		} else if (ch == EOF) {
			return_val = -1;
			break;
		}

		*pc++ = ch;
		curr_length++;

		if ((curr_length + 1) == line_len) {
			line_len *= 2;
			buff = realloc(buff, line_len * sizeof(char));
		}
	}

	// If a line was read, return value isn't actually -2 or -1
	if (curr_length != 0)
		return_val = 0;

	*pc = '\0';
	curr_length++;

	// We didn't necessarily use all the allocated memory in the first buffer
	// Construct a proper return string with actualy length
	buff = (char *) realloc(buff, curr_length);

	*lineptr = buff;
	*len = curr_length;

	return return_val;
}

map map_import(char *mappath)
{
	FILE *mapfile = fopen(mappath, "r");
	if (!mapfile) {
		printf("Error! Could not open %s\n", mappath);
	}

	map map = {0};
	int row_index = 0;

	int first_run_flag = true;
	char  *line;
	size_t len;

	while (get_line(&line, &len, mapfile) != -1) {
		map.data = (char **) realloc(map.data, (map.width + 1) * sizeof(char *));
		map.data[row_index] = (char *) malloc(len * sizeof(char));

		strcpy(map.data[row_index], line);

		++row_index;
		++map.height;

		if (first_run_flag) {
			map.width = (int) len - 1;
			first_run_flag = false;
		}
	}

	return map;
}

int main()
{
	char *mappath = "map.txt";
	map map = map_import(mappath);

	int region_cnt = map_count_regions(&map);
	printf("%d\n", region_cnt);

	return 0;
}