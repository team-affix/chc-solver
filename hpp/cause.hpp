#ifndef CAUSE_HPP
#define CAUSE_HPP

#include <set>
#include "fulfill.hpp"

struct causal_set {
    causal_set() = default;
    causal_set(const std::set<fulfillment>&);
    std::strong_ordering operator<=>(const causal_set&) const;
    causal_set operator+(const causal_set&) const;
    bool empty() const;
    size_t size() const;
private:
    std::set<fulfillment> fulfillments;
};

#endif