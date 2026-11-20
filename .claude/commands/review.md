---
description: Senior engineer code review of the engine and macOS app
argument-hint: [context, e.g. "early prototype, focus on structure and naming"]
---

You are a senior engineer reviewing the macOS port of Geiss.

Context: $ARGUMENTS

If no context is given, review the whole port as it stands on the current branch.

## Scope

Review only:
- `engine/`: the portable C++ visualization engine
- `macos/Geiss/EngineBridge.mm`: the Objective-C++ bridge
- `macos/`: the SwiftUI app

Leave out the legacy Windows screensaver and Winamp plug-in sources, and build output.
Read `AGENTS.md` and `docs/spec/macos-port.md` first. They hold the agreed product decisions.

## Focus

- Correctness and edge cases
- Real-time audio safety: the audio callback only copies samples, with no allocation, locks, Objective-C messaging, or logging
- Threading: the engine reads the audio frame on the main thread, and data shared across threads is synchronized
- Boundaries: the engine stays independent of the app shell, and the bridge is the only file that includes both Cocoa and the engine
- Object lifetimes and ownership across the bridge
- Audio tap lifecycle: recreated when the default output device changes, stopped on sleep, started again on wake, and failures shown to the user as the spec describes
- Frame-rate-independent timing
- Code structure and maintainability
- Idiomatic C++, Swift, and Objective-C++

Out of scope: styling and formatting, which clang-tidy and SwiftLint handle.
Don't suggest new visualizer controls or features unless the context asks for them.

## Output

For every finding, give:
- `file:line`
- what is wrong
- a concrete way it fails (inputs or state, and the result)
- a suggested fix

Group the findings as:
1. Critical issues: bugs, crashes, data races, broken logic
2. Suggestions: improvements worth making
3. What works well

Keep only findings you've checked against the code. If a section has nothing in it, say so.
