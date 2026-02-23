// #ifndef CONSTRAINT_STORE_HPP
// #define CONSTRAINT_STORE_HPP

// #include "trail.hpp"
// #include "bind_map.hpp"
// #include "resolution.hpp"
// #include "expr.hpp"

// struct constraint_store {
//     constraint_store(trail&, bind_map&);
//     void insert_family(const resolution*);
//     void insert_constraint(const resolution*, subgoal_id, const expr*);
//     bool resolve(const resolution*, const rule&);
// #ifndef DEBUG
// private:
// #endif
//     void erase_family(const resolution*);
//     void erase_constraint(const resolution*, subgoal_id);
//     trail& trail_ref;
//     bind_map& bind_map_ref;
//     std::map<const resolution*, std::map<subgoal_id, const expr*>> constraints;
// };

// #endif
