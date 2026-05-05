#include "../../hpp/infrastructure/initial_goal_weight_values.hpp"

initial_goal_weight_values::initial_goal_weight_values(const std::vector<double>& weight_values) :
    weight_values(weight_values) {
}

double initial_goal_weight_values::at(size_t index) const {
    return weight_values.at(index);
}
