#ifndef I_CANDIDATE_ELIMINATOR_HPP
#define I_CANDIDATE_ELIMINATOR_HPP

#include "../value_objects/lineage.hpp"

struct i_candidate_eliminator {
    virtual ~i_candidate_eliminator() = default;
    virtual void eliminate(const goal_lineage*, size_t) = 0;
};

#endif
