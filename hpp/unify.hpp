#ifndef UNIFY_HPP
#define UNIFY_HPP

#include <map>
#include <set>
#include <memory>
#include "expr.hpp"
#include "fulfill.hpp"

struct unification_edge {
    const expr* dest;
    fulfillment cause;
};

struct unification_graph {
    unification_graph(trail&);
    std::set<fulfillment> unify(const expr*, const expr*, const fulfillment&);
    std::set<fulfillment> conflict_bfs(const expr*) const;
private:
    trail& trail_ref;
    std::map<const expr*, const std::set<unification_edge>> edges;
    std::map<const expr*, std::shared_ptr<const expr*>> reps;
};

#endif
