#!/usr/bin/env bash
# Runs clang-tidy on the C++ and Objective-C++ files passed by pre-commit.
# Compiler flags are given here directly, so no build directory is needed.
set -euo pipefail

tidy="$(command -v clang-tidy || true)"
if [[ -z "$tidy" ]]; then
    for candidate in /opt/homebrew/opt/llvm/bin/clang-tidy /usr/local/opt/llvm/bin/clang-tidy; do
        [[ -x "$candidate" ]] && tidy="$candidate" && break
    done
fi
if [[ -z "$tidy" ]]; then
    echo "clang-tidy not found. Install it with: brew install llvm" >&2
    exit 1
fi

sdk="$(xcrun --show-sdk-path 2>/dev/null || true)"
common=(-std=c++20 -Iengine/include -Wall -Wextra)
[[ -n "$sdk" ]] && common+=(-isysroot "$sdk")

status=0
for file in "$@"; do
    case "$file" in
        *.mm) flags=(-x objective-c++ -fobjc-arc "${common[@]}") ;;
        *)    flags=(-x c++ "${common[@]}") ;;
    esac
    "$tidy" --quiet "$file" -- "${flags[@]}" || status=1
done
exit "$status"
