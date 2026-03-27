#include <stdexcept>
#include "../hpp/goal_store.hpp"

goal_store::goal_store(
    const database& db,
    copier& cp,
    bind_map& bm,
    lineage_pool& lp)
    : db(db), cp(cp), bm(bm), lp(lp), goals({})
{
}

void goal_store::add(const goal_lineage* gl, const expr* e) {
    goals.insert({gl, e});
}

void goal_store::resolve(const resolution_lineage* rl) {
    // get the goal lineage
    const goal_lineage* gl = rl->parent;

    // get the goal index
    size_t i = rl->idx;
    
    // get the goal expression
    const expr* goal_expr = goals.at(gl);

    // remove the goal from the goals store
    goals.erase(gl);

    // get the rule in the db
    const rule& r = db.at(i);

    // create the translation map for copying the rule
    std::map<uint32_t, uint32_t> translation_map;

    // copy the head of the rule
    const expr* copied_head = cp(r.head, translation_map);

    // copy the body of the rule
    std::vector<const expr*> copied_body;
    for (const expr* e : r.body)
        copied_body.push_back(cp(e, translation_map));

    // unify the head with the goal
    if (!bm.unify(copied_head, goal_expr))
        throw std::runtime_error("Failed to unify the head with the goal");

    // add the body expressions to the goal store
    for (size_t j = 0; j < copied_body.size(); ++j) {
        const expr* e = copied_body[j];
        const goal_lineage* child_gl = lp.goal(rl, j);
        add(child_gl, e);
    }
}

bool goal_store::solved() const {
    return goals.empty();
}

const expr* goal_store::at(const goal_lineage* gl) const {
    return goals.at(gl);
}
