#include <stdlib.h>

#include "scheduler.h"


void initialize(scheduler_t *scheduler){
    scheduler->tree = rb_create(compare_tasks, NULL);
    scheduler->runtime = 0;
}

void destroy(scheduler_t *scheduler){
    rb_free(scheduler->tree);
}

void add_task(scheduler_t *scheduler, task_t *task) {
    rb_insert(scheduler->tree, task);
}

void run_task(scheduler_t *scheduler){
    rb_delete()
}

int tasks_left(scheduler_t* scheduler){
    return scheduler->tree->count;
}

struct tracker {
    float total;
    unsigned int num;
};

void sum_turnaround(task_t *task, struct tracker* tracker){
    tracker->num++;
    tracker->total+= task->metrics.completion - task->metrics.arrival;
}

float avg_turnaround(scheduler_t* scheduler){
    struct tracker turnaround_tracker = { 0 };
    rb_walk(scheduler->tree, sum_turnaround, &turnaround_tracker);
    return turnaround_tracker.total / turnaround_tracker.num;
}  

void sum_response(task_t *task, struct tracker* tracker){
    tracker->num++;
    tracker->total+= task->metrics.first_run - task->metrics.arrival;
}

float avg_response(scheduler_t* scheduler){
    struct tracker response_tracker = {0};
    rb_walk(scheduler->tree, sum_response, &response_tracker);
    return response_tracker.total / response_tracker.num;
}

void show_metrics(scheduler_t* scheduler){
  float avg_t = avg_turnaround(scheduler);
  float avg_r = avg_response(scheduler);
  printf("Average Turnaround Time: %f \n", avg_t);
  printf("Average Response Time: %f \n", avg_r);
}

void update_runtime(task_t* task, float dt){
    task->v_runtime  += (dt * NICE_0) / nice_to_weight(task->nice);
    task->metrics.duration += dt;
}


