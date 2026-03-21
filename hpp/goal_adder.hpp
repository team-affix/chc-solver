#ifndef GOAL_ADDER_HPP
#define GOAL_ADDER_HPP

#include "lineage.hpp"
#include "expr.hpp"
#include "defs.hpp"

struct goal_adder {
    goal_adder(
        goal_store&,
        candidate_store&,
        const database&);
    void operator()(const goal_lineage*, const expr*);
#ifndef DEBUG
private:
#endif
    goal_store& goals;
    candidate_store& candidates;
    const database& db;
};

#endif