#include "../hpp/lineage.hpp"

const lineage* lineage_pool::make_lineage(const lineage* parent, lineage_type type, size_t idx) {
    return intern(lineage{parent, type, idx});
}

void lineage_pool::pin(const lineage* l) {
    if (!l)
        return; /*at root*/
    bool& is_pinned = lineages.at(*l);
    if (is_pinned)
        return; /*from here to root is already pinned*/
    is_pinned = true;
    pin(l->parent); /*pin from here to root*/
}

void lineage_pool::trim() {
    for (auto it = lineages.begin(); it != lineages.end();) {
        if (it->second) { ++it; continue; }
        lineages.erase(it++); // remove unpinned lineages
    }
}

size_t lineage_pool::size() const {
    return lineages.size();
}

const lineage* lineage_pool::intern(lineage&& l) {
    return &lineages.emplace(std::move(l), false).first->first;
}
