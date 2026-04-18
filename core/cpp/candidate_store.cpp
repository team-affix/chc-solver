#include "../hpp/candidate_store.hpp"

// Return the predicate name of an expression if it is a pred, else nullopt.
static const std::string* pred_name(const expr* e) {
    if (const expr::pred* p = std::get_if<expr::pred>(&e->content))
        return &p->name;
    return nullptr;
}

candidate_store::candidate_store(
    const database& db,
    const goals& goals,
    lineage_pool& lp) :
    frontier<std::vector<size_t>>(db, lp),
    db(db),
    lp(lp),
    pi(db)
{
    for (int i = 0; i < goals.size(); ++i) {
        const std::string* name = pred_name(goals.at(i));
        const std::vector<size_t>& candidates =
            name ? pi.at(*name) : predicate_index::empty_candidates;
        insert(lp.goal(nullptr, i), candidates);
    }
}

size_t candidate_store::eliminate(const std::function<bool(const goal_lineage*, size_t)>& pred) {
    size_t result = 0;
    for (auto it = begin(); it != end(); ++it) {
        const goal_lineage* gl = it->first;
        std::vector<size_t>& candidates = it->second;
        for (size_t i = 0; i < candidates.size();) {
            if (pred(gl, candidates[i])) {
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
    for (auto it = begin(); it != end(); ++it) {
        const goal_lineage* key = it->first;
        const std::vector<size_t>& candidates = it->second;
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
       begin(),
        end(),
        [](const auto& e) { return e.second.size() == 0; });
}

std::vector<std::vector<size_t>> candidate_store::expand(const std::vector<size_t>&, const rule& r) {
    std::vector<std::vector<size_t>> result;
    for (const expr* body_lit : r.body) {
        const std::string* name = pred_name(body_lit);
        result.push_back(name ? pi.at(*name) : predicate_index::empty_candidates);
    }
    return result;
}
