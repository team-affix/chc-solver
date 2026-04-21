#ifndef BIND_MAP_HPP
#define BIND_MAP_HPP

#include <map>
#include <set>
#include "expr.hpp"

struct bind_map {
    bind_map(trail&);
    const expr* whnf(const expr*);
    bool unify(const expr*, const expr*);
    void watch(uint32_t);
    std::set<uint32_t> flush();
#ifndef DEBUG
private:
#endif
    bool occurs_check(uint32_t, const expr*);
    void bind(uint32_t, const expr*);
    std::map<uint32_t, const expr*> bindings;
    std::set<uint32_t> watched_vars;
    std::set<uint32_t> watched_var_updates;
    trail& trail_ref;
};

#endif
