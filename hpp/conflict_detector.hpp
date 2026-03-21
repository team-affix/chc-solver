#ifndef CONFLICT_DETECTOR_HPP
#define CONFLICT_DETECTOR_HPP

#include "defs.hpp"

struct conflict_detector {
    conflict_detector(
        const goal_store&,
        const candidate_store&
    );
    bool operator()();
#ifndef DEBUG
private:
#endif
    const goal_store& gs;
    const candidate_store& cs;
};

#endif
