#include "../../hpp/infrastructure/mcts_choice_generator_goal_visitor.hpp"

mcts_choice_generator_goal_visitor::mcts_choice_generator_goal_visitor(size_t goal_count) {
    internal_choices.reserve(goal_count);
}

void mcts_choice_generator_goal_visitor::visit(const goal_lineage* gl) {
    internal_choices.push_back(gl);
}

const std::vector<mcts_choice>& mcts_choice_generator_goal_visitor::choices() const {
    return internal_choices;
}
