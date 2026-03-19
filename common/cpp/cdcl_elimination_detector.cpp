#include "../hpp/cdcl_elimination_detector.hpp"

cdcl_elimination_detector::cdcl_elimination_detector(
    const avoidance_store& as,
    lineage_pool& lp
)
    : as(as), lp(lp)
{}

bool cdcl_elimination_detector::operator()(const goal_lineage* gl, size_t i) {
    // Construct the resolution lineage
    const resolution_lineage* rl = lp.resolution(gl, i);

    // Check if the resolution lineage exists by itself in the avoidance store
    return as.count({rl}) > 0;
}
