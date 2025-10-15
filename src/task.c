#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // memset
#include "cJSON.h"
#include "task.h"


// 將 status 字串轉為 enum
TaskStatus parse_satus(const char *status) {
    if (strcmp(status, "in_progress") == 0) return STATUS_IN_PROGRESS;
    if (strcmp(status, "done") == 0) return STATUS_DONE;
    return STATUS_TODO;
}

// 將 enum 轉回字串
const char* status_to_string(TaskStatus s) {
    switch(s) {
        case STATUS_IN_PROGRESS: return "in_progress";
        case STATUS_DONE: return "done";
        default: return "todo";
    }
}

void load_tasks_from_json(TaskManager *tm, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("open");
        return;
    }

    // ?
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);

    char *data = malloc(len + 1);
    fread(data, 1, len, fp);
    data[len] = '\0';
    fclose(fp);

    cJSON *root = cJSON_Parse(data);
    if (!root) {
        printf("Error parsing JSON\n");
        free(data);
        return;
    }

    memset(tm, 0, sizeof(TaskManager));

    // 載入 todos/in_progress/done
    const char *sections[] = {"todos", "in_progress", "done"};
    Task **targets[] = {&tm->todos, &tm->in_progress, &tm->done};
    int *counts[] = {&tm->todo_count, &tm->in_progress_count, &tm->done_count};

    for (int i = 0; i < 3; i++) {
        cJSON *array = cJSON_GetObjectItem(root, sections[i]);
        if (!cJSON_IsArray(array)) continue;

        int size = cJSON_GetArraySize(array);
        *targets[i] = calloc(size, sizeof(Task));
        *counts[i] = size;
        
        for (int j = 0; j < size; j++) {
            cJSON *obj = cJSON_GetArrayItem(array, j);
            Task *t = &((*targets[i])[j]);
            t->title = strdup(cJSON_GetObjectItem(obj, "title")->valuestring);
            t->status = parse_satus(cJSON_GetObjectItem(obj, "status")->valuestring);
            t->is_collapsed = cJSON_IsTrue(cJSON_GetObjectItem(obj, "is_collapsed"));
            cJSON *subs = cJSON_GetObjectItem(obj, "subtasks");
            if (cJSON_IsArray(subs)) {
                t->subtask_count = cJSON_GetArraySize(subs);
                t->subtasks = calloc(t->subtask_count, sizeof(SubTask));
                for (int k = 0; k < t->subtask_count; k++) {
                    cJSON *sobj = cJSON_GetArrayItem(subs, k);
                    t->subtasks[k].title = strdup(cJSON_GetObjectItem(sobj, "title")->valuestring);
                    t->subtasks[k].status = parse_satus(cJSON_GetObjectItem(sobj, "status")->valuestring);
                }
            }
        }
    }

    // Notes
    cJSON *notes = cJSON_GetObjectItem(root, "notes");
    if (cJSON_IsArray(notes)) {
        tm->note_count = cJSON_GetArraySize(notes);
        tm->notes = calloc(tm->note_count, sizeof(char*));
        for (int i = 0; i < tm->note_count; i++) {
            tm->notes[i] = strdup(cJSON_GetArrayItem(notes, i)->valuestring);
        }
    }

    cJSON_Delete(root);
    free(data);
}

void save_tasks_to_json(const TaskManager *tm, const char *filename) {
    cJSON *root = cJSON_CreateObject();

    const char *sections[] = {"todos", "in_progress", "done"};
    Task *sources[] = {tm->todos, tm->in_progress, tm->done};
    int counts[] = {tm->todo_count, tm->in_progress_count, tm->done_count};

    for (int i = 0; i < 3; i++) {
        cJSON *arr = cJSON_CreateArray();
        for (int j = 0; j < counts[i]; j++) {
            Task *t = &sources[i][j];
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddStringToObject(obj, "title", t->title);
            cJSON_AddStringToObject(obj, "status", status_to_string(t->status));
            cJSON_AddBoolToObject(obj, "is_collapsed", t->is_collapsed);

            cJSON *subs = cJSON_CreateArray();
            for (int k = 0; k < t->subtask_count; k++) {
                cJSON *sobj = cJSON_CreateObject();
                cJSON_AddStringToObject(sobj, "title", t->subtasks[k].title);
                cJSON_AddStringToObject(sobj, "status", status_to_string(t->subtasks[k].status));
                cJSON_AddItemToArray(subs, sobj);
            }
            cJSON_AddItemToObject(obj, "subtasks", subs);
            cJSON_AddItemToArray(arr, obj);
        }
        cJSON_AddItemToObject(root, sections[i], arr);
    }
    cJSON *notes = cJSON_CreateArray();
    for (int i = 0; i < tm->note_count; i++) {
        cJSON_AddItemToArray(notes, cJSON_CreateString(tm->notes[i]));
    }
    cJSON_AddItemToObject(root, "notes",notes);

    char *json_str = cJSON_Print(root);
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%s", json_str);
    fclose(fp);

    free(json_str);
    cJSON_Delete(root);
}

void print_tasks(const TaskManager *tm) {
    printf("=== To-Do ===\n");
    for (int i = 0; i < tm->todo_count; i++) {
        printf("[%d] %s (%s)\n", i + 1, tm->todos[i].title, status_to_string(tm->todos[i].status));
        if (!tm->todos[i].is_collapsed) {
            for (int j = 0; j < tm->todos[i].subtask_count; i++) {
                printf("   - [%s] %s\n",
                        tm->todos[i].subtasks[j].status == STATUS_DONE ? "x" : " ",
                        tm->todos[i].subtasks[j].title);
            }
        }
    }

    printf("\n=== Done ===\n");
    for (int i = 0; i < tm->done_count; i++) {
        printf("[%d] %s\n", i + 1, tm->done[i].title);
    }

    printf("\n=== Notes ===\n");
    for (int i = 0; i < tm->note_count; i++) {
        printf("- %s\n", tm->notes[i]);
    }
}

void free_task_manager(TaskManager *tm) {
    Task *groups[] = {tm->todos, tm->in_progress, tm->done};
    int counts[] = {tm->todo_count, tm->in_progress_count, tm->done_count};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < counts[i]; j++) {
            Task *t = &groups[i][j];
            free(t->title);
            for (int k = 0; k < t->subtask_count; k++) {
                free(t->subtasks[k].title);
            }
            free(t->subtasks);
        }
        free(groups[i]);
    }

    for (int i = 0; i < tm->note_count; i++)
        free(tm->notes[i]);
    free(tm->notes);
}
