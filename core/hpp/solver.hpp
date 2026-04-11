#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <optional>
#include <memory>
#include "trail.hpp"
#include "cdcl.hpp"
#include "sim.hpp"
#include "lemma.hpp"

struct solver {
    virtual ~solver();
    solver(
        trail&
    );
    bool operator()(std::optional<lemma>&);
#ifndef DEBUG
private:
#endif
    virtual std::unique_ptr<sim> construct_sim() = 0;
    virtual void on_terminate_sim(sim*) = 0;

    trail& t;

    cdcl c;
    std::unique_ptr<sim> s;
};

#endif
