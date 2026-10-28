#!/bin/bash

# Phase 6 Integration Tests Verification Script
# This script checks that all required integration tests have been created

RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
NC="\033[0m" # No Color

echo "=== Phase 6 Integration Tests Verification ==="
echo

echo "Checking test files..."
echo

# Expected test files
expected_files=(
    "test_memory_integration.cpp"
    "test_prediction_integration.cpp"
    "test_cognition_integration.cpp"
    "test_neuromodulation_integration.cpp"
    "test_development_integration.cpp"
    "test_full_brain_integration.cpp"
)

# Check each expected file
all_files_exist=true
for file in "${expected_files[@]}"; do
    if [ -f "/workspace/app/tests/$file" ]; then
        echo -e "${GREEN}✓${NC} Found: $file"
    else
        echo -e "${RED}✗${NC} Missing: $file"
        all_files_exist=false
    fi
done

echo
echo "Checking CMakeLists.txt..."
if grep -q "test_memory_integration.cpp" "/workspace/app/tests/CMakeLists.txt" && \
   grep -q "test_prediction_integration.cpp" "/workspace/app/tests/CMakeLists.txt" && \
   grep -q "test_cognition_integration.cpp" "/workspace/app/tests/CMakeLists.txt" && \
   grep -q "test_neuromodulation_integration.cpp" "/workspace/app/tests/CMakeLists.txt" && \
   grep -q "test_development_integration.cpp" "/workspace/app/tests/CMakeLists.txt" && \
   grep -q "test_full_brain_integration.cpp" "/workspace/app/tests/CMakeLists.txt"; then
    echo -e "${GREEN}✓${NC} CMakeLists.txt includes all test files"
else
    echo -e "${RED}✗${NC} CMakeLists.txt missing some test files"
    all_files_exist=false
fi

echo
echo "Checking test_main.cpp..."
if grep -q "namespace test_memory_integration" "/workspace/app/tests/test_main.cpp" && \
   grep -q "namespace test_prediction_integration" "/workspace/app/tests/test_main.cpp" && \
   grep -q "namespace test_cognition_integration" "/workspace/app/tests/test_main.cpp" && \
   grep -q "namespace test_neuromodulation_integration" "/workspace/app/tests/test_main.cpp" && \
   grep -q "namespace test_development_integration" "/workspace/app/tests/test_main.cpp" && \
   grep -q "namespace test_full_brain_integration" "/workspace/app/tests/test_main.cpp"; then
    echo -e "${GREEN}✓${NC} test_main.cpp includes all integration test namespaces"
else
    echo -e "${RED}✗${NC} test_main.cpp missing some integration test namespaces"
    all_files_exist=false
fi

echo
enecho "Checking for required includes in test files..."
# Check a few key files for required includes
key_checks=(
    "test_memory_integration.cpp:core/Config/Config.hpp brain/Brain.hpp memory/NeuralWorkingMemory.hpp memory/NeuralEpisodicMemory.hpp"
    "test_prediction_integration.cpp:core/Config/Config.hpp brain/Brain.hpp prediction/PredictionSystem.hpp"
    "test_cognition_integration.cpp:core/Config/Config.hpp brain/Brain.hpp cognition/NeuralPlanner.hpp cognition/ConceptFormation.hpp"
)

for check in "${key_checks[@]}"; do
    file=$(echo $check | cut -d: -f1)
    includes=$(echo $check | cut -d: -f2)
    
    all_includes_found=true
    for inc in $includes; do
        if grep -q "#include \"$inc\"" "/workspace/app/tests/$file" || \
           grep -q '#include "${inc}"' "/workspace/app/tests/$file"; then
            continue
        else
            echo -e "${YELLOW}⚠${NC} File $file may be missing include: $inc"
            all_includes_found=false
        fi
    done
    
done

echo
enecho "=== Verification Summary ==="
if [ "$all_files_exist" = true ]; then
    echo -e "${GREEN}✓ All Phase 6 integration tests have been created successfully!${NC}"
    echo
echo "Created integration tests cover:"
echo "  ✓ Memory systems integration"
echo "  ✓ Prediction system integration"
echo "  ✓ Cognition systems integration"
echo "  ✓ Neuromodulation systems integration"
echo "  ✓ Development system integration"
echo "  ✓ Full brain loop integration"
else
    echo -e "${RED}✗ Some Phase 6 integration tests are missing or incomplete.${NC}"
fi

echo
enecho "Next Steps:"
echo "1. Run the test suite to verify functionality:"
echo "   mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
echo "   ./nlm_test"
echo
echo "2. Review the generated test results"
echo "3. Verify all Phase 6 integration claims are validated"