# Skope Coding Conventions

Skope follows strict coding conventions to keep the codebase simple, explicit, and maintainable.

Every contributor, including the project lead, MUST follow these rules. Pull requests that violate them may be rejected.

Rules are divided into three categories: **BANNED**, **USE WITH CAUTION**, and **RECOMMENDED**.

## 1. BANNED Practices

1. **Defining macros:** Do not define macros for constants, functions, or abstractions. Necessary directives such as `#include` are permitted.
2. **Using `goto`:** Do not use `goto`, including for cleanup.
3. **Replicating OOP:** Avoid recreating classes, inheritance, or elaborate object systems in C.
4. **Misusing `errno`:** Use Skope's `Result` model for application errors and `errno` only where appropriate for filesystem operations.
5. **Ignoring return values:** Check function results or explicitly document why a result is intentionally unused.
6. **Ignoring ownership:** Understand the ownership and lifetime of memory and resources. Avoid leaks, dangling pointers, and use-after-free.
7. **Inconsistent naming:** Use `snake_case` for functions and variables, `PascalCase` for structs and enums, and `UPPER_SNAKE_CASE` for constants.
8. **Useless abstractions:** Do not add abstractions without a clear, meaningful benefit.
9. **Unexplained commits:** Every commit MUST explain why the change is needed and how it addresses the problem.
10. **Spaghetti code:** Avoid tangled control flow, hidden side effects, and flawed or unnecessarily complex logic.

## 2. USE WITH CAUTION

1. **`typedef`:** Appropriate for structs and enums, but avoid unnecessary aliases such as `String` for `char *` or `i64` for `int64_t`.
2. **Dependencies:** New dependencies require justification and maintainer discussion.
3. **Generic pointers:** Use `void *` only when necessary, with clear type and ownership expectations.
4. **Function pointers:** Use when they provide a meaningful benefit; explain their purpose in the commit or pull request.
5. **Preprocessor:** Use only when necessary. Do not define macros.

## 3. RECOMMENDED Practices

1. **Naming:** Follow the naming conventions consistently throughout the codebase.
2. **SQL schema:** Store schema SQL statements in named constants using `UPPER_SNAKE_CASE`.
3. **Unix philosophy:** Each function and module SHOULD do one thing and do it well.
4. **Loose coupling:** Minimize unnecessary dependencies between modules and keep interfaces clear.
5. **Comments:** Explain complex logic, non-obvious decisions, and important invariants. Prefer explaining *why* over repeating *what* the code does.

## 4. Applying These Rules

These conventions apply to all new code and modifications. Discuss proposed exceptions with maintainers before implementation.
