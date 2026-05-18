#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "../interfaces/i_solver.hpp"
#include "../interfaces/i_sim_setup.hpp"
#include "../interfaces/i_sim_teardown.hpp"
#include "../interfaces/i_sim.hpp"
#include "../interfaces/i_cdcl_elimination_generator.hpp"
#include "../interfaces/i_elimination_router.hpp"

struct solver : i_solver {
    solver(
        i_sim_setup& sim_setup,
        i_sim_teardown& sim_teardown,
        i_sim& sim,
        i_cdcl_elimination_generator& cdcl_elimination_generator,
        i_elimination_router& elimination_router);
    virtual ~solver();
    state_machine<solver_yield> solve() override;
private:
    i_sim_setup& sim_setup;
    i_sim_teardown& sim_teardown;
    i_sim& sim;
    i_cdcl_elimination_generator& cdcl_elimination_generator;
    i_elimination_router& elimination_router;
};

#endif
