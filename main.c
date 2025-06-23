#include <stdio.h>
#include <stdlib.h>

#define MAX_TITLE_LEN 256
#define TODO_FILE "todo.txt"

typedef struct {
    int id;
    char title[MAX_TITLE_LEN];
    int done;
} TodoItem;

void add_todo(const char *title) {
    FILE *file = fopen(TODO_FILE, "a");
    if (!file) {
        perror("無法開啟檔案");
        exit(1);
    }

    // new id
    int id = 1;
    FILE *read_file = fopen(TODO_FILE, "r");
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), read_file)) {
        id++;
    }
    fclose(read_file);

    fprintf(file, "%d|%s|0\n", id, title);
    fclose(file);
    printf("✅ 已新增：%s\n", title);
}

