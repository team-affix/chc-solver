#include "../hpp/cdcl.hpp"

cdcl::cdcl(trail& t) :
    is_refuted(t, false),
    next_avoidance_id(t),
    avoidances(t, {}),
    watched_goals(t, {}) {

}

void cdcl::learn(const lemma& l) {
    // 1. get a new id for the avoidance
    size_t id = next_avoidance_id();
    
    // 2. add the avoidance to the store
    upsert(id, l.get());

    // 3. get all the goals that are watching this avoidance
    //    and link the avoidance to the goals
    for (const resolution_lineage* rl : l.get()) {
        auto gl = rl->parent;
        if (!watched_goals.get().contains(gl))
            watched_goals.insert(gl, {id});
        else {
            std::set<size_t> ids = watched_goals.get().at(gl);
            ids.insert(id);
            watched_goals.assign(gl, ids);
        }
    }
}

void cdcl::constrain(const resolution_lineage* rl) {
    // 1. get the parent goal
    const goal_lineage* gl = rl->parent;

    // 2. if the goal is not watched, then there is nothing to do
    if (!watched_goals.get().contains(gl))
        return;
    
    // 3. get the set of avoidances that concern this resolution
    std::set<size_t> ids = watched_goals.get().at(gl);

    // 4. for each avoidance, if the avoidance contains the resolution,
    //    reduce the avoidance. Else, remove the avoidance from the store.
    for (size_t id : ids) {

        // 4. get the avoidance
        resolutions rs = avoidances.get().at(id);

        // 5. reduce the avoidance
        size_t erased = rs.erase(rl);

        // 6a. if the resolution was found, then it is consistent
        if (erased > 0)
            upsert(id, rs);

        // 6b. if the resolution was not found, then it is conflicting
        else
            erase(id);
    }

    // 7. Remove the parent goal from the watched goals
    watched_goals.erase(gl);
}

void cdcl::upsert(size_t id, const resolutions& rs) {
    // 1. if the avoidance is empty, we are refuted.
    if (rs.empty() && !is_refuted.get())
        is_refuted.mutate(
            [](bool& is_refuted) { is_refuted = true; },
            [](bool& is_refuted) { is_refuted = false; }
        );

    // 2. update the avoidance in the store
    if (avoidances.get().contains(id))
        avoidances.assign(id, rs);
    else
        avoidances.insert(id, rs);
}

void cdcl::erase(size_t id) {
    // 1. get the avoidance
    const resolutions& rs = avoidances.get().at(id);

    // 2. for each goal that is watching this avoidance,
    //    unlink the avoidance from the goal
    for (const resolution_lineage* rl : rs) {
        const goal_lineage* gl = rl->parent;
        if (!watched_goals.get().contains(gl))
            continue;
        // previoius concerned avoidances
        std::set<size_t> ids = watched_goals.get().at(gl);
        ids.erase(id);
        // update the concerned avoidances
        watched_goals.assign(gl, ids);
    }
    
    // 3. remove the avoidance from the store
    avoidances.erase(id);
}

bool cdcl::refuted() const {
    return is_refuted.get();
}
