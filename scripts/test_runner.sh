#!/bin/sh

set -eu

BUILD_DIR="builddir"

run_filtered_test() {
    test_bin="$1"
    filter="$2"

    if [ ! -x "$test_bin" ]; then
        echo "ERROR: Test binary not found: $test_bin"
        exit 1
    fi

    printf '\nRunning %s: %s\n' "$test_bin" "$filter"

    "$test_bin" --filter="$filter"
}

run_full_test() {
    test_bin="$1"

    if [ ! -x "$test_bin" ]; then
        echo "ERROR: Test binary not found: $test_bin"
        exit 1
    fi

    printf '\nRunning %s\n' "$test_bin"

    "$test_bin"
}

echo "Building Skope..."
meson compile -C "$BUILD_DIR"

echo
echo "Running unit tests..."

# Scope tests
run_filtered_test "$BUILD_DIR/scope_unit_bin" "scope_unit/scope_search_test"
run_filtered_test "$BUILD_DIR/scope_unit_bin" "scope_unit/scope_crud_test"

# ScopeFile tests
run_filtered_test "$BUILD_DIR/scopefile_unit_bin" "scopefile_unit/scopefile_search_test"
run_filtered_test "$BUILD_DIR/scopefile_unit_bin" "scopefile_unit/scopefile_crud_test"

# Other tests
run_full_test "$BUILD_DIR/relation_unit_bin"
run_full_test "$BUILD_DIR/entity_unit_bin"

echo
echo "All tests passed!"
