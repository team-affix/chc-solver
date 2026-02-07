#ifndef BOX_HPP
#define BOX_HPP

#include <memory>

// very simple smart pointer that adds copy handling to unique_ptr
template<typename T>
struct box {
    std::unique_ptr<T> m_ptr;
    box();
    box(const T*);
    box(const box&);
    box& operator=(const box&);
};

#endif

