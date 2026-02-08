#include "../hpp/expr.hpp"

atom::atom(const std::string& a_value)
    : m_value(a_value)
{

}

const std::string& atom::value() const
{
    return m_value;
}

cons::cons(const cons& a_other)
: m_lhs(a_other.m_lhs ? new expr(*a_other.m_lhs) : nullptr),
  m_rhs(a_other.m_rhs ? new expr(*a_other.m_rhs) : nullptr)
{

}

cons& cons::operator=(const cons& a_other)
{
    m_lhs.reset(a_other.m_lhs ? new expr(*a_other.m_lhs) : nullptr);
    m_rhs.reset(a_other.m_rhs ? new expr(*a_other.m_rhs) : nullptr);
    return *this;
}

cons::cons(cons&& a_other)
    : m_lhs(std::move(a_other.m_lhs)), m_rhs(std::move(a_other.m_rhs))
{

}

cons& cons::operator=(cons&& a_other)
{
    m_lhs = std::move(a_other.m_lhs);
    m_rhs = std::move(a_other.m_rhs);
    return *this;
}

cons::cons(const expr& a_lhs, const expr& a_rhs)
    : m_lhs(new expr(a_lhs)), m_rhs(new expr(a_rhs))
{

}

const expr& cons::lhs() const
{
    return *m_lhs;
}

const expr& cons::rhs() const
{
    return *m_rhs;
}

var::var(uint32_t a_index)
    : m_index(a_index)
{

}

uint32_t var::index() const
{
    return m_index;
}

expr::expr(const std::variant<atom, cons, var>& a_content)
    : m_content(a_content)
{

}

const std::variant<atom, cons, var>& expr::content() const
{
    return m_content;
}

