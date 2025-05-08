#include <stdio.h>

#include "scheduler.h"

char run_once(unsigned long runtime, task_t *task)
{
    // Simulate running the task for 1 MIN_GRANULARITY
    printf("Running task %lu at runtime %lu\n", task->pid, runtime);
    return runtime > 2000000 ? 2 : 0;
}

int main(int argc, char *argv[])
{
    // Initialize the scheduler
    scheduler_t scheduler;
    initialize(&scheduler);

    // Create tasks
    task_t *task1 = create_task(1, 0, run_once);
    task_t *task2 = create_task(2, 10, run_once);
    task_t *task3 = create_task(3, 0, run_once);
    task3->metrics.arrival = 10000;
    
    // Schedule tasks
    schedule_task_now(&scheduler, task1);
    schedule_task_now(&scheduler, task2);
    schedule_task(&scheduler, task3);

    // Run all tasks
    run_all_tasks(&scheduler);

    show_metrics(&scheduler);

    // Destroy the scheduler
    destroy(&scheduler);

    return 0;

}