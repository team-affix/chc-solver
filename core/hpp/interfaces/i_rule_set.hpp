#ifndef I_RULE_SET_HPP
#define I_RULE_SET_HPP

#include "../value_objects/rule.hpp"
#include "../interfaces/i_visitor.hpp"

struct i_rule_set {
    virtual ~i_rule_set() = default;
    virtual void insert(rule) = 0;
    virtual void erase(rule) = 0;
    virtual void accept(i_visitor<const rule&>&) const = 0;
    virtual size_t size() const = 0;
};

#endif
