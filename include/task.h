#ifndef TASK_H
#define TASK_H

typedef struct task_metrics {
    unsigned long arrival;
    unsigned long first_run;
    unsigned long duration;
    unsigned long completion;
} task_metrics_t;

typedef struct task {
    unsigned long pid;
    unsigned long v_runtime;
    char (*run)(unsigned long, struct task*); // runs a task for 1 MIN_GRANULARITY. Returns 0 to keep running, 1 to be context switched, and 2 to complete.
    char nice;
    task_metrics_t metrics;
} task_t;

/*
    Creates a task with the given pid, nice value, and run function.
    @param pid The process ID of the task.
    @param nice The nice value of the task.
    @param run The function to run the task.
    @return A pointer to the created task, or NULL if memory allocation fails.
*/
task_t *create_task(unsigned long pid, char nice, char (*run)(unsigned long, task_t*));

/*
    Compares two tasks for ordering based on their v_runtime.
    @param t1 The first task to compare.
    @param t2 The second task to compare.
    @return A negative value if t1 < t2, a positive value if t1 > t2, and 0 if they are equal.
*/
int compare_running_tasks(task_t *t1, task_t *t2);

/*
    Compares two tasks for ordering based on their arrival time.
    @param t1 The first task to compare.
    @param t2 The second task to compare.
    @return A negative value if t1 < t2, a positive value if t1 > t2, and 0 if they are equal.
*/
int compare_scheduled_tasks(task_t *t1, task_t *t2);

#endif // TASK_H