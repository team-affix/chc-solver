#ifndef CONFLICT_DETECTOR_HPP
#define CONFLICT_DETECTOR_HPP

#include "a01_defs.hpp"

struct conflict_detector {
    conflict_detector(
        const a01_goal_store&,
        const a01_candidate_store&
    );
    bool operator()();
#ifndef DEBUG
private:
#endif
    const a01_goal_store& gs;
    const a01_candidate_store& cs;
};

#endif
