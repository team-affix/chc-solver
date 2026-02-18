#ifndef UNIFY_HPP
#define UNIFY_HPP

#include <map>
#include <set>
#include "expr.hpp"
#include "cause.hpp"

struct unification_graph {
    struct edge {
        const expr* dst;
        causal_set cause;
    };
    unification_graph(trail&);
    causal_set unify(const expr*, const expr*, const causal_set&);
    std::pair<bool, causal_set> cin_dijkstra(const expr*) const;
private:
    trail& trail_ref;
    std::map<const expr*, std::set<edge>> edges;
};

#endif
