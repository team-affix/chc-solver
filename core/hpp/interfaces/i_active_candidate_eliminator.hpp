#ifndef I_ACTIVE_CANDIDATE_ELIMINATOR_HPP
#define I_ACTIVE_CANDIDATE_ELIMINATOR_HPP

#include "../value_objects/lineage.hpp"
#include "../value_objects/elimination_result.hpp"

struct i_active_candidate_eliminator {
    virtual ~i_active_candidate_eliminator() = default;
    virtual elimination_result eliminate(const resolution_lineage*) = 0;
};

#endif
