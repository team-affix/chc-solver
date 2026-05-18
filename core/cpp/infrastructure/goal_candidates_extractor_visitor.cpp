#include "../../hpp/infrastructure/goal_candidates_extractor_visitor.hpp"

goal_candidates_extractor_visitor::goal_candidates_extractor_visitor(std::unordered_set<const resolution_lineage*>& extracted_candidates)
    : extracted_candidates(extracted_candidates) {}

void goal_candidates_extractor_visitor::visit(const resolution_lineage* rl) {
    extracted_candidates.insert(rl);
}
