#include "../hpp/trail.hpp"

void frame::revert() const {
    for (
        auto it = actions.rbegin();
        it != actions.rend();
        ++it)
        it->undo();
}

void frame::replay() const {
    for (
        auto it = actions.begin();
        it != actions.end();
        ++it)
        it->redo();
}

void trail::log(action a) {
    current_frame->actions.emplace_back(a);
}

void trail::translate(frame* target) {
    std::list<frame*> path;
    
    // current frame is deeper than target
    while (current_frame->depth > target->depth) {
        current_frame->revert();
        current_frame = current_frame->parent;
    }

    // target is deeper than current frame
    while (target->depth > current_frame->depth) {
        path.push_front(target);
        target = target->parent;
    }
    
    // equal depth phase
    while (current_frame != target) {
        current_frame->revert();
        current_frame = current_frame->parent;
        path.push_front(current_frame);
        target = target->parent;
    }

    // apply path
    for (frame* f : path)
        f->replay();

    current_frame = path.back();
}

void trail::push() {
    frame* c = current_frame;
    auto new_frame = std::make_unique<frame>(frame(
        c, {}, c->depth + 1, {}));
    auto [it, _] = c->children.emplace(
        new_frame.get(), std::move(new_frame));
    current_frame = it->second.get();
}

void trail::pop() {
    auto f = current_frame;
    f->revert();
    current_frame = current_frame->parent;
    current_frame->children.erase(f);
}

frame* trail::location() const {
    return current_frame;
}
