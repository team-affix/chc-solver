#include "../hpp/locator.hpp"

void locator::unbind(std::string name) {
    goal_to_resolution.erase(name);
}

void locator::purge() {
    goal_to_resolution.clear();
}
