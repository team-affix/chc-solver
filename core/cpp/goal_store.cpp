#include "../hpp/goal_store.hpp"

goal_store::goal_store(
    const database& db,
    const goals& goals,
    trail& t,
    copier& cp,
    bind_map& bm,
    lineage_pool& lp)
    :
    frontier<const expr*, goal_expander>(db, lp),
    t(t),
    cp(cp),
    bm(bm),
    lp(lp)
{
    // add the goals to the frontier
    for (int i = 0; i < goals.size(); ++i)
        insert(lp.goal(nullptr, i), goals.at(i));
}

bool goal_store::applicable(const expr* const& e, const rule& r) {
    // push a temporary frame since bindings must be temporary
    t.push();

    // try to unify the head with the goal
    bool result = bm.unify(e, r.head);

    // pop the temporary frame
    t.pop();

    // return the result
    return result;
}

goal_expander goal_store::make_expander(const expr* const& e, const rule& r) {
    return goal_expander(e, r, cp, bm);
}
