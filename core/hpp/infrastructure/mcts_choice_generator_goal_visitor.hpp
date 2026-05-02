#ifndef MCTS_CHOICE_GENERATOR_GOAL_VISITOR_HPP
#define MCTS_CHOICE_GENERATOR_GOAL_VISITOR_HPP

#include <vector>
#include "../domain/interfaces/i_goal_visitor.hpp"
#include "mcts_choice.hpp"

struct mcts_choice_generator_goal_visitor : i_goal_visitor {
    mcts_choice_generator_goal_visitor(size_t);
    void visit(const goal_lineage*) override;
    const std::vector<mcts_choice>& choices() const;
private:
    std::vector<mcts_choice> internal_choices;
};

#endif
