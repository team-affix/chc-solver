#include "../hpp/horizon.hpp"
#include "../hpp/horizon_sim.hpp"

horizon::horizon(
    const database& db,
    const goals& goals,
    trail& t,
    sequencer& vars,
    bind_map& bm,
    size_t max_resolutions,
    double exploration_constant,
    std::mt19937& rng
) :
    solver(db, goals, t, vars, bm, max_resolutions),
    exploration_constant(exploration_constant),
    rng(rng),
    root(),
    mc_sim(std::nullopt)
{}

std::unique_ptr<sim> horizon::construct_sim() {
    mc_sim.emplace(root, exploration_constant, rng);
    return std::make_unique<horizon_sim>(max_resolutions, db, gl, t, vars, ep, bm, lp, c, *mc_sim);
}

void horizon::terminate(sim& s) {
    mc_sim->terminate(static_cast<horizon_sim&>(s).reward());
}
