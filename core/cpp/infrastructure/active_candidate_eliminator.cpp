#include "../../hpp/infrastructure/active_candidate_eliminator.hpp"

active_candidate_eliminator::active_candidate_eliminator(
    i_candidate_deactivator& cd,
    i_get_goal_candidates_size& gcs)
    : cd(cd), gcs(gcs) {
}

elimination_result active_candidate_eliminator::eliminate(const resolution_lineage* rl) {
    // 1. deactivate the candidate
    cd.deactivate(rl);
    // 2. get the goal candidates size
    size_t candidate_count = gcs.size(rl->parent);
    // 3. if the goal candidates size is 0, return eliminated
    if (candidate_count == 0)
        return goal_candidates_empty{rl->parent};
    // 4. if the goal candidates size is 1, return goal made unit
    if (candidate_count == 1)
        return goal_made_unit{rl->parent};
    // 5. return eliminated
    return eliminated{};
}
