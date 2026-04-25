#ifndef FRONTIER_WATCH_HPP
#define FRONTIER_WATCH_HPP

#include "defs.hpp"
#include "lineage.hpp"

struct frontier_watch {
    frontier_watch(
        const database&,
        lineage_pool&,
        std::function<void(const goal_lineage*)>,
        std::function<void(const resolution_lineage*)>
    );
    void initialize(const goals&);
    void insert(const goal_lineage*);
    void resolve(const resolution_lineage*);
#ifndef DEBUG
private:
#endif
    const database& db;
    lineage_pool& lp;

    std::unordered_set<const goal_lineage*> current_goals;
    std::function<void(const goal_lineage*)> insert_callback;
    std::function<void(const resolution_lineage*)> resolve_callback;
};

#endif