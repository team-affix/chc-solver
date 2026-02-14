#include "../hpp/expr.hpp"
#include <iostream>
#include "test_utils.hpp"

void test_trail_constructor() {
    // Basic construction - should not crash
    trail t1;
    assert(t1.depth() == 0);
    
    // Multiple trails can be constructed
    trail t2;
    trail t3;
    assert(t2.depth() == 0);
    assert(t3.depth() == 0);
    
    // Trail should be usable immediately after construction
    t1.push();
    assert(t1.depth() == 1);
    t1.pop();
    assert(t1.depth() == 0);
}

void test_trail_push_pop() {
    trail t;
    assert(t.depth() == 0);
    
    // Single push/pop with no logged operations
    t.push();
    assert(t.depth() == 1);
    t.pop();
    assert(t.depth() == 0);
    
    // Multiple push/pop pairs with no logged operations
    t.push();
    assert(t.depth() == 1);
    t.pop();
    assert(t.depth() == 0);
    
    t.push();
    assert(t.depth() == 1);
    t.pop();
    assert(t.depth() == 0);
    
    t.push();
    assert(t.depth() == 1);
    t.pop();
    assert(t.depth() == 0);
    
    // Nested push/pop
    t.push();
    assert(t.depth() == 1);
    t.push();
    assert(t.depth() == 2);
    t.pop();
    assert(t.depth() == 1);
    t.pop();
    assert(t.depth() == 0);
    
    // Deeper nesting
    t.push();
    assert(t.depth() == 1);
    t.push();
    assert(t.depth() == 2);
    t.push();
    assert(t.depth() == 3);
    t.pop();
    assert(t.depth() == 2);
    t.pop();
    assert(t.depth() == 1);
    t.pop();
    assert(t.depth() == 0);
    
    // Mixed nesting
    t.push();
    assert(t.depth() == 1);
    t.push();
    assert(t.depth() == 2);
    t.pop();
    assert(t.depth() == 1);
    t.push();
    assert(t.depth() == 2);
    t.pop();
    assert(t.depth() == 1);
    t.pop();
    assert(t.depth() == 0);
}

void test_trail_log() {
    // Test 1: Single log operation
    {
        trail t;
        int x = 5;
        t.push();
        x = 10;
        t.log([&x]() { x = 5; });
        assert(x == 10);
        t.pop();
        assert(x == 5);
    }
    
    // Test 2: Multiple log operations in one frame
    {
        trail t;
        int a = 1, b = 2, c = 3;
        t.push();
        a = 10;
        t.log([&a]() { a = 1; });
        b = 20;
        t.log([&b]() { b = 2; });
        c = 30;
        t.log([&c]() { c = 3; });
        
        assert(a == 10 && b == 20 && c == 30);
        t.pop();
        assert(a == 1 && b == 2 && c == 3);
    }
    
    // Test 3: Nested frames with logs
    {
        trail t;

        int x = 0;
        
        t.push();  // Frame 1
        x = 1;
        t.log([&x]() { x = 0; });
        
        t.push();  // Frame 2
        x = 2;
        t.log([&x]() { x = 1; });
        
        assert(x == 2);
        t.pop();  // Pop frame 2
        assert(x == 1);
        t.pop();  // Pop frame 1
        assert(x == 0);
    }
    
    // Test 4: Multiple operations per frame with nesting
    {
        trail t;

        int a = 100, b = 200, c = 300;
        
        t.push();  // Frame 1
        a = 111;
        t.log([&a]() { a = 100; });
        b = 222;
        t.log([&b]() { b = 200; });
        
        t.push();  // Frame 2
        b = 333;
        t.log([&b]() { b = 222; });
        c = 444;
        t.log([&c]() { c = 300; });
        
        t.push();  // Frame 3
        a = 555;
        t.log([&a]() { a = 111; });
        
        assert(a == 555 && b == 333 && c == 444);
        
        t.pop();  // Pop frame 3
        assert(a == 111 && b == 333 && c == 444);
        
        t.pop();  // Pop frame 2
        assert(a == 111 && b == 222 && c == 300);
        
        t.pop();  // Pop frame 1
        assert(a == 100 && b == 200 && c == 300);
    }
    
    // Test 5: Empty frame (push/pop with no logs)
    {
        trail t;

        int x = 42;
        t.push();
        // No logs
        x = 99;
        assert(x == 99);
        t.pop();
        assert(x == 99);  // Should remain unchanged since no undo was logged
    }
    
    // Test 6: Complex nested scenario with partial pops
    {
        trail t;

        int val = 0;
        
        t.push();  // Frame A
        val = 1;
        t.log([&val]() { val = 0; });
        
        t.push();  // Frame B
        val = 2;
        t.log([&val]() { val = 1; });
        
        t.push();  // Frame C
        val = 3;
        t.log([&val]() { val = 2; });
        
        assert(val == 3);
        t.pop();  // Pop C
        assert(val == 2);
        
        // Add more to frame B
        val = 4;
        t.log([&val]() { val = 2; });
        
        assert(val == 4);
        t.pop();  // Pop B (should undo both operations in B)
        assert(val == 1);
        
        t.pop();  // Pop A
        assert(val == 0);
    }
    
    // Test 7: Multiple variables with complex state changes
    {
        trail t;

        int x = 10, y = 20, z = 30;
        
        t.push();  // Level 1
        x += 5;
        t.log([&x]() { x -= 5; });
        y *= 2;
        t.log([&y]() { y /= 2; });
        
        assert(x == 15 && y == 40 && z == 30);
        
        t.push();  // Level 2
        z = x + y;  // z = 55
        t.log([&z]() { z = 30; });
        x = 0;
        t.log([&x]() { x = 15; });
        
        assert(x == 0 && y == 40 && z == 55);
        
        t.push();  // Level 3
        y = 100;
        t.log([&y]() { y = 40; });
        
        assert(x == 0 && y == 100 && z == 55);
        
        t.pop();  // Pop level 3
        assert(x == 0 && y == 40 && z == 55);
        
        t.pop();  // Pop level 2
        assert(x == 15 && y == 40 && z == 30);
        
        t.pop();  // Pop level 1
        assert(x == 10 && y == 20 && z == 30);
    }
    
    // Test 8: Deeply nested frames (5 levels)
    {
        trail t;

        int depth = 0;
        
        t.push();
        depth = 1;
        t.log([&depth]() { depth = 0; });
        
        t.push();
        depth = 2;
        t.log([&depth]() { depth = 1; });
        
        t.push();
        depth = 3;
        t.log([&depth]() { depth = 2; });
        
        t.push();
        depth = 4;
        t.log([&depth]() { depth = 3; });
        
        t.push();
        depth = 5;
        t.log([&depth]() { depth = 4; });
        
        assert(depth == 5);
        t.pop();
        assert(depth == 4);
        t.pop();
        assert(depth == 3);
        t.pop();
        assert(depth == 2);
        t.pop();
        assert(depth == 1);
        t.pop();
        assert(depth == 0);
    }
    
    // Test 9: Many operations in a single frame
    {
        trail t;

        std::vector<int> values(10, 0);
        
        t.push();
        for (int i = 0; i < 10; i++) {
            values[i] = i + 1;
            t.log([&values, i]() { values[i] = 0; });
        }
        
        for (int i = 0; i < 10; i++) {
            assert(values[i] == i + 1);
        }
        
        t.pop();
        
        for (int i = 0; i < 10; i++) {
            assert(values[i] == 0);
        }
    }
    
    // Test 10: Interleaved push/pop/log operations
    {
        trail t;

        int state = 0;
        
        t.push();  // Frame 1
        state = 1;
        t.log([&state]() { state = 0; });
        
        t.push();  // Frame 2
        state = 2;
        t.log([&state]() { state = 1; });
        
        t.pop();  // Pop frame 2
        assert(state == 1);
        
        t.push();  // New frame 2
        state = 3;
        t.log([&state]() { state = 1; });
        
        t.push();  // Frame 3
        state = 4;
        t.log([&state]() { state = 3; });
        
        assert(state == 4);
        t.pop();  // Pop frame 3
        assert(state == 3);
        t.pop();  // Pop frame 2
        assert(state == 1);
        t.pop();  // Pop frame 1
        assert(state == 0);
    }
    
    // Test 11: String modifications
    {
        trail t;
        
        std::string str = "original";
        
        t.push();
        str = "modified";
        t.log([&str]() { str = "original"; });
        
        assert(str == "modified");
        t.pop();
        assert(str == "original");
    }
    
    // Test 12: Multiple independent trails
    {
        trail t1, t2;
        int x = 1, y = 2;
        
        t1.push();
        x = 10;
        t1.log([&x]() { x = 1; });
        
        t2.push();
        y = 20;
        t2.log([&y]() { y = 2; });
        
        assert(x == 10 && y == 20);
        
        t1.pop();
        assert(x == 1 && y == 20);
        
        t2.pop();
        assert(x == 1 && y == 2);
    }
    
    // Test 13: Complex multiple independent trails with nested frames
    {
        trail t1, t2, t3;
        
        // Each trail manages its own independent data
        int data1 = 100;
        int data2 = 200;
        int data3 = 300;
        
        assert(t1.depth() == 0);
        assert(t2.depth() == 0);
        assert(t3.depth() == 0);
        
        // === Trail 1: Nested frames with data1 ===
        t1.push();  // Frame 1.1
        assert(t1.depth() == 1);
        data1 += 10;  // 110
        t1.log([&data1]() { data1 -= 10; });
        
        t1.push();  // Frame 1.2
        assert(t1.depth() == 2);
        data1 *= 2;  // 220
        t1.log([&data1]() { data1 /= 2; });
        
        t1.push();  // Frame 1.3
        assert(t1.depth() == 3);
        data1 += 80;  // 300
        t1.log([&data1]() { data1 -= 80; });
        
        assert(data1 == 300);
        
        // === Trail 2: Nested frames with data2 ===
        t2.push();  // Frame 2.1
        assert(t2.depth() == 1);
        data2 -= 50;  // 150
        t2.log([&data2]() { data2 += 50; });
        
        t2.push();  // Frame 2.2
        assert(t2.depth() == 2);
        data2 *= 3;  // 450
        t2.log([&data2]() { data2 /= 3; });
        
        assert(data2 == 450);
        
        // === Trail 3: Single frame with multiple operations on data3 ===
        t3.push();  // Frame 3.1
        assert(t3.depth() == 1);
        data3 /= 3;  // 100
        t3.log([&data3]() { data3 *= 3; });
        data3 += 50;  // 150
        t3.log([&data3]() { data3 -= 50; });
        data3 *= 4;  // 600
        t3.log([&data3]() { data3 /= 4; });
        
        assert(data3 == 600);
        
        // Verify all data is at expected state
        assert(data1 == 300 && data2 == 450 && data3 == 600);
        
        // === Pop trail 1 innermost frame ===
        t1.pop();  // Pop frame 1.3
        assert(t1.depth() == 2);
        assert(data1 == 220);  // Restored from frame 1.3
        assert(data2 == 450);  // Unchanged
        assert(data3 == 600);  // Unchanged
        
        // === Add more to trail 2 ===
        t2.push();  // Frame 2.3
        assert(t2.depth() == 3);
        data2 += 50;  // 500
        t2.log([&data2]() { data2 -= 50; });
        
        assert(data1 == 220 && data2 == 500 && data3 == 600);
        
        // === Pop trail 3 completely ===
        t3.pop();  // Pop frame 3.1
        assert(t3.depth() == 0);
        assert(data1 == 220);  // Unchanged
        assert(data2 == 500);  // Unchanged
        assert(data3 == 300);  // Restored to original
        
        // === Add new frame to trail 3 ===
        t3.push();  // New frame 3.1
        assert(t3.depth() == 1);
        data3 -= 100;  // 200
        t3.log([&data3]() { data3 += 100; });
        data3 *= 5;  // 1000
        t3.log([&data3]() { data3 /= 5; });
        
        assert(data1 == 220 && data2 == 500 && data3 == 1000);
        
        // === Pop trail 2 innermost frame ===
        t2.pop();  // Pop frame 2.3
        assert(t2.depth() == 2);
        assert(data1 == 220);  // Unchanged
        assert(data2 == 450);  // Restored from frame 2.3
        assert(data3 == 1000);  // Unchanged
        
        // === Pop trail 1 middle frame ===
        t1.pop();  // Pop frame 1.2
        assert(t1.depth() == 1);
        assert(data1 == 110);  // Restored from frame 1.2
        assert(data2 == 450);  // Unchanged
        assert(data3 == 1000);  // Unchanged
        
        // === Pop trail 2 all remaining frames ===
        t2.pop();  // Pop frame 2.2
        assert(t2.depth() == 1);
        assert(data2 == 150);  // Restored from frame 2.2
        
        t2.pop();  // Pop frame 2.1
        assert(t2.depth() == 0);
        assert(data2 == 200);  // Restored to original
        
        assert(data1 == 110 && data2 == 200 && data3 == 1000);
        
        // === Pop trail 3 ===
        t3.pop();  // Pop frame 3.1
        assert(t3.depth() == 0);
        assert(data3 == 300);  // Restored to original
        
        assert(data1 == 110 && data2 == 200 && data3 == 300);
        
        // === Pop trail 1 last frame ===
        t1.pop();  // Pop frame 1.1
        assert(t1.depth() == 0);
        assert(data1 == 100);  // Restored to original
        
        // All data restored to original values
        assert(data1 == 100 && data2 == 200 && data3 == 300);
        assert(t1.depth() == 0 && t2.depth() == 0 && t3.depth() == 0);
    }

    // Test 14: COMPREHENSIVE SEQUENCE REVERSAL TEST WITH CHECKPOINTS
    {
        trail t;
        
        int val = 100;  // Starting value
        
        // === FRAME 1 ===
        t.push();
        assert(t.depth() == 1);
        
        // Step 1: Add 5 -> 105
        val += 5;
        t.log([&val]() { val -= 5; });
        assert(val == 105);
        
        // Step 2: Multiply by 2 -> 210
        val *= 2;
        t.log([&val]() { val /= 2; });
        assert(val == 210);
        
        // Step 3: Subtract 10 -> 200
        val -= 10;
        t.log([&val]() { val += 10; });
        assert(val == 200);
        
        // Step 4: Add 50 -> 250
        val += 50;
        t.log([&val]() { val -= 50; });
        assert(val == 250);
        
        // CHECKPOINT 1: val should be 250
        int checkpoint1 = val;
        assert(checkpoint1 == 250);
        
        // === FRAME 2 ===
        t.push();
        assert(t.depth() == 2);
        
        // Step 5: Divide by 5 -> 50
        val /= 5;
        t.log([&val]() { val *= 5; });
        assert(val == 50);
        
        // Step 6: Add 150 -> 200
        val += 150;
        t.log([&val]() { val -= 150; });
        assert(val == 200);
        
        // Step 7: Multiply by 3 -> 600
        val *= 3;
        t.log([&val]() { val /= 3; });
        assert(val == 600);
        
        // Step 8: Subtract 100 -> 500
        val -= 100;
        t.log([&val]() { val += 100; });
        assert(val == 500);
        
        // Step 9: Add 25 -> 525
        val += 25;
        t.log([&val]() { val -= 25; });
        assert(val == 525);
        
        // CHECKPOINT 2: val should be 525
        int checkpoint2 = val;
        assert(checkpoint2 == 525);
        
        // === FRAME 3 ===
        t.push();
        assert(t.depth() == 3);
        
        // Step 10: Subtract 25 -> 500
        val -= 25;
        t.log([&val]() { val += 25; });
        assert(val == 500);
        
        // Step 11: Divide by 4 -> 125
        val /= 4;
        t.log([&val]() { val *= 4; });
        assert(val == 125);
        
        // Step 12: Add 75 -> 200
        val += 75;
        t.log([&val]() { val -= 75; });
        assert(val == 200);
        
        // Step 13: Multiply by 2 -> 400
        val *= 2;
        t.log([&val]() { val /= 2; });
        assert(val == 400);
        
        // Step 14: Subtract 50 -> 350
        val -= 50;
        t.log([&val]() { val += 50; });
        assert(val == 350);
        
        // Step 15: Add 150 -> 500
        val += 150;
        t.log([&val]() { val -= 150; });
        assert(val == 500);
        
        // CHECKPOINT 3: val should be 500
        int checkpoint3 = val;
        assert(checkpoint3 == 500);
        
        // === FRAME 4 ===
        t.push();
        assert(t.depth() == 4);
        
        // Step 16: Divide by 10 -> 50
        val /= 10;
        t.log([&val]() { val *= 10; });
        assert(val == 50);
        
        // Step 17: Add 450 -> 500
        val += 450;
        t.log([&val]() { val -= 450; });
        assert(val == 500);
        
        // Step 18: Multiply by 2 -> 1000
        val *= 2;
        t.log([&val]() { val /= 2; });
        assert(val == 1000);
        
        // Step 19: Subtract 200 -> 800
        val -= 200;
        t.log([&val]() { val += 200; });
        assert(val == 800);
        
        // Step 20: Add 100 -> 900
        val += 100;
        t.log([&val]() { val -= 100; });
        assert(val == 900);
        
        // CHECKPOINT 4: val should be 900
        int checkpoint4 = val;
        assert(checkpoint4 == 900);
        
        // === NOW UNDO IN REVERSE ORDER ===
        
        // Pop frame 4 - should restore to checkpoint 3 (500)
        t.pop();
        assert(t.depth() == 3);
        assert(val == checkpoint3);
        assert(val == 500);
        
        // Pop frame 3 - should restore to checkpoint 2 (525)
        t.pop();
        assert(t.depth() == 2);
        assert(val == checkpoint2);
        assert(val == 525);
        
        // Pop frame 2 - should restore to checkpoint 1 (250)
        t.pop();
        assert(t.depth() == 1);
        assert(val == checkpoint1);
        assert(val == 250);
        
        // Pop frame 1 - should restore to original (100)
        t.pop();
        assert(t.depth() == 0);
        assert(val == 100);
    }
}

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

void test_expr_pool_atom() {
    trail t;
    expr_pool pool(t);
    
    // Push initial frame before any operations
    t.push();
    
    // Basic atom creation
    const expr* e1 = pool.atom("test");
    assert(e1 != nullptr);
    assert(std::holds_alternative<expr::atom>(e1->content));
    assert(std::get<expr::atom>(e1->content).value == "test");
    assert(pool.size() == 1);
    
    // Empty string
    const expr* e2 = pool.atom("");
    assert(std::get<expr::atom>(e2->content).value == "");
    assert(pool.size() == 2);
    
    // Interning - same string should return same pointer
    const expr* e3 = pool.atom("test");
    assert(e1 == e3);
    assert(pool.size() == 2);  // No new entry added
    
    // Different strings should return different pointers
    const expr* e4 = pool.atom("different");
    assert(e1 != e4);
    assert(pool.size() == 3);
    
    // Multiple calls with same string
    const expr* e5 = pool.atom("shared");
    const expr* e6 = pool.atom("shared");
    const expr* e7 = pool.atom("shared");
    assert(e5 == e6);
    assert(e6 == e7);
    assert(pool.size() == 4);  // Only one "shared" added
    
    // Special characters
    const expr* e8 = pool.atom("!@#$");
    const expr* e9 = pool.atom("!@#$");
    assert(e8 == e9);
    assert(pool.size() == 5);
    
    // Long strings
    std::string long_str(1000, 'x');
    const expr* e10 = pool.atom(long_str);
    const expr* e11 = pool.atom(long_str);
    assert(e10 == e11);
    assert(pool.size() == 6);
    
    // Test backtracking: push frame, add content, pop frame
    size_t size_before = pool.size();
    t.push();
    const expr* temp1 = pool.atom("temporary1");
    const expr* temp2 = pool.atom("temporary2");
    assert(pool.size() == size_before + 2);
    t.pop();
    assert(pool.size() == size_before);  // Should be back to original size
    
    // Test corner case: intern same content in nested frames
    t.push();  // Frame 1
    const expr* content_c = pool.atom("content_c");
    size_t checkpoint1 = pool.size();
    assert(content_c != nullptr);
    
    t.push();  // Frame 2
    const expr* content_c_again = pool.atom("content_c");  // Should return same pointer, no log
    assert(content_c == content_c_again);
    assert(pool.size() == checkpoint1);  // Size unchanged
    
    t.pop();  // Pop frame 2
    assert(pool.size() == checkpoint1);  // Size still unchanged
    
    // Verify content_c is still there
    const expr* content_c_verify = pool.atom("content_c");
    assert(content_c == content_c_verify);
    assert(pool.size() == checkpoint1);
    
    t.pop();  // Pop frame 1
    // Now content_c should be removed

    assert(pool.size() == size_before);
    
    // Test nested pushes with checkpoints
    size_t checkpoint_start = pool.size();
    
    t.push();  // Level 1
    pool.atom("level1_a");
    pool.atom("level1_b");
    size_t checkpoint_level1 = pool.size();
    assert(checkpoint_level1 == checkpoint_start + 2);
    
    t.push();  // Level 2
    pool.atom("level2_a");
    pool.atom("level2_b");
    pool.atom("level2_c");
    size_t checkpoint_level2 = pool.size();
    assert(checkpoint_level2 == checkpoint_level1 + 3);
    
    t.push();  // Level 3
    pool.atom("level3_a");
    size_t checkpoint_level3 = pool.size();
    assert(checkpoint_level3 == checkpoint_level2 + 1);
    
    // Pop level 3
    t.pop();
    assert(pool.size() == checkpoint_level2);
    
    // Pop level 2
    t.pop();
    assert(pool.size() == checkpoint_level1);
    
    // Pop level 1
    t.pop();
    assert(pool.size() == checkpoint_start);
    
    // Test corner case: content added in earlier frame should not be removed by later frame pop
    t.push();  // Frame A
    const expr* early_content_1 = pool.atom("early_1");
    const expr* early_content_2 = pool.atom("early_2");
    size_t checkpoint_a = pool.size();
    assert(checkpoint_a == checkpoint_start + 2);
    
    t.push();  // Frame B
    const expr* mid_content = pool.atom("mid_content");
    size_t checkpoint_b = pool.size();
    assert(checkpoint_b == checkpoint_a + 1);
    
    // Re-intern early content in Frame B - should not log since already exists
    const expr* early_content_1_again = pool.atom("early_1");
    assert(early_content_1 == early_content_1_again);
    assert(pool.size() == checkpoint_b);  // Size unchanged
    
    // Add more new content in Frame B
    const expr* late_content_1 = pool.atom("late_1");
    const expr* late_content_2 = pool.atom("late_2");
    size_t checkpoint_b_final = pool.size();
    assert(checkpoint_b_final == checkpoint_b + 2);
    
    t.push();  // Frame C
    // Re-intern content from both Frame A and Frame B
    const expr* early_content_2_again = pool.atom("early_2");
    assert(early_content_2 == early_content_2_again);
    const expr* mid_content_again = pool.atom("mid_content");
    assert(mid_content == mid_content_again);
    assert(pool.size() == checkpoint_b_final);  // Size unchanged
    
    // Add new content in Frame C
    const expr* frame_c_content = pool.atom("frame_c");
    size_t checkpoint_c = pool.size();
    assert(checkpoint_c == checkpoint_b_final + 1);
    
    // Pop Frame C - only frame_c_content should be removed
    t.pop();
    assert(pool.size() == checkpoint_b_final);
    
    // Verify early and mid content still exist
    const expr* verify_early_1 = pool.atom("early_1");
    assert(verify_early_1 == early_content_1);
    const expr* verify_mid = pool.atom("mid_content");
    assert(verify_mid == mid_content);
    const expr* verify_late_1 = pool.atom("late_1");
    assert(verify_late_1 == late_content_1);
    assert(pool.size() == checkpoint_b_final);  // Still unchanged
    
    // Pop Frame B - should remove mid_content, late_1, late_2 but NOT early_1, early_2
    t.pop();
    assert(pool.size() == checkpoint_a);
    
    // Verify early content still exists
    const expr* verify_early_1_after_b = pool.atom("early_1");
    assert(verify_early_1_after_b == early_content_1);
    const expr* verify_early_2_after_b = pool.atom("early_2");
    assert(verify_early_2_after_b == early_content_2);
    assert(pool.size() == checkpoint_a);  // Still unchanged
    
    // Pop Frame A - should remove early_1 and early_2
    t.pop();
    assert(pool.size() == checkpoint_start);
    
    // Pop initial frame
    t.pop();
    assert(pool.size() == 0);
}

// void test_expr_pool_var() {
//     expr_pool pool;
    
//     // Basic var creation
//     const expr* e1 = pool.var(0);
//     assert(e1 != nullptr);
//     assert(std::holds_alternative<expr::var>(e1->content));
//     assert(std::get<expr::var>(e1->content).index == 0);
    
//     // Interning - same index should return same pointer
//     const expr* e2 = pool.var(0);
//     assert(e1 == e2);
    
//     // Different indices should return different pointers
//     const expr* e3 = pool.var(1);
//     assert(e1 != e3);
    
//     // Multiple calls with same index
//     const expr* e4 = pool.var(42);
//     const expr* e5 = pool.var(42);
//     const expr* e6 = pool.var(42);
//     assert(e4 == e5);
//     assert(e5 == e6);
    
//     // Edge cases
//     const expr* e7 = pool.var(UINT32_MAX);
//     const expr* e8 = pool.var(UINT32_MAX);
//     assert(e7 == e8);
    
//     // Sequential indices
//     for (uint32_t i = 0; i < 100; i++) {
//         const expr* v1 = pool.var(i);
//         const expr* v2 = pool.var(i);
//         assert(v1 == v2);
//         assert(std::get<expr::var>(v1->content).index == i);
//     }
// }

// void test_expr_pool_cons() {
//     expr_pool pool;
    
//     // Basic cons creation
//     const expr* left = pool.atom("left");
//     const expr* right = pool.atom("right");
//     const expr* c1 = pool.cons(left, right);
    
//     assert(c1 != nullptr);
//     assert(std::holds_alternative<expr::cons>(c1->content));
//     const expr::cons& cons1 = std::get<expr::cons>(c1->content);
//     assert(cons1.lhs == left);
//     assert(cons1.rhs == right);
    
//     // Interning - same cons should return same pointer
//     const expr* c2 = pool.cons(left, right);
//     assert(c1 == c2);
    
//     // Different cons should return different pointers
//     const expr* c3 = pool.cons(right, left);  // Swapped
//     assert(c1 != c3);
    
//     // Cons with variables
//     const expr* v1 = pool.var(10);
//     const expr* v2 = pool.var(20);
//     const expr* c4 = pool.cons(v1, v2);
//     const expr* c5 = pool.cons(v1, v2);
//     assert(c4 == c5);
    
//     // Nested cons
//     const expr* inner = pool.cons(pool.atom("a"), pool.atom("b"));
//     const expr* outer = pool.cons(inner, pool.atom("c"));
//     const expr* outer2 = pool.cons(inner, pool.atom("c"));
//     assert(outer == outer2);
    
//     // Same expr on both sides
//     const expr* same = pool.atom("same");
//     const expr* c6 = pool.cons(same, same);
//     const expr* c7 = pool.cons(same, same);
//     assert(c6 == c7);
    
//     // Deep nesting with interning
//     const expr* d1 = pool.cons(pool.atom("x"), pool.atom("y"));
//     const expr* d2 = pool.cons(d1, d1);
//     const expr* d3 = pool.cons(d2, d2);
    
//     const expr* d1_dup = pool.cons(pool.atom("x"), pool.atom("y"));
//     const expr* d2_dup = pool.cons(d1_dup, d1_dup);
//     const expr* d3_dup = pool.cons(d2_dup, d2_dup);
    
//     assert(d1 == d1_dup);
//     assert(d2 == d2_dup);
//     assert(d3 == d3_dup);
// }

void unit_test_main() {
    constexpr bool ENABLE_DEBUG_LOGS = true;

    // test cases
    TEST(test_trail_constructor);
    TEST(test_trail_push_pop);
    TEST(test_trail_log);
    TEST(test_atom_constructor);
    TEST(test_var_constructor);
    TEST(test_cons_constructor);
    TEST(test_expr_constructor);
    TEST(test_expr_pool_atom);
    // TEST(test_expr_pool_var);
    // TEST(test_expr_pool_cons);
}

int main() {
    unit_test_main();
    return 0;
}
