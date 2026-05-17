#include "../../hpp/infrastructure/frontier.hpp"

void frontier::insert(key_type gl, value_type g) {
    goals_.emplace(gl, std::move(g));
}

bool frontier::contains(key_type gl) const {
    return goals_.count(gl) > 0;
}

std::unordered_map<size_t, std::unique_ptr<candidate>>& frontier::at(const goal_lineage* gl) {
    return goals_.at(gl);
}

const std::unordered_map<size_t, std::unique_ptr<candidate>>& frontier::at(const goal_lineage* gl) const {
    return goals_.at(gl);
}

void frontier::erase(const goal_lineage* gl) {
    goals_.erase(gl);
}

void frontier::clear() {
    goals_.clear();
}

size_t frontier::size() const {
    return goals_.size();
}

void frontier::accept(i_visitor<std::pair<key_type, value_type&>>& v) {
    for (auto& entry : goals_)
    v.visit(
        std::pair<key_type, value_type&>{
            entry.first, entry.second});
}
