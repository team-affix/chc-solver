#ifndef I_FRONTIER_HPP
#define I_FRONTIER_HPP

#include <cstddef>
#include <memory>
#include <unordered_map>
#include "../value_objects/lineage.hpp"
#include "../value_objects/candidate.hpp"
#include "i_visitor.hpp"

struct i_frontier {
    using key_type = const goal_lineage*;
    using value_type = std::unordered_map<size_t, std::unique_ptr<candidate>>;
    virtual ~i_frontier() = default;
    virtual void insert(key_type, value_type) = 0;
    virtual bool contains(key_type) const = 0;
    virtual value_type& at(key_type) = 0;
    virtual const value_type& at(key_type) const = 0;
    virtual void erase(key_type) = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual void accept(i_visitor<std::pair<key_type, value_type&>>&) = 0;
};

#endif
