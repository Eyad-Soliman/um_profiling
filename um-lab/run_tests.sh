#!/bin/sh

test_dir="$2"

# Loop through all .um test files in the directory
for testFile in "$test_dir/um_tests"/*.um; do
    # Extract filename without path or extension
    testName=$(basename "$testFile" .um)

    echo "Running test: $testName"
    ./um "$testFile"

    expected="$test_dir/output_tests$testName.out"
    output=$(./um "$testFile")

    if [ "$output" = "$(cat "$expected")" ]; then
        echo "PASS"
    else
        echo "FAIL"
    fi

done
