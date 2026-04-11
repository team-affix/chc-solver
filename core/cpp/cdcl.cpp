#include "../hpp/cdcl.hpp"

cdcl::cdcl(trail& t) :
    is_refuted(t, false),
    next_avoidance_id(t),
    avoidances(t, {}),
    watched_goals(t, {}) {

}

void cdcl::learn(const decisions& ds) {
    // 1. reduce the decision store to a set of leaf resolutions
    avoidance av = reduce(ds);

    // 2. get a new id for the avoidance
    size_t id = next_avoidance_id();
    
    // 3. add the avoidance to the store
    upsert(id, av);

    // 4. get all the goals that are watching this avoidance
    //    and link the avoidance to the goals
    for (const resolution_lineage* rl : av) {
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
        avoidance av = avoidances.get().at(id);

        // 5. reduce the avoidance
        size_t erased = av.erase(rl);

        // 6a. if the resolution was found, then it is consistent
        if (erased > 0)
            upsert(id, av);

        // 6b. if the resolution was not found, then it is conflicting
        else
            erase(id);
    }

    // 7. Remove the parent goal from the watched goals
    watched_goals.erase(gl);
}

void cdcl::upsert(size_t id, const avoidance& av) {
    // 1. if the avoidance is empty, we are refuted.
    if (av.empty() && !is_refuted.get())
        is_refuted.mutate(
            [](bool& is_refuted) { is_refuted = true; },
            [](bool& is_refuted) { is_refuted = false; }
        );

    // 2. update the avoidance in the store
    if (avoidances.get().contains(id))
        avoidances.assign(id, av);
    else
        avoidances.insert(id, av);
}

void cdcl::erase(size_t id) {
    // 1. get the avoidance
    const avoidance& av = avoidances.get().at(id);

    // 2. for each goal that is watching this avoidance,
    //    unlink the avoidance from the goal
    for (const resolution_lineage* rl : av) {
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

avoidance cdcl::reduce(const decisions& ds) {
    // 1. create sets of visited lineages
    std::set<const resolution_lineage*> visited;

    // 2. create result avoidance
    avoidance av = ds;

    // 3. iterate over ds, and for each entry, remove all ancestors from av
    for (const resolution_lineage* rl : ds)
        remove_ancestors(rl, av, visited);

    // 4. return the avoidance
    return av;
    
}

void cdcl::remove_ancestors(const resolution_lineage* rl, avoidance& av, std::set<const resolution_lineage*>& visited) {
    while (rl) {
        // 1. get grandparent
        //    (double-dereference safe because resolutions should
        //     never have null parent goals)
        const resolution_lineage* grandparent = rl->parent->parent;
        
        // 2. check grandparent visited
        if (visited.contains(grandparent))
            break;

        // 3. visit grandparent
        visited.insert(grandparent);

        // 4. remove grandparent from av
        av.erase(grandparent);

        // 5. rl = grandparent
        rl = grandparent;

    }
}

bool cdcl::refuted() const {
    return is_refuted.get();
}
