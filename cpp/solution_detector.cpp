#include "../hpp/solution_detector.hpp"

solution_detector::solution_detector(
    const goal_store& gs
)
    : gs(gs)
{}

bool solution_detector::operator()() {
    return gs.empty();
}
