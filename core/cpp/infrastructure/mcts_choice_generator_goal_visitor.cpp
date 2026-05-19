#include "../../hpp/infrastructure/mcts_choice_generator_goal_visitor.hpp"

mcts_choice_generator_goal_visitor::mcts_choice_generator_goal_visitor(std::vector<mcts_choice>& internal_choices)
    : internal_choices(internal_choices) {
}

void mcts_choice_generator_goal_visitor::visit(const goal_lineage* gl) {
    internal_choices.push_back(gl);
}
