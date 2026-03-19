#include "../hpp/a02_goal_adder.hpp"

a02_goal_adder::a02_goal_adder(
    a02_goal_store& gs,
    a02_candidate_store& cs,
    a02_goal_rewards& rws,
    const a02_database& db)
    : gs(gs), cs(cs), rws(rws), db(db), a01_ga(gs, cs, db)
{
}

void a02_goal_adder::operator()(const goal_lineage* l, const expr* e, double r)
{
    a01_ga(l, e);
    rws.insert({l, r});
}
