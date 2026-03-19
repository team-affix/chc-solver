#include "../hpp/conflict_detector.hpp"

conflict_detector::conflict_detector(
    const goal_store& gs,
    const candidate_store& cs
)
    : gs(gs), cs(cs)
{}

bool conflict_detector::operator()() {
    return std::any_of(gs.begin(), gs.end(), [this](const auto& e) {
        return cs.count(e.first) == 0;
    });
}
