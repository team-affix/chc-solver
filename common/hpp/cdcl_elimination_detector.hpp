#ifndef CDCL_ELIMINATION_DETECTOR_HPP
#define CDCL_ELIMINATION_DETECTOR_HPP

#include "defs.hpp"
#include "lineage.hpp"

struct cdcl_elimination_detector {
    cdcl_elimination_detector(
        const avoidance_store&,
        lineage_pool&
    );
    bool operator()(const goal_lineage*, size_t);
#ifndef DEBUG
private:
#endif
    const avoidance_store& as;
    lineage_pool& lp;
};

#endif
