#include "../hpp/frontier.hpp"

frontier::frontier(const database& db, lineage_pool& lp) : db(db), lp(lp) {
}

void frontier::add(const goal_lineage* gl) {
    internal_members.insert(gl);
}

void frontier::resolve(const resolution_lineage* rl) {
    // remove the parent goal from the frontier
    internal_members.erase(rl->parent);

    // look up the rule in the db
    size_t body_size = db.at(rl->idx).body.size();

    // add the subgoals to the frontier
    for (size_t i = 0; i < body_size; i++)
        add(lp.goal(rl, i));

}

const std::unordered_set<const goal_lineage*>& frontier::members() const {
    return internal_members;
}
