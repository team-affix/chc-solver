#ifndef A02_GOAL_ADDER_HPP
#define A02_GOAL_ADDER_HPP

#include "defs.hpp"
#include "../../a01/hpp/goal_adder.hpp"

struct a02_goal_adder : public a01_goal_adder {
    a02_goal_adder(
        a02_goal_store&,
        a02_candidate_store&,
        a02_goal_rewards&,
        const a02_database&
    );
    void operator()(const goal_lineage*, const expr*, double r);
#ifndef DEBUG
private:
#endif
    a02_goal_rewards& rws;
};

#endif
