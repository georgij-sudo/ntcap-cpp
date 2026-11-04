# Orchestrator Workflow Protocol

For any non-trivial coding task, you MUST follow this two-phase delegation protocol:

## Phase 1 — Research (mandatory first step)
Delegate to the `researcher` mode using `new_task`. Instruct the researcher to locate and read all relevant files, and to return factual findings. Do NOT ask the researcher to propose changes.

## Phase 2 — Planning (mandatory second step)
After the `researcher` returns, take its findings and delegate to the `architect` mode using `new_task`. Provide the architect with the full research context. Instruct the architect to produce a concrete, step-by-step plan.

Only after Phase 2 is complete may you proceed to implementation. Never skip Phase 1 before Phase 2, and never delegate directly to `architect` without first running `researcher`, unless the user explicitly provides all necessary context and overrides this protocol.

## Debugging Protocol (mandatory two-phase cycle)

When the user's request involves diagnosing or fixing a bug, you MUST NOT
delegate directly to `debug`. Instead, follow this cycle:

**Phase 1 — Context Gathering (researcher):**
- Delegate to `researcher` using `new_task`.
- Instruct the researcher to locate and read all relevant files: source code,
  error logs, stack traces, test files, and any configuration involved in the
  bug. Ask it to return **factual findings only** — no proposed fixes.
- Wait for the researcher to complete and return its summary.

**Phase 2 — Analysis & Fix (debug):**
- Only after Phase 1 is complete, delegate to `debug` using `new_task`.
- In the `message` parameter, include the full research summary from Phase 1.
  Explicitly tell `debug`: "The context below has been gathered by a researcher.
  Analyze it, identify the root cause, and propose or implement a fix. Do not
  attempt to read additional files yourself."
- If `debug` returns a request for more information, re-run Phase 1 (researcher)
  for the specific gap, then re-delegate to `debug` with the updated context.

**Critical rule:** Never delegate to `debug` before `researcher` has completed
and its findings have been passed to `debug`. The only exception is if the user
explicitly provides all necessary logs and code snippets in the initial message
and asks you to skip research.