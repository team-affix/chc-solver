#ifndef ELIMINATION_RESULT_HPP
#define ELIMINATION_RESULT_HPP

#include <variant>
#include "../value_objects/lineage.hpp"

struct eliminated {};
struct goal_candidates_empty {const goal_lineage* gl;};
struct goal_made_unit {const goal_lineage* gl;};
struct already_deactivated {};
struct added_to_backlog {};

using elimination_result =
    std::variant<
        eliminated,
        goal_candidates_empty,
        goal_made_unit,
        already_deactivated,
        added_to_backlog>;

#endif
