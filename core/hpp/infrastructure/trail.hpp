#ifndef TRAIL_HPP
#define TRAIL_HPP

#include <stack>
#include <functional>
#include "../domain/interfaces/i_trail.hpp"

struct trail : i_trail {
    void push() override;
    void pop() override;
    void log(const std::function<void()>&) override;
    size_t depth() const;
#ifndef DEBUG
private:
#endif
    std::stack<std::function<void()>> undo_stack;
    std::stack<size_t>                frame_boundary_stack;
};

#endif
