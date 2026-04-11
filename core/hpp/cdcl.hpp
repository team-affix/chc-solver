#ifndef AVOIDANCE_HPP
#define AVOIDANCE_HPP

#include "trail.hpp"
#include "lineage.hpp"
#include "defs.hpp"
#include "delta_map.hpp"
#include "sequencer.hpp"

using avoidance = std::unordered_set<const resolution_lineage*>;

struct cdcl {
    cdcl(trail&);
    void learn(const decisions&);
    void constrain(const resolution_lineage*);
    bool refuted() const;
    #ifndef DEBUG
    private:
    #endif
    void upsert(size_t, const avoidance&);
    void erase(size_t);
    static avoidance reduce(const decisions&);
    static void remove_ancestors(const resolution_lineage*, avoidance&, std::set<const resolution_lineage*>&);

    tracked<bool> is_refuted;
    sequencer next_avoidance_id;
    delta<std::map<size_t, avoidance>> avoidances;
    delta<std::map<const goal_lineage*, std::set<size_t>>> watched_goals;
};

#endif
