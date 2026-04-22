#ifndef BIND_WATCH_HPP
#define BIND_WATCH_HPP

#include <cstdint>
#include <map>
#include <unordered_set>
#include "lineage.hpp"

struct bind_watch {
    void operator()(const goal_lineage*, uint32_t);
    void erase(const goal_lineage*);
#ifndef DEBUG
private:
#endif
    std::map<const goal_lineage*, std::unordered_set<uint32_t>> watches;
};

#endif
