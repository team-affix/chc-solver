#ifndef UNIFY_HPP
#define UNIFY_HPP

#include <map>
#include <set>
#include "expr.hpp"
#include "fulfill.hpp"

struct unification_edge {
    const expr* dest;
    fulfillment cause;
};

struct unification_graph {
    unification_graph(trail&);
    bool unify(const expr*, const expr*);
private:
    trail& trail_ref;
    std::map<const expr*, const std::set<unification_edge>> edges;
};

#endif
