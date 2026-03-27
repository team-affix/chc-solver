#ifndef HEAD_ELIMINATION_DETECTOR_HPP
#define HEAD_ELIMINATION_DETECTOR_HPP

#include "bind_map.hpp"
#include "lineage.hpp"
#include "defs.hpp"
#include "goal_store.hpp"

struct head_elimination_detector {
    head_elimination_detector(
        trail&,
        bind_map&,
        const goal_store&,
        const database&
    );
    bool operator()(const goal_lineage*, size_t);
#ifdef DEBUG
private:
#endif
    trail& t;
    bind_map& bm;
    const goal_store& gs;
    const database& db;
};

#endif
