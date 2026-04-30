#ifndef I_RESTORABLE_SET_HPP
#define I_RESTORABLE_SET_HPP

template<typename T>
struct i_restorable_set {
    virtual ~i_restorable_set() = default;
    virtual void restore() = 0;
    virtual void insert(const T&) = 0;
    virtual void erase(const T&) = 0;
};

#endif