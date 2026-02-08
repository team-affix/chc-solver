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
    atom a1("left");
    atom a2("right");
    expr e1{std::variant<atom, cons, var>{a1}};
    expr e2{std::variant<atom, cons, var>{a2}};
    cons c1(e1, e2);
    
    // Verify lhs and rhs
    const expr& lhs1 = c1.lhs();
    const expr& rhs1 = c1.rhs();
    assert(std::get<atom>(lhs1.content()).value() == "left");
    assert(std::get<atom>(rhs1.content()).value() == "right");
    
    // Cons with variables
    var v1(0);
    var v2(1);
    expr e3{std::variant<atom, cons, var>{v1}};
    expr e4{std::variant<atom, cons, var>{v2}};
    cons c2(e3, e4);
    
    assert(std::get<var>(c2.lhs().content()).index() == 0);
    assert(std::get<var>(c2.rhs().content()).index() == 1);
    
    // Cons with mixed types (atom and var)
    atom a3("atom");
    var v3(42);
    expr e5{std::variant<atom, cons, var>{a3}};
    expr e6{std::variant<atom, cons, var>{v3}};
    cons c3(e5, e6);
    
    assert(std::get<atom>(c3.lhs().content()).value() == "atom");
    assert(std::get<var>(c3.rhs().content()).index() == 42);
    
    // Cons with same expr on both sides
    atom a4("same");
    expr e7{std::variant<atom, cons, var>{a4}};
    cons c4(e7, e7);
    
    assert(std::get<atom>(c4.lhs().content()).value() == "same");
    assert(std::get<atom>(c4.rhs().content()).value() == "same");
    
    // Nested cons - cons on left
    atom a5("inner_left");
    atom a6("inner_right");
    expr e8{std::variant<atom, cons, var>{a5}};
    expr e9{std::variant<atom, cons, var>{a6}};
    cons c5(e8, e9);
    
    expr e10{std::variant<atom, cons, var>{c5}};
    atom a7("outer_right");
    expr e11{std::variant<atom, cons, var>{a7}};
    cons c6(e10, e11);
    
    const cons& inner_left = std::get<cons>(c6.lhs().content());
    assert(std::get<atom>(inner_left.lhs().content()).value() == "inner_left");
    assert(std::get<atom>(inner_left.rhs().content()).value() == "inner_right");
    assert(std::get<atom>(c6.rhs().content()).value() == "outer_right");
    
    // Nested cons - cons on right
    atom a8("outer_left");
    expr e12{std::variant<atom, cons, var>{a8}};
    expr e13{std::variant<atom, cons, var>{c5}};
    cons c7(e12, e13);
    
    assert(std::get<atom>(c7.lhs().content()).value() == "outer_left");
    const cons& inner_right = std::get<cons>(c7.rhs().content());
    assert(std::get<atom>(inner_right.lhs().content()).value() == "inner_left");
    assert(std::get<atom>(inner_right.rhs().content()).value() == "inner_right");
    
    // Nested cons - cons on both sides
    atom a9("left1");
    atom a10("left2");
    expr e14{std::variant<atom, cons, var>{a9}};
    expr e15{std::variant<atom, cons, var>{a10}};
    cons c8(e14, e15);
    
    atom a11("right1");
    atom a12("right2");
    expr e16{std::variant<atom, cons, var>{a11}};
    expr e17{std::variant<atom, cons, var>{a12}};
    cons c9(e16, e17);
    
    expr e18{std::variant<atom, cons, var>{c8}};
    expr e19{std::variant<atom, cons, var>{c9}};
    cons c10(e18, e19);
    
    const cons& left_cons = std::get<cons>(c10.lhs().content());
    const cons& right_cons = std::get<cons>(c10.rhs().content());
    assert(std::get<atom>(left_cons.lhs().content()).value() == "left1");
    assert(std::get<atom>(left_cons.rhs().content()).value() == "left2");
    assert(std::get<atom>(right_cons.lhs().content()).value() == "right1");
    assert(std::get<atom>(right_cons.rhs().content()).value() == "right2");
    
    // Deep nesting (3 levels)
    expr e20{std::variant<atom, cons, var>{c10}};
    var v4(999);
    expr e21{std::variant<atom, cons, var>{v4}};
    cons c11(e20, e21);
    
    const cons& level2 = std::get<cons>(c11.lhs().content());
    const cons& level2_left = std::get<cons>(level2.lhs().content());
    assert(std::get<atom>(level2_left.lhs().content()).value() == "left1");
    assert(std::get<var>(c11.rhs().content()).index() == 999);
    
    // Empty string atoms in cons
    atom a13("");
    atom a14("");
    expr e22{std::variant<atom, cons, var>{a13}};
    expr e23{std::variant<atom, cons, var>{a14}};
    cons c12(e22, e23);
    
    assert(std::get<atom>(c12.lhs().content()).value() == "");
    assert(std::get<atom>(c12.rhs().content()).value() == "");
}

void test_cons_copy() {
    // Basic copy constructor
    atom a1("left");
    atom a2("right");
    expr e1{std::variant<atom, cons, var>{a1}};
    expr e2{std::variant<atom, cons, var>{a2}};
    cons c1(e1, e2);
    
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
    var v1(100);
    var v2(200);
    expr e3{std::variant<atom, cons, var>{v1}};
    expr e4{std::variant<atom, cons, var>{v2}};
    cons c3(e3, e4);
    cons c4(c3);
    
    assert(std::get<var>(c3.lhs().content()).index() == 100);
    assert(std::get<var>(c3.rhs().content()).index() == 200);
    assert(std::get<var>(c4.lhs().content()).index() == 100);
    assert(std::get<var>(c4.rhs().content()).index() == 200);
    
    // Copy constructor with nested cons
    atom a3("inner1");
    atom a4("inner2");
    expr e5{std::variant<atom, cons, var>{a3}};
    expr e6{std::variant<atom, cons, var>{a4}};
    cons c5(e5, e6);
    
    expr e7{std::variant<atom, cons, var>{c5}};
    atom a5("outer");
    expr e8{std::variant<atom, cons, var>{a5}};
    cons c6(e7, e8);
    
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
    atom a6("assign_left");
    atom a7("assign_right");
    expr e9{std::variant<atom, cons, var>{a6}};
    expr e10{std::variant<atom, cons, var>{a7}};
    cons c8(e9, e10);
    
    atom a8("old_left");
    atom a9("old_right");
    expr e11{std::variant<atom, cons, var>{a8}};
    expr e12{std::variant<atom, cons, var>{a9}};
    cons c9(e11, e12);
    
    c9 = c8;  // Copy assignment
    
    assert(std::get<atom>(c8.lhs().content()).value() == "assign_left");
    assert(std::get<atom>(c8.rhs().content()).value() == "assign_right");
    assert(std::get<atom>(c9.lhs().content()).value() == "assign_left");
    assert(std::get<atom>(c9.rhs().content()).value() == "assign_right");
    
    // Copy assignment with variables
    var v3(42);
    var v4(84);
    expr e13{std::variant<atom, cons, var>{v3}};
    expr e14{std::variant<atom, cons, var>{v4}};
    cons c10(e13, e14);
    
    var v5(1);
    var v6(2);
    expr e15{std::variant<atom, cons, var>{v5}};
    expr e16{std::variant<atom, cons, var>{v6}};
    cons c11(e15, e16);
    
    c11 = c10;
    
    assert(std::get<var>(c10.lhs().content()).index() == 42);
    assert(std::get<var>(c10.rhs().content()).index() == 84);
    assert(std::get<var>(c11.lhs().content()).index() == 42);
    assert(std::get<var>(c11.rhs().content()).index() == 84);
    
    // Copy assignment with nested cons
    atom a10("nested_a");
    atom a11("nested_b");
    expr e17{std::variant<atom, cons, var>{a10}};
    expr e18{std::variant<atom, cons, var>{a11}};
    cons c12(e17, e18);
    
    expr e19{std::variant<atom, cons, var>{c12}};
    var v7(999);
    expr e20{std::variant<atom, cons, var>{v7}};
    cons c13(e19, e20);
    
    atom a12("target");
    expr e21{std::variant<atom, cons, var>{a12}};
    cons c14(e21, e21);
    
    c14 = c13;
    
    const cons& assigned_inner = std::get<cons>(c14.lhs().content());
    assert(std::get<atom>(assigned_inner.lhs().content()).value() == "nested_a");
    assert(std::get<atom>(assigned_inner.rhs().content()).value() == "nested_b");
    assert(std::get<var>(c14.rhs().content()).index() == 999);
    
    // Self-assignment
    atom a13("self");
    expr e22{std::variant<atom, cons, var>{a13}};
    cons c15(e22, e22);
    c15 = c15;
    
    assert(std::get<atom>(c15.lhs().content()).value() == "self");
    assert(std::get<atom>(c15.rhs().content()).value() == "self");
    
    // Chained copy assignment
    atom a14("chain");
    expr e23{std::variant<atom, cons, var>{a14}};
    cons c16(e23, e23);
    
    atom a15("target1");
    expr e24{std::variant<atom, cons, var>{a15}};
    cons c17(e24, e24);
    
    atom a16("target2");
    expr e25{std::variant<atom, cons, var>{a16}};
    cons c18(e25, e25);
    
    c18 = c17 = c16;
    
    assert(std::get<atom>(c16.lhs().content()).value() == "chain");
    assert(std::get<atom>(c17.lhs().content()).value() == "chain");
    assert(std::get<atom>(c18.lhs().content()).value() == "chain");
    
    // Multiple copies from same source
    atom a17("source");
    expr e26{std::variant<atom, cons, var>{a17}};
    cons c19(e26, e26);
    
    cons c20(c19);
    cons c21(c19);
    cons c22(c19);
    
    assert(std::get<atom>(c20.lhs().content()).value() == "source");
    assert(std::get<atom>(c21.lhs().content()).value() == "source");
    assert(std::get<atom>(c22.lhs().content()).value() == "source");
    
    // Verify independence after copy
    atom a18("original");
    expr e27{std::variant<atom, cons, var>{a18}};
    cons c23(e27, e27);
    cons c24(c23);
    
    // Modify c23 via assignment
    atom a19("modified");
    expr e28{std::variant<atom, cons, var>{a19}};
    cons c25(e28, e28);
    c23 = c25;
    
    // c24 should still have original values
    assert(std::get<atom>(c24.lhs().content()).value() == "original");
    assert(std::get<atom>(c23.lhs().content()).value() == "modified");
}

void test_cons_move() {
    // Basic move constructor
    atom a1("left");
    atom a2("right");
    expr e1{std::variant<atom, cons, var>{a1}};
    expr e2{std::variant<atom, cons, var>{a2}};
    cons c1(e1, e2);
    
    cons c2(std::move(c1));  // Move constructor
    
    // c2 should have the values
    assert(std::get<atom>(c2.lhs().content()).value() == "left");
    assert(std::get<atom>(c2.rhs().content()).value() == "right");
    
    // Move constructor with variables
    var v1(123);
    var v2(456);
    expr e3{std::variant<atom, cons, var>{v1}};
    expr e4{std::variant<atom, cons, var>{v2}};
    cons c3(e3, e4);
    
    cons c4(std::move(c3));
    
    assert(std::get<var>(c4.lhs().content()).index() == 123);
    assert(std::get<var>(c4.rhs().content()).index() == 456);
    
    // Move constructor with nested cons
    atom a3("inner_left");
    atom a4("inner_right");
    expr e5{std::variant<atom, cons, var>{a3}};
    expr e6{std::variant<atom, cons, var>{a4}};
    cons c5(e5, e6);
    
    expr e7{std::variant<atom, cons, var>{c5}};
    atom a5("outer");
    expr e8{std::variant<atom, cons, var>{a5}};
    cons c6(e7, e8);
    
    cons c7(std::move(c6));
    
    const cons& moved_inner = std::get<cons>(c7.lhs().content());
    assert(std::get<atom>(moved_inner.lhs().content()).value() == "inner_left");
    assert(std::get<atom>(moved_inner.rhs().content()).value() == "inner_right");
    assert(std::get<atom>(c7.rhs().content()).value() == "outer");
    
    // Basic move assignment
    atom a6("move_left");
    atom a7("move_right");
    expr e9{std::variant<atom, cons, var>{a6}};
    expr e10{std::variant<atom, cons, var>{a7}};
    cons c8(e9, e10);
    
    atom a8("target_left");
    atom a9("target_right");
    expr e11{std::variant<atom, cons, var>{a8}};
    expr e12{std::variant<atom, cons, var>{a9}};
    cons c9(e11, e12);
    
    c9 = std::move(c8);  // Move assignment
    
    assert(std::get<atom>(c9.lhs().content()).value() == "move_left");
    assert(std::get<atom>(c9.rhs().content()).value() == "move_right");
    
    // Move assignment with variables
    var v3(777);
    var v4(888);
    expr e13{std::variant<atom, cons, var>{v3}};
    expr e14{std::variant<atom, cons, var>{v4}};
    cons c10(e13, e14);
    
    var v5(1);
    var v6(2);
    expr e15{std::variant<atom, cons, var>{v5}};
    expr e16{std::variant<atom, cons, var>{v6}};
    cons c11(e15, e16);
    
    c11 = std::move(c10);
    
    assert(std::get<var>(c11.lhs().content()).index() == 777);
    assert(std::get<var>(c11.rhs().content()).index() == 888);
    
    // Move assignment with nested cons
    atom a10("deep_a");
    atom a11("deep_b");
    expr e17{std::variant<atom, cons, var>{a10}};
    expr e18{std::variant<atom, cons, var>{a11}};
    cons c12(e17, e18);
    
    expr e19{std::variant<atom, cons, var>{c12}};
    var v7(555);
    expr e20{std::variant<atom, cons, var>{v7}};
    cons c13(e19, e20);
    
    atom a12("old");
    expr e21{std::variant<atom, cons, var>{a12}};
    cons c14(e21, e21);
    
    c14 = std::move(c13);
    
    const cons& moved_nested = std::get<cons>(c14.lhs().content());
    assert(std::get<atom>(moved_nested.lhs().content()).value() == "deep_a");
    assert(std::get<atom>(moved_nested.rhs().content()).value() == "deep_b");
    assert(std::get<var>(c14.rhs().content()).index() == 555);
    
    // Self-move (should be safe, though state may be unspecified)
    atom a13("self");
    expr e22{std::variant<atom, cons, var>{a13}};
    cons c15(e22, e22);
    c15 = std::move(c15);
    // Don't assert on values after self-move as behavior is unspecified
    
    // Move into vector (tests move semantics in container)
    std::vector<cons> vec;
    
    for (int i = 0; i < 10; i++) {
        atom left("l" + std::to_string(i));
        atom right("r" + std::to_string(i));
        expr el{std::variant<atom, cons, var>{left}};
        expr er{std::variant<atom, cons, var>{right}};
        vec.push_back(cons(el, er));  // Move temporary into vector
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
    atom a14("chain_source");
    expr e23{std::variant<atom, cons, var>{a14}};
    cons c16(e23, e23);
    
    atom a15("target1");
    expr e24{std::variant<atom, cons, var>{a15}};
    cons c17(e24, e24);
    
    atom a16("target2");
    expr e25{std::variant<atom, cons, var>{a16}};
    cons c18(e25, e25);
    
    c18 = std::move(c17 = std::move(c16));
    
    assert(std::get<atom>(c18.lhs().content()).value() == "chain_source");
    
    // Move with mixed types
    atom a17("atom_val");
    var v8(999);
    expr e26{std::variant<atom, cons, var>{a17}};
    expr e27{std::variant<atom, cons, var>{v8}};
    cons c19(e26, e27);
    
    cons c20(std::move(c19));
    
    assert(std::get<atom>(c20.lhs().content()).value() == "atom_val");
    assert(std::get<var>(c20.rhs().content()).index() == 999);
    
    // Move deeply nested structure
    atom a18("level3");
    expr e28{std::variant<atom, cons, var>{a18}};
    cons c21(e28, e28);
    
    expr e29{std::variant<atom, cons, var>{c21}};
    cons c22(e29, e29);
    
    expr e30{std::variant<atom, cons, var>{c22}};
    cons c23(e30, e30);
    
    cons c24(std::move(c23));
    
    const cons& level1 = std::get<cons>(c24.lhs().content());
    const cons& level2 = std::get<cons>(level1.lhs().content());
    assert(std::get<atom>(level2.lhs().content()).value() == "level3");
}

void test_expr_constructor() {
    // Expr with atom
    atom a1("test_atom");
    expr e1{std::variant<atom, cons, var>{a1}};
    
    assert(std::holds_alternative<atom>(e1.content()));
    assert(std::get<atom>(e1.content()).value() == "test_atom");
    
    // Expr with empty string atom
    atom a2("");
    expr e2{std::variant<atom, cons, var>{a2}};
    
    assert(std::holds_alternative<atom>(e2.content()));
    assert(std::get<atom>(e2.content()).value() == "");
    
    // Expr with special character atom
    atom a3("!@#$%^&*()");
    expr e3{std::variant<atom, cons, var>{a3}};
    
    assert(std::holds_alternative<atom>(e3.content()));
    assert(std::get<atom>(e3.content()).value() == "!@#$%^&*()");
    
    // Expr with var - index 0
    var v1(0);
    expr e4{std::variant<atom, cons, var>{v1}};
    
    assert(std::holds_alternative<var>(e4.content()));
    assert(std::get<var>(e4.content()).index() == 0);
    
    // Expr with var - various indices
    var v2(42);
    expr e5{std::variant<atom, cons, var>{v2}};
    
    assert(std::holds_alternative<var>(e5.content()));
    assert(std::get<var>(e5.content()).index() == 42);
    
    var v3(UINT32_MAX);
    expr e6{std::variant<atom, cons, var>{v3}};
    
    assert(std::holds_alternative<var>(e6.content()));
    assert(std::get<var>(e6.content()).index() == UINT32_MAX);
    
    // Expr with cons - simple
    atom a4("left");
    atom a5("right");
    expr e7{std::variant<atom, cons, var>{a4}};
    expr e8{std::variant<atom, cons, var>{a5}};
    cons c1(e7, e8);
    expr e9{std::variant<atom, cons, var>{c1}};
    
    assert(std::holds_alternative<cons>(e9.content()));
    const cons& c1_ref = std::get<cons>(e9.content());
    assert(std::get<atom>(c1_ref.lhs().content()).value() == "left");
    assert(std::get<atom>(c1_ref.rhs().content()).value() == "right");
    
    // Expr with cons containing vars
    var v4(10);
    var v5(20);
    expr e10{std::variant<atom, cons, var>{v4}};
    expr e11{std::variant<atom, cons, var>{v5}};
    cons c2(e10, e11);
    expr e12{std::variant<atom, cons, var>{c2}};
    
    assert(std::holds_alternative<cons>(e12.content()));
    const cons& c2_ref = std::get<cons>(e12.content());
    assert(std::get<var>(c2_ref.lhs().content()).index() == 10);
    assert(std::get<var>(c2_ref.rhs().content()).index() == 20);
    
    // Expr with cons containing mixed types
    atom a6("atom");
    var v6(100);
    expr e13{std::variant<atom, cons, var>{a6}};
    expr e14{std::variant<atom, cons, var>{v6}};
    cons c3(e13, e14);
    expr e15{std::variant<atom, cons, var>{c3}};
    
    assert(std::holds_alternative<cons>(e15.content()));
    const cons& c3_ref = std::get<cons>(e15.content());
    assert(std::get<atom>(c3_ref.lhs().content()).value() == "atom");
    assert(std::get<var>(c3_ref.rhs().content()).index() == 100);
    
    // Expr with nested cons
    atom a7("inner1");
    atom a8("inner2");
    expr e16{std::variant<atom, cons, var>{a7}};
    expr e17{std::variant<atom, cons, var>{a8}};
    cons c4(e16, e17);
    
    expr e18{std::variant<atom, cons, var>{c4}};
    atom a9("outer");
    expr e19{std::variant<atom, cons, var>{a9}};
    cons c5(e18, e19);
    
    expr e20{std::variant<atom, cons, var>{c5}};
    
    assert(std::holds_alternative<cons>(e20.content()));
    const cons& outer_cons = std::get<cons>(e20.content());
    assert(std::holds_alternative<cons>(outer_cons.lhs().content()));
    const cons& inner_cons = std::get<cons>(outer_cons.lhs().content());
    assert(std::get<atom>(inner_cons.lhs().content()).value() == "inner1");
    assert(std::get<atom>(inner_cons.rhs().content()).value() == "inner2");
    assert(std::get<atom>(outer_cons.rhs().content()).value() == "outer");
    
    // Multiple exprs with same atom
    atom a10("shared");
    expr e21{std::variant<atom, cons, var>{a10}};
    expr e22{std::variant<atom, cons, var>{a10}};
    
    assert(std::get<atom>(e21.content()).value() == "shared");
    assert(std::get<atom>(e22.content()).value() == "shared");
    
    // Multiple exprs with same var
    var v7(999);
    expr e23{std::variant<atom, cons, var>{v7}};
    expr e24{std::variant<atom, cons, var>{v7}};
    
    assert(std::get<var>(e23.content()).index() == 999);
    assert(std::get<var>(e24.content()).index() == 999);
    
    // Test all three types in sequence
    atom a11("atom_type");
    var v8(42);
    
    atom a12("cons_left");
    atom a13("cons_right");
    expr e25{std::variant<atom, cons, var>{a12}};
    expr e26{std::variant<atom, cons, var>{a13}};
    cons c6(e25, e26);
    
    expr e27{std::variant<atom, cons, var>{a11}};
    expr e28{std::variant<atom, cons, var>{v8}};
    expr e29{std::variant<atom, cons, var>{c6}};
    
    assert(std::holds_alternative<atom>(e27.content()));
    assert(std::holds_alternative<var>(e28.content()));
    assert(std::holds_alternative<cons>(e29.content()));
    
    // Deeply nested structure (3 levels)
    atom a14("deep");
    expr e30{std::variant<atom, cons, var>{a14}};
    cons c7(e30, e30);
    
    expr e31{std::variant<atom, cons, var>{c7}};
    cons c8(e31, e31);
    
    expr e32{std::variant<atom, cons, var>{c8}};
    cons c9(e32, e32);
    
    expr e33{std::variant<atom, cons, var>{c9}};
    
    assert(std::holds_alternative<cons>(e33.content()));
    const cons& level1 = std::get<cons>(e33.content());
    const cons& level2 = std::get<cons>(level1.lhs().content());
    const cons& level3 = std::get<cons>(level2.lhs().content());
    assert(std::get<atom>(level3.lhs().content()).value() == "deep");
    
    // Vector of exprs with different types
    std::vector<expr> exprs;
    
    atom a15("vec_atom");
    exprs.push_back(expr{std::variant<atom, cons, var>{a15}});
    
    var v9(123);
    exprs.push_back(expr{std::variant<atom, cons, var>{v9}});
    
    atom a16("l");
    atom a17("r");
    expr el{std::variant<atom, cons, var>{a16}};
    expr er{std::variant<atom, cons, var>{a17}};
    cons c10(el, er);
    exprs.push_back(expr{std::variant<atom, cons, var>{c10}});
    
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

