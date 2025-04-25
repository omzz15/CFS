// time allocated to each run in ms
#define MIN_GRANULARITY 0.1

typedef struct task_metrics {
    float arrival;
    float first_run;
    float duration;
    float completion;
} task_metrics_t;

typedef struct task {
    float v_runtime;
    char (*run)(float, task_t*); // runs a task for 1 MIN_GRANULARITY. Returns 0 to keep running, 1 to be context switched, and 2 to complete.
    char nice;
    task_metrics_t metrics;
} task_t;


int compare_tasks(task_t *t1, task_t *t2){
    return t1->v_runtime > t2->v_runtime;
}