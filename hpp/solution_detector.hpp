#ifndef SOLUTION_DETECTOR_HPP
#define SOLUTION_DETECTOR_HPP

#include "defs.hpp"

struct solution_detector {
    solution_detector(
        const goal_store&
    );
    bool operator()();
#ifndef DEBUG
private:
#endif
    const goal_store& gs;
};

#endif
