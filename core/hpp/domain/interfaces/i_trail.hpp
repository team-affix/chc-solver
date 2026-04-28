#ifndef I_TRAIL_HPP
#define I_TRAIL_HPP

#include <functional>

struct i_trail {
    virtual ~i_trail() = default;
    virtual void push() = 0;
    virtual void pop() = 0;
    virtual void log(const std::function<void()>&) = 0;
};

#endif
