#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "todo.h"

void print_usage() {
    printf("📋 使用方法（可使用子命令或選項）：\n\n");
    printf("  todo add \"代辦事項\"       或    todo -a \"代辦事項\"\n");
    printf("  todo list                 或    todo -l\n");
    printf("  todo done <id>            或    todo -m <id>\n");
    printf("  todo delete <id>          或    todo -d <id>\n");
    printf("  todo clear                或    todo -c\n");
}

int main(int argc, char *argv[]) {
    init_todo_file();

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

    } else if (strcmp(argv[1], "clear") == 0) {
        clear_todos();

    } else if (argv[1][0] == '-') {
        int opt;
        while ((opt = getopt(argc, argv, "a:d:lm:ch")) != -1) {
            switch (opt) {
                case 'a':
                    add_todo(optarg);
                    break;
                case 'd':
                    delete_todo(optarg);
                    break;
                case 'l':
                    list_todos();
                    break;
                case 'm':
                    done_todo(optarg);
                    break;
                case 'c':
                    clear_todos();
                    break;
                case 'h':
                    print_usage();
                    break;
                default:
                    print_usage();
                    return 1;
            }
        }
    } else {
        fprintf(stderr, "❌ 無效的指令:%s\n", argv[1]);
        print_usage();
        return 1;
    }

    return 0;
}
