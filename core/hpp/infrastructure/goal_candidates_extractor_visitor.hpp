#ifndef GOAL_CANDIDATES_EXTRACTOR_VISITOR_HPP
#define GOAL_CANDIDATES_EXTRACTOR_VISITOR_HPP

#include <unordered_set>
#include "../interfaces/i_goal_candidates_extractor_visitor.hpp"

struct goal_candidates_extractor_visitor : i_goal_candidates_extractor_visitor {
    goal_candidates_extractor_visitor(std::unordered_set<const resolution_lineage*>&);
    void visit(const resolution_lineage*) override;
private:
    std::unordered_set<const resolution_lineage*>& extracted_candidates;
};

#endif
