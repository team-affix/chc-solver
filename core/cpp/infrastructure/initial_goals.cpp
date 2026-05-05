#include "../../hpp/infrastructure/initial_goals.hpp"

initial_goals::initial_goals() :
    initial_goal_count(0) {
}

size_t initial_goals::size() const {
    return initial_goal_count;
}
