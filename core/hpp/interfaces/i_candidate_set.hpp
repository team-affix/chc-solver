#ifndef I_CANDIDATE_SET_HPP
#define I_CANDIDATE_SET_HPP

#include <cstddef>
#include "../interfaces/i_visitor.hpp"

struct i_candidate_set {
    virtual ~i_candidate_set() = default;
    virtual void insert(size_t) = 0;
    virtual void erase(size_t) = 0;
    virtual void accept(i_visitor<size_t>&) const = 0;
    virtual size_t size() const = 0;
};

#endif
