#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include <unordered_map>
#include <string>

struct locator {
    template<typename T>
    T& operator()(std::string name);
    template<typename T>
    void bind(std::string name, T& value);
    void unbind(std::string name);
    void purge();
#ifndef DEBUG
private:
#endif
    std::unordered_map<std::string, void*> goal_to_resolution;
};

template<typename T>
T& locator::operator()(std::string name) {
    return *reinterpret_cast<T*>(goal_to_resolution.at(name));
}

template<typename T>
void locator::bind(std::string name, T& value) {
    goal_to_resolution[name] = &value;
}

#endif
