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
    
}

char tasks_left(scheduler_t* scheduler){
    return scheduler->tree->count > 0;
}

struct tracker {
    float total;
    unsigned int num;
};

/**
list<Process> rr(pqueue_arrival workload) {
  list<Process> complete;
  pqueue_arrival xs = workload;
  int time = 0;
  //First run
  std::queue<Process> newWorkload;
  newWorkload.push(xs.top());
  xs.pop();
  while(!(newWorkload.empty()) || !(xs.empty())){ //xs.empty used in case still elements left. 
    while((!(xs.empty()) && xs.top().arrival <= time) || newWorkload.empty()){
      newWorkload.push(xs.top());
      xs.pop();
    }
    Process p = newWorkload.front();
    if (time < p.arrival){time = p.arrival;}
    int duration = p.duration;
    if (p.first_run == -1) {p.first_run = time;}
    time += 1;
    p.duration = duration - 1;
    if (p.duration == 0) {
      p.completion = time;
      complete.push_back(p);
    }
    else{
      newWorkload.push(p);
    }
    newWorkload.pop();
  }
  return complete;
}
*/

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

// RB Tree 
void calculate_virtual_runtime(task_t* task){
    task->v_runtime  += (task->tasduration * task->nice) / nice_to_weight(nice);
}


