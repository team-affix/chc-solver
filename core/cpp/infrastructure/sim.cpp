#include "../../hpp/infrastructure/sim.hpp"

sim::sim(
    size_t max_resolutions,
    i_unit_goals& ug,
    i_decision_generator& dg,
    i_elimination_generator& eg,
    i_elimination_router& er,
    i_resolver& r)
    :
    max_resolutions(max_resolutions),
    ug(ug),
    dg(dg),
    eg(eg),
    er(er),
    r(r) {
}

sim_termination sim::run() {
    for (size_t i = 0; i < max_resolutions; ++i) {
        // 1. get the next resolution
        const resolution_lineage* rl = next_resolution();
        // 2. generate eliminations from this
        auto eliminations = eg.constrain(rl);
        // 3. route the eliminations
        while (!eliminations.done()) {
            auto res = eliminations.resume();
            if (!res.has_value())
                continue;
            auto elim_result = er.route(res.value());
            if (handle_elimination_result(elim_result))
                return sim_termination::conflicted;
        }
        // 4. resolve given rl
        r.resolve(rl);
    }
}

const resolution_lineage* sim::next_resolution() {
    if (!ug.empty()) {
        // 1. get the next unit goal
        const goal_lineage* gl = ug.pop();
        // 2. determine which candidate it still has
        
    }
}

bool sim::handle_elimination_result(const elimination_result& result) {
    if (const auto* unit = std::get_if<goal_made_unit>(&result))
        ug.push(unit->gl);
    else if (std::holds_alternative<goal_candidates_empty>(result))
        return true;
    return false;
}
