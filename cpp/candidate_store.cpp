#include "../hpp/candidate_store.hpp"
    
candidate_store::candidate_store(
    const database& db,
    lineage_pool& lp)
    : db(db), lp(lp), initial_candidates(db.size()), candidates({})
{
    for (size_t i = 0; i < db.size(); ++i)
        initial_candidates[i] = i;
}

void candidate_store::add(const goal_lineage* gl) {
    // add EVERY rule in the database as a candidate for the goal (trivial)
    candidates[gl] = initial_candidates;
}

void candidate_store::resolve(const resolution_lineage* rl) {
    candidates.erase(rl->parent);
    const rule& r = db.at(rl->idx);
    for (int j = 0; j < r.body.size(); ++j)
        add(lp.goal(rl, j));
}

size_t candidate_store::eliminate(const std::function<bool(const goal_lineage*, size_t)>& f) {
    size_t result = 0;
    for (auto& [gl, candidates] : candidates) {
        for (size_t i = 0; i < candidates.size();) {
            if (f(gl, candidates[i])) {
                candidates[i] = candidates.back();
                candidates.pop_back();
                ++result;
            }
            else {
                ++i;
            }
        }
    }
    return result;
}

bool candidate_store::unit(const goal_lineage*& gl, size_t& candidate) const {
    for (const auto& [key, candidates] : candidates) {
        if (candidates.size() == 1) {
            gl = key;
            candidate = candidates.front();
            return true;
        }
    }
    return false;
}

bool candidate_store::conflicted() const {
    return std::any_of(
       candidates.begin(),
        candidates.end(),
        [](const auto& e) { return e.second.size() == 0; });
}

const std::vector<size_t>& candidate_store::at(const goal_lineage* gl) const {
    return candidates.at(gl);
}
