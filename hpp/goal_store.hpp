#ifndef GOAL_STORE_HPP
#define GOAL_STORE_HPP

#include "defs.hpp"
#include "copier.hpp"
#include "bind_map.hpp"
#include "lineage.hpp"

struct goal_store {
    goal_store(
        const database&,
        copier&,
        bind_map&,
        lineage_pool&
    );
    void add(const goal_lineage*, const expr*);
    void resolve(const resolution_lineage*);
    bool solved() const;
    const expr* at(const goal_lineage*) const;
#ifndef DEBUG
private:
#endif
    const database& db;
    copier& cp;
    bind_map& bm;
    lineage_pool& lp;

    std::map<const goal_lineage*, const expr*> goals;
};

#endif
