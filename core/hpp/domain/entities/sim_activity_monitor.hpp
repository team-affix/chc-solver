#ifndef SIM_ACTIVITY_MONITOR_HPP
#define SIM_ACTIVITY_MONITOR_HPP

#include "../interfaces/i_get_sim_active.hpp"

struct sim_activity_monitor : i_get_sim_active {
    sim_activity_monitor();
    bool get_sim_active() override;
    void set_sim_active(bool active);
private:
    bool sim_active;
};

#endif
