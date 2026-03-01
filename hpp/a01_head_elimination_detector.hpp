#ifndef A01_HEAD_ELIMINATION_DETECTOR_HPP
#define A01_HEAD_ELIMINATION_DETECTOR_HPP

#include "bind_map.hpp"
#include "lineage.hpp"
#include "a01_defs.hpp"

struct a01_head_elimination_detector {
    a01_head_elimination_detector(
        trail&,
        bind_map&,
        const a01_goal_store&,
        const a01_database&
    );
    bool operator()(const goal_lineage*, size_t);
#ifdef DEBUG
private:
#endif
    trail& t;
    bind_map& bm;
    const a01_goal_store& gs;
    const a01_database& db;
};

#endif
