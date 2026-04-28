#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <queue>
#include <vector>
#include "task.hpp"
#include "task_compare.hpp"

struct scheduler {
    void schedule(task*);
    void tick();
#ifndef DEBUG
private:
#endif
    std::priority_queue<task*, std::vector<task*>, task_compare> tasks;
};

#endif
