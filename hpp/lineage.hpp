#ifndef LINEAGE_HPP
#define LINEAGE_HPP

#include <cstddef>
#include <map>

enum class lineage_type {
    GOAL,
    RESOLUTION,
};

struct lineage {
    const lineage* parent;
    lineage_type type;
    size_t idx;
    auto operator<=>(const lineage&) const = default;
};

struct lineage_pool {
    const lineage* make_lineage(const lineage*, lineage_type, size_t);
    void pin(const lineage*);
    void trim();
    size_t size() const;
#ifndef DEBUG
private:
#endif
    const lineage* intern(lineage&&);
    std::map<lineage, bool> lineages;
};

#endif
