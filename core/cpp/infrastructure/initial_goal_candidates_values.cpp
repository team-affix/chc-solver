#include "../../hpp/infrastructure/initial_goal_candidates_values.hpp"

initial_goal_candidates_values::initial_goal_candidates_values(const std::vector<candidate_set>& candidate_sets) :
    candidate_sets(candidate_sets) {
}

const candidate_set& initial_goal_candidates_values::at(size_t index) const {
    return candidate_sets.at(index);
}
