#include "../hpp/goal_adder.hpp"

goal_adder::goal_adder(
    goal_store& g,
    candidate_store& c,
    const database& d)
    : goals(g), candidates(c), db(d)
{
}

void goal_adder::operator()(const goal_lineage* l, const expr* e)
{
    goals.insert({l, e});

    // add EVERY rule in the database as a candidate for the goal (trivial)
    for (size_t i = 0; i < db.size(); ++i)
        candidates.insert({l, i});
}
