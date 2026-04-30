#ifndef I_RESTORABLE_SET_HPP
#define I_RESTORABLE_SET_HPP

#include <set>

template<typename T>
struct i_restorable_set {
    virtual ~i_restorable_set() = default;
    virtual void restore() = 0;
    virtual void insert(const T&);
    virtual void erase(const T&);
};

#endif