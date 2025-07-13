#!/bin/bash
# Integration test for line-by-line processing in nej CLI tool

set -euo pipefail

# Define colors for output
GREEN='[0;32m'
RED='[0;31m'
NC='[0m' # No Color

# --- Test Setup ---
TEST_DIR=$(mktemp -d -t nej_test_XXXXXX)
INPUT_FILE="${TEST_DIR}/input.txt"
NEJ_BIN="../../build/nej" # Path to nej executable relative to the script

echo "Running integration test in: ${TEST_DIR}"

# Create a test input file
cat <<EOF > "${INPUT_FILE}"
Hello world!
This line has an emoji: 👋
Another line without.
And one more with multiple: ✨🐛📝
Final line.
EOF

# --- Test 1: Dry Run Mode ---
echo -n "Test 1: Dry Run Mode... "
DRY_RUN_RAW_OUTPUT=$("${NEJ_BIN}" --dry-run "${INPUT_FILE}" 2>&1 || true)
DRY_RUN_OUTPUT=$(printf %s "${DRY_RUN_RAW_OUTPUT}" | sed 's/\n$//')

EXPECTED_DRY_RUN_OUTPUT="File: \"${INPUT_FILE}\", Emojis removed: 4"

if [[ "${DRY_RUN_OUTPUT}" == "${EXPECTED_DRY_RUN_OUTPUT}" ]]; then
    echo -e "${GREEN}PASS${NC}"
else
    echo -e "${RED}FAIL${NC}"
    echo "Expected: '${EXPECTED_DRY_RUN_OUTPUT}'"
    echo "Actual:   '${DRY_RUN_OUTPUT}'"
    exit 1
fi


# --- Test 2: In-Place Modification ---
echo -n "Test 2: In-Place Modification... "
"${NEJ_BIN}" -i.bak "${INPUT_FILE}" > /dev/null # Suppress stdout

EXPECTED_CONTENT=$(cat <<EOF
Hello world!
This line has an emoji:  
Another line without.
And one more with multiple:    
Final line.
EOF
)

ACTUAL_CONTENT=$(cat "${INPUT_FILE}")

if [[ "${ACTUAL_CONTENT}" == "${EXPECTED_CONTENT}" ]]; then
    echo -e "${GREEN}PASS${NC}"
else
    echo -e "${RED}FAIL${NC}"
    echo "Expected content:"
    echo "${EXPECTED_CONTENT}"
    echo "Actual content:"
    echo "${ACTUAL_CONTENT}"
    exit 1
fi

# Verify backup file exists
if [[ -f "${INPUT_FILE}.bak" ]]; then
    echo -n "Test 2.1: Backup file created... "
    echo -e "${GREEN}PASS${NC}"
else
    echo -e "${RED}FAIL${NC}"
    echo "Backup file ${INPUT_FILE}.bak was not created."
    exit 1
fi

# --- Test Cleanup ---
echo "Cleaning up test directory: ${TEST_DIR}"
rm -rf "${TEST_DIR}"

echo "All integration tests passed."
exit 0
