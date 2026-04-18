#include "../hpp/ridge.hpp"
#include "../hpp/ridge_sim.hpp"

ridge::ridge(
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

std::unique_ptr<sim> ridge::construct_sim() {
    mc_sim.emplace(root, exploration_constant, rng);
    return std::make_unique<ridge_sim>(max_resolutions, db, gl, t, vars, ep, bm, lp, c, *mc_sim);
}

void ridge::terminate(sim& s) {
    mc_sim->terminate(-(double)s.get_decisions().size());
}
