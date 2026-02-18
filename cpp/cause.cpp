#include "../hpp/cause.hpp"

causal_set::causal_set(const std::set<fulfillment>& fulfillments)
    : fulfillments(fulfillments)
{}

std::strong_ordering causal_set::operator<=>(const causal_set& other) const {
    if (auto cmp = fulfillments.size() <=> other.fulfillments.size(); cmp != 0)
        return cmp;
    return fulfillments <=> other.fulfillments;
}

causal_set causal_set::operator+(const causal_set& other) const {
    causal_set result;
    result.fulfillments.insert(fulfillments.begin(), fulfillments.end());
    result.fulfillments.insert(other.fulfillments.begin(), other.fulfillments.end());
    return result;
}

bool causal_set::empty() const {
    return fulfillments.empty();
}

size_t causal_set::size() const {
    return fulfillments.size();
}
