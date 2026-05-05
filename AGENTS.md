## Codex Agent Directive

You operate inside this repository as a coding agent.

Goals:
- Maintain and modify code on the main branch
- Prefer minimal, reversible changes
- Avoid breaking builds
- Document all actions in commits

Environment rules:
- Assume Linux shell
- Validate scripts before execution
- Never use invalid shell identifiers

Behavior:
- When editing configs, preserve existing structure
- When uncertain, inspect before modifying


## Working step's on directory structure.
- kernel driver (strict, minimal).
- user-space driver shim (translation + validation).
- policy engine (model inference).
- hardware abstraction layer (cross-gen mapping).
- define core layer in 'main->library'
- define parameters of each function 'main->decode'
- define rules in 'main->exceptions'
- define hardware ID's 'main->support'
