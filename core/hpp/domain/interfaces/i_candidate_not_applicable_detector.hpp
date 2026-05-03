#ifndef I_CANDIDATE_NOT_APPLICABLE_DETECTOR_HPP
#define I_CANDIDATE_NOT_APPLICABLE_DETECTOR_HPP

#include "../value_objects/lineage.hpp"

struct i_candidate_not_applicable_detector {
    virtual ~i_candidate_not_applicable_detector() = default;
    virtual void goal_expr_changed(const goal_lineage*) = 0;
};

#endif
