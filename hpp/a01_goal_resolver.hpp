#ifndef A01_GOAL_RESOLVER_HPP
#define A01_GOAL_RESOLVER_HPP

#include "lineage.hpp"
#include "a01_defs.hpp"
#include "a01_goal_adder.hpp"
#include "bind_map.hpp"
#include "lineage.hpp"

struct a01_goal_resolver {
    a01_goal_resolver(
        a01_resolution_store&,
        a01_goal_store&,
        a01_candidate_store&,
        const a01_database&,
        bind_map&,
        lineage_pool&,
        a01_goal_adder&);
    void operator()(const goal_lineage*, size_t);
#ifndef DEBUG
private:
#endif
    a01_resolution_store& rs;
    a01_goal_store& gs;
    a01_candidate_store& cs;
    const a01_database& db;
    bind_map& bm;
    lineage_pool& lp;
    a01_goal_adder& ga;
};

#endif
