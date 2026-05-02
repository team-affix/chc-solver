#ifndef EXPR_PRINTER_HPP
#define EXPR_PRINTER_HPP

#include <ostream>
#include <map>
#include <string>
#include <cstdint>
#include "../domain/interfaces/i_expr_printer.hpp"

struct expr_printer : i_expr_printer {
    expr_printer(std::ostream&, const std::map<uint32_t, std::string>& var_names);
    void print(const expr*) const override;
#ifndef DEBUG
private:
#endif
    std::ostream& os;
    const std::map<uint32_t, std::string>& var_names;
};

#endif
