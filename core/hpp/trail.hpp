#ifndef TRAIL_HPP
#define TRAIL_HPP

#include <functional>
#include <list>
#include <unordered_map>
#include <memory>

struct action {
    std::function<void()> undo;
    std::function<void()> redo;
};

struct frame {
    frame* parent;
    std::unordered_map<const frame*, std::unique_ptr<frame>> children;
    size_t depth;
    std::list<action> actions;
    void revert() const;
    void replay() const;
};

struct trail {
    void log(action);
    void translate(frame*);
    void push();
    void pop();
    frame* location() const;
#ifndef DEBUG
private:
#endif
    frame root;
    frame* current_frame;
};

#endif
