#ifndef FRONTIER_HPP
#define FRONTIER_HPP

#include <unordered_map>
#include "../interfaces/i_frontier.hpp"

struct frontier : i_frontier {
    void insert(key_type, value_type) override;
    bool contains(key_type) const override;
    value_type& at(key_type) override;
    const value_type& at(key_type) const override;
    void erase(key_type) override;
    void clear() override;
    size_t size() const override;
    void accept(i_visitor<std::pair<key_type, value_type&>>&) override;
private:
    std::unordered_map<key_type, value_type> goals_;
};

#endif
