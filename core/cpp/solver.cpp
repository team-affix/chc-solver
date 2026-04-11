#include "../hpp/solver.hpp"

solver::~solver() {
    t.pop();
}

solver::solver(
    trail& t
) :
    t(t),
    c(t),
    s()
{
    t.push();
}

bool solver::operator()(std::optional<lemma>& soln) {
    // default to no solution
    soln = std::nullopt;

    // manage the trail and memory
    t.pop();
    s = nullptr;

    // check for refutation
    if (c.refuted())
        return false;
    
    // construct the simulation on new frame
    t.push();
    s = construct_sim();

    // run the simulation
    bool solved = s->operator()();

    // learn to avoid the decisions
    c.learn(lemma(s->get_decisions()));

    // if solved, return solution
    if (solved)
        soln = lemma(s->get_resolutions());

    // terminate the simulation
    on_terminate_sim(s.get());
    
    // return the result
    return true;
}
