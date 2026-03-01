#include "../hpp/a01_goal_adder.hpp"

a01_goal_adder::a01_goal_adder(
    a01_goal_store& g,
    a01_candidate_store& c,
    const a01_database& d)
    : goals(g), candidates(c), database(d)
{
}

void a01_goal_adder::operator()(const goal_lineage* l, const expr* e)
{
    goals.insert({l, e});

    // add EVERY rule in the database as a candidate for the goal (trivial)
    for (size_t i = 0; i < database.size(); ++i)
        candidates.insert({l, i});
}
