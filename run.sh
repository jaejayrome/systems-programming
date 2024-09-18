#!/bin/bash

# Create an array of test files
test_files=("test0" "test1" "test2" "test3" "test4" "test5")

# Path to the test files directory
test_files_dir="../test_files"

# Loop over each test file
for test_file in "${test_files[@]}"; do
    input_file="$test_files_dir/${test_file}.c"
    
    # Check if input file exists
    if [ -f "$input_file" ]; then
        # Run ./decomment and redirect outputs
        ./decomment < "$input_file" > "${test_file}_output" 2> "${test_file}_errors"
        echo "Processed $input_file. Output saved to ${test_file}_output, errors to ${test_file}_errors."
    else
        echo "Test file $input_file not found."
    fi
done
