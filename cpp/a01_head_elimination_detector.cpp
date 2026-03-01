#include "../hpp/a01_head_elimination_detector.hpp"

a01_head_elimination_detector::a01_head_elimination_detector(
    trail& t,
    bind_map& bm,
    const a01_goal_store& gs,
    const a01_database& db
)
    : t(t), bm(bm), gs(gs), db(db)
{}

bool a01_head_elimination_detector::operator()(const goal_lineage* gl, size_t i) {
    // push a temporary frame since bindings must be temporary
    t.push();

    // get the goal
    const expr* goal = gs.at(gl);

    // get the candidate
    const rule& candidate = db.at(i);

    // try to unify directly with the head without copying (since it will be rolled back)
    bool unified = bm.unify(goal, candidate.head);

    // pop the temporary frame
    t.pop();

    return !unified; // if not unified, then elimination should occur
    
}
