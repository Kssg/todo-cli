#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE_LEN 256
#define TODO_FILE "todo.txt"

typedef struct {
    int id;
    char title[MAX_TITLE_LEN];
    int done;   // 0 = 未完成，1 = 已完成
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

void list_todos() {
    FILE *file = fopen(TODO_FILE, "r");
    if (!file) {
        printf("📂 尚無代辦事項。\n");
        return;
    }

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), file)) {
        TodoItem item;
        char *token = strtok(buffer, "|");
        if (token) item.id = atoi(token);

        token = strtok(NULL, "|");
        if (token) strncpy(item.title, token, MAX_TITLE_LEN);

        token = strtok(NULL, "|");
        if (token) item.done = atoi(token);

        printf("[%d] [%s] %s\n", item.id, item.done ? "✅" : "❌", item.title);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("用法:todo add \"事項\" 或 todo list\n");
        return 1;
    }

    if (strcmp(argv[1], "add") == 0 && argc >=3) {
        add_todo(argv[2]);
    } else if (strcmp(argv[1], "list") == 0) {
        list_todos();
    } else {
        printf("未知指令:%s\n", argv[1]);
    }

    return 0;
}

