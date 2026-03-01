#ifndef A01_GOAL_ADDER_HPP
#define A01_GOAL_ADDER_HPP

#include "lineage.hpp"
#include "expr.hpp"
#include "a01_defs.hpp"

struct a01_goal_adder {
    a01_goal_adder(
        a01_goal_store&,
        a01_candidate_store&,
        const a01_database&);
    void operator()(const goal_lineage*, const expr*);
#ifndef DEBUG
private:
#endif
    a01_goal_store& goals;
    a01_candidate_store& candidates;
    const a01_database& database;
};

#endif