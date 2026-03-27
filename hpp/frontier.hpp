#ifndef FRONTIER_HPP
#define FRONTIER_HPP

#include <unordered_set>
#include "defs.hpp"

struct frontier {
    frontier(
        const database&,
        lineage_pool&
    );
    void add(const goal_lineage*);
    void resolve(const resolution_lineage*);
    const std::unordered_set<const goal_lineage*>& members() const;
#ifndef DEBUG
private:
#endif
    const database& db;
    lineage_pool& lp;

    std::unordered_set<const goal_lineage*> internal_members;
};

#endif
