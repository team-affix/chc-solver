#include <stdexcept>
#include "../hpp/a01_defs.hpp"
#include "../hpp/a01_goal_resolver.hpp"
#include "../hpp/a01_head_elimination_detector.hpp"
#include "../hpp/unit_propagation_detector.hpp"
#include "../hpp/solution_detector.hpp"
#include "../hpp/conflict_detector.hpp"
#include "../hpp/a01_cdcl_elimination_detector.hpp"
#include "../hpp/a01_decider.hpp"
#include "../hpp/normalizer.hpp"

bool a01_sim_one(
    lineage_pool& lp,
    a01_goal_store& gs,
    a01_candidate_store& cs,
    a01_decision_store& ds,
    conflict_detector& cd,
    solution_detector& sd,
    a01_head_elimination_detector& he,
    a01_cdcl_elimination_detector& ce,
    unit_propagation_detector& up,
    a01_goal_resolver& gr,
    a01_decider& dec) {
    
    while (!cd() && !sd()) {
        size_t elim0 = std::erase_if(cs, [&he](const auto& e) { return he(e.first, e.second); });
        size_t elim1 = std::erase_if(cs, [&ce](const auto& e) { return ce(e.first, e.second); });
        const auto it = std::find_if(gs.begin(), gs.end(), [&up](const auto& e) { return up(e.first); });

        // enact the propagation
        if (it != gs.end())
            gr(it->first, cs.find(it->first)->second);

        // continue until fixpoint
        if (elim0 > 0 || elim1 > 0 || it != gs.end())
            continue;

        auto [chosen_goal, chosen_candidate] = dec();

        gr(chosen_goal, chosen_candidate);

        auto rl = lp.resolution(chosen_goal, chosen_candidate);

        ds.insert(rl);

    }

    return sd();
    
}

struct example_problem {
    a01_database db;
    a01_goal_store gs;
};

example_problem init_ep_0(expr_pool& ep, lineage_pool& lp) {
    // a.
    // :- a.
    example_problem p;
    // edit database
    p.db.push_back(rule{ep.atom("a"), {}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("a")});

    return p;
}

example_problem init_ep_1(expr_pool& ep, lineage_pool& lp) {
    // a.
    // :- b.
    example_problem p;
    // edit database
    p.db.push_back(rule{ep.atom("a"), {}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("b")});

    return p;
}

example_problem init_ep_2(expr_pool& ep, lineage_pool& lp) {
    // a :- b.
    // %no b candidates (refuted)
    // :- a.
    example_problem p;

    // edit database
    p.db.push_back(rule{ep.atom("a"), {ep.atom("b")}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("a")});

    return p;
}

example_problem init_ep_3(expr_pool& ep, lineage_pool& lp) {
    // a :- b.
    // b.
    // :- a.
    example_problem p;

    // edit database
    p.db.push_back(rule{ep.atom("a"), {ep.atom("b")}});
    p.db.push_back(rule{ep.atom("b"), {}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("a")});

    return p;
}

example_problem init_ep_4(expr_pool& ep, lineage_pool& lp) {
    // a :- b, c.
    // b.
    // :- a.
    // %no c candidates (refuted)
    example_problem p;
    // edit database
    p.db.push_back(rule{ep.atom("a"), {ep.atom("b"), ep.atom("c")}});
    p.db.push_back(rule{ep.atom("b"), {}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("a")});

    return p;
}

example_problem init_ep_5(expr_pool& ep, lineage_pool& lp) {
    // a :- b, c.
    // b.
    // c.
    // :- a.
    example_problem p;
    // edit database
    p.db.push_back(rule{ep.atom("a"), {ep.atom("b"), ep.atom("c")}});
    p.db.push_back(rule{ep.atom("b"), {}});
    p.db.push_back(rule{ep.atom("c"), {}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("a")});

    return p;
}

example_problem init_ep_6(expr_pool& ep, lineage_pool& lp) {
    // a :- b, c.
    // a :- d.
    // b.
    // %no d or c candidates (refuted inductively)
    // :- a.
    example_problem p;
    // edit database
    p.db.push_back(rule{ep.atom("a"), {ep.atom("b"), ep.atom("c")}});
    p.db.push_back(rule{ep.atom("a"), {ep.atom("d")}});
    p.db.push_back(rule{ep.atom("b"), {}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("a")});

    return p;
}

example_problem init_ep_7(expr_pool& ep, lineage_pool& lp) {
    // a :- b, c.
    // a :- d.
    // b.
    // c.
    // d.
    // :- a.
    example_problem p;
    // edit database
    p.db.push_back(rule{ep.atom("a"), {ep.atom("b"), ep.atom("c")}});
    p.db.push_back(rule{ep.atom("a"), {ep.atom("d")}});
    p.db.push_back(rule{ep.atom("b"), {}});
    p.db.push_back(rule{ep.atom("c"), {}});
    p.db.push_back(rule{ep.atom("d"), {}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("a")});

    return p;
}

example_problem init_ep_8(expr_pool& ep, lineage_pool& lp) {
    // a :- b, c.
    // a :- d.
    // b.
    // c.
    // d.
    // :- a.
    example_problem p;
    // edit database
    p.db.push_back(rule{ep.atom("a"), {ep.atom("b"), ep.atom("c")}});
    p.db.push_back(rule{ep.atom("a"), {ep.atom("d")}});
    p.db.push_back(rule{ep.atom("b"), {}});
    p.db.push_back(rule{ep.atom("c"), {}});
    p.db.push_back(rule{ep.atom("d"), {}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("a")});

    return p;
}

example_problem init_ep_9(expr_pool& ep, lineage_pool& lp) {
    // a :- b.
    // a :- c.
    // a :- d.
    // a :- e.
    // a :- f.
    // no b, c, d, e, f candidates (refuted)
    example_problem p;
    // edit database
    p.db.push_back(rule{ep.atom("a"), {ep.atom("b")}});
    p.db.push_back(rule{ep.atom("a"), {ep.atom("c")}});
    p.db.push_back(rule{ep.atom("a"), {ep.atom("d")}});
    p.db.push_back(rule{ep.atom("a"), {ep.atom("e")}});
    p.db.push_back(rule{ep.atom("a"), {ep.atom("f")}});

    // edit goal store
    p.gs.insert({lp.goal(nullptr, 0), ep.atom("a")});

    return p;
}

struct solution {
    
};

void a01() {
    trail t;
    bind_map bm(t);
    sequencer vars(t);
    expr_pool ep(t);
    copier cp(vars, ep);
    normalizer n(ep, bm);
    lineage_pool lp;
    
    a01_goal_store gs_main;
    a01_goal_store gs_working_copy;
    a01_candidate_store cs_main;
    a01_candidate_store cs_working_copy;
    a01_resolution_store rs_main;
    a01_resolution_store rs_working_copy;
    a01_decision_store ds_main;
    a01_decision_store ds_working_copy;
    a01_avoidance_store as_main;
    a01_avoidance_store as_working_copy;
    a01_database db;

    solution_detector sd(gs_working_copy);
    conflict_detector cd(gs_working_copy, cs_working_copy);
    a01_head_elimination_detector he(t, bm, gs_working_copy, db);
    a01_cdcl_elimination_detector ce(as_working_copy, lp);
    unit_propagation_detector up(cs_working_copy);
    a01_goal_adder ga(gs_working_copy, cs_working_copy, db);
    a01_goal_resolver gr(rs_working_copy, gs_working_copy, cs_working_copy, db, cp, bm, lp, ga, as_working_copy);

    // PUSH INITIAL FRAME
    t.push();

    // CHOOSE EXAMPLE PROBLEM
    {
        example_problem epm = init_ep_9(ep, lp);
        // extract db
        db = epm.db;
        // extract gs
        a01_goal_adder ga_main(gs_main, cs_main, db);
        for (const auto& [gl, ge] : epm.gs)
            ga_main(gl, ge);
    }
    
    constexpr size_t ITERATIONS_BEFORE_CDCL = 1000;
    constexpr double EXPLORATION_CONSTANT = 1.414;
    
    while(true) {
        monte_carlo::tree_node<a01_decider::choice> root;

        a01_decision_store smallest_ds;
        size_t smallest_ds_size = std::numeric_limits<size_t>::max();
        
        for (size_t i = 0; i < ITERATIONS_BEFORE_CDCL; ++i) {

            std::mt19937 rng(10);

            monte_carlo::simulation<a01_decider::choice, std::mt19937> sim(root, EXPLORATION_CONSTANT, rng);

            a01_decider dec(gs_working_copy, cs_working_copy, sim);

            // reset all working copies
            gs_working_copy = gs_main;
            cs_working_copy = cs_main;
            rs_working_copy = {};
            ds_working_copy = {};
            as_working_copy = as_main;

            t.push();

            if (a01_sim_one(lp, gs_working_copy, cs_working_copy, ds_working_copy, cd, sd, he, ce, up, gr, dec)) {
                throw std::runtime_error("Solution found");
            }

            // check for refutation
            if (ds_working_copy.empty()) {
                throw std::runtime_error("Refuted");
            }

            t.pop();

            // prefer smaller decision stores
            sim.terminate(-ds_working_copy.size());

            if (ds_working_copy.size() < smallest_ds_size) {
                smallest_ds = ds_working_copy;
                smallest_ds_size = ds_working_copy.size();
            }
        }

        // CDCL learning
        as_main.insert(smallest_ds);

        // trim (not yet implmented)
        
    }
    
}

#ifndef DEBUG

int main() {
    a01();
    return 0;
}

#endif
