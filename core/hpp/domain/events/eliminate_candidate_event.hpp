#ifndef ELIMINATE_CANDIDATE_HPP
#define ELIMINATE_CANDIDATE_HPP

#include "../value_objects/lineage.hpp"

struct eliminate_candidate_event {
    const resolution_lineage* rl;
};

#endif
