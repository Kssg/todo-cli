#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // for errno
#include <limits.h> // for INT_MAX
#include "todo.h"



const char* get_todo_filename() {
    const char *env = getenv("TODO_FILE");
    return env ? env : "todo.txt";
}

void init_todo_file() {
    const char *fname = get_todo_filename();
    FILE *fp = fopen(fname, "a");
    if (!fp) {
        fprintf(stderr, "無法建立 %s: %s\n", fname, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fclose(fp);
}

void strip_newline(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

int safe_atoi(const char *str, int *out) {
    if (!str || !*str) return 0;

    char *end;
    errno = 0;
    long val = strtol(str, &end, 10);

    if (errno != 0 || *end != '\0' || val > INT_MAX || val < INT_MIN) {
        printf("inner if error %d \n", *end == '\0');
        return 0;
    }

    *out = (int)val;
    return 1;
}

int read_all_lines(char lines[MAX_ITEMS][MAX_TITLE_LEN]) {
    FILE *file = fopen(get_todo_filename(), "r");
    if (!file) {
        perror("❌ 無法開啟檔案");
        return -1;
    }

    int count = 0;
    // fix: file size may > sizeof(lines[0])
    while (fgets(lines[count], sizeof(lines[0]), file)) {
        if (++count >= MAX_ITEMS) {
            fprintf(stderr, "❌ 超出支援上限，請擴充程式邏輯。\n");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return count;
}

int parse_line(const char *line, TodoItem *item) {
    char tmp[MAX_TITLE_LEN];
    strncpy(tmp, line, MAX_TITLE_LEN);

    char *token = strtok(tmp, "|");
    if (!token) {
        fprintf(stderr, "❌ 無法解析 ID 欄位：%s\n", line);
        return -1;
    }
    if (!safe_atoi(token, &item->id)) {
        fprintf(stderr, "❌ ID 欄位格式錯誤：%s\n", token);
        return -1;
    }

    token = strtok(NULL, "|");
    if (!token) {
        fprintf(stderr, "❌ 無法解析 Title 欄位：%s\n", line);
        return -1;
    }
    strncpy(item->title, token, MAX_TITLE_LEN);

    token = strtok(NULL, "|");
    if (!token) {
        fprintf(stderr, "❌ 無法解析 Done 欄位：%s\n", line);
        return -1;
    }
    strip_newline(token);
    if (!safe_atoi(token, &item->done)) {
        fprintf(stderr, "❌ Done 欄位格式錯誤：%s\n", token);
        return -1;
    }

    return 0;
}

void write_all_items(FILE *file, TodoItem items[], int count) {
    for (int i = 0; i < count; ++i) {
        fprintf(file, "%d|%s|%d\n", items[i].id, items[i].title, items[i].done);
    }
}

void print_item(const TodoItem *item) {
    printf("[%d] [%s] %s\n", item->id, item->done ? "✅" : "❌", item->title);
}
        
void add_todo(const char *title) {
    FILE *read_file = fopen(get_todo_filename(), "r");
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
    size_t len = strlen(title);
    for (size_t i = 0; i < len; i++) {
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

    FILE *file = fopen(get_todo_filename(), "a");
    if (!file) {
        perror("無法寫入檔案");
        return;
    }
    fprintf(file, "%d|%s|0\n", id, title);
    fclose(file);
    printf("✅ 已新增：%s\n", title);
}

void delete_todo(const char *index_str) {
    char lines[MAX_ITEMS][MAX_TITLE_LEN];
    int count = read_all_lines(lines);

    // check range 
    int index;
    if (!safe_atoi(index_str, &index) || index <= 0 || index > count) {
        fprintf(stderr, "❌ 數字超出資料範圍。\n");
        return;
    }

    // 解析被刪除的項目標題
    // 這樣比較有重用
    TodoItem deleted_item;
    if (parse_line(lines[index - 1], &deleted_item) != 0) {
        fprintf(stderr, "❌ 無法解析要刪除的項目。\n");
        return;
    }

    // 建立新的項目陣列，並跳過要刪除的那一筆
    TodoItem items[MAX_ITEMS];
    int new_count = 0;
    int new_id = 1;
    for (int i = 0; i < count; ++i) {
        if (i == index - 1) continue;

        TodoItem item;
        if (parse_line(lines[i], &item) != 0) continue;
        item.id = new_id++;
        items[new_count++] = item;
    }

    FILE *file = fopen(get_todo_filename(), "w");
    if (!file) {
        perror("❌ 無法寫入檔案");
        return;
    }
    
    write_all_items(file, items, new_count);
    fclose(file);

    printf("✅ 已刪除：[%d] %s\n", deleted_item.id, deleted_item.title);
}

void done_todo(const char *id) {
    char lines[MAX_ITEMS][MAX_TITLE_LEN];
    int count = read_all_lines(lines);

    // 檢查 index 超出範圍
    int index;
    if (!safe_atoi(id, &index) || index <= 0 || index > count) {
        fprintf(stderr, "❌ 數字超出資料範圍。\n");
        return;
    }

    // 邏輯有點改變
    TodoItem items[MAX_ITEMS];

    for (int i = 0; i < count; ++i) {
        if (parse_line(lines[i], &items[i]) != 0) {
            fprintf(stderr, "❌ 無法解析第 %d 行。\n", i + 1);
            return;
        }
    }

    if (items[index - 1].done) {
        fprintf(stderr, "⚠️ 該項目已完成：%s\n", items[index - 1].title);
        return;
    }

    items[index - 1].done = 1;

    FILE *file = fopen(get_todo_filename(), "w");
    if (!file) {
        perror("❌ 無法寫入檔案");
        return;
    }

    write_all_items(file, items, count); 
    fclose(file);

    printf("✅ 已完成：[%d] %s\n", items[index - 1].id, items[index - 1].title);
}

void list_todos(void) {
    FILE *file = fopen(get_todo_filename(), "r");
    if (!file) {
        perror("❌ 無法開啟代辦事項\n");
        return;
    }

    char buffer[MAX_TITLE_LEN];   // fix
    int has_data = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        TodoItem item;
        if (parse_line(buffer, &item) != 0) continue;

        print_item(&item);
        has_data = 1;
    }

    fclose(file);

    if (!has_data) {
        fprintf(stderr, "📂 尚無代辦事項\n");
        return;
    }
}

void clear_todos(void) {
    char lines[MAX_ITEMS][MAX_TITLE_LEN];
    int count = read_all_lines(lines);

    TodoItem items[MAX_ITEMS];
    int new_count = 0;
    int new_id = 1;

    TodoItem deleted_items[MAX_ITEMS];
    int deleted_count = 0;

    for (int i = 0; i < count; i++) {
        TodoItem item;
        if (parse_line(lines[i], &item) != 0) continue;
        if (item.done == 1) {
            deleted_items[deleted_count++] = item;
            continue;
        }
        item.id = new_id++;
        items[new_count++] = item;
    }

    FILE *file = fopen(get_todo_filename(), "w");
    if (!file) {
        perror("❌ 無法寫入檔案");
        return;
    }

    write_all_items(file, items, new_count);
    fclose(file);

    printf("清除已完成事項：\n");
    for (int i = 0; i < deleted_count; i++) {
        print_item(&deleted_items[i]);
    } 
}
