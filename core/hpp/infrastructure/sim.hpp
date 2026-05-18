#ifndef SIM_HPP
#define SIM_HPP

#include "../interfaces/i_sim.hpp"
#include "../interfaces/i_unit_goals.hpp"
#include "../interfaces/i_decision_generator.hpp"
#include "../interfaces/i_elimination_generator.hpp"
#include "../interfaces/i_elimination_router.hpp"
#include "../interfaces/i_resolver.hpp"

struct sim : i_sim {
    sim(
        size_t max_resolutions,
        i_unit_goals& ug,
        i_decision_generator& dg,
        i_elimination_generator& eg,
        i_elimination_router& er,
        i_resolver& r);
    sim_termination run() override;
private:
    size_t max_resolutions;
    i_unit_goals& ug;
    i_decision_generator& dg;
    i_elimination_generator& eg;
    i_elimination_router& er;
    i_resolver& r;
};

#endif
