#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include "trail.hpp"
#include "tracked.hpp"

struct sequencer {
    sequencer(trail&);
    size_t operator()();
#ifndef DEBUG
private:
#endif
    tracked<size_t> index;
};

#endif
