#include "../hpp/cdcl_eliminator.hpp"

cdcl_eliminator::cdcl_eliminator(cdcl& c, lineage_pool& lp, candidate_store& cs) : c(c), lp(lp), cs(cs) {}

void cdcl_eliminator::pipe() {
    auto& new_eliminated_resolutions = c.new_eliminated_resolutions;
    
    // fill the elimination backlog with the new eliminated resolutions
    while (!new_eliminated_resolutions.empty()) {
        // get the next eliminated resolution
        const resolution_lineage* rl = new_eliminated_resolutions.front();
        new_eliminated_resolutions.pop();
        
        // push the resolution to the elimination backlog
        elimination_backlog[rl->parent].insert(rl->idx);
    }
}
void cdcl_eliminator::execute() {
    // process the elimination backlog according to the goals in the frontier
    for (auto& [gl, backlog] : elimination_backlog) {
        for (size_t idx : backlog)
            eliminated_frontier_resolutions.push(lp.resolution(gl, idx));
        backlog.clear();
    }
}