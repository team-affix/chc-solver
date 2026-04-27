#ifndef TASK_HPP
#define TASK_HPP

struct task {
    virtual ~task() = default;
    virtual void execute() = 0;
};

#endif
