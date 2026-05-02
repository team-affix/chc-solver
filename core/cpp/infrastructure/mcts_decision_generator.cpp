#include "../../hpp/infrastructure/mcts_decision_generator.hpp"
#include "../../hpp/bootstrap/resolver.hpp"

mcts_decision_generator::mcts_decision_generator()
    :
    lp(resolver::resolve<i_lineage_pool>()),
    ags(resolver::resolve<i_active_goal_store>()),
    gcs(resolver::resolve<i_goal_candidates_store>()),
    sim(resolver::resolve<monte_carlo::simulation<choice, std::mt19937>>()){
}

const resolution_lineage* mcts_decision_generator::generate() {
    const goal_lineage* chosen_gl = choose_goal();
    const size_t chosen_i = choose_candidate(chosen_gl);
    return lp.resolution(chosen_gl, chosen_i);
}

const goal_lineage* mcts_decision_generator::choose_goal() {
    // Get the goals to choose from
    std::vector<choice> goals;
    goals.reserve(ags.size());

    // Convert the goals to choices
    for (const goal_lineage* gl : ags.all_as_vector())
        goals.push_back(gl);

    // Choose a goal to resolve
    const choice choice_a = sim.choose(goals);
    return std::get<const goal_lineage*>(choice_a);
}

size_t mcts_decision_generator::choose_candidate(const goal_lineage* goal) {
    // Get the candidates to choose from
    std::vector<choice> candidates;
    candidates.reserve(cs.get().at(chosen_gl).size());

    // Convert the candidates to choices
    for (size_t rule_id : cs.get().at(chosen_gl))
        candidates.push_back(rule_id);

    // Choose a candidate for the goal
    const choice choice_b = sim.choose(candidates);
    return std::get<size_t>(choice_b);
}
