#ifndef UNIT_PROPAGATION_DETECTOR_HPP
#define UNIT_PROPAGATION_DETECTOR_HPP

#include "defs.hpp"

struct unit_propagation_detector {
    unit_propagation_detector(
        const candidate_store&
    );
    bool operator()(const goal_lineage*);
#ifndef DEBUG
private:
#endif
    const candidate_store& cs;
};

#endif
