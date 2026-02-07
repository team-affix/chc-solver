#include "../hpp/expr.hpp"

expr::cons::cons()
    : m_lhs(nullptr), m_rhs(nullptr)
{

}

expr::cons::cons(const cons& a_other)
    : m_lhs(new expr(*a_other.m_lhs)), m_rhs(new expr(*a_other.m_rhs))
{

}

expr::cons& expr::cons::operator=(const cons& a_other)
{
    m_lhs.reset(new expr(*a_other.m_lhs));
    m_rhs.reset(new expr(*a_other.m_rhs));
    return *this;
}

expr::cons::cons(cons&& a_other)
    : m_lhs(std::move(a_other.m_lhs)), m_rhs(std::move(a_other.m_rhs))
{

}

expr::cons& expr::cons::operator=(cons&& a_other)
{
    m_lhs = std::move(a_other.m_lhs);
    m_rhs = std::move(a_other.m_rhs);
    return *this;
}

expr::cons::cons(const expr& a_lhs, const expr& a_rhs)
    : m_lhs(new expr(a_lhs)), m_rhs(new expr(a_rhs))
{

}

