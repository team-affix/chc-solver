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

void test_bind_map_occurs_check() {
    trail t;
    
    // occurs_check determines if a variable (by index) occurs anywhere in an expression,
    // even through indirection via bindings. It uses whnf to follow chains.
    // Tests cover: atoms, unbound vars, bound vars, chains, cons structures, nested cons,
    // and various combinations. Circular bindings (undefined behavior) are not tested.
    
    // Test 1: occurs_check on atom - should return false
    {
        bind_map bm(t);
        expr a1{expr::atom{"test"}};
        assert(!bm.occurs_check(0, &a1));
        assert(!bm.occurs_check(100, &a1));
        assert(bm.bindings.size() == 0);
    }
    
    // Test 2: occurs_check on unbound var with same index - should return true
    {
        bind_map bm(t);
        expr v1{expr::var{5}};
        assert(bm.occurs_check(5, &v1));
        assert(bm.bindings.size() == 1);  // whnf creates entry
    }
    
    // Test 3: occurs_check on unbound var with different index - should return false
    {
        bind_map bm(t);
        expr v1{expr::var{10}};
        assert(!bm.occurs_check(5, &v1));
        assert(!bm.occurs_check(11, &v1));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 4: occurs_check on var bound to atom - should return false
    {
        bind_map bm(t);
        expr v1{expr::var{15}};
        expr a1{expr::atom{"bound"}};
        bm.bindings[15] = &a1;
        
        assert(!bm.occurs_check(15, &v1));  // v1 reduces to atom
        assert(!bm.occurs_check(20, &v1));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 5: occurs_check on var bound to same var - creates infinite loop, SKIP
    // This is undefined behavior - circular binding
    
    // Test 6: occurs_check through chain ending in unbound var
    {
        bind_map bm(t);
        expr v1{expr::var{30}};
        expr v2{expr::var{31}};
        expr v3{expr::var{32}};
        
        // Chain: v1 -> v2 -> v3 (v3 is unbound, don't set it explicitly)
        bm.bindings[30] = &v2;
        bm.bindings[31] = &v3;
        // Don't set bindings[32] - let whnf handle it
        
        // v1 eventually points to v3 (var 32)
        assert(bm.occurs_check(32, &v1));
        assert(bm.bindings.size() == 3);  // whnf will create entry for 32
    }
    
    // Test 7: occurs_check through chain ending in atom
    {
        bind_map bm(t);
        expr v1{expr::var{40}};
        expr v2{expr::var{41}};
        expr a1{expr::atom{"end"}};
        
        // Chain: v1 -> v2 -> atom
        bm.bindings[40] = &v2;
        bm.bindings[41] = &a1;
        
        assert(!bm.occurs_check(40, &v1));
        assert(!bm.occurs_check(41, &v1));
        assert(!bm.occurs_check(99, &v1));
        assert(bm.bindings.size() == 2);
    }
    
    // Test 8: occurs_check on cons with no vars
    {
        bind_map bm(t);
        expr a1{expr::atom{"left"}};
        expr a2{expr::atom{"right"}};
        expr c1{expr::cons{&a1, &a2}};
        
        assert(!bm.occurs_check(0, &c1));
        assert(!bm.occurs_check(50, &c1));
        assert(bm.bindings.size() == 0);
    }
    
    // Test 9: occurs_check on cons with matching var in lhs
    {
        bind_map bm(t);
        expr v1{expr::var{55}};
        expr a1{expr::atom{"right"}};
        expr c1{expr::cons{&v1, &a1}};
        
        assert(bm.occurs_check(55, &c1));
        assert(!bm.occurs_check(56, &c1));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 10: occurs_check on cons with matching var in rhs
    {
        bind_map bm(t);
        expr a1{expr::atom{"left"}};
        expr v1{expr::var{60}};
        expr c1{expr::cons{&a1, &v1}};
        
        assert(bm.occurs_check(60, &c1));
        assert(!bm.occurs_check(61, &c1));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 11: occurs_check on cons with matching var in both children
    {
        bind_map bm(t);
        expr v1{expr::var{65}};
        expr v2{expr::var{65}};  // Same index
        expr c1{expr::cons{&v1, &v2}};
        
        assert(bm.occurs_check(65, &c1));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 12: occurs_check on cons with different vars
    {
        bind_map bm(t);
        expr v1{expr::var{70}};
        expr v2{expr::var{71}};
        expr c1{expr::cons{&v1, &v2}};
        
        assert(bm.occurs_check(70, &c1));
        assert(bm.occurs_check(71, &c1));
        assert(!bm.occurs_check(72, &c1));
        assert(bm.bindings.size() == 2);
    }
    
    // Test 13: occurs_check on nested cons
    {
        bind_map bm(t);
        expr v1{expr::var{75}};
        expr a1{expr::atom{"inner"}};
        expr inner_cons{expr::cons{&v1, &a1}};
        expr a2{expr::atom{"outer"}};
        expr outer_cons{expr::cons{&inner_cons, &a2}};
        
        assert(bm.occurs_check(75, &outer_cons));  // v1 is in nested cons
        assert(!bm.occurs_check(76, &outer_cons));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 14: occurs_check on deeply nested cons
    {
        bind_map bm(t);
        expr v1{expr::var{80}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        
        // Build: cons(cons(cons(v1, a1), a2), a3)
        expr inner1{expr::cons{&v1, &a1}};
        expr inner2{expr::cons{&inner1, &a2}};
        expr outer{expr::cons{&inner2, &a3}};
        
        assert(bm.occurs_check(80, &outer));
        assert(!bm.occurs_check(81, &outer));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 15: occurs_check with var bound to cons containing same var (indirect cycle)
    // This is undefined behavior - circular binding
    // {
    //     bind_map bm(t);
    //     expr v1{expr::var{85}};
    //     expr v2{expr::var{85}};  // Same index
    //     expr a1{expr::atom{"test"}};
    //     expr c1{expr::cons{&v2, &a1}};
    //     
    //     bm.bindings[85] = &c1;
    //     
    //     // v1 is bound to cons containing v2 (same index), so occurs_check should find it
    //     assert(bm.occurs_check(85, &v1));
    //     assert(bm.bindings.size() == 1);
    // }
    
    // Test 16: occurs_check with var bound to cons not containing that var
    {
        bind_map bm(t);
        expr v1{expr::var{90}};
        expr v2{expr::var{91}};
        expr a1{expr::atom{"test"}};
        expr c1{expr::cons{&v2, &a1}};
        
        bm.bindings[90] = &c1;
        
        assert(!bm.occurs_check(90, &v1));  // v1 -> c1, but c1 doesn't contain v1
        assert(bm.occurs_check(91, &v1));   // v1 -> c1 which contains v2
        assert(bm.bindings.size() == 2);
    }
    
    // Test 17: occurs_check through chain to cons
    {
        bind_map bm(t);
        expr v1{expr::var{95}};
        expr v2{expr::var{96}};
        expr v3{expr::var{97}};
        expr a1{expr::atom{"test"}};
        expr c1{expr::cons{&v3, &a1}};
        
        // Chain: v1 -> v2 -> c1 (which contains v3, v3 unbound)
        bm.bindings[95] = &v2;
        bm.bindings[96] = &c1;
        
        assert(!bm.occurs_check(95, &v1));  // After whnf, v1 -> c1, doesn't contain v1
        assert(!bm.occurs_check(96, &v1));
        assert(bm.occurs_check(97, &v1));   // c1 contains v3
        assert(bm.bindings.size() == 3);
    }
    
    // Test 18: occurs_check on cons with bound vars
    {
        bind_map bm(t);
        expr v1{expr::var{100}};
        expr v2{expr::var{101}};
        expr a1{expr::atom{"bound"}};
        
        // Bind v2 to atom
        bm.bindings[101] = &a1;
        
        expr c1{expr::cons{&v1, &v2}};
        
        assert(bm.occurs_check(100, &c1));  // v1 is in cons
        assert(!bm.occurs_check(101, &c1)); // v2 reduces to atom
        assert(bm.bindings.size() == 2);
    }
    
    // Test 19: occurs_check with multiple levels of indirection
    {
        bind_map bm(t);
        expr v1{expr::var{105}};
        expr v2{expr::var{106}};
        expr v3{expr::var{107}};
        expr v4{expr::var{108}};
        
        // Chain: v1 -> v2 -> v3 -> v4 (v4 unbound)
        bm.bindings[105] = &v2;
        bm.bindings[106] = &v3;
        bm.bindings[107] = &v4;
        
        assert(bm.occurs_check(108, &v1));  // Eventually points to v4
        assert(!bm.occurs_check(105, &v1)); // After path compression
        assert(bm.bindings.size() == 4);
    }
    
    // Test 20: occurs_check on cons where both children are bound vars
    {
        bind_map bm(t);
        expr v1{expr::var{110}};
        expr v2{expr::var{111}};
        expr a1{expr::atom{"left_bound"}};
        expr a2{expr::atom{"right_bound"}};
        
        bm.bindings[110] = &a1;
        bm.bindings[111] = &a2;
        
        expr c1{expr::cons{&v1, &v2}};
        
        assert(!bm.occurs_check(110, &c1));  // Both reduce to atoms
        assert(!bm.occurs_check(111, &c1));
        assert(!bm.occurs_check(112, &c1));
        assert(bm.bindings.size() == 2);
    }
    
    // Test 21: occurs_check on cons with chain in lhs
    {
        bind_map bm(t);
        expr v1{expr::var{115}};
        expr v2{expr::var{116}};
        expr v3{expr::var{117}};
        expr a1{expr::atom{"rhs"}};
        
        // v1 -> v2 -> v3 (v3 unbound)
        bm.bindings[115] = &v2;
        bm.bindings[116] = &v3;
        
        expr c1{expr::cons{&v1, &a1}};
        
        assert(bm.occurs_check(117, &c1));  // v1 chains to v3
        assert(!bm.occurs_check(115, &c1)); // After compression
        assert(!bm.occurs_check(116, &c1));
        assert(bm.bindings.size() == 3);
    }
    
    // Test 22: occurs_check with cons of cons, target var in nested structure
    {
        bind_map bm(t);
        expr v1{expr::var{120}};
        expr v2{expr::var{121}};
        expr a1{expr::atom{"a"}};
        
        expr inner{expr::cons{&v1, &a1}};
        expr outer{expr::cons{&inner, &v2}};
        
        assert(bm.occurs_check(120, &outer));
        assert(bm.occurs_check(121, &outer));
        assert(!bm.occurs_check(122, &outer));
        assert(bm.bindings.size() == 2);
    }
    
    // Test 23: occurs_check on var bound to deeply nested structure
    {
        bind_map bm(t);
        expr v_outer{expr::var{125}};
        expr v_inner{expr::var{126}};
        expr a1{expr::atom{"deep"}};
        expr a2{expr::atom{"deeper"}};
        
        // Build nested: cons(cons(v_inner, a1), a2)
        expr inner{expr::cons{&v_inner, &a1}};
        expr outer{expr::cons{&inner, &a2}};
        
        bm.bindings[125] = &outer;
        
        assert(!bm.occurs_check(125, &v_outer));  // v_outer -> outer, doesn't contain itself
        assert(bm.occurs_check(126, &v_outer));   // outer contains v_inner
        assert(bm.bindings.size() == 2);
    }
    
    // Test 24: occurs_check with symmetric cons (same var on both sides)
    {
        bind_map bm(t);
        expr v1{expr::var{130}};
        expr v2{expr::var{130}};  // Same var
        expr c1{expr::cons{&v1, &v2}};
        
        assert(bm.occurs_check(130, &c1));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 25: occurs_check on empty bindings map
    {
        bind_map bm(t);
        expr v1{expr::var{135}};
        
        assert(bm.bindings.size() == 0);
        assert(bm.occurs_check(135, &v1));
        assert(bm.bindings.size() == 1);  // whnf creates entry
    }
    
    // Test 26: occurs_check with var bound through multiple cons layers
    {
        bind_map bm(t);
        expr v1{expr::var{140}};
        expr v2{expr::var{141}};
        expr a1{expr::atom{"base"}};
        
        // Build: cons(cons(a1, v2), a1) where v2 is unbound
        expr inner{expr::cons{&a1, &v2}};
        expr outer{expr::cons{&inner, &a1}};
        
        bm.bindings[140] = &outer;
        
        assert(!bm.occurs_check(140, &v1));  // v1 -> outer, doesn't contain itself
        assert(bm.occurs_check(141, &v1));   // outer contains v2
        assert(bm.bindings.size() == 2);
    }
    
    // Test 27: occurs_check with all atoms (no vars anywhere)
    {
        bind_map bm(t);
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        
        expr c1{expr::cons{&a1, &a2}};
        expr c2{expr::cons{&c1, &a3}};
        
        assert(!bm.occurs_check(0, &c2));
        assert(!bm.occurs_check(999, &c2));
        assert(bm.bindings.size() == 0);
    }
    
    // Test 28: occurs_check with var bound to var bound to cons containing target
    {
        bind_map bm(t);
        expr v1{expr::var{145}};
        expr v2{expr::var{146}};
        expr v3{expr::var{147}};
        expr a1{expr::atom{"test"}};
        
        expr c1{expr::cons{&v3, &a1}};  // v3 unbound
        bm.bindings[145] = &v2;
        bm.bindings[146] = &c1;
        
        assert(!bm.occurs_check(145, &v1));  // After whnf, v1 -> c1
        assert(!bm.occurs_check(146, &v1));
        assert(bm.occurs_check(147, &v1));   // c1 contains v3
        assert(bm.bindings.size() == 3);
    }
    
    // Test 29: Very long chain (10 levels) ending in unbound var
    {
        bind_map bm(t);
        expr v0{expr::var{200}};
        expr v1{expr::var{201}};
        expr v2{expr::var{202}};
        expr v3{expr::var{203}};
        expr v4{expr::var{204}};
        expr v5{expr::var{205}};
        expr v6{expr::var{206}};
        expr v7{expr::var{207}};
        expr v8{expr::var{208}};
        expr v9{expr::var{209}};
        
        // Chain: v0 -> v1 -> v2 -> ... -> v9 (unbound)
        bm.bindings[200] = &v1;
        bm.bindings[201] = &v2;
        bm.bindings[202] = &v3;
        bm.bindings[203] = &v4;
        bm.bindings[204] = &v5;
        bm.bindings[205] = &v6;
        bm.bindings[206] = &v7;
        bm.bindings[207] = &v8;
        bm.bindings[208] = &v9;
        
        assert(bm.occurs_check(209, &v0));  // v0 eventually points to v9
        assert(!bm.occurs_check(200, &v0)); // After path compression
        assert(!bm.occurs_check(205, &v0)); // Middle of chain
        assert(bm.bindings.size() == 10);  // 9 bindings + 1 for v9 from whnf
    }
    
    // Test 30: Very long chain ending in atom
    {
        bind_map bm(t);
        expr v0{expr::var{210}};
        expr v1{expr::var{211}};
        expr v2{expr::var{212}};
        expr v3{expr::var{213}};
        expr v4{expr::var{214}};
        expr v5{expr::var{215}};
        expr v6{expr::var{216}};
        expr v7{expr::var{217}};
        expr a1{expr::atom{"end"}};
        
        bm.bindings[210] = &v1;
        bm.bindings[211] = &v2;
        bm.bindings[212] = &v3;
        bm.bindings[213] = &v4;
        bm.bindings[214] = &v5;
        bm.bindings[215] = &v6;
        bm.bindings[216] = &v7;
        bm.bindings[217] = &a1;
        
        assert(!bm.occurs_check(210, &v0));
        assert(!bm.occurs_check(217, &v0));
        assert(!bm.occurs_check(999, &v0));
        assert(bm.bindings.size() == 8);
    }
    
    // Test 31: Deeply nested cons (5 levels) with var at bottom
    {
        bind_map bm(t);
        expr v1{expr::var{220}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        expr a4{expr::atom{"d"}};
        
        // Build: cons(cons(cons(cons(cons(v1, a1), a2), a3), a4), a1)
        expr level1{expr::cons{&v1, &a1}};
        expr level2{expr::cons{&level1, &a2}};
        expr level3{expr::cons{&level2, &a3}};
        expr level4{expr::cons{&level3, &a4}};
        expr level5{expr::cons{&level4, &a1}};
        
        assert(bm.occurs_check(220, &level5));
        assert(!bm.occurs_check(221, &level5));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 32: Deeply nested cons with var at different positions
    {
        bind_map bm(t);
        expr v1{expr::var{225}};
        expr v2{expr::var{226}};
        expr v3{expr::var{227}};
        expr a1{expr::atom{"x"}};
        
        // Build: cons(cons(v1, v2), cons(a1, v3))
        expr left{expr::cons{&v1, &v2}};
        expr right{expr::cons{&a1, &v3}};
        expr outer{expr::cons{&left, &right}};
        
        assert(bm.occurs_check(225, &outer));  // v1 in left subtree
        assert(bm.occurs_check(226, &outer));  // v2 in left subtree
        assert(bm.occurs_check(227, &outer));  // v3 in right subtree
        assert(!bm.occurs_check(228, &outer));
        assert(bm.bindings.size() == 3);
    }
    
    // Test 33: Var bound to deeply nested cons containing bound vars
    {
        bind_map bm(t);
        expr v_outer{expr::var{230}};
        expr v1{expr::var{231}};
        expr v2{expr::var{232}};
        expr v3{expr::var{233}};
        expr a1{expr::atom{"bound1"}};
        expr a2{expr::atom{"bound2"}};
        
        // Bind some vars to atoms
        bm.bindings[231] = &a1;
        bm.bindings[232] = &a2;
        
        // Build: cons(cons(v1, v2), cons(v3, a1))
        expr left{expr::cons{&v1, &v2}};
        expr right{expr::cons{&v3, &a1}};
        expr outer{expr::cons{&left, &right}};
        
        bm.bindings[230] = &outer;
        
        // v_outer -> outer, check what's in outer
        assert(!bm.occurs_check(230, &v_outer));  // v_outer doesn't contain itself
        assert(!bm.occurs_check(231, &v_outer));  // v1 reduces to atom
        assert(!bm.occurs_check(232, &v_outer));  // v2 reduces to atom
        assert(bm.occurs_check(233, &v_outer));   // v3 is unbound in outer
        assert(bm.bindings.size() == 4);
    }
    
    // Test 34: Long chain to deeply nested cons with target var deep inside
    {
        bind_map bm(t);
        expr v_chain1{expr::var{240}};
        expr v_chain2{expr::var{241}};
        expr v_chain3{expr::var{242}};
        expr v_target{expr::var{243}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Build nested cons: cons(cons(cons(v_target, a1), a2), a1)
        expr inner1{expr::cons{&v_target, &a1}};
        expr inner2{expr::cons{&inner1, &a2}};
        expr outer{expr::cons{&inner2, &a1}};
        
        // Chain: v_chain1 -> v_chain2 -> v_chain3 -> outer
        bm.bindings[240] = &v_chain2;
        bm.bindings[241] = &v_chain3;
        bm.bindings[242] = &outer;
        
        assert(bm.occurs_check(243, &v_chain1));  // Target is deep in the cons
        assert(!bm.occurs_check(240, &v_chain1)); // After path compression
        assert(!bm.occurs_check(241, &v_chain1));
        assert(!bm.occurs_check(242, &v_chain1));
        assert(bm.bindings.size() == 4);
    }
    
    // Test 35: Cons with chains in both children
    {
        bind_map bm(t);
        expr v_left1{expr::var{250}};
        expr v_left2{expr::var{251}};
        expr v_left3{expr::var{252}};
        expr v_right1{expr::var{253}};
        expr v_right2{expr::var{254}};
        expr v_right3{expr::var{255}};
        
        // Left chain: v_left1 -> v_left2 -> v_left3 (unbound)
        bm.bindings[250] = &v_left2;
        bm.bindings[251] = &v_left3;
        
        // Right chain: v_right1 -> v_right2 -> v_right3 (unbound)
        bm.bindings[253] = &v_right2;
        bm.bindings[254] = &v_right3;
        
        expr c1{expr::cons{&v_left1, &v_right1}};
        
        assert(bm.occurs_check(252, &c1));  // v_left3 via left child
        assert(bm.occurs_check(255, &c1));  // v_right3 via right child
        assert(!bm.occurs_check(250, &c1)); // After path compression
        assert(!bm.occurs_check(253, &c1)); // After path compression
        assert(bm.bindings.size() == 6);  // 4 initial + 2 from whnf on unbound vars
    }
    
    // Test 36: Very complex nested structure with multiple vars at different depths
    {
        bind_map bm(t);
        expr v1{expr::var{260}};
        expr v2{expr::var{261}};
        expr v3{expr::var{262}};
        expr v4{expr::var{263}};
        expr v5{expr::var{264}};
        expr a1{expr::atom{"atom"}};
        
        // Build: cons(cons(v1, cons(v2, v3)), cons(cons(v4, v5), a1))
        expr inner_left_right{expr::cons{&v2, &v3}};
        expr inner_left{expr::cons{&v1, &inner_left_right}};
        expr inner_right_left{expr::cons{&v4, &v5}};
        expr inner_right{expr::cons{&inner_right_left, &a1}};
        expr outer{expr::cons{&inner_left, &inner_right}};
        
        assert(bm.occurs_check(260, &outer));  // v1 in left subtree
        assert(bm.occurs_check(261, &outer));  // v2 in left subtree, nested
        assert(bm.occurs_check(262, &outer));  // v3 in left subtree, nested
        assert(bm.occurs_check(263, &outer));  // v4 in right subtree, nested
        assert(bm.occurs_check(264, &outer));  // v5 in right subtree, nested
        assert(!bm.occurs_check(265, &outer));
        assert(bm.bindings.size() == 5);
    }
    
    // Test 37: Chain to cons, where cons children are also chains
    {
        bind_map bm(t);
        expr v_outer{expr::var{270}};
        expr v_mid{expr::var{271}};
        
        expr v_left1{expr::var{272}};
        expr v_left2{expr::var{273}};
        expr a_left{expr::atom{"left_end"}};
        
        expr v_right1{expr::var{274}};
        expr v_right2{expr::var{275}};
        expr a_right{expr::atom{"right_end"}};
        
        // Left chain: v_left1 -> v_left2 -> a_left
        bm.bindings[272] = &v_left2;
        bm.bindings[273] = &a_left;
        
        // Right chain: v_right1 -> v_right2 -> a_right
        bm.bindings[274] = &v_right2;
        bm.bindings[275] = &a_right;
        
        expr c1{expr::cons{&v_left1, &v_right1}};
        
        // Outer chain: v_outer -> v_mid -> c1
        bm.bindings[270] = &v_mid;
        bm.bindings[271] = &c1;
        
        // After whnf, v_outer -> c1
        // c1's children will be reduced when occurs_check recurses
        assert(!bm.occurs_check(270, &v_outer));
        assert(!bm.occurs_check(271, &v_outer));
        assert(!bm.occurs_check(272, &v_outer));  // v_left1 reduces to atom
        assert(!bm.occurs_check(273, &v_outer));  // v_left2 reduces to atom
        assert(!bm.occurs_check(274, &v_outer));  // v_right1 reduces to atom
        assert(!bm.occurs_check(275, &v_outer));  // v_right2 reduces to atom
        assert(bm.bindings.size() == 6);
    }
    
    // Test 38: Cons with one child being a long chain ending in target var
    {
        bind_map bm(t);
        expr v1{expr::var{280}};
        expr v2{expr::var{281}};
        expr v3{expr::var{282}};
        expr v4{expr::var{283}};
        expr v5{expr::var{284}};
        expr a1{expr::atom{"other"}};
        
        // Chain: v1 -> v2 -> v3 -> v4 -> v5 (unbound)
        bm.bindings[280] = &v2;
        bm.bindings[281] = &v3;
        bm.bindings[282] = &v4;
        bm.bindings[283] = &v5;
        
        expr c1{expr::cons{&v1, &a1}};
        
        assert(bm.occurs_check(284, &c1));  // v5 is at end of chain in lhs
        assert(!bm.occurs_check(280, &c1)); // After path compression
        assert(bm.bindings.size() == 5);  // 4 initial + 1 from whnf on v5
    }
    
    // Test 39: Multiple nested cons with same var appearing in different positions
    {
        bind_map bm(t);
        expr v1{expr::var{290}};
        expr v2{expr::var{290}};  // Same index as v1
        expr v3{expr::var{290}};  // Same index as v1
        expr a1{expr::atom{"x"}};
        
        // Build: cons(cons(v1, a1), cons(v2, v3))
        expr left{expr::cons{&v1, &a1}};
        expr right{expr::cons{&v2, &v3}};
        expr outer{expr::cons{&left, &right}};
        
        assert(bm.occurs_check(290, &outer));  // Var 290 appears 3 times
        assert(bm.bindings.size() == 1);
    }
    
    // Test 40: Stress test - very deep nesting (10 levels) with var at bottom
    {
        bind_map bm(t);
        expr v1{expr::var{300}};
        expr a1{expr::atom{"a"}};
        
        // Build 10 levels of nesting
        expr* current = &v1;
        expr level1{expr::cons{current, &a1}};
        expr level2{expr::cons{&level1, &a1}};
        expr level3{expr::cons{&level2, &a1}};
        expr level4{expr::cons{&level3, &a1}};
        expr level5{expr::cons{&level4, &a1}};
        expr level6{expr::cons{&level5, &a1}};
        expr level7{expr::cons{&level6, &a1}};
        expr level8{expr::cons{&level7, &a1}};
        expr level9{expr::cons{&level8, &a1}};
        expr level10{expr::cons{&level9, &a1}};
        
        assert(bm.occurs_check(300, &level10));  // Should find v1 at the bottom
        assert(!bm.occurs_check(301, &level10));
        assert(bm.bindings.size() == 1);
    }
    
    // Test 41: ULTIMATE STRESS TEST - Long chain to deeply nested cons with chains inside
    {
        bind_map bm(t);
        
        // Outer chain (5 levels): v0 -> v1 -> v2 -> v3 -> v4 -> nested_cons
        expr v0{expr::var{400}};
        expr v1{expr::var{401}};
        expr v2{expr::var{402}};
        expr v3{expr::var{403}};
        expr v4{expr::var{404}};
        
        // Left inner chain (3 levels): v_left1 -> v_left2 -> v_left3 (target)
        expr v_left1{expr::var{405}};
        expr v_left2{expr::var{406}};
        expr v_left3{expr::var{407}};
        bm.bindings[405] = &v_left2;
        bm.bindings[406] = &v_left3;
        
        // Right inner chain (3 levels): v_right1 -> v_right2 -> v_right3
        expr v_right1{expr::var{408}};
        expr v_right2{expr::var{409}};
        expr v_right3{expr::var{410}};
        bm.bindings[408] = &v_right2;
        bm.bindings[409] = &v_right3;
        
        // Build deeply nested cons (4 levels) with chains at the leaves
        // Structure: cons(cons(cons(cons(v_left1, v_right1), atom), atom), atom)
        expr a1{expr::atom{"a"}};
        expr level1{expr::cons{&v_left1, &v_right1}};  // Chains in both children
        expr level2{expr::cons{&level1, &a1}};
        expr level3{expr::cons{&level2, &a1}};
        expr level4{expr::cons{&level3, &a1}};
        
        // Setup outer chain
        bm.bindings[400] = &v1;
        bm.bindings[401] = &v2;
        bm.bindings[402] = &v3;
        bm.bindings[403] = &v4;
        bm.bindings[404] = &level4;
        
        // Test: v0 chains to level4, which contains level1, which has v_left1 in lhs
        // v_left1 chains to v_left3 (target)
        assert(bm.occurs_check(407, &v0));  // Should find v_left3 through all the indirection
        assert(bm.occurs_check(410, &v0));  // Should find v_right3 through all the indirection
        assert(!bm.occurs_check(400, &v0)); // After path compression
        assert(!bm.occurs_check(405, &v0)); // v_left1 chains to v_left3
        assert(!bm.occurs_check(408, &v0)); // v_right1 chains to v_right3
        assert(bm.bindings.size() == 11);  // 9 explicit + 2 from whnf on unbound vars
    }
    
    // Test 42: Nested cons where EVERY node has a chain
    {
        bind_map bm(t);
        
        // Build: cons(cons(chain_a, chain_b), cons(chain_c, chain_d))
        // where each chain is 2 levels deep
        
        // Chain A: va1 -> va2 (unbound)
        expr va1{expr::var{500}};
        expr va2{expr::var{501}};
        bm.bindings[500] = &va2;
        
        // Chain B: vb1 -> vb2 (unbound)
        expr vb1{expr::var{502}};
        expr vb2{expr::var{503}};
        bm.bindings[502] = &vb2;
        
        // Chain C: vc1 -> vc2 (unbound)
        expr vc1{expr::var{504}};
        expr vc2{expr::var{505}};
        bm.bindings[504] = &vc2;
        
        // Chain D: vd1 -> vd2 (unbound)
        expr vd1{expr::var{506}};
        expr vd2{expr::var{507}};
        bm.bindings[506] = &vd2;
        
        expr left{expr::cons{&va1, &vb1}};
        expr right{expr::cons{&vc1, &vd1}};
        expr outer{expr::cons{&left, &right}};
        
        // All four target vars should be found
        assert(bm.occurs_check(501, &outer));  // va2
        // After first check, all 4 unbound vars have been visited and have entries
        assert(bm.occurs_check(503, &outer));  // vb2
        assert(bm.occurs_check(505, &outer));  // vc2
        assert(bm.occurs_check(507, &outer));  // vd2
        
        // None of the chain heads should be found (after compression)
        assert(!bm.occurs_check(500, &outer));
        assert(!bm.occurs_check(502, &outer));
        assert(!bm.occurs_check(504, &outer));
        assert(!bm.occurs_check(506, &outer));
        
        // occurs_check traverses tree and calls whnf on nodes, creating entries as needed
        // The exact count depends on traversal order and short-circuit evaluation
        assert(bm.bindings.size() >= 8);  // At least the 8 explicit bindings
    }
    
    // Test 43: Chain to nested cons, where nested cons contains chains to more nested cons
    {
        bind_map bm(t);
        
        // Outer chain: v0 -> v1 -> outer_cons
        expr v0{expr::var{600}};
        expr v1{expr::var{601}};
        
        // Inner left chain: v_left -> inner_left_cons
        expr v_left{expr::var{602}};
        
        // Inner right chain: v_right -> inner_right_cons
        expr v_right{expr::var{603}};
        
        // Target var deep inside
        expr v_target{expr::var{604}};
        expr a1{expr::atom{"a"}};
        
        // Build: outer_cons = cons(v_left, v_right)
        //        v_left -> inner_left_cons = cons(v_target, a1)
        //        v_right -> inner_right_cons = cons(a1, a1)
        expr inner_left_cons{expr::cons{&v_target, &a1}};
        expr inner_right_cons{expr::cons{&a1, &a1}};
        
        bm.bindings[602] = &inner_left_cons;
        bm.bindings[603] = &inner_right_cons;
        
        expr outer_cons{expr::cons{&v_left, &v_right}};
        
        bm.bindings[600] = &v1;
        bm.bindings[601] = &outer_cons;
        
        // v0 -> v1 -> outer_cons
        // outer_cons.lhs = v_left -> inner_left_cons
        // inner_left_cons.lhs = v_target
        assert(bm.occurs_check(604, &v0));  // Should find v_target through all the indirection
        assert(!bm.occurs_check(600, &v0)); // After path compression
        assert(!bm.occurs_check(602, &v0)); // v_left chains to cons (no var 602 in result)
        assert(!bm.occurs_check(603, &v0)); // v_right chains to cons (no var 603 in result)
        assert(bm.bindings.size() == 5);
    }
}

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

void test_bind_map_unify() {
    trail t;
    
    // unify() performs Prolog-like unification with occurs check
    // It's a symmetric operation: unify(a, b) should behave like unify(b, a)
    // Tests verify symmetry by commuting arguments between consecutive test cases
    
    // ========== BASIC CASES ==========
    
    // Test 1: Identical expressions (same pointer) - should succeed
    {
        bind_map bm(t);
        expr a1{expr::atom{"test"}};
        assert(bm.unify(&a1, &a1));
        assert(bm.bindings.size() == 0);
    }
    
    // Test 2: Two different atoms - should fail
    {
        bind_map bm(t);
        expr a1{expr::atom{"foo"}};
        expr a2{expr::atom{"bar"}};
        assert(!bm.unify(&a1, &a2));
        assert(bm.bindings.size() == 0);
    }
    
    // Test 3: Two different atoms (commuted) - should fail
    {
        bind_map bm(t);
        expr a1{expr::atom{"foo"}};
        expr a2{expr::atom{"bar"}};
        assert(!bm.unify(&a2, &a1));  // Commuted
        assert(bm.bindings.size() == 0);
    }
    
    // Test 4: Two identical atoms (different objects) - should succeed
    {
        bind_map bm(t);
        expr a1{expr::atom{"same"}};
        expr a2{expr::atom{"same"}};
        assert(bm.unify(&a1, &a2));
        assert(bm.bindings.size() == 0);
    }
    
    // Test 5: Two identical atoms (commuted) - should succeed
    {
        bind_map bm(t);
        expr a1{expr::atom{"same"}};
        expr a2{expr::atom{"same"}};
        assert(bm.unify(&a2, &a1));  // Commuted
        assert(bm.bindings.size() == 0);
    }
    
    // Test 6: Unbound var with atom - should succeed and create binding
    {
        bind_map bm(t);
        expr v1{expr::var{0}};
        expr a1{expr::atom{"bound"}};
        assert(bm.unify(&v1, &a1));
        assert(bm.bindings.size() == 1);  // Just v1 -> a1
        assert(bm.bindings.count(0) == 1);
        assert(bm.whnf(&v1) == &a1);
    }
    
    // Test 7: Atom with unbound var (commuted) - should succeed and create binding
    {
        bind_map bm(t);
        expr v1{expr::var{1}};
        expr a1{expr::atom{"bound"}};
        assert(bm.unify(&a1, &v1));  // Commuted
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.count(1) == 1);
        assert(bm.whnf(&v1) == &a1);
    }
    
    // Test 8: Unbound var with cons - should succeed and create binding
    {
        bind_map bm(t);
        expr v1{expr::var{2}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        assert(bm.unify(&v1, &c1));
        assert(bm.bindings.size() == 1);
        assert(bm.whnf(&v1) == &c1);
    }
    
    // Test 9: Cons with unbound var (commuted) - should succeed and create binding
    {
        bind_map bm(t);
        expr v1{expr::var{3}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        assert(bm.unify(&c1, &v1));  // Commuted
        assert(bm.bindings.size() == 1);
        assert(bm.whnf(&v1) == &c1);
    }
    
    // Test 10: Two unbound vars - should succeed and create binding
    {
        bind_map bm(t);
        expr v1{expr::var{4}};
        expr v2{expr::var{5}};
        assert(bm.unify(&v1, &v2));
        assert(bm.bindings.size() == 2);  // Both get entries from whnf
        // One should be bound to the other
        const expr* result1 = bm.whnf(&v1);
        const expr* result2 = bm.whnf(&v2);
        assert(result1 == result2);  // Both reduce to same thing
    }
    
    // Test 11: Two unbound vars (commuted) - should succeed and create binding
    {
        bind_map bm(t);
        expr v1{expr::var{6}};
        expr v2{expr::var{7}};
        assert(bm.unify(&v2, &v1));  // Commuted
        assert(bm.bindings.size() == 2);
        const expr* result1 = bm.whnf(&v1);
        const expr* result2 = bm.whnf(&v2);
        assert(result1 == result2);
    }
    
    // ========== OCCURS CHECK CASES ==========
    
    // Test 12: Var with cons containing that var - should fail (occurs check)
    {
        bind_map bm(t);
        expr v1{expr::var{10}};
        expr v2{expr::var{10}};  // Same index
        expr a1{expr::atom{"test"}};
        expr c1{expr::cons{&v2, &a1}};
        assert(!bm.unify(&v1, &c1));  // Should fail occurs check
        // Bindings may be created during occurs_check
    }
    
    // Test 13: Cons containing var with that var (commuted) - should fail
    {
        bind_map bm(t);
        expr v1{expr::var{11}};
        expr v2{expr::var{11}};
        expr a1{expr::atom{"test"}};
        expr c1{expr::cons{&v2, &a1}};
        assert(!bm.unify(&c1, &v1));  // Commuted
    }
    
    // Test 14: Var with cons containing chain to that var - should fail
    {
        bind_map bm(t);
        expr v1{expr::var{12}};
        expr v2{expr::var{13}};
        expr v3{expr::var{12}};  // Same as v1
        expr a1{expr::atom{"test"}};
        
        // v2 -> v3 (which is same index as v1)
        bm.bindings[13] = &v3;
        
        expr c1{expr::cons{&v2, &a1}};
        assert(!bm.unify(&v1, &c1));  // Should fail: v1 with cons containing chain to v1
    }
    
    // Test 15: Cons with chain to var, unified with that var (commuted) - should fail
    {
        bind_map bm(t);
        expr v1{expr::var{14}};
        expr v2{expr::var{15}};
        expr v3{expr::var{14}};
        expr a1{expr::atom{"test"}};
        
        bm.bindings[15] = &v3;
        
        expr c1{expr::cons{&v2, &a1}};
        assert(!bm.unify(&c1, &v1));  // Commuted
    }
    
    // Test 16: Var with deeply nested cons containing that var - should fail
    {
        bind_map bm(t);
        expr v1{expr::var{16}};
        expr v2{expr::var{16}};
        expr a1{expr::atom{"a"}};
        
        // Build: cons(cons(cons(v2, a1), a1), a1)
        expr inner1{expr::cons{&v2, &a1}};
        expr inner2{expr::cons{&inner1, &a1}};
        expr outer{expr::cons{&inner2, &a1}};
        
        assert(!bm.unify(&v1, &outer));  // Should fail
    }
    
    // Test 17: Deeply nested cons with var, unified with that var (commuted) - should fail
    {
        bind_map bm(t);
        expr v1{expr::var{17}};
        expr v2{expr::var{17}};
        expr a1{expr::atom{"a"}};
        
        expr inner1{expr::cons{&v2, &a1}};
        expr inner2{expr::cons{&inner1, &a1}};
        expr outer{expr::cons{&inner2, &a1}};
        
        assert(!bm.unify(&outer, &v1));  // Commuted
    }
    
    // ========== CHAIN CASES ==========
    
    // Test 18: Var bound to another var, unify with atom - should follow chain
    {
        bind_map bm(t);
        expr v1{expr::var{20}};
        expr v2{expr::var{21}};
        expr a1{expr::atom{"target"}};
        
        // v1 -> v2 (unbound)
        bm.bindings[20] = &v2;
        
        assert(bm.unify(&v1, &a1));  // Should bind v2 to a1
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);
    }
    
    // Test 19: Atom with var bound to another var (commuted) - should follow chain
    {
        bind_map bm(t);
        expr v1{expr::var{22}};
        expr v2{expr::var{23}};
        expr a1{expr::atom{"target"}};
        
        bm.bindings[22] = &v2;
        
        assert(bm.unify(&a1, &v1));  // Commuted
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);
    }
    
    // Test 20: Two vars in a chain, unify them - should succeed (already unified)
    {
        bind_map bm(t);
        expr v1{expr::var{24}};
        expr v2{expr::var{25}};
        
        // v1 -> v2
        bm.bindings[24] = &v2;
        
        assert(bm.unify(&v1, &v2));  // Both reduce to v2
        assert(bm.whnf(&v1) == bm.whnf(&v2));
    }
    
    // Test 21: Two vars in a chain, unify them (commuted) - should succeed
    {
        bind_map bm(t);
        expr v1{expr::var{26}};
        expr v2{expr::var{27}};
        
        bm.bindings[26] = &v2;
        
        assert(bm.unify(&v2, &v1));  // Commuted
        assert(bm.whnf(&v1) == bm.whnf(&v2));
    }
    
    // Test 22: Long chain of vars, unify head with atom
    {
        bind_map bm(t);
        expr v1{expr::var{28}};
        expr v2{expr::var{29}};
        expr v3{expr::var{30}};
        expr v4{expr::var{31}};
        expr a1{expr::atom{"end"}};
        
        // Chain: v1 -> v2 -> v3 -> v4 (unbound)
        bm.bindings[28] = &v2;
        bm.bindings[29] = &v3;
        bm.bindings[30] = &v4;
        
        assert(bm.unify(&v1, &a1));  // Should bind v4 to a1
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v4) == &a1);
    }
    
    // Test 23: Atom with long chain of vars (commuted)
    {
        bind_map bm(t);
        expr v1{expr::var{32}};
        expr v2{expr::var{33}};
        expr v3{expr::var{34}};
        expr v4{expr::var{35}};
        expr a1{expr::atom{"end"}};
        
        bm.bindings[32] = &v2;
        bm.bindings[33] = &v3;
        bm.bindings[34] = &v4;
        
        assert(bm.unify(&a1, &v1));  // Commuted
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v4) == &a1);
    }
    
    // ========== CONS UNIFICATION CASES ==========
    
    // Test 24: Two cons with same atoms - should succeed
    {
        bind_map bm(t);
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"x"}};
        expr a4{expr::atom{"y"}};
        expr c2{expr::cons{&a3, &a4}};
        
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);
    }
    
    // Test 25: Two cons with same atoms (commuted) - should succeed
    {
        bind_map bm(t);
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"x"}};
        expr a4{expr::atom{"y"}};
        expr c2{expr::cons{&a3, &a4}};
        
        assert(bm.unify(&c2, &c1));  // Commuted
        assert(bm.bindings.size() == 0);
    }
    
    // Test 26: Two cons with different atoms in lhs - should fail
    {
        bind_map bm(t);
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"z"}};  // Different
        expr a4{expr::atom{"y"}};
        expr c2{expr::cons{&a3, &a4}};
        
        assert(!bm.unify(&c1, &c2));
    }
    
    // Test 27: Two cons with different atoms in rhs - should fail
    {
        bind_map bm(t);
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"x"}};
        expr a4{expr::atom{"z"}};  // Different
        expr c2{expr::cons{&a3, &a4}};
        
        assert(!bm.unify(&c1, &c2));
    }
    
    // Test 28: Cons with vars that can be unified - should succeed
    {
        bind_map bm(t);
        expr v1{expr::var{40}};
        expr a1{expr::atom{"y"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr c2{expr::cons{&a2, &a3}};
        
        assert(bm.unify(&c1, &c2));  // Should bind v1 to a2
        assert(bm.whnf(&v1) == &a2);
    }
    
    // Test 29: Cons with vars that can be unified (commuted) - should succeed
    {
        bind_map bm(t);
        expr v1{expr::var{41}};
        expr a1{expr::atom{"y"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr c2{expr::cons{&a2, &a3}};
        
        assert(bm.unify(&c2, &c1));  // Commuted
        assert(bm.whnf(&v1) == &a2);
    }
    
    // Test 30: Cons with vars that cannot be unified (conflicting) - should fail
    {
        bind_map bm(t);
        expr v1{expr::var{42}};
        expr a1{expr::atom{"y"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"z"}};  // Conflicts with a1
        expr c2{expr::cons{&a2, &a3}};
        
        assert(!bm.unify(&c1, &c2));  // rhs children don't match
    }
    
    // Test 31: Cons with vars that cannot be unified (commuted) - should fail
    {
        bind_map bm(t);
        expr v1{expr::var{43}};
        expr a1{expr::atom{"y"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"z"}};
        expr c2{expr::cons{&a2, &a3}};
        
        assert(!bm.unify(&c2, &c1));  // Commuted
    }
    
    // Test 32: Nested cons structures - should recursively unify
    {
        bind_map bm(t);
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr inner1{expr::cons{&a1, &a2}};
        expr a3{expr::atom{"c"}};
        expr outer1{expr::cons{&inner1, &a3}};
        
        expr a4{expr::atom{"a"}};
        expr a5{expr::atom{"b"}};
        expr inner2{expr::cons{&a4, &a5}};
        expr a6{expr::atom{"c"}};
        expr outer2{expr::cons{&inner2, &a6}};
        
        assert(bm.unify(&outer1, &outer2));
        assert(bm.bindings.size() == 0);
    }
    
    // Test 33: Nested cons structures (commuted) - should recursively unify
    {
        bind_map bm(t);
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr inner1{expr::cons{&a1, &a2}};
        expr a3{expr::atom{"c"}};
        expr outer1{expr::cons{&inner1, &a3}};
        
        expr a4{expr::atom{"a"}};
        expr a5{expr::atom{"b"}};
        expr inner2{expr::cons{&a4, &a5}};
        expr a6{expr::atom{"c"}};
        expr outer2{expr::cons{&inner2, &a6}};
        
        assert(bm.unify(&outer2, &outer1));  // Commuted
        assert(bm.bindings.size() == 0);
    }
    
    // ========== TYPE MISMATCH CASES ==========
    
    // Test 34: Atom with cons - should fail
    {
        bind_map bm(t);
        expr a1{expr::atom{"test"}};
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr c1{expr::cons{&a2, &a3}};
        
        assert(!bm.unify(&a1, &c1));
        assert(bm.bindings.size() == 0);
    }
    
    // Test 35: Cons with atom (commuted) - should fail
    {
        bind_map bm(t);
        expr a1{expr::atom{"test"}};
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr c1{expr::cons{&a2, &a3}};
        
        assert(!bm.unify(&c1, &a1));  // Commuted
        assert(bm.bindings.size() == 0);
    }
    
    // ========== COMPLEX CASES ==========
    
    // Test 36: Unify two cons where children need transitive unification
    {
        bind_map bm(t);
        expr v1{expr::var{50}};
        expr a1{expr::atom{"a"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr v2{expr::var{51}};
        expr a2{expr::atom{"a"}};
        expr c2{expr::cons{&v2, &a2}};
        
        assert(bm.unify(&c1, &c2));  // Should bind v1 to v2
        assert(bm.whnf(&v1) == bm.whnf(&v2));
    }
    
    // Test 37: Transitive unification (commuted)
    {
        bind_map bm(t);
        expr v1{expr::var{52}};
        expr a1{expr::atom{"a"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr v2{expr::var{53}};
        expr a2{expr::atom{"a"}};
        expr c2{expr::cons{&v2, &a2}};
        
        assert(bm.unify(&c2, &c1));  // Commuted
        assert(bm.whnf(&v1) == bm.whnf(&v2));
    }
    
    // Test 38: Unify cons with nested vars - cons(v1, cons(v2, a)) with cons(a, cons(b, a))
    {
        bind_map bm(t);
        expr v1{expr::var{54}};
        expr v2{expr::var{55}};
        expr a1{expr::atom{"a"}};
        expr inner1{expr::cons{&v2, &a1}};
        expr c1{expr::cons{&v1, &inner1}};
        
        expr a2{expr::atom{"a"}};
        expr a3{expr::atom{"b"}};
        expr a4{expr::atom{"a"}};
        expr inner2{expr::cons{&a3, &a4}};
        expr c2{expr::cons{&a2, &inner2}};
        
        assert(bm.unify(&c1, &c2));  // v1 -> a2, v2 -> a3
        assert(bm.whnf(&v1) == &a2);
        assert(bm.whnf(&v2) == &a3);
    }
    
    // Test 39: Nested vars (commuted)
    {
        bind_map bm(t);
        expr v1{expr::var{56}};
        expr v2{expr::var{57}};
        expr a1{expr::atom{"a"}};
        expr inner1{expr::cons{&v2, &a1}};
        expr c1{expr::cons{&v1, &inner1}};
        
        expr a2{expr::atom{"a"}};
        expr a3{expr::atom{"b"}};
        expr a4{expr::atom{"a"}};
        expr inner2{expr::cons{&a3, &a4}};
        expr c2{expr::cons{&a2, &inner2}};
        
        assert(bm.unify(&c2, &c1));  // Commuted
        assert(bm.whnf(&v1) == &a2);
        assert(bm.whnf(&v2) == &a3);
    }
    
    // Test 40: Multiple unifications building up bindings
    {
        bind_map bm(t);
        expr v1{expr::var{60}};
        expr v2{expr::var{61}};
        expr v3{expr::var{62}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr a3{expr::atom{"z"}};
        
        // First unification: v1 with a1
        assert(bm.unify(&v1, &a1));
        assert(bm.whnf(&v1) == &a1);
        
        // Second unification: v2 with a2
        assert(bm.unify(&v2, &a2));
        assert(bm.whnf(&v2) == &a2);
        
        // Third unification: v3 with a3
        assert(bm.unify(&v3, &a3));
        assert(bm.whnf(&v3) == &a3);
        
        // All bindings should persist
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a2);
        assert(bm.whnf(&v3) == &a3);
    }
    
    // Test 41: Unify after previous unification - bindings persist
    {
        bind_map bm(t);
        expr v1{expr::var{63}};
        expr v2{expr::var{64}};
        expr a1{expr::atom{"first"}};
        
        // First: unify v1 with v2
        assert(bm.unify(&v1, &v2));
        
        // Second: unify v1 with a1 (should bind v2 to a1 via v1)
        assert(bm.unify(&v1, &a1));
        
        // Both should now point to a1
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);
    }
    
    // Test 42: Self-unification of var - should succeed
    {
        bind_map bm(t);
        expr v1{expr::var{65}};
        assert(bm.unify(&v1, &v1));  // Same pointer
    }
    
    // Test 43: Unify bound var with its binding - should succeed
    {
        bind_map bm(t);
        expr v1{expr::var{66}};
        expr a1{expr::atom{"bound"}};
        
        bm.bindings[66] = &a1;
        
        assert(bm.unify(&v1, &a1));  // v1 reduces to a1, unifying a1 with a1
    }
    
    // Test 44: Unify two bound vars bound to same thing - should succeed
    {
        bind_map bm(t);
        expr v1{expr::var{67}};
        expr v2{expr::var{68}};
        expr a1{expr::atom{"same"}};
        
        bm.bindings[67] = &a1;
        bm.bindings[68] = &a1;
        
        assert(bm.unify(&v1, &v2));  // Both reduce to a1
    }
    
    // Test 45: Unify two bound vars bound to different atoms - should fail
    {
        bind_map bm(t);
        expr v1{expr::var{69}};
        expr v2{expr::var{70}};
        expr a1{expr::atom{"first"}};
        expr a2{expr::atom{"second"}};
        
        bm.bindings[69] = &a1;
        bm.bindings[70] = &a2;
        
        assert(!bm.unify(&v1, &v2));  // Reduce to different atoms
    }
    
    // Test 46: Deeply nested cons unification with vars at various levels
    {
        bind_map bm(t);
        expr v1{expr::var{71}};
        expr v2{expr::var{72}};
        expr v3{expr::var{73}};
        expr a1{expr::atom{"a"}};
        
        // Build: cons(cons(v1, v2), cons(v3, a1))
        expr left1{expr::cons{&v1, &v2}};
        expr right1{expr::cons{&v3, &a1}};
        expr outer1{expr::cons{&left1, &right1}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr a4{expr::atom{"z"}};
        expr a5{expr::atom{"a"}};
        
        // Build: cons(cons(a2, a3), cons(a4, a5))
        expr left2{expr::cons{&a2, &a3}};
        expr right2{expr::cons{&a4, &a5}};
        expr outer2{expr::cons{&left2, &right2}};
        
        assert(bm.unify(&outer1, &outer2));
        assert(bm.whnf(&v1) == &a2);
        assert(bm.whnf(&v2) == &a3);
        assert(bm.whnf(&v3) == &a4);
    }
    
    // Test 47: Unify with chains on both sides
    {
        bind_map bm(t);
        expr v1{expr::var{74}};
        expr v2{expr::var{75}};
        expr v3{expr::var{76}};
        expr v4{expr::var{77}};
        
        // Left chain: v1 -> v2 (unbound)
        bm.bindings[74] = &v2;
        
        // Right chain: v3 -> v4 (unbound)
        bm.bindings[76] = &v4;
        
        assert(bm.unify(&v1, &v3));  // Should unify v2 with v4
        assert(bm.whnf(&v1) == bm.whnf(&v3));
        assert(bm.whnf(&v2) == bm.whnf(&v4));
    }
    
    // Test 48: Cons unification where lhs succeeds but rhs fails
    {
        bind_map bm(t);
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"x"}};
        expr a4{expr::atom{"z"}};  // Different from a2
        expr c2{expr::cons{&a3, &a4}};
        
        assert(!bm.unify(&c1, &c2));  // lhs matches, rhs doesn't
    }
    
    // Test 49: Cons unification where first child fails (short circuit)
    {
        bind_map bm(t);
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"z"}};  // Different from a1
        expr a4{expr::atom{"y"}};
        expr c2{expr::cons{&a3, &a4}};
        
        assert(!bm.unify(&c1, &c2));  // Should fail on lhs
    }
    
    // Test 50: Very complex nested unification
    {
        bind_map bm(t);
        expr v1{expr::var{80}};
        expr v2{expr::var{81}};
        
        // Build complex structure with vars
        expr a1{expr::atom{"a"}};
        expr inner1{expr::cons{&v1, &a1}};
        expr inner2{expr::cons{&inner1, &v2}};
        expr outer1{expr::cons{&inner2, &a1}};
        
        // Build matching structure with atoms
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"a"}};
        expr inner3{expr::cons{&a2, &a3}};
        expr a4{expr::atom{"y"}};
        expr inner4{expr::cons{&inner3, &a4}};
        expr a5{expr::atom{"a"}};
        expr outer2{expr::cons{&inner4, &a5}};
        
        assert(bm.unify(&outer1, &outer2));
        assert(bm.whnf(&v1) == &a2);
        assert(bm.whnf(&v2) == &a4);
    }
}

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
    TEST(test_bind_map_occurs_check);
    TEST(test_bind_map_whnf);
    // TEST(test_bind_map_unify);
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
