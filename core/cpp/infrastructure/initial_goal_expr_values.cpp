#include "../../hpp/infrastructure/initial_goal_expr_values.hpp"

initial_goal_expr_values::initial_goal_expr_values(const std::vector<const expr*>& expr_values) :
    expr_values(expr_values) {
}

const expr* initial_goal_expr_values::at(size_t index) const {
    return expr_values.at(index);
}
