#include <stdio.h>
#include "task.h"

int main() {
    TaskManager tm;

    load_tasks_from_json(&tm, "tasks.json");

    printf("📋 載入結果：\n");
    print_tasks(&tm);

    // 修改某任務
    tm.todos[0].is_collapsed = !tm.todos[0].is_collapsed;

    save_tasks_to_json(&tm, "task_out.json");
    free_task_manager(&tm);

    printf("\n✅ 已將資料儲存到 tasks_out.json\n");
    return 0;
}
