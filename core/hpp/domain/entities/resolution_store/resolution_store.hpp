#ifndef RESOLUTION_STORE_HPP
#define RESOLUTION_STORE_HPP

#include <unordered_set>
#include "../../../infrastructure/tracked_set.hpp"
#include "../../value_objects/lineage.hpp"

struct resolution_store {
    resolution_store();
    void insert(const resolution_lineage*);
    const std::unordered_set<const resolution_lineage*>& get() const;
#ifndef DEBUG
private:
#endif
    tracked_set<std::unordered_set<const resolution_lineage*>> resolutions;
};

#endif
