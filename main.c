#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 100
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
    if (strlen(title) == 0) {
        printf("❌ 代辦事項標題不能為空。\n");
        fclose(file);
        return;
    }
    if (strlen(title) > MAX_TITLE_LEN - 1) {
        printf("❌ 代辦事項標題過長，請限制在 %d 字元以內。\n", MAX_TITLE_LEN - 1);
        fclose(file);
        return;
    }
    // 中文怎辦阿阿
    for (int i = 0; i < strlen(title); i++) {
        if (title[i] == '|' || title[i] == '\n') {
            printf("❌ 代辦事項標題不能包含 '|' 或 '\n' 字元。\n");
            fclose(file);
            return;
        }
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

void delete_todo(const char *id) {
    FILE *file = fopen(TODO_FILE, "r");
    if (!file) {
        perror("❌ 無法開啟檔案");
        exit(1);
    }

    // count items
    char lines[MAX_ITEMS][MAX_TITLE_LEN];
    int count = 0;
    while (fgets(lines[count], sizeof(lines[0]), file)) {
        if (++count >= MAX_ITEMS) {
            printf("❌ 超出支援上限，請擴充程式邏輯。\n");
            fclose(file);
            exit(1);
        }
    }
    fclose(file);
    
    int index = atoi(id);
    if (index <= 0 || index > count) {
        printf("❌ 數字超出資料範圍。\n");
        exit(1);
    }

    char deleted_line[MAX_TITLE_LEN];
    strncpy(deleted_line, lines[index - 1], MAX_TITLE_LEN);

    file = fopen(TODO_FILE, "w");
    if (!file) {
        perror("❌ 無法寫入檔案");
        exit(1);
    }
    // char buffer[512]; 我寫這幹嘛
    int num = 1;
    for (int i = 0; i < count; ++i) {
        if (i == index - 1) continue;
        TodoItem item;
        
        char *token = strtok(lines[i], "|");
        item.id = num++;
        // num 後面不會用了

        token = strtok(NULL, "|");
        if (token) strncpy(item.title, token, MAX_TITLE_LEN);

        token = strtok(NULL, "|");
        if (token) item.done = atoi(token);

        fprintf(file, "%d|%s|%d\n", item.id, item.title, item.done);
    }
    fclose(file);
    printf("✅ 已刪除：%s", lines[index - 1]);
}

void list_todos() {
    FILE *file = fopen(TODO_FILE, "r");
    if (!file) {
        perror("📂 尚無代辦事項。\n");
        exit(1);
    }

    char buffer[512];
    int has_data = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        has_data = 1;
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

    if (!has_data) {
        printf("📂 尚無代辦事項。\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("用法:todo add \"事項\" 或 todo list\n");
        return 1;
    }

    if (strcmp(argv[1], "add") == 0 && argc >=3) {
        add_todo(argv[2]);
    } else if (strcmp(argv[1], "remove") == 0 && argc >= 3) {
        delete_todo(argv[2]);
    } else if (strcmp(argv[1], "list") == 0) {
        list_todos();
    } else {
        printf("未知指令:%s\n", argv[1]);
    }

    return 0;
}

