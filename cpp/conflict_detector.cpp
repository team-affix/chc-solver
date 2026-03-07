#include "../hpp/conflict_detector.hpp"

conflict_detector::conflict_detector(
    const a01_goal_store& gs,
    const a01_candidate_store& cs
)
    : gs(gs), cs(cs)
{}

bool conflict_detector::operator()() {
    return std::any_of(gs.begin(), gs.end(), [this](const auto& e) {
        return cs.count(e.first) == 0;
    });
}
