#include "../../hpp/infrastructure/scheduler.hpp"

void scheduler::schedule(task* t) {
    tasks.push(t);
}

void scheduler::tick() {
    tasks.front()->execute();
    tasks.pop();
}
