# Copilot — "milendall" repository instructions

These rules guide Copilot (Chat, Inline, PR) when working in this repository. They cover response style, code conventions, tooling, build/test flows, and project specifics.

## Scope and technologies

- Primary languages:
  - C++ (CMake/Ninja) in `game/src`, binaries and libraries under `build/`.
  - Python in `generator/pygenerator` (generator, tests, tools).
- Data and rendering: glTF, textures, levels under `data/` and `levels/`.
- OS: Linux; default shell: bash.

## Response style

- Tone: very concise; avoid verbosity.

## Code editing rules

- Prefer minimal changes that satisfy the request.
- Add new files in appropriate locations (see conventions below).
- For all changes, iterate until tests pass, or you need to ask a specific requirement that was not provided.

## Build and execution

- C++:
   - clean and rebuild: "ninja -C $PWD/build clean all"
   - incremental rebuild: "ninja -C $PWD/build clean all"

- Python:
  - Use the repo venv: `.venv`; interpreter is `./.venv/bin/python`.
  - Dependency management via `pyproject.toml` + `pip-tools` (`pip-compile`) to generate a pinned `requirements.txt`.

## Tests

- Python (unittest):
  - Tests under `generator/pygenerator/test/`.
  - Ensure `PYTHONPATH` includes `generator/pygenerator/src`.
- C++: ctest/Ninja under `build/` (if applicable).
- After substantive changes, run a minimal test (happy path + 1–2 edges) and fix before concluding.

### Python tests prerequisites and how to run

To run the tests, just run this command at the root directory of the project:
```bash
make python-test
```

### Python typing

To check typing, run this command at the root directory of the project:
```bash
make python-check
```

### Python ruff

To check linting, run this command at the root directory of the project:
```bash
make python-ruff
```

## Quality and validations

For python, always execute typing (`make python-check`), linting (`make python-ruff`), and tests (`make python-test`), and iterate until all errors are corrected.

## Writing Python dependencies

- Add/update in `generator/pygenerator/pyproject.toml`.
- Regenerate `requirements.txt` with `pip-compile` (pinned), and install into `.venv`.
- For dev only, regenerate `dev-requirements.txt` with `pip-compile` (pinned), and install into `.venv`.
- Keep extras (e.g., `ollama`) minimal if only one backend is desired.
