#include "../hpp/expr.hpp"
#include "../hpp/bind_map.hpp"
#include "../hpp/constraint.hpp"
#include "../hpp/resolution.hpp"
#include "test_utils.hpp"

void test_trail_constructor() {
    // Basic construction - should not crash
    trail t1;
    assert(t1.depth() == 0);
    assert(t1.undo_stack.size() == 0);
    assert(t1.frame_boundary_stack.size() == 0);
    
    // Multiple trails can be constructed
    trail t2;
    trail t3;
    assert(t2.depth() == 0);
    assert(t3.depth() == 0);
    assert(t2.undo_stack.size() == 0);
    assert(t2.frame_boundary_stack.size() == 0);
    assert(t3.undo_stack.size() == 0);
    assert(t3.frame_boundary_stack.size() == 0);
    
    // Trail should be usable immediately after construction
    t1.push();
    assert(t1.depth() == 1);
    assert(t1.frame_boundary_stack.size() == 1);
    assert(t1.frame_boundary_stack.top() == 0);  // Boundary at position 0
    assert(t1.undo_stack.size() == 0);  // No operations logged yet
    t1.pop();
    assert(t1.depth() == 0);
    assert(t1.undo_stack.size() == 0);
    assert(t1.frame_boundary_stack.size() == 0);
}

void test_trail_push_pop() {
    trail t;
    assert(t.depth() == 0);
    assert(t.undo_stack.size() == 0);
    assert(t.frame_boundary_stack.size() == 0);
    
    // Single push/pop with no logged operations
    t.push();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    assert(t.frame_boundary_stack.top() == 0);
    assert(t.undo_stack.size() == 0);
    t.pop();
    assert(t.depth() == 0);
    assert(t.undo_stack.size() == 0);
    assert(t.frame_boundary_stack.size() == 0);
    
    // Multiple push/pop pairs with no logged operations
    t.push();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    assert(t.frame_boundary_stack.top() == 0);
    t.pop();
    assert(t.depth() == 0);
    assert(t.frame_boundary_stack.size() == 0);
    
    t.push();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    assert(t.frame_boundary_stack.top() == 0);
    t.pop();
    assert(t.depth() == 0);
    assert(t.frame_boundary_stack.size() == 0);
    
    t.push();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    assert(t.frame_boundary_stack.top() == 0);
    t.pop();
    assert(t.depth() == 0);
    assert(t.frame_boundary_stack.size() == 0);
    
    // Nested push/pop
    t.push();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    assert(t.frame_boundary_stack.top() == 0);
    t.push();
    assert(t.depth() == 2);
    assert(t.frame_boundary_stack.size() == 2);
    assert(t.frame_boundary_stack.top() == 0);  // Second frame also at position 0
    t.pop();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    assert(t.frame_boundary_stack.top() == 0);
    t.pop();
    assert(t.depth() == 0);
    assert(t.frame_boundary_stack.size() == 0);
    
    // Deeper nesting
    t.push();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    assert(t.frame_boundary_stack.top() == 0);
    t.push();
    assert(t.depth() == 2);
    assert(t.frame_boundary_stack.size() == 2);
    assert(t.frame_boundary_stack.top() == 0);
    t.push();
    assert(t.depth() == 3);
    assert(t.frame_boundary_stack.size() == 3);
    assert(t.frame_boundary_stack.top() == 0);
    t.pop();
    assert(t.depth() == 2);
    assert(t.frame_boundary_stack.size() == 2);
    t.pop();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    t.pop();
    assert(t.depth() == 0);
    assert(t.frame_boundary_stack.size() == 0);
    
    // Mixed nesting
    t.push();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    assert(t.frame_boundary_stack.top() == 0);
    t.push();
    assert(t.depth() == 2);
    assert(t.frame_boundary_stack.size() == 2);
    assert(t.frame_boundary_stack.top() == 0);
    t.pop();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    assert(t.frame_boundary_stack.top() == 0);
    t.push();
    assert(t.depth() == 2);
    assert(t.frame_boundary_stack.size() == 2);
    assert(t.frame_boundary_stack.top() == 0);
    t.pop();
    assert(t.depth() == 1);
    assert(t.frame_boundary_stack.size() == 1);
    t.pop();
    assert(t.depth() == 0);
    assert(t.frame_boundary_stack.size() == 0);
}

void test_trail_log() {
    // Test 1: Single log operation
    {
        trail t;
        int x = 5;
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
        
        t.push();
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        assert(t.undo_stack.size() == 0);
        
        x = 10;
        t.log([&x]() { x = 5; });
        assert(x == 10);
        assert(t.undo_stack.size() == 1);
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        t.pop();
        assert(x == 5);
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 2: Multiple log operations in one frame
    {
        trail t;
        int a = 1, b = 2, c = 3;
        t.push();
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        assert(t.undo_stack.size() == 0);
        
        a = 10;
        t.log([&a]() { a = 1; });
        assert(t.undo_stack.size() == 1);
        
        b = 20;
        t.log([&b]() { b = 2; });
        assert(t.undo_stack.size() == 2);
        
        c = 30;
        t.log([&c]() { c = 3; });
        assert(t.undo_stack.size() == 3);
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);  // Boundary still at 0
        
        assert(a == 10 && b == 20 && c == 30);
        t.pop();
        assert(a == 1 && b == 2 && c == 3);
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 3: Nested frames with logs
    {
        trail t;

        int x = 0;
        
        t.push();  // Frame 1
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        x = 1;
        t.log([&x]() { x = 0; });
        assert(t.undo_stack.size() == 1);
        
        t.push();  // Frame 2
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 1);  // Boundary at position 1
        assert(t.undo_stack.size() == 1);
        
        x = 2;
        t.log([&x]() { x = 1; });
        assert(t.undo_stack.size() == 2);
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 1);
        
        assert(x == 2);
        t.pop();  // Pop frame 2
        assert(x == 1);
        assert(t.undo_stack.size() == 1);  // One undo from frame 1 remains
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        t.pop();  // Pop frame 1
        assert(x == 0);
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 4: Multiple operations per frame with nesting
    {
        trail t;

        int a = 100, b = 200, c = 300;
        
        t.push();  // Frame 1
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        a = 111;
        t.log([&a]() { a = 100; });
        assert(t.undo_stack.size() == 1);
        
        b = 222;
        t.log([&b]() { b = 200; });
        assert(t.undo_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 0);
        
        t.push();  // Frame 2
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 2);  // Boundary at position 2
        assert(t.undo_stack.size() == 2);
        
        b = 333;
        t.log([&b]() { b = 222; });
        assert(t.undo_stack.size() == 3);
        
        c = 444;
        t.log([&c]() { c = 300; });
        assert(t.undo_stack.size() == 4);
        assert(t.frame_boundary_stack.top() == 2);
        
        t.push();  // Frame 3
        assert(t.frame_boundary_stack.size() == 3);
        assert(t.frame_boundary_stack.top() == 4);  // Boundary at position 4
        assert(t.undo_stack.size() == 4);
        
        a = 555;
        t.log([&a]() { a = 111; });
        assert(t.undo_stack.size() == 5);
        assert(t.frame_boundary_stack.size() == 3);
        assert(t.frame_boundary_stack.top() == 4);
        
        assert(a == 555 && b == 333 && c == 444);
        
        t.pop();  // Pop frame 3
        assert(a == 111 && b == 333 && c == 444);
        assert(t.undo_stack.size() == 4);  // Frame 3's undo removed
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 2);
        
        t.pop();  // Pop frame 2
        assert(a == 111 && b == 222 && c == 300);
        assert(t.undo_stack.size() == 2);  // Frame 2's undos removed
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        t.pop();  // Pop frame 1
        assert(a == 100 && b == 200 && c == 300);
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 5: Empty frame (push/pop with no logs)
    {
        trail t;

        int x = 42;
        t.push();
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        assert(t.undo_stack.size() == 0);
        
        // No logs
        x = 99;
        assert(x == 99);
        assert(t.undo_stack.size() == 0);  // Still no undos
        
        t.pop();
        assert(x == 99);  // Should remain unchanged since no undo was logged
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 6: Complex nested scenario with partial pops
    {
        trail t;

        int val = 0;
        
        t.push();  // Frame A
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        val = 1;
        t.log([&val]() { val = 0; });
        assert(t.undo_stack.size() == 1);
        
        t.push();  // Frame B
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 1);  // Boundary at position 1
        
        val = 2;
        t.log([&val]() { val = 1; });
        assert(t.undo_stack.size() == 2);
        
        t.push();  // Frame C
        assert(t.frame_boundary_stack.size() == 3);
        assert(t.frame_boundary_stack.top() == 2);  // Boundary at position 2
        
        val = 3;
        t.log([&val]() { val = 2; });
        assert(t.undo_stack.size() == 3);
        assert(t.frame_boundary_stack.top() == 2);
        
        assert(val == 3);
        t.pop();  // Pop C
        assert(val == 2);
        assert(t.undo_stack.size() == 2);  // Frame C's undo removed
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 1);
        
        // Add more to frame B
        val = 4;
        t.log([&val]() { val = 2; });
        assert(t.undo_stack.size() == 3);  // New undo added to frame B
        assert(t.frame_boundary_stack.top() == 1);  // Boundary unchanged
        
        assert(val == 4);
        t.pop();  // Pop B (should undo both operations in B)
        assert(val == 1);
        assert(t.undo_stack.size() == 1);  // Frame B's undos removed, frame A's remains
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        t.pop();  // Pop A
        assert(val == 0);
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 7: Multiple variables with complex state changes
    {
        trail t;

        int x = 10, y = 20, z = 30;
        
        t.push();  // Level 1
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        x += 5;
        t.log([&x]() { x -= 5; });
        y *= 2;
        t.log([&y]() { y /= 2; });
        assert(t.undo_stack.size() == 2);
        
        assert(x == 15 && y == 40 && z == 30);
        
        t.push();  // Level 2
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 2);
        
        z = x + y;  // z = 55
        t.log([&z]() { z = 30; });
        x = 0;
        t.log([&x]() { x = 15; });
        assert(t.undo_stack.size() == 4);
        
        assert(x == 0 && y == 40 && z == 55);
        
        t.push();  // Level 3
        assert(t.frame_boundary_stack.size() == 3);
        assert(t.frame_boundary_stack.top() == 4);
        
        y = 100;
        t.log([&y]() { y = 40; });
        assert(t.undo_stack.size() == 5);
        
        assert(x == 0 && y == 100 && z == 55);
        
        t.pop();  // Pop level 3
        assert(x == 0 && y == 40 && z == 55);
        assert(t.undo_stack.size() == 4);
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 2);
        
        t.pop();  // Pop level 2
        assert(x == 15 && y == 40 && z == 30);
        assert(t.undo_stack.size() == 2);
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        t.pop();  // Pop level 1
        assert(x == 10 && y == 20 && z == 30);
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 8: Deeply nested frames (5 levels)
    {
        trail t;

        int depth = 0;
        
        t.push();
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        depth = 1;
        t.log([&depth]() { depth = 0; });
        assert(t.undo_stack.size() == 1);
        
        t.push();
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 1);
        depth = 2;
        t.log([&depth]() { depth = 1; });
        assert(t.undo_stack.size() == 2);
        
        t.push();
        assert(t.frame_boundary_stack.size() == 3);
        assert(t.frame_boundary_stack.top() == 2);
        depth = 3;
        t.log([&depth]() { depth = 2; });
        assert(t.undo_stack.size() == 3);
        
        t.push();
        assert(t.frame_boundary_stack.size() == 4);
        assert(t.frame_boundary_stack.top() == 3);
        depth = 4;
        t.log([&depth]() { depth = 3; });
        assert(t.undo_stack.size() == 4);
        
        t.push();
        assert(t.frame_boundary_stack.size() == 5);
        assert(t.frame_boundary_stack.top() == 4);
        depth = 5;
        t.log([&depth]() { depth = 4; });
        assert(t.undo_stack.size() == 5);
        
        assert(depth == 5);
        t.pop();
        assert(depth == 4);
        assert(t.undo_stack.size() == 4);
        assert(t.frame_boundary_stack.size() == 4);
        t.pop();
        assert(depth == 3);
        assert(t.undo_stack.size() == 3);
        assert(t.frame_boundary_stack.size() == 3);
        t.pop();
        assert(depth == 2);
        assert(t.undo_stack.size() == 2);
        assert(t.frame_boundary_stack.size() == 2);
        t.pop();
        assert(depth == 1);
        assert(t.undo_stack.size() == 1);
        assert(t.frame_boundary_stack.size() == 1);
        t.pop();
        assert(depth == 0);
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 9: Many operations in a single frame
    {
        trail t;

        std::vector<int> values(10, 0);
        
        t.push();
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        for (int i = 0; i < 10; i++) {
            values[i] = i + 1;
            t.log([&values, i]() { values[i] = 0; });
        }
        assert(t.undo_stack.size() == 10);  // All 10 operations logged
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);  // Boundary unchanged
        
        for (int i = 0; i < 10; i++) {
            assert(values[i] == i + 1);
        }
        
        t.pop();
        assert(t.undo_stack.size() == 0);  // All undos executed
        assert(t.frame_boundary_stack.size() == 0);
        
        for (int i = 0; i < 10; i++) {
            assert(values[i] == 0);
        }
    }
    
    // Test 10: Interleaved push/pop/log operations
    {
        trail t;

        int state = 0;
        
        t.push();  // Frame 1
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        state = 1;
        t.log([&state]() { state = 0; });
        assert(t.undo_stack.size() == 1);
        
        t.push();  // Frame 2
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 1);
        state = 2;
        t.log([&state]() { state = 1; });
        assert(t.undo_stack.size() == 2);
        
        t.pop();  // Pop frame 2
        assert(state == 1);
        assert(t.undo_stack.size() == 1);
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        
        t.push();  // New frame 2
        assert(t.frame_boundary_stack.size() == 2);
        assert(t.frame_boundary_stack.top() == 1);
        state = 3;
        t.log([&state]() { state = 1; });
        assert(t.undo_stack.size() == 2);
        
        t.push();  // Frame 3
        assert(t.frame_boundary_stack.size() == 3);
        assert(t.frame_boundary_stack.top() == 2);
        state = 4;
        t.log([&state]() { state = 3; });
        assert(t.undo_stack.size() == 3);
        
        assert(state == 4);
        t.pop();  // Pop frame 3
        assert(state == 3);
        assert(t.undo_stack.size() == 2);
        assert(t.frame_boundary_stack.size() == 2);
        t.pop();  // Pop frame 2
        assert(state == 1);
        assert(t.undo_stack.size() == 1);
        assert(t.frame_boundary_stack.size() == 1);
        t.pop();  // Pop frame 1
        assert(state == 0);
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 11: String modifications
    {
        trail t;
        
        std::string str = "original";
        
        t.push();
        assert(t.frame_boundary_stack.size() == 1);
        assert(t.frame_boundary_stack.top() == 0);
        str = "modified";
        t.log([&str]() { str = "original"; });
        assert(t.undo_stack.size() == 1);
        
        assert(str == "modified");
        t.pop();
        assert(str == "original");
        assert(t.undo_stack.size() == 0);
        assert(t.frame_boundary_stack.size() == 0);
    }
    
    // Test 12: Multiple independent trails
    {
        trail t1, t2;
        int x = 1, y = 2;
        
        t1.push();
        assert(t1.frame_boundary_stack.size() == 1);
        assert(t1.frame_boundary_stack.top() == 0);
        assert(t2.frame_boundary_stack.size() == 0);  // t2 unaffected
        x = 10;
        t1.log([&x]() { x = 1; });
        assert(t1.undo_stack.size() == 1);
        
        t2.push();
        assert(t2.frame_boundary_stack.size() == 1);
        assert(t2.frame_boundary_stack.top() == 0);
        assert(t1.frame_boundary_stack.size() == 1);  // t1 unaffected
        y = 20;
        t2.log([&y]() { y = 2; });
        assert(t2.undo_stack.size() == 1);
        
        assert(x == 10 && y == 20);
        
        t1.pop();
        assert(x == 1 && y == 20);
        assert(t1.undo_stack.size() == 0);
        assert(t1.frame_boundary_stack.size() == 0);
        assert(t2.undo_stack.size() == 1);  // t2 unaffected
        
        t2.pop();
        assert(x == 1 && y == 2);
        assert(t2.undo_stack.size() == 0);
        assert(t2.frame_boundary_stack.size() == 0);
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
        assert(t1.frame_boundary_stack.size() == 1);
        assert(t1.frame_boundary_stack.top() == 0);
        data1 += 10;  // 110
        t1.log([&data1]() { data1 -= 10; });
        assert(t1.undo_stack.size() == 1);
        
        t1.push();  // Frame 1.2
        assert(t1.depth() == 2);
        assert(t1.frame_boundary_stack.size() == 2);
        assert(t1.frame_boundary_stack.top() == 1);
        data1 *= 2;  // 220
        t1.log([&data1]() { data1 /= 2; });
        assert(t1.undo_stack.size() == 2);
        
        t1.push();  // Frame 1.3
        assert(t1.depth() == 3);
        assert(t1.frame_boundary_stack.size() == 3);
        assert(t1.frame_boundary_stack.top() == 2);
        data1 += 80;  // 300
        t1.log([&data1]() { data1 -= 80; });
        assert(t1.undo_stack.size() == 3);
        
        assert(data1 == 300);
        
        // === Trail 2: Nested frames with data2 ===
        t2.push();  // Frame 2.1
        assert(t2.depth() == 1);
        assert(t2.frame_boundary_stack.size() == 1);
        assert(t2.frame_boundary_stack.top() == 0);
        data2 -= 50;  // 150
        t2.log([&data2]() { data2 += 50; });
        assert(t2.undo_stack.size() == 1);
        
        t2.push();  // Frame 2.2
        assert(t2.depth() == 2);
        assert(t2.frame_boundary_stack.size() == 2);
        assert(t2.frame_boundary_stack.top() == 1);
        data2 *= 3;  // 450
        t2.log([&data2]() { data2 /= 3; });
        assert(t2.undo_stack.size() == 2);
        
        assert(data2 == 450);
        
        // === Trail 3: Single frame with multiple operations on data3 ===
        t3.push();  // Frame 3.1
        assert(t3.depth() == 1);
        assert(t3.frame_boundary_stack.size() == 1);
        assert(t3.frame_boundary_stack.top() == 0);
        data3 /= 3;  // 100
        t3.log([&data3]() { data3 *= 3; });
        assert(t3.undo_stack.size() == 1);
        data3 += 50;  // 150
        t3.log([&data3]() { data3 -= 50; });
        assert(t3.undo_stack.size() == 2);
        data3 *= 4;  // 600
        t3.log([&data3]() { data3 /= 4; });
        assert(t3.undo_stack.size() == 3);
        
        assert(data3 == 600);
        
        // Verify all data is at expected state
        assert(data1 == 300 && data2 == 450 && data3 == 600);
        
        // === Pop trail 1 innermost frame ===
        t1.pop();  // Pop frame 1.3
        assert(t1.depth() == 2);
        assert(t1.undo_stack.size() == 2);
        assert(t1.frame_boundary_stack.size() == 2);
        assert(data1 == 220);  // Restored from frame 1.3
        assert(data2 == 450);  // Unchanged
        assert(data3 == 600);  // Unchanged
        
        // === Add more to trail 2 ===
        t2.push();  // Frame 2.3
        assert(t2.depth() == 3);
        assert(t2.frame_boundary_stack.size() == 3);
        assert(t2.frame_boundary_stack.top() == 2);
        data2 += 50;  // 500
        t2.log([&data2]() { data2 -= 50; });
        assert(t2.undo_stack.size() == 3);
        
        assert(data1 == 220 && data2 == 500 && data3 == 600);
        
        // === Pop trail 3 completely ===
        t3.pop();  // Pop frame 3.1
        assert(t3.depth() == 0);
        assert(t3.undo_stack.size() == 0);
        assert(t3.frame_boundary_stack.size() == 0);
        assert(data1 == 220);  // Unchanged
        assert(data2 == 500);  // Unchanged
        assert(data3 == 300);  // Restored to original
        
        // === Add new frame to trail 3 ===
        t3.push();  // New frame 3.1
        assert(t3.depth() == 1);
        assert(t3.frame_boundary_stack.size() == 1);
        assert(t3.frame_boundary_stack.top() == 0);
        data3 -= 100;  // 200
        t3.log([&data3]() { data3 += 100; });
        assert(t3.undo_stack.size() == 1);
        data3 *= 5;  // 1000
        t3.log([&data3]() { data3 /= 5; });
        assert(t3.undo_stack.size() == 2);
        
        assert(data1 == 220 && data2 == 500 && data3 == 1000);
        
        // === Pop trail 2 innermost frame ===
        t2.pop();  // Pop frame 2.3
        assert(t2.depth() == 2);
        assert(t2.undo_stack.size() == 2);
        assert(t2.frame_boundary_stack.size() == 2);
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
        assert(t2.undo_stack.size() == 1);
        assert(t2.frame_boundary_stack.size() == 1);
        assert(data2 == 150);  // Restored from frame 2.2
        
        t2.pop();  // Pop frame 2.1
        assert(t2.depth() == 0);
        assert(t2.undo_stack.size() == 0);
        assert(t2.frame_boundary_stack.size() == 0);
        assert(data2 == 200);  // Restored to original
        
        assert(data1 == 110 && data2 == 200 && data3 == 1000);
        
        // === Pop trail 3 ===
        t3.pop();  // Pop frame 3.1
        assert(t3.depth() == 0);
        assert(t3.undo_stack.size() == 0);
        assert(t3.frame_boundary_stack.size() == 0);
        assert(data3 == 300);  // Restored to original
        
        assert(data1 == 110 && data2 == 200 && data3 == 300);
        
        // === Pop trail 1 last frame ===
        t1.pop();  // Pop frame 1.1
        assert(t1.depth() == 0);
        assert(t1.undo_stack.size() == 0);
        assert(t1.frame_boundary_stack.size() == 0);
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
    assert(pool1.exprs.size() == 0);
    assert(pool1.exprs.empty());
    
    // Multiple pools can be constructed with same trail
    expr_pool pool2(t);
    assert(pool2.size() == 0);
    assert(pool2.exprs.size() == 0);
    assert(pool2.exprs.empty());
    
    // Multiple pools with different trails
    trail t2;
    expr_pool pool3(t2);
    assert(pool3.size() == 0);
    assert(pool3.exprs.size() == 0);
    assert(pool3.exprs.empty());
    
    // Pool should be usable immediately after construction
    t.push();
    const expr* e1 = pool1.atom("test");
    assert(e1 != nullptr);
    assert(pool1.size() == 1);
    assert(pool1.exprs.size() == 1);
    assert(pool1.exprs.count(*e1) == 1);
    
    // Other pools are independent
    assert(pool2.size() == 0);
    assert(pool2.exprs.size() == 0);
    assert(pool3.size() == 0);
    assert(pool3.exprs.size() == 0);
    
    // Add to pool2 with same trail
    const expr* e2 = pool2.atom("test2");
    assert(pool2.size() == 1);
    assert(pool2.exprs.size() == 1);
    assert(pool2.exprs.count(*e2) == 1);
    assert(pool1.size() == 1);  // pool1 unchanged
    assert(pool1.exprs.size() == 1);
    
    // Pop should affect both pool1 and pool2 since they share trail t
    t.pop();
    assert(pool1.size() == 0);
    assert(pool1.exprs.size() == 0);
    assert(pool2.size() == 0);
    assert(pool2.exprs.size() == 0);
    
    // pool3 with different trail is unaffected
    assert(pool3.size() == 0);
    assert(pool3.exprs.size() == 0);
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
    assert(pool.exprs.size() == 1);
    assert(pool.exprs.count(*e1) == 1);
    
    // Empty string
    const expr* e2 = pool.atom("");
    assert(std::get<expr::atom>(e2->content).value == "");
    assert(pool.size() == 2);
    assert(pool.exprs.size() == 2);
    assert(pool.exprs.count(*e2) == 1);
    assert(e1 != e2);
    
    // Interning - same string should return same pointer
    const expr* e3 = pool.atom("test");
    assert(e1 == e3);
    assert(pool.size() == 2);  // No new entry added
    assert(pool.exprs.size() == 2);
    
    // Different strings should return different pointers
    const expr* e4 = pool.atom("different");
    assert(e1 != e4);
    assert(pool.size() == 3);
    assert(pool.exprs.size() == 3);
    assert(pool.exprs.count(*e4) == 1);
    
    // Multiple calls with same string
    const expr* e5 = pool.atom("shared");
    assert(pool.exprs.size() == 4);
    const expr* e6 = pool.atom("shared");
    const expr* e7 = pool.atom("shared");
    assert(e5 == e6);
    assert(e6 == e7);
    assert(pool.size() == 4);  // Only one "shared" added
    assert(pool.exprs.size() == 4);
    assert(pool.exprs.count(*e5) == 1);
    
    // Special characters
    const expr* e8 = pool.atom("!@#$");
    assert(pool.exprs.size() == 5);
    const expr* e9 = pool.atom("!@#$");
    assert(e8 == e9);
    assert(pool.size() == 5);
    assert(pool.exprs.size() == 5);
    assert(pool.exprs.count(*e8) == 1);
    
    // Long strings
    std::string long_str(1000, 'x');
    const expr* e10 = pool.atom(long_str);
    assert(pool.exprs.size() == 6);
    const expr* e11 = pool.atom(long_str);
    assert(e10 == e11);
    assert(pool.size() == 6);
    assert(pool.exprs.size() == 6);
    assert(pool.exprs.count(*e10) == 1);
    
    // Test backtracking: push frame, add content, pop frame
    size_t size_before = pool.size();
    assert(pool.exprs.size() == size_before);
    t.push();
    const expr* temp1 = pool.atom("temporary1");
    const expr* temp2 = pool.atom("temporary2");
    assert(pool.size() == size_before + 2);
    assert(pool.exprs.size() == size_before + 2);
    assert(pool.exprs.count(*temp1) == 1);
    assert(pool.exprs.count(*temp2) == 1);
    t.pop();
    assert(pool.size() == size_before);  // Should be back to original size
    assert(pool.exprs.size() == size_before);
    
    // Test corner case: intern same content in nested frames
    t.push();  // Frame 1
    const expr* content_c = pool.atom("content_c");
    size_t checkpoint1 = pool.size();
    assert(content_c != nullptr);
    assert(pool.exprs.size() == checkpoint1);
    assert(pool.exprs.count(*content_c) == 1);
    
    t.push();  // Frame 2
    const expr* content_c_again = pool.atom("content_c");  // Should return same pointer, no log
    assert(content_c == content_c_again);
    assert(pool.size() == checkpoint1);  // Size unchanged
    assert(pool.exprs.size() == checkpoint1);
    
    t.pop();  // Pop frame 2
    assert(pool.size() == checkpoint1);  // Size still unchanged
    assert(pool.exprs.size() == checkpoint1);
    
    // Verify content_c is still there
    const expr* content_c_verify = pool.atom("content_c");
    assert(content_c == content_c_verify);
    assert(pool.size() == checkpoint1);
    assert(pool.exprs.size() == checkpoint1);
    assert(pool.exprs.count(*content_c) == 1);
    
    t.pop();  // Pop frame 1
    // Now content_c should be removed

    assert(pool.size() == size_before);
    assert(pool.exprs.size() == size_before);
    
    // Test nested pushes with checkpoints
    size_t checkpoint_start = pool.size();
    assert(pool.exprs.size() == checkpoint_start);
    
    t.push();  // Level 1
    pool.atom("level1_a");
    pool.atom("level1_b");
    size_t checkpoint_level1 = pool.size();
    assert(checkpoint_level1 == checkpoint_start + 2);
    assert(pool.exprs.size() == checkpoint_level1);
    
    t.push();  // Level 2
    pool.atom("level2_a");
    pool.atom("level2_b");
    pool.atom("level2_c");
    size_t checkpoint_level2 = pool.size();
    assert(checkpoint_level2 == checkpoint_level1 + 3);
    assert(pool.exprs.size() == checkpoint_level2);
    
    t.push();  // Level 3
    pool.atom("level3_a");
    size_t checkpoint_level3 = pool.size();
    assert(checkpoint_level3 == checkpoint_level2 + 1);
    assert(pool.exprs.size() == checkpoint_level3);
    
    // Pop level 3
    t.pop();
    assert(pool.size() == checkpoint_level2);
    assert(pool.exprs.size() == checkpoint_level2);
    
    // Pop level 2
    t.pop();
    assert(pool.size() == checkpoint_level1);
    assert(pool.exprs.size() == checkpoint_level1);
    
    // Pop level 1
    t.pop();
    assert(pool.size() == checkpoint_start);
    assert(pool.exprs.size() == checkpoint_start);
    
    // Test corner case: content added in earlier frame should not be removed by later frame pop
    t.push();  // Frame A
    assert(pool.exprs.size() == checkpoint_start);
    
    const expr* early_content_1 = pool.atom("early_1");
    assert(pool.exprs.size() == checkpoint_start + 1);
    assert(pool.exprs.count(*early_content_1) == 1);
    
    const expr* early_content_2 = pool.atom("early_2");
    assert(pool.exprs.size() == checkpoint_start + 2);
    assert(pool.exprs.count(*early_content_2) == 1);
    
    size_t checkpoint_a = pool.size();
    assert(checkpoint_a == checkpoint_start + 2);
    assert(pool.exprs.size() == checkpoint_a);
    
    t.push();  // Frame B
    assert(pool.exprs.size() == checkpoint_a);
    
    const expr* mid_content = pool.atom("mid_content");
    assert(pool.exprs.size() == checkpoint_a + 1);
    assert(pool.exprs.count(*mid_content) == 1);
    
    size_t checkpoint_b = pool.size();
    assert(checkpoint_b == checkpoint_a + 1);
    assert(pool.exprs.size() == checkpoint_b);
    
    // Re-intern early content in Frame B - should not log since already exists
    const expr* early_content_1_again = pool.atom("early_1");
    assert(early_content_1 == early_content_1_again);
    assert(pool.size() == checkpoint_b);  // Size unchanged
    assert(pool.exprs.size() == checkpoint_b);  // Set size also unchanged
    assert(pool.exprs.count(*early_content_1) == 1);  // Still in set
    
    // Add more new content in Frame B
    const expr* late_content_1 = pool.atom("late_1");
    assert(pool.exprs.size() == checkpoint_b + 1);
    assert(pool.exprs.count(*late_content_1) == 1);
    
    const expr* late_content_2 = pool.atom("late_2");
    assert(pool.exprs.size() == checkpoint_b + 2);
    assert(pool.exprs.count(*late_content_2) == 1);
    
    size_t checkpoint_b_final = pool.size();
    assert(checkpoint_b_final == checkpoint_b + 2);
    assert(pool.exprs.size() == checkpoint_b_final);
    
    t.push();  // Frame C
    assert(pool.exprs.size() == checkpoint_b_final);
    
    // Re-intern content from both Frame A and Frame B
    const expr* early_content_2_again = pool.atom("early_2");
    assert(early_content_2 == early_content_2_again);
    assert(pool.exprs.size() == checkpoint_b_final);  // No change
    assert(pool.exprs.count(*early_content_2) == 1);
    
    const expr* mid_content_again = pool.atom("mid_content");
    assert(mid_content == mid_content_again);
    assert(pool.size() == checkpoint_b_final);  // Size unchanged
    assert(pool.exprs.size() == checkpoint_b_final);  // Set size unchanged
    assert(pool.exprs.count(*mid_content) == 1);
    
    // Add new content in Frame C
    const expr* frame_c_content = pool.atom("frame_c");
    assert(pool.exprs.size() == checkpoint_b_final + 1);
    assert(pool.exprs.count(*frame_c_content) == 1);
    
    size_t checkpoint_c = pool.size();
    assert(checkpoint_c == checkpoint_b_final + 1);
    assert(pool.exprs.size() == checkpoint_c);
    
    // Verify all content from all frames is present
    assert(pool.exprs.count(*early_content_1) == 1);
    assert(pool.exprs.count(*early_content_2) == 1);
    assert(pool.exprs.count(*mid_content) == 1);
    assert(pool.exprs.count(*late_content_1) == 1);
    assert(pool.exprs.count(*late_content_2) == 1);
    assert(pool.exprs.count(*frame_c_content) == 1);
    
    // Pop Frame C - only frame_c_content should be removed
    t.pop();
    assert(pool.size() == checkpoint_b_final);
    assert(pool.exprs.size() == checkpoint_b_final);
    
    // frame_c_content should no longer be in the set
    // Note: We can't safely dereference frame_c_content after it's removed from the set
    // because the pointer may be invalidated. We can only check what remains.
    
    // Verify early and mid content still exist
    const expr* verify_early_1 = pool.atom("early_1");
    assert(verify_early_1 == early_content_1);
    assert(pool.exprs.count(*verify_early_1) == 1);
    
    const expr* verify_mid = pool.atom("mid_content");
    assert(verify_mid == mid_content);
    assert(pool.exprs.count(*verify_mid) == 1);
    
    const expr* verify_late_1 = pool.atom("late_1");
    assert(verify_late_1 == late_content_1);
    assert(pool.exprs.count(*verify_late_1) == 1);
    
    assert(pool.size() == checkpoint_b_final);  // Still unchanged
    assert(pool.exprs.size() == checkpoint_b_final);
    
    // Verify all Frame A and Frame B content is still present
    assert(pool.exprs.count(*early_content_1) == 1);
    assert(pool.exprs.count(*early_content_2) == 1);
    assert(pool.exprs.count(*mid_content) == 1);
    assert(pool.exprs.count(*late_content_1) == 1);
    assert(pool.exprs.count(*late_content_2) == 1);
    
    // Pop Frame B - should remove mid_content, late_1, late_2 but NOT early_1, early_2
    t.pop();
    assert(pool.size() == checkpoint_a);
    assert(pool.exprs.size() == checkpoint_a);
    
    // Verify early content still exists
    const expr* verify_early_1_after_b = pool.atom("early_1");
    assert(verify_early_1_after_b == early_content_1);
    assert(pool.exprs.count(*verify_early_1_after_b) == 1);
    
    const expr* verify_early_2_after_b = pool.atom("early_2");
    assert(verify_early_2_after_b == early_content_2);
    assert(pool.exprs.count(*verify_early_2_after_b) == 1);
    
    assert(pool.size() == checkpoint_a);  // Still unchanged
    assert(pool.exprs.size() == checkpoint_a);
    
    // Verify only Frame A content remains
    assert(pool.exprs.count(*early_content_1) == 1);
    assert(pool.exprs.count(*early_content_2) == 1);
    
    // Pop Frame A - should remove early_1 and early_2
    t.pop();
    assert(pool.size() == checkpoint_start);
    assert(pool.exprs.size() == checkpoint_start);
    
    // Pop initial frame
    t.pop();
    assert(pool.size() == 0);
    assert(pool.exprs.size() == 0);
    assert(pool.exprs.empty());
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
    assert(pool.exprs.size() == 1);
    assert(pool.exprs.count(*e1) == 1);
    
    // Interning - same index should return same pointer
    const expr* e2 = pool.var(0);
    assert(e1 == e2);
    assert(pool.size() == 1);  // No new entry added
    assert(pool.exprs.size() == 1);
    
    // Different indices should return different pointers
    const expr* e3 = pool.var(1);
    assert(e1 != e3);
    assert(pool.size() == 2);
    assert(pool.exprs.size() == 2);
    assert(pool.exprs.count(*e3) == 1);
    
    // Multiple calls with same index
    const expr* e4 = pool.var(42);
    assert(pool.exprs.size() == 3);
    const expr* e5 = pool.var(42);
    const expr* e6 = pool.var(42);
    assert(e4 == e5);
    assert(e5 == e6);
    assert(pool.size() == 3);  // Only one var(42) added
    assert(pool.exprs.size() == 3);
    assert(pool.exprs.count(*e4) == 1);
    
    // Edge cases
    const expr* e7 = pool.var(UINT32_MAX);
    assert(pool.exprs.size() == 4);
    const expr* e8 = pool.var(UINT32_MAX);
    assert(e7 == e8);
    assert(pool.size() == 4);
    assert(pool.exprs.size() == 4);
    assert(pool.exprs.count(*e7) == 1);
    
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
    assert(pool.exprs.size() == size_before);
    t.push();
    const expr* temp1 = pool.var(9999);
    const expr* temp2 = pool.var(8888);
    assert(pool.size() == size_before + 2);
    assert(pool.exprs.size() == size_before + 2);
    assert(pool.exprs.count(*temp1) == 1);
    assert(pool.exprs.count(*temp2) == 1);
    t.pop();
    assert(pool.size() == size_before);  // Should be back to original size
    assert(pool.exprs.size() == size_before);
    
    // Test corner case: intern same content in nested frames
    t.push();  // Frame 1
    const expr* var_100 = pool.var(100);
    size_t checkpoint1 = pool.size();
    assert(var_100 != nullptr);
    assert(pool.exprs.size() == checkpoint1);
    assert(pool.exprs.count(*var_100) == 1);
    
    t.push();  // Frame 2
    const expr* var_100_again = pool.var(100);  // Should return same pointer, no log
    assert(var_100 == var_100_again);
    assert(pool.size() == checkpoint1);  // Size unchanged
    assert(pool.exprs.size() == checkpoint1);
    
    t.pop();  // Pop frame 2
    assert(pool.size() == checkpoint1);  // Size still unchanged
    assert(pool.exprs.size() == checkpoint1);
    
    // Verify var_100 is still there
    const expr* var_100_verify = pool.var(100);
    assert(var_100 == var_100_verify);
    assert(pool.size() == checkpoint1);
    assert(pool.exprs.size() == checkpoint1);
    assert(pool.exprs.count(*var_100) == 1);
    
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
    assert(pool.exprs.size() == checkpoint_start);
    
    const expr* early_var_1 = pool.var(500);
    assert(pool.exprs.size() == checkpoint_start + 1);
    assert(pool.exprs.count(*early_var_1) == 1);
    
    const expr* early_var_2 = pool.var(501);
    assert(pool.exprs.size() == checkpoint_start + 2);
    assert(pool.exprs.count(*early_var_2) == 1);
    
    size_t checkpoint_a = pool.size();
    assert(checkpoint_a == checkpoint_start + 2);
    assert(pool.exprs.size() == checkpoint_a);
    
    t.push();  // Frame B
    assert(pool.exprs.size() == checkpoint_a);
    
    const expr* mid_var = pool.var(600);
    assert(pool.exprs.size() == checkpoint_a + 1);
    assert(pool.exprs.count(*mid_var) == 1);
    
    size_t checkpoint_b = pool.size();
    assert(checkpoint_b == checkpoint_a + 1);
    assert(pool.exprs.size() == checkpoint_b);
    
    // Re-intern early content in Frame B - should not log since already exists
    const expr* early_var_1_again = pool.var(500);
    assert(early_var_1 == early_var_1_again);
    assert(pool.size() == checkpoint_b);  // Size unchanged
    assert(pool.exprs.size() == checkpoint_b);  // Set size also unchanged
    assert(pool.exprs.count(*early_var_1) == 1);  // Still in set
    
    // Add more new content in Frame B
    const expr* late_var_1 = pool.var(700);
    assert(pool.exprs.size() == checkpoint_b + 1);
    assert(pool.exprs.count(*late_var_1) == 1);
    
    const expr* late_var_2 = pool.var(701);
    assert(pool.exprs.size() == checkpoint_b + 2);
    assert(pool.exprs.count(*late_var_2) == 1);
    
    size_t checkpoint_b_final = pool.size();
    assert(checkpoint_b_final == checkpoint_b + 2);
    assert(pool.exprs.size() == checkpoint_b_final);
    
    t.push();  // Frame C
    assert(pool.exprs.size() == checkpoint_b_final);
    
    // Re-intern content from both Frame A and Frame B
    const expr* early_var_2_again = pool.var(501);
    assert(early_var_2 == early_var_2_again);
    assert(pool.exprs.size() == checkpoint_b_final);  // No change
    assert(pool.exprs.count(*early_var_2) == 1);
    
    const expr* mid_var_again = pool.var(600);
    assert(mid_var == mid_var_again);
    assert(pool.size() == checkpoint_b_final);  // Size unchanged
    assert(pool.exprs.size() == checkpoint_b_final);  // Set size unchanged
    assert(pool.exprs.count(*mid_var) == 1);
    
    // Add new content in Frame C
    const expr* frame_c_var = pool.var(800);
    assert(pool.exprs.size() == checkpoint_b_final + 1);
    assert(pool.exprs.count(*frame_c_var) == 1);
    
    size_t checkpoint_c = pool.size();
    assert(checkpoint_c == checkpoint_b_final + 1);
    assert(pool.exprs.size() == checkpoint_c);
    
    // Verify all content from all frames is present
    assert(pool.exprs.count(*early_var_1) == 1);
    assert(pool.exprs.count(*early_var_2) == 1);
    assert(pool.exprs.count(*mid_var) == 1);
    assert(pool.exprs.count(*late_var_1) == 1);
    assert(pool.exprs.count(*late_var_2) == 1);
    assert(pool.exprs.count(*frame_c_var) == 1);
    
    // Pop Frame C - only frame_c_var should be removed
    t.pop();
    assert(pool.size() == checkpoint_b_final);
    assert(pool.exprs.size() == checkpoint_b_final);
    
    // Verify early and mid content still exist
    const expr* verify_early_1 = pool.var(500);
    assert(verify_early_1 == early_var_1);
    assert(pool.exprs.count(*verify_early_1) == 1);
    
    const expr* verify_mid = pool.var(600);
    assert(verify_mid == mid_var);
    assert(pool.exprs.count(*verify_mid) == 1);
    
    const expr* verify_late_1 = pool.var(700);
    assert(verify_late_1 == late_var_1);
    assert(pool.exprs.count(*verify_late_1) == 1);
    
    assert(pool.size() == checkpoint_b_final);  // Still unchanged
    assert(pool.exprs.size() == checkpoint_b_final);
    
    // Verify all Frame A and Frame B content is still present
    assert(pool.exprs.count(*early_var_1) == 1);
    assert(pool.exprs.count(*early_var_2) == 1);
    assert(pool.exprs.count(*mid_var) == 1);
    assert(pool.exprs.count(*late_var_1) == 1);
    assert(pool.exprs.count(*late_var_2) == 1);
    
    // Pop Frame B - should remove mid_var, late_var_1, late_var_2 but NOT early vars
    t.pop();
    assert(pool.size() == checkpoint_a);
    assert(pool.exprs.size() == checkpoint_a);
    
    // Verify early content still exists
    const expr* verify_early_1_after_b = pool.var(500);
    assert(verify_early_1_after_b == early_var_1);
    assert(pool.exprs.count(*verify_early_1_after_b) == 1);
    
    const expr* verify_early_2_after_b = pool.var(501);
    assert(verify_early_2_after_b == early_var_2);
    assert(pool.exprs.count(*verify_early_2_after_b) == 1);
    
    assert(pool.size() == checkpoint_a);  // Still unchanged
    assert(pool.exprs.size() == checkpoint_a);
    
    // Verify only Frame A content remains
    assert(pool.exprs.count(*early_var_1) == 1);
    assert(pool.exprs.count(*early_var_2) == 1);
    
    // Pop Frame A - should remove early vars
    t.pop();
    assert(pool.size() == checkpoint_start);
    assert(pool.exprs.size() == checkpoint_start);
    
    // Pop initial frame
    t.pop();
    assert(pool.size() == 0);
    assert(pool.exprs.size() == 0);
    assert(pool.exprs.empty());
}

void test_expr_pool_cons() {
    trail t;
    expr_pool pool(t);
    
    // Push initial frame before any operations
    t.push();
    
    // Basic cons creation
    const expr* left = pool.atom("left");
    const expr* right = pool.atom("right");
    assert(pool.exprs.size() == 2);
    const expr* c1 = pool.cons(left, right);
    
    assert(c1 != nullptr);
    assert(std::holds_alternative<expr::cons>(c1->content));
    const expr::cons& cons1 = std::get<expr::cons>(c1->content);
    assert(cons1.lhs == left);
    assert(cons1.rhs == right);
    assert(pool.size() == 3);  // left, right, cons
    assert(pool.exprs.size() == 3);
    assert(pool.exprs.count(*c1) == 1);
    
    // Interning - same cons should return same pointer
    const expr* c2 = pool.cons(left, right);
    assert(c1 == c2);
    assert(pool.size() == 3);  // No new entry added
    assert(pool.exprs.size() == 3);
    
    // Different cons should return different pointers
    const expr* c3 = pool.cons(right, left);  // Swapped
    assert(c1 != c3);
    assert(pool.size() == 4);
    assert(pool.exprs.size() == 4);
    assert(pool.exprs.count(*c3) == 1);
    
    // Cons with variables
    const expr* v1 = pool.var(10);
    const expr* v2 = pool.var(20);
    assert(pool.exprs.size() == 6);
    const expr* c4 = pool.cons(v1, v2);
    assert(pool.exprs.size() == 7);
    const expr* c5 = pool.cons(v1, v2);
    assert(c4 == c5);
    assert(pool.size() == 7);  // v1, v2, cons(v1,v2)
    assert(pool.exprs.size() == 7);
    assert(pool.exprs.count(*c4) == 1);
    
    // Nested cons
    const expr* inner = pool.cons(pool.atom("a"), pool.atom("b"));
    assert(pool.exprs.count(*inner) == 1);
    const expr* outer = pool.cons(inner, pool.atom("c"));
    assert(pool.exprs.count(*outer) == 1);
    const expr* outer2 = pool.cons(inner, pool.atom("c"));
    assert(outer == outer2);
    size_t size_after_nested = pool.size();
    assert(pool.exprs.size() == size_after_nested);
    
    // Same expr on both sides
    const expr* same = pool.atom("same");
    const expr* c6 = pool.cons(same, same);
    assert(pool.exprs.count(*c6) == 1);
    const expr* c7 = pool.cons(same, same);
    assert(c6 == c7);
    assert(pool.size() == size_after_nested + 2);  // same, cons(same,same)
    assert(pool.exprs.size() == size_after_nested + 2);
    
    // Deep nesting with interning
    const expr* d1 = pool.cons(pool.atom("x"), pool.atom("y"));
    assert(pool.exprs.count(*d1) == 1);
    const expr* d2 = pool.cons(d1, d1);
    assert(pool.exprs.count(*d2) == 1);
    const expr* d3 = pool.cons(d2, d2);
    assert(pool.exprs.count(*d3) == 1);
    
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
    assert(pool.exprs.size() == checkpoint_start);
    
    const expr* early_atom_1 = pool.atom("early_atom_1");
    assert(pool.exprs.size() == checkpoint_start + 1);
    assert(pool.exprs.count(*early_atom_1) == 1);
    
    const expr* early_atom_2 = pool.atom("early_atom_2");
    assert(pool.exprs.size() == checkpoint_start + 2);
    assert(pool.exprs.count(*early_atom_2) == 1);
    
    const expr* early_cons = pool.cons(early_atom_1, early_atom_2);
    assert(pool.exprs.size() == checkpoint_start + 3);
    assert(pool.exprs.count(*early_cons) == 1);
    
    size_t checkpoint_a = pool.size();
    assert(checkpoint_a == checkpoint_start + 3);
    assert(pool.exprs.size() == checkpoint_a);
    
    t.push();  // Frame B
    assert(pool.exprs.size() == checkpoint_a);
    
    const expr* mid_atom = pool.atom("mid_atom");
    assert(pool.exprs.size() == checkpoint_a + 1);
    assert(pool.exprs.count(*mid_atom) == 1);
    
    const expr* mid_cons = pool.cons(early_atom_1, mid_atom);  // Uses early_atom_1
    assert(pool.exprs.size() == checkpoint_a + 2);
    assert(pool.exprs.count(*mid_cons) == 1);
    
    size_t checkpoint_b = pool.size();
    assert(checkpoint_b == checkpoint_a + 2);  // mid_atom, mid_cons
    assert(pool.exprs.size() == checkpoint_b);
    
    // Re-intern early cons in Frame B - should not log since already exists
    const expr* early_cons_again = pool.cons(early_atom_1, early_atom_2);
    assert(early_cons == early_cons_again);
    assert(pool.size() == checkpoint_b);  // Size unchanged
    assert(pool.exprs.size() == checkpoint_b);  // Set size also unchanged
    assert(pool.exprs.count(*early_cons) == 1);  // Still in set
    
    // Add more new content in Frame B
    const expr* late_atom = pool.atom("late_atom");
    assert(pool.exprs.size() == checkpoint_b + 1);
    assert(pool.exprs.count(*late_atom) == 1);
    
    const expr* late_cons = pool.cons(late_atom, mid_atom);
    assert(pool.exprs.size() == checkpoint_b + 2);
    assert(pool.exprs.count(*late_cons) == 1);
    
    size_t checkpoint_b_final = pool.size();
    assert(checkpoint_b_final == checkpoint_b + 2);
    assert(pool.exprs.size() == checkpoint_b_final);
    
    t.push();  // Frame C
    assert(pool.exprs.size() == checkpoint_b_final);
    
    // Re-intern content from both Frame A and Frame B
    const expr* early_cons_again2 = pool.cons(early_atom_1, early_atom_2);
    assert(early_cons == early_cons_again2);
    assert(pool.exprs.size() == checkpoint_b_final);  // No change
    assert(pool.exprs.count(*early_cons) == 1);
    
    const expr* mid_cons_again = pool.cons(early_atom_1, mid_atom);
    assert(mid_cons == mid_cons_again);
    assert(pool.size() == checkpoint_b_final);  // Size unchanged
    assert(pool.exprs.size() == checkpoint_b_final);  // Set size unchanged
    assert(pool.exprs.count(*mid_cons) == 1);
    
    // Add new content in Frame C
    const expr* frame_c_atom = pool.atom("frame_c_atom");
    assert(pool.exprs.size() == checkpoint_b_final + 1);
    assert(pool.exprs.count(*frame_c_atom) == 1);
    
    const expr* frame_c_cons = pool.cons(frame_c_atom, early_atom_1);
    assert(pool.exprs.size() == checkpoint_b_final + 2);
    assert(pool.exprs.count(*frame_c_cons) == 1);
    
    size_t checkpoint_c = pool.size();
    assert(checkpoint_c == checkpoint_b_final + 2);
    assert(pool.exprs.size() == checkpoint_c);
    
    // Verify all content from all frames is present
    assert(pool.exprs.count(*early_atom_1) == 1);
    assert(pool.exprs.count(*early_atom_2) == 1);
    assert(pool.exprs.count(*early_cons) == 1);
    assert(pool.exprs.count(*mid_atom) == 1);
    assert(pool.exprs.count(*mid_cons) == 1);
    assert(pool.exprs.count(*late_atom) == 1);
    assert(pool.exprs.count(*late_cons) == 1);
    assert(pool.exprs.count(*frame_c_atom) == 1);
    assert(pool.exprs.count(*frame_c_cons) == 1);
    
    // Pop Frame C - only frame_c_atom and frame_c_cons should be removed
    t.pop();
    assert(pool.size() == checkpoint_b_final);
    assert(pool.exprs.size() == checkpoint_b_final);
    
    // Verify early and mid content still exist
    const expr* verify_early_cons = pool.cons(early_atom_1, early_atom_2);
    assert(verify_early_cons == early_cons);
    assert(pool.exprs.count(*verify_early_cons) == 1);
    
    const expr* verify_mid_cons = pool.cons(early_atom_1, mid_atom);
    assert(verify_mid_cons == mid_cons);
    assert(pool.exprs.count(*verify_mid_cons) == 1);
    
    assert(pool.size() == checkpoint_b_final);  // Still unchanged
    assert(pool.exprs.size() == checkpoint_b_final);
    
    // Verify all Frame A and Frame B content is still present
    assert(pool.exprs.count(*early_atom_1) == 1);
    assert(pool.exprs.count(*early_atom_2) == 1);
    assert(pool.exprs.count(*early_cons) == 1);
    assert(pool.exprs.count(*mid_atom) == 1);
    assert(pool.exprs.count(*mid_cons) == 1);
    assert(pool.exprs.count(*late_atom) == 1);
    assert(pool.exprs.count(*late_cons) == 1);
    
    // Pop Frame B - should remove mid_atom, mid_cons, late_atom, late_cons but NOT early content
    t.pop();
    assert(pool.size() == checkpoint_a);
    assert(pool.exprs.size() == checkpoint_a);
    
    // Verify early content still exists
    const expr* verify_early_cons_after_b = pool.cons(early_atom_1, early_atom_2);
    assert(verify_early_cons_after_b == early_cons);
    assert(pool.exprs.count(*verify_early_cons_after_b) == 1);
    
    assert(pool.size() == checkpoint_a);  // Still unchanged
    assert(pool.exprs.size() == checkpoint_a);
    
    // Verify only Frame A content remains
    assert(pool.exprs.count(*early_atom_1) == 1);
    assert(pool.exprs.count(*early_atom_2) == 1);
    assert(pool.exprs.count(*early_cons) == 1);
    
    // Pop Frame A - should remove early atoms and cons
    t.pop();
    assert(pool.size() == checkpoint_start);
    assert(pool.exprs.size() == checkpoint_start);
    
    // Pop initial frame
    t.pop();
    assert(pool.size() == 0);
    assert(pool.exprs.size() == 0);
    assert(pool.exprs.empty());
}

void test_bind_map_bind() {
    // bind() is the fundamental function for managing bindings with trail support
    // It tracks all changes to the bindings map and logs rollback operations
    // Tests verify: new bindings, updates, no-op optimization, trail integration
    
    // ========== BASIC BIND OPERATIONS ==========
    
    // Test 1: Bind new entry - should insert and log erase
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr a1{expr::atom{"test"}};
        bm.bind(0, &a1);
        
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.count(0) == 1);
        assert(bm.bindings.at(0) == &a1);
        
        // Pop should erase the entry
        t.pop();
        assert(bm.bindings.size() == 0);
        assert(bm.bindings.count(0) == 0);
    }
    
    // Test 2: Bind multiple new entries
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr a1{expr::atom{"first"}};
        expr a2{expr::atom{"second"}};
        expr a3{expr::atom{"third"}};
        
        bm.bind(0, &a1);
        bm.bind(1, &a2);
        bm.bind(2, &a3);
        
        assert(bm.bindings.size() == 3);
        assert(bm.bindings.at(0) == &a1);
        assert(bm.bindings.at(1) == &a2);
        assert(bm.bindings.at(2) == &a3);
        
        // Pop should erase all entries
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 3: Update existing entry - should log old value
    {
        trail t;
        bind_map bm(t);
        
        // Frame 1: Create initial binding
        t.push();
        expr a1{expr::atom{"old"}};
        bm.bind(5, &a1);
        assert(bm.bindings.at(5) == &a1);
        assert(bm.bindings.size() == 1);
        
        // Frame 2: Update to new value
        t.push();
        expr a2{expr::atom{"new"}};
        bm.bind(5, &a2);
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.at(5) == &a2);
        
        // Pop Frame 2: should restore old value
        t.pop();
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.at(5) == &a1);  // Restored to old value
        
        // Pop Frame 1: should remove entry
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 4: No-op optimization - binding same value twice
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr a1{expr::atom{"same"}};
        
        bm.bind(10, &a1);
        assert(bm.bindings.at(10) == &a1);
        assert(bm.bindings.size() == 1);
        
        // Check undo stack size after first bind
        size_t undo_stack_size = t.undo_stack.size();
        assert(undo_stack_size == 1);  // One undo for the insert
        
        // Bind same value again - should be no-op
        bm.bind(10, &a1);
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.at(10) == &a1);
        
        // Verify undo stack did NOT grow (no-op optimization)
        assert(t.undo_stack.size() == undo_stack_size);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 5: Binding different values sequentially
    {
        trail t;
        bind_map bm(t);
        
        expr a1{expr::atom{"first"}};
        expr a2{expr::atom{"second"}};
        expr a3{expr::atom{"third"}};
        
        // Frame 1: Initial binding
        t.push();
        bm.bind(15, &a1);
        assert(bm.bindings.at(15) == &a1);
        assert(bm.bindings.size() == 1);
        
        // Frame 2: Update to second value
        t.push();
        bm.bind(15, &a2);
        assert(bm.bindings.at(15) == &a2);
        assert(bm.bindings.size() == 1);
        
        // Frame 3: Update to third value
        t.push();
        bm.bind(15, &a3);
        assert(bm.bindings.at(15) == &a3);
        assert(bm.bindings.size() == 1);
        
        // Pop Frame 3: restore to a2
        t.pop();
        assert(bm.bindings.at(15) == &a2);
        assert(bm.bindings.size() == 1);
        
        // Pop Frame 2: restore to a1
        t.pop();
        assert(bm.bindings.at(15) == &a1);
        assert(bm.bindings.size() == 1);
        
        // Pop Frame 1: remove entry
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // ========== MULTIPLE TRAIL FRAMES ==========
    
    // Test 6: Multiple frames with bindings at different levels
    {
        trail t;
        bind_map bm(t);
        
        // Frame 1
        t.push();
        expr a1{expr::atom{"frame1"}};
        bm.bind(20, &a1);
        assert(bm.bindings.size() == 1);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        // Frame 2
        t.push();
        expr a2{expr::atom{"frame2"}};
        bm.bind(21, &a2);
        assert(bm.bindings.size() == 2);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        // Frame 3
        t.push();
        expr a3{expr::atom{"frame3"}};
        bm.bind(22, &a3);
        assert(bm.bindings.size() == 3);
        
        // Pop frame 3
        t.pop();
        assert(bm.bindings == checkpoint2);
        assert(bm.bindings.size() == 2);
        
        // Pop frame 2
        t.pop();
        assert(bm.bindings == checkpoint1);
        assert(bm.bindings.size() == 1);
        
        // Pop frame 1
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 7: Nested frames with updates to same index
    {
        trail t;
        bind_map bm(t);
        
        // Frame 1: bind index 30 to a1
        t.push();
        expr a1{expr::atom{"v1"}};
        bm.bind(30, &a1);
        assert(bm.bindings.at(30) == &a1);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        // Frame 2: update index 30 to a2
        t.push();
        expr a2{expr::atom{"v2"}};
        bm.bind(30, &a2);
        assert(bm.bindings.at(30) == &a2);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        // Frame 3: update index 30 to a3
        t.push();
        expr a3{expr::atom{"v3"}};
        bm.bind(30, &a3);
        assert(bm.bindings.at(30) == &a3);
        
        // Pop frame 3: should restore to a2
        t.pop();
        assert(bm.bindings == checkpoint2);
        assert(bm.bindings.at(30) == &a2);
        
        // Pop frame 2: should restore to a1
        t.pop();
        assert(bm.bindings == checkpoint1);
        assert(bm.bindings.at(30) == &a1);
        
        // Pop frame 1: should erase entry
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 8: Multiple indices across multiple frames
    {
        trail t;
        bind_map bm(t);
        
        t.push();
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        bm.bind(40, &a1);
        bm.bind(41, &a2);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        t.push();
        expr a3{expr::atom{"c"}};
        expr a4{expr::atom{"d"}};
        bm.bind(42, &a3);
        bm.bind(43, &a4);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        t.push();
        expr a5{expr::atom{"e"}};
        bm.bind(40, &a5);  // Update existing from frame 1
        assert(bm.bindings.size() == 4);
        assert(bm.bindings.at(40) == &a5);
        
        t.pop();
        assert(bm.bindings == checkpoint2);
        assert(bm.bindings.at(40) == &a1);  // Restored
        
        t.pop();
        assert(bm.bindings == checkpoint1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // ========== COMPLEX SCENARIOS ==========
    
    // Test 9: Binding vars to atoms, cons, and other vars across frames
    {
        trail t;
        bind_map bm(t);
        
        expr a1{expr::atom{"atom"}};
        expr v1{expr::var{50}};
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr c1{expr::cons{&a2, &a3}};
        
        // Frame 1: var -> atom
        t.push();
        bm.bind(50, &a1);
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.at(50) == &a1);
        
        // Frame 2: var -> var
        t.push();
        bm.bind(51, &v1);
        assert(bm.bindings.size() == 2);
        assert(bm.bindings.at(51) == &v1);
        
        // Frame 3: var -> cons
        t.push();
        bm.bind(52, &c1);
        assert(bm.bindings.size() == 3);
        assert(bm.bindings.at(52) == &c1);
        
        // Pop frames
        t.pop();
        assert(bm.bindings.size() == 2);
        
        t.pop();
        assert(bm.bindings.size() == 1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 10: Chain of bindings built incrementally across frames
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{60}};
        expr v2{expr::var{61}};
        expr v3{expr::var{62}};
        expr a1{expr::atom{"end"}};
        
        // Frame 1: Start chain 60 -> v1
        t.push();
        bm.bind(60, &v1);
        assert(bm.bindings.size() == 1);
        
        // Frame 2: Extend 61 -> v2
        t.push();
        bm.bind(61, &v2);
        assert(bm.bindings.size() == 2);
        
        // Frame 3: Extend 62 -> v3
        t.push();
        bm.bind(62, &v3);
        assert(bm.bindings.size() == 3);
        
        // Frame 4: Terminate 63 -> a1
        t.push();
        bm.bind(63, &a1);
        assert(bm.bindings.size() == 4);
        
        // Pop frames incrementally
        t.pop();
        assert(bm.bindings.size() == 3);
        
        t.pop();
        assert(bm.bindings.size() == 2);
        
        t.pop();
        assert(bm.bindings.size() == 1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 11: Deeply nested cons with multiple var bindings across frames
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{70}};
        expr v2{expr::var{71}};
        expr a1{expr::atom{"a"}};
        
        // Build nested: cons(cons(v1, a1), v2)
        expr inner{expr::cons{&v1, &a1}};
        expr outer{expr::cons{&inner, &v2}};
        
        expr a2{expr::atom{"bound1"}};
        expr a3{expr::atom{"bound2"}};
        
        // Frame 1: Bind first var
        t.push();
        bm.bind(70, &a2);
        assert(bm.bindings.size() == 1);
        
        // Frame 2: Bind second var
        t.push();
        bm.bind(71, &a3);
        assert(bm.bindings.size() == 2);
        
        // Frame 3: Bind to nested cons structure
        t.push();
        bm.bind(72, &outer);
        assert(bm.bindings.size() == 3);
        assert(bm.bindings.at(70) == &a2);
        assert(bm.bindings.at(71) == &a3);
        assert(bm.bindings.at(72) == &outer);
        
        // Pop frames
        t.pop();
        assert(bm.bindings.size() == 2);
        
        t.pop();
        assert(bm.bindings.size() == 1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 12: Complex scenario - multiple frames with mixed operations
    {
        trail t;
        bind_map bm(t);
        
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        expr a4{expr::atom{"d"}};
        expr a5{expr::atom{"e"}};
        
        // Frame 1: Create initial bindings
        t.push();
        bm.bind(80, &a1);
        bm.bind(81, &a2);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        // Frame 2: Add new and update existing
        t.push();
        bm.bind(82, &a3);
        bm.bind(80, &a4);  // Update 80
        assert(bm.bindings.size() == 3);
        assert(bm.bindings.at(80) == &a4);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        // Frame 3: More updates
        t.push();
        bm.bind(81, &a5);  // Update 81
        bm.bind(83, &a1);  // New
        assert(bm.bindings.size() == 4);
        
        // Pop frame 3
        t.pop();
        assert(bm.bindings == checkpoint2);
        assert(bm.bindings.at(81) == &a2);  // Restored
        assert(bm.bindings.count(83) == 0);  // Erased
        
        // Pop frame 2
        t.pop();
        assert(bm.bindings == checkpoint1);
        assert(bm.bindings.at(80) == &a1);  // Restored
        assert(bm.bindings.count(82) == 0);  // Erased
        
        // Pop frame 1
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 13: Binding same value multiple times across frames (no-op optimization)
    {
        trail t;
        bind_map bm(t);
        
        expr a1{expr::atom{"same"}};
        
        // Frame 1: Initial binding
        t.push();
        bm.bind(90, &a1);
        assert(bm.bindings.size() == 1);
        size_t undo_after_frame1 = t.undo_stack.size();
        
        // Frame 2: No-op binding (same value)
        t.push();
        bm.bind(90, &a1);  // Should be no-op
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.at(90) == &a1);
        // Verify undo stack didn't grow (no-op optimization)
        assert(t.undo_stack.size() == undo_after_frame1);  // No new undo logged
        
        // Frame 3: Another no-op
        t.push();
        bm.bind(90, &a1);  // Should be no-op
        bm.bind(90, &a1);  // Should be no-op
        assert(bm.bindings.at(90) == &a1);
        // Verify undo stack didn't grow
        assert(t.undo_stack.size() == undo_after_frame1);  // Still no new undos
        
        // Pop all frames
        t.pop();
        assert(bm.bindings.size() == 1);
        
        t.pop();
        assert(bm.bindings.size() == 1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 14: Alternating between two values
    {
        trail t;
        bind_map bm(t);
        
        expr a1{expr::atom{"val1"}};
        expr a2{expr::atom{"val2"}};
        
        t.push();
        bm.bind(95, &a1);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        t.push();
        bm.bind(95, &a2);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        t.push();
        bm.bind(95, &a1);  // Back to a1
        std::map<uint32_t, const expr*> checkpoint3 = bm.bindings;
        
        t.push();
        bm.bind(95, &a2);  // Back to a2
        assert(bm.bindings.at(95) == &a2);
        
        t.pop();
        assert(bm.bindings == checkpoint3);
        assert(bm.bindings.at(95) == &a1);
        
        t.pop();
        assert(bm.bindings == checkpoint2);
        assert(bm.bindings.at(95) == &a2);
        
        t.pop();
        assert(bm.bindings == checkpoint1);
        assert(bm.bindings.at(95) == &a1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // ========== COMPLEX STRUCTURES ==========
    
    // Test 15: Building chains across multiple frames
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{100}};
        expr v2{expr::var{101}};
        expr v3{expr::var{102}};
        expr a1{expr::atom{"end"}};
        
        // Frame 1: Start of chain
        t.push();
        bm.bind(100, &v1);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        // Frame 2: Extend chain
        t.push();
        bm.bind(101, &v2);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        // Frame 3: Extend more
        t.push();
        bm.bind(102, &v3);
        std::map<uint32_t, const expr*> checkpoint3 = bm.bindings;
        
        // Frame 4: Terminate chain
        t.push();
        bm.bind(103, &a1);
        assert(bm.bindings.size() == 4);
        
        // Pop frames one by one
        t.pop();
        assert(bm.bindings == checkpoint3);
        assert(bm.bindings.size() == 3);
        
        t.pop();
        assert(bm.bindings == checkpoint2);
        assert(bm.bindings.size() == 2);
        
        t.pop();
        assert(bm.bindings == checkpoint1);
        assert(bm.bindings.size() == 1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 16: Binding to cons structures across frames
    {
        trail t;
        bind_map bm(t);
        
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"z"}};
        expr c2{expr::cons{&c1, &a3}};
        
        t.push();
        bm.bind(110, &c1);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        t.push();
        bm.bind(111, &c2);
        assert(bm.bindings.size() == 2);
        
        t.pop();
        assert(bm.bindings == checkpoint1);
        assert(bm.bindings.size() == 1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 17: Very complex - multiple indices, multiple frames, updates
    {
        trail t;
        bind_map bm(t);
        
        expr a1{expr::atom{"1"}};
        expr a2{expr::atom{"2"}};
        expr a3{expr::atom{"3"}};
        expr a4{expr::atom{"4"}};
        expr a5{expr::atom{"5"}};
        expr a6{expr::atom{"6"}};
        
        // Frame 1
        t.push();
        bm.bind(120, &a1);
        bm.bind(121, &a2);
        bm.bind(122, &a3);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        // Frame 2
        t.push();
        bm.bind(120, &a4);  // Update
        bm.bind(123, &a5);  // New
        assert(bm.bindings.size() == 4);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        // Frame 3
        t.push();
        bm.bind(121, &a6);  // Update
        bm.bind(124, &a1);  // New
        bm.bind(122, &a2);  // Update
        assert(bm.bindings.size() == 5);
        
        t.pop();
        assert(bm.bindings == checkpoint2);
        
        t.pop();
        assert(bm.bindings == checkpoint1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 18: Deeply nested cons with vars bound across frames
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{130}};
        expr v2{expr::var{131}};
        expr v3{expr::var{132}};
        expr a1{expr::atom{"a"}};
        
        // Build: cons(cons(v1, v2), cons(v3, a1))
        expr left{expr::cons{&v1, &v2}};
        expr right{expr::cons{&v3, &a1}};
        expr outer{expr::cons{&left, &right}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr a4{expr::atom{"z"}};
        
        t.push();
        bm.bind(130, &a2);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        t.push();
        bm.bind(131, &a3);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        t.push();
        bm.bind(132, &a4);
        bm.bind(133, &outer);
        assert(bm.bindings.size() == 4);
        
        t.pop();
        assert(bm.bindings == checkpoint2);
        assert(bm.bindings.size() == 2);
        
        t.pop();
        assert(bm.bindings == checkpoint1);
        assert(bm.bindings.size() == 1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 19: Stress test - many bindings, many frames, many updates
    {
        trail t;
        bind_map bm(t);
        
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        
        // Frame 1: 10 bindings
        t.push();
        for (uint32_t i = 140; i < 150; i++) {
            bm.bind(i, &a1);
        }
        assert(bm.bindings.size() == 10);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        // Frame 2: Update half, add 5 new
        t.push();
        for (uint32_t i = 140; i < 145; i++) {
            bm.bind(i, &a2);  // Update
        }
        for (uint32_t i = 150; i < 155; i++) {
            bm.bind(i, &a3);  // New
        }
        assert(bm.bindings.size() == 15);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        // Frame 3: More updates
        t.push();
        for (uint32_t i = 145; i < 150; i++) {
            bm.bind(i, &a3);  // Update
        }
        assert(bm.bindings.size() == 15);
        
        t.pop();
        assert(bm.bindings == checkpoint2);
        
        t.pop();
        assert(bm.bindings == checkpoint1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 20: No-op in nested frames with undo stack verification
    {
        trail t;
        bind_map bm(t);
        
        expr a1{expr::atom{"same"}};
        
        t.push();
        bm.bind(160, &a1);
        size_t undo_after_bind = t.undo_stack.size();
        
        t.push();
        bm.bind(160, &a1);  // No-op
        // Verify no undo was logged
        assert(t.undo_stack.size() == undo_after_bind);
        
        t.push();
        bm.bind(160, &a1);  // No-op
        // Verify no undo was logged
        assert(t.undo_stack.size() == undo_after_bind);
        
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.at(160) == &a1);
        
        t.pop();
        assert(bm.bindings.at(160) == &a1);
        
        t.pop();
        assert(bm.bindings.at(160) == &a1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
    }
}

void test_bind_map_whnf() {
    // Test 1: whnf of atom returns itself
    {
        trail t;
        bind_map bm(t);
        expr a1{expr::atom{"test"}};
        const expr* result = bm.whnf(&a1);
        assert(result == &a1);
        assert(bm.bindings.size() == 0);  // No bindings created for non-vars
    }
    
    // Test 2: whnf of cons returns itself
    {
        trail t;
        bind_map bm(t);
        expr a1{expr::atom{"left"}};
        expr a2{expr::atom{"right"}};
        expr c1{expr::cons{&a1, &a2}};
        const expr* result = bm.whnf(&c1);
        assert(result == &c1);
        assert(bm.bindings.size() == 0);
    }
    
    // Test 3: whnf of unbound var returns itself (no entry created)
    {
        trail t;
        bind_map bm(t);
        expr v1{expr::var{0}};
        const expr* result = bm.whnf(&v1);
        assert(result == &v1);
        assert(bm.bindings.size() == 0);  // No default initialization
    }
    
    // Test 4: whnf of bound var returns the binding
    {
        trail t;
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
        trail t;
        bind_map bm(t);
        t.push();  // Need frame for path compression
        
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
        
        t.pop();
    }
    
    // Test 6: whnf with long chain
    {
        trail t;
        bind_map bm(t);
        t.push();  // Need frame for path compression
        
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
        
        t.pop();
    }
    
    // Test 7: whnf with var bound to cons
    {
        trail t;
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
        trail t;
        bind_map bm(t);
        t.push();  // Need frame for path compression
        
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
        
        t.pop();
    }
    
    // Test 9: Multiple unbound vars remain unbound (no entries created)
    {
        trail t;
        bind_map bm(t);
        expr v1{expr::var{50}};
        expr v2{expr::var{51}};
        expr v3{expr::var{52}};
        
        assert(bm.whnf(&v1) == &v1);
        assert(bm.bindings.size() == 0);
        
        assert(bm.whnf(&v2) == &v2);
        assert(bm.bindings.size() == 0);
        
        assert(bm.whnf(&v3) == &v3);
        assert(bm.bindings.size() == 0);
    }
    
    // Test 10: whnf called multiple times on same var with binding
    {
        trail t;
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
        trail t;
        bind_map bm(t);
        t.push();  // Need frame for path compression
        
        expr v1{expr::var{70}};
        expr v2{expr::var{71}};
        expr v3{expr::var{72}};
        
        // Chain: v1 -> v2 -> v3 (v3 is unbound - no entry for it)
        bm.bindings[70] = &v2;
        bm.bindings[71] = &v3;
        assert(bm.bindings.size() == 2);
        
        const expr* result = bm.whnf(&v1);
        assert(result == &v3);  // Should return v3, the unbound var
        
        // Verify path compression to v3
        assert(bm.bindings.at(70) == &v3);
        assert(bm.bindings.at(71) == &v3);
        assert(bm.bindings.size() == 2);
        
        t.pop();
    }
    
    // Test 12: whnf with nested cons structures
    {
        trail t;
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
        trail t;
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
        trail t;
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
        trail t;
        bind_map bm(t);
        t.push();  // Need frame for path compression
        
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
        
        t.pop();
    }
    
    // Test 16: WHNF does NOT reduce vars inside cons (WEAK head, not strong)
    {
        trail t;
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
        trail t;
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
        trail t;
        bind_map bm(t);
        t.push();  // Need frame for path compression
        
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
        
        t.pop();
    }
    
    // Test 19: Deeply nested cons with vars at all levels
    {
        trail t;
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
        trail t;
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
        trail t;
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
        trail t;
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
        trail t;
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
        trail t;
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
        trail t;
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
        trail t;
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
        trail t;
        bind_map bm(t);
        t.push();  // Need frame for path compression
        
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
        
        t.pop();
    }
    
    // Test 28: Deeply nested cons with vars everywhere
    {
        trail t;
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
        trail t;
        bind_map bm(t);
        t.push();  // Need frame for path compression
        
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
        
        t.pop();
    }
    
    // Test 30: Empty bindings map remains empty for unbound vars
    {
        trail t;
        bind_map bm(t);
        assert(bm.bindings.size() == 0);
        
        expr v1{expr::var{240}};
        bm.whnf(&v1);
        assert(bm.bindings.size() == 0);
        
        expr v2{expr::var{241}};
        bm.whnf(&v2);
        assert(bm.bindings.size() == 0);
    }
    
    // Test 31: Same unbound var called multiple times creates no entries
    {
        trail t;
        bind_map bm(t);
        expr v1{expr::var{250}};
        
        bm.whnf(&v1);
        assert(bm.bindings.size() == 0);
        
        bm.whnf(&v1);
        assert(bm.bindings.size() == 0);
        
        bm.whnf(&v1);
        assert(bm.bindings.size() == 0);
    }
    
    // ========== TRAIL INTEGRATION TESTS ==========
    
    // Test 32: whnf with trail - bind in frame, pop frame, verify rollback
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{300}};
        expr a1{expr::atom{"frame1"}};
        
        t.push();
        bm.bind(300, &a1);
        assert(bm.bindings.size() == 1);
        
        const expr* result = bm.whnf(&v1);
        assert(result == &a1);
        assert(bm.bindings.size() == 1);
        
        t.pop();
        assert(bm.bindings.size() == 0);
        
        // After pop, v1 is unbound again
        result = bm.whnf(&v1);
        assert(result == &v1);
        assert(bm.bindings.size() == 0);
    }
    
    // Test 33: whnf with nested frames - multiple bindings
    {
        trail t;
        bind_map bm(t);

        expr v1{expr::var{310}};
        expr v2{expr::var{311}};
        expr a1{expr::atom{"outer"}};
        expr a2{expr::atom{"inner"}};
        
        // Frame 1: bind v1 to a1
        t.push();
        bm.bind(310, &a1);
        assert(bm.whnf(&v1) == &a1);
        assert(bm.bindings.size() == 1);
        
        // Frame 2: bind v2 to a2
        t.push();
        bm.bind(311, &a2);
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a2);
        assert(bm.bindings.size() == 2);
        
        // Pop frame 2
        t.pop();
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &v2);  // v2 unbound again
        assert(bm.bindings.size() == 1);
        
        // Pop frame 1
        t.pop();
        assert(bm.whnf(&v1) == &v1);  // v1 unbound again
        assert(bm.bindings.size() == 0);
    }
    
    // Test 34: whnf with path compression across frames
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{320}};
        expr v2{expr::var{321}};
        expr v3{expr::var{322}};
        expr a1{expr::atom{"end"}};
        
        // Frame 1: v1 -> v2
        t.push();
        bm.bind(320, &v2);
        assert(bm.bindings.size() == 1);
        
        // Frame 2: v2 -> v3
        t.push();
        bm.bind(321, &v3);
        assert(bm.bindings.size() == 2);
        
        // Frame 3: v3 -> a1
        t.push();
        bm.bind(322, &a1);
        assert(bm.bindings.size() == 3);
        
        // whnf should follow chain and compress
        const expr* result = bm.whnf(&v1);
        assert(result == &a1);
        assert(bm.bindings.size() == 3);  // Path compression updates values, not count
        
        // After compression, v1 should point directly to a1
        assert(bm.bindings.at(320) == &a1);
        
        // Pop frame 3
        t.pop();
        assert(bm.bindings.size() == 2);
        // v1 should be restored to point to v2
        assert(bm.bindings.at(320) == &v2);
        
        // Pop frame 2
        t.pop();
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.at(320) == &v2);  // Still points to v2
        
        // Pop frame 1
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 35: whnf with trail - update binding in nested frame
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{330}};
        expr a1{expr::atom{"first"}};
        expr a2{expr::atom{"second"}};
        
        // Frame 1: bind v1 to a1
        t.push();
        bm.bind(330, &a1);
        assert(bm.whnf(&v1) == &a1);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        // Frame 2: update v1 to a2
        t.push();
        bm.bind(330, &a2);
        assert(bm.whnf(&v1) == &a2);
        assert(bm.bindings.size() == 1);
        
        // Pop frame 2 - should restore to a1
        t.pop();
        assert(bm.bindings == checkpoint1);
        assert(bm.whnf(&v1) == &a1);
        
        // Pop frame 1 - should be unbound
        t.pop();
        assert(bm.bindings.size() == 0);
        assert(bm.whnf(&v1) == &v1);
    }
    
    // Test 36: Complex - chain built across frames, then popped
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{340}};
        expr v2{expr::var{341}};
        expr v3{expr::var{342}};
        expr v4{expr::var{343}};
        expr a1{expr::atom{"final"}};
        
        // Frame 1: v1 -> v2
        t.push();
        bm.bind(340, &v2);
        
        // Frame 2: v2 -> v3
        t.push();
        bm.bind(341, &v3);
        
        // Frame 3: v3 -> v4
        t.push();
        bm.bind(342, &v4);
        
        // Frame 4: v4 -> a1
        t.push();
        bm.bind(343, &a1);
        
        // Full chain: v1 -> v2 -> v3 -> v4 -> a1
        assert(bm.whnf(&v1) == &a1);
        assert(bm.bindings.size() == 4);
        
        // Pop frame 4: v4 becomes unbound
        t.pop();
        assert(bm.bindings.size() == 3);
        assert(bm.whnf(&v1) == &v4);  // Chain ends at v4 now
        
        // Pop frame 3: v3 becomes unbound
        t.pop();
        assert(bm.bindings.size() == 2);
        assert(bm.whnf(&v1) == &v3);  // Chain ends at v3 now
        
        // Pop frame 2: v2 becomes unbound
        t.pop();
        assert(bm.bindings.size() == 1);
        assert(bm.whnf(&v1) == &v2);  // Chain ends at v2 now
        
        // Pop frame 1: v1 becomes unbound
        t.pop();
        assert(bm.bindings.size() == 0);
        assert(bm.whnf(&v1) == &v1);  // v1 is unbound
    }
    
    // Test 37: whnf with cons containing vars bound in frames
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{350}};
        expr v2{expr::var{351}};
        expr a1{expr::atom{"left"}};
        expr a2{expr::atom{"right"}};
        expr c1{expr::cons{&v1, &v2}};
        
        // Frame 1: bind v1 to a1
        t.push();
        bm.bind(350, &a1);
        
        // whnf of cons returns cons itself (weak head normal form)
        const expr* result = bm.whnf(&c1);
        assert(result == &c1);
        
        // But whnf of v1 returns a1
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &v2);  // v2 still unbound
        
        // Frame 2: bind v2 to a2
        t.push();
        bm.bind(351, &a2);
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a2);
        assert(bm.whnf(&c1) == &c1);  // cons still returns itself
        
        // Pop frame 2
        t.pop();
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &v2);
        assert(bm.bindings.size() == 1);
        
        // Pop frame 1
        t.pop();
        assert(bm.whnf(&v1) == &v1);
        assert(bm.bindings.size() == 0);
    }
    
    // Test 38: Multiple independent chains across frames
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{360}};
        expr v2{expr::var{361}};
        expr v3{expr::var{362}};
        expr v4{expr::var{363}};
        expr a1{expr::atom{"chain1"}};
        expr a2{expr::atom{"chain2"}};
        
        // Frame 1: Start two chains
        t.push();
        bm.bind(360, &v2);  // Chain 1: v1 -> v2
        bm.bind(362, &v4);  // Chain 2: v3 -> v4
        assert(bm.bindings.size() == 2);
        
        // Frame 2: Complete both chains
        t.push();
        bm.bind(361, &a1);  // v2 -> a1
        bm.bind(363, &a2);  // v4 -> a2
        assert(bm.bindings.size() == 4);
        
        // Both chains resolve
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v3) == &a2);
        
        // Pop frame 2: chains end at v2 and v4
        t.pop();
        assert(bm.bindings.size() == 2);
        assert(bm.whnf(&v1) == &v2);
        assert(bm.whnf(&v3) == &v4);
        
        // Pop frame 1: all unbound
        t.pop();
        assert(bm.bindings.size() == 0);
        assert(bm.whnf(&v1) == &v1);
        assert(bm.whnf(&v3) == &v3);
    }
    
    // Test 39: Very complex - multiple frames with overlapping updates and chains
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{370}};
        expr v2{expr::var{371}};
        expr v3{expr::var{372}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        
        // Frame 1: v1 -> a1
        t.push();
        bm.bind(370, &a1);
        assert(bm.whnf(&v1) == &a1);
        std::map<uint32_t, const expr*> checkpoint1 = bm.bindings;
        
        // Frame 2: v1 -> a2 (update v1's binding)
        t.push();
        bm.bind(370, &a2);
        assert(bm.whnf(&v1) == &a2);
        assert(bm.bindings.size() == 1);
        std::map<uint32_t, const expr*> checkpoint2 = bm.bindings;
        
        // Frame 3: v1 -> a3 (update again), v2 -> a1
        t.push();
        bm.bind(370, &a3);
        bm.bind(371, &a1);
        assert(bm.whnf(&v1) == &a3);
        assert(bm.whnf(&v2) == &a1);
        assert(bm.bindings.size() == 2);
        
        // Pop frame 3: back to v1 -> a2
        t.pop();
        assert(bm.bindings == checkpoint2);
        assert(bm.whnf(&v1) == &a2);
        assert(bm.bindings.size() == 1);
        
        // Pop frame 2: back to v1 -> a1
        t.pop();
        assert(bm.bindings == checkpoint1);
        assert(bm.whnf(&v1) == &a1);
        assert(bm.bindings.size() == 1);
        
        // Pop frame 1: all unbound
        t.pop();
        assert(bm.bindings.size() == 0);
        assert(bm.whnf(&v1) == &v1);
    }
    
    // Test 40: Path compression with trail - verify compressed paths are restored
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{380}};
        expr v2{expr::var{381}};
        expr v3{expr::var{382}};
        expr a1{expr::atom{"target"}};
        
        // Build chain in single frame: v1 -> v2 -> v3 -> a1
        t.push();
        bm.bind(380, &v2);
        bm.bind(381, &v3);
        bm.bind(382, &a1);
        assert(bm.bindings.size() == 3);
        
        // Before whnf, v1 points to v2
        assert(bm.bindings.at(380) == &v2);
        
        // whnf performs path compression
        assert(bm.whnf(&v1) == &a1);
        
        // After whnf, v1 should point directly to a1 (path compression)
        assert(bm.bindings.at(380) == &a1);
        assert(bm.bindings.size() == 3);  // Count unchanged
        
        // Pop frame - should restore original v1 -> v2 binding
        t.pop();
        assert(bm.bindings.size() == 0);
    }
    
    // Test 41: Deeply nested frames with cons structures
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{390}};
        expr v2{expr::var{391}};
        expr v3{expr::var{392}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&v2, &a1}};
        expr c2{expr::cons{&v3, &a2}};
        
        // Frame 1: v1 -> c1
        t.push();
        bm.bind(390, &c1);
        assert(bm.whnf(&v1) == &c1);
        
        // Frame 2: v2 -> a1 (bind var inside cons)
        t.push();
        bm.bind(391, &a1);
        assert(bm.whnf(&v1) == &c1);  // Still returns c1 (WHNF)
        assert(bm.whnf(&v2) == &a1);  // But v2 resolves to a1
        
        // Frame 3: v1 -> c2 (rebind v1)
        t.push();
        bm.bind(390, &c2);
        assert(bm.whnf(&v1) == &c2);
        assert(bm.whnf(&v3) == &v3);  // v3 unbound
        
        // Frame 4: v3 -> a2
        t.push();
        bm.bind(392, &a2);
        assert(bm.whnf(&v1) == &c2);
        assert(bm.whnf(&v3) == &a2);
        
        // Pop frame 4
        t.pop();
        assert(bm.whnf(&v1) == &c2);
        assert(bm.whnf(&v3) == &v3);
        
        // Pop frame 3
        t.pop();
        assert(bm.whnf(&v1) == &c1);
        assert(bm.whnf(&v2) == &a1);
        
        // Pop frame 2
        t.pop();
        assert(bm.whnf(&v1) == &c1);
        assert(bm.whnf(&v2) == &v2);
        
        // Pop frame 1
        t.pop();
        assert(bm.bindings.size() == 0);
        assert(bm.whnf(&v1) == &v1);
    }
    
    // Test 42: Stress test - path compression with trail rollback
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{400}};
        expr v2{expr::var{401}};
        expr v3{expr::var{402}};
        expr v4{expr::var{403}};
        expr v5{expr::var{404}};
        expr a1{expr::atom{"1"}};
        expr a2{expr::atom{"2"}};
        expr a3{expr::atom{"3"}};
        
        // Frame 1: Build chain v1 -> v2 -> a1
        t.push();
        bm.bind(400, &v2);  // v1 -> v2
        bm.bind(401, &a1);  // v2 -> a1
        
        // Before whnf: bindings[400] = &v2, bindings[401] = &a1
        assert(bm.bindings.at(400) == &v2);
        assert(bm.bindings.at(401) == &a1);
        
        // Call whnf to trigger path compression
        assert(bm.whnf(&v1) == &a1);
        
        // After whnf with path compression: bindings[400] should be compressed to &a1
        // The compression happens via bind(400, &a1) which logs to Frame 1
        assert(bm.bindings.at(400) == &a1);  // Path compressed!
        assert(bm.bindings.size() == 2);
        
        // Frame 2: Build another chain v3 -> v4 -> a2
        t.push();
        bm.bind(402, &v4);  // v3 -> v4
        bm.bind(403, &a2);  // v4 -> a2
        
        // v1 still resolves to a1 (already compressed)
        assert(bm.whnf(&v1) == &a1);
        
        // Trigger path compression for v3
        assert(bm.whnf(&v3) == &a2);
        assert(bm.bindings.at(402) == &a2);  // Path compressed!
        assert(bm.bindings.size() == 4);
        
        // Frame 3: Update v2's binding (but v1 is already compressed, so won't affect v1)
        t.push();
        bm.bind(401, &v5);  // Update: v2 -> v5 (was v2 -> a1)
        bm.bind(404, &a3);  // v5 -> a3
        
        // v1 still points to a1 because it was compressed in Frame 1
        // bindings[400] = &a1 directly, doesn't go through v2 anymore
        assert(bm.whnf(&v1) == &a1);
        
        // Check v2's chain before calling whnf (before compression)
        assert(bm.bindings.at(401) == &v5);
        assert(bm.bindings.at(404) == &a3);
        
        // v2 resolves to a3, and this triggers path compression
        assert(bm.whnf(&v2) == &a3);
        
        // After whnf, v2 is compressed to point directly to a3
        assert(bm.bindings.at(401) == &a3);  // Path compressed in Frame 3!
        assert(bm.bindings.size() == 5);
        
        // Frame 4: Update v5 to point to v3
        t.push();
        bm.bind(404, &v3);  // Update: v5 -> v3 (was v5 -> a3)
        
        // v2 still points to a3 because it was compressed in Frame 3
        // bindings[401] = &a3 directly, doesn't go through v5 anymore
        assert(bm.whnf(&v2) == &a3);
        
        // But v5 now resolves to a2 (v5 -> v3 -> a2)
        assert(bm.whnf(&v5) == &a2);
        assert(bm.bindings.at(404) == &a2);  // Path compressed in Frame 4!
        assert(bm.bindings.size() == 5);
        
        // Pop Frame 4: restore v5 -> a3, undo v5's compression
        t.pop();
        assert(bm.bindings.at(404) == &a3);  // Restored
        assert(bm.bindings.at(401) == &a3);  // v2 still compressed (from Frame 3)
        assert(bm.whnf(&v2) == &a3);
        assert(bm.whnf(&v5) == &a3);
        assert(bm.bindings.size() == 5);
        
        // Pop Frame 3: restore v2 -> a1, remove v5 binding, undo v2's compression
        t.pop();
        assert(bm.bindings.at(401) == &a1);  // Restored to original Frame 1 value
        assert(bm.bindings.count(404) == 0);  // v5 binding removed
        
        // v1 is still compressed from Frame 1
        assert(bm.bindings.at(400) == &a1);
        assert(bm.whnf(&v1) == &a1);
        
        // v2 now points directly to a1 again
        assert(bm.whnf(&v2) == &a1);
        
        // v3 is still compressed from Frame 2
        assert(bm.bindings.at(402) == &a2);
        assert(bm.whnf(&v3) == &a2);
        
        assert(bm.bindings.size() == 4);
        
        // Pop Frame 2: remove v3, v4 bindings, undo v3's compression
        t.pop();
        assert(bm.bindings.count(402) == 0);  // v3 binding removed
        assert(bm.bindings.count(403) == 0);  // v4 binding removed
        
        // v1 is STILL compressed from Frame 1 (compression persists across frame pops)
        assert(bm.bindings.at(400) == &a1);
        assert(bm.whnf(&v1) == &a1);
        
        // v2 still points to a1 from Frame 1
        assert(bm.bindings.at(401) == &a1);
        assert(bm.whnf(&v2) == &a1);
        
        // v3 is now unbound
        assert(bm.whnf(&v3) == &v3);
        
        assert(bm.bindings.size() == 2);
        
        // Pop Frame 1: undo v1's compression, remove all bindings
        t.pop();
        assert(bm.bindings.size() == 0);
        
        // All vars are now unbound
        assert(bm.whnf(&v1) == &v1);
        assert(bm.whnf(&v2) == &v2);
        assert(bm.whnf(&v3) == &v3);
        assert(bm.whnf(&v4) == &v4);
        assert(bm.whnf(&v5) == &v5);
    }
    
    // Test 43: Path compression with overlapping chains and multiple compressions per frame
    {
        trail t;
        bind_map bm(t);
        
        expr v1{expr::var{500}};
        expr v2{expr::var{501}};
        expr v3{expr::var{502}};
        expr v4{expr::var{503}};
        expr v5{expr::var{504}};
        expr v6{expr::var{505}};
        expr a1{expr::atom{"end"}};
        
        // Frame 1: Create a long chain v1 -> v2 -> v3 -> v4 -> a1
        t.push();
        bm.bind(500, &v2);
        bm.bind(501, &v3);
        bm.bind(502, &v4);
        bm.bind(503, &a1);
        
        // Compress the entire chain
        assert(bm.whnf(&v1) == &a1);
        
        // All intermediate nodes should be compressed to a1
        assert(bm.bindings.at(500) == &a1);  // v1 compressed
        assert(bm.bindings.at(501) == &a1);  // v2 compressed
        assert(bm.bindings.at(502) == &a1);  // v3 compressed
        assert(bm.bindings.at(503) == &a1);  // v4 already pointed to a1
        
        // Frame 2: Create a branch - update v3 to point to v5 -> v6 -> a1
        t.push();
        bm.bind(502, &v5);  // Update: v3 -> v5 (was v3 -> a1 from compression)
        bm.bind(504, &v6);  // v5 -> v6
        bm.bind(505, &a1);  // v6 -> a1
        
        // v1 and v2 still point to a1 (compressed in Frame 1)
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);
        
        // v3 now goes through the new branch
        assert(bm.whnf(&v3) == &a1);
        
        // After whnf(&v3), path compression should update v3, v5, v6
        assert(bm.bindings.at(502) == &a1);  // v3 compressed in Frame 2
        assert(bm.bindings.at(504) == &a1);  // v5 compressed in Frame 2
        assert(bm.bindings.at(505) == &a1);  // v6 compressed in Frame 2
        
        // Frame 3: Update v2 to point to v3 (creating a new connection)
        t.push();
        bm.bind(501, &v3);  // Update: v2 -> v3 (was v2 -> a1 from Frame 1)
        
        // v2 now goes through v3, which is compressed to a1
        assert(bm.whnf(&v2) == &a1);
        
        // v2 should be compressed again in Frame 3
        assert(bm.bindings.at(501) == &a1);  // v2 re-compressed in Frame 3
        
        // Pop Frame 3: restore v2 -> a1 from Frame 1
        t.pop();
        assert(bm.bindings.at(501) == &a1);  // Back to Frame 1's compression
        assert(bm.whnf(&v2) == &a1);
        
        // Pop Frame 2: restore v3 -> a1 from Frame 1, remove v5, v6
        t.pop();
        assert(bm.bindings.at(502) == &a1);  // Back to Frame 1's compression
        assert(bm.bindings.count(504) == 0);  // v5 removed
        assert(bm.bindings.count(505) == 0);  // v6 removed
        assert(bm.whnf(&v3) == &a1);
        
        // Pop Frame 1: remove all bindings, undo all compressions
        t.pop();
        assert(bm.bindings.size() == 0);
        assert(bm.whnf(&v1) == &v1);
        assert(bm.whnf(&v2) == &v2);
        assert(bm.whnf(&v3) == &v3);
    }
}

void test_bind_map_occurs_check() {
    // occurs_check determines if a variable (by index) occurs anywhere in an expression,
    // even through indirection via bindings. It uses whnf to follow chains.
    // Tests cover: atoms, unbound vars, bound vars, chains, cons structures, nested cons,
    // and various combinations. Circular bindings (undefined behavior) are not tested.
    
    // Test 1: occurs_check on atom - should return false
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr a1{expr::atom{"test"}};
        assert(!bm.occurs_check(0, &a1));
        assert(!bm.occurs_check(100, &a1));
        assert(bm.bindings.size() == 0);
        
        t.pop();
    }
    
    // Test 2: occurs_check on unbound var with same index - should return true
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{5}};
        assert(bm.occurs_check(5, &v1));
        assert(bm.bindings.size() == 0);  // whnf no longer creates entries
        
        t.pop();
    }
    
    // Test 3: occurs_check on unbound var with different index - should return false
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{10}};
        assert(!bm.occurs_check(5, &v1));
        assert(!bm.occurs_check(11, &v1));
        assert(bm.bindings.size() == 0);  // No entries created
        
        t.pop();
    }
    
    // Test 4: occurs_check on var bound to atom - should return false
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{15}};
        expr a1{expr::atom{"bound"}};
        bm.bindings[15] = &a1;
        
        assert(!bm.occurs_check(15, &v1));  // v1 reduces to atom
        assert(!bm.occurs_check(20, &v1));
        assert(bm.bindings.size() == 1);
        
        t.pop();
    }
    
    // Test 5: occurs_check on var bound to same var - creates infinite loop, SKIP
    // This is undefined behavior - circular binding
    
    // Test 6: occurs_check through chain ending in unbound var
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{30}};
        expr v2{expr::var{31}};
        expr v3{expr::var{32}};
        
        // Chain: v1 -> v2 -> v3 (v3 is unbound, don't set it explicitly)
        bm.bindings[30] = &v2;
        bm.bindings[31] = &v3;
        // Don't set bindings[32] - let whnf handle it
        
        // v1 eventually points to v3 (var 32)
        assert(bm.occurs_check(32, &v1));
        assert(bm.bindings.size() == 2);  // Only the 2 explicit bindings
        
        t.pop();
    }
    
    // Test 7: occurs_check through chain ending in atom
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        
        t.pop();
    }
    
    // Test 8: occurs_check on cons with no vars
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr a1{expr::atom{"left"}};
        expr a2{expr::atom{"right"}};
        expr c1{expr::cons{&a1, &a2}};
        
        assert(!bm.occurs_check(0, &c1));
        assert(!bm.occurs_check(50, &c1));
        assert(bm.bindings.size() == 0);
        
        t.pop();
    }
    
    // Test 9: occurs_check on cons with matching var in lhs
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{55}};
        expr a1{expr::atom{"right"}};
        expr c1{expr::cons{&v1, &a1}};
        
        assert(bm.occurs_check(55, &c1));
        assert(!bm.occurs_check(56, &c1));
        assert(bm.bindings.size() == 0);  // No entries created
        
        t.pop();
    }
    
    // Test 10: occurs_check on cons with matching var in rhs
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr a1{expr::atom{"left"}};
        expr v1{expr::var{60}};
        expr c1{expr::cons{&a1, &v1}};
        
        assert(bm.occurs_check(60, &c1));
        assert(!bm.occurs_check(61, &c1));
        assert(bm.bindings.size() == 0);
        
        t.pop();
    }
    
    // Test 11: occurs_check on cons with matching var in both children
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{65}};
        expr v2{expr::var{65}};  // Same index
        expr c1{expr::cons{&v1, &v2}};
        
        assert(bm.occurs_check(65, &c1));
        assert(bm.bindings.size() == 0);
        
        t.pop();
    }
    
    // Test 12: occurs_check on cons with different vars
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{70}};
        expr v2{expr::var{71}};
        expr c1{expr::cons{&v1, &v2}};
        
        assert(bm.occurs_check(70, &c1));
        assert(bm.occurs_check(71, &c1));
        assert(!bm.occurs_check(72, &c1));
        assert(bm.bindings.size() == 0);
        
        t.pop();
    }
    
    // Test 13: occurs_check on nested cons
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{75}};
        expr a1{expr::atom{"inner"}};
        expr inner_cons{expr::cons{&v1, &a1}};
        expr a2{expr::atom{"outer"}};
        expr outer_cons{expr::cons{&inner_cons, &a2}};
        
        assert(bm.occurs_check(75, &outer_cons));  // v1 is in nested cons
        assert(!bm.occurs_check(76, &outer_cons));
        assert(bm.bindings.size() == 0);
        
        t.pop();
    }
    
    // Test 14: occurs_check on deeply nested cons
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 0);
        
        t.pop();
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
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{90}};
        expr v2{expr::var{91}};
        expr a1{expr::atom{"test"}};
        expr c1{expr::cons{&v2, &a1}};
        
        bm.bindings[90] = &c1;
        
        assert(!bm.occurs_check(90, &v1));  // v1 -> c1, but c1 doesn't contain v1
        assert(bm.occurs_check(91, &v1));   // v1 -> c1 which contains v2
        assert(bm.bindings.size() == 1);  // Only the explicit binding
        
        t.pop();
    }
    
    // Test 17: occurs_check through chain to cons
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 2);  // Only the 2 explicit bindings
        
        t.pop();
    }
    
    // Test 18: occurs_check on cons with bound vars
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{100}};
        expr v2{expr::var{101}};
        expr a1{expr::atom{"bound"}};
        
        // Bind v2 to atom
        bm.bindings[101] = &a1;
        
        expr c1{expr::cons{&v1, &v2}};
        
        assert(bm.occurs_check(100, &c1));  // v1 is in cons
        assert(!bm.occurs_check(101, &c1)); // v2 reduces to atom
        assert(bm.bindings.size() == 1);  // Only v2's binding
        
        t.pop();
    }
    
    // Test 19: occurs_check with multiple levels of indirection
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 3);  // Only the 3 explicit bindings
        
        t.pop();
    }
    
    // Test 20: occurs_check on cons where both children are bound vars
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 2);  // The 2 explicit bindings
        
        t.pop();
    }
    
    // Test 21: occurs_check on cons with chain in lhs
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 2);  // Only the 2 explicit bindings
        
        t.pop();
    }
    
    // Test 22: occurs_check with cons of cons, target var in nested structure
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{120}};
        expr v2{expr::var{121}};
        expr a1{expr::atom{"a"}};
        
        expr inner{expr::cons{&v1, &a1}};
        expr outer{expr::cons{&inner, &v2}};
        
        assert(bm.occurs_check(120, &outer));
        assert(bm.occurs_check(121, &outer));
        assert(!bm.occurs_check(122, &outer));
        assert(bm.bindings.size() == 0);  // No explicit bindings
        
        t.pop();
    }
    
    // Test 23: occurs_check on var bound to deeply nested structure
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 1);  // Only v_outer's binding
        
        t.pop();
    }
    
    // Test 24: occurs_check with symmetric cons (same var on both sides)
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{130}};
        expr v2{expr::var{130}};  // Same var
        expr c1{expr::cons{&v1, &v2}};
        
        assert(bm.occurs_check(130, &c1));
        assert(bm.bindings.size() == 0);  // No explicit bindings
        
        t.pop();
    }
    
    // Test 25: occurs_check on empty bindings map
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{135}};
        
        assert(bm.bindings.size() == 0);
        assert(bm.occurs_check(135, &v1));
        assert(bm.bindings.size() == 0);  // whnf no longer creates entries
        
        t.pop();
    }
    
    // Test 26: occurs_check with var bound through multiple cons layers
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{140}};
        expr v2{expr::var{141}};
        expr a1{expr::atom{"base"}};
        
        // Build: cons(cons(a1, v2), a1) where v2 is unbound
        expr inner{expr::cons{&a1, &v2}};
        expr outer{expr::cons{&inner, &a1}};
        
        bm.bindings[140] = &outer;
        
        assert(!bm.occurs_check(140, &v1));  // v1 -> outer, doesn't contain itself
        assert(bm.occurs_check(141, &v1));   // outer contains v2
        assert(bm.bindings.size() == 1);  // Only v1's binding
        
        t.pop();
    }
    
    // Test 27: occurs_check with all atoms (no vars anywhere)
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        
        expr c1{expr::cons{&a1, &a2}};
        expr c2{expr::cons{&c1, &a3}};
        
        assert(!bm.occurs_check(0, &c2));
        assert(!bm.occurs_check(999, &c2));
        assert(bm.bindings.size() == 0);
        
        t.pop();
    }
    
    // Test 28: occurs_check with var bound to var bound to cons containing target
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 2);  // Only the 2 explicit bindings
        
        t.pop();
    }
    
    // Test 29: Very long chain (10 levels) ending in unbound var
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 9);  // Only the 9 explicit bindings
        
        t.pop();
    }
    
    // Test 30: Very long chain ending in atom
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        
        t.pop();
    }
    
    // Test 31: Deeply nested cons (5 levels) with var at bottom
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 0);  // No explicit bindings
        
        t.pop();
    }
    
    // Test 32: Deeply nested cons with var at different positions
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 0);  // No explicit bindings
        
        t.pop();
    }
    
    // Test 33: Var bound to deeply nested cons containing bound vars
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 3);  // Only the 3 explicit bindings
        
        t.pop();
    }
    
    // Test 34: Long chain to deeply nested cons with target var deep inside
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 3);  // Only the 3 explicit bindings
        
        t.pop();
    }
    
    // Test 35: Cons with chains in both children
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 4);  // Only the 4 explicit bindings
        
        t.pop();
    }
    
    // Test 36: Very complex nested structure with multiple vars at different depths
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 0);  // No explicit bindings
        
        t.pop();
    }
    
    // Test 37: Chain to cons, where cons children are also chains
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 6);  // All 6 explicit bindings
        
        t.pop();
    }
    
    // Test 38: Cons with one child being a long chain ending in target var
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 4);  // Only the 4 explicit bindings
        
        t.pop();
    }
    
    // Test 39: Multiple nested cons with same var appearing in different positions
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{290}};
        expr v2{expr::var{290}};  // Same index as v1
        expr v3{expr::var{290}};  // Same index as v1
        expr a1{expr::atom{"x"}};
        
        // Build: cons(cons(v1, a1), cons(v2, v3))
        expr left{expr::cons{&v1, &a1}};
        expr right{expr::cons{&v2, &v3}};
        expr outer{expr::cons{&left, &right}};
        
        assert(bm.occurs_check(290, &outer));  // Var 290 appears 3 times
        assert(bm.bindings.size() == 0);  // No explicit bindings
        
        t.pop();
    }
    
    // Test 40: Stress test - very deep nesting (10 levels) with var at bottom
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 0);  // No explicit bindings
        
        t.pop();
    }
    
    // Test 41: ULTIMATE STRESS TEST - Long chain to deeply nested cons with chains inside
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 9);  // Only the 9 explicit bindings
        
        t.pop();
    }
    
    // Test 42: Nested cons where EVERY node has a chain
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.occurs_check(503, &outer));  // vb2
        assert(bm.occurs_check(505, &outer));  // vc2
        assert(bm.occurs_check(507, &outer));  // vd2
        
        // None of the chain heads should be found (after compression)
        assert(!bm.occurs_check(500, &outer));
        assert(!bm.occurs_check(502, &outer));
        assert(!bm.occurs_check(504, &outer));
        assert(!bm.occurs_check(506, &outer));
        
        // Path compression updates values but doesn't change count
        assert(bm.bindings.size() == 4);  // Still 4 entries
        
        t.pop();
    }
    
    // Test 43: Chain to nested cons, where nested cons contains chains to more nested cons
    {
        trail t;
        bind_map bm(t);
        t.push();
        
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
        assert(bm.bindings.size() == 4);  // Path compression doesn't change count
        
        t.pop();
    }
    
    // Test 44: Cons where both children are bound to the SAME atom
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{700}};
        expr v2{expr::var{701}};
        expr a1{expr::atom{"shared"}};
        
        // Both vars bound to the same atom
        bm.bindings[700] = &a1;
        bm.bindings[701] = &a1;
        
        expr c1{expr::cons{&v1, &v2}};
        
        // Both children reduce to the same atom, so no vars should be found
        assert(!bm.occurs_check(700, &c1));  // v1 reduces to atom
        assert(!bm.occurs_check(701, &c1));  // v2 reduces to atom
        assert(!bm.occurs_check(702, &c1));  // Unrelated var
        assert(bm.bindings.size() == 2);
        
        t.pop();
    }
    
    // Test 45: Cons where both children are bound to the SAME var
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{710}};
        expr v2{expr::var{711}};
        expr v_target{expr::var{712}};
        
        // Both vars bound to the same target var (unbound)
        bm.bindings[710] = &v_target;
        bm.bindings[711] = &v_target;
        
        expr c1{expr::cons{&v1, &v2}};
        
        // Both children reduce to v_target, so searching for v_target should find it
        assert(bm.occurs_check(712, &c1));   // v_target found in both children
        assert(!bm.occurs_check(710, &c1));  // v1 reduces to v_target
        assert(!bm.occurs_check(711, &c1));  // v2 reduces to v_target
        assert(bm.bindings.size() == 2);
        
        t.pop();
    }
    
    // Test 46: Cons where both children are bound to the SAME cons
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v1{expr::var{720}};
        expr v2{expr::var{721}};
        expr v_inner{expr::var{722}};
        expr a1{expr::atom{"x"}};
        expr shared_cons{expr::cons{&v_inner, &a1}};
        
        // Both vars bound to the same cons
        bm.bindings[720] = &shared_cons;
        bm.bindings[721] = &shared_cons;
        
        expr c1{expr::cons{&v1, &v2}};
        
        // Both children reduce to shared_cons which contains v_inner
        assert(bm.occurs_check(722, &c1));   // v_inner found in shared_cons
        assert(!bm.occurs_check(720, &c1));  // v1 reduces to cons
        assert(!bm.occurs_check(721, &c1));  // v2 reduces to cons
        assert(bm.bindings.size() == 2);
        
        t.pop();
    }
    
    // Test 47: Cons where both children are chains that converge to the SAME var
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v_left1{expr::var{730}};
        expr v_left2{expr::var{731}};
        expr v_right1{expr::var{732}};
        expr v_right2{expr::var{733}};
        expr v_target{expr::var{734}};
        
        // Left chain: v_left1 -> v_left2 -> v_target
        bm.bindings[730] = &v_left2;
        bm.bindings[731] = &v_target;
        
        // Right chain: v_right1 -> v_right2 -> v_target (same target!)
        bm.bindings[732] = &v_right2;
        bm.bindings[733] = &v_target;
        
        expr c1{expr::cons{&v_left1, &v_right1}};
        
        // Both chains converge to v_target
        assert(bm.occurs_check(734, &c1));    // v_target found via both chains
        assert(!bm.occurs_check(730, &c1));   // v_left1 compressed away
        assert(!bm.occurs_check(731, &c1));   // v_left2 compressed away
        assert(!bm.occurs_check(732, &c1));   // v_right1 compressed away
        assert(!bm.occurs_check(733, &c1));   // v_right2 compressed away
        assert(bm.bindings.size() == 4);
        
        t.pop();
    }
    
    // Test 48: Cons where both children are chains that converge to the SAME atom
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v_left1{expr::var{740}};
        expr v_left2{expr::var{741}};
        expr v_right1{expr::var{742}};
        expr v_right2{expr::var{743}};
        expr a_target{expr::atom{"convergence"}};
        
        // Left chain: v_left1 -> v_left2 -> a_target
        bm.bindings[740] = &v_left2;
        bm.bindings[741] = &a_target;
        
        // Right chain: v_right1 -> v_right2 -> a_target (same target!)
        bm.bindings[742] = &v_right2;
        bm.bindings[743] = &a_target;
        
        expr c1{expr::cons{&v_left1, &v_right1}};
        
        // Both chains converge to atom, so no vars should be found
        assert(!bm.occurs_check(740, &c1));   // v_left1 compressed away
        assert(!bm.occurs_check(741, &c1));   // v_left2 compressed away
        assert(!bm.occurs_check(742, &c1));   // v_right1 compressed away
        assert(!bm.occurs_check(743, &c1));   // v_right2 compressed away
        assert(!bm.occurs_check(999, &c1));   // No vars in result
        assert(bm.bindings.size() == 4);
        
        t.pop();
    }
    
    // Test 49: Cons where both children are chains that converge to the SAME cons
    {
        trail t;
        bind_map bm(t);
        t.push();
        
        expr v_left1{expr::var{750}};
        expr v_left2{expr::var{751}};
        expr v_right1{expr::var{752}};
        expr v_right2{expr::var{753}};
        expr v_inner{expr::var{754}};
        expr a1{expr::atom{"y"}};
        expr target_cons{expr::cons{&v_inner, &a1}};
        
        // Left chain: v_left1 -> v_left2 -> target_cons
        bm.bindings[750] = &v_left2;
        bm.bindings[751] = &target_cons;
        
        // Right chain: v_right1 -> v_right2 -> target_cons (same target!)
        bm.bindings[752] = &v_right2;
        bm.bindings[753] = &target_cons;
        
        expr c1{expr::cons{&v_left1, &v_right1}};
        
        // Both chains converge to target_cons which contains v_inner
        assert(bm.occurs_check(754, &c1));    // v_inner found in target_cons
        assert(!bm.occurs_check(750, &c1));   // v_left1 compressed away
        assert(!bm.occurs_check(751, &c1));   // v_left2 compressed away
        assert(!bm.occurs_check(752, &c1));   // v_right1 compressed away
        assert(!bm.occurs_check(753, &c1));   // v_right2 compressed away
        assert(bm.bindings.size() == 4);
        
        t.pop();
    }
}

void test_bind_map_unify() {
    // unify() performs Prolog-like unification with occurs check
    // It's a symmetric operation: unify(a, b) should behave like unify(b, a)
    // Tests verify symmetry by commuting arguments between consecutive test cases
    
    // ========== BASIC CASES ==========
    
    // Test 1: Identical expressions (same pointer) - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"test"}};
        assert(bm.unify(&a1, &a1));
        assert(bm.bindings.size() == 0);

        t.pop();
    }
    
    // Test 2: Two different atoms - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"foo"}};
        expr a2{expr::atom{"bar"}};
        assert(!bm.unify(&a1, &a2));
        assert(bm.bindings.size() == 0);

        t.pop();
    }
    
    // Test 3: Two different atoms (commuted) - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"foo"}};
        expr a2{expr::atom{"bar"}};
        assert(!bm.unify(&a2, &a1));  // Commuted
        assert(bm.bindings.size() == 0);

        t.pop();
    }
    
    // Test 4: Two identical atoms (different objects) - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"same"}};
        expr a2{expr::atom{"same"}};
        assert(bm.unify(&a1, &a2));
        assert(bm.bindings.size() == 0);

        t.pop();
    }
    
    // Test 5: Two identical atoms (commuted) - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"same"}};
        expr a2{expr::atom{"same"}};
        assert(bm.unify(&a2, &a1));  // Commuted
        assert(bm.bindings.size() == 0);

        t.pop();
    }
    
    // Test 6: Unbound var with atom - should succeed and create binding
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{0}};
        expr a1{expr::atom{"bound"}};
        assert(bm.unify(&v1, &a1));
        assert(bm.bindings.size() == 1);  // Just v1 -> a1
        assert(bm.bindings.count(0) == 1);
        assert(bm.whnf(&v1) == &a1);

        t.pop();
    }
    
    // Test 7: Atom with unbound var (commuted) - should succeed and create binding
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{1}};
        expr a1{expr::atom{"bound"}};
        assert(bm.unify(&a1, &v1));  // Commuted
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.count(1) == 1);
        assert(bm.whnf(&v1) == &a1);

        t.pop();
    }
    
    // Test 8: Unbound var with cons - should succeed and create binding
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{2}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        assert(bm.unify(&v1, &c1));
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.count(2) == 1);  // v1 is bound
        assert(bm.whnf(&v1) == &c1);

        t.pop();
    }
    
    // Test 9: Cons with unbound var (commuted) - should succeed and create binding
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{3}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        assert(bm.unify(&c1, &v1));  // Commuted
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.count(3) == 1);  // v1 is bound
        assert(bm.whnf(&v1) == &c1);

        t.pop();
    }
    
    // Test 10: Two unbound vars - should succeed and create binding
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{4}};
        expr v2{expr::var{5}};
        assert(bm.unify(&v1, &v2));
        assert(bm.bindings.size() == 1);  // One var bound to the other
        // Verify one of them is bound (either 4->v2 or 5->v1)
        assert(bm.bindings.count(4) == 1 || bm.bindings.count(5) == 1);
        // Both should reduce to the same thing
        const expr* result1 = bm.whnf(&v1);
        const expr* result2 = bm.whnf(&v2);
        assert(result1 == result2);
        assert(result1 == &v1 || result1 == &v2);  // Result is one of the vars
        t.pop();
    }
    
    // Test 11: Two unbound vars (commuted) - should succeed and create binding
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{6}};
        expr v2{expr::var{7}};
        assert(bm.unify(&v2, &v1));  // Commuted
        assert(bm.bindings.size() == 1);
        // Verify one of them is bound (either 6->v2 or 7->v1)
        assert(bm.bindings.count(6) == 1 || bm.bindings.count(7) == 1);
        // Both should reduce to the same thing
        const expr* result1 = bm.whnf(&v1);
        const expr* result2 = bm.whnf(&v2);
        assert(result1 == result2);
        assert(result1 == &v1 || result1 == &v2);  // Result is one of the vars

        t.pop();
    }
    
    // ========== OCCURS CHECK CASES ==========
    
    // Test 12: Var with cons containing that var - should fail (occurs check)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{10}};
        expr v2{expr::var{10}};  // Same index
        expr a1{expr::atom{"test"}};
        expr c1{expr::cons{&v2, &a1}};
        assert(!bm.unify(&v1, &c1));  // Should fail occurs check
        assert(bm.bindings.size() == 0);  // No binding created (occurs check before bind)

        t.pop();
    }
    
    // Test 13: Cons containing var with that var (commuted) - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{11}};
        expr v2{expr::var{11}};
        expr a1{expr::atom{"test"}};
        expr c1{expr::cons{&v2, &a1}};
        assert(!bm.unify(&c1, &v1));  // Commuted
        assert(bm.bindings.size() == 0);  // No binding created (occurs check before bind)

        t.pop();
    }
    
    // Test 14: Var with cons containing chain to that var - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{12}};
        expr v2{expr::var{13}};
        expr v3{expr::var{12}};  // Same as v1
        expr a1{expr::atom{"test"}};
        
        // v2 -> v3 (which is same index as v1)
        bm.bindings[13] = &v3;
        
        expr c1{expr::cons{&v2, &a1}};
        assert(!bm.unify(&v1, &c1));  // Should fail: v1 with cons containing chain to v1
        assert(bm.bindings.size() == 1);  // Only original binding (v2 -> v3), no new binding

        t.pop();
    }
    
    // Test 15: Cons with chain to var, unified with that var (commuted) - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{14}};
        expr v2{expr::var{15}};
        expr v3{expr::var{14}};
        expr a1{expr::atom{"test"}};
        
        bm.bindings[15] = &v3;
        
        expr c1{expr::cons{&v2, &a1}};
        assert(!bm.unify(&c1, &v1));  // Commuted
        assert(bm.bindings.size() == 1);  // Only original binding (v2 -> v3), no new binding

        t.pop();
    }
    
    // Test 16: Var with deeply nested cons containing that var - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{16}};
        expr v2{expr::var{16}};
        expr a1{expr::atom{"a"}};
        
        // Build: cons(cons(cons(v2, a1), a1), a1)
        expr inner1{expr::cons{&v2, &a1}};
        expr inner2{expr::cons{&inner1, &a1}};
        expr outer{expr::cons{&inner2, &a1}};
        
        assert(!bm.unify(&v1, &outer));  // Should fail
        assert(bm.bindings.size() == 0);  // No binding created (occurs check before bind)

        t.pop();
    }
    
    // Test 17: Deeply nested cons with var, unified with that var (commuted) - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{17}};
        expr v2{expr::var{17}};
        expr a1{expr::atom{"a"}};
        
        expr inner1{expr::cons{&v2, &a1}};
        expr inner2{expr::cons{&inner1, &a1}};
        expr outer{expr::cons{&inner2, &a1}};
        
        assert(!bm.unify(&outer, &v1));  // Commuted
        assert(bm.bindings.size() == 0);  // No binding created (occurs check before bind)

        t.pop();
    }
    
    // ========== CHAIN CASES ==========
    
    // Test 18: Var bound to another var, unify with atom - should follow chain
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{20}};
        expr v2{expr::var{21}};
        expr a1{expr::atom{"target"}};
        
        // v1 -> v2 (unbound)
        bm.bindings[20] = &v2;
        
        assert(bm.unify(&v1, &a1));  // Should bind v2 to a1
        assert(bm.bindings.size() == 2);  // Original chain + new binding (v2 -> a1)
        assert(bm.bindings.count(21) == 1);  // v2 is now bound
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);

        t.pop();
    }
    
    // Test 19: Atom with var bound to another var (commuted) - should follow chain
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{22}};
        expr v2{expr::var{23}};
        expr a1{expr::atom{"target"}};
        
        bm.bindings[22] = &v2;
        
        assert(bm.unify(&a1, &v1));  // Commuted
        assert(bm.bindings.size() == 2);  // Original chain + new binding (v2 -> a1)
        assert(bm.bindings.count(23) == 1);  // v2 is now bound
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);

        t.pop();
    }
    
    // Test 20: Two vars in a chain, unify them - should succeed (already unified)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{24}};
        expr v2{expr::var{25}};
        
        // v1 -> v2
        bm.bindings[24] = &v2;
        
        assert(bm.unify(&v1, &v2));  // Both reduce to v2, already unified
        assert(bm.bindings.size() == 1);  // No new binding created
        assert(bm.whnf(&v1) == &v2);
        assert(bm.whnf(&v2) == &v2);

        t.pop();
    }
    
    // Test 21: Two vars in a chain, unify them (commuted) - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{26}};
        expr v2{expr::var{27}};
        
        bm.bindings[26] = &v2;
        
        assert(bm.unify(&v2, &v1));  // Commuted
        assert(bm.bindings.size() == 1);  // No new binding created
        assert(bm.whnf(&v1) == &v2);
        assert(bm.whnf(&v2) == &v2);

        t.pop();
    }
    
    // Test 22: Long chain of vars, unify head with atom
    {
        trail t;
        bind_map bm(t);
        t.push();
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
        assert(bm.bindings.size() == 4);  // 3 original chain + 1 new binding (v4 -> a1)
        assert(bm.bindings.count(31) == 1);  // v4 is now bound
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v4) == &a1);

        t.pop();
    }
    
    // Test 23: Atom with long chain of vars (commuted)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{32}};
        expr v2{expr::var{33}};
        expr v3{expr::var{34}};
        expr v4{expr::var{35}};
        expr a1{expr::atom{"end"}};
        
        bm.bindings[32] = &v2;
        bm.bindings[33] = &v3;
        bm.bindings[34] = &v4;
        
        assert(bm.unify(&a1, &v1));  // Commuted
        assert(bm.bindings.size() == 4);  // 3 original chain + 1 new binding (v4 -> a1)
        assert(bm.bindings.count(35) == 1);  // v4 is now bound
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v4) == &a1);

        t.pop();
    }
    
    // ========== CONS UNIFICATION CASES ==========
    
    // Test 24: Two cons with same atoms - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"x"}};
        expr a4{expr::atom{"y"}};
        expr c2{expr::cons{&a3, &a4}};
        
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);

        t.pop();
    }
    
    // Test 25: Two cons with same atoms (commuted) - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"x"}};
        expr a4{expr::atom{"y"}};
        expr c2{expr::cons{&a3, &a4}};
        
        assert(bm.unify(&c2, &c1));  // Commuted
        assert(bm.bindings.size() == 0);

        t.pop();
    }
    
    // Test 26: Two cons with different atoms in lhs - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"z"}};  // Different
        expr a4{expr::atom{"y"}};
        expr c2{expr::cons{&a3, &a4}};
        
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // No bindings created (all atoms)

        t.pop();
    }
    
    // Test 27: Two cons with different atoms in rhs - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"x"}};
        expr a4{expr::atom{"z"}};  // Different
        expr c2{expr::cons{&a3, &a4}};
        
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // No bindings created (all atoms)

        t.pop();
    }
    
    // Test 28: Cons with vars that can be unified - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{40}};
        expr a1{expr::atom{"y"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr c2{expr::cons{&a2, &a3}};
        
        assert(bm.unify(&c1, &c2));  // Should bind v1 to a2
        assert(bm.bindings.size() == 1);  // One binding created
        assert(bm.bindings.count(40) == 1);  // v1 is bound
        assert(bm.whnf(&v1) == &a2);

        t.pop();
    }
    
    // Test 29: Cons with vars that can be unified (commuted) - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{41}};
        expr a1{expr::atom{"y"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr c2{expr::cons{&a2, &a3}};
        
        assert(bm.unify(&c2, &c1));  // Commuted
        assert(bm.bindings.size() == 1);  // One binding created
        assert(bm.bindings.count(41) == 1);  // v1 is bound
        assert(bm.whnf(&v1) == &a2);

        t.pop();
    }
    
    // Test 30: Cons with vars that cannot be unified (conflicting) - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{42}};
        expr a1{expr::atom{"y"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"z"}};  // Conflicts with a1
        expr c2{expr::cons{&a2, &a3}};
        
        assert(!bm.unify(&c1, &c2));  // rhs children don't match
        // Partial binding left: v1 was bound to a2 before rhs failed
        assert(bm.bindings.size() == 1);
        assert(bm.whnf(&v1) == &a2);

        t.pop();
    }
    
    // Test 31: Cons with vars that cannot be unified (commuted) - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{43}};
        expr a1{expr::atom{"y"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"z"}};
        expr c2{expr::cons{&a2, &a3}};
        
        assert(!bm.unify(&c2, &c1));  // Commuted
        // Partial binding left: v1 was bound to a2 before rhs failed
        assert(bm.bindings.size() == 1);
        assert(bm.whnf(&v1) == &a2);

        t.pop();
    }
    
    // Test 32: Nested cons structures - should recursively unify
    {
        trail t;
        bind_map bm(t);
        t.push();
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

        t.pop();
    }
    
    // Test 33: Nested cons structures (commuted) - should recursively unify
    {
        trail t;
        bind_map bm(t);
        t.push();
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

        t.pop();
    }
    
    // ========== TYPE MISMATCH CASES ==========
    
    // Test 34: Atom with cons - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"test"}};
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr c1{expr::cons{&a2, &a3}};
        
        assert(!bm.unify(&a1, &c1));
        assert(bm.bindings.size() == 0);

        t.pop();
    }
    
    // Test 35: Cons with atom (commuted) - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"test"}};
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr c1{expr::cons{&a2, &a3}};
        
        assert(!bm.unify(&c1, &a1));  // Commuted
        assert(bm.bindings.size() == 0);

        t.pop();
    }
    
    // ========== COMPLEX CASES ==========
    
    // Test 36: Unify two cons where children need transitive unification
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{50}};
        expr a1{expr::atom{"a"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr v2{expr::var{51}};
        expr a2{expr::atom{"a"}};
        expr c2{expr::cons{&v2, &a2}};
        
        assert(bm.unify(&c1, &c2));  // Should bind v1 to v2
        assert(bm.bindings.size() == 1);  // Verify binding was created
        assert(bm.bindings.count(50) == 1 || bm.bindings.count(51) == 1);
        assert(bm.whnf(&v1) == bm.whnf(&v2));
        // Both should reduce to one of the vars
        const expr* result = bm.whnf(&v1);
        assert(result == &v1 || result == &v2);

        t.pop();
    }
    
    // Test 37: Transitive unification (commuted)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{52}};
        expr a1{expr::atom{"a"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr v2{expr::var{53}};
        expr a2{expr::atom{"a"}};
        expr c2{expr::cons{&v2, &a2}};
        
        assert(bm.unify(&c2, &c1));  // Commuted
        assert(bm.bindings.size() == 1);  // Verify binding was created
        assert(bm.bindings.count(52) == 1 || bm.bindings.count(53) == 1);
        assert(bm.whnf(&v1) == bm.whnf(&v2));
        // Both should reduce to one of the vars
        const expr* result = bm.whnf(&v1);
        assert(result == &v1 || result == &v2);

        t.pop();
    }
    
    // Test 38: Unify cons with nested vars - cons(v1, cons(v2, a)) with cons(a, cons(b, a))
    {
        trail t;
        bind_map bm(t);
        t.push();
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
        assert(bm.bindings.size() == 2);  // Two bindings created
        assert(bm.bindings.count(54) == 1);  // v1 is bound
        assert(bm.bindings.count(55) == 1);  // v2 is bound
        assert(bm.whnf(&v1) == &a2);
        assert(bm.whnf(&v2) == &a3);

        t.pop();
    }
    
    // Test 39: Nested vars (commuted)
    {
        trail t;
        bind_map bm(t);
        t.push();
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
        assert(bm.bindings.size() == 2);  // Two bindings created
        assert(bm.bindings.count(56) == 1);  // v1 is bound
        assert(bm.bindings.count(57) == 1);  // v2 is bound
        assert(bm.whnf(&v1) == &a2);
        assert(bm.whnf(&v2) == &a3);

        t.pop();
    }
    
    // Test 40: Multiple unifications building up bindings
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{60}};
        expr v2{expr::var{61}};
        expr v3{expr::var{62}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr a3{expr::atom{"z"}};
        
        // First unification: v1 with a1
        assert(bm.unify(&v1, &a1));
        assert(bm.bindings.size() == 1);
        assert(bm.whnf(&v1) == &a1);
        
        // Second unification: v2 with a2
        assert(bm.unify(&v2, &a2));
        assert(bm.bindings.size() == 2);
        assert(bm.whnf(&v2) == &a2);
        
        // Third unification: v3 with a3
        assert(bm.unify(&v3, &a3));
        assert(bm.bindings.size() == 3);
        assert(bm.whnf(&v3) == &a3);
        
        // All bindings should persist
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a2);
        assert(bm.whnf(&v3) == &a3);

        t.pop();
    }
    
    // Test 41: Unify after previous unification - bindings persist
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{63}};
        expr v2{expr::var{64}};
        expr a1{expr::atom{"first"}};
        
        // First: unify v1 with v2
        assert(bm.unify(&v1, &v2));
        assert(bm.bindings.size() == 1);  // One binding created
        
        // Second: unify v1 with a1 (should bind v2 to a1 via v1)
        assert(bm.unify(&v1, &a1));
        assert(bm.bindings.size() == 2);  // Two bindings total
        
        // Both should now point to a1
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);

        t.pop();
    }
    
    // Test 42: Self-unification of var - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{65}};
        assert(bm.unify(&v1, &v1));  // Same pointer
        assert(bm.bindings.size() == 0);  // No binding created

        t.pop();
    }
    
    // Test 43: Unify bound var with its binding - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{66}};
        expr a1{expr::atom{"bound"}};
        
        bm.bindings[66] = &a1;
        
        assert(bm.unify(&v1, &a1));  // v1 reduces to a1, unifying a1 with a1
        assert(bm.bindings.size() == 1);  // No new binding, still just the original

        t.pop();
    }
    
    // Test 44: Unify two bound vars bound to same thing - should succeed
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{67}};
        expr v2{expr::var{68}};
        expr a1{expr::atom{"same"}};
        
        bm.bindings[67] = &a1;
        bm.bindings[68] = &a1;
        
        assert(bm.unify(&v1, &v2));  // Both reduce to a1
        assert(bm.bindings.size() == 2);  // No new binding, still just the two original

        t.pop();
    }
    
    // Test 45: Unify two bound vars bound to different atoms - should fail
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{69}};
        expr v2{expr::var{70}};
        expr a1{expr::atom{"first"}};
        expr a2{expr::atom{"second"}};
        
        bm.bindings[69] = &a1;
        bm.bindings[70] = &a2;
        
        assert(!bm.unify(&v1, &v2));  // Reduce to different atoms
        assert(bm.bindings.size() == 2);  // No new binding, still just the two original

        t.pop();
    }
    
    // Test 46: Deeply nested cons unification with vars at various levels
    {
        trail t;
        bind_map bm(t);
        t.push();
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
        assert(bm.bindings.size() == 3);  // Three bindings: v1->a2, v2->a3, v3->a4
        assert(bm.bindings.count(71) == 1);  // v1 is bound
        assert(bm.bindings.count(72) == 1);  // v2 is bound
        assert(bm.bindings.count(73) == 1);  // v3 is bound
        assert(bm.whnf(&v1) == &a2);
        assert(bm.whnf(&v2) == &a3);
        assert(bm.whnf(&v3) == &a4);

        t.pop();
    }
    
    // Test 47: Unify with chains on both sides
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{74}};
        expr v2{expr::var{75}};
        expr v3{expr::var{76}};
        expr v4{expr::var{77}};
        
        // Left chain: v1 -> v2 (unbound)
        bm.bindings[74] = &v2;
        
        // Right chain: v3 -> v4 (unbound)
        bm.bindings[76] = &v4;
        
        assert(bm.unify(&v1, &v3));  // Should unify v2 with v4
        assert(bm.bindings.size() == 3);  // Original 2 chains + 1 new binding (v2 <-> v4)
        assert(bm.whnf(&v1) == bm.whnf(&v3));
        assert(bm.whnf(&v2) == bm.whnf(&v4));
        // All four vars should reduce to the same final target
        const expr* result = bm.whnf(&v1);
        assert(result == &v2 || result == &v4);  // Should be one of the tail vars

        t.pop();
    }
    
    // Test 48: Cons unification where lhs succeeds but rhs fails
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"x"}};
        expr a4{expr::atom{"z"}};  // Different from a2
        expr c2{expr::cons{&a3, &a4}};
        
        assert(!bm.unify(&c1, &c2));  // lhs matches, rhs doesn't
        assert(bm.bindings.size() == 0);  // No bindings created (all atoms)

        t.pop();
    }
    
    // Test 49: Cons unification where first child fails (short circuit)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&a1, &a2}};
        
        expr a3{expr::atom{"z"}};  // Different from a1
        expr a4{expr::atom{"y"}};
        expr c2{expr::cons{&a3, &a4}};
        
        assert(!bm.unify(&c1, &c2));  // Should fail on lhs
        assert(bm.bindings.size() == 0);  // No bindings created (all atoms)

        t.pop();
    }
    
    // Test 50: Very complex nested unification
    {
        trail t;
        bind_map bm(t);
        t.push();
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
        assert(bm.bindings.size() == 2);  // Two bindings: v1->a2, v2->a4
        assert(bm.bindings.count(80) == 1);  // v1 is bound
        assert(bm.bindings.count(81) == 1);  // v2 is bound
        assert(bm.whnf(&v1) == &a2);
        assert(bm.whnf(&v2) == &a4);

        t.pop();
    }
    
    // Test 51: Unification failure with trail - partial bindings should be undone
    {
        trail t;
        bind_map bm(t);
        expr v1{expr::var{82}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr c1{expr::cons{&v1, &a1}};
        
        expr a3{expr::atom{"z"}};
        expr a4{expr::atom{"w"}};  // Different from a1
        expr c2{expr::cons{&a3, &a4}};
        
        // Push frame before unification
        t.push();
        
        // This should fail (rhs mismatch), leaving partial binding v1->a3
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 1);  // Partial binding left
        assert(bm.bindings.count(82) == 1);
        assert(bm.whnf(&v1) == &a3);
        
        // Pop frame - partial bindings should be undone
        t.pop();
        assert(bm.bindings.size() == 0);  // All bindings undone
        assert(bm.bindings.count(82) == 0);  // v1 is unbound again
        assert(bm.whnf(&v1) == &v1);  // v1 reduces to itself
    }
    
    // Test 52: Multiple unifications with shared vars - transitive constraints
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{83}};
        expr v2{expr::var{84}};
        expr v3{expr::var{85}};
        
        // First structure: cons(V1, V1) - both children are same var
        expr c1{expr::cons{&v1, &v1}};
        
        // Second structure: cons(a, V2)
        expr a1{expr::atom{"a"}};
        expr c2{expr::cons{&a1, &v2}};
        
        // Unify cons(V1, V1) with cons(a, V2)
        // This should bind V1 to 'a' and V2 to 'a'
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 2);  // V1->a and V2->a (or V2->V1)
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);
        
        // Now try to unify cons(V1, V1) with cons(V3, k)
        expr a2{expr::atom{"k"}};
        expr c3{expr::cons{&v3, &a2}};
        
        // This should fail because V1 reduces to 'a', so we're trying to unify
        // cons(a, a) with cons(V3, k), which would bind V3 to 'a', but then
        // the rhs would try to unify 'a' with 'k', which fails
        assert(!bm.unify(&c1, &c3));
        // Partial binding: V3 was bound to 'a' before rhs failed
        assert(bm.bindings.size() == 3);  // Original 2 + V3->a
        assert(bm.bindings.count(85) == 1);
        assert(bm.whnf(&v3) == &a1);
        
        t.pop();
    }
    
    // Test 53: Chained structural unifications - transitive binding propagation
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{86}};
        expr v2{expr::var{87}};
        expr v3{expr::var{88}};
        
        // First structure: cons(V1, V1)
        expr c1{expr::cons{&v1, &v1}};
        
        // Second structure: cons(V2, V3)
        expr c2{expr::cons{&v2, &v3}};
        
        // Unify cons(V1, V1) with cons(V2, V3)
        // This unifies V1 with V2 (lhs), then V1 with V3 (rhs)
        // Result: all three vars in same equivalence class
        assert(bm.unify(&c1, &c2));
        size_t bindings_after_first = bm.bindings.size();
        assert(bindings_after_first == 2);  // 2 bindings (2 vars point to the third)
        
        // V1, V2, V3 should all reduce to the same thing
        const expr* result1 = bm.whnf(&v1);
        const expr* result2 = bm.whnf(&v2);
        const expr* result3 = bm.whnf(&v3);
        assert(result1 == result2);
        assert(result2 == result3);
        // Result should be one of the three vars
        assert(result1 == &v1 || result1 == &v2 || result1 == &v3);
        
        // Now unify V3 with atom 'a'
        expr a1{expr::atom{"a"}};
        assert(bm.unify(&v3, &a1));
        assert(bm.bindings.size() == bindings_after_first + 1);  // One more binding
        
        // All three vars should now reduce to 'a'
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);
        assert(bm.whnf(&v3) == &a1);
        
        t.pop();
    }
    
    // ========== STRUCTURAL OCCURS CHECK CASES ==========
    
    // Test 54: Structural occurs check - cons(V1, a) with cons(cons(V1, b), a)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{89}};
        expr v2{expr::var{89}};  // Same index
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        expr c1{expr::cons{&v1, &a1}};
        
        // Build cons(cons(V1, b), a) - V1 appears nested inside
        expr inner{expr::cons{&v2, &a2}};
        expr c2{expr::cons{&inner, &a1}};
        
        // Should fail: trying to bind V1 to cons(cons(V1, b), a) which contains V1
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 55: Structural occurs check - cons(V1, V2) with cons(cons(V1, a), V2)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{90}};
        expr v2{expr::var{91}};
        expr v3{expr::var{90}};  // Same as V1
        expr v4{expr::var{91}};  // Same as V2
        expr a1{expr::atom{"a"}};
        
        // Build cons(V1, V2)
        expr c1{expr::cons{&v1, &v2}};
        
        // Build cons(cons(V1, a), V2)
        expr inner{expr::cons{&v3, &a1}};
        expr c2{expr::cons{&inner, &v4}};
        
        // Unifying cons(V1, V2) with cons(cons(V1, a), V2)
        // lhs: V1 with cons(V1, a) - should fail occurs check (V1 in structure)
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 56: Structural occurs check - cons(V1, cons(V2, V1)) with cons(cons(V1, a), b)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{92}};
        expr v2{expr::var{93}};
        expr v3{expr::var{92}};  // Same as V1
        expr v4{expr::var{92}};  // Same as V1
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Build cons(V1, cons(V2, V1))
        expr inner1{expr::cons{&v2, &v3}};
        expr c1{expr::cons{&v1, &inner1}};
        
        // Build cons(cons(V1, a), b)
        expr inner2{expr::cons{&v4, &a1}};
        expr c2{expr::cons{&inner2, &a2}};
        
        // Unifying cons(V1, cons(V2, V1)) with cons(cons(V1, a), b)
        // lhs: V1 with cons(V1, a) - should fail occurs check (V1 in structure)
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 57: Structural occurs check through chain - cons(V1, V2) with cons(a, cons(V3, b)) where V1->V3
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{93}};
        expr v2{expr::var{94}};
        expr v3{expr::var{93}};  // Same as V1
        expr v4{expr::var{95}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Pre-existing chain: V4 -> V1
        bm.bindings[95] = &v1;
        
        expr c1{expr::cons{&v4, &v2}};
        
        // Build cons(a, cons(V1, b))
        expr inner{expr::cons{&v3, &a2}};
        expr c2{expr::cons{&a1, &inner}};
        
        // Unifying cons(V4, V2) with cons(a, cons(V1, b))
        // V4 reduces to V1 via chain
        // lhs: V1 with a - succeeds, binds V1 to a
        // rhs: V2 with cons(V1, b) - should fail occurs check through chain (V4->V1 in structure)
        // Wait, V1 is already bound to 'a' at this point, so cons(V1, b) reduces to cons(a, b)
        // Actually this should succeed
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 3);  // Original chain + V1->a + V2->cons(a,b)
        
        t.pop();
    }
    
    // Test 58: Structural occurs check - V1 appears multiple times, one occurrence creates cycle
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{96}};
        expr v2{expr::var{96}};  // Same as V1
        expr v3{expr::var{96}};  // Same as V1
        expr a1{expr::atom{"a"}};
        
        // Build cons(V1, cons(a, V1)) - V1 appears twice
        expr inner{expr::cons{&a1, &v2}};
        expr c1{expr::cons{&v1, &inner}};
        
        // Build cons(cons(V1, a), cons(a, b))
        expr a2{expr::atom{"b"}};
        expr inner2{expr::cons{&v3, &a1}};
        expr inner3{expr::cons{&a1, &a2}};
        expr c2{expr::cons{&inner2, &inner3}};
        
        // Unifying cons(V1, cons(a, V1)) with cons(cons(V1, a), cons(a, b))
        // lhs: V1 with cons(V1, a) - should fail occurs check immediately
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // ========== MULTIPLE OCCURRENCES OF SAME VAR ==========
    
    // Test 59: cons(V1, cons(V1, V1)) unified with cons(a, cons(a, a))
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{97}};
        expr v2{expr::var{97}};  // Same as V1
        expr v3{expr::var{97}};  // Same as V1
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"a"}};
        expr a3{expr::atom{"a"}};
        
        // Build cons(V1, cons(V1, V1))
        expr inner1{expr::cons{&v2, &v3}};
        expr c1{expr::cons{&v1, &inner1}};
        
        // Build cons(a, cons(a, a))
        expr inner2{expr::cons{&a2, &a3}};
        expr c2{expr::cons{&a1, &inner2}};
        
        // All three occurrences of V1 must unify to 'a'
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 1);  // Just V1->a
        assert(bm.bindings.count(97) == 1);
        assert(bm.whnf(&v1) == &a1);
        
        t.pop();
    }
    
    // Test 60: cons(cons(V1, V1), V1) - var in nested lhs and top-level rhs
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{98}};
        expr v2{expr::var{98}};  // Same as V1
        expr v3{expr::var{98}};  // Same as V1
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"x"}};
        
        // Build cons(cons(V1, V1), V1)
        expr inner1{expr::cons{&v1, &v2}};
        expr c1{expr::cons{&inner1, &v3}};
        
        // Build cons(cons(x, x), x)
        expr inner2{expr::cons{&a1, &a2}};
        expr c2{expr::cons{&inner2, &a3}};
        
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 1);  // Just V1->x
        assert(bm.bindings.count(98) == 1);
        assert(bm.whnf(&v1) == &a1);
        
        t.pop();
    }
    
    // Test 61: cons(V1, cons(V1, V1)) fails occurs check with cons(cons(V1, a), b)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{99}};
        expr v2{expr::var{99}};  // Same as V1
        expr v3{expr::var{99}};  // Same as V1
        expr v4{expr::var{99}};  // Same as V1
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Build cons(V1, cons(V1, V1))
        expr inner1{expr::cons{&v2, &v3}};
        expr c1{expr::cons{&v1, &inner1}};
        
        // Build cons(cons(V1, a), b)
        expr inner2{expr::cons{&v4, &a1}};
        expr c2{expr::cons{&inner2, &a2}};
        
        // Unifying cons(V1, cons(V1, V1)) with cons(cons(V1, a), b)
        // lhs: V1 with cons(V1, a) - should fail occurs check
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // ========== ADDITIONAL STRUCTURAL TESTS ==========
    
    // Test 62: Var appears in both lhs and rhs of cons - consistent binding
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{100}};
        expr v2{expr::var{101}};
        expr a1{expr::atom{"a"}};
        
        // Pre-bind V2 to V1
        bm.bindings[101] = &v1;
        
        expr c1{expr::cons{&v1, &v2}};
        expr c2{expr::cons{&a1, &v1}};
        
        // Unifying cons(V1, V2) with cons(a, V1)
        // V2 reduces to V1, so we're unifying cons(V1, V1) with cons(a, V1)
        // lhs: V1 with a - binds V1 to a
        // rhs: V1 with V1 - same thing, succeeds
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 2);  // Original V2->V1 + V1->a
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a1);  // V2 transitively reduces to a
        
        t.pop();
    }
    
    // Test 63: cons(V1, cons(V2, V1)) with cons(V2, cons(V1, V2)) - complex symmetry
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{102}};
        expr v2{expr::var{103}};
        
        // Build cons(V1, cons(V2, V1))
        expr inner1{expr::cons{&v2, &v1}};
        expr c1{expr::cons{&v1, &inner1}};
        
        // Build cons(V2, cons(V1, V2))
        expr inner2{expr::cons{&v1, &v2}};
        expr c2{expr::cons{&v2, &inner2}};
        
        // Unifying cons(V1, cons(V2, V1)) with cons(V2, cons(V1, V2))
        // lhs: V1 with V2 - creates binding (say 102->103)
        // rhs: cons(V2, V1) with cons(V1, V2)
        //   Both V1 and V2 now reduce to 103, so:
        //   - lhs of rhs: V2 with V1 - both reduce to 103, succeeds
        //   - rhs of rhs: V1 with V2 - both reduce to 103, succeeds
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 1);  // Just V1 and V2 unified
        assert(bm.bindings.count(102) == 1 || bm.bindings.count(103) == 1);
        assert(bm.whnf(&v1) == bm.whnf(&v2));
        const expr* result = bm.whnf(&v1);
        assert(result == &v1 || result == &v2);
        
        t.pop();
    }
    
    // ========== CHAIN EXTENSION THROUGH UNIFICATION ==========
    
    // Test 64: Pre-existing chain extended by structural unification
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{104}};
        expr v2{expr::var{105}};
        expr v3{expr::var{106}};
        expr a1{expr::atom{"a"}};
        
        // Pre-existing chain: V1 -> V2
        bm.bindings[104] = &v2;
        
        expr c1{expr::cons{&v1, &a1}};
        expr c2{expr::cons{&v3, &a1}};
        
        // Unify cons(V1, a) with cons(V3, a)
        // V1 reduces to V2, so we unify V2 with V3
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 2);  // Original chain + new binding
        // Either V2->V3 or V3->V2
        assert(bm.bindings.count(105) == 1 || bm.bindings.count(106) == 1);
        assert(bm.whnf(&v1) == bm.whnf(&v3));
        assert(bm.whnf(&v2) == bm.whnf(&v3));
        
        t.pop();
    }
    
    // Test 65: Multiple chains merge through structural unification
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{107}};
        expr v2{expr::var{108}};
        expr v3{expr::var{109}};
        expr v4{expr::var{110}};
        expr v5{expr::var{111}};
        
        // Pre-existing chains: V1->V2, V3->V4
        bm.bindings[107] = &v2;
        bm.bindings[109] = &v4;
        
        // Unify cons(V2, V4) with cons(V5, V5)
        expr c1{expr::cons{&v2, &v4}};
        expr c2{expr::cons{&v5, &v5}};
        
        // lhs: V2 with V5 - binds one to the other
        // rhs: V4 with V5 - both now in same equivalence class
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 3 || bm.bindings.size() == 4);  // Original 2 chains + 1-2 new bindings
        
        // All five vars should reduce to the same thing
        const expr* result = bm.whnf(&v1);
        assert(bm.whnf(&v2) == result);
        assert(bm.whnf(&v3) == result);
        assert(bm.whnf(&v4) == result);
        assert(bm.whnf(&v5) == result);
        
        t.pop();
    }
    
    // ========== NESTED CONS WITH SHARED INNER STRUCTURE ==========
    
    // Test 66: Shared inner cons cell
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{112}};
        expr v2{expr::var{113}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Create shared inner: cons(V1, a)
        expr inner{expr::cons{&v1, &a1}};
        
        // Build cons(inner, V2) and cons(inner, b)
        expr c1{expr::cons{&inner, &v2}};
        expr c2{expr::cons{&inner, &a2}};
        
        // Unify cons(inner, V2) with cons(inner, b)
        // lhs: inner with inner - same pointer, succeeds
        // rhs: V2 with b - binds V2 to b
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 1);  // Just V2->b, inner unchanged
        assert(bm.bindings.count(113) == 1);
        assert(bm.whnf(&v2) == &a2);
        assert(bm.whnf(&v1) == &v1);  // V1 still unbound
        
        t.pop();
    }
    
    // ========== MULTIPLE FAILURES WITH TRAIL ISOLATION ==========
    
    // Test 67: Multiple failures in sequence - trail isolation
    {
        trail t;
        bind_map bm(t);
        expr v1{expr::var{114}};
        expr v2{expr::var{115}};
        expr a1{expr::atom{"x"}};
        expr a2{expr::atom{"y"}};
        expr a3{expr::atom{"z"}};
        
        // First failure
        t.push();
        expr c1{expr::cons{&v1, &a1}};
        expr c2{expr::cons{&a2, &a3}};  // Mismatched rhs
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 1);  // Partial: V1->a2
        assert(bm.whnf(&v1) == &a2);
        t.pop();
        
        // After pop, bindings should be undone
        assert(bm.bindings.size() == 0);
        assert(bm.whnf(&v1) == &v1);
        assert(bm.whnf(&v2) == &v2);
        
        // Second failure - different partial bindings
        t.push();
        expr a4{expr::atom{"w"}};
        expr a5{expr::atom{"q"}};
        expr c3{expr::cons{&v2, &a4}};
        expr c4{expr::cons{&a5, &a1}};  // Mismatched rhs
        assert(!bm.unify(&c3, &c4));
        assert(bm.bindings.size() == 1);  // Partial: V2->a5
        assert(bm.whnf(&v2) == &a5);
        assert(bm.whnf(&v1) == &v1);  // V1 still unbound (isolated from first failure)
        t.pop();
        
        // After second pop, all bindings undone
        assert(bm.bindings.size() == 0);
        assert(bm.whnf(&v1) == &v1);
        assert(bm.whnf(&v2) == &v2);
    }
    
    // ========== DEEPLY NESTED IDENTICAL SUBSTRUCTURES ==========
    
    // Test 68: cons(cons(V1, V1), cons(V1, V1)) - var appears 4 times
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{116}};
        expr v2{expr::var{116}};
        expr v3{expr::var{116}};
        expr v4{expr::var{116}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"a"}};
        expr a3{expr::atom{"a"}};
        expr a4{expr::atom{"a"}};
        
        // Build cons(cons(V1, V1), cons(V1, V1))
        expr left1{expr::cons{&v1, &v2}};
        expr right1{expr::cons{&v3, &v4}};
        expr c1{expr::cons{&left1, &right1}};
        
        // Build cons(cons(a, a), cons(a, a))
        expr left2{expr::cons{&a1, &a2}};
        expr right2{expr::cons{&a3, &a4}};
        expr c2{expr::cons{&left2, &right2}};
        
        // All four occurrences must bind consistently
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 1);  // Just V1->a
        assert(bm.bindings.count(116) == 1);
        assert(bm.whnf(&v1) == &a1);
        
        t.pop();
    }
    
    // ========== CONS CHILDREN BOUND TO CONS CELLS ==========
    
    // Test 69: Vars bound to cons cells, then unified
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{117}};
        expr v2{expr::var{118}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Create cons(a, b)
        expr c_ab{expr::cons{&a1, &a2}};
        
        // Bind both V1 and V2 to the same cons cell
        bm.bindings[117] = &c_ab;
        bm.bindings[118] = &c_ab;
        
        // Unify V1 with V2 - both reduce to same cons, should succeed
        assert(bm.unify(&v1, &v2));
        assert(bm.bindings.size() == 2);  // No new binding
        assert(bm.whnf(&v1) == &c_ab);
        assert(bm.whnf(&v2) == &c_ab);
        
        t.pop();
    }
    
    // Test 70: Var bound to cons, unified with cons containing vars
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{119}};
        expr v2{expr::var{120}};
        expr v3{expr::var{121}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Create cons(a, b) and bind V1 to it
        expr c_ab{expr::cons{&a1, &a2}};
        bm.bindings[119] = &c_ab;
        
        // Unify V1 with cons(V2, V3)
        expr c2{expr::cons{&v2, &v3}};
        
        // V1 reduces to cons(a, b), so we unify cons(a, b) with cons(V2, V3)
        // This should bind V2 to a and V3 to b
        assert(bm.unify(&v1, &c2));
        assert(bm.bindings.size() == 3);  // Original V1->cons + V2->a + V3->b
        assert(bm.bindings.count(120) == 1);
        assert(bm.bindings.count(121) == 1);
        assert(bm.whnf(&v2) == &a1);
        assert(bm.whnf(&v3) == &a2);
        
        t.pop();
    }
    
    // ========== UNIFICATION ORDERING MATTERS ==========
    
    // Test 71: Pre-bound var causes failure in structural unification
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{122}};
        expr v2{expr::var{123}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        
        // Pre-bind V1 to 'c'
        bm.bindings[122] = &a3;
        
        // Unify cons(V1, a) with cons(b, V2)
        expr c1{expr::cons{&v1, &a1}};
        expr c2{expr::cons{&a2, &v2}};
        
        // lhs: V1 (reduces to c) with b - should fail
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 1);  // Only original binding, no partial
        assert(bm.whnf(&v1) == &a3);
        assert(bm.whnf(&v2) == &v2);  // V2 unbound
        
        t.pop();
    }
    
    // Test 72: Pre-bound var in rhs causes partial binding
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{124}};
        expr v2{expr::var{125}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        
        // Pre-bind V2 to 'c'
        bm.bindings[125] = &a3;
        
        // Unify cons(V1, a) with cons(b, V2)
        expr c1{expr::cons{&v1, &a1}};
        expr c2{expr::cons{&a2, &v2}};
        
        // lhs: V1 with b - succeeds, binds V1 to b
        // rhs: a with V2 (reduces to c) - should fail
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 2);  // Original V2->c + partial V1->b
        assert(bm.bindings.count(124) == 1);
        assert(bm.whnf(&v1) == &a2);
        assert(bm.whnf(&v2) == &a3);
        
        t.pop();
    }
    
    // ========== TRIPLE-NESTED CONS ==========
    
    // Test 73: Triple-nested cons with vars at each level
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{126}};
        expr v2{expr::var{127}};
        expr v3{expr::var{128}};
        expr v4{expr::var{129}};
        expr a1{expr::atom{"w"}};
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr a4{expr::atom{"z"}};
        
        // Build cons(cons(cons(V1, V2), V3), V4)
        expr inner1{expr::cons{&v1, &v2}};
        expr inner2{expr::cons{&inner1, &v3}};
        expr c1{expr::cons{&inner2, &v4}};
        
        // Build cons(cons(cons(w, x), y), z)
        expr inner3{expr::cons{&a1, &a2}};
        expr inner4{expr::cons{&inner3, &a3}};
        expr c2{expr::cons{&inner4, &a4}};
        
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 4);  // Four bindings created
        assert(bm.bindings.count(126) == 1);
        assert(bm.bindings.count(127) == 1);
        assert(bm.bindings.count(128) == 1);
        assert(bm.bindings.count(129) == 1);
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a2);
        assert(bm.whnf(&v3) == &a3);
        assert(bm.whnf(&v4) == &a4);
        
        t.pop();
    }
    
    // Test 74: Triple-nested with some pre-existing bindings
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{130}};
        expr v2{expr::var{131}};
        expr v3{expr::var{132}};
        expr v4{expr::var{133}};
        expr a1{expr::atom{"w"}};
        expr a2{expr::atom{"x"}};
        expr a3{expr::atom{"y"}};
        expr a4{expr::atom{"z"}};
        
        // Pre-bind V2 to x
        bm.bindings[131] = &a2;
        
        // Build cons(cons(cons(V1, V2), V3), V4)
        expr inner1{expr::cons{&v1, &v2}};
        expr inner2{expr::cons{&inner1, &v3}};
        expr c1{expr::cons{&inner2, &v4}};
        
        // Build cons(cons(cons(w, x), y), z)
        expr inner3{expr::cons{&a1, &a2}};
        expr inner4{expr::cons{&inner3, &a3}};
        expr c2{expr::cons{&inner4, &a4}};
        
        // V2 is already bound to x, so unification should succeed
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 4);  // Original 1 + 3 new bindings
        assert(bm.bindings.count(130) == 1);
        assert(bm.bindings.count(132) == 1);
        assert(bm.bindings.count(133) == 1);
        assert(bm.whnf(&v1) == &a1);
        assert(bm.whnf(&v2) == &a2);
        assert(bm.whnf(&v3) == &a3);
        assert(bm.whnf(&v4) == &a4);
        
        t.pop();
    }
    
    // ========== COMPLEX CHAIN NETWORKS ==========
    
    // Test 75: Unification creates long chains through structure
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{134}};
        expr v2{expr::var{135}};
        expr v3{expr::var{136}};
        expr v4{expr::var{137}};
        expr v5{expr::var{138}};
        expr v6{expr::var{139}};
        
        // First unification: cons(V1, V2) with cons(V3, V4)
        expr c1{expr::cons{&v1, &v2}};
        expr c2{expr::cons{&v3, &v4}};
        assert(bm.unify(&c1, &c2));
        size_t bindings_after_first = bm.bindings.size();
        assert(bindings_after_first == 2);  // V1 and V2 each bound
        
        // Second unification: cons(V3, V5) with cons(V6, V1)
        expr c3{expr::cons{&v3, &v5}};
        expr c4{expr::cons{&v6, &v1}};
        assert(bm.unify(&c3, &c4));
        
        // Now we have a complex network:
        // From first: V1 and V3 unified, V2 and V4 unified
        // From second: V3 and V6 unified, V5 and V1 unified
        // Result: V1, V3, V5, V6 all in one equivalence class
        //         V2, V4 in another equivalence class
        assert(bm.whnf(&v1) == bm.whnf(&v3));
        assert(bm.whnf(&v1) == bm.whnf(&v5));
        assert(bm.whnf(&v1) == bm.whnf(&v6));
        assert(bm.whnf(&v2) == bm.whnf(&v4));
        
        t.pop();
    }
    
    // ========== STRUCTURAL OCCURS CHECK WITH MULTIPLE VAR OCCURRENCES ==========
    
    // Test 76: Occurs check with var appearing multiple times in target structure
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{140}};
        expr v2{expr::var{140}};  // Same as V1
        expr v3{expr::var{140}};  // Same as V1
        expr a1{expr::atom{"a"}};
        
        // Build cons(cons(V1, V1), a) - V1 appears twice in nested structure
        expr inner{expr::cons{&v2, &v3}};
        expr c1{expr::cons{&inner, &a1}};
        
        // Try to unify V1 with this structure containing V1
        assert(!bm.unify(&v1, &c1));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 77: Var unified with deeply nested cons containing another var
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{141}};
        expr v2{expr::var{142}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"x"}};
        
        // Build deeply nested structure: cons(cons(cons(V1, a), a), a)
        expr inner1{expr::cons{&v1, &a1}};
        expr inner2{expr::cons{&inner1, &a1}};
        expr inner3{expr::cons{&inner2, &a1}};
        
        // Unify V2 with this nested structure
        assert(bm.unify(&v2, &inner3));
        assert(bm.bindings.size() == 1);  // V2 bound to nested structure
        assert(bm.bindings.count(142) == 1);
        assert(bm.whnf(&v2) == &inner3);
        assert(bm.whnf(&v1) == &v1);  // V1 still unbound
        
        // Now unify V1 with atom
        assert(bm.unify(&v1, &a2));
        assert(bm.bindings.size() == 2);  // V2->nested + V1->x
        assert(bm.bindings.count(141) == 1);
        assert(bm.whnf(&v1) == &a2);
        
        t.pop();
    }
    
    // Test 78: Occurs check during nested cons unification
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{143}};
        expr v2{expr::var{144}};
        expr v3{expr::var{143}};  // Same as V1
        expr a1{expr::atom{"a"}};
        
        // Build cons(V1, V2)
        expr c1{expr::cons{&v1, &v2}};
        
        // Build cons(a, cons(V1, a)) - V1 appears in rhs
        expr inner{expr::cons{&v3, &a1}};
        expr c2{expr::cons{&a1, &inner}};
        
        // Unifying cons(V1, V2) with cons(a, cons(V1, a))
        // lhs: V1 with a - succeeds, binds V1 to a
        // rhs: V2 with cons(V1, a) - V1 now reduces to a, so cons(a, a), should succeed
        assert(bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 2);  // V1->a, V2->cons(a,a)
        assert(bm.whnf(&v1) == &a1);
        // V2 should be bound to a cons cell
        const expr* v2_result = bm.whnf(&v2);
        assert(std::holds_alternative<expr::cons>(v2_result->content));
        
        t.pop();
    }
    
    // ========== ADDITIONAL PURE OCCURS CHECK TESTS ==========
    
    // Test 79: Occurs check at rhs child level - cons(a, V1) with cons(a, cons(b, V1))
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{145}};
        expr v2{expr::var{145}};  // Same as V1
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        expr c1{expr::cons{&a1, &v1}};
        
        // Build cons(a, cons(b, V1))
        expr inner{expr::cons{&a2, &v2}};
        expr c2{expr::cons{&a1, &inner}};
        
        // Unifying cons(a, V1) with cons(a, cons(b, V1))
        // lhs: a with a - succeeds
        // rhs: V1 with cons(b, V1) - should fail occurs check (V1 in structure)
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 80: Occurs check in nested cons child - cons(cons(V1, a), b) with cons(cons(cons(V1, c), a), b)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{146}};
        expr v2{expr::var{146}};  // Same as V1
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        
        // Build cons(cons(V1, a), b)
        expr inner1{expr::cons{&v1, &a1}};
        expr c1{expr::cons{&inner1, &a2}};
        
        // Build cons(cons(cons(V1, c), a), b)
        expr inner2{expr::cons{&v2, &a3}};
        expr inner3{expr::cons{&inner2, &a1}};
        expr c2{expr::cons{&inner3, &a2}};
        
        // Unifying cons(cons(V1, a), b) with cons(cons(cons(V1, c), a), b)
        // lhs: cons(V1, a) with cons(cons(V1, c), a)
        //   - lhs of lhs: V1 with cons(V1, c) - should fail occurs check
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 81: Occurs check with var in both children - cons(V1, V2) with cons(cons(V1, a), cons(b, V1))
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{147}};
        expr v2{expr::var{148}};
        expr v3{expr::var{147}};  // Same as V1
        expr v4{expr::var{147}};  // Same as V1
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        expr c1{expr::cons{&v1, &v2}};
        
        // Build cons(cons(V1, a), cons(b, V1))
        expr inner1{expr::cons{&v3, &a1}};
        expr inner2{expr::cons{&a2, &v4}};
        expr c2{expr::cons{&inner1, &inner2}};
        
        // Unifying cons(V1, V2) with cons(cons(V1, a), cons(b, V1))
        // lhs: V1 with cons(V1, a) - should fail occurs check immediately
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 82: Occurs check through multiple levels - cons(V1, a) with cons(cons(cons(V1, b), c), a)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{149}};
        expr v2{expr::var{149}};  // Same as V1
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        expr a3{expr::atom{"c"}};
        
        expr c1{expr::cons{&v1, &a1}};
        
        // Build cons(cons(cons(V1, b), c), a) - V1 deeply nested in lhs
        expr inner1{expr::cons{&v2, &a2}};
        expr inner2{expr::cons{&inner1, &a3}};
        expr c2{expr::cons{&inner2, &a1}};
        
        // Unifying cons(V1, a) with cons(cons(cons(V1, b), c), a)
        // lhs: V1 with cons(cons(V1, b), c) - should fail occurs check (V1 deeply nested)
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 83: Occurs check with pre-existing chain - V2->V1, then cons(V1, a) with cons(cons(V2, b), a)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{150}};
        expr v2{expr::var{151}};
        expr v3{expr::var{151}};  // Same as V2
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Pre-existing chain: V2 -> V1
        bm.bindings[151] = &v1;
        
        expr c1{expr::cons{&v1, &a1}};
        
        // Build cons(cons(V2, b), a) - V2 chains to V1
        expr inner{expr::cons{&v3, &a2}};
        expr c2{expr::cons{&inner, &a1}};
        
        // Unifying cons(V1, a) with cons(cons(V2, b), a)
        // lhs: V1 with cons(V2, b)
        //   V2 reduces to V1 via chain, so cons(V2, b) contains V1 indirectly
        //   Should fail occurs check through chain
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 1);  // Only original chain, occurs check prevents new binding
        
        t.pop();
    }
    
    // Test 84: Occurs check with var appearing in multiple nested positions
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{152}};
        expr v2{expr::var{152}};  // Same as V1
        expr v3{expr::var{152}};  // Same as V1
        expr v4{expr::var{153}};
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Build cons(cons(V1, cons(V1, a)), b) - V1 appears twice in nested structure
        expr inner1{expr::cons{&v2, &a1}};
        expr inner2{expr::cons{&v3, &inner1}};
        expr c1{expr::cons{&inner2, &a2}};
        
        // Try to unify V1 with this structure
        assert(!bm.unify(&v1, &c1));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 85: Occurs check - cons(cons(V1, V2), a) with cons(cons(cons(V1, b), V2), a)
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{154}};
        expr v2{expr::var{155}};
        expr v3{expr::var{154}};  // Same as V1
        expr v4{expr::var{155}};  // Same as V2
        expr a1{expr::atom{"a"}};
        expr a2{expr::atom{"b"}};
        
        // Build cons(cons(V1, V2), a)
        expr inner1{expr::cons{&v1, &v2}};
        expr c1{expr::cons{&inner1, &a1}};
        
        // Build cons(cons(cons(V1, b), V2), a)
        expr inner2{expr::cons{&v3, &a2}};
        expr inner3{expr::cons{&inner2, &v4}};
        expr c2{expr::cons{&inner3, &a1}};
        
        // Unifying cons(cons(V1, V2), a) with cons(cons(cons(V1, b), V2), a)
        // lhs: cons(V1, V2) with cons(cons(V1, b), V2)
        //   - lhs of lhs: V1 with cons(V1, b) - should fail occurs check
        assert(!bm.unify(&c1, &c2));
        assert(bm.bindings.size() == 0);  // Occurs check prevents binding
        
        t.pop();
    }
    
    // Test 86: Occurs check through equivalence class - cons(V1, V2) with cons(V2, cons(V1, a))
    {
        trail t;
        bind_map bm(t);
        t.push();
        expr v1{expr::var{156}};
        expr v2{expr::var{157}};
        expr v3{expr::var{157}};  // Same as V2
        expr v4{expr::var{156}};  // Same as V1
        expr a1{expr::atom{"a"}};
        
        expr c1{expr::cons{&v1, &v2}};
        
        // Build cons(V2, cons(V1, a))
        expr inner{expr::cons{&v4, &a1}};
        expr c2{expr::cons{&v3, &inner}};
        
        // Unifying cons(V1, V2) with cons(V2, cons(V1, a))
        // lhs: V1 with V2 - creates binding (either 156->157 or 157->156)
        // rhs: V2 with cons(V1, a)
        //   After lhs binding, V1 and V2 are in same equivalence class
        //   occurs_check(V2_index, cons(V1, a)) follows V1's binding via whnf()
        //   and correctly detects that V1 reduces to V2, thus V2 occurs in structure
        assert(!bm.unify(&c1, &c2));
        // Partial binding: one binding from lhs before rhs occurs check
        assert(bm.bindings.size() == 1);
        assert(bm.bindings.count(156) == 1 || bm.bindings.count(157) == 1);
        
        t.pop();
    }
}

// void test_constraint_id_pool_constructor() {
//     // Basic construction
//     constraint_id_pool pool1;
//     assert(pool1.size() == 0);
//     assert(pool1.constraint_ids.size() == 0);
//     assert(pool1.constraint_ids.empty());
    
//     // Multiple pools can be constructed independently
//     constraint_id_pool pool2;
//     constraint_id_pool pool3;
//     assert(pool2.size() == 0);
//     assert(pool3.size() == 0);
//     assert(pool2.constraint_ids.empty());
//     assert(pool3.constraint_ids.empty());
// }

// void test_constraint_id_pool_intern() {
//     // Test 1: Intern a simple constraint_id with nullptr parent
//     {
//         constraint_id_pool pool;
//         assert(pool.size() == 0);
        
//         constraint_id id1{nullptr, 1, 0};
//         const constraint_id* ptr1 = pool.intern(std::move(id1));
        
//         assert(ptr1 != nullptr);
//         assert(ptr1->parent == nullptr);
//         assert(ptr1->chosen_rule == 1);
//         assert(ptr1->body_index == 0);
//         assert(pool.size() == 1);
//         assert(pool.constraint_ids.size() == 1);
//         assert(pool.constraint_ids.count(*ptr1) == 1);
//     }
    
//     // Test 2: Intern a different constraint_id
//     {
//         constraint_id_pool pool;
        
//         constraint_id id2{nullptr, 2, 1};
//         const constraint_id* ptr2 = pool.intern(std::move(id2));
        
//         assert(ptr2 != nullptr);
//         assert(ptr2->parent == nullptr);
//         assert(ptr2->chosen_rule == 2);
//         assert(ptr2->body_index == 1);
//         assert(pool.size() == 1);
//         assert(pool.constraint_ids.size() == 1);
//         assert(pool.constraint_ids.count(*ptr2) == 1);
//     }
    
//     // Test 3: Intern duplicate - should return same pointer
//     {
//         constraint_id_pool pool;
        
//         constraint_id id1{nullptr, 1, 0};
//         const constraint_id* ptr1 = pool.intern(std::move(id1));
        
//         constraint_id id3{nullptr, 1, 0};  // Same as id1
//         const constraint_id* ptr3 = pool.intern(std::move(id3));
        
//         assert(ptr3 == ptr1);
//         assert(pool.size() == 1);  // No new entry added
//         assert(pool.constraint_ids.size() == 1);
//     }
    
//     // Test 4: Intern constraint_id with parent pointer
//     {
//         constraint_id_pool pool;
        
//         // Create parent
//         constraint_id id_parent{nullptr, 1, 0};
//         const constraint_id* parent = pool.intern(std::move(id_parent));
        
//         constraint_id id4{parent, 5, 2};
//         const constraint_id* ptr4 = pool.intern(std::move(id4));
        
//         assert(ptr4 != nullptr);
//         assert(ptr4->parent == parent);
//         assert(ptr4->chosen_rule == 5);
//         assert(ptr4->body_index == 2);
//         assert(pool.size() == 2);
//         assert(pool.constraint_ids.size() == 2);
//     }
    
//     // Test 5: Intern multiple with same parent
//     {
//         constraint_id_pool pool;
        
//         // Create parent
//         constraint_id id_parent{nullptr, 2, 1};
//         const constraint_id* parent = pool.intern(std::move(id_parent));
//         assert(pool.constraint_ids.size() == 1);
        
//         constraint_id id5{parent, 10, 0};
//         const constraint_id* ptr5 = pool.intern(std::move(id5));
//         assert(pool.size() == 2);
//         assert(pool.constraint_ids.size() == 2);
//         assert(ptr5->parent == parent);
//         assert(ptr5->chosen_rule == 10);
//         assert(ptr5->body_index == 0);
        
//         constraint_id id6{parent, 10, 1};
//         const constraint_id* ptr6 = pool.intern(std::move(id6));
//         assert(pool.size() == 3);
//         assert(pool.constraint_ids.size() == 3);
//         assert(ptr6->parent == parent);
//         assert(ptr6->chosen_rule == 10);
//         assert(ptr6->body_index == 1);
//         assert(ptr5 != ptr6);  // Different body_index means different constraint_ids
        
//         // Same parent, same rule, same index - duplicate
//         constraint_id id7{parent, 10, 0};
//         const constraint_id* ptr7 = pool.intern(std::move(id7));
//         assert(ptr7 == ptr5);  // Should be same pointer
//         assert(pool.size() == 3);  // No new entry
//         assert(pool.constraint_ids.size() == 3);
//     }
    
//     // Test 6: Different parent, same rule and index
//     {
//         constraint_id_pool pool;
        
//         // Create two different parents
//         constraint_id id_p1{nullptr, 1, 0};
//         const constraint_id* parent1 = pool.intern(std::move(id_p1));
        
//         constraint_id id_p2{nullptr, 2, 1};
//         const constraint_id* parent2 = pool.intern(std::move(id_p2));
//         assert(parent1 != parent2);
        
//         constraint_id id8{parent1, 99, 5};
//         const constraint_id* ptr8 = pool.intern(std::move(id8));
//         assert(pool.size() == 3);
//         assert(pool.constraint_ids.size() == 3);
//         assert(ptr8->parent == parent1);
//         assert(ptr8->chosen_rule == 99);
//         assert(ptr8->body_index == 5);
        
//         constraint_id id9{parent2, 99, 5};
//         const constraint_id* ptr9 = pool.intern(std::move(id9));
//         assert(ptr9 != ptr8);  // Different parents, so different constraint_ids
//         assert(pool.size() == 4);
//         assert(pool.constraint_ids.size() == 4);
//         assert(ptr9->parent == parent2);
//         assert(ptr9->chosen_rule == 99);
//         assert(ptr9->body_index == 5);
//     }
    
//     // Test 7: Chain of parents
//     {
//         constraint_id_pool pool;
        
//         constraint_id id1{nullptr, 1, 0};
//         const constraint_id* p1 = pool.intern(std::move(id1));
//         assert(pool.constraint_ids.size() == 1);
        
//         constraint_id id10{p1, 20, 0};
//         const constraint_id* p2 = pool.intern(std::move(id10));
//         assert(pool.size() == 2);
//         assert(pool.constraint_ids.size() == 2);
//         assert(p2->chosen_rule == 20);
        
//         constraint_id id11{p2, 21, 0};
//         const constraint_id* p3 = pool.intern(std::move(id11));
//         assert(pool.size() == 3);
//         assert(pool.constraint_ids.size() == 3);
//         assert(p3->chosen_rule == 21);
        
//         constraint_id id12{p3, 22, 0};
//         const constraint_id* p4 = pool.intern(std::move(id12));
//         assert(pool.size() == 4);
//         assert(pool.constraint_ids.size() == 4);
//         assert(p4->chosen_rule == 22);
        
//         // Verify the chain
//         assert(p4->parent == p3);
//         assert(p3->parent == p2);
//         assert(p2->parent == p1);
//         assert(p1->parent == nullptr);
//     }
    
//     // Test 8: Large body_index values
//     {
//         constraint_id_pool pool;
        
//         constraint_id id13{nullptr, 1000, 999};
//         const constraint_id* ptr13 = pool.intern(std::move(id13));
//         assert(ptr13->chosen_rule == 1000);
//         assert(ptr13->body_index == 999);
//         assert(ptr13->parent == nullptr);
//         assert(pool.size() == 1);
//         assert(pool.constraint_ids.size() == 1);
//         assert(pool.constraint_ids.count(*ptr13) == 1);
//     }
    
//     // Test 9: Zero values
//     {
//         constraint_id_pool pool;
        
//         constraint_id id14{nullptr, 0, 0};
//         const constraint_id* ptr14 = pool.intern(std::move(id14));
//         assert(ptr14->chosen_rule == 0);
//         assert(ptr14->body_index == 0);
//         assert(ptr14->parent == nullptr);
//         assert(pool.size() == 1);
//         assert(pool.constraint_ids.size() == 1);
//         assert(pool.constraint_ids.count(*ptr14) == 1);
//     }
    
//     // Test 10: Same parent, different rule, same body_index
//     {
//         constraint_id_pool pool;
        
//         constraint_id id_parent{nullptr, 100, 0};
//         const constraint_id* parent = pool.intern(std::move(id_parent));
        
//         constraint_id id1{parent, 10, 5};
//         const constraint_id* ptr1 = pool.intern(std::move(id1));
//         assert(pool.size() == 2);
        
//         constraint_id id2{parent, 20, 5};
//         const constraint_id* ptr2 = pool.intern(std::move(id2));
//         assert(pool.size() == 3);
//         assert(pool.constraint_ids.size() == 3);
        
//         // Different rules means different constraint_ids
//         assert(ptr1 != ptr2);
//         assert(ptr1->parent == parent);
//         assert(ptr2->parent == parent);
//         assert(ptr1->chosen_rule == 10);
//         assert(ptr2->chosen_rule == 20);
//         assert(ptr1->body_index == 5);
//         assert(ptr2->body_index == 5);
//     }
    
//     // Test 11: Multiple duplicates in sequence
//     {
//         constraint_id_pool pool;
        
//         constraint_id id1{nullptr, 42, 7};
//         const constraint_id* ptr1 = pool.intern(std::move(id1));
//         assert(pool.size() == 1);
        
//         // Intern the same thing multiple times
//         constraint_id id2{nullptr, 42, 7};
//         const constraint_id* ptr2 = pool.intern(std::move(id2));
//         assert(ptr2 == ptr1);
//         assert(pool.size() == 1);
        
//         constraint_id id3{nullptr, 42, 7};
//         const constraint_id* ptr3 = pool.intern(std::move(id3));
//         assert(ptr3 == ptr1);
//         assert(pool.size() == 1);
        
//         constraint_id id4{nullptr, 42, 7};
//         const constraint_id* ptr4 = pool.intern(std::move(id4));
//         assert(ptr4 == ptr1);
//         assert(pool.size() == 1);
//         assert(pool.constraint_ids.size() == 1);
//     }
    
//     // Test 12: Pointer stability - pointers remain valid after more insertions
//     {
//         constraint_id_pool pool;
        
//         constraint_id id1{nullptr, 1, 0};
//         const constraint_id* ptr1 = pool.intern(std::move(id1));
        
//         // Store values
//         const constraint_id* parent_stored = ptr1->parent;
//         rule_id rule_stored = ptr1->chosen_rule;
//         uint32_t index_stored = ptr1->body_index;
        
//         // Add many more entries
//         for (uint32_t i = 2; i < 50; ++i) {
//             constraint_id id{nullptr, i, i * 2};
//             pool.intern(std::move(id));
//         }
        
//         // Original pointer should still be valid with same values
//         assert(ptr1->parent == parent_stored);
//         assert(ptr1->chosen_rule == rule_stored);
//         assert(ptr1->body_index == index_stored);
//         assert(pool.constraint_ids.count(*ptr1) == 1);
//     }
// }

// void test_constraint_id_pool_fulfillment_child() {
//     // Test 1: Create root fulfillment child (nullptr parent)
//     {
//         constraint_id_pool pool;
//         assert(pool.size() == 0);
        
//         const constraint_id* child1 = pool.fulfillment_child(nullptr, 1, 0);
        
//         assert(child1 != nullptr);
//         assert(child1->parent == nullptr);
//         assert(child1->chosen_rule == 1);
//         assert(child1->body_index == 0);
//         assert(pool.size() == 1);
//         assert(pool.constraint_ids.size() == 1);
//         assert(pool.constraint_ids.count(*child1) == 1);
//     }
    
//     // Test 2: Create another root with different parameters
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* child2 = pool.fulfillment_child(nullptr, 5, 2);
        
//         assert(child2 != nullptr);
//         assert(child2->parent == nullptr);
//         assert(child2->chosen_rule == 5);
//         assert(child2->body_index == 2);
//         assert(pool.size() == 1);
//     }
    
//     // Test 3: Create duplicate root - should return same pointer
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* child1 = pool.fulfillment_child(nullptr, 1, 0);
//         assert(pool.size() == 1);
//         assert(pool.constraint_ids.size() == 1);
        
//         const constraint_id* child3 = pool.fulfillment_child(nullptr, 1, 0);
        
//         assert(child3 == child1);  // Should be interned to same pointer
//         assert(pool.size() == 1);  // No new entry
//         assert(pool.constraint_ids.size() == 1);
//     }
    
//     // Test 4: Create child with parent
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* parent = pool.fulfillment_child(nullptr, 10, 0);
//         assert(pool.size() == 1);
        
//         const constraint_id* child = pool.fulfillment_child(parent, 11, 1);
        
//         assert(child != nullptr);
//         assert(child->parent == parent);
//         assert(child->chosen_rule == 11);
//         assert(child->body_index == 1);
//         assert(pool.size() == 2);
//         assert(pool.constraint_ids.count(*child) == 1);
//     }
    
//     // Test 5: Create multiple children from same parent
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* parent = pool.fulfillment_child(nullptr, 20, 0);
//         assert(pool.size() == 1);
//         assert(pool.constraint_ids.size() == 1);
        
//         const constraint_id* child1 = pool.fulfillment_child(parent, 21, 0);
//         assert(pool.size() == 2);
//         assert(pool.constraint_ids.size() == 2);
//         assert(child1->parent == parent);
//         assert(child1->chosen_rule == 21);
//         assert(child1->body_index == 0);
        
//         const constraint_id* child2 = pool.fulfillment_child(parent, 21, 1);
//         assert(pool.size() == 3);
//         assert(pool.constraint_ids.size() == 3);
//         assert(child2->parent == parent);
//         assert(child2->chosen_rule == 21);
//         assert(child2->body_index == 1);
        
//         const constraint_id* child3 = pool.fulfillment_child(parent, 21, 2);
//         assert(pool.size() == 4);
//         assert(pool.constraint_ids.size() == 4);
//         assert(child3->parent == parent);
//         assert(child3->chosen_rule == 21);
//         assert(child3->body_index == 2);
        
//         // All children should be distinct
//         assert(child1 != child2);
//         assert(child2 != child3);
//         assert(child1 != child3);
//     }
    
//     // Test 6: Create duplicate child - should return same pointer
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* parent = pool.fulfillment_child(nullptr, 20, 0);
        
//         const constraint_id* original = pool.fulfillment_child(parent, 21, 1);
//         size_t size_before = pool.size();
//         assert(pool.constraint_ids.size() == size_before);
        
//         const constraint_id* duplicate = pool.fulfillment_child(parent, 21, 1);
        
//         assert(duplicate == original);  // Should be same pointer
//         assert(pool.size() == size_before);  // No new entry
//         assert(pool.constraint_ids.size() == size_before);
//     }
    
//     // Test 7: Create deep chain of fulfillments
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* level0 = pool.fulfillment_child(nullptr, 100, 0);
//         size_t size_after_level0 = pool.size();
//         assert(level0->chosen_rule == 100);
//         assert(level0->body_index == 0);
//         assert(pool.constraint_ids.size() == size_after_level0);
        
//         const constraint_id* level1 = pool.fulfillment_child(level0, 101, 0);
//         assert(pool.size() == size_after_level0 + 1);
//         assert(pool.constraint_ids.size() == size_after_level0 + 1);
//         assert(level1->parent == level0);
//         assert(level1->chosen_rule == 101);
        
//         const constraint_id* level2 = pool.fulfillment_child(level1, 102, 0);
//         assert(pool.size() == size_after_level0 + 2);
//         assert(pool.constraint_ids.size() == size_after_level0 + 2);
//         assert(level2->parent == level1);
//         assert(level2->chosen_rule == 102);
        
//         const constraint_id* level3 = pool.fulfillment_child(level2, 103, 0);
//         assert(pool.size() == size_after_level0 + 3);
//         assert(pool.constraint_ids.size() == size_after_level0 + 3);
//         assert(level3->parent == level2);
//         assert(level3->chosen_rule == 103);
        
//         const constraint_id* level4 = pool.fulfillment_child(level3, 104, 0);
//         assert(pool.size() == size_after_level0 + 4);
//         assert(pool.constraint_ids.size() == size_after_level0 + 4);
//         assert(level4->parent == level3);
//         assert(level4->chosen_rule == 104);
        
//         // Verify the full chain
//         assert(level4->parent->parent->parent->parent == level0);
//         assert(level0->parent == nullptr);
//     }
    
//     // Test 8: Different parents, same rule and body_index
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* parent1 = pool.fulfillment_child(nullptr, 200, 0);
//         const constraint_id* parent2 = pool.fulfillment_child(nullptr, 201, 0);
//         assert(parent1 != parent2);
//         size_t size_before = pool.size();
//         assert(pool.constraint_ids.size() == size_before);
        
//         const constraint_id* child1 = pool.fulfillment_child(parent1, 50, 5);
//         assert(pool.size() == size_before + 1);
//         assert(pool.constraint_ids.size() == size_before + 1);
        
//         const constraint_id* child2 = pool.fulfillment_child(parent2, 50, 5);
//         assert(pool.size() == size_before + 2);
//         assert(pool.constraint_ids.size() == size_before + 2);
        
//         // Different parents means different constraint_ids
//         assert(child1 != child2);
//         assert(child1->parent == parent1);
//         assert(child2->parent == parent2);
//         assert(child1->chosen_rule == child2->chosen_rule);
//         assert(child1->chosen_rule == 50);
//         assert(child1->body_index == child2->body_index);
//         assert(child1->body_index == 5);
//     }
    
//     // Test 9: Branching tree structure
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* root = pool.fulfillment_child(nullptr, 300, 0);
//         size_t size_after_root = pool.size();
        
//         // Create multiple branches from root
//         const constraint_id* branch1 = pool.fulfillment_child(root, 301, 0);
//         const constraint_id* branch2 = pool.fulfillment_child(root, 302, 0);
//         const constraint_id* branch3 = pool.fulfillment_child(root, 303, 0);
//         assert(pool.size() == size_after_root + 3);
        
//         // Extend each branch
//         const constraint_id* branch1_child = pool.fulfillment_child(branch1, 311, 0);
//         const constraint_id* branch2_child = pool.fulfillment_child(branch2, 312, 0);
//         const constraint_id* branch3_child = pool.fulfillment_child(branch3, 313, 0);
//         assert(pool.size() == size_after_root + 6);
        
//         // Verify all branches point to root
//         assert(branch1->parent == root);
//         assert(branch2->parent == root);
//         assert(branch3->parent == root);
        
//         // Verify all children point to their respective branches
//         assert(branch1_child->parent == branch1);
//         assert(branch2_child->parent == branch2);
//         assert(branch3_child->parent == branch3);
//     }
    
//     // Test 10: Same parent, same rule, different body_index
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* parent = pool.fulfillment_child(nullptr, 400, 0);
//         size_t size_before = pool.size();
        
//         const constraint_id* child0 = pool.fulfillment_child(parent, 401, 0);
//         const constraint_id* child1 = pool.fulfillment_child(parent, 401, 1);
//         const constraint_id* child2 = pool.fulfillment_child(parent, 401, 2);
//         const constraint_id* child3 = pool.fulfillment_child(parent, 401, 3);
        
//         assert(pool.size() == size_before + 4);
//         assert(pool.constraint_ids.size() == size_before + 4);
        
//         // All should be distinct
//         assert(child0 != child1);
//         assert(child0 != child2);
//         assert(child0 != child3);
//         assert(child1 != child2);
//         assert(child1 != child3);
//         assert(child2 != child3);
        
//         // All should have same parent and rule
//         assert(child0->parent == parent);
//         assert(child1->parent == parent);
//         assert(child2->parent == parent);
//         assert(child3->parent == parent);
//         assert(child0->chosen_rule == 401);
//         assert(child1->chosen_rule == 401);
//         assert(child2->chosen_rule == 401);
//         assert(child3->chosen_rule == 401);
        
//         // Verify body_index values
//         assert(child0->body_index == 0);
//         assert(child1->body_index == 1);
//         assert(child2->body_index == 2);
//         assert(child3->body_index == 3);
//     }
    
//     // Test 11: Verify all entries are in the set
//     {
//         constraint_id_pool pool;
        
//         // Add some entries
//         const constraint_id* c1 = pool.fulfillment_child(nullptr, 1, 0);
//         const constraint_id* c2 = pool.fulfillment_child(nullptr, 2, 0);
//         const constraint_id* c3 = pool.fulfillment_child(c1, 3, 0);
        
//         size_t final_size = pool.size();
//         assert(pool.constraint_ids.size() == final_size);
        
//         // Every pointer returned should be in the set
//         for (const auto& id : pool.constraint_ids) {
//             assert(pool.constraint_ids.count(id) == 1);
//         }
//     }
    
//     // Test 12: Edge case - maximum values
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* max_child = pool.fulfillment_child(nullptr, UINT32_MAX, UINT32_MAX);
//         assert(max_child->chosen_rule == UINT32_MAX);
//         assert(max_child->body_index == UINT32_MAX);
//         assert(max_child->parent == nullptr);
//         assert(pool.size() == 1);
//         assert(pool.constraint_ids.size() == 1);
//         assert(pool.constraint_ids.count(*max_child) == 1);
//     }
    
//     // Test 13: Same parent, different rule, same body_index
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* parent = pool.fulfillment_child(nullptr, 500, 0);
        
//         const constraint_id* child1 = pool.fulfillment_child(parent, 10, 7);
//         const constraint_id* child2 = pool.fulfillment_child(parent, 20, 7);
//         const constraint_id* child3 = pool.fulfillment_child(parent, 30, 7);
        
//         assert(pool.size() == 4);
//         assert(pool.constraint_ids.size() == 4);
        
//         // All should be distinct (different rules)
//         assert(child1 != child2);
//         assert(child2 != child3);
//         assert(child1 != child3);
        
//         // Verify all have same parent and body_index but different rules
//         assert(child1->parent == parent);
//         assert(child2->parent == parent);
//         assert(child3->parent == parent);
//         assert(child1->body_index == 7);
//         assert(child2->body_index == 7);
//         assert(child3->body_index == 7);
//         assert(child1->chosen_rule == 10);
//         assert(child2->chosen_rule == 20);
//         assert(child3->chosen_rule == 30);
//     }
    
//     // Test 14: Multiple duplicates in sequence
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* parent = pool.fulfillment_child(nullptr, 600, 0);
//         const constraint_id* first = pool.fulfillment_child(parent, 601, 5);
//         assert(pool.size() == 2);
        
//         // Call multiple times with same parameters
//         const constraint_id* dup1 = pool.fulfillment_child(parent, 601, 5);
//         assert(dup1 == first);
//         assert(pool.size() == 2);
        
//         const constraint_id* dup2 = pool.fulfillment_child(parent, 601, 5);
//         assert(dup2 == first);
//         assert(pool.size() == 2);
        
//         const constraint_id* dup3 = pool.fulfillment_child(parent, 601, 5);
//         assert(dup3 == first);
//         assert(pool.size() == 2);
//         assert(pool.constraint_ids.size() == 2);
//     }
    
//     // Test 15: Pointer stability across many insertions
//     {
//         constraint_id_pool pool;
        
//         const constraint_id* root = pool.fulfillment_child(nullptr, 1, 0);
//         const constraint_id* child = pool.fulfillment_child(root, 2, 0);
        
//         // Store the values
//         const constraint_id* root_parent = root->parent;
//         rule_id root_rule = root->chosen_rule;
//         uint32_t root_index = root->body_index;
//         const constraint_id* child_parent = child->parent;
//         rule_id child_rule = child->chosen_rule;
//         uint32_t child_index = child->body_index;
        
//         // Add many more entries
//         for (uint32_t i = 3; i < 100; ++i) {
//             pool.fulfillment_child(nullptr, i, i % 10);
//         }
        
//         // Original pointers should still be valid with same values
//         assert(root->parent == root_parent);
//         assert(root->chosen_rule == root_rule);
//         assert(root->body_index == root_index);
//         assert(child->parent == child_parent);
//         assert(child->chosen_rule == child_rule);
//         assert(child->body_index == child_index);
//         assert(child->parent == root);  // Relationship still valid
//     }
// }

void test_resolution_pool_constructor() {
    // Basic construction
    resolution_pool pool1;
    assert(pool1.size() == 0);
    assert(pool1.resolutions.size() == 0);
    assert(pool1.resolutions.empty());
}

void test_resolution_pool_intern() {
    // Test 1: Intern a simple resolution with nullptr parent (root)
    {
        resolution_pool pool;
        assert(pool.size() == 0);
        
        resolution r1{nullptr, 1, 10};
        const resolution* ptr1 = pool.intern(std::move(r1));
        
        assert(ptr1 != nullptr);
        assert(ptr1->parent == nullptr);
        assert(ptr1->chosen_subgoal == 1);
        assert(ptr1->chosen_rule == 10);
        assert(pool.size() == 1);
        assert(pool.resolutions.size() == 1);
        assert(pool.resolutions.count(*ptr1) == 1);
        assert(pool.resolutions.at(*ptr1) == false);  // Not pinned by default
    }
    
    // Test 2: Intern a different resolution
    {
        resolution_pool pool;
        
        resolution r2{nullptr, 2, 20};
        const resolution* ptr2 = pool.intern(std::move(r2));
        
        assert(ptr2 != nullptr);
        assert(ptr2->parent == nullptr);
        assert(ptr2->chosen_subgoal == 2);
        assert(ptr2->chosen_rule == 20);
        assert(pool.size() == 1);
        assert(pool.resolutions.size() == 1);
        assert(pool.resolutions.at(*ptr2) == false);
    }
    
    // Test 3: Intern duplicate - should return same pointer
    {
        resolution_pool pool;
        
        resolution r1{nullptr, 1, 10};
        const resolution* ptr1 = pool.intern(std::move(r1));
        
        resolution r3{nullptr, 1, 10};  // Same as r1
        const resolution* ptr3 = pool.intern(std::move(r3));
        
        assert(ptr3 == ptr1);
        assert(pool.size() == 1);  // No new entry added
        assert(pool.resolutions.size() == 1);
    }
    
    // Test 4: Intern resolution with parent pointer
    {
        resolution_pool pool;
        
        // Create parent
        resolution r_parent{nullptr, 1, 10};
        const resolution* parent = pool.intern(std::move(r_parent));
        assert(pool.resolutions.size() == 1);
        
        resolution r4{parent, 5, 50};
        const resolution* ptr4 = pool.intern(std::move(r4));
        
        assert(ptr4 != nullptr);
        assert(ptr4->parent == parent);
        assert(ptr4->chosen_subgoal == 5);
        assert(ptr4->chosen_rule == 50);
        assert(pool.size() == 2);
        assert(pool.resolutions.size() == 2);
        assert(pool.resolutions.at(*ptr4) == false);
    }
    
    // Test 5: Intern multiple with same parent
    {
        resolution_pool pool;
        
        // Create parent
        resolution r_parent{nullptr, 2, 20};
        const resolution* parent = pool.intern(std::move(r_parent));
        
        resolution r5{parent, 10, 100};
        const resolution* ptr5 = pool.intern(std::move(r5));
        assert(pool.size() == 2);
        assert(ptr5->parent == parent);
        assert(ptr5->chosen_subgoal == 10);
        assert(ptr5->chosen_rule == 100);
        
        resolution r6{parent, 10, 101};
        const resolution* ptr6 = pool.intern(std::move(r6));
        assert(pool.size() == 3);
        assert(ptr6->parent == parent);
        assert(ptr6->chosen_subgoal == 10);
        assert(ptr6->chosen_rule == 101);
        assert(ptr5 != ptr6);  // Different rule means different resolution
        
        // Same parent, same subgoal, same rule - duplicate
        resolution r7{parent, 10, 100};
        const resolution* ptr7 = pool.intern(std::move(r7));
        assert(ptr7 == ptr5);  // Should be same pointer
        assert(pool.size() == 3);  // No new entry
    }
    
    // Test 6: Different parent, same subgoal and rule
    {
        resolution_pool pool;
        
        // Create two different parents
        resolution r_p1{nullptr, 1, 10};
        const resolution* parent1 = pool.intern(std::move(r_p1));
        
        resolution r_p2{nullptr, 2, 20};
        const resolution* parent2 = pool.intern(std::move(r_p2));
        assert(parent1 != parent2);
        
        resolution r8{parent1, 99, 999};
        const resolution* ptr8 = pool.intern(std::move(r8));
        assert(pool.size() == 3);
        assert(ptr8->parent == parent1);
        
        resolution r9{parent2, 99, 999};
        const resolution* ptr9 = pool.intern(std::move(r9));
        assert(ptr9 != ptr8);  // Different parents, so different resolutions
        assert(pool.size() == 4);
        assert(ptr9->parent == parent2);
        assert(ptr9->chosen_subgoal == 99);
        assert(ptr9->chosen_rule == 999);
    }
    
    // Test 7: Chain of parents
    {
        resolution_pool pool;
        
        resolution r1{nullptr, 1, 10};
        const resolution* p1 = pool.intern(std::move(r1));
        assert(pool.resolutions.size() == 1);
        
        resolution r10{p1, 2, 20};
        const resolution* p2 = pool.intern(std::move(r10));
        assert(pool.size() == 2);
        assert(p2->chosen_subgoal == 2);
        
        resolution r11{p2, 3, 30};
        const resolution* p3 = pool.intern(std::move(r11));
        assert(pool.size() == 3);
        assert(p3->chosen_subgoal == 3);
        
        resolution r12{p3, 4, 40};
        const resolution* p4 = pool.intern(std::move(r12));
        assert(pool.size() == 4);
        assert(p4->chosen_subgoal == 4);
        
        // Verify the chain
        assert(p4->parent == p3);
        assert(p3->parent == p2);
        assert(p2->parent == p1);
        assert(p1->parent == nullptr);
    }
    
    // Test 8: Edge cases - maximum values
    {
        resolution_pool pool;
        
        resolution r13{nullptr, UINT32_MAX, UINT32_MAX};
        const resolution* ptr13 = pool.intern(std::move(r13));
        assert(ptr13->chosen_subgoal == UINT32_MAX);
        assert(ptr13->chosen_rule == UINT32_MAX);
        assert(ptr13->parent == nullptr);
        assert(pool.size() == 1);
        assert(pool.resolutions.at(*ptr13) == false);
    }
    
    // Test 9: Zero values
    {
        resolution_pool pool;
        
        resolution r14{nullptr, 0, 0};
        const resolution* ptr14 = pool.intern(std::move(r14));
        assert(ptr14->chosen_subgoal == 0);
        assert(ptr14->chosen_rule == 0);
        assert(ptr14->parent == nullptr);
        assert(pool.size() == 1);
        assert(pool.resolutions.at(*ptr14) == false);
    }
}

void test_resolution_pool_make_resolution() {
    // Test 1: Create root resolution (nullptr parent)
    {
        resolution_pool pool;
        assert(pool.size() == 0);
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        
        assert(r1 != nullptr);
        assert(r1->parent == nullptr);
        assert(r1->chosen_subgoal == 1);
        assert(r1->chosen_rule == 10);
        assert(pool.size() == 1);
        assert(pool.resolutions.size() == 1);
        assert(pool.resolutions.count(*r1) == 1);
        assert(pool.resolutions.at(*r1) == false);
    }
    
    // Test 2: Create another root with different parameters
    {
        resolution_pool pool;
        
        const resolution* r2 = pool.make_resolution(nullptr, 5, 50);
        
        assert(r2 != nullptr);
        assert(r2->parent == nullptr);
        assert(r2->chosen_subgoal == 5);
        assert(r2->chosen_rule == 50);
        assert(pool.size() == 1);
    }
    
    // Test 3: Create duplicate root - should return same pointer
    {
        resolution_pool pool;
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        const resolution* r3 = pool.make_resolution(nullptr, 1, 10);
        
        assert(r3 == r1);  // Should be interned to same pointer
        assert(pool.size() == 1);  // No new entry
        assert(pool.resolutions.size() == 1);
    }
    
    // Test 4: Create child with parent
    {
        resolution_pool pool;
        
        const resolution* parent = pool.make_resolution(nullptr, 10, 100);
        assert(pool.size() == 1);
        
        const resolution* child = pool.make_resolution(parent, 11, 110);
        
        assert(child != nullptr);
        assert(child->parent == parent);
        assert(child->chosen_subgoal == 11);
        assert(child->chosen_rule == 110);
        assert(pool.size() == 2);
        assert(pool.resolutions.count(*child) == 1);
    }
    
    // Test 5: Create multiple children from same parent
    {
        resolution_pool pool;
        
        const resolution* parent = pool.make_resolution(nullptr, 20, 200);
        assert(pool.size() == 1);
        
        const resolution* child1 = pool.make_resolution(parent, 21, 210);
        assert(pool.size() == 2);
        assert(child1->parent == parent);
        assert(child1->chosen_subgoal == 21);
        
        const resolution* child2 = pool.make_resolution(parent, 22, 220);
        assert(pool.size() == 3);
        assert(child2->parent == parent);
        assert(child2->chosen_subgoal == 22);
        
        const resolution* child3 = pool.make_resolution(parent, 23, 230);
        assert(pool.size() == 4);
        assert(child3->parent == parent);
        assert(child3->chosen_subgoal == 23);
        
        // All children should be distinct
        assert(child1 != child2);
        assert(child2 != child3);
        assert(child1 != child3);
    }
    
    // Test 6: Create duplicate child - should return same pointer
    {
        resolution_pool pool;
        
        const resolution* parent = pool.make_resolution(nullptr, 20, 200);
        const resolution* original = pool.make_resolution(parent, 21, 210);
        size_t size_before = pool.size();
        
        const resolution* duplicate = pool.make_resolution(parent, 21, 210);
        
        assert(duplicate == original);  // Should be same pointer
        assert(pool.size() == size_before);  // No new entry
    }
    
    // Test 7: Create deep chain of resolutions
    {
        resolution_pool pool;
        
        const resolution* level0 = pool.make_resolution(nullptr, 100, 1000);
        size_t size_after_level0 = pool.size();
        assert(level0->chosen_subgoal == 100);
        assert(pool.resolutions.size() == size_after_level0);
        
        const resolution* level1 = pool.make_resolution(level0, 101, 1010);
        assert(pool.size() == size_after_level0 + 1);
        assert(level1->parent == level0);
        assert(level1->chosen_subgoal == 101);
        
        const resolution* level2 = pool.make_resolution(level1, 102, 1020);
        assert(pool.size() == size_after_level0 + 2);
        assert(level2->parent == level1);
        
        const resolution* level3 = pool.make_resolution(level2, 103, 1030);
        assert(pool.size() == size_after_level0 + 3);
        assert(level3->parent == level2);
        
        const resolution* level4 = pool.make_resolution(level3, 104, 1040);
        assert(pool.size() == size_after_level0 + 4);
        assert(level4->parent == level3);
        
        // Verify the full chain
        assert(level4->parent->parent->parent->parent == level0);
        assert(level0->parent == nullptr);
    }
    
    // Test 8: Different parents, same subgoal and rule
    {
        resolution_pool pool;
        
        const resolution* parent1 = pool.make_resolution(nullptr, 200, 2000);
        const resolution* parent2 = pool.make_resolution(nullptr, 201, 2010);
        assert(parent1 != parent2);
        size_t size_before = pool.size();
        
        const resolution* child1 = pool.make_resolution(parent1, 50, 500);
        assert(pool.size() == size_before + 1);
        
        const resolution* child2 = pool.make_resolution(parent2, 50, 500);
        assert(pool.size() == size_before + 2);
        
        // Different parents means different resolutions
        assert(child1 != child2);
        assert(child1->parent == parent1);
        assert(child2->parent == parent2);
        assert(child1->chosen_subgoal == child2->chosen_subgoal);
        assert(child1->chosen_rule == child2->chosen_rule);
    }
    
    // Test 9: Branching tree structure
    {
        resolution_pool pool;
        
        const resolution* root = pool.make_resolution(nullptr, 300, 3000);
        size_t size_after_root = pool.size();
        
        // Create multiple branches from root
        const resolution* branch1 = pool.make_resolution(root, 301, 3010);
        const resolution* branch2 = pool.make_resolution(root, 302, 3020);
        const resolution* branch3 = pool.make_resolution(root, 303, 3030);
        assert(pool.size() == size_after_root + 3);
        
        // Extend each branch
        const resolution* branch1_child = pool.make_resolution(branch1, 311, 3110);
        const resolution* branch2_child = pool.make_resolution(branch2, 312, 3120);
        const resolution* branch3_child = pool.make_resolution(branch3, 313, 3130);
        assert(pool.size() == size_after_root + 6);
        
        // Verify all branches point to root
        assert(branch1->parent == root);
        assert(branch2->parent == root);
        assert(branch3->parent == root);
        
        // Verify all children point to their respective branches
        assert(branch1_child->parent == branch1);
        assert(branch2_child->parent == branch2);
        assert(branch3_child->parent == branch3);
    }
}

void test_resolution_pool_pin() {
    // Test 1: Pin nullptr (root) - should not crash
    {
        resolution_pool pool;
        
        pool.pin(nullptr);  // Should return immediately
        assert(pool.size() == 0);
    }
    
    // Test 2: Pin a single root resolution
    {
        resolution_pool pool;
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        assert(pool.resolutions.at(*r1) == false);
        
        pool.pin(r1);
        assert(pool.resolutions.at(*r1) == true);  // Now pinned
        assert(pool.size() == 1);
    }
    
    // Test 3: Pin the same resolution twice - should be idempotent
    {
        resolution_pool pool;
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        assert(pool.resolutions.at(*r1) == false);
        
        pool.pin(r1);
        assert(pool.resolutions.at(*r1) == true);
        
        pool.pin(r1);  // Pin again
        assert(pool.resolutions.at(*r1) == true);  // Still pinned
        assert(pool.size() == 1);
    }
    
    // Test 4: Pin a child - should pin parent chain to root
    {
        resolution_pool pool;
        
        const resolution* parent = pool.make_resolution(nullptr, 1, 10);
        const resolution* child = pool.make_resolution(parent, 2, 20);
        
        assert(pool.resolutions.at(*parent) == false);
        assert(pool.resolutions.at(*child) == false);
        
        pool.pin(child);
        
        // Both child and parent should be pinned
        assert(pool.resolutions.at(*child) == true);
        assert(pool.resolutions.at(*parent) == true);
    }
    
    // Test 5: Pin deep chain - all ancestors should be pinned
    {
        resolution_pool pool;
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        const resolution* r2 = pool.make_resolution(r1, 2, 20);
        const resolution* r3 = pool.make_resolution(r2, 3, 30);
        const resolution* r4 = pool.make_resolution(r3, 4, 40);
        const resolution* r5 = pool.make_resolution(r4, 5, 50);
        
        // All should be unpinned initially
        assert(pool.resolutions.at(*r1) == false);
        assert(pool.resolutions.at(*r2) == false);
        assert(pool.resolutions.at(*r3) == false);
        assert(pool.resolutions.at(*r4) == false);
        assert(pool.resolutions.at(*r5) == false);
        
        pool.pin(r5);
        
        // All should be pinned now
        assert(pool.resolutions.at(*r1) == true);
        assert(pool.resolutions.at(*r2) == true);
        assert(pool.resolutions.at(*r3) == true);
        assert(pool.resolutions.at(*r4) == true);
        assert(pool.resolutions.at(*r5) == true);
    }
    
    // Test 6: Pin multiple branches - shared ancestors pinned once
    {
        resolution_pool pool;
        
        const resolution* root = pool.make_resolution(nullptr, 1, 10);
        const resolution* branch1 = pool.make_resolution(root, 2, 20);
        const resolution* branch2 = pool.make_resolution(root, 3, 30);
        const resolution* leaf1 = pool.make_resolution(branch1, 4, 40);
        const resolution* leaf2 = pool.make_resolution(branch2, 5, 50);
        
        pool.pin(leaf1);
        
        // leaf1, branch1, and root should be pinned
        assert(pool.resolutions.at(*root) == true);
        assert(pool.resolutions.at(*branch1) == true);
        assert(pool.resolutions.at(*leaf1) == true);
        // branch2 and leaf2 should still be unpinned
        assert(pool.resolutions.at(*branch2) == false);
        assert(pool.resolutions.at(*leaf2) == false);
        
        pool.pin(leaf2);
        
        // Now everything should be pinned
        assert(pool.resolutions.at(*root) == true);
        assert(pool.resolutions.at(*branch1) == true);
        assert(pool.resolutions.at(*branch2) == true);
        assert(pool.resolutions.at(*leaf1) == true);
        assert(pool.resolutions.at(*leaf2) == true);
    }
    
    // Test 7: Pin parent after child - parent should already be pinned
    {
        resolution_pool pool;
        
        const resolution* parent = pool.make_resolution(nullptr, 1, 10);
        const resolution* child = pool.make_resolution(parent, 2, 20);
        
        pool.pin(child);
        assert(pool.resolutions.at(*parent) == true);
        assert(pool.resolutions.at(*child) == true);
        
        // Pin parent again - should be no-op since already pinned
        pool.pin(parent);
        assert(pool.resolutions.at(*parent) == true);
        assert(pool.size() == 2);
    }
    
    // Test 8: Pin sibling nodes independently
    {
        resolution_pool pool;
        
        const resolution* parent = pool.make_resolution(nullptr, 1, 10);
        const resolution* child1 = pool.make_resolution(parent, 2, 20);
        const resolution* child2 = pool.make_resolution(parent, 3, 30);
        const resolution* child3 = pool.make_resolution(parent, 4, 40);
        
        // Pin only child1
        pool.pin(child1);
        assert(pool.resolutions.at(*parent) == true);
        assert(pool.resolutions.at(*child1) == true);
        assert(pool.resolutions.at(*child2) == false);
        assert(pool.resolutions.at(*child3) == false);
        
        // Pin child3
        pool.pin(child3);
        assert(pool.resolutions.at(*parent) == true);
        assert(pool.resolutions.at(*child1) == true);
        assert(pool.resolutions.at(*child2) == false);  // Still unpinned
        assert(pool.resolutions.at(*child3) == true);
    }
    
    // Test 9: Complex tree with multiple levels
    {
        resolution_pool pool;
        
        const resolution* root = pool.make_resolution(nullptr, 0, 0);
        const resolution* l1_a = pool.make_resolution(root, 1, 10);
        const resolution* l1_b = pool.make_resolution(root, 2, 20);
        const resolution* l2_a = pool.make_resolution(l1_a, 3, 30);
        const resolution* l2_b = pool.make_resolution(l1_a, 4, 40);
        const resolution* l2_c = pool.make_resolution(l1_b, 5, 50);
        const resolution* l3_a = pool.make_resolution(l2_a, 6, 60);
        
        // Pin l3_a - should pin l3_a, l2_a, l1_a, root
        pool.pin(l3_a);
        assert(pool.resolutions.at(*root) == true);
        assert(pool.resolutions.at(*l1_a) == true);
        assert(pool.resolutions.at(*l1_b) == false);
        assert(pool.resolutions.at(*l2_a) == true);
        assert(pool.resolutions.at(*l2_b) == false);
        assert(pool.resolutions.at(*l2_c) == false);
        assert(pool.resolutions.at(*l3_a) == true);
    }
    
    // Test 10: Pin parent does NOT pin children
    {
        resolution_pool pool;
        
        const resolution* parent = pool.make_resolution(nullptr, 1, 10);
        const resolution* child1 = pool.make_resolution(parent, 2, 20);
        const resolution* child2 = pool.make_resolution(parent, 3, 30);
        const resolution* grandchild = pool.make_resolution(child1, 4, 40);
        assert(pool.size() == 4);
        
        // All should be unpinned initially
        assert(pool.resolutions.at(*parent) == false);
        assert(pool.resolutions.at(*child1) == false);
        assert(pool.resolutions.at(*child2) == false);
        assert(pool.resolutions.at(*grandchild) == false);
        
        // Pin only the parent
        pool.pin(parent);
        
        // Only parent should be pinned, children should remain unpinned
        assert(pool.resolutions.at(*parent) == true);
        assert(pool.resolutions.at(*child1) == false);
        assert(pool.resolutions.at(*child2) == false);
        assert(pool.resolutions.at(*grandchild) == false);
        assert(pool.size() == 4);  // Size unchanged by pin
    }
}

void test_resolution_pool_trim() {
    // Test 1: Trim empty pool - should not crash
    {
        resolution_pool pool;
        
        pool.trim();
        assert(pool.size() == 0);
    }
    
    // Test 2: Trim pool with all unpinned entries - should remove all
    {
        resolution_pool pool;
        
        pool.make_resolution(nullptr, 1, 10);
        pool.make_resolution(nullptr, 2, 20);
        pool.make_resolution(nullptr, 3, 30);
        assert(pool.size() == 3);
        
        pool.trim();
        assert(pool.size() == 0);
        assert(pool.resolutions.empty());
    }
    
    // Test 3: Trim pool with all pinned entries - should remove none
    {
        resolution_pool pool;
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        const resolution* r2 = pool.make_resolution(nullptr, 2, 20);
        const resolution* r3 = pool.make_resolution(nullptr, 3, 30);
        assert(pool.size() == 3);
        
        pool.pin(r1);
        pool.pin(r2);
        pool.pin(r3);
        
        pool.trim();
        assert(pool.size() == 3);
        assert(pool.resolutions.count(*r1) == 1);
        assert(pool.resolutions.count(*r2) == 1);
        assert(pool.resolutions.count(*r3) == 1);
    }
    
    // Test 4: Trim pool with mixed pinned/unpinned - remove only unpinned
    {
        resolution_pool pool;
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        const resolution* r2 = pool.make_resolution(nullptr, 2, 20);
        const resolution* r3 = pool.make_resolution(nullptr, 3, 30);
        assert(pool.size() == 3);
        
        pool.pin(r2);  // Pin only r2
        
        pool.trim();
        assert(pool.size() == 1);  // Only r2 remains
        assert(pool.resolutions.count(*r2) == 1);
        assert(pool.resolutions.at(*r2) == true);
    }
    
    // Test 5: Trim preserves pinned chain
    {
        resolution_pool pool;
        
        const resolution* parent = pool.make_resolution(nullptr, 1, 10);
        const resolution* child = pool.make_resolution(parent, 2, 20);
        const resolution* unrelated = pool.make_resolution(nullptr, 3, 30);
        assert(pool.size() == 3);
        
        pool.pin(child);  // This pins both child and parent
        
        pool.trim();
        assert(pool.size() == 2);  // parent and child remain
        assert(pool.resolutions.count(*parent) == 1);
        assert(pool.resolutions.count(*child) == 1);
        assert(pool.resolutions.at(*parent) == true);
        assert(pool.resolutions.at(*child) == true);
    }
    
    // Test 6: Trim with branching structure
    {
        resolution_pool pool;
        
        const resolution* root = pool.make_resolution(nullptr, 1, 10);
        const resolution* branch1 = pool.make_resolution(root, 2, 20);
        const resolution* branch2 = pool.make_resolution(root, 3, 30);
        const resolution* leaf1 = pool.make_resolution(branch1, 4, 40);
        const resolution* leaf2 = pool.make_resolution(branch2, 5, 50);
        assert(pool.size() == 5);
        
        pool.pin(leaf1);  // Pins leaf1, branch1, root
        
        pool.trim();
        assert(pool.size() == 3);  // root, branch1, leaf1 remain
        assert(pool.resolutions.count(*root) == 1);
        assert(pool.resolutions.count(*branch1) == 1);
        assert(pool.resolutions.count(*leaf1) == 1);
    }
    
    // Test 7: Multiple trims in sequence
    {
        resolution_pool pool;
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        const resolution* r2 = pool.make_resolution(nullptr, 2, 20);
        assert(pool.size() == 2);
        
        pool.pin(r1);
        pool.trim();
        assert(pool.size() == 1);
        
        // Add more entries
        const resolution* r3 = pool.make_resolution(nullptr, 3, 30);
        const resolution* r4 = pool.make_resolution(nullptr, 4, 40);
        assert(pool.size() == 3);
        
        pool.trim();  // r3 and r4 should be removed
        assert(pool.size() == 1);
        assert(pool.resolutions.count(*r1) == 1);
    }
    
    // // Test 8: Trim after unpinning is not possible (no unpin function)
    // // This test verifies that once pinned, entries stay pinned
    // {
    //     resolution_pool pool;
        
    //     const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
    //     pool.pin(r1);
    //     assert(pool.resolutions.at(*r1) == true);
        
    //     pool.trim();
    //     assert(pool.size() == 1);  // r1 remains
        
    //     // Trim again - r1 should still be there
    //     pool.trim();
    //     assert(pool.size() == 1);
    // }
    
    // Test 9: Complex scenario with multiple pin operations and trim
    {
        resolution_pool pool;
        
        // Create a tree structure
        const resolution* root = pool.make_resolution(nullptr, 0, 0);
        const resolution* l1_a = pool.make_resolution(root, 1, 10);
        const resolution* l1_b = pool.make_resolution(root, 2, 20);
        const resolution* l2_a = pool.make_resolution(l1_a, 3, 30);
        const resolution* l2_b = pool.make_resolution(l1_a, 4, 40);
        const resolution* l2_c = pool.make_resolution(l1_b, 5, 50);
        const resolution* l3_a = pool.make_resolution(l2_a, 6, 60);
        assert(pool.size() == 7);
        
        // Pin only l3_a
        pool.pin(l3_a);
        
        pool.trim();
        // Should keep: root, l1_a, l2_a, l3_a (the chain)
        // Should remove: l1_b, l2_b, l2_c
        assert(pool.size() == 4);
        assert(pool.resolutions.count(*root) == 1);
        assert(pool.resolutions.count(*l1_a) == 1);
        assert(pool.resolutions.count(*l2_a) == 1);
        assert(pool.resolutions.count(*l3_a) == 1);
    }
    
    // Test 10: Trim with multiple pinned branches
    {
        resolution_pool pool;
        
        const resolution* root = pool.make_resolution(nullptr, 0, 0);
        const resolution* b1 = pool.make_resolution(root, 1, 10);
        const resolution* b2 = pool.make_resolution(root, 2, 20);
        const resolution* b3 = pool.make_resolution(root, 3, 30);
        const resolution* b1_child = pool.make_resolution(b1, 4, 40);
        const resolution* b3_child = pool.make_resolution(b3, 5, 50);
        assert(pool.size() == 6);
        
        pool.pin(b1_child);  // Pins b1_child, b1, root
        pool.pin(b3_child);  // Pins b3_child, b3, root (root already pinned)
        
        pool.trim();
        // Should keep: root, b1, b3, b1_child, b3_child
        // Should remove: b2
        assert(pool.size() == 5);
        assert(pool.resolutions.count(*root) == 1);
        assert(pool.resolutions.count(*b1) == 1);
        assert(pool.resolutions.count(*b3) == 1);
        assert(pool.resolutions.count(*b1_child) == 1);
        assert(pool.resolutions.count(*b3_child) == 1);
    }
    
    // Test 11: Verify pointers remain valid after trim
    {
        resolution_pool pool;
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        const resolution* r2 = pool.make_resolution(r1, 2, 20);
        
        // Store values before trim
        const resolution* r1_parent = r1->parent;
        subgoal_id r1_subgoal = r1->chosen_subgoal;
        rule_id r1_rule = r1->chosen_rule;
        const resolution* r2_parent = r2->parent;
        subgoal_id r2_subgoal = r2->chosen_subgoal;
        rule_id r2_rule = r2->chosen_rule;
        
        pool.pin(r2);
        pool.trim();
        
        // Pointers should still be valid with same values
        assert(r1->parent == r1_parent);
        assert(r1->chosen_subgoal == r1_subgoal);
        assert(r1->chosen_rule == r1_rule);
        assert(r2->parent == r2_parent);
        assert(r2->chosen_subgoal == r2_subgoal);
        assert(r2->chosen_rule == r2_rule);
        assert(r2->parent == r1);
    }
}

void test_resolution_pool_size() {
    // Test 1: Size of empty pool
    {
        resolution_pool pool;
        assert(pool.size() == 0);
        assert(pool.resolutions.size() == 0);
    }
    
    // Test 2: Size increases with each unique entry
    {
        resolution_pool pool;
        
        pool.make_resolution(nullptr, 1, 10);
        assert(pool.size() == 1);
        assert(pool.resolutions.size() == 1);
        
        pool.make_resolution(nullptr, 2, 20);
        assert(pool.size() == 2);
        assert(pool.resolutions.size() == 2);
        
        pool.make_resolution(nullptr, 3, 30);
        assert(pool.size() == 3);
        assert(pool.resolutions.size() == 3);
    }
    
    // Test 3: Size doesn't increase for duplicates
    {
        resolution_pool pool;
        
        const resolution* r1 = pool.make_resolution(nullptr, 1, 10);
        assert(pool.size() == 1);
        
        const resolution* r2 = pool.make_resolution(nullptr, 1, 10);
        assert(r1 == r2);
        assert(pool.size() == 1);  // No increase
    }
    
    // Test 4: Size decreases after trim
    {
        resolution_pool pool;
        
        pool.make_resolution(nullptr, 1, 10);
        pool.make_resolution(nullptr, 2, 20);
        const resolution* r3 = pool.make_resolution(nullptr, 3, 30);
        assert(pool.size() == 3);
        
        pool.pin(r3);
        pool.trim();
        assert(pool.size() == 1);
        assert(pool.resolutions.size() == 1);
    }
    
    // Test 5: Size consistency through pin and trim operations
    {
        resolution_pool pool;
        
        const resolution* root = pool.make_resolution(nullptr, 0, 0);
        const resolution* child1 = pool.make_resolution(root, 1, 10);
        const resolution* child2 = pool.make_resolution(root, 2, 20);
        assert(pool.size() == 3);
        
        pool.pin(child1);
        assert(pool.size() == 3);  // Pin doesn't change size
        
        pool.trim();
        assert(pool.size() == 2);  // root and child1 remain
        assert(pool.resolutions.size() == 2);
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
    TEST(test_bind_map_bind);
    TEST(test_bind_map_whnf);
    TEST(test_bind_map_occurs_check);
    TEST(test_bind_map_unify);
    TEST(test_resolution_pool_constructor);
    TEST(test_resolution_pool_intern);
    TEST(test_resolution_pool_make_resolution);
    TEST(test_resolution_pool_pin);
    TEST(test_resolution_pool_trim);
    TEST(test_resolution_pool_size);
}

int main() {
    unit_test_main();
    return 0;
}
