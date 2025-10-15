#ifndef TASK_H
#define TASK_H

typedef enum {
    STATUS_TODO,
    STATUS_IN_PROGRESS,
    STATUS_DONE
} TaskStatus;

typedef struct {
    char *title;
    TaskStatus status;
} SubTask;

typedef struct {
    char *title;
    TaskStatus status;
    int subtask_count;
    SubTask *subtasks;
    int is_collapsed;  // 0 = 展開, 1 = 折疊
} Task;

typedef struct {
    Task *todos;
    int todo_count;

    Task *in_progress;
    int in_progress_count;

    Task *done;
    int done_count;

    char **notes;
    int note_count;
} TaskManager;


void load_tasks_from_json(TaskManager *tm, const char *filename);
void save_tasks_to_json(const TaskManager *tm, const char *filename);
void print_tasks(const TaskManager *tm);
void free_task_manager(TaskManager *tm);

#endif
