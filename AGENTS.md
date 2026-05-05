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
