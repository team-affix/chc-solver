#include <stdexcept>
#include "../hpp/a01_goal_resolver.hpp"

a01_goal_resolver::a01_goal_resolver(
    a01_resolution_store& r,
    a01_goal_store& g,
    a01_candidate_store& c,
    const a01_database& d,
    bind_map& b,
    lineage_pool& l,
    a01_goal_adder& a)
    : rs(r), gs(g), cs(c), db(d), bm(b), lp(l), ga(a)
{
}

void a01_goal_resolver::operator()(const goal_lineage* gl, size_t i)
{
    // get the goal expression
    const expr* goal_expr = gs.at(gl);

    // remove the goal from the goals store
    gs.erase(gl);

    // remove the candidate entries from the candidates store
    cs.erase(gl);

    // construct the resolution lineage
    const resolution_lineage* rl = lp.resolution(gl, i);

    // add the resolution to the resolutions store
    rs.insert(rl);

    // get the rule in the db
    const rule& r = db.at(i);

    // unify the head with the goal
    if (!bm.unify(r.head, goal_expr))
        throw std::runtime_error("Failed to unify the head with the goal");

    // add the body expressions to the goal store
    for (size_t j = 0; j < r.body.size(); ++j) {
        const expr* e = r.body[j];
        const goal_lineage* child_gl = lp.goal(rl, j);
        ga(child_gl, e);
    }
}
    