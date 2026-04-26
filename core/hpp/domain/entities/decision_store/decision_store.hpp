#ifndef DECISION_STORE_HPP
#define DECISION_STORE_HPP

#include <set>
#include "../../value_objects/lineage.hpp"

struct decision_store {
    decision_store();
    void add(const resolution_lineage*);
    const std::set<const resolution_lineage*>& get() const;
#ifndef DEBUG
private:
#endif
    std::set<const resolution_lineage*> members;
};

#endif
