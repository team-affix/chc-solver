#include "../hpp/resolution.hpp"

const resolution* resolution_pool::make_resolution(const resolution* parent, subgoal_id chosen_subgoal, rule_id chosen_rule) {
    return intern(resolution{parent, chosen_subgoal, chosen_rule});
}

void resolution_pool::pin(const resolution* r) {
    if (!r)
        return; // at root
    bool& is_pinned = resolutions.at(*r);
    if (is_pinned)
        return; // from here to root is already pinned
    is_pinned = true;
    pin(r->parent); // pin from here to root
}

void resolution_pool::trim() {
    for (auto it = resolutions.begin(); it != resolutions.end();) {
        if (it->second) continue;
        resolutions.erase(it++); // remove unpinned resolutions
    }
}

size_t resolution_pool::size() const {
    return resolutions.size();
}

const resolution* resolution_pool::intern(resolution&& r) {
    // auto [it, inserted] = resolutions.insert({std::move(r), false});
    // if (inserted) trail_ref.log([this, it]() { if (!it->second) resolutions.erase(it); });
    // return &it->first;
    return &resolutions.emplace(std::move(r), false).first->first;
}
