#ifndef AVOIDANCE_ADDER_HPP
#define AVOIDANCE_ADDER_HPP

#include "defs.hpp"

struct avoidance_adder {
    avoidance_adder(
        avoidance_store&,
        avoidance_map&
    );
    void operator()(const avoidance&);
#ifndef DEBUG
private:
#endif
    avoidance_store& as;
    avoidance_map& am;
};

#endif
