#ifndef TODO_H
#define TODO_H

#include <stdio.h>

#define MAX_ITEMS 100
#define MAX_TITLE_LEN 256

typedef struct {
    int id;
    char title[MAX_TITLE_LEN];
    int done;   // 0: 未完成, 1: 已完成
} TodoItem;

void add_todo(const char *title);
void delete_todo(const char *index_str);
void done_todo(const char *id);
void list_todos(void);
void clear_todos(void);

// utils
void init_todo_file(void);
void strip_newline(char *s);
int safe_atoi(const char *str, int *out);
const char* get_todo_filename();
int read_all_lines(char lines[MAX_ITEMS][MAX_TITLE_LEN], const char *filename);
int parse_line(const char *line, TodoItem *item);
void write_all_items(FILE *file, TodoItem items[], int count);
void print_item(const TodoItem *item);

#endif
