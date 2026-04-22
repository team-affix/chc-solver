#include "../hpp/bind_watch.hpp"

void bind_watch::operator()(const goal_lineage* gl, uint32_t var) {
    watches[gl].insert(var);
}

void bind_watch::erase(const goal_lineage* gl) {
    watches.erase(gl);
}
