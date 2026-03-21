#include "../hpp/avoidance_trimmer.hpp"

avoidance_trimmer::avoidance_trimmer(
    avoidance_store& as,
    avoidance_map& am) : as(as), am(am) {
}

void avoidance_trimmer::operator()(const resolution_lineage* rl) {
    const goal_lineage* gl = rl->parent;
    
    // get the range of avoidances concerning the parent goal
    auto rng = am.equal_range(gl);

    for (auto it = rng.first; it != rng.second; ++it) {
        // get the iterator to the avoidance
        auto av_it = it->second;

        // check if the avoidance contains the resolution
        if (av_it->contains(rl))
            // erase the resolution from the avoidance if it is present
            av_it->erase(rl);
        else
            // otherwise, erase the avoidance
            as.erase(av_it);
    }

    // this goal is resolved, thus its concerned avoidances are no longer needed
    am.erase(gl);
}
