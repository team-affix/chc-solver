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
- When a test throws an error:
  - Analyze why the error was thrown
  - If you suspect it was due to the test case being written wrong, fix the test case automatically
  - If a bug in the core source code is highly likely, **STOP TESTING IMMEDIATELY** and report it to the user
- **DO NOT CORRECT BUGS IN CORE SOURCE CODE** - report them to the user and let them fix it

### Assertive Testing Philosophy

- **Don't be afraid to assert what you think should be true, even if you're unsure**
- If you have an idea about what the state of something should be mid-test, make an assertion for it
- When an assertion fails:
  1. Check if the failure makes sense given the system's actual behavior
  2. If the behavior is correct, adjust the assertion to match reality
  3. This process is VALUABLE because it forces you to confront and justify the actual behavior
- **Benefits of this approach:**
  - Forces deep understanding of the system by vetting assumptions
  - Even "wrong" initial assertions lead to correct final assertions
  - The corrected assertions serve as regression tests for the future
  - Comprehensive assertions catch bugs that might otherwise go unnoticed
- **Example:** When testing `expr_pool` with nested frames, assert `pool.exprs.size()` after every operation, even if you're not 100% sure what the size should be. If it fails, investigate why, then fix the assertion to match the correct behavior.

## Communication

- Do not be overly verbose or use unnecessary superlatives
- Get straight to the point
- When asked to find bugs, analyze the code thoroughly and report findings concisely
