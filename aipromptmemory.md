# AI Prompt Memory - Important Rules and Guidelines

## File Creation

- **NEVER create files without explicit permission**
- Do not create documentation files, bug reports, or any other files "willy nilly"
- Only create files when the user explicitly asks for them

## Testing Guidelines

- Go one function at a time when testing
- After writing each test, RUN make, then RUN the program (./main)
- Use the TEST() macro from test_utils.hpp - do NOT add "all tests passed" print statements
- Tests should use assert() extensively to verify behavior
- Think of corner cases that could cause functions to fail
- Do NOT create separate test cases for edge cases that should be covered in the main tests
- Test functions in the SAME ORDER as they are declared in the header file

## Code Style

- Users construct expressions naturally, NOT with explicit variant construction:
  - Use: `expr e1{atom{"test"}}`
  - NOT: `expr e1{std::variant<atom, cons, var>{atom{"test"}}}`
- Use inline construction for deeply nested structures:
  ```cpp
  expr e{cons{
      expr{cons{expr{atom{"a"}}, expr{atom{"b"}}}},
      expr{var{1}}
  }};
  ```

## Design Principles

- All expression structures (atom, var, cons, expr) are immutable once constructed
- cons pointers (m_lhs, m_rhs) should always be non-null due to the restrictive interface
- The interface only allows construction with references, not nullable pointers
- Moved-from cons objects are invalid and should not be used (standard C++ behavior)

## Bug Finding and Testing

- Actively think about common bugs that happen with certain function types:
  - Copy constructors: self-assignment, deep vs shallow copy, resource leaks
  - Move constructors: self-move, leaving valid state, double-free
  - Assignment operators: self-assignment, resource cleanup before assignment
  - Destructors: double-free, resource leaks
  - Getters with pointers: nullptr dereference
  - Functions with unique_ptr: moved-from state, nullptr handling
  - Any other code pattern that might present known issues (etc.)
- Consider the architecture of the program and test for architectural bugs
- When a bug is found: **STOP TESTING IMMEDIATELY**
- **DO NOT CORRECT THE BUG** - report it to the user and let them fix it
- Tell the user which test found the bug and what the bug is

## Communication

- Do not be overly verbose or use unnecessary superlatives
- Get straight to the point
- When asked to find bugs, analyze the code thoroughly and report findings concisely
