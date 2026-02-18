#ifndef CAUSE_HPP
#define CAUSE_HPP

#include <set>
#include "fulfill.hpp"

struct causal_set {
    std::strong_ordering operator<=>(const causal_set& other) const;
    causal_set operator+(const causal_set& other) const;
    bool empty() const;
    size_t size() const;
private:
    std::set<fulfillment> fulfillments;
};

#endif