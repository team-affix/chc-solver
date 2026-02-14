#include "../hpp/expr.hpp"
#include <iostream>
#include "test_utils.hpp"

void test_atom_constructor() {
    // Basic string
    expr::atom a1{"hello"};
    assert(a1.value == "hello");
    
    // Empty string
    expr::atom a2{""};
    assert(a2.value == "");
    
    // Single character
    expr::atom a3{"x"};
    assert(a3.value == "x");
    
    // Numeric strings
    expr::atom a4{"0"};
    assert(a4.value == "0");
    expr::atom a5{"12345"};
    assert(a5.value == "12345");
    expr::atom a6{"-42"};
    assert(a6.value == "-42");
    
    // Special characters
    expr::atom a7{"test_123"};
    assert(a7.value == "test_123");
    expr::atom a8{"hello world"};
    assert(a8.value == "hello world");
    expr::atom a9{"!@#$%^&*()"};
    assert(a9.value == "!@#$%^&*()");
    
    // Whitespace variations
    expr::atom a10{" "};
    assert(a10.value == " ");
    expr::atom a11{"\t"};
    assert(a11.value == "\t");
    expr::atom a12{"\n"};
    assert(a12.value == "\n");
    
    // Long string
    std::string long_str(1000, 'a');
    expr::atom a13{long_str};
    assert(a13.value == long_str);
    
    // Unicode/special characters
    expr::atom a14{"αβγδ"};
    assert(a14.value == "αβγδ");
    expr::atom a15{"日本語"};
    assert(a15.value == "日本語");
    
    // Escape sequences
    expr::atom a16{"line1\nline2"};
    assert(a16.value == "line1\nline2");
    
    // Multiple atoms with same value
    expr::atom a17{"duplicate"};
    expr::atom a18{"duplicate"};
    assert(a17.value == a18.value);
    assert((a17 <=> a18) == 0);
    
    // Test spaceship operator with different values
    expr::atom a19{"aaa"};
    expr::atom a20{"bbb"};
    assert((a19 <=> a20) < 0);
    assert((a20 <=> a19) > 0);
}

void test_var_constructor() {
    // Zero index
    expr::var v1{0};
    assert(v1.index == 0);
    
    // Small indices
    expr::var v2{1};
    assert(v2.index == 1);
    expr::var v3{10};
    assert(v3.index == 10);
    
    // Medium indices
    expr::var v4{100};
    assert(v4.index == 100);
    expr::var v5{999};
    assert(v5.index == 999);
    
    // Large indices
    expr::var v6{1000000};
    assert(v6.index == 1000000);
    expr::var v7{2147483647};
    assert(v7.index == 2147483647);
    
    // Maximum value
    expr::var v8{UINT32_MAX};
    assert(v8.index == UINT32_MAX);
    
    // Multiple vars with same index
    expr::var v9{42};
    expr::var v10{42};
    assert(v9.index == v10.index);
    assert((v9 <=> v10) == 0);
    
    // Test spaceship operator with different indices
    expr::var v11{10};
    expr::var v12{20};
    assert((v11 <=> v12) < 0);
    assert((v12 <=> v11) > 0);
    
    // Sequential indices
    for (uint32_t i = 0; i < 100; i++) {
        expr::var v{i};
        assert(v.index == i);
    }
}

void test_cons_constructor() {
    // Basic cons with raw pointers (testing the struct itself)
    expr e1{expr::atom{"left"}};
    expr e2{expr::atom{"right"}};
    expr::cons c1{&e1, &e2};
    
    assert(c1.lhs == &e1);
    assert(c1.rhs == &e2);
    assert(std::get<expr::atom>(c1.lhs->content).value == "left");
    assert(std::get<expr::atom>(c1.rhs->content).value == "right");
    
    // Cons with variables
    expr e3{expr::var{0}};
    expr e4{expr::var{1}};
    expr::cons c2{&e3, &e4};
    
    assert(std::get<expr::var>(c2.lhs->content).index == 0);
    assert(std::get<expr::var>(c2.rhs->content).index == 1);
    
    // Cons with mixed types
    expr e5{expr::atom{"atom"}};
    expr e6{expr::var{42}};
    expr::cons c3{&e5, &e6};
    
    assert(std::get<expr::atom>(c3.lhs->content).value == "atom");
    assert(std::get<expr::var>(c3.rhs->content).index == 42);
    
    // Cons with same expr on both sides
    expr e7{expr::atom{"same"}};
    expr::cons c4{&e7, &e7};
    
    assert(c4.lhs == c4.rhs);
    assert(c4.lhs == &e7);
    
    // Test spaceship operator
    expr e8{expr::atom{"a"}};
    expr e9{expr::atom{"b"}};
    expr::cons c5{&e8, &e9};
    expr::cons c6{&e8, &e9};
    
    assert((c5 <=> c6) == 0);
    
    // Different cons
    expr e10{expr::atom{"c"}};
    expr::cons c7{&e8, &e10};
    
    assert((c5 <=> c7) != 0);
}

void test_expr_constructor() {
    // Expr with atom
    expr e1{expr::atom{"test"}};
    assert(std::holds_alternative<expr::atom>(e1.content));
    assert(std::get<expr::atom>(e1.content).value == "test");
    
    // Expr with empty atom
    expr e2{expr::atom{""}};
    assert(std::holds_alternative<expr::atom>(e2.content));
    assert(std::get<expr::atom>(e2.content).value == "");
    
    // Expr with var
    expr e3{expr::var{42}};
    assert(std::holds_alternative<expr::var>(e3.content));
    assert(std::get<expr::var>(e3.content).index == 42);
    
    // Expr with var - edge cases
    expr e4{expr::var{0}};
    assert(std::get<expr::var>(e4.content).index == 0);
    
    expr e5{expr::var{UINT32_MAX}};
    assert(std::get<expr::var>(e5.content).index == UINT32_MAX);
    
    // Expr with cons
    expr left{expr::atom{"left"}};
    expr right{expr::atom{"right"}};
    expr e6{expr::cons{&left, &right}};
    
    assert(std::holds_alternative<expr::cons>(e6.content));
    const expr::cons& c1 = std::get<expr::cons>(e6.content);
    assert(std::get<expr::atom>(c1.lhs->content).value == "left");
    assert(std::get<expr::atom>(c1.rhs->content).value == "right");
    
    // Test spaceship operator
    expr e7{expr::atom{"aaa"}};
    expr e8{expr::atom{"aaa"}};
    assert((e7 <=> e8) == 0);
    
    expr e9{expr::atom{"bbb"}};
    assert((e7 <=> e9) < 0);
    assert((e9 <=> e7) > 0);
    
    // Different variant types
    expr e10{expr::var{0}};
    // Comparison between different variant types is well-defined by spaceship
    assert((e7 <=> e10) != 0);
}

// test case for expr_pool

void test_expr_pool_intern() {
    expr_pool pool;
    
    // Test that intern is called by atom/var/cons
    // Already tested above, but let's verify the interning behavior more explicitly
    
    // Create many atoms with same value
    std::vector<const expr*> atoms;
    for (int i = 0; i < 100; i++) {
        atoms.push_back(pool.atom("interned"));
    }
    
    // All should be the same pointer
    for (size_t i = 1; i < atoms.size(); i++) {
        assert(atoms[0] == atoms[i]);
    }
    
    // Create many vars with same index
    std::vector<const expr*> vars;
    for (int i = 0; i < 100; i++) {
        vars.push_back(pool.var(123));
    }
    
    // All should be the same pointer
    for (size_t i = 1; i < vars.size(); i++) {
        assert(vars[0] == vars[i]);
    }
    
    // Create many cons with same structure
    const expr* a = pool.atom("a");
    const expr* b = pool.atom("b");
    std::vector<const expr*> conses;
    for (int i = 0; i < 100; i++) {
        conses.push_back(pool.cons(a, b));
    }
    
    // All should be the same pointer
    for (size_t i = 1; i < conses.size(); i++) {
        assert(conses[0] == conses[i]);
    }
    
    // Test that different structures get different pointers
    const expr* c1 = pool.cons(pool.atom("x"), pool.atom("y"));
    const expr* c2 = pool.cons(pool.atom("x"), pool.atom("z"));
    const expr* c3 = pool.cons(pool.atom("y"), pool.atom("y"));
    
    assert(c1 != c2);
    assert(c1 != c3);
    assert(c2 != c3);
    
    // Test interning with nested structures
    const expr* inner1 = pool.cons(pool.atom("inner"), pool.var(1));
    const expr* outer1 = pool.cons(inner1, pool.atom("outer"));
    
    const expr* inner2 = pool.cons(pool.atom("inner"), pool.var(1));
    const expr* outer2 = pool.cons(inner2, pool.atom("outer"));
    
    // inner1 and inner2 should be same (interned)
    assert(inner1 == inner2);
    // outer1 and outer2 should be same (interned)
    assert(outer1 == outer2);
}

void test_expr_pool_atom() {
    expr_pool pool;
    
    // Basic atom creation
    const expr* e1 = pool.atom("test");
    assert(e1 != nullptr);
    assert(std::holds_alternative<expr::atom>(e1->content));
    assert(std::get<expr::atom>(e1->content).value == "test");
    
    // Empty string
    const expr* e2 = pool.atom("");
    assert(std::get<expr::atom>(e2->content).value == "");
    
    // Interning - same string should return same pointer
    const expr* e3 = pool.atom("test");
    assert(e1 == e3);
    
    // Different strings should return different pointers
    const expr* e4 = pool.atom("different");
    assert(e1 != e4);
    
    // Multiple calls with same string
    const expr* e5 = pool.atom("shared");
    const expr* e6 = pool.atom("shared");
    const expr* e7 = pool.atom("shared");
    assert(e5 == e6);
    assert(e6 == e7);
    
    // Special characters
    const expr* e8 = pool.atom("!@#$");
    const expr* e9 = pool.atom("!@#$");
    assert(e8 == e9);
    
    // Long strings
    std::string long_str(1000, 'x');
    const expr* e10 = pool.atom(long_str);
    const expr* e11 = pool.atom(long_str);
    assert(e10 == e11);
}

void test_expr_pool_var() {
    expr_pool pool;
    
    // Basic var creation
    const expr* e1 = pool.var(0);
    assert(e1 != nullptr);
    assert(std::holds_alternative<expr::var>(e1->content));
    assert(std::get<expr::var>(e1->content).index == 0);
    
    // Interning - same index should return same pointer
    const expr* e2 = pool.var(0);
    assert(e1 == e2);
    
    // Different indices should return different pointers
    const expr* e3 = pool.var(1);
    assert(e1 != e3);
    
    // Multiple calls with same index
    const expr* e4 = pool.var(42);
    const expr* e5 = pool.var(42);
    const expr* e6 = pool.var(42);
    assert(e4 == e5);
    assert(e5 == e6);
    
    // Edge cases
    const expr* e7 = pool.var(UINT32_MAX);
    const expr* e8 = pool.var(UINT32_MAX);
    assert(e7 == e8);
    
    // Sequential indices
    for (uint32_t i = 0; i < 100; i++) {
        const expr* v1 = pool.var(i);
        const expr* v2 = pool.var(i);
        assert(v1 == v2);
        assert(std::get<expr::var>(v1->content).index == i);
    }
}

void test_expr_pool_cons() {
    expr_pool pool;
    
    // Basic cons creation
    const expr* left = pool.atom("left");
    const expr* right = pool.atom("right");
    const expr* c1 = pool.cons(left, right);
    
    assert(c1 != nullptr);
    assert(std::holds_alternative<expr::cons>(c1->content));
    const expr::cons& cons1 = std::get<expr::cons>(c1->content);
    assert(cons1.lhs == left);
    assert(cons1.rhs == right);
    
    // Interning - same cons should return same pointer
    const expr* c2 = pool.cons(left, right);
    assert(c1 == c2);
    
    // Different cons should return different pointers
    const expr* c3 = pool.cons(right, left);  // Swapped
    assert(c1 != c3);
    
    // Cons with variables
    const expr* v1 = pool.var(10);
    const expr* v2 = pool.var(20);
    const expr* c4 = pool.cons(v1, v2);
    const expr* c5 = pool.cons(v1, v2);
    assert(c4 == c5);
    
    // Nested cons
    const expr* inner = pool.cons(pool.atom("a"), pool.atom("b"));
    const expr* outer = pool.cons(inner, pool.atom("c"));
    const expr* outer2 = pool.cons(inner, pool.atom("c"));
    assert(outer == outer2);
    
    // Same expr on both sides
    const expr* same = pool.atom("same");
    const expr* c6 = pool.cons(same, same);
    const expr* c7 = pool.cons(same, same);
    assert(c6 == c7);
    
    // Deep nesting with interning
    const expr* d1 = pool.cons(pool.atom("x"), pool.atom("y"));
    const expr* d2 = pool.cons(d1, d1);
    const expr* d3 = pool.cons(d2, d2);
    
    const expr* d1_dup = pool.cons(pool.atom("x"), pool.atom("y"));
    const expr* d2_dup = pool.cons(d1_dup, d1_dup);
    const expr* d3_dup = pool.cons(d2_dup, d2_dup);
    
    assert(d1 == d1_dup);
    assert(d2 == d2_dup);
    assert(d3 == d3_dup);
}

void unit_test_main() {
    constexpr bool ENABLE_DEBUG_LOGS = true;

    // test cases
    TEST(test_atom_constructor);
    TEST(test_var_constructor);
    TEST(test_cons_constructor);
    TEST(test_expr_constructor);
    TEST(test_expr_pool_intern);
    TEST(test_expr_pool_atom);
    TEST(test_expr_pool_var);
    TEST(test_expr_pool_cons);
}

int main() {
    unit_test_main();
    return 0;
}
