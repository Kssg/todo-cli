#include <stdio.h>
#include <string.h>
#include "todo.h"

void print_usage() {
    printf("📋 使用方法：\n");
    printf("  todo add \"代辦事項\"\n");
    printf("  todo list\n");
    printf("  todo done <id>\n");
    printf("  todo delete <id>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            fprintf(stderr, "❌ 請提供要新增的代辦事項內容。\n");
            return 1;
        }
        add_todo(argv[2]);

    } else if (strcmp(argv[1], "list") == 0) {
        list_todos();

    } else if (strcmp(argv[1], "done") == 0) {
        if (argc < 3) {
            fprintf(stderr, "❌ 請提供要標記完成的項目 id。\n");
            return 1;
        }
        done_todo(argv[2]);

    } else if (strcmp(argv[1], "delete") == 0) {
        if (argc < 3) {
            fprintf(stderr, "❌ 請提供要刪除的項目 id。\n");
            return 1;
        }
        delete_todo(argv[2]);

    } else {
        fprintf(stderr, "❌ 無效的指令:%s\n", argv[1]);
        return 1;
    }

    return 0;
}
