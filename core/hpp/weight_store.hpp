#ifndef WEIGHT_STORE_HPP
#define WEIGHT_STORE_HPP

#include "frontier.hpp"
#include "rule.hpp"

struct weight_store : frontier<double> {
    virtual ~weight_store() = default;
    weight_store(
        const goals&,
        const database&,
        lineage_pool&,
        trail&
    );
    double total() const;
    std::optional<std::vector<double>> expand(const double&, const rule&) override;
#ifndef DEBUG
private:
#endif

    tracked<double> cgw;
};

#endif
