#ifndef CANDIDATE_STORE_HPP
#define CANDIDATE_STORE_HPP

#include "lineage.hpp"
#include "defs.hpp"

struct candidate_store {
    candidate_store(
        const database&,
        lineage_pool&
    );
    void add(const goal_lineage*);
    void resolve(const resolution_lineage*);
    size_t eliminate(const std::function<bool(const goal_lineage*, size_t)>&);
    bool unit(const goal_lineage*&, size_t&) const;
    bool conflicted() const;
    const std::vector<size_t>& at(const goal_lineage*) const;
#ifndef DEBUG
private:
#endif
    const database& db;
    lineage_pool& lp;

    std::vector<size_t> initial_candidates;
    std::map<const goal_lineage*, std::vector<size_t>> candidates;
};

#endif
