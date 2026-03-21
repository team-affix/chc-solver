#include "../hpp/avoidance_trimmer.hpp"

avoidance_trimmer::avoidance_trimmer(
    avoidance_map& am) : am(am) {
}

void avoidance_trimmer::operator()(const resolution_lineage* rl) {
    // get the parent goal
    const goal_lineage* gl = rl->parent;
    
    // get the range of avoidances concerning the parent goal
    auto concerned_avs = am.equal_range(gl);

    // trim the avoidances concerning the resolution
    for (auto it = concerned_avs.first; it != concerned_avs.second; ++it) {
        if (it->second->contains(rl))
            it->second->erase(rl);
    }

    // remove the goal from the avoidance map
    am.erase(gl);

}
