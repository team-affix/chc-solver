#include "../../../hpp/domain/entities/cdcl.hpp"
#include "../../../hpp/bootstrap/resolver.hpp"

cdcl::cdcl() :
    avoidance_is_unit_producer(resolver::resolve<i_event_producer<avoidance_is_unit_event>>()),
    avoidance_is_empty_producer(resolver::resolve<i_event_producer<avoidance_is_empty_event>>()),
    next_avoidance_id(resolver::resolve<i_cdcl_sequencer>()),
    avoidances(resolver::resolve<i_trail>(), {}),
    watched_goals(resolver::resolve<i_trail>(), {}),
    unit_avoidances(resolver::resolve<i_trail>(), {}),
    empty_avoidances(resolver::resolve<i_trail>(), {}) {
}

void cdcl::learn(const lemma& l) {
    // 1. copy the already-trimmed resolutions into a local avoidance
    avoidance av{l.get_resolutions()};

    // 2. get a new id for the avoidance
    size_t id = next_avoidance_id.next();

    // 3. add the avoidance to the store
    upsert(id, av);

    // 4. get all the goals that are watching this avoidance
    //    and link the avoidance to the goals
    for (const resolution_lineage* rl : av.decisions) {
        
        watched_goals[rl->parent].insert(id);
    }

}

// void cdcl::constrain(const resolution_lineage* rl) {
//     // 1. get the parent goal
//     const goal_lineage* gl = rl->parent;
    
//     // 2. get the set of avoidances that concern this resolution
//     std::set<size_t> ids = watched_goals[gl];

//     // 3. for each avoidance, if the avoidance contains the resolution,
//     //    reduce the avoidance. Else, remove the avoidance from the store.
//     for (size_t id : ids) {

//         // 4. get the avoidance
//         avoidance av = avoidances.at(id);

//         // 5. reduce the avoidance
//         size_t erased = av.erase(rl);

//         // 6a. if the resolution was found, then it is consistent
//         if (erased > 0)
//             upsert(id, av);

//         // 6b. if the resolution was not found, then it is conflicting
//         else
//             erase(id);
//     }

//     // 7. Remove the parent goal from the watched goals
//     watched_goals.erase(gl);
// }

// void cdcl::emit_eliminated_candidates() {
//     for (const resolution_lineage* rl : eliminated_resolutions)
//         cdcl_eliminated_candidate_topic.produce(cdcl_eliminated_candidate_event{rl});
// }

// bool cdcl::check_for_conflict() {
//     bool result = avoidances.contains({});
    
//     if (result)
//         conflicted_topic.produce(conflicted_event{});

//     return result;
// }

// void cdcl::upsert(size_t id, const avoidance& av) {
//     // 1. update the avoidance in the store
//     avoidances[id] = av;

//     // 2. if the avoidance is singleton, we have eliminated something.
//     if (av.size() == 1) {
//         auto [_, inserted] = eliminated_resolutions.insert(*av.begin());

//         // 3. if the elimination was new, call the callback
//         if (inserted)
//             cdcl_eliminated_candidate_topic.produce(cdcl_eliminated_candidate_event{*av.begin()});
//     }
// }

// void cdcl::erase(size_t id) {
//     // 1. get the avoidance
//     const avoidance& av = avoidances.at(id);

//     // 2. for each goal that is watching this avoidance,
//     //    unlink the avoidance from the goal
//     for (const resolution_lineage* rl : av)
//         watched_goals[rl->parent].erase(id);
    
//     // 3. remove the avoidance from the store
//     avoidances.erase(id);
// }
