#include "../hpp/predicate_index.hpp"
#include "../hpp/expr.hpp"

const std::vector<size_t> predicate_index::empty_candidates;

predicate_index::predicate_index(const database& db) {
    for (size_t i = 0; i < db.size(); ++i) {
        const expr* head = db.at(i).head;
        if (const expr::pred* p = std::get_if<expr::pred>(&head->content))
            index[p->name].push_back(i);
    }
}

const std::vector<size_t>& predicate_index::at(const std::string& name) const {
    auto it = index.find(name);
    if (it == index.end())
        return empty_candidates;
    return it->second;
}
