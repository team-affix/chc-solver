// #include "../hpp/constraint_store.hpp"

// constraint_store::constraint_store(trail& t, bind_map& bm) : trail_ref(t), bind_map_ref(bm) {
    
// }

// void constraint_store::insert_family(const resolution* parent) {
//     trail_ref.log([this, parent]{constraints.erase(parent);});
//     constraints[parent] = {};
// }

// void constraint_store::insert_constraint(const resolution* parent, subgoal_id subgoal, const expr* e) {
//     trail_ref.log([this, parent, subgoal]{constraints.at(parent).erase(subgoal);});
//     constraints.at(parent)[subgoal] = e;
// }

// void constraint_store::erase_constraint(const resolution* parent, subgoal_id subgoal) {
//     const expr* e = constraints.at(parent).at(subgoal);
//     trail_ref.log([this, parent, subgoal, e]{constraints.at(parent)[subgoal] = e;});
//     constraints.at(parent).erase(subgoal);
// }

// void constraint_store::erase_family(const resolution* parent) {

// }

// bool constraint_store::resolve(const resolution* new_resolution, const rule& rl) {
//     // 1. Get the mentioned constraint
//     const expr* e = constraints.at(new_resolution->parent).at(new_resolution->chosen_subgoal);

//     // 2. Unify the constraint with the rule head
//     if (!bind_map_ref.unify(e, rl.head))
//         return false;

//     // 3. Remove the constraint from the constraint store
//     erase(new_resolution->parent, new_resolution->chosen_subgoal);

//     // 4. Add the new constraints to the constraint store
//     insert_family(new_resolution, rl.body);
    
// }
