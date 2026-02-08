#include "../hpp/expr.hpp"
#include <iostream>
#include <list>
#include <vector>
#include "test_utils.hpp"

void test_atom_constructor() {
    // Basic string
    atom a1("hello");
    assert(a1.value() == "hello");
    
    // Empty string
    atom a2("");
    assert(a2.value() == "");
    
    // Single character
    atom a3("x");
    assert(a3.value() == "x");
    
    // Numeric strings
    atom a4("0");
    assert(a4.value() == "0");
    atom a5("12345");
    assert(a5.value() == "12345");
    atom a6("-42");
    assert(a6.value() == "-42");
    
    // Special characters
    atom a7("test_123");
    assert(a7.value() == "test_123");
    atom a8("hello world");
    assert(a8.value() == "hello world");
    atom a9("!@#$%^&*()");
    assert(a9.value() == "!@#$%^&*()");
    atom a10("a+b=c");
    assert(a10.value() == "a+b=c");
    
    // Whitespace variations
    atom a11(" ");
    assert(a11.value() == " ");
    atom a12("\t");
    assert(a12.value() == "\t");
    atom a13("\n");
    assert(a13.value() == "\n");
    atom a14("  spaces  ");
    assert(a14.value() == "  spaces  ");
    
    // Long string
    std::string long_str(1000, 'a');
    atom a15(long_str);
    assert(a15.value() == long_str);
    assert(a15.value().length() == 1000);
    
    // Unicode/special characters
    atom a16("αβγδ");
    assert(a16.value() == "αβγδ");
    atom a17("日本語");
    assert(a17.value() == "日本語");
    
    // Escape sequences
    atom a18("line1\nline2");
    assert(a18.value() == "line1\nline2");
    atom a19("tab\there");
    assert(a19.value() == "tab\there");
    
    // Multiple atoms with same value
    atom a20("duplicate");
    atom a21("duplicate");
    assert(a20.value() == a21.value());
    assert(a20.value() == "duplicate");
    assert(a21.value() == "duplicate");
}

void test_var_constructor() {
    // Zero index
    var v1(0);
    assert(v1.index() == 0);
    
    // Small indices
    var v2(1);
    assert(v2.index() == 1);
    var v3(2);
    assert(v3.index() == 2);
    var v4(10);
    assert(v4.index() == 10);
    
    // Medium indices
    var v5(100);
    assert(v5.index() == 100);
    var v6(999);
    assert(v6.index() == 999);
    var v7(1000);
    assert(v7.index() == 1000);
    
    // Large indices
    var v8(1000000);
    assert(v8.index() == 1000000);
    var v9(2147483647);
    assert(v9.index() == 2147483647);
    
    // Maximum value
    var v10(UINT32_MAX);
    assert(v10.index() == UINT32_MAX);
    var v11(4294967295u);
    assert(v11.index() == 4294967295u);
    
    // Multiple vars with same index
    var v12(42);
    var v13(42);
    assert(v12.index() == 42);
    assert(v13.index() == 42);
    assert(v12.index() == v13.index());
    
    // Sequential indices
    for (uint32_t i = 0; i < 100; i++) {
        var v(i);
        assert(v.index() == i);
    }
}

void test_cons_constructor() {
    // Basic cons with atoms
    cons c1(expr{atom{"left"}}, expr{atom{"right"}});
    
    // Verify lhs and rhs
    assert(std::get<atom>(c1.lhs().content()).value() == "left");
    assert(std::get<atom>(c1.rhs().content()).value() == "right");
    
    // Cons with variables
    cons c2(expr{var{0}}, expr{var{1}});
    
    assert(std::get<var>(c2.lhs().content()).index() == 0);
    assert(std::get<var>(c2.rhs().content()).index() == 1);
    
    // Cons with mixed types (atom and var)
    cons c3(expr{atom{"atom"}}, expr{var{42}});
    
    assert(std::get<atom>(c3.lhs().content()).value() == "atom");
    assert(std::get<var>(c3.rhs().content()).index() == 42);
    
    // Cons with same expr on both sides
    expr e1{atom{"same"}};
    cons c4(e1, e1);
    
    assert(std::get<atom>(c4.lhs().content()).value() == "same");
    assert(std::get<atom>(c4.rhs().content()).value() == "same");
    
    // Nested cons - cons on left
    cons c5(expr{atom{"inner_left"}}, expr{atom{"inner_right"}});
    cons c6(expr{c5}, expr{atom{"outer_right"}});
    
    const cons& inner_left = std::get<cons>(c6.lhs().content());
    assert(std::get<atom>(inner_left.lhs().content()).value() == "inner_left");
    assert(std::get<atom>(inner_left.rhs().content()).value() == "inner_right");
    assert(std::get<atom>(c6.rhs().content()).value() == "outer_right");
    
    // Nested cons - cons on right
    cons c7(expr{atom{"outer_left"}}, expr{c5});
    
    assert(std::get<atom>(c7.lhs().content()).value() == "outer_left");
    const cons& inner_right = std::get<cons>(c7.rhs().content());
    assert(std::get<atom>(inner_right.lhs().content()).value() == "inner_left");
    assert(std::get<atom>(inner_right.rhs().content()).value() == "inner_right");
    
    // Nested cons - cons on both sides
    cons c8(expr{atom{"left1"}}, expr{atom{"left2"}});
    cons c9(expr{atom{"right1"}}, expr{atom{"right2"}});
    cons c10(expr{c8}, expr{c9});
    
    const cons& left_cons = std::get<cons>(c10.lhs().content());
    const cons& right_cons = std::get<cons>(c10.rhs().content());
    assert(std::get<atom>(left_cons.lhs().content()).value() == "left1");
    assert(std::get<atom>(left_cons.rhs().content()).value() == "left2");
    assert(std::get<atom>(right_cons.lhs().content()).value() == "right1");
    assert(std::get<atom>(right_cons.rhs().content()).value() == "right2");
    
    // Deep nesting (3 levels)
    cons c11(expr{c10}, expr{var{999}});
    
    const cons& level2 = std::get<cons>(c11.lhs().content());
    const cons& level2_left = std::get<cons>(level2.lhs().content());
    assert(std::get<atom>(level2_left.lhs().content()).value() == "left1");
    assert(std::get<var>(c11.rhs().content()).index() == 999);
    
    // Empty string atoms in cons
    cons c12(expr{atom{""}}, expr{atom{""}});
    
    assert(std::get<atom>(c12.lhs().content()).value() == "");
    assert(std::get<atom>(c12.rhs().content()).value() == "");
    
    // All-in-one deeply nested construction
    expr deeply_nested{cons{
        expr{cons{expr{atom{"a"}}, expr{atom{"b"}}}},
        expr{cons{expr{var{1}}, expr{var{2}}}}
    }};
    
    const cons& outer = std::get<cons>(deeply_nested.content());
    const cons& left_inner = std::get<cons>(outer.lhs().content());
    const cons& right_inner = std::get<cons>(outer.rhs().content());
    assert(std::get<atom>(left_inner.lhs().content()).value() == "a");
    assert(std::get<atom>(left_inner.rhs().content()).value() == "b");
    assert(std::get<var>(right_inner.lhs().content()).index() == 1);
    assert(std::get<var>(right_inner.rhs().content()).index() == 2);
}

void test_cons_copy() {
    // Basic copy constructor
    cons c1(expr{atom{"left"}}, expr{atom{"right"}});
    cons c2(c1);  // Copy constructor
    
    // Verify both have same values
    assert(std::get<atom>(c1.lhs().content()).value() == "left");
    assert(std::get<atom>(c1.rhs().content()).value() == "right");
    assert(std::get<atom>(c2.lhs().content()).value() == "left");
    assert(std::get<atom>(c2.rhs().content()).value() == "right");
    
    // Verify deep copy (different addresses)
    assert(&c1.lhs() != &c2.lhs());
    assert(&c1.rhs() != &c2.rhs());
    
    // Copy constructor with variables
    cons c3(expr{var{100}}, expr{var{200}});
    cons c4(c3);
    
    assert(std::get<var>(c3.lhs().content()).index() == 100);
    assert(std::get<var>(c3.rhs().content()).index() == 200);
    assert(std::get<var>(c4.lhs().content()).index() == 100);
    assert(std::get<var>(c4.rhs().content()).index() == 200);
    
    // Copy constructor with nested cons
    cons c5(expr{atom{"inner1"}}, expr{atom{"inner2"}});
    cons c6(expr{c5}, expr{atom{"outer"}});
    cons c7(c6);  // Copy nested cons
    
    const cons& orig_inner = std::get<cons>(c6.lhs().content());
    const cons& copy_inner = std::get<cons>(c7.lhs().content());
    
    assert(std::get<atom>(orig_inner.lhs().content()).value() == "inner1");
    assert(std::get<atom>(orig_inner.rhs().content()).value() == "inner2");
    assert(std::get<atom>(copy_inner.lhs().content()).value() == "inner1");
    assert(std::get<atom>(copy_inner.rhs().content()).value() == "inner2");
    assert(std::get<atom>(c6.rhs().content()).value() == "outer");
    assert(std::get<atom>(c7.rhs().content()).value() == "outer");
    
    // Basic copy assignment
    cons c8(expr{atom{"assign_left"}}, expr{atom{"assign_right"}});
    cons c9(expr{atom{"old_left"}}, expr{atom{"old_right"}});
    
    c9 = c8;  // Copy assignment
    
    assert(std::get<atom>(c8.lhs().content()).value() == "assign_left");
    assert(std::get<atom>(c8.rhs().content()).value() == "assign_right");
    assert(std::get<atom>(c9.lhs().content()).value() == "assign_left");
    assert(std::get<atom>(c9.rhs().content()).value() == "assign_right");
    
    // Copy assignment with variables
    cons c10(expr{var{42}}, expr{var{84}});
    cons c11(expr{var{1}}, expr{var{2}});
    
    c11 = c10;
    
    assert(std::get<var>(c10.lhs().content()).index() == 42);
    assert(std::get<var>(c10.rhs().content()).index() == 84);
    assert(std::get<var>(c11.lhs().content()).index() == 42);
    assert(std::get<var>(c11.rhs().content()).index() == 84);
    
    // Copy assignment with nested cons
    cons c12(expr{atom{"nested_a"}}, expr{atom{"nested_b"}});
    cons c13(expr{c12}, expr{var{999}});
    cons c14(expr{atom{"target"}}, expr{atom{"target"}});
    
    c14 = c13;
    
    const cons& assigned_inner = std::get<cons>(c14.lhs().content());
    assert(std::get<atom>(assigned_inner.lhs().content()).value() == "nested_a");
    assert(std::get<atom>(assigned_inner.rhs().content()).value() == "nested_b");
    assert(std::get<var>(c14.rhs().content()).index() == 999);
    
    // Self-assignment
    cons c15(expr{atom{"self"}}, expr{atom{"self"}});
    c15 = c15;
    
    assert(std::get<atom>(c15.lhs().content()).value() == "self");
    assert(std::get<atom>(c15.rhs().content()).value() == "self");
    
    // Chained copy assignment
    cons c16(expr{atom{"chain"}}, expr{atom{"chain"}});
    cons c17(expr{atom{"target1"}}, expr{atom{"target1"}});
    cons c18(expr{atom{"target2"}}, expr{atom{"target2"}});
    
    c18 = c17 = c16;
    
    assert(std::get<atom>(c16.lhs().content()).value() == "chain");
    assert(std::get<atom>(c17.lhs().content()).value() == "chain");
    assert(std::get<atom>(c18.lhs().content()).value() == "chain");
    
    // Multiple copies from same source
    cons c19(expr{atom{"source"}}, expr{atom{"source"}});
    cons c20(c19);
    cons c21(c19);
    cons c22(c19);
    
    assert(std::get<atom>(c20.lhs().content()).value() == "source");
    assert(std::get<atom>(c21.lhs().content()).value() == "source");
    assert(std::get<atom>(c22.lhs().content()).value() == "source");
    
    // Verify independence after copy
    cons c23(expr{atom{"original"}}, expr{atom{"original"}});
    cons c24(c23);
    
    // Modify c23 via assignment
    cons c25(expr{atom{"modified"}}, expr{atom{"modified"}});
    c23 = c25;
    
    // c24 should still have original values
    assert(std::get<atom>(c24.lhs().content()).value() == "original");
    assert(std::get<atom>(c23.lhs().content()).value() == "modified");
}

void test_cons_move() {
    // Basic move constructor
    cons c1(expr{atom{"left"}}, expr{atom{"right"}});
    cons c2(std::move(c1));  // Move constructor
    
    // c2 should have the values
    assert(std::get<atom>(c2.lhs().content()).value() == "left");
    assert(std::get<atom>(c2.rhs().content()).value() == "right");
    
    // Move constructor with variables
    cons c3(expr{var{123}}, expr{var{456}});
    cons c4(std::move(c3));
    
    assert(std::get<var>(c4.lhs().content()).index() == 123);
    assert(std::get<var>(c4.rhs().content()).index() == 456);
    
    // Move constructor with nested cons
    cons c5(expr{atom{"inner_left"}}, expr{atom{"inner_right"}});
    cons c6(expr{c5}, expr{atom{"outer"}});
    cons c7(std::move(c6));
    
    const cons& moved_inner = std::get<cons>(c7.lhs().content());
    assert(std::get<atom>(moved_inner.lhs().content()).value() == "inner_left");
    assert(std::get<atom>(moved_inner.rhs().content()).value() == "inner_right");
    assert(std::get<atom>(c7.rhs().content()).value() == "outer");
    
    // Basic move assignment
    cons c8(expr{atom{"move_left"}}, expr{atom{"move_right"}});
    cons c9(expr{atom{"target_left"}}, expr{atom{"target_right"}});
    
    c9 = std::move(c8);  // Move assignment
    
    assert(std::get<atom>(c9.lhs().content()).value() == "move_left");
    assert(std::get<atom>(c9.rhs().content()).value() == "move_right");
    
    // Move assignment with variables
    cons c10(expr{var{777}}, expr{var{888}});
    cons c11(expr{var{1}}, expr{var{2}});
    
    c11 = std::move(c10);
    
    assert(std::get<var>(c11.lhs().content()).index() == 777);
    assert(std::get<var>(c11.rhs().content()).index() == 888);
    
    // Move assignment with nested cons
    cons c12(expr{atom{"deep_a"}}, expr{atom{"deep_b"}});
    cons c13(expr{c12}, expr{var{555}});
    cons c14(expr{atom{"old"}}, expr{atom{"old"}});
    
    c14 = std::move(c13);
    
    const cons& moved_nested = std::get<cons>(c14.lhs().content());
    assert(std::get<atom>(moved_nested.lhs().content()).value() == "deep_a");
    assert(std::get<atom>(moved_nested.rhs().content()).value() == "deep_b");
    assert(std::get<var>(c14.rhs().content()).index() == 555);
    
    // Self-move (should be safe, though state may be unspecified)
    cons c15(expr{atom{"self"}}, expr{atom{"self"}});
    c15 = std::move(c15);
    // Don't assert on values after self-move as behavior is unspecified
    
    // Move into vector (tests move semantics in container)
    std::vector<cons> vec;
    
    for (int i = 0; i < 10; i++) {
        vec.push_back(cons(expr{atom{"l" + std::to_string(i)}}, expr{atom{"r" + std::to_string(i)}}));
    }
    
    assert(vec.size() == 10);
    assert(std::get<atom>(vec[0].lhs().content()).value() == "l0");
    assert(std::get<atom>(vec[0].rhs().content()).value() == "r0");
    assert(std::get<atom>(vec[5].lhs().content()).value() == "l5");
    assert(std::get<atom>(vec[9].rhs().content()).value() == "r9");
    
    // Move from vector to vector
    std::vector<cons> vec2;
    for (auto& c : vec) {
        vec2.push_back(std::move(c));
    }
    
    assert(vec2.size() == 10);
    assert(std::get<atom>(vec2[0].lhs().content()).value() == "l0");
    assert(std::get<atom>(vec2[5].lhs().content()).value() == "l5");
    
    // Chained move assignment
    cons c16(expr{atom{"chain_source"}}, expr{atom{"chain_source"}});
    cons c17(expr{atom{"target1"}}, expr{atom{"target1"}});
    cons c18(expr{atom{"target2"}}, expr{atom{"target2"}});
    
    c18 = std::move(c17 = std::move(c16));
    
    assert(std::get<atom>(c18.lhs().content()).value() == "chain_source");
    
    // Move with mixed types
    cons c19(expr{atom{"atom_val"}}, expr{var{999}});
    cons c20(std::move(c19));
    
    assert(std::get<atom>(c20.lhs().content()).value() == "atom_val");
    assert(std::get<var>(c20.rhs().content()).index() == 999);
    
    // Move deeply nested structure
    cons c21(expr{atom{"level3"}}, expr{atom{"level3"}});
    cons c22(expr{c21}, expr{c21});
    cons c23(expr{c22}, expr{c22});
    cons c24(std::move(c23));
    
    const cons& level1 = std::get<cons>(c24.lhs().content());
    const cons& level2 = std::get<cons>(level1.lhs().content());
    assert(std::get<atom>(level2.lhs().content()).value() == "level3");
}

void test_expr_constructor() {
    // Expr with atom
    expr e1{atom{"test_atom"}};
    
    assert(std::holds_alternative<atom>(e1.content()));
    assert(std::get<atom>(e1.content()).value() == "test_atom");
    
    // Expr with empty string atom
    expr e2{atom{""}};
    
    assert(std::holds_alternative<atom>(e2.content()));
    assert(std::get<atom>(e2.content()).value() == "");
    
    // Expr with special character atom
    expr e3{atom{"!@#$%^&*()"}};
    
    assert(std::holds_alternative<atom>(e3.content()));
    assert(std::get<atom>(e3.content()).value() == "!@#$%^&*()");
    
    // Expr with var - index 0
    expr e4{var{0}};
    
    assert(std::holds_alternative<var>(e4.content()));
    assert(std::get<var>(e4.content()).index() == 0);
    
    // Expr with var - various indices
    expr e5{var{42}};
    
    assert(std::holds_alternative<var>(e5.content()));
    assert(std::get<var>(e5.content()).index() == 42);
    
    expr e6{var{UINT32_MAX}};
    
    assert(std::holds_alternative<var>(e6.content()));
    assert(std::get<var>(e6.content()).index() == UINT32_MAX);
    
    // Expr with cons - simple
    expr e9{cons{expr{atom{"left"}}, expr{atom{"right"}}}};
    
    assert(std::holds_alternative<cons>(e9.content()));
    const cons& c1_ref = std::get<cons>(e9.content());
    assert(std::get<atom>(c1_ref.lhs().content()).value() == "left");
    assert(std::get<atom>(c1_ref.rhs().content()).value() == "right");
    
    // Expr with cons containing vars
    expr e12{cons{expr{var{10}}, expr{var{20}}}};
    
    assert(std::holds_alternative<cons>(e12.content()));
    const cons& c2_ref = std::get<cons>(e12.content());
    assert(std::get<var>(c2_ref.lhs().content()).index() == 10);
    assert(std::get<var>(c2_ref.rhs().content()).index() == 20);
    
    // Expr with cons containing mixed types
    expr e15{cons{expr{atom{"atom"}}, expr{var{100}}}};
    
    assert(std::holds_alternative<cons>(e15.content()));
    const cons& c3_ref = std::get<cons>(e15.content());
    assert(std::get<atom>(c3_ref.lhs().content()).value() == "atom");
    assert(std::get<var>(c3_ref.rhs().content()).index() == 100);
    
    // Expr with nested cons
    expr e20{cons{
        expr{cons{expr{atom{"inner1"}}, expr{atom{"inner2"}}}},
        expr{atom{"outer"}}
    }};
    
    assert(std::holds_alternative<cons>(e20.content()));
    const cons& outer_cons = std::get<cons>(e20.content());
    assert(std::holds_alternative<cons>(outer_cons.lhs().content()));
    const cons& inner_cons = std::get<cons>(outer_cons.lhs().content());
    assert(std::get<atom>(inner_cons.lhs().content()).value() == "inner1");
    assert(std::get<atom>(inner_cons.rhs().content()).value() == "inner2");
    assert(std::get<atom>(outer_cons.rhs().content()).value() == "outer");
    
    // Multiple exprs with same atom
    atom a10("shared");
    expr e21{a10};
    expr e22{a10};
    
    assert(std::get<atom>(e21.content()).value() == "shared");
    assert(std::get<atom>(e22.content()).value() == "shared");
    
    // Multiple exprs with same var
    var v7(999);
    expr e23{v7};
    expr e24{v7};
    
    assert(std::get<var>(e23.content()).index() == 999);
    assert(std::get<var>(e24.content()).index() == 999);
    
    // Test all three types in sequence
    expr e27{atom{"atom_type"}};
    expr e28{var{42}};
    expr e29{cons{expr{atom{"cons_left"}}, expr{atom{"cons_right"}}}};
    
    assert(std::holds_alternative<atom>(e27.content()));
    assert(std::holds_alternative<var>(e28.content()));
    assert(std::holds_alternative<cons>(e29.content()));
    
    // Deeply nested structure (3 levels)
    expr e33{cons{
        expr{cons{
            expr{cons{expr{atom{"deep"}}, expr{atom{"deep"}}}},
            expr{cons{expr{atom{"deep"}}, expr{atom{"deep"}}}}
        }},
        expr{cons{
            expr{cons{expr{atom{"deep"}}, expr{atom{"deep"}}}},
            expr{cons{expr{atom{"deep"}}, expr{atom{"deep"}}}}
        }}
    }};
    
    assert(std::holds_alternative<cons>(e33.content()));
    const cons& level1 = std::get<cons>(e33.content());
    const cons& level2 = std::get<cons>(level1.lhs().content());
    const cons& level3 = std::get<cons>(level2.lhs().content());
    assert(std::get<atom>(level3.lhs().content()).value() == "deep");
    
    // Vector of exprs with different types
    std::vector<expr> exprs;
    
    exprs.push_back(expr{atom{"vec_atom"}});
    exprs.push_back(expr{var{123}});
    exprs.push_back(expr{cons{expr{atom{"l"}}, expr{atom{"r"}}}});
    
    assert(exprs.size() == 3);
    assert(std::holds_alternative<atom>(exprs[0].content()));
    assert(std::holds_alternative<var>(exprs[1].content()));
    assert(std::holds_alternative<cons>(exprs[2].content()));
    
    assert(std::get<atom>(exprs[0].content()).value() == "vec_atom");
    assert(std::get<var>(exprs[1].content()).index() == 123);
    assert(std::get<atom>(std::get<cons>(exprs[2].content()).lhs().content()).value() == "l");
}

void unit_test_main() {
    constexpr bool ENABLE_DEBUG_LOGS = true;

    // test cases
    TEST(test_atom_constructor);
    TEST(test_var_constructor);
    TEST(test_cons_constructor);
    TEST(test_cons_copy);
    TEST(test_cons_move);
    TEST(test_expr_constructor);
}

int main() {
    unit_test_main();
    return 0;
}
