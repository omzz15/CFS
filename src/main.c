#include <stdio.h>

#include "scheduler.h"



int main(int argc, char *argv[])
{
    // Initialize the scheduler
    scheduler_t scheduler;
    initialize(&scheduler);

    // Create tasks
    task_t *task1 = create_task(1, 0, NULL);
    task_t *task2 = create_task(2, 0, NULL);
    task_t *task3 = create_task(3, 0, NULL);

    // Schedule tasks
    schedule_task_now(&scheduler, task1);
    schedule_task_now(&scheduler, task2);
    schedule_task_now(&scheduler, task3);

    // Run all tasks
    run_all_tasks(&scheduler);

    // Destroy the scheduler
    destroy(&scheduler);

    return 0;

}