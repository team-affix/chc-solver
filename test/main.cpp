#include "../hpp/expr.hpp"
#include "../hpp/bind_map.hpp"
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

void test_expr_pool_constructor() {
    trail t;
    
    // Basic construction with trail reference
    expr_pool pool1(t);
    assert(pool1.size() == 0);
    
    // Multiple pools can be constructed with same trail
    expr_pool pool2(t);
    assert(pool2.size() == 0);
    
    // Multiple pools with different trails
    trail t2;
    expr_pool pool3(t2);
    assert(pool3.size() == 0);
    
    // Pool should be usable immediately after construction
    t.push();
    const expr* e1 = pool1.atom("test");
    assert(e1 != nullptr);
    assert(pool1.size() == 1);
    
    // Other pools are independent
    assert(pool2.size() == 0);
    assert(pool3.size() == 0);
    
    // Add to pool2 with same trail
    const expr* e2 = pool2.atom("test2");
    assert(pool2.size() == 1);
    assert(pool1.size() == 1);  // pool1 unchanged
    
    // Pop should affect both pool1 and pool2 since they share trail t
    t.pop();
    assert(pool1.size() == 0);
    assert(pool2.size() == 0);
    
    // pool3 with different trail is unaffected
    assert(pool3.size() == 0);
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

void test_expr_pool_var() {
    trail t;
    expr_pool pool(t);
    
    // Push initial frame before any operations
    t.push();
    
    // Basic var creation
    const expr* e1 = pool.var(0);
    assert(e1 != nullptr);
    assert(std::holds_alternative<expr::var>(e1->content));
    assert(std::get<expr::var>(e1->content).index == 0);
    assert(pool.size() == 1);
    
    // Interning - same index should return same pointer
    const expr* e2 = pool.var(0);
    assert(e1 == e2);
    assert(pool.size() == 1);  // No new entry added
    
    // Different indices should return different pointers
    const expr* e3 = pool.var(1);
    assert(e1 != e3);
    assert(pool.size() == 2);
    
    // Multiple calls with same index
    const expr* e4 = pool.var(42);
    const expr* e5 = pool.var(42);
    const expr* e6 = pool.var(42);
    assert(e4 == e5);
    assert(e5 == e6);
    assert(pool.size() == 3);  // Only one var(42) added
    
    // Edge cases
    const expr* e7 = pool.var(UINT32_MAX);
    const expr* e8 = pool.var(UINT32_MAX);
    assert(e7 == e8);
    assert(pool.size() == 4);
    
    // Sequential indices
    for (uint32_t i = 0; i < 100; i++) {
        const expr* v1 = pool.var(i);
        const expr* v2 = pool.var(i);
        assert(v1 == v2);
        assert(std::get<expr::var>(v1->content).index == i);
    }
    size_t size_after_sequential = pool.size();
    
    // Test backtracking: push frame, add content, pop frame
    size_t size_before = pool.size();
    t.push();
    const expr* temp1 = pool.var(9999);
    const expr* temp2 = pool.var(8888);
    assert(pool.size() == size_before + 2);
    t.pop();
    assert(pool.size() == size_before);  // Should be back to original size
    
    // Test corner case: intern same content in nested frames
    t.push();  // Frame 1
    const expr* var_100 = pool.var(100);
    size_t checkpoint1 = pool.size();
    assert(var_100 != nullptr);
    
    t.push();  // Frame 2
    const expr* var_100_again = pool.var(100);  // Should return same pointer, no log
    assert(var_100 == var_100_again);
    assert(pool.size() == checkpoint1);  // Size unchanged
    
    t.pop();  // Pop frame 2
    assert(pool.size() == checkpoint1);  // Size still unchanged
    
    // Verify var_100 is still there
    const expr* var_100_verify = pool.var(100);
    assert(var_100 == var_100_verify);
    assert(pool.size() == checkpoint1);
    
    t.pop();  // Pop frame 1
    // Now var_100 should be removed
    
    // Test nested pushes with checkpoints
    size_t checkpoint_start = pool.size();
    
    t.push();  // Level 1
    pool.var(200);
    pool.var(201);
    size_t checkpoint_level1 = pool.size();
    assert(checkpoint_level1 == checkpoint_start + 2);
    
    t.push();  // Level 2
    pool.var(300);
    pool.var(301);
    pool.var(302);
    size_t checkpoint_level2 = pool.size();
    assert(checkpoint_level2 == checkpoint_level1 + 3);
    
    t.push();  // Level 3
    pool.var(400);
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
    const expr* early_var_1 = pool.var(500);
    const expr* early_var_2 = pool.var(501);
    size_t checkpoint_a = pool.size();
    assert(checkpoint_a == checkpoint_start + 2);
    
    t.push();  // Frame B
    const expr* mid_var = pool.var(600);
    size_t checkpoint_b = pool.size();
    assert(checkpoint_b == checkpoint_a + 1);
    
    // Re-intern early content in Frame B - should not log since already exists
    const expr* early_var_1_again = pool.var(500);
    assert(early_var_1 == early_var_1_again);
    assert(pool.size() == checkpoint_b);  // Size unchanged
    
    // Add more new content in Frame B
    const expr* late_var_1 = pool.var(700);
    const expr* late_var_2 = pool.var(701);
    size_t checkpoint_b_final = pool.size();
    assert(checkpoint_b_final == checkpoint_b + 2);
    
    t.push();  // Frame C
    // Re-intern content from both Frame A and Frame B
    const expr* early_var_2_again = pool.var(501);
    assert(early_var_2 == early_var_2_again);
    const expr* mid_var_again = pool.var(600);
    assert(mid_var == mid_var_again);
    assert(pool.size() == checkpoint_b_final);  // Size unchanged
    
    // Add new content in Frame C
    const expr* frame_c_var = pool.var(800);
    size_t checkpoint_c = pool.size();
    assert(checkpoint_c == checkpoint_b_final + 1);
    
    // Pop Frame C - only frame_c_var should be removed
    t.pop();
    assert(pool.size() == checkpoint_b_final);
    
    // Verify early and mid content still exist
    const expr* verify_early_1 = pool.var(500);
    assert(verify_early_1 == early_var_1);
    const expr* verify_mid = pool.var(600);
    assert(verify_mid == mid_var);
    const expr* verify_late_1 = pool.var(700);
    assert(verify_late_1 == late_var_1);
    assert(pool.size() == checkpoint_b_final);  // Still unchanged
    
    // Pop Frame B - should remove mid_var, late_var_1, late_var_2 but NOT early vars
    t.pop();
    assert(pool.size() == checkpoint_a);
    
    // Verify early content still exists
    const expr* verify_early_1_after_b = pool.var(500);
    assert(verify_early_1_after_b == early_var_1);
    const expr* verify_early_2_after_b = pool.var(501);
    assert(verify_early_2_after_b == early_var_2);
    assert(pool.size() == checkpoint_a);  // Still unchanged
    
    // Pop Frame A - should remove early vars
    t.pop();
    assert(pool.size() == checkpoint_start);
    
    // Pop initial frame
    t.pop();
    assert(pool.size() == 0);
}

void test_expr_pool_cons() {
    trail t;
    expr_pool pool(t);
    
    // Push initial frame before any operations
    t.push();
    
    // Basic cons creation
    const expr* left = pool.atom("left");
    const expr* right = pool.atom("right");
    const expr* c1 = pool.cons(left, right);
    
    assert(c1 != nullptr);
    assert(std::holds_alternative<expr::cons>(c1->content));
    const expr::cons& cons1 = std::get<expr::cons>(c1->content);
    assert(cons1.lhs == left);
    assert(cons1.rhs == right);
    assert(pool.size() == 3);  // left, right, cons
    
    // Interning - same cons should return same pointer
    const expr* c2 = pool.cons(left, right);
    assert(c1 == c2);
    assert(pool.size() == 3);  // No new entry added
    
    // Different cons should return different pointers
    const expr* c3 = pool.cons(right, left);  // Swapped
    assert(c1 != c3);
    assert(pool.size() == 4);
    
    // Cons with variables
    const expr* v1 = pool.var(10);
    const expr* v2 = pool.var(20);
    const expr* c4 = pool.cons(v1, v2);
    const expr* c5 = pool.cons(v1, v2);
    assert(c4 == c5);
    assert(pool.size() == 7);  // v1, v2, cons(v1,v2)
    
    // Nested cons
    const expr* inner = pool.cons(pool.atom("a"), pool.atom("b"));
    const expr* outer = pool.cons(inner, pool.atom("c"));
    const expr* outer2 = pool.cons(inner, pool.atom("c"));
    assert(outer == outer2);
    size_t size_after_nested = pool.size();
    
    // Same expr on both sides
    const expr* same = pool.atom("same");
    const expr* c6 = pool.cons(same, same);
    const expr* c7 = pool.cons(same, same);
    assert(c6 == c7);
    assert(pool.size() == size_after_nested + 2);  // same, cons(same,same)
    
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
    size_t size_after_deep = pool.size();
    
    // Test backtracking: push frame, add content, pop frame
    size_t size_before = pool.size();
    t.push();
    const expr* temp_left = pool.atom("temp_left");
    const expr* temp_right = pool.atom("temp_right");
    const expr* temp_cons = pool.cons(temp_left, temp_right);
    assert(pool.size() == size_before + 3);
    t.pop();
    assert(pool.size() == size_before);  // Should be back to original size
    
    // Test corner case: intern same cons in nested frames
    t.push();  // Frame 1
    const expr* cons_a = pool.atom("cons_a");
    const expr* cons_b = pool.atom("cons_b");
    const expr* cons_ab = pool.cons(cons_a, cons_b);
    size_t checkpoint1 = pool.size();
    assert(cons_ab != nullptr);
    
    t.push();  // Frame 2
    const expr* cons_ab_again = pool.cons(cons_a, cons_b);  // Should return same pointer, no log
    assert(cons_ab == cons_ab_again);
    assert(pool.size() == checkpoint1);  // Size unchanged
    
    t.pop();  // Pop frame 2
    assert(pool.size() == checkpoint1);  // Size still unchanged
    
    // Verify cons_ab is still there
    const expr* cons_ab_verify = pool.cons(cons_a, cons_b);
    assert(cons_ab == cons_ab_verify);
    assert(pool.size() == checkpoint1);
    
    t.pop();  // Pop frame 1
    // Now cons_ab, cons_a, cons_b should be removed
    
    // Test nested pushes with checkpoints
    size_t checkpoint_start = pool.size();
    
    t.push();  // Level 1
    const expr* l1_a = pool.atom("l1_a");
    const expr* l1_b = pool.atom("l1_b");
    pool.cons(l1_a, l1_b);
    size_t checkpoint_level1 = pool.size();
    assert(checkpoint_level1 == checkpoint_start + 3);
    
    t.push();  // Level 2
    const expr* l2_a = pool.var(100);
    const expr* l2_b = pool.var(200);
    pool.cons(l2_a, l2_b);
    pool.cons(l1_a, l2_a);  // Mix from level 1 and level 2
    size_t checkpoint_level2 = pool.size();
    assert(checkpoint_level2 == checkpoint_level1 + 4);  // l2_a, l2_b, cons(l2_a,l2_b), cons(l1_a,l2_a)
    
    t.push();  // Level 3
    pool.cons(l1_a, l1_b);  // Re-intern from level 1, no new entry
    const expr* l3_cons = pool.cons(l2_a, l1_a);
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
    const expr* early_atom_1 = pool.atom("early_atom_1");
    const expr* early_atom_2 = pool.atom("early_atom_2");
    const expr* early_cons = pool.cons(early_atom_1, early_atom_2);
    size_t checkpoint_a = pool.size();
    assert(checkpoint_a == checkpoint_start + 3);
    
    t.push();  // Frame B
    const expr* mid_atom = pool.atom("mid_atom");
    const expr* mid_cons = pool.cons(early_atom_1, mid_atom);  // Uses early_atom_1
    size_t checkpoint_b = pool.size();
    assert(checkpoint_b == checkpoint_a + 2);  // mid_atom, mid_cons
    
    // Re-intern early cons in Frame B - should not log since already exists
    const expr* early_cons_again = pool.cons(early_atom_1, early_atom_2);
    assert(early_cons == early_cons_again);
    assert(pool.size() == checkpoint_b);  // Size unchanged
    
    // Add more new content in Frame B
    const expr* late_atom = pool.atom("late_atom");
    const expr* late_cons = pool.cons(late_atom, mid_atom);
    size_t checkpoint_b_final = pool.size();
    assert(checkpoint_b_final == checkpoint_b + 2);
    
    t.push();  // Frame C
    // Re-intern content from both Frame A and Frame B
    const expr* early_cons_again2 = pool.cons(early_atom_1, early_atom_2);
    assert(early_cons == early_cons_again2);
    const expr* mid_cons_again = pool.cons(early_atom_1, mid_atom);
    assert(mid_cons == mid_cons_again);
    assert(pool.size() == checkpoint_b_final);  // Size unchanged
    
    // Add new content in Frame C
    const expr* frame_c_atom = pool.atom("frame_c_atom");
    const expr* frame_c_cons = pool.cons(frame_c_atom, early_atom_1);
    size_t checkpoint_c = pool.size();
    assert(checkpoint_c == checkpoint_b_final + 2);
    
    // Pop Frame C - only frame_c_atom and frame_c_cons should be removed
    t.pop();
    assert(pool.size() == checkpoint_b_final);
    
    // Verify early and mid content still exist
    const expr* verify_early_cons = pool.cons(early_atom_1, early_atom_2);
    assert(verify_early_cons == early_cons);
    const expr* verify_mid_cons = pool.cons(early_atom_1, mid_atom);
    assert(verify_mid_cons == mid_cons);
    assert(pool.size() == checkpoint_b_final);  // Still unchanged
    
    // Pop Frame B - should remove mid_atom, mid_cons, late_atom, late_cons but NOT early content
    t.pop();
    assert(pool.size() == checkpoint_a);
    
    // Verify early content still exists
    const expr* verify_early_cons_after_b = pool.cons(early_atom_1, early_atom_2);
    assert(verify_early_cons_after_b == early_cons);
    assert(pool.size() == checkpoint_a);  // Still unchanged
    
    // Pop Frame A - should remove early atoms and cons
    t.pop();
    assert(pool.size() == checkpoint_start);
    
    // Pop initial frame
    t.pop();
    assert(pool.size() == 0);
}

// void test_causal_set_empty() {
//     // Helper to create fulfillments
//     auto make_fulfillment = [](uint32_t c_id, uint32_t r_id) -> fulfillment {
//         return fulfillment{constraint_id{c_id}, r_id};
//     };
    
//     // Test default-constructed causal_set is empty
//     causal_set cs1;
//     assert(cs1.empty() == true);
    
//     // Test causal_set constructed with empty set is empty
//     std::set<fulfillment> empty_set;
//     causal_set cs2(empty_set);
//     assert(cs2.empty() == true);
    
//     // Test non-empty causal_set with 1 element is not empty
//     std::set<fulfillment> set1;
//     set1.insert(make_fulfillment(1, 1));
//     causal_set cs3(set1);
//     assert(cs3.empty() == false);
    
//     // Test non-empty causal_set with multiple elements is not empty
//     std::set<fulfillment> set2;
//     set2.insert(make_fulfillment(1, 1));
//     set2.insert(make_fulfillment(2, 2));
//     set2.insert(make_fulfillment(3, 3));
//     causal_set cs4(set2);
//     assert(cs4.empty() == false);
    
//     // Test non-empty causal_set with many elements is not empty
//     std::set<fulfillment> set3;
//     for (uint32_t i = 0; i < 50; i++) {
//         set3.insert(make_fulfillment(i, i));
//     }
//     causal_set cs5(set3);
//     assert(cs5.empty() == false);
// }

// void test_causal_set_size() {
//     // Helper to create fulfillments
//     auto make_fulfillment = [](uint32_t c_id, uint32_t r_id) -> fulfillment {
//         return fulfillment{constraint_id{c_id}, r_id};
//     };
    
//     // Test default-constructed causal_set has size 0
//     causal_set cs1;
//     assert(cs1.size() == 0);
    
//     // Test causal_set constructed with empty set has size 0
//     std::set<fulfillment> empty_set;
//     causal_set cs2(empty_set);
//     assert(cs2.size() == 0);
    
//     // Test causal_set with 1 element
//     std::set<fulfillment> set1;
//     set1.insert(make_fulfillment(1, 1));
//     causal_set cs3(set1);
//     assert(cs3.size() == 1);
    
//     // Test causal_set with 2 elements
//     std::set<fulfillment> set2;
//     set2.insert(make_fulfillment(1, 1));
//     set2.insert(make_fulfillment(2, 2));
//     causal_set cs4(set2);
//     assert(cs4.size() == 2);
    
//     // Test causal_set with 3 elements
//     std::set<fulfillment> set3;
//     set3.insert(make_fulfillment(1, 1));
//     set3.insert(make_fulfillment(2, 2));
//     set3.insert(make_fulfillment(3, 3));
//     causal_set cs5(set3);
//     assert(cs5.size() == 3);
    
//     // Test causal_set with 5 elements
//     std::set<fulfillment> set4;
//     set4.insert(make_fulfillment(10, 10));
//     set4.insert(make_fulfillment(20, 20));
//     set4.insert(make_fulfillment(30, 30));
//     set4.insert(make_fulfillment(40, 40));
//     set4.insert(make_fulfillment(50, 50));
//     causal_set cs6(set4);
//     assert(cs6.size() == 5);
    
//     // Test causal_set with many elements
//     std::set<fulfillment> set5;
//     for (uint32_t i = 0; i < 100; i++) {
//         set5.insert(make_fulfillment(i, i));
//     }
//     causal_set cs7(set5);
//     assert(cs7.size() == 100);
    
//     // Test that duplicate insertions in std::set don't increase size
//     std::set<fulfillment> set6;
//     set6.insert(make_fulfillment(1, 1));
//     set6.insert(make_fulfillment(1, 1));  // Duplicate
//     set6.insert(make_fulfillment(1, 1));  // Duplicate
//     causal_set cs8(set6);
//     assert(cs8.size() == 1);  // Only one unique element
// }

// void test_causal_set_count() {
//     // Helper to create fulfillments
//     auto make_fulfillment = [](uint32_t c_id, uint32_t r_id) -> fulfillment {
//         return fulfillment{constraint_id{c_id}, r_id};
//     };
    
//     // Test count on empty set
//     causal_set cs1;
//     assert(cs1.count(make_fulfillment(1, 1)) == false);
    
//     // Test count on set with 1 element
//     std::set<fulfillment> set1;
//     set1.insert(make_fulfillment(1, 1));
//     causal_set cs2(set1);
//     assert(cs2.count(make_fulfillment(1, 1)) == true);
//     assert(cs2.count(make_fulfillment(2, 2)) == false);
    
//     // Test count on set with multiple elements
//     std::set<fulfillment> set2;
//     set2.insert(make_fulfillment(1, 1));
//     set2.insert(make_fulfillment(2, 2));
//     set2.insert(make_fulfillment(3, 3));
//     causal_set cs3(set2);
    
//     assert(cs3.count(make_fulfillment(1, 1)) == true);
//     assert(cs3.count(make_fulfillment(2, 2)) == true);
//     assert(cs3.count(make_fulfillment(3, 3)) == true);
//     assert(cs3.count(make_fulfillment(4, 4)) == false);
//     assert(cs3.count(make_fulfillment(0, 0)) == false);
    
//     // Test count with various constraint_id and rule_id combinations
//     std::set<fulfillment> set3;
//     set3.insert(make_fulfillment(10, 20));
//     set3.insert(make_fulfillment(20, 10));
//     set3.insert(make_fulfillment(10, 10));
//     causal_set cs4(set3);
    
//     assert(cs4.count(make_fulfillment(10, 20)) == true);
//     assert(cs4.count(make_fulfillment(20, 10)) == true);
//     assert(cs4.count(make_fulfillment(10, 10)) == true);
//     assert(cs4.count(make_fulfillment(10, 21)) == false);
//     assert(cs4.count(make_fulfillment(11, 20)) == false);
    
//     // Test count with large IDs
//     std::set<fulfillment> set4;
//     set4.insert(make_fulfillment(UINT32_MAX, UINT32_MAX));
//     causal_set cs5(set4);
    
//     assert(cs5.count(make_fulfillment(UINT32_MAX, UINT32_MAX)) == true);
//     assert(cs5.count(make_fulfillment(UINT32_MAX, 0)) == false);
//     assert(cs5.count(make_fulfillment(0, UINT32_MAX)) == false);
// }

// void test_causal_set_operator_plus() {
//     // Helper to create fulfillments
//     auto make_fulfillment = [](uint32_t c_id, uint32_t r_id) -> fulfillment {
//         return fulfillment{constraint_id{c_id}, r_id};
//     };
    
//     // Test empty + empty = empty
//     causal_set cs1;
//     causal_set cs2;
//     causal_set result = cs1 + cs2;
//     assert(result.empty());
//     assert(result.size() == 0);
    
//     // Test empty + non-empty = non-empty
//     std::set<fulfillment> set1;
//     set1.insert(make_fulfillment(1, 1));
//     causal_set cs3(set1);
//     causal_set result2 = cs1 + cs3;
//     assert(result2.size() == 1);
//     assert(!result2.empty());
    
//     // Test non-empty + empty = non-empty
//     causal_set result3 = cs3 + cs1;
//     assert(result3.size() == 1);
//     assert(!result3.empty());
    
//     // Test commutativity: a + b == b + a (in terms of size)
//     std::set<fulfillment> set2;
//     set2.insert(make_fulfillment(2, 2));
//     set2.insert(make_fulfillment(3, 3));
//     causal_set cs4(set2);
    
//     causal_set ab = cs3 + cs4;
//     causal_set ba = cs4 + cs3;
//     assert(ab.size() == ba.size());
//     assert(ab.size() == 3);  // 1 + 2 = 3
    
//     // Test associativity: (a + b) + c == a + (b + c)
//     std::set<fulfillment> set3;
//     set3.insert(make_fulfillment(4, 4));
//     causal_set cs5(set3);
    
//     causal_set left = (cs3 + cs4) + cs5;
//     causal_set right = cs3 + (cs4 + cs5);
//     assert(left.size() == right.size());
//     assert(left.size() == 4);  // 1 + 2 + 1 = 4
    
//     // Test that duplicates are handled correctly (set behavior)
//     std::set<fulfillment> set4;
//     set4.insert(make_fulfillment(1, 1));  // Duplicate
//     set4.insert(make_fulfillment(5, 5));  // New
//     causal_set cs6(set4);
    
//     causal_set result4 = cs3 + cs6;
//     assert(result4.size() == 2);  // {1,1} and {5,5}, not 3
    
//     // Test multiple additions
//     std::set<fulfillment> set5;
//     set5.insert(make_fulfillment(6, 6));
//     causal_set cs7(set5);
    
//     causal_set result5 = cs3 + cs4 + cs5 + cs6 + cs7;
//     assert(result5.size() == 6);  // {1,1}, {2,2}, {3,3}, {4,4}, {5,5}, {6,6}
    
//     // Test identity: a + empty = a
//     causal_set identity = cs3 + cs1;
//     assert(identity.size() == cs3.size());
    
//     // Test with overlapping sets
//     std::set<fulfillment> set6;
//     set6.insert(make_fulfillment(1, 1));
//     set6.insert(make_fulfillment(2, 2));
//     causal_set cs8(set6);
    
//     std::set<fulfillment> set7;
//     set7.insert(make_fulfillment(2, 2));
//     set7.insert(make_fulfillment(3, 3));
//     causal_set cs9(set7);
    
//     causal_set overlap = cs8 + cs9;
//     assert(overlap.size() == 3);  // {1,1}, {2,2}, {3,3} - {2,2} appears once
    
//     // Test self-addition
//     causal_set self = cs3 + cs3;
//     assert(self.size() == 1);  // Adding to itself doesn't duplicate
// }

// void test_causal_set_operator_spaceship() {
//     // Helper to create fulfillments
//     auto make_fulfillment = [](uint32_t c_id, uint32_t r_id) -> fulfillment {
//         return fulfillment{constraint_id{c_id}, r_id};
//     };
    
//     // Test that two empty causal_sets are equal
//     causal_set cs1;
//     causal_set cs2;
//     assert((cs1 <=> cs2) == std::strong_ordering::equal);
    
//     // Test reflexivity: a <=> a == equal
//     assert((cs1 <=> cs1) == std::strong_ordering::equal);
    
//     // Test with non-empty sets
//     std::set<fulfillment> set1;
//     set1.insert(make_fulfillment(1, 1));
//     causal_set cs3(set1);
//     assert((cs3 <=> cs3) == std::strong_ordering::equal);
    
//     // Test size comparison: smaller size is less
//     std::set<fulfillment> set2;
//     set2.insert(make_fulfillment(1, 1));
//     set2.insert(make_fulfillment(2, 2));
//     causal_set cs4(set2);
    
//     assert((cs3 <=> cs4) == std::strong_ordering::less);
//     assert((cs4 <=> cs3) == std::strong_ordering::greater);
    
//     // Test empty vs non-empty
//     assert((cs1 <=> cs3) == std::strong_ordering::less);
//     assert((cs3 <=> cs1) == std::strong_ordering::greater);
    
//     // Test same size, different content (lexicographic comparison)
//     std::set<fulfillment> set3;
//     set3.insert(make_fulfillment(1, 1));
//     causal_set cs5(set3);
    
//     std::set<fulfillment> set4;
//     set4.insert(make_fulfillment(2, 2));
//     causal_set cs6(set4);
    
//     // Both have size 1, but different fulfillments
//     // Comparison should be based on set comparison
//     auto cmp = cs5 <=> cs6;
//     assert(cmp != std::strong_ordering::equal);
    
//     // Test same size, same content
//     std::set<fulfillment> set5;
//     set5.insert(make_fulfillment(1, 1));
//     causal_set cs7(set5);
//     assert((cs5 <=> cs7) == std::strong_ordering::equal);
    
//     // Test transitivity: if a < b and b < c, then a < c
//     std::set<fulfillment> set6;
//     set6.insert(make_fulfillment(1, 1));
//     set6.insert(make_fulfillment(2, 2));
//     set6.insert(make_fulfillment(3, 3));
//     causal_set cs8(set6);
    
//     // cs3 has 1 element, cs4 has 2, cs8 has 3
//     assert((cs3 <=> cs4) == std::strong_ordering::less);
//     assert((cs4 <=> cs8) == std::strong_ordering::less);
//     assert((cs3 <=> cs8) == std::strong_ordering::less);
    
//     // Test with multiple elements, same size
//     std::set<fulfillment> set7;
//     set7.insert(make_fulfillment(1, 1));
//     set7.insert(make_fulfillment(3, 3));
//     causal_set cs9(set7);
    
//     std::set<fulfillment> set8;
//     set8.insert(make_fulfillment(2, 2));
//     set8.insert(make_fulfillment(4, 4));
//     causal_set cs10(set8);
    
//     // Both have size 2, compare lexicographically
//     auto cmp2 = cs9 <=> cs10;
//     assert(cmp2 != std::strong_ordering::equal);
    
//     // Test that operator+ preserves comparison properties
//     causal_set combined1 = cs3 + cs4;
//     causal_set combined2 = cs3 + cs4;
//     assert((combined1 <=> combined2) == std::strong_ordering::equal);
    
//     // Test antisymmetry: if a <= b and b <= a, then a == b
//     if ((cs5 <=> cs7) == std::strong_ordering::equal) {
//         assert((cs7 <=> cs5) == std::strong_ordering::equal);
//     }
    
//     // Test size takes precedence over content
//     std::set<fulfillment> set9;
//     set9.insert(make_fulfillment(100, 100));  // Large ID
//     causal_set cs11(set9);
    
//     std::set<fulfillment> set10;
//     set10.insert(make_fulfillment(1, 1));
//     set10.insert(make_fulfillment(2, 2));
//     causal_set cs12(set10);
    
//     // cs11 has 1 element, cs12 has 2, so cs11 < cs12 regardless of content
//     assert((cs11 <=> cs12) == std::strong_ordering::less);
// }

void test_bind_map_whnf() {
    trail t;
    
    // Test 1: whnf of atom returns itself
    {
        bind_map bm(t);
        expr a1{expr::atom{"test"}};
        const expr* result = bm.whnf(&a1);
        assert(result == &a1);
        assert(bm.bindings.size() == 0);  // No bindings created for non-vars
    }
    
    // Test 2: whnf of cons returns itself
    {
        bind_map bm(t);
        expr a1{expr::atom{"left"}};
        expr a2{expr::atom{"right"}};
        expr c1{expr::cons{&a1, &a2}};
        const expr* result = bm.whnf(&c1);
        assert(result == &c1);
        assert(bm.bindings.size() == 0);
    }
    
    // Test 3: whnf of unbound var returns itself
    {
        bind_map bm(t);
        expr v1{expr::var{0}};
        const expr* result = bm.whnf(&v1);
        assert(result == &v1);
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.at(0) == nullptr);
    }
    
    // Test 4: whnf of bound var returns the binding
    {
        bind_map bm(t);
        expr v1{expr::var{1}};
        expr a1{expr::atom{"bound"}};
        bm.bindings[1] = &a1;
        
        const expr* result = bm.whnf(&v1);
        assert(result == &a1);
        assert(bm.bindings.size() == 1);
    }
    
    // Test 5: whnf with chain of var bindings (path compression)
    {
        bind_map bm(t);
        expr v1{expr::var{10}};
        expr v2{expr::var{11}};
        expr v3{expr::var{12}};
        expr a1{expr::atom{"end"}};
        
        // Create chain: v1 -> v2 -> v3 -> a1
        bm.bindings[10] = &v2;
        bm.bindings[11] = &v3;
        bm.bindings[12] = &a1;
        assert(bm.bindings.size() == 3);
        
        const expr* result = bm.whnf(&v1);
        assert(result == &a1);
        
        // Verify path compression: v1 should now point directly to a1
        assert(bm.bindings.at(10) == &a1);
        // v2 should also be compressed to a1
        assert(bm.bindings.at(11) == &a1);
        assert(bm.bindings.size() == 3);
    }
    
    // Test 6: whnf with long chain
    {
        bind_map bm(t);
        expr v1{expr::var{20}};
        expr v2{expr::var{21}};
        expr v3{expr::var{22}};
        expr v4{expr::var{23}};
        expr v5{expr::var{24}};
        expr a1{expr::atom{"final"}};
        
        // Create chain: v1 -> v2 -> v3 -> v4 -> v5 -> a1
        bm.bindings[20] = &v2;
        bm.bindings[21] = &v3;
        bm.bindings[22] = &v4;
        bm.bindings[23] = &v5;
        bm.bindings[24] = &a1;
        assert(bm.bindings.size() == 5);
        
        const expr* result = bm.whnf(&v1);
        assert(result == &a1);
        
        // Verify all intermediate vars are compressed to a1
        assert(bm.bindings.at(20) == &a1);
        assert(bm.bindings.at(21) == &a1);
        assert(bm.bindings.at(22) == &a1);
        assert(bm.bindings.at(23) == &a1);
        assert(bm.bindings.size() == 5);
    }
    
    // Test 7: whnf with var bound to cons
    {
        bind_map bm(t);
        expr v1{expr::var{30}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        bm.bindings[30] = &c1;
        assert(bm.bindings.size() == 1);
        
        const expr* result = bm.whnf(&v1);
        assert(result == &c1);
        assert(std::holds_alternative<expr::cons>(result->content));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 8: whnf with var bound to another var bound to cons
    {
        bind_map bm(t);
        expr v1{expr::var{40}};
        expr v2{expr::var{41}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr c1{expr::cons{&a1, &a2}};
        
        bm.bindings[40] = &v2;
        bm.bindings[41] = &c1;
        assert(bm.bindings.size() == 2);
        
        const expr* result = bm.whnf(&v1);
        assert(result == &c1);
        // Verify path compression
        assert(bm.bindings.at(40) == &c1);
        assert(bm.bindings.size() == 2);
    }
    
    // Test 9: Multiple unbound vars remain unbound
    {
        bind_map bm(t);
        expr v1{expr::var{50}};
        expr v2{expr::var{51}};
        expr v3{expr::var{52}};
        
        assert(bm.whnf(&v1) == &v1);
        assert(bm.bindings.size() == 1);
        
        assert(bm.whnf(&v2) == &v2);
        assert(bm.bindings.size() == 2);
        
        assert(bm.whnf(&v3) == &v3);
        assert(bm.bindings.size() == 3);
        
        // Verify all entries are null
        assert(bm.bindings.at(50) == nullptr);
        assert(bm.bindings.at(51) == nullptr);
        assert(bm.bindings.at(52) == nullptr);
    }
    
    // Test 10: whnf called multiple times on same var with binding
    {
        bind_map bm(t);
        expr v1{expr::var{60}};
        expr a1{expr::atom{"repeated"}};
        bm.bindings[60] = &a1;
        assert(bm.bindings.size() == 1);
        
        const expr* r1 = bm.whnf(&v1);
        const expr* r2 = bm.whnf(&v1);
        const expr* r3 = bm.whnf(&v1);
        
        assert(r1 == &a1);
        assert(r2 == &a1);
        assert(r3 == &a1);
        assert(bm.bindings.size() == 1);
    }
    
    // Test 11: whnf with var bound to var bound to var (partial chain)
    {
        bind_map bm(t);
        expr v1{expr::var{70}};
        expr v2{expr::var{71}};
        expr v3{expr::var{72}};
        
        // Chain: v1 -> v2 -> v3 (v3 is unbound)
        bm.bindings[70] = &v2;
        bm.bindings[71] = &v3;
        bm.bindings[72] = nullptr;  // Explicitly unbound
        assert(bm.bindings.size() == 3);
        
        const expr* result = bm.whnf(&v1);
        assert(result == &v3);  // Should return v3, the unbound var
        
        // Verify path compression to v3
        assert(bm.bindings.at(70) == &v3);
        assert(bm.bindings.at(71) == &v3);
        assert(bm.bindings.size() == 3);
    }
    
    // Test 12: whnf with nested cons structures
    {
        bind_map bm(t);
        expr v1{expr::var{80}};
        expr a1{expr::atom{"inner"}};
        expr a2{expr::atom{"outer"}};
        expr inner_cons{expr::cons{&a1, &a2}};
        expr a3{expr::atom{"wrap"}};
        expr outer_cons{expr::cons{&inner_cons, &a3}};
        
        bm.bindings[80] = &outer_cons;
        assert(bm.bindings.size() == 1);
        
        const expr* result = bm.whnf(&v1);
        assert(result == &outer_cons);
        assert(std::holds_alternative<expr::cons>(result->content));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 13: whnf with different var indices
    {
        bind_map bm(t);
        expr v_small{expr::var{0}};
        expr v_large{expr::var{UINT32_MAX}};
        expr a1{expr::atom{"small"}};
        expr a2{expr::atom{"large"}};
        
        bm.bindings[0] = &a1;
        bm.bindings[UINT32_MAX] = &a2;
        assert(bm.bindings.size() == 2);
        
        assert(bm.whnf(&v_small) == &a1);
        assert(bm.whnf(&v_large) == &a2);
        assert(bm.bindings.size() == 2);
    }
    
    // Test 14: whnf preserves atom values
    {
        bind_map bm(t);
        expr a1{expr::atom{""}};
        expr a2{expr::atom{"test123"}};
        expr a3{expr::atom{"!@#$%"}};
        
        assert(bm.whnf(&a1) == &a1);
        assert(bm.whnf(&a2) == &a2);
        assert(bm.whnf(&a3) == &a3);
        assert(bm.bindings.size() == 0);
    }
    
    // Test 15: whnf with complex chain ending in cons
    {
        bind_map bm(t);
        expr v1{expr::var{90}};
        expr v2{expr::var{91}};
        expr v3{expr::var{92}};
        expr a1{expr::atom{"left"}};
        expr a2{expr::atom{"right"}};
        expr c1{expr::cons{&a1, &a2}};
        
        // Chain: v1 -> v2 -> v3 -> c1
        bm.bindings[90] = &v2;
        bm.bindings[91] = &v3;
        bm.bindings[92] = &c1;
        assert(bm.bindings.size() == 3);
        
        const expr* result = bm.whnf(&v1);
        assert(result == &c1);
        
        // Verify all vars compressed to c1
        assert(bm.bindings.at(90) == &c1);
        assert(bm.bindings.at(91) == &c1);
        assert(bm.bindings.size() == 3);
    }
    
    // Test 16: WHNF does NOT reduce vars inside cons (WEAK head, not strong)
    {
        bind_map bm(t);
        expr v1{expr::var{100}};
        expr v2{expr::var{101}};
        expr a1{expr::atom{"bound_atom"}};
        
        // Bind v2 to an atom
        bm.bindings[101] = &a1;
        
        // Create cons with v1 and v2 as children
        expr c1{expr::cons{&v1, &v2}};
        
        // whnf of the cons should return the cons itself, NOT reduce children
        const expr* result = bm.whnf(&c1);
        assert(result == &c1);
        assert(std::holds_alternative<expr::cons>(result->content));
        
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        // Children should still be the original var pointers, NOT reduced
        assert(cons_ref.lhs == &v1);
        assert(cons_ref.rhs == &v2);
        assert(bm.bindings.size() == 1);
    }
    
    // Test 17: Var bound to cons containing bound vars - children not reduced
    {
        bind_map bm(t);
        expr v_outer{expr::var{110}};
        expr v_left{expr::var{111}};
        expr v_right{expr::var{112}};
        expr a1{expr::atom{"left_val"}};
        expr a2{expr::atom{"right_val"}};
        
        // Bind the inner vars
        bm.bindings[111] = &a1;
        bm.bindings[112] = &a2;
        
        // Create cons with bound vars as children
        expr c1{expr::cons{&v_left, &v_right}};
        bm.bindings[110] = &c1;
        assert(bm.bindings.size() == 3);
        
        const expr* result = bm.whnf(&v_outer);
        assert(result == &c1);
        
        // The cons children should still point to vars, not atoms
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        assert(cons_ref.lhs == &v_left);
        assert(cons_ref.rhs == &v_right);
        assert(bm.bindings.size() == 3);
    }
    
    // Test 18: Chain ending in cons with bound vars inside
    {
        bind_map bm(t);
        expr v_chain1{expr::var{120}};
        expr v_chain2{expr::var{121}};
        expr v_inner1{expr::var{122}};
        expr v_inner2{expr::var{123}};
        expr a1{expr::atom{"inner_bound"}};
        
        bm.bindings[122] = &a1;
        
        expr c1{expr::cons{&v_inner1, &v_inner2}};
        bm.bindings[120] = &v_chain2;
        bm.bindings[121] = &c1;
        assert(bm.bindings.size() == 3);
        
        const expr* result = bm.whnf(&v_chain1);
        assert(result == &c1);
        
        // Cons children should be unchanged
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        assert(cons_ref.lhs == &v_inner1);
        assert(cons_ref.rhs == &v_inner2);
        
        // Path compression on outer chain
        assert(bm.bindings.at(120) == &c1);
        assert(bm.bindings.size() == 3);
    }
    
    // Test 19: Deeply nested cons with vars at all levels
    {
        bind_map bm(t);
        expr v_outer{expr::var{130}};
        expr v1{expr::var{131}};
        expr v2{expr::var{132}};
        expr v3{expr::var{133}};
        expr v4{expr::var{134}};
        
        // Bind some inner vars
        expr a1{expr::atom{"bound1"}};
        expr a2{expr::atom{"bound2"}};
        bm.bindings[131] = &a1;
        bm.bindings[133] = &a2;
        
        // Create nested structure: cons(cons(v1, v2), cons(v3, v4))
        expr inner_left{expr::cons{&v1, &v2}};
        expr inner_right{expr::cons{&v3, &v4}};
        expr outer{expr::cons{&inner_left, &inner_right}};
        
        bm.bindings[130] = &outer;
        assert(bm.bindings.size() == 3);
        
        const expr* result = bm.whnf(&v_outer);
        assert(result == &outer);
        
        // Verify structure is unchanged - vars inside cons are not reduced
        const expr::cons& outer_cons = std::get<expr::cons>(result->content);
        assert(outer_cons.lhs == &inner_left);
        assert(outer_cons.rhs == &inner_right);
        
        const expr::cons& left_cons = std::get<expr::cons>(outer_cons.lhs->content);
        assert(left_cons.lhs == &v1);  // Still points to var, not a1
        assert(left_cons.rhs == &v2);
        
        const expr::cons& right_cons = std::get<expr::cons>(outer_cons.rhs->content);
        assert(right_cons.lhs == &v3);  // Still points to var, not a2
        assert(right_cons.rhs == &v4);
        
        assert(bm.bindings.size() == 3);
    }
    
    // Test 20: Var bound to cons of vars, one of which chains to atom
    {
        bind_map bm(t);
        expr v_outer{expr::var{140}};
        expr v_left{expr::var{141}};
        expr v_right{expr::var{142}};
        expr v_chain{expr::var{143}};
        expr a1{expr::atom{"chained"}};
        
        // v_left chains to atom
        bm.bindings[141] = &v_chain;
        bm.bindings[143] = &a1;
        
        expr c1{expr::cons{&v_left, &v_right}};
        bm.bindings[140] = &c1;
        assert(bm.bindings.size() == 3);
        
        const expr* result = bm.whnf(&v_outer);
        assert(result == &c1);
        
        // Cons children should be unchanged - v_left is NOT reduced to a1
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        assert(cons_ref.lhs == &v_left);
        assert(cons_ref.rhs == &v_right);
        assert(bm.bindings.size() == 3);
    }
    
    // Test 21: Multiple vars bound to same atom
    {
        bind_map bm(t);
        expr v1{expr::var{150}};
        expr v2{expr::var{151}};
        expr v3{expr::var{152}};
        expr a1{expr::atom{"shared"}};
        
        bm.bindings[150] = &a1;
        bm.bindings[151] = &a1;
        bm.bindings[152] = &a1;
        assert(bm.bindings.size() == 3);
        
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);
        assert(bm.whnf(&v3) == &a1);
        assert(bm.bindings.size() == 3);
    }
    
    // Test 22: Multiple vars bound to same cons
    {
        bind_map bm(t);
        expr v1{expr::var{160}};
        expr v2{expr::var{161}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        bm.bindings[160] = &c1;
        bm.bindings[161] = &c1;
        assert(bm.bindings.size() == 2);
        
        assert(bm.whnf(&v1) == &c1);
        assert(bm.whnf(&v2) == &c1);
        assert(bm.bindings.size() == 2);
    }
    
    // Test 23: Cons of unbound vars
    {
        bind_map bm(t);
        expr v1{expr::var{170}};
        expr v2{expr::var{171}};
        expr c1{expr::cons{&v1, &v2}};
        
        const expr* result = bm.whnf(&c1);
        assert(result == &c1);
        
        // Cons children remain as unbound vars
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        assert(cons_ref.lhs == &v1);
        assert(cons_ref.rhs == &v2);
        assert(bm.bindings.size() == 0);  // No bindings created
    }
    
    // Test 24: Cons of mix (atom and var)
    {
        bind_map bm(t);
        expr a1{expr::atom{"atom"}};
        expr v1{expr::var{180}};
        expr c1{expr::cons{&a1, &v1}};
        
        const expr* result = bm.whnf(&c1);
        assert(result == &c1);
        
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        assert(cons_ref.lhs == &a1);
        assert(cons_ref.rhs == &v1);  // Var not reduced
        assert(bm.bindings.size() == 0);
    }
    
    // Test 25: Cons of mix (var and cons)
    {
        bind_map bm(t);
        expr v1{expr::var{190}};
        expr a1{expr::atom{"inner"}};
        expr a2{expr::atom{"inner2"}};
        expr inner_cons{expr::cons{&a1, &a2}};
        expr outer_cons{expr::cons{&v1, &inner_cons}};
        
        const expr* result = bm.whnf(&outer_cons);
        assert(result == &outer_cons);
        
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        assert(cons_ref.lhs == &v1);  // Var not reduced
        assert(cons_ref.rhs == &inner_cons);
        assert(bm.bindings.size() == 0);
    }
    
    // Test 26: Var bound to cons of bound vars - only outer reduced
    {
        bind_map bm(t);
        expr v_outer{expr::var{200}};
        expr v_inner1{expr::var{201}};
        expr v_inner2{expr::var{202}};
        expr a1{expr::atom{"val1"}};
        expr a2{expr::atom{"val2"}};
        
        // Bind inner vars
        bm.bindings[201] = &a1;
        bm.bindings[202] = &a2;
        
        // Create cons with bound vars
        expr c1{expr::cons{&v_inner1, &v_inner2}};
        bm.bindings[200] = &c1;
        assert(bm.bindings.size() == 3);
        
        const expr* result = bm.whnf(&v_outer);
        assert(result == &c1);
        
        // Children should STILL be vars, not reduced to atoms
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        assert(cons_ref.lhs == &v_inner1);
        assert(cons_ref.rhs == &v_inner2);
        assert(std::holds_alternative<expr::var>(cons_ref.lhs->content));
        assert(std::holds_alternative<expr::var>(cons_ref.rhs->content));
        assert(bm.bindings.size() == 3);
    }
    
    // Test 27: Chain to cons of chained vars
    {
        bind_map bm(t);
        expr v_outer{expr::var{210}};
        expr v_mid{expr::var{211}};
        expr v_left{expr::var{212}};
        expr v_right{expr::var{213}};
        expr v_left_chain{expr::var{214}};
        expr a1{expr::atom{"left_end"}};
        
        // v_left chains to atom
        bm.bindings[212] = &v_left_chain;
        bm.bindings[214] = &a1;
        
        // Cons contains chained var and unbound var
        expr c1{expr::cons{&v_left, &v_right}};
        bm.bindings[210] = &v_mid;
        bm.bindings[211] = &c1;
        assert(bm.bindings.size() == 4);
        
        const expr* result = bm.whnf(&v_outer);
        assert(result == &c1);
        
        // Cons children should be unchanged - not reduced
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        assert(cons_ref.lhs == &v_left);  // Still v_left, not a1
        assert(cons_ref.rhs == &v_right);
        
        // Outer chain should be compressed
        assert(bm.bindings.at(210) == &c1);
        assert(bm.bindings.size() == 4);
    }
    
    // Test 28: Deeply nested cons with vars everywhere
    {
        bind_map bm(t);
        expr v1{expr::var{220}};
        expr v2{expr::var{221}};
        expr v3{expr::var{222}};
        expr v4{expr::var{223}};
        expr a1{expr::atom{"deep"}};
        
        // Bind v4
        bm.bindings[223] = &a1;
        
        // Create: cons(cons(v1, v2), cons(v3, v4))
        expr inner_left{expr::cons{&v1, &v2}};
        expr inner_right{expr::cons{&v3, &v4}};
        expr outer{expr::cons{&inner_left, &inner_right}};
        
        const expr* result = bm.whnf(&outer);
        assert(result == &outer);
        
        // All structure should be preserved, vars not reduced
        const expr::cons& outer_cons = std::get<expr::cons>(result->content);
        assert(outer_cons.lhs == &inner_left);
        assert(outer_cons.rhs == &inner_right);
        
        const expr::cons& left_cons = std::get<expr::cons>(outer_cons.lhs->content);
        assert(left_cons.lhs == &v1);
        assert(left_cons.rhs == &v2);
        
        const expr::cons& right_cons = std::get<expr::cons>(outer_cons.rhs->content);
        assert(right_cons.lhs == &v3);
        assert(right_cons.rhs == &v4);  // Still v4, not a1
        
        assert(bm.bindings.size() == 1);
    }
    
    // Test 29: Var chain to cons of var chains
    {
        bind_map bm(t);
        expr v_outer{expr::var{230}};
        expr v_outer_chain{expr::var{231}};
        expr v_left{expr::var{232}};
        expr v_left_chain{expr::var{233}};
        expr v_right{expr::var{234}};
        expr a_left{expr::atom{"left"}};
        expr a_right{expr::atom{"right"}};
        
        // Setup chains
        bm.bindings[232] = &v_left_chain;
        bm.bindings[233] = &a_left;
        bm.bindings[234] = &a_right;
        
        expr c1{expr::cons{&v_left, &v_right}};
        bm.bindings[230] = &v_outer_chain;
        bm.bindings[231] = &c1;
        assert(bm.bindings.size() == 5);
        
        const expr* result = bm.whnf(&v_outer);
        assert(result == &c1);
        
        // Cons children are still original vars, not reduced
        const expr::cons& cons_ref = std::get<expr::cons>(result->content);
        assert(cons_ref.lhs == &v_left);
        assert(cons_ref.rhs == &v_right);
        
        // Outer chain compressed
        assert(bm.bindings.at(230) == &c1);
        assert(bm.bindings.size() == 5);
    }
    
    // Test 30: Empty bindings map behavior
    {
        bind_map bm(t);
        assert(bm.bindings.size() == 0);
        
        expr v1{expr::var{240}};
        bm.whnf(&v1);
        assert(bm.bindings.size() == 1);
        
        expr v2{expr::var{241}};
        bm.whnf(&v2);
        assert(bm.bindings.size() == 2);
    }
    
    // Test 31: Same var called multiple times only creates one entry
    {
        bind_map bm(t);
        expr v1{expr::var{250}};
        
        bm.whnf(&v1);
        assert(bm.bindings.size() == 1);
        
        bm.whnf(&v1);
        assert(bm.bindings.size() == 1);
        
        bm.whnf(&v1);
        assert(bm.bindings.size() == 1);
    }
}

// void test_unification_graph_cin_dijkstra() {
//     trail t;
    
//     // Helper to create dummy fulfillments
//     auto make_fulfillment = [](uint32_t c_id, uint32_t r_id) -> fulfillment {
//         return fulfillment{constraint_id{c_id}, r_id};  // clause_id, candidate_id
//     };
    
//     // Helper to create causal set with N fulfillments
//     auto make_cause = [&](std::initializer_list<std::pair<uint32_t, uint32_t>> pairs) -> causal_set {
//         std::set<fulfillment> fulfills;
//         for (auto [c, r] : pairs) {
//             fulfills.insert(make_fulfillment(c, r));
//         }
//         return causal_set(fulfills);
//     };
    
//     // Test 1: Source is already instantiated (atom) - should return immediately
//     {
//         unification_graph ug(t);
//         expr atom1{expr::atom{"test"}};
//         ug.edges[&atom1];  // Initialize entry
//         auto [found, cause] = ug.cin_dijkstra(&atom1);
//         assert(found == true);
//         assert(cause.empty());  // No path needed, already at instantiated node
//     }
    
//     // Test 2: Source is already instantiated (cons) - should return immediately
//     {
//         unification_graph ug(t);
//         expr a1{expr::atom{"a"}};
//         expr a2{expr::atom{"b"}};
//         expr cons1{expr::cons{&a1, &a2}};
//         ug.edges[&cons1];  // Initialize entry
//         auto [found, cause] = ug.cin_dijkstra(&cons1);
//         assert(found == true);
//         assert(cause.empty());
//     }
    
//     // Test 3: Single edge from var to atom
//     {
//         unification_graph ug(t);
//         expr var1{expr::var{0}};
//         expr atom1{expr::atom{"target"}};
        
//         causal_set edge_cause = make_cause({{1, 1}});
//         ug.edges[&var1].insert({&atom1, edge_cause});
//         ug.edges[&atom1];  // Initialize atom entry
        
//         auto [found, cause] = ug.cin_dijkstra(&var1);
//         assert(found == true);
//         assert(cause.size() == 1);
//         assert((cause <=> edge_cause) == std::strong_ordering::equal);
//     }
    
//     // Test 4: No instantiated node reachable (var -> var -> var)
//     {
//         unification_graph ug(t);
//         expr var1{expr::var{1}};
//         expr var2{expr::var{2}};
//         expr var3{expr::var{3}};
        
//         causal_set cause12 = make_cause({{2, 1}});
//         causal_set cause23 = make_cause({{3, 1}});
        
//         ug.edges[&var1].insert({&var2, cause12});
//         ug.edges[&var2].insert({&var3, cause23});
//         ug.edges[&var3];  // Initialize var3 entry
        
//         auto [found, cause] = ug.cin_dijkstra(&var1);
//         assert(found == false);
//         assert(cause.empty());
//     }
    
//     // Test 5: Chain of vars leading to atom (var -> var -> atom)
//     {
//         unification_graph ug(t);
//         expr var1{expr::var{10}};
//         expr var2{expr::var{11}};
//         expr atom1{expr::atom{"end"}};
        
//         causal_set cause12 = make_cause({{10, 1}});
//         causal_set cause23 = make_cause({{11, 1}});
        
//         ug.edges[&var1].insert({&var2, cause12});
//         ug.edges[&var2].insert({&atom1, cause23});
//         ug.edges[&atom1];  // Initialize atom entry
        
//         auto [found, cause] = ug.cin_dijkstra(&var1);
//         assert(found == true);
//         assert(cause.size() == 2);  // Both edges' causes merged
//     }
    
//     // Test 6: Multiple paths - shortest path wins
//     {
//         unification_graph ug(t);
//         expr var_start{expr::var{20}};
//         expr var_mid{expr::var{21}};
//         expr atom_end{expr::atom{"goal"}};
        
//         // Path 1: direct (cost 5)
//         causal_set direct = make_cause({{20, 1}, {20, 2}, {20, 3}, {20, 4}, {20, 5}});
//         // Path 2: indirect (cost 3)
//         causal_set to_mid = make_cause({{21, 1}});
//         causal_set mid_to_end = make_cause({{22, 1}, {22, 2}});
        
//         ug.edges[&var_start].insert({&atom_end, direct});
//         ug.edges[&var_start].insert({&var_mid, to_mid});
//         ug.edges[&var_mid].insert({&atom_end, mid_to_end});
//         ug.edges[&atom_end];
        
//         auto [found, cause] = ug.cin_dijkstra(&var_start);
//         assert(found == true);
//         assert(cause.size() == 3);  // Should take indirect path
//         assert(cause.count(make_fulfillment(21, 1)) == true);
//         assert(cause.count(make_fulfillment(22, 1)) == true);
//         assert(cause.count(make_fulfillment(22, 2)) == true);
//     }
    
//     // Test 7: Path with duplicate fulfillments - duplicates reduce effective cost
//     {
//         unification_graph ug(t);
//         expr var_start{expr::var{30}};
//         expr var_mid{expr::var{31}};
//         expr atom_end{expr::atom{"dup_path"}};
        
//         // Path with overlapping fulfillments
//         causal_set edge1 = make_cause({{100, 1}, {100, 2}, {200, 1}});
//         causal_set edge2 = make_cause({{100, 1}, {300, 1}});  // {100,1} is duplicate
        
//         ug.edges[&var_start].insert({&var_mid, edge1});
//         ug.edges[&var_mid].insert({&atom_end, edge2});
//         ug.edges[&atom_end];
        
//         auto [found, cause] = ug.cin_dijkstra(&var_start);
//         assert(found == true);
//         assert(cause.size() == 4);  // 4 unique, not 5
//         assert(cause.count(make_fulfillment(100, 1)) == true);
//         assert(cause.count(make_fulfillment(100, 2)) == true);
//         assert(cause.count(make_fulfillment(200, 1)) == true);
//         assert(cause.count(make_fulfillment(300, 1)) == true);
//     }
    
//     // Test 8: COMPLEX BIDIRECTIONAL GRAPH with multiple paths
//     {
//         unification_graph ug(t);
        
//         // Create a complex graph with 6 vars and 2 atoms
//         expr v0{expr::var{200}};
//         expr v1{expr::var{201}};
//         expr v2{expr::var{202}};
//         expr v3{expr::var{203}};
//         expr v4{expr::var{204}};
//         expr v5{expr::var{205}};
//         expr atom_a{expr::atom{"target_a"}};
//         expr atom_b{expr::atom{"target_b"}};
        
//         // Create bidirectional edges with SAME causal set in both directions
//         // v0 <-> v1 (cost 2)
//         causal_set c01 = make_cause({{1, 1}, {1, 2}});
        
//         // v0 <-> v2 (cost 1)
//         causal_set c02 = make_cause({{3, 1}});
        
//         // v1 <-> v3 (cost 3)
//         causal_set c13 = make_cause({{5, 1}, {5, 2}, {5, 3}});
        
//         // v2 <-> v3 (cost 1)
//         causal_set c23 = make_cause({{7, 1}});
        
//         // v2 <-> v4 (cost 2)
//         causal_set c24 = make_cause({{9, 1}, {9, 2}});
        
//         // v3 <-> v5 (cost 1)
//         causal_set c35 = make_cause({{11, 1}});
        
//         // v4 <-> v5 (cost 4)
//         causal_set c45 = make_cause({{13, 1}, {13, 2}, {13, 3}, {13, 4}});
        
//         // v5 <-> atom_a (cost 1)
//         causal_set c5a = make_cause({{15, 1}});
        
//         // v4 <-> atom_b (cost 2)
//         causal_set c4b = make_cause({{16, 1}, {16, 2}});
        
//         // Add all bidirectional edges with SAME causal set both ways
//         ug.edges[&v0].insert({&v1, c01});
//         ug.edges[&v1].insert({&v0, c01});
        
//         ug.edges[&v0].insert({&v2, c02});
//         ug.edges[&v2].insert({&v0, c02});
        
//         ug.edges[&v1].insert({&v3, c13});
//         ug.edges[&v3].insert({&v1, c13});
        
//         ug.edges[&v2].insert({&v3, c23});
//         ug.edges[&v3].insert({&v2, c23});
        
//         ug.edges[&v2].insert({&v4, c24});
//         ug.edges[&v4].insert({&v2, c24});
        
//         ug.edges[&v3].insert({&v5, c35});
//         ug.edges[&v5].insert({&v3, c35});
        
//         ug.edges[&v4].insert({&v5, c45});
//         ug.edges[&v5].insert({&v4, c45});
        
//         ug.edges[&v5].insert({&atom_a, c5a});
//         ug.edges[&atom_a].insert({&v5, c5a});
        
//         ug.edges[&v4].insert({&atom_b, c4b});
//         ug.edges[&atom_b].insert({&v4, c4b});
        
//         // Test from v0: Should find shortest path to closest atom
//         // Possible paths to atom_a:
//         // - v0 -> v2 -> v3 -> v5 -> atom_a: cost 1+1+1+1 = 4
//         // - v0 -> v2 -> v4 -> v5 -> atom_a: cost 1+2+4+1 = 8
//         // - v0 -> v1 -> v3 -> v5 -> atom_a: cost 2+3+1+1 = 7
//         // Possible paths to atom_b:
//         // - v0 -> v2 -> v4 -> atom_b: cost 1+2+2 = 5
//         // Shortest is v0 -> v2 -> v3 -> v5 -> atom_a with cost 4
//         auto [found, cause] = ug.cin_dijkstra(&v0);
//         assert(found == true);
//         assert(cause.size() == 4);
//         assert(cause.count(make_fulfillment(3, 1)) == true);  // v0->v2
//         assert(cause.count(make_fulfillment(7, 1)) == true);  // v2->v3
//         assert(cause.count(make_fulfillment(11, 1)) == true); // v3->v5
//         assert(cause.count(make_fulfillment(15, 1)) == true); // v5->atom_a
        
//         // Test from v1: Should also find optimal path
//         auto [found2, cause2] = ug.cin_dijkstra(&v1);
//         assert(found2 == true);
//         // v1 -> v3 -> v5 -> atom_a: cost 3+1+1 = 5
//         // v1 -> v0 -> v2 -> v4 -> atom_b: cost 2+1+2+2 = 7
//         assert(cause2.size() == 5);
        
//         // Test from v4: Should find atom_b directly
//         auto [found3, cause3] = ug.cin_dijkstra(&v4);
//         assert(found3 == true);
//         assert(cause3.size() == 2);
//         assert(cause3.count(make_fulfillment(16, 1)) == true);
//         assert(cause3.count(make_fulfillment(16, 2)) == true);
//     }
    
//     // Test 9: VERY COMPLEX GRAPH with overlapping causes - finds causally minimal path
//     {
//         unification_graph ug(t);
        
//         // Create a complex graph with 8 vars and 3 atoms
//         expr v0{expr::var{300}};
//         expr v1{expr::var{301}};
//         expr v2{expr::var{302}};
//         expr v3{expr::var{303}};
//         expr v4{expr::var{304}};
//         expr v5{expr::var{305}};
//         expr v6{expr::var{306}};
//         expr v7{expr::var{307}};
//         expr atom_x{expr::atom{"target_x"}};
//         expr atom_y{expr::atom{"target_y"}};
//         expr atom_z{expr::atom{"target_z"}};
        
//         // Define shared fulfillments that appear in multiple edges
//         // These create overlaps that reduce effective path costs
//         causal_set shared_a = make_cause({{100, 1}});
//         causal_set shared_b = make_cause({{200, 1}});
//         causal_set shared_c = make_cause({{300, 1}});
        
//         // v0 <-> v1: uses shared_a + unique
//         causal_set c01 = make_cause({{100, 1}, {1, 1}});  // cost 2
        
//         // v0 <-> v2: uses shared_b + unique
//         causal_set c02 = make_cause({{200, 1}, {2, 1}});  // cost 2
        
//         // v1 <-> v3: uses shared_a + unique (overlap with c01!)
//         causal_set c13 = make_cause({{100, 1}, {3, 1}, {3, 2}});  // cost 3, but {100,1} overlaps
        
//         // v2 <-> v4: uses shared_b + unique (overlap with c02!)
//         causal_set c24 = make_cause({{200, 1}, {4, 1}});  // cost 2, but {200,1} overlaps
        
//         // v1 <-> v5: uses shared_c
//         causal_set c15 = make_cause({{300, 1}, {5, 1}, {5, 2}});  // cost 3
        
//         // v3 <-> v6: uses shared_c (overlap with c15!)
//         causal_set c36 = make_cause({{300, 1}, {6, 1}});  // cost 2, but {300,1} overlaps
        
//         // v4 <-> v6: unique
//         causal_set c46 = make_cause({{7, 1}, {7, 2}, {7, 3}});  // cost 3
        
//         // v5 <-> v7: unique
//         causal_set c57 = make_cause({{8, 1}});  // cost 1
        
//         // v6 <-> v7: unique
//         causal_set c67 = make_cause({{9, 1}, {9, 2}});  // cost 2
        
//         // v7 <-> atom_x: unique
//         causal_set c7x = make_cause({{10, 1}});  // cost 1
        
//         // v6 <-> atom_y: unique
//         causal_set c6y = make_cause({{11, 1}, {11, 2}});  // cost 2
        
//         // v4 <-> atom_z: unique
//         causal_set c4z = make_cause({{12, 1}});  // cost 1
        
//         // Add all bidirectional edges
//         ug.edges[&v0].insert({&v1, c01});
//         ug.edges[&v1].insert({&v0, c01});
        
//         ug.edges[&v0].insert({&v2, c02});
//         ug.edges[&v2].insert({&v0, c02});
        
//         ug.edges[&v1].insert({&v3, c13});
//         ug.edges[&v3].insert({&v1, c13});
        
//         ug.edges[&v2].insert({&v4, c24});
//         ug.edges[&v4].insert({&v2, c24});
        
//         ug.edges[&v1].insert({&v5, c15});
//         ug.edges[&v5].insert({&v1, c15});
        
//         ug.edges[&v3].insert({&v6, c36});
//         ug.edges[&v6].insert({&v3, c36});
        
//         ug.edges[&v4].insert({&v6, c46});
//         ug.edges[&v6].insert({&v4, c46});
        
//         ug.edges[&v5].insert({&v7, c57});
//         ug.edges[&v7].insert({&v5, c57});
        
//         ug.edges[&v6].insert({&v7, c67});
//         ug.edges[&v7].insert({&v6, c67});
        
//         ug.edges[&v7].insert({&atom_x, c7x});
//         ug.edges[&atom_x].insert({&v7, c7x});
        
//         ug.edges[&v6].insert({&atom_y, c6y});
//         ug.edges[&atom_y].insert({&v6, c6y});
        
//         ug.edges[&v4].insert({&atom_z, c4z});
//         ug.edges[&atom_z].insert({&v4, c4z});
        
//         // Test from v0: Multiple paths to multiple atoms
//         // Path to atom_z: v0 -> v2 -> v4 -> atom_z
//         //   Causes: {200,1},{2,1} + {200,1},{4,1} + {12,1}
//         //   Unique: {200,1}, {2,1}, {4,1}, {12,1} = 4 unique
//         //
//         // Path to atom_x: v0 -> v1 -> v5 -> v7 -> atom_x
//         //   Causes: {100,1},{1,1} + {300,1},{5,1},{5,2} + {8,1} + {10,1}
//         //   Unique: {100,1}, {1,1}, {300,1}, {5,1}, {5,2}, {8,1}, {10,1} = 7 unique
//         //
//         // Path to atom_x (alt): v0 -> v1 -> v3 -> v6 -> v7 -> atom_x
//         //   Causes: {100,1},{1,1} + {100,1},{3,1},{3,2} + {300,1},{6,1} + {9,1},{9,2} + {10,1}
//         //   Unique: {100,1}, {1,1}, {3,1}, {3,2}, {300,1}, {6,1}, {9,1}, {9,2}, {10,1} = 9 unique
//         //
//         // Shortest should be path to atom_z with 4 unique fulfillments
//         auto [found, cause] = ug.cin_dijkstra(&v0);
//         assert(found == true);
//         assert(cause.size() == 4);
//         assert(cause.count(make_fulfillment(200, 1)) == true);
//         assert(cause.count(make_fulfillment(2, 1)) == true);
//         assert(cause.count(make_fulfillment(4, 1)) == true);
//         assert(cause.count(make_fulfillment(12, 1)) == true);
        
//         // Test from v1: Should find path with heavy overlap
//         // Path to atom_x: v1 -> v5 -> v7 -> atom_x
//         //   Causes: {300,1},{5,1},{5,2} + {8,1} + {10,1}
//         //   Unique: {300,1}, {5,1}, {5,2}, {8,1}, {10,1} = 5 unique
//         //
//         // Path to atom_y: v1 -> v3 -> v6 -> atom_y
//         //   Causes: {100,1},{3,1},{3,2} + {300,1},{6,1} + {11,1},{11,2}
//         //   Unique: {100,1}, {3,1}, {3,2}, {300,1}, {6,1}, {11,1}, {11,2} = 7 unique
//         auto [found2, cause2] = ug.cin_dijkstra(&v1);
//         assert(found2 == true);
//         assert(cause2.size() == 5);
//         assert(cause2.count(make_fulfillment(300, 1)) == true);
//         assert(cause2.count(make_fulfillment(5, 1)) == true);
//         assert(cause2.count(make_fulfillment(5, 2)) == true);
//         assert(cause2.count(make_fulfillment(8, 1)) == true);
//         assert(cause2.count(make_fulfillment(10, 1)) == true);
        
//         // Test from v3: Multiple paths with different overlap patterns
//         // Path to atom_x: v3 -> v6 -> v7 -> atom_x
//         //   Causes: {300,1},{6,1} + {9,1},{9,2} + {10,1}
//         //   Unique: {300,1}, {6,1}, {9,1}, {9,2}, {10,1} = 5 unique
//         //
//         // Path to atom_y: v3 -> v6 -> atom_y
//         //   Causes: {300,1},{6,1} + {11,1},{11,2}
//         //   Unique: {300,1}, {6,1}, {11,1}, {11,2} = 4 unique
//         auto [found3, cause3] = ug.cin_dijkstra(&v3);
//         assert(found3 == true);
//         assert(cause3.size() == 4);
//         assert(cause3.count(make_fulfillment(300, 1)) == true);
//         assert(cause3.count(make_fulfillment(6, 1)) == true);
//         assert(cause3.count(make_fulfillment(11, 1)) == true);
//         assert(cause3.count(make_fulfillment(11, 2)) == true);
        
//         // Test from v6: Direct access to two atoms
//         // Path to atom_y: direct, cost 2
//         // Path to atom_x: v6 -> v7 -> atom_x, cost 2+1 = 3
//         auto [found4, cause4] = ug.cin_dijkstra(&v6);
//         assert(found4 == true);
//         assert(cause4.size() == 2);  // Should choose atom_y
//         assert(cause4.count(make_fulfillment(11, 1)) == true);
//         assert(cause4.count(make_fulfillment(11, 2)) == true);
//     }
    
//     // Test 14: Self-loop edge (var points to itself)
//     // COMMENTED OUT - may cause infinite loop
//     // {
//     //     expr var_self{expr::var{100}};
//     //     expr atom_end{expr::atom{"self_loop_end"}};
//     //     
//     //     causal_set self_cause = make_cause({{100, 1}});
//     //     causal_set to_atom = make_cause({{101, 1}});
//     //     
//     //     ug.edges[&var_self].insert({&var_self, self_cause});  // Self-loop
//     //     ug.edges[&var_self].insert({&atom_end, to_atom});
//     //     ug.edges[&atom_end];  // Initialize atom entry
//     //     
//     //     auto [found, cause] = ug.cin_dijkstra(&var_self);
//     //     assert(found == true);
//     //     assert(cause.size() == 1);  // Should skip self-loop and find atom
//     //     assert(cause == to_atom);
//     // }
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
    TEST(test_expr_pool_constructor);
    TEST(test_expr_pool_atom);
    TEST(test_expr_pool_var);
    TEST(test_expr_pool_cons);
    TEST(test_bind_map_whnf);
    // TEST(test_causal_set_empty);
    // TEST(test_causal_set_size);
    // TEST(test_causal_set_count);
    // TEST(test_causal_set_operator_plus);
    // TEST(test_causal_set_operator_spaceship);
    // TEST(test_unification_graph_cin_dijkstra);
}

int main() {
    unit_test_main();
    return 0;
}
