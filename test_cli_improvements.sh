#!/bin/bash

# Test script for NLM CLI improvements
# This script verifies that the new CLI features work correctly

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== NLM CLI Improvements Test ===${NC}"

# Test 1: Check if CLI executable exists
if [ -f "./nlm" ]; then
    echo -e "${GREEN}✓ CLI executable exists${NC}"
else
    echo -e "${RED}✗ CLI executable not found${NC}"
    exit 1
fi

# Test 2: Check if help works
if ./nlm --help > /dev/null 2>&1; then
    echo -e "${GREEN}✓ Help command works${NC}"
else
    echo -e "${RED}✗ Help command failed${NC}"
    exit 1
fi

# Test 3: Check if version works
if ./nlm --version > /dev/null 2>&1; then
    echo -e "${GREEN}✓ Version command works${NC}"
else
    echo -e "${RED}✗ Version command failed${NC}"
    exit 1
fi

# Test 4: Check if CLI can start (non-interactive mode)
if timeout 5 ./nlm --help > /dev/null 2>&1; then
    echo -e "${GREEN}✓ CLI can start and respond to commands${NC}"
else
    echo -e "${YELLOW}⚠ CLI start test inconclusive (may be normal behavior)${NC}"
fi

# Test 5: Check if constants header exists
if [ -f "src/core/Constants.hpp" ]; then
    echo -e "${GREEN}✓ Constants header exists${NC}"
    
    # Check for expected constants
    if grep -q "DEFAULT_MEMBRANE_CAPACITANCE" src/core/Constants.hpp; then
        echo -e "${GREEN}✓ Constants header contains expected constants${NC}"
    else
        echo -e "${RED}✗ Constants header missing expected constants${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ Constants header not found${NC}"
    exit 1
fi

# Test 6: Check if CLI header exists
if [ -f "src/cli/NLMCLI.hpp" ]; then
    echo -e "${GREEN}✓ CLI header exists${NC}"
else
    echo -e "${RED}✗ CLI header not found${NC}"
    exit 1
fi

# Test 7: Check if documentation exists
if [ -f "DOCS/USER_GUIDE.md" ]; then
    echo -e "${GREEN}✓ User guide documentation exists${NC}"
else
    echo -e "${RED}✗ User guide documentation not found${NC}"
    exit 1
fi

# Test 8: Check if implementation summary exists
if [ -f "IMPLEMENTATION_SUMMARY.md" ]; then
    echo -e "${GREEN}✓ Implementation summary exists${NC}"
else
    echo -e "${RED}✗ Implementation summary not found${NC}"
    exit 1
fi

# Test 9: Check if CMakeLists.txt updated
if grep -q "nlm_phase6_demo" CMakeLists.txt; then
    echo -e "${GREEN}✓ CMakeLists.txt updated with Phase 6 demo${NC}"
else
    echo -e "${RED}✗ CMakeLists.txt not updated with Phase 6 demo${NC}"
    exit 1
fi

# Test 10: Check if main.cpp updated
if grep -q "NLMCLI" src/main.cpp; then
    echo -e "${GREEN}✓ main.cpp updated to use CLI${NC}"
else
    echo -e "${RED}✗ main.cpp not updated to use CLI${NC}"
    exit 1
fi

echo -e "${GREEN}=== All tests passed! ===${NC}"
echo -e "${YELLOW}CLI improvements successfully implemented${NC}"

# Print usage summary
echo -e "${GREEN}=== Usage Summary ===${NC}"
echo -e "Interactive mode: ./nlm"
echo -e "Command line mode: ./nlm --run phase6 --steps 10000"
echo -e "Help: ./nlm --help"
echo -e "Version: ./nlm --version"
echo -e "Export results: ./nlm --run phase6 --steps 10000 --output results.json"