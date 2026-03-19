#include "../hpp/goal_adder.hpp"

a02_goal_adder::a02_goal_adder(
    a02_goal_store& gs,
    a02_candidate_store& cs,
    a02_goal_rewards& rws,
    const a02_database& db)
    : a01_goal_adder(gs, cs, db), rws(rws)
{
}

void a02_goal_adder::operator()(const goal_lineage* l, const expr* e, double r)
{
    a01_goal_adder::operator()(l, e);
    rws.insert({l, r});
}
