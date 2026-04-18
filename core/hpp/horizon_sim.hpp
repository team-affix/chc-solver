#ifndef HORIZON_SIM_HPP
#define HORIZON_SIM_HPP

#include "../../mcts/include/mcts.hpp"
#include "mcts_decider.hpp"
#include "sim.hpp"
#include "weight_store.hpp"

struct horizon_sim : sim {
    horizon_sim(
        size_t,
        const database&,
        const goals&,
        trail&,
        sequencer&,
        expr_pool&,
        bind_map&,
        lineage_pool&,
        cdcl,
        monte_carlo::simulation<mcts_decider::choice, std::mt19937>&
    );
    double reward();
#ifndef DEBUG
protected:
#endif
    const resolution_lineage* decide_one() override;
    void on_resolve(const resolution_lineage*) override;

    mcts_decider dec;
    weight_store ws;
};

#endif
