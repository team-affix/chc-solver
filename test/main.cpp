#include "../hpp/expr.hpp"
#include <iostream>
#include <list>
#include "test_utils.hpp"

void test_atom_constructor() {
    // Basic construction with normal string
    atom a1("hello");
    
    // Empty string
    atom a2("");
    
    // String with special characters
    atom a3("test_123");
    atom a4("with spaces");
    atom a5("special!@#$%");
    
    // Long string
    atom a6("this_is_a_very_long_string_that_might_test_memory_allocation");
    
    std::cout << "  All atom constructor tests passed!" << std::endl;
}

void test_var_constructor() {
    // Basic construction with various indices
    var v1(0);
    var v2(1);
    var v3(100);
    
    // Edge cases
    var v4(UINT32_MAX);  // Maximum value
    
    std::cout << "  All var constructor tests passed!" << std::endl;
}

void unit_test_main() {
    constexpr bool ENABLE_DEBUG_LOGS = true;

    // test cases
    TEST(test_atom_constructor);
    TEST(test_var_constructor);
}

int main() {
    unit_test_main();
    return 0;
}

