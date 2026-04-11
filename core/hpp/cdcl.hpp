#ifndef AVOIDANCE_HPP
#define AVOIDANCE_HPP

#include "trail.hpp"
#include "lineage.hpp"
#include "defs.hpp"
#include "delta_map.hpp"
#include "sequencer.hpp"
#include "lemma.hpp"

struct cdcl {
    cdcl(trail&);
    void learn(const lemma&);
    void constrain(const resolution_lineage*);
    bool refuted() const;
    #ifndef DEBUG
    private:
    #endif
    void upsert(size_t, const resolutions&);
    void erase(size_t);
    
    tracked<bool> is_refuted;
    sequencer next_avoidance_id;
    delta<std::map<size_t, resolutions>> avoidances;
    delta<std::map<const goal_lineage*, std::set<size_t>>> watched_goals;
};

#endif
