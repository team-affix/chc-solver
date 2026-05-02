#ifndef BIND_MAP_HPP
#define BIND_MAP_HPP

#include <unordered_map>
#include "../domain/interfaces/i_bind_map.hpp"
#include "../domain/value_objects/expr.hpp"
#include "../utility/tracked.hpp"
#include <queue>

struct bind_map : i_bind_map {
    bind_map();
    const expr* whnf(const expr*) override;
    bool unify(const expr*, const expr*, std::queue<uint32_t>&) override;
private:
    bool occurs_check(uint32_t, const expr*);
    void bind(uint32_t, const expr*);
    
    tracked<std::unordered_map<uint32_t, const expr*>> bindings;
};

#endif
