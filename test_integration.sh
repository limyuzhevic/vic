#!/usr/bin/env bash

# NLM Phase 6 Integration Test Script
# Tests and validates the integrated artificial brain systems

# Color codes for output
GREEN="\033[32;42m"
RED="\033[31m"
YELLOW="\033[33m"
BLUE="\033[34m"
NC="\033[0m" # No Color

echo -e "${BLUE}=== NLM Phase 6 Integration Test Script ===${NC}"
echo -e "${YELLOW}Tests and validates the integrated artificial brain systems${NC}"

echo -e "${GREEN}Building the project...${NC}"
cd /workspace/app
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=OFF
make -j4

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Build successful!${NC}"

echo -e "${YELLOW}Running integration tests...${NC}"
./nlm_phase6_demo

if [ $? -ne 0 ]; then
    echo -e "${RED}Integration test failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Integration test passed!${NC}"

# Create summary
echo -e "${YELLOW}=== TEST SUMMARY ===${NC}"
./nlm_benchmark

# Check if Python bindings work
echo -e "${YELLOW}Testing Python bindings...${NC}"
cd /workspace/app
python3 -c "
try:
    import pynlm
    print('Python bindings: SUCCESS')
    print(f'Available modules: {dir(pynlm)}')
except ImportError as e:
    print(f'Python bindings: FAILED - {e}')
"

echo -e "${GREEN}All tests completed!${NC}"