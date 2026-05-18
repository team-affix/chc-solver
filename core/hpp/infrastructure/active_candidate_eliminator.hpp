#ifndef ACTIVE_CANDIDATE_ELIMINATOR_HPP
#define ACTIVE_CANDIDATE_ELIMINATOR_HPP

#include "../interfaces/i_active_candidate_eliminator.hpp"
#include "../interfaces/i_candidate_deactivator.hpp"
#include "../interfaces/i_get_goal_candidates_size.hpp"

struct active_candidate_eliminator : i_active_candidate_eliminator {
    active_candidate_eliminator(
        i_candidate_deactivator& cd,
        i_get_goal_candidates_size& gcs);
    elimination_result eliminate(const resolution_lineage*) override;
private:
    i_candidate_deactivator& cd;
    i_get_goal_candidates_size& gcs;
};

#endif
