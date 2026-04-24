#ifndef CDCL_ELIMINATOR_HPP
#define CDCL_ELIMINATOR_HPP

#include "cdcl.hpp"
#include "lineage.hpp"
#include "candidate_store.hpp"

struct cdcl_eliminator {
    cdcl_eliminator(cdcl&, lineage_pool&);
    void add_frontier_goal(const goal_lineage*);
    void remove_frontier_goal(const goal_lineage*);
    void pipe();
#ifndef DEBUG
private:
#endif
    cdcl& c;
    lineage_pool& lp;
    candidate_store& cs;

    std::unordered_map<const goal_lineage*, std::unordered_set<size_t>> elimination_backlog;
};

#endif
