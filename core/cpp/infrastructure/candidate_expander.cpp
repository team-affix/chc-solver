#include "../../hpp/infrastructure/candidate_expander.hpp"

candidate_expander::candidate_expander(
    i_candidate_activator& candidate_activator,
    i_conflict_detector& cd,
    i_unit_goal_detector& ugd,
    i_unit_goals& ug,
    i_lineage_pool& lp,
    i_database& db)
    :
    candidate_activator(candidate_activator),
    cd(cd),
    ugd(ugd),
    ug(ug),
    lp(lp),
    db(db) {
}

bool candidate_expander::expand(const goal_lineage* gl) {
    for (int j = 0; j < db.size(); ++j)
        candidate_activator.try_activate(lp.resolution(gl, j));
    // check for conflicts
    if (cd.detect(gl))
        return false;
    // check for unit goals
    if (ugd.detect(gl))
        ug.push(gl);
    return true;
}
