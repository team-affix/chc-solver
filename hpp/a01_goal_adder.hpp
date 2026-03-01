#ifndef A01_GOAL_ADDER_HPP
#define A01_GOAL_ADDER_HPP

#include "lineage.hpp"
#include "expr.hpp"
#include "rule.hpp"

struct a01_goal_adder {
    a01_goal_adder(
        std::map<const goal_lineage*, const expr*>&,
        std::multimap<const goal_lineage*, size_t>&,
        const std::vector<const rule*>&);
    void operator()(const goal_lineage*, const expr*);
#ifndef DEBUG
private:
#endif
    std::map<const goal_lineage*, const expr*>& goals;
    std::multimap<const goal_lineage*, size_t>& candidates;
    const std::vector<const rule*>& database;
};

#endif