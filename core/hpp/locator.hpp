#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include <unordered_map>
#include <cassert>
#include <stack>
#include <unordered_set>

enum class locator_keys {
    inst_database,
    inst_goals,
    inst_trail,
    inst_var_sequencer,
    inst_bind_map,
    inst_normalizer,
    inst_expr_printer,
    inst_expr_pool,
    inst_goal_store,
    inst_candidate_store,
    inst_copier,
    inst_cdcl,
    inst_cdcl_eliminator,
    inst_head_eliminator,
    inst_frontier_watch,
    inst_resolution_lineage,
    inst_goal_lineage,
    inst_lineage_pool,
    inst_rep_changed_topic,
    inst_goal_inserted_topic,
    inst_goal_resolved_topic,
    inst_new_eliminated_resolution_topic,
    inst_unit_topic,
};

struct locator {
    template<typename T>
    T& operator()(locator_keys);
    template<typename T>
    void bind(locator_keys, T& value);
    void unbind(locator_keys);
    void push_frame();
    void pop_frame();
#ifndef DEBUG
private:
#endif
    std::unordered_map<locator_keys, void*> entries;
    std::unordered_set<locator_keys> current_frame_additions;
    std::stack<std::unordered_set<locator_keys>> past_frames;
};

template<typename T>
T& locator::operator()(locator_keys key) {
    return *reinterpret_cast<T*>(entries.at(key));
}

template<typename T>
void locator::bind(locator_keys key, T& value) {
    auto [_, success] = entries.insert({key, &value});
    assert(success);
    current_frame_additions.insert(key);
}

#endif
