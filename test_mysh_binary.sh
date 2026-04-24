#!/bin/bash

PASS=0
FAIL=0
BINARY="./mysh"

green() { printf "\033[0;32m[PASS]\033[0m %s\n" "$1"; }
red()   { printf "\033[0;31m[FAIL]\033[0m %s\n      detail : %s\n      penalty: -%d%%\n" "$1" "$2" "$3"; }

check() {
    local label="$1" detail="$2" penalty="$3" ok="$4"
    if [ "$ok" -eq 0 ]; then
        green "$label"
        ((PASS++))
    else
        red "$label" "$detail" "$penalty"
        ((FAIL++))
    fi
}

run_shell() {
    printf "%s\n" "$@" | timeout 4 "$BINARY" 2>&1
}

echo ""
echo "══════════════════════════════════════════"
echo "  mysh Test Script"
echo "══════════════════════════════════════════"
echo ""

echo "── Build ──"

[ -f "Makefile" ]
check "Makefile present" "Makefile not found in project root" 10 $?

make compile > /dev/null 2>&1
BUILD_RC=$?
check "make compile succeeds (no errors)" "$(make compile 2>&1 | tail -3)" 0 $BUILD_RC

if [ $BUILD_RC -ne 0 ] || [ ! -x "$BINARY" ]; then
    echo ""
    echo "Binary not built — cannot run shell tests."
    exit 1
fi

echo ""
echo "── Prompt Format ────────────────────────────────────────────────────────"

CWD=$(pwd)
EXPECTED_PROMPT="${CWD}\$mysh> "

PROMPT_LINE=$(printf "exit\n" | timeout 3 "$BINARY" 2>/dev/null)

[[ "$PROMPT_LINE" == "$EXPECTED_PROMPT" ]]
check "Prompt format: '${EXPECTED_PROMPT}'" \
      "got: '${PROMPT_LINE}'" 10 $?

echo ""
echo "── Built-in: pwd ────────────────────────────────────────────────────────"

OUT=$(run_shell "pwd" "exit")

echo "$OUT" | grep -qF "$CWD"
check "pwd outputs current working directory" \
      "expected '$CWD' in output, got: $(echo "$OUT" | tr '\n' '|')" 10 $?

echo ""
echo "── Built-in: cd ─────────────────────────────────────────────────────────"

OUT=$(run_shell "cd /tmp" "pwd" "exit")
echo "$OUT" | grep -qF "/tmp"
check "cd /tmp changes directory (pwd shows /tmp)" \
      "pwd after 'cd /tmp' did not contain /tmp — got: $(echo "$OUT" | tr '\n' '|')" 10 $?

PROMPT_AFTER=$(echo "$OUT" | grep '\$mysh>' | tail -1)
[[ "$PROMPT_AFTER" == "/tmp\$mysh> "* ]]
check "Prompt updates after cd (shows new path)" \
      "prompt after 'cd /tmp' should start with '/tmp\$mysh> ', got: '${PROMPT_AFTER}'" 10 $?

OUT_CD_HOME=$(run_shell "cd" "pwd" "exit")
echo "$OUT_CD_HOME" | grep -xqF "$HOME"
check "cd (no arg) changes to \$HOME ($HOME)" \
      "pwd after bare 'cd' did not output '$HOME' — got: $(echo "$OUT_CD_HOME" | tr '\n' '|')" 10 $?

echo ""
echo "── Built-in: exit ───────────────────────────────────────────────────────"

timeout 3 bash -c "printf 'exit\n' | $BINARY > /dev/null 2>&1"
check "exit terminates shell within 3 seconds" \
      "shell did not exit (hung or crashed)" 10 $?

echo ""
echo "── External Commands (fork/exec/wait) ───────────────────────────────────"

OUT=$(run_shell "ls" "exit")
echo "$OUT" | grep -qE "(Makefile|mysh\.c)"
check "ls executes and shows directory contents" \
      "expected Makefile or mysh.c in output, got: $(echo "$OUT" | tr '\n' '|')" 50 $?

OUT=$(run_shell "echo hello world" "exit")
echo "$OUT" | grep -q "hello world"
check "echo hello world passes arguments correctly" \
      "expected 'hello world', got: $(echo "$OUT" | tr '\n' '|')" 50 $?

OUT=$(run_shell "/bin/echo test_external" "exit")
echo "$OUT" | grep -q "test_external"
check "/bin/echo test_external (absolute path external cmd)" \
      "expected 'test_external', got: $(echo "$OUT" | tr '\n' '|')" 50 $?

echo ""
echo "── Error Handling ───────────────────────────────────────────────────────"

OUT=$(printf "nonexistentcmd_xyz\nexit\n" | timeout 3 "$BINARY" 2>&1)
echo "$OUT" | grep -q "Failed to execute command"
check "Unknown command → 'Failed to execute command' on stderr" \
      "message not found in output" 0 $?

# Summary ─────────────────────────────────────────────────────────────────────
echo ""
echo "══════════════════════════════════════════"
printf "  Passed : %d\n" "$PASS"
printf "  Failed : %d\n" "$FAIL"
echo "══════════════════════════════════════════"
echo ""

[ $FAIL -eq 0 ] && echo "  All checks passed" || \
    echo "  Fix failures"
echo ""

exit $FAIL
