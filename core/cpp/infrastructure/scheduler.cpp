#include "../../hpp/infrastructure/scheduler.hpp"

void scheduler::schedule(task* t) {
    tasks.push(t);
}

void scheduler::tick() {
    tasks.top()->execute();
    tasks.pop();
}
