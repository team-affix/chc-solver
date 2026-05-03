#ifndef CANDIDATE_NOT_APPLICABLE_DETECTOR_HPP
#define CANDIDATE_NOT_APPLICABLE_DETECTOR_HPP

#include "../interfaces/i_candidate_not_applicable_detector.hpp"
#include "../interfaces/i_event_producer.hpp"
#include "../../utility/i_trail.hpp"
#include "../interfaces/i_bind_map.hpp"
#include "../interfaces/i_goal_expr_store.hpp"
#include "../interfaces/i_goal_candidates_store.hpp"
#include "../interfaces/i_lineage_pool.hpp"
#include "../interfaces/i_database.hpp"
#include "../events/candidate_not_applicable_event.hpp"

struct candidate_not_applicable_detector : i_candidate_not_applicable_detector {
    candidate_not_applicable_detector();
    void goal_expr_changed(const goal_lineage*) override;
private:
    bool candidate_applicable(const goal_lineage*, size_t);

    i_event_producer<candidate_not_applicable_event>& candidate_not_applicable_producer;
    i_bind_map& bm;
    i_goal_expr_store& ges;
    i_goal_candidates_store& gcs;
    i_lineage_pool& lp;
    const i_database& db;
    i_trail& t;
};

#endif
