#include "../hpp/expr.hpp"
#include <iostream>
#include <list>

int main() {
    // expr l_expr_3 {
    //     cons{expr{atom{"test"}}, expr{atom{"test2"}}}
    // };
    expr l_expr_3 {atom{"test"}};
//    expr l_expr_4 = l_expr_3;
//    l_expr_4 = std::move(l_expr_3);
  expr l_expr_5{
      expr{cons{
          expr{atom{
              "test"
          }},
          expr{cons{
              expr{atom{"jake"}},
              expr{atom{"leon"}}
          }}
      }}
  };
    return 0;
}

