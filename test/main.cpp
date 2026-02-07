#include "../hpp/expr.hpp"
#include <iostream>
int main() {
    expr l_expr;
    expr l_expr_2;
    l_expr_2 = l_expr;
    expr l_expr_3 {
        expr::cons{expr{expr::atom{"test"}}, expr{expr::atom{"test2"}}}
    };
    return 0;
}

