#ifndef BOX_HPP
#define BOX_HPP

#include <memory>

// // very simple smart pointer that adds copy handling to unique_ptr
// template<typename T>
// struct box {
//     std::unique_ptr<T> m_ptr;
//     box() {}
//     box(const T* a_t) : m_ptr(a_t) {}
//     box(const box& a_b) : m_ptr(new T(*a_b.m_ptr)) {}
//     box& operator=(const box& a_b) {m_ptr.reset(new T(*a_b.m_ptr));}
// };

#endif

