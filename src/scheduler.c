#include <stdlib.h>

#include "scheduler.h"

void initialize(scheduler_t *scheduler)
{
    scheduler->tree = rb_create(compare_tasks, NULL);
    scheduler->runtime = 0;
}

void destroy(scheduler_t *scheduler)
{
    rb_free(scheduler->tree);
}

void add_task(scheduler_t *scheduler, task_t *task)
{
    rb_insert(scheduler->tree, task);
}

void run_task(scheduler_t *scheduler)
{
    // check if we have have any tasks left
    if (tasks_left(scheduler) == 0)
    {
        return;
    }
    // get task with smallest v - runtime
    task_t *task = rb_min(scheduler->tree);
    if (task == NULL)
    {
        return;
    }

    rb_delete(scheduler->tree, task);

    // track first run time if this is the first run
    if (task->metrics.first_run == 0)
    {
        task->metrics.first_run = scheduler->runtime;
    }

    char status = task->run(MIN_GRANULARITY, task);
    scheduler->runtime += MIN_GRANULARITY;
    update_runtime(task, MIN_GRANULARITY);

    switch (status)
    {
    // task continues to run
    case 0:
        while (status == 0)
        {
            status = task->run(MIN_GRANULARITY, task);
            scheduler->runtime += MIN_GRANULARITY;
            update_runtime(task, MIN_GRANULARITY);

            // Check if another task now has higher priority
            task_t *next_task = rb_min(scheduler->tree);
            if (next_task && compare_tasks(task, next_task) > 0)
            {
                break;
            }
        }
        if (status == 2)
        {
            task->metrics.completion = scheduler->runtime;
            free(task);
            return;
        }
    // context switch time
    case 1:
        add_task(scheduler, task);
        break;
    case 2: // task completed (don't add back to tree)
        task->metrics.completion = scheduler->runtime;
        free(task);
        break;
    }
}

int tasks_left(scheduler_t *scheduler)
{
    return scheduler->tree->count;
}

struct tracker
{
    float total;
    unsigned int num;
};

void sum_turnaround(task_t *task, struct tracker *tracker)
{
    tracker->num++;
    tracker->total += task->metrics.completion - task->metrics.arrival;
}

float avg_turnaround(scheduler_t *scheduler)
{
    struct tracker turnaround_tracker = {0};
    rb_walk(scheduler->tree, sum_turnaround, &turnaround_tracker);
    return turnaround_tracker.total / turnaround_tracker.num;
}

void sum_response(task_t *task, struct tracker *tracker)
{
    tracker->num++;
    tracker->total += task->metrics.first_run - task->metrics.arrival;
}

float avg_response(scheduler_t *scheduler)
{
    struct tracker response_tracker = {0};
    rb_walk(scheduler->tree, sum_response, &response_tracker);
    return response_tracker.total / response_tracker.num;
}

void show_metrics(scheduler_t *scheduler)
{
    float avg_t = avg_turnaround(scheduler);
    float avg_r = avg_response(scheduler);
    printf("Average Turnaround Time: %f \n", avg_t);
    printf("Average Response Time: %f \n", avg_r);
}

void update_runtime(task_t *task, float dt)
{
    task->v_runtime += (dt * NICE_0) / nice_to_weight(task->nice);
    task->metrics.duration += dt;
}
