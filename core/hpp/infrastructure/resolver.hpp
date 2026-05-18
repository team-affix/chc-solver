#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include <optional>
#include "../interfaces/i_resolver.hpp"
#include "../interfaces/i_lineage_pool.hpp"
#include "../interfaces/i_database.hpp"
#include "../interfaces/i_goal_activator.hpp"
#include "../interfaces/i_goal_deactivator.hpp"
#include "../interfaces/i_candidate_activator.hpp"
#include "../interfaces/i_candidate_deactivator.hpp"
#include "../interfaces/i_get_goal_candidates_size.hpp"
#include "../interfaces/i_goal_candidates_acceptor.hpp"

struct resolver : i_resolver {
    explicit resolver(
        i_database& db,
        i_lineage_pool& lp,
        i_goal_activator& goal_activator,
        i_goal_deactivator& goal_deactivator,
        i_candidate_activator& candidate_activator,
        i_candidate_deactivator& candidate_deactivator,
        i_get_goal_candidates_size& gcs,
        i_goal_candidates_acceptor& gca);
    std::optional<sim_termination> resolve(const resolution_lineage*) override;
private:
    i_database& db;
    i_lineage_pool& lp;
    i_goal_activator& goal_activator;
    i_goal_deactivator& goal_deactivator;
    i_candidate_activator& candidate_activator;
    i_candidate_deactivator& candidate_deactivator;
    i_get_goal_candidates_size& gcs;
    i_goal_candidates_acceptor& gca;
};

#endif
