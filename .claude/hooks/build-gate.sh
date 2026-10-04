#!/bin/sh
# build-gate.sh — the one rule in CLAUDE.md that a person cannot be trusted
# with: never commit a state that does not build.
#
# Not a .git/hooks/pre-commit, because that lives outside the tracked tree, is
# not copied into a worktree, and protects exactly one clone. This runs in
# front of the tool call instead: Claude Code hands every Bash invocation to
# it on stdin, and a non-zero exit stops the command.
#
# It is the cheap gate. CI is the authority. Escape hatch: --no-verify.

set -eu

command=$(python3 -c 'import json,sys; print(json.load(sys.stdin).get("tool_input",{}).get("command",""))' 2>/dev/null || true)

case "$command" in
    *"git commit"*) ;;
    *) exit 0 ;;
esac
case "$command" in
    *--no-verify*) exit 0 ;;
esac

root=$(git rev-parse --show-toplevel 2>/dev/null) || exit 0
cd "$root"

set -- apps/*.cpp
if [ -e "$1" ] && ! out=$(make 2>&1); then
    printf 'Refusing the commit: the tree does not build.\n\n%s\n' "$out" >&2
    exit 2
fi

if command -v cairn >/dev/null 2>&1; then
    if ! out=$(cairn check 2>&1); then
        printf 'Refusing the commit: cairn check fails.\n\n%s\n' "$out" >&2
        exit 2
    fi
else
    printf 'Note: cairn is not installed, so the roadmap was not validated.\n' >&2
fi

# CLAUDE.md's second claim: phreaking has no code. The pattern lives here,
# outside the tree it searches, because a file that has to be free of a word
# cannot contain the grep for it.
if out=$(grep -rniE 'blue_box|bluebox|whistle_attack|<chrono>' include/ 2>/dev/null); then
    printf 'Refusing the commit: include/ may contain none of these (rules 3 and 4).\n\n%s\n' "$out" >&2
    exit 2
fi

exit 0
