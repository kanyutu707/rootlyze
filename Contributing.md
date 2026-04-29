# Contributing to rootlyze

Thank you for your interest in contributing to rootlyze. This document explains how to get your changes reviewed and merged.

---

## Getting Started

### 1. Fork and Clone

Fork the repository on GitHub, then clone your fork locally:

```bash
git clone https://github.com/<your-username>/rootlyze.git
cd rootlyze
```

Add the upstream remote so you can stay in sync:

```bash
git remote add upstream https://github.com/kanyutu707/rootlyze.git
```

### 2. Create a Branch

**Never commit directly to `main`.** All contributions must come in via a branch.

Name your branch after what it does:

```bash
git checkout -b feat/vector-doc-index
git checkout -b fix/context-builder-null-trace
git checkout -b docs/research-engine-config
```

Branch naming conventions:

| Prefix | Use for |
|--------|---------|
| `feat/` | New features or pipeline stages |
| `fix/` | Bug fixes |
| `refactor/` | Code restructuring without behaviour change |
| `docs/` | Documentation updates |
| `test/` | Test additions or fixes |
| `chore/` | Build, config, or dependency changes |

---

## Making Changes

### Build First

Make sure the project builds cleanly before you start:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

All changes must build without warnings under `-Wall -Wextra`.

### Follow the Architecture

rootlyze has a strict component model. Before writing code, read the relevant section of the architecture documentation for the component you are changing.

The key rules:

- Each pipeline stage has exactly one responsibility. Do not add logic to a stage that belongs in another.
- Components communicate via structs passed through `PipelineQueue<T>`. Do not introduce shared mutable state between stages.
- Use the defined interfaces (`IngestorInterface`, `DocumentIndexInterface`, `DiagnosisEngineInterface`, `ResearchEngineInterface`) when adding new backends. Do not reach around them.
- New patterns for the processing engine belong in the rules configuration directory, not in source code.

### Code Style

- C++17 throughout. No C++20 features until the project baseline is updated.
- Prefer move semantics over copies for structs passing through the pipeline.
- Use `std::optional<T>` for values that may be absent. Do not use sentinel values or null pointers for optional fields.
- Error handling follows the project's layered strategy: recoverable per-entry errors produce warnings and continue; fatal startup errors terminate immediately with a clear message.
- No heap allocation in the hot path unless it is justified and documented.

---

## Testing

**Tests are required for all new components.** PRs that add or modify pipeline components without corresponding tests will not be merged.

Run the test suite before pushing:

```bash
cd build
cmake --build .
ctest --output-on-failure
```

What to test:

- Unit tests for each component in isolation using mock queues
- Edge cases: malformed input, missing optional fields, empty queues, timeout conditions
- Research engine tests must cover the degradation path (external research unavailable)
- If you are adding a new diagnosis rule, include a log fixture that triggers it and verify the expected `DiagnosisResult`

Test files live in `tests/`. Follow the existing structure, one test file per component.

---

## Submitting Your Changes

### 1. Sync with Upstream

Before pushing, make sure your branch is up to date:

```bash
git fetch upstream
git rebase upstream/main
```

### 2. Push Your Branch

```bash
git push origin feat/your-branch-name
```

### 3. Open a Pull Request

Open a pull request against `main` on the upstream repository. In the PR description, include:

- **What** the change does
- **Why** it is needed (link the relevant issue if one exists)
- **Which component(s)** are affected
- **How** you tested it
- Any known limitations or follow-up work

### 4. Wait for Review

@kanyutu707 will review and merge approved changes. Do not merge your own PR. If changes are requested, push additional commits to the same branch rather than opening a new PR.

---

## Opening Issues

Before submitting a PR for a non-trivial change, open an issue first. Describe what you want to change and why. This avoids situations where significant work is done in a direction that does not align with the project roadmap.

Bug reports should include a minimal log input that reproduces the problem and the actual vs expected output.

---

## Roadmap Alignment

The project follows a phased roadmap. Contributions that jump ahead of the current phase are welcome as long as they do not break in-progress work and the interfaces they depend on are already defined.

If your contribution is scoped to a future phase, note that clearly in the PR description.

---

## Licence

By contributing to rootlyze, you agree that your contributions will be licensed under the MIT licence that covers this project.