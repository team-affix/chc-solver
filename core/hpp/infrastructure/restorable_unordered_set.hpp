#ifndef RESTORABLE_UNORDERED_SET_HPP
#define RESTORABLE_UNORDERED_SET_HPP

#include <unordered_set>
#include "../domain/interfaces/i_restorable_set.hpp"

template<typename T>
struct restorable_unordered_set : i_restorable_set<T> {
    void insert(const T&) override;
    void erase(const T&) override;
#ifndef DEBUG
private:
#endif
    std::unordered_set<T> current;
    std::unordered_set<T> additions;
    std::unordered_set<T> subtractions;
};

template<typename T>
void restorable_unordered_set<T>::insert(const T& value) {
    auto [_, inserted] = current.insert(value);
    if (!inserted) return;
    auto [_, subtraction_erased] = subtractions.erase(value);
    if (subtraction_erased) return;
    additions.insert(value);
}

template<typename T>
void restorable_unordered_set<T>::erase(const T& value) {
    auto [_, erased] = current.erase(value);
    if (!erased) return;
    auto [_, addition_erased] = additions.erase(value);
    if (addition_erased) return;
    subtractions.insert(value);
}

#endif
