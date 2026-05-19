#include "../../hpp/infrastructure/mcts_choice_generator_candidate_visitor.hpp"

mcts_choice_generator_candidate_visitor::mcts_choice_generator_candidate_visitor(std::vector<mcts_choice>& internal_choices)
    : internal_choices(internal_choices) {
}

void mcts_choice_generator_candidate_visitor::visit(const rule* r) {
    internal_choices.push_back(r);
}
