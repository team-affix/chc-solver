#ifndef GOAL_EXPR_CHANGED_DETECTOR_HPP
#define GOAL_EXPR_CHANGED_DETECTOR_HPP

#include <unordered_map>
#include <unordered_set>
#include "../interfaces/i_goal_expr_changed_detector.hpp"
#include "../interfaces/i_event_producer.hpp"
#include "../interfaces/i_bind_map.hpp"
#include "../interfaces/i_expr_pool.hpp"
#include "../interfaces/i_goal_expr_store.hpp"
#include "../events/goal_expr_changed_event.hpp"
#include "../value_objects/expr.hpp"

struct goal_expr_changed_detector : i_goal_expr_changed_detector {
    goal_expr_changed_detector();
    void goal_activated(const goal_lineage*) override;
    void goal_deactivated(const goal_lineage*) override;
    void rep_updated(uint32_t) override;
private:
    void extract_rep_vars(const expr*, std::unordered_set<uint32_t>&);
    void watch(const std::unordered_set<uint32_t>&, const std::unordered_set<const goal_lineage*>&);
    std::unordered_set<const goal_lineage*> unwatch(uint32_t);
    std::unordered_set<uint32_t> unwatch(const goal_lineage*);
    void update_rep_watches(uint32_t);

    i_event_producer<goal_expr_changed_event>& goal_expr_changed_producer;
    i_bind_map& bm;
    i_expr_pool& ep;
    i_goal_expr_store& ges;

    std::unordered_map<uint32_t, std::unordered_set<const goal_lineage*>> rep_to_goals;
    std::unordered_map<const goal_lineage*, std::unordered_set<uint32_t>> goal_to_reps;
};

#endif
