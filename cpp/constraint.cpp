// #include "../hpp/constraint.hpp"

// const constraint_id* constraint_id_pool::fulfillment_child(const constraint_id* parent, rule_id chosen_rule, uint32_t body_index) {
//     constraint_id id{parent, chosen_rule, body_index};
//     return intern(std::move(id));
// }

// size_t constraint_id_pool::size() const {
//     return constraint_ids.size();
// }

// const constraint_id* constraint_id_pool::intern(constraint_id&& id) {
//     return &*constraint_ids.insert(std::move(id)).first;
// }
