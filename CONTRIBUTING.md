# Contributing to Skope

Thank you for your interest in contributing to Skope!

Skope aims to remain simple, explicit, and maintainable. Every contribution should support these goals.

## Before Contributing

- Read [`CONVENTIONS.md`](CONVENTIONS.md) and follow its rules.
- For significant changes, open an issue or discuss the proposal with the maintainers before implementation.
- Bug fixes and focused improvements are welcome, provided they follow the project's conventions.

## Making Changes

- Keep changes focused and avoid unrelated refactoring.
- Explain the reasoning behind non-trivial implementation decisions.
- Update relevant documentation when behavior or architecture changes.
- Add or update tests when introducing behavior or fixing bugs.
- Do not introduce new dependencies without discussing them with the maintainers.

## Commit Messages

Write clear commit messages that explain why a change is needed and what it accomplishes.

Follow [Conventional Commits](https://www.conventionalcommits.org/) where practical.

Examples:

```text
feat(scope): support nested scopes
fix(database): handle duplicate scope names
refactor!: remove the Wiki abstraction
docs: clarify coding conventions
```

For significant changes, include additional context in the commit body. Reference related issues when applicable.

## Pull Requests

Before submitting a pull request:

- Ensure your changes follow `CONVENTIONS.md`.
- Build the project and run relevant tests.
- Describe the problem, your solution, and any important trade-offs.
- Link related issues.
- Keep the pull request focused and reviewable.

Pull requests that violate project conventions may be requested to change before they can be merged.

## Reporting Bugs and Proposing Features

When reporting a bug, include steps to reproduce it, expected behavior, actual behavior, and relevant error messages.

When proposing a feature, explain the problem it solves, why it belongs in Skope, and your proposed approach.

Significant features and architectural changes should be discussed with the maintainers before implementation.

## Code of Conduct

Be respectful, constructive, and open to feedback. Technical disagreements should focus on the code, design, and reasoning rather than individuals.
