#ifndef TRACKED_MAP_HPP
#define TRACKED_MAP_HPP

#include "trail.hpp"
#include <utility>

template<typename M>
struct tracked_map {
    tracked_map(trail&);
    std::pair<typename M::const_iterator, bool> upsert(const M::key_type&, const M::value_type&);
    std::pair<typename M::const_iterator, bool> erase(const M::key_type&);
    const M& get() const;
#ifndef DEBUG
private:
#endif
    trail& t;
    M members;
};

template<typename M>
std::pair<typename M::const_iterator, bool> tracked_map<M>::upsert(const M::key_type& key, const M::value_type& value) {
    auto [it, inserted] = members.insert({key, value});
    if (inserted) {
        t.log([this, key, value]() { members.erase(key); });
    }
    else {
        auto old_value = it->second;
        it->second = value;
        t.log([this, key, old_value]() { members.at(key) = old_value; });
    }
    return {it, inserted};
}

template<typename M>
std::pair<typename M::const_iterator, bool> tracked_map<M>::erase(const M::key_type& key) {
    auto it = members.find(key);
    if (it == members.end())
        return {it, false};
    auto old_value = it->second;
    auto result = members.erase(it);
    t.log([this, key, old_value]() { members.insert({key, old_value}); });
    return result;
}

#endif
