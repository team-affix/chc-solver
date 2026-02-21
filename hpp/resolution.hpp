#ifndef RESOLUTION_HPP
#define RESOLUTION_HPP

#include <map>
#include "goal.hpp"
#include "rule.hpp"

struct resolution {
    const resolution* parent;
    subgoal_id chosen_subgoal;
    rule_id    chosen_rule;
    auto operator<=>(const resolution&) const = default;
};

struct resolution_pool {
    const resolution* make_resolution(const resolution*, subgoal_id, rule_id);
    void pin(const resolution*);
    void trim();
    size_t size() const;
#ifndef DEBUG
private:
#endif
    const resolution* intern(resolution&&);
    std::map<resolution, bool> resolutions;
};

#endif
