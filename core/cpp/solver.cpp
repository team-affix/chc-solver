#include "../hpp/solver.hpp"

solver::solver(
    const database& db,
    const goals& gl,
    trail& t,
    sequencer& vars,
    bind_map& bm,
    size_t max_resolutions
) :
    db(db),
    gl(gl),
    t(t),
    vars(vars),
    bm(bm),
    ep(t),
    lp(),
    max_resolutions(max_resolutions),
    c(),
    managed_sim(nullptr)
{
    t.push();
}

solver::~solver() {
    t.pop();
}

bool solver::operator()() {
    if (c.refuted())
        return false;

    // tear down the previous frame and sim, then set up a fresh frame
    t.pop();
    managed_sim = nullptr;
    lp.trim();
    t.push();

    // build and run the simulation for this iteration
    managed_sim = construct_sim();
    bool result = (*managed_sim)();

    // derived-class post-processing (e.g. MCTS backpropagation)
    terminate(*managed_sim);

    // learn to avoid the exact derivation path taken this iteration;
    // this guarantees we never revisit the same decisions regardless of outcome
    const decisions& ds = managed_sim->get_decisions();
    c.learn(ds);

    // pin decision lineages so they survive the next lp.trim()
    for (const resolution_lineage* rl : ds)
        lp.pin(rl);

    return result;
}
