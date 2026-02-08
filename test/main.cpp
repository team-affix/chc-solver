#include "../hpp/expr.hpp"
#include <iostream>
#include <list>
#include <vector>
#include "test_utils.hpp"

void test_atom_constructor() {
    // Basic construction with normal string
    atom a1("hello");
    
    // Empty string - should work
    atom a2("");
    
    // String with special characters
    atom a3("test_123");
    atom a4("with spaces");
    atom a5("special!@#$%^&*()");
    atom a6("unicode_αβγδ");
    
    // Long string
    atom a7("this_is_a_very_long_string_that_might_test_memory_allocation_and_string_handling_with_many_characters");
    
    // Single character
    atom a8("x");
    
    // Numbers as strings
    atom a9("12345");
    atom a10("0");
    
    // Whitespace variations
    atom a11(" ");
    atom a12("\t");
    atom a13("\n");
    
    // Multiple atoms with same value (test string copying)
    atom a14("duplicate");
    atom a15("duplicate");
    
    // Test that construction doesn't throw
    std::vector<atom> atoms;
    for (int i = 0; i < 100; i++) {
        atoms.push_back(atom("atom_" + std::to_string(i)));
    }
    assert(atoms.size() == 100);
}

void test_var_constructor() {
    // Basic construction with various indices
    var v1(0);
    var v2(1);
    var v3(100);
    var v4(999);
    var v5(1000000);
    
    // Edge cases
    var v6(UINT32_MAX);  // Maximum value
    
    // Multiple vars with same index
    var v7(42);
    var v8(42);
    
    // Test bulk construction
    std::vector<var> vars;
    for (uint32_t i = 0; i < 1000; i++) {
        vars.push_back(var(i));
    }
    assert(vars.size() == 1000);
}

void test_cons_constructor() {
    // Basic cons construction with atoms
    atom a1("left");
    atom a2("right");
    expr e1{std::variant<atom, cons, var>{a1}};
    expr e2{std::variant<atom, cons, var>{a2}};
    cons c1(e1, e2);
    
    // Cons with variables
    var v1(0);
    var v2(1);
    expr e3{std::variant<atom, cons, var>{v1}};
    expr e4{std::variant<atom, cons, var>{v2}};
    cons c2(e3, e4);
    
    // Cons with mixed types
    atom a3("atom");
    var v3(5);
    expr e5{std::variant<atom, cons, var>{a3}};
    expr e6{std::variant<atom, cons, var>{v3}};
    cons c3(e5, e6);
    
    // Nested cons (cons of cons) - left side
    expr e7{std::variant<atom, cons, var>{c1}};
    atom a4("right_atom");
    expr e8{std::variant<atom, cons, var>{a4}};
    cons c4(e7, e8);
    
    // Nested cons (cons of cons) - right side
    atom a5("left_atom");
    expr e9{std::variant<atom, cons, var>{a5}};
    expr e10{std::variant<atom, cons, var>{c1}};
    cons c5(e9, e10);
    
    // Nested cons (cons of cons) - both sides
    expr e11{std::variant<atom, cons, var>{c1}};
    expr e12{std::variant<atom, cons, var>{c2}};
    cons c6(e11, e12);
    
    // Deep nesting (3 levels)
    expr e13{std::variant<atom, cons, var>{c6}};
    atom a6("deep");
    expr e14{std::variant<atom, cons, var>{a6}};
    cons c7(e13, e14);
    
    // Very deep nesting (4 levels)
    expr e15{std::variant<atom, cons, var>{c7}};
    var v4(99);
    expr e16{std::variant<atom, cons, var>{v4}};
    cons c8(e15, e16);
    
    // Same expr used for both sides
    atom a7("same");
    expr e17{std::variant<atom, cons, var>{a7}};
    cons c9(e17, e17);
    
    // Test bulk construction
    std::vector<cons> conses;
    for (int i = 0; i < 50; i++) {
        atom left("left_" + std::to_string(i));
        atom right("right_" + std::to_string(i));
        expr el{std::variant<atom, cons, var>{left}};
        expr er{std::variant<atom, cons, var>{right}};
        conses.push_back(cons(el, er));
    }
    assert(conses.size() == 50);
}

void test_cons_copy() {
    // Test copy constructor - basic
    atom a1("left");
    atom a2("right");
    expr e1{std::variant<atom, cons, var>{a1}};
    expr e2{std::variant<atom, cons, var>{a2}};
    cons c1(e1, e2);
    
    cons c2(c1);  // Copy constructor
    
    // Test copy constructor - nested
    expr e3{std::variant<atom, cons, var>{c1}};
    atom a3("nested");
    expr e4{std::variant<atom, cons, var>{a3}};
    cons c3(e3, e4);
    
    cons c4(c3);  // Copy constructor with nested cons
    
    // Test copy assignment - basic
    atom a4("another");
    atom a5("pair");
    expr e5{std::variant<atom, cons, var>{a4}};
    expr e6{std::variant<atom, cons, var>{a5}};
    cons c5(e5, e6);
    
    cons c6(e5, e6);
    c6 = c1;  // Copy assignment
    
    // Test copy assignment - nested
    cons c7(e5, e6);
    c7 = c3;  // Copy assignment with nested cons
    
    // Test self-assignment
    c1 = c1;
    
    // Test chained copy assignment
    atom a6("chain1");
    atom a7("chain2");
    expr e7{std::variant<atom, cons, var>{a6}};
    expr e8{std::variant<atom, cons, var>{a7}};
    cons c8(e7, e8);
    cons c9(e7, e8);
    cons c10(e7, e8);
    
    c10 = c9 = c8;
    
    // Test multiple copies of same object
    cons c11(c1);
    cons c12(c1);
    cons c13(c1);
    
    // Test copy then modify original (ensure deep copy)
    atom a8("original");
    atom a9("data");
    expr e9{std::variant<atom, cons, var>{a8}};
    expr e10{std::variant<atom, cons, var>{a9}};
    cons c14(e9, e10);
    cons c15(c14);
    
    // Reassign c14
    atom a10("new");
    atom a11("values");
    expr e11{std::variant<atom, cons, var>{a10}};
    expr e12{std::variant<atom, cons, var>{a11}};
    cons c16(e11, e12);
    c14 = c16;
    
    // c15 should still be valid (deep copy)
    
    // Test bulk copy operations
    std::vector<cons> originals;
    std::vector<cons> copies;
    for (int i = 0; i < 20; i++) {
        atom left("l" + std::to_string(i));
        atom right("r" + std::to_string(i));
        expr el{std::variant<atom, cons, var>{left}};
        expr er{std::variant<atom, cons, var>{right}};
        originals.push_back(cons(el, er));
    }
    
    for (const auto& orig : originals) {
        copies.push_back(cons(orig));
    }
    assert(copies.size() == 20);
}

void test_cons_move() {
    // Test move constructor - basic
    atom a1("left");
    atom a2("right");
    expr e1{std::variant<atom, cons, var>{a1}};
    expr e2{std::variant<atom, cons, var>{a2}};
    cons c1(e1, e2);
    
    cons c2(std::move(c1));  // Move constructor
    
    // Test move constructor - nested
    atom a3("nested_left");
    atom a4("nested_right");
    expr e3{std::variant<atom, cons, var>{a3}};
    expr e4{std::variant<atom, cons, var>{a4}};
    cons c3(e3, e4);
    
    expr e5{std::variant<atom, cons, var>{c3}};
    atom a5("outer");
    expr e6{std::variant<atom, cons, var>{a5}};
    cons c4(e5, e6);
    
    cons c5(std::move(c4));  // Move constructor with nested cons
    
    // Test move assignment - basic
    atom a6("move1");
    atom a7("move2");
    expr e7{std::variant<atom, cons, var>{a6}};
    expr e8{std::variant<atom, cons, var>{a7}};
    cons c6(e7, e8);
    
    atom a8("target1");
    atom a9("target2");
    expr e9{std::variant<atom, cons, var>{a8}};
    expr e10{std::variant<atom, cons, var>{a9}};
    cons c7(e9, e10);
    
    c7 = std::move(c6);  // Move assignment
    
    // Test move assignment - nested
    atom a10("deep1");
    atom a11("deep2");
    expr e11{std::variant<atom, cons, var>{a10}};
    expr e12{std::variant<atom, cons, var>{a11}};
    cons c8(e11, e12);
    
    expr e13{std::variant<atom, cons, var>{c8}};
    var v1(42);
    expr e14{std::variant<atom, cons, var>{v1}};
    cons c9(e13, e14);
    
    atom a12("target");
    expr e15{std::variant<atom, cons, var>{a12}};
    cons c10(e15, e15);
    
    c10 = std::move(c9);  // Move assignment with nested cons
    
    // Test self-move (should be safe)
    atom a13("self");
    expr e16{std::variant<atom, cons, var>{a13}};
    cons c11(e16, e16);
    c11 = std::move(c11);
    
    // Test chained move assignment
    atom a14("chain");
    expr e17{std::variant<atom, cons, var>{a14}};
    cons c12(e17, e17);
    cons c13(e17, e17);
    cons c14(e17, e17);
    
    c14 = std::move(c13 = std::move(c12));
    
    // Test move in vector
    std::vector<cons> vec;
    for (int i = 0; i < 30; i++) {
        atom left("l" + std::to_string(i));
        atom right("r" + std::to_string(i));
        expr el{std::variant<atom, cons, var>{left}};
        expr er{std::variant<atom, cons, var>{right}};
        vec.push_back(cons(el, er));  // Move into vector
    }
    assert(vec.size() == 30);
    
    // Test moving from vector
    std::vector<cons> vec2;
    for (auto& c : vec) {
        vec2.push_back(std::move(c));
    }
    assert(vec2.size() == 30);
}

void test_expr_constructor() {
    // Test with atom
    atom a1("test");
    expr e1{std::variant<atom, cons, var>{a1}};
    
    // Test with var
    var v1(42);
    expr e2{std::variant<atom, cons, var>{v1}};
    
    // Test with different atoms
    atom a2("atom1");
    atom a3("atom2");
    atom a4("");
    expr e3{std::variant<atom, cons, var>{a2}};
    expr e4{std::variant<atom, cons, var>{a3}};
    expr e5{std::variant<atom, cons, var>{a4}};
    
    // Test with different vars
    var v2(0);
    var v3(UINT32_MAX);
    expr e6{std::variant<atom, cons, var>{v2}};
    expr e7{std::variant<atom, cons, var>{v3}};
    
    // Test with cons
    atom a5("left");
    atom a6("right");
    expr e8{std::variant<atom, cons, var>{a5}};
    expr e9{std::variant<atom, cons, var>{a6}};
    cons c1(e8, e9);
    expr e10{std::variant<atom, cons, var>{c1}};
    
    // Multiple exprs from same variant
    atom a7("shared");
    expr e11{std::variant<atom, cons, var>{a7}};
    expr e12{std::variant<atom, cons, var>{a7}};
    
    // Test bulk construction
    std::vector<expr> exprs;
    for (int i = 0; i < 100; i++) {
        atom a("expr_" + std::to_string(i));
        exprs.push_back(expr{std::variant<atom, cons, var>{a}});
    }
    assert(exprs.size() == 100);
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

