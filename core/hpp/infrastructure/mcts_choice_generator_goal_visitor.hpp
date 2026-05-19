#ifndef MCTS_CHOICE_GENERATOR_GOAL_VISITOR_HPP
#define MCTS_CHOICE_GENERATOR_GOAL_VISITOR_HPP

#include <vector>
#include "../interfaces/i_visitor.hpp"
#include "../value_objects/lineage.hpp"
#include "../value_objects/mcts_choice.hpp"

struct mcts_choice_generator_goal_visitor : i_visitor<const goal_lineage*> {
    mcts_choice_generator_goal_visitor(std::vector<mcts_choice>& internal_choices);
    void visit(const goal_lineage*) override;
private:
    std::vector<mcts_choice>& internal_choices;
};

#endif
