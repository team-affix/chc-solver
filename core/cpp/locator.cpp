#include "../hpp/locator.hpp"

void locator::unbind(locator_keys key) {
    entries.erase(key);
}

void locator::push_frame() {
    past_frames.push(current_frame_additions);
    current_frame_additions.clear();
}

void locator::pop_frame() {
    for (auto key : current_frame_additions)
        unbind(key);
    current_frame_additions = past_frames.top();
    past_frames.pop();
}