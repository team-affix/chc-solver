#ifndef TRACKED_SET_HPP
#define TRACKED_SET_HPP

#include <set>
#include "tracked.hpp"
#include "trail.hpp"

template<typename T>
struct tracked<std::set<T>> {
    tracked<std::set<T>>(trail&);
    void insert(const T&);
    void erase(const T&);
    const std::set<T>& get() const;
#ifndef DEBUG
private:
#endif
    trail& t;
    std::set<T> members;
};

template<typename T>
void tracked<std::set<T>>::insert(const T& val) {
    auto [_, inserted] = members.insert(val);
    if (inserted) t.log([this, val]() { members.erase(val); });
}

template<typename T>
void tracked<std::set<T>>::erase(const T& val) {
    auto [_, erased] = members.erase(val);
    if (erased) t.log([this, val]() { members.insert(val); });
}

#endif
