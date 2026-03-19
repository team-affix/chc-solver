#ifndef A02_GOAL_ADDER_HPP
#define A02_GOAL_ADDER_HPP

#include "a02_defs.hpp"
#include "a01_goal_adder.hpp"

struct a02_goal_adder {
    a02_goal_adder(
        a02_goal_store&,
        a02_candidate_store&,
        a02_goal_rewards&,
        const a02_database&
    );
    void operator()(const goal_lineage*, const expr*, double);
#ifndef DEBUG
private:
#endif
    a02_goal_store& gs;
    a02_candidate_store& cs;
    a02_goal_rewards& rws;
    const a02_database& db;
    a01_goal_adder a01_ga;
};

#endif
