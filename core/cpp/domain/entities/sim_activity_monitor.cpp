#include "../../../hpp/domain/entities/sim_activity_monitor.hpp"

sim_activity_monitor::sim_activity_monitor() :
    sim_active(false) {
}

bool sim_activity_monitor::get_sim_active() {
    return sim_active;
}

void sim_activity_monitor::set_sim_active(bool active) {
    sim_active = active;
}
