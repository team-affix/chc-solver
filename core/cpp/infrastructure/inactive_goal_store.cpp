#include "../../hpp/infrastructure/inactive_goal_store.hpp"

inactive_goal_store::inactive_goal_store() {
}

void inactive_goal_store::insert(const goal_lineage* gl) {
    inactive_goals.insert(gl);
}

void inactive_goal_store::erase(const goal_lineage* gl) {
    inactive_goals.erase(gl);
}

void inactive_goal_store::clear() {
    inactive_goals.clear();
}

bool inactive_goal_store::contains(const goal_lineage* gl) {
    return inactive_goals.contains(gl);
}
