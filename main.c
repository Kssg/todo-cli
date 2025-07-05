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
    FILE *read_file = fopen(TODO_FILE, "r");
    if (!read_file) {
        perror("無法開啟檔案");
        return;
    }

    if (strlen(title) == 0) {
        fprintf(stderr, "❌ 代辦事項標題不能為空。\n");
        fclose(read_file);
        return;
    }

    if (strlen(title) > MAX_TITLE_LEN - 1) {
        fprintf(stderr, "❌ 代辦事項標題過長，請限制在 %d 字元以內。\n", MAX_TITLE_LEN - 1);
        fclose(read_file);
        return;
    }

    for (int i = 0; i < strlen(title); i++) {
        if (title[i] == '|' || title[i] == '\n') {
            fprintf(stderr, "❌ 代辦事項標題不能包含 '|' 或 '\n' 字元。\n");
            fclose(read_file);
            return;
        }
    }

    int id = 1;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), read_file)) {
        id++;
    }
    fclose(read_file);

    FILE *file = fopen(TODO_FILE, "a");
    if (!file) {
        perror("無法寫入檔案");
        return;
    }
    fprintf(file, "%d|%s|0\n", id, title);
    fclose(file);
    printf("✅ 已新增：%s\n", title);
}

void delete_todo(const char *index_str) {
    FILE *file = fopen(TODO_FILE, "r");
    if (!file) {
        perror("❌ 無法開啟檔案");
        return;
    }

    char lines[MAX_ITEMS][MAX_TITLE_LEN];
    int count = 0;
    while (fgets(lines[count], sizeof(lines[0]), file)) {   // fix：file 會大於 MAX_TITLE_LEN
        if (++count >= MAX_ITEMS) {
            fprintf(stderr, "❌ 超出支援上限，請擴充程式邏輯。\n");
            fclose(file);
            return;
        }
    }
    fclose(file);

    // check range 
    int index = atoi(index_str);    // fix
    if (index <= 0 || index > count) {
        fprintf(stderr, "❌ 數字超出資料範圍。\n");
        return;
    }

    // store title of deleted line to print output
    char deleted_title[MAX_TITLE_LEN];
    char tmp_line[MAX_TITLE_LEN];
    strncpy(tmp_line, lines[index - 1], MAX_TITLE_LEN);

    char *token = strtok(tmp_line, "|");
    token = strtok(NULL, "|");
    if (token) strncpy(deleted_title, token, MAX_TITLE_LEN);

    file = fopen(TODO_FILE, "w");
    if (!file) {
        perror("❌ 無法寫入檔案");
        return;
    }

    int num = 1;
    for (int i = 0; i < count; ++i) {
        if (i == index - 1) continue;

        TodoItem item;
        char *token = strtok(lines[i], "|");
        item.id = num++;
        token = strtok(NULL, "|");
        if (token) strncpy(item.title, token, MAX_TITLE_LEN);
        token = strtok(NULL, "|");
        if (token) item.done = atoi(token);

        fprintf(file, "%d|%s|%d\n", item.id, item.title, item.done);
    }
    fclose(file);
    printf("✅ 已刪除：%s\n", deleted_title);
}

void done_todo(const char *id) {
    FILE *file = fopen(TODO_FILE, "r");
    if (!file) {
        perror("❌ 無法開啟檔案");
        return;
    }

    char lines[MAX_ITEMS][MAX_TITLE_LEN];
    int count = 0;
    while (fgets(lines[count], sizeof(lines[0]), file)) {
        if (++count >= MAX_ITEMS) {
            fprintf(stderr, "❌ 超出支援上限，請擴充程式邏輯。\n");
            fclose(file);
            return;
        }
    }
    fclose(file);

    int index = atoi(id);   // fix
    if (index <= 0 || index > count) {
        fprintf(stderr, "❌ 數字超出資料範圍。\n");
        return;
    }

    // for 之前，先把 title 取出來
    char done_title[MAX_TITLE_LEN];
    char tmp_line[MAX_TITLE_LEN];
    strncpy(tmp_line, lines[index - 1], MAX_TITLE_LEN);

    char *token = strtok(tmp_line, "|");    // 取 id 跳過
    token = strtok(NULL, "|");  // 取 title
    if (token) strncpy(done_title, token, MAX_TITLE_LEN);
    
    // 如果已經 done 就不用做下去了
    token = strtok(NULL, "|");
    if (token) {
        if (atoi(token) == 1) {
            fprintf(stderr, "⚠️ 該項目已完成：%s\n", done_title);
            return;
        }
    } 

    file = fopen(TODO_FILE, "w");
    if (!file) {
        perror("❌ 無法寫入檔案");
        return;
    }

    for (int i = 0; i < count; ++i) {
        TodoItem item;

        char *token = strtok(lines[i], "|");
        if (token) item.id = atoi(token);
        token = strtok(NULL, "|");
        if (token) strncpy(item.title, token, MAX_TITLE_LEN);
        token = strtok(NULL, "|");
        if (token) item.done = atoi(token);
        
        if (i == index - 1 && item.done == 0) {
           item.done = 1; 
        }
        fprintf(file, "%d|%s|%d\n", item.id, item.title, item.done);
    }
    fclose(file);
    printf("✅ 已完成：[%d] %s\n", index, done_title);
}

void list_todos() {
    FILE *file = fopen(TODO_FILE, "r");
    if (!file) {
        perror("📂 尚無代辦事項。\n");
        return;
    }

    char buffer[MAX_TITLE_LEN];   // fix
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
        fprintf(stderr, "📂 尚無代辦事項。\n");
        return;
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
    } else if (strcmp(argv[1], "done") == 0 && argc >= 3) {
        done_todo(argv[2]);
    } else {
        printf("未知指令:%s\n", argv[1]);
    }

    return 0;
}

