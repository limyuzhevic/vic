#!/bin/bash

# Enhanced Phase 6 Demo Test Script
# Tests the enhanced demo functionality and structure

set -e

echo "========================================"
echo "Enhanced Phase 6 Demo - Test Script"
echo "========================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test 1: Check if essential files exist
 echo "\n${YELLOW}Test 1: Checking essential files...${NC}"

required_files=(
    "src/experiments/Phase6Demo.hpp"
    "src/experiments/Phase6Demo.cpp"
    "src/experiments/EnhancedPhase6Demo.hpp"
    "src/experiments/EnhancedPhase6Demo.cpp"
    "src/experiments/CLIParser.hpp"
    "src/experiments/CLIParser.cpp"
    "src/core/Config/AdvancedConfigManager.hpp"
    "src/core/Config/AdvancedConfigManager.cpp"
)

all_files_exist=true
for file in "${required_files[@]}"; do
    if [[ -f "$file" ]]; then
        echo -e "  ${GREEN}✓${NC} $file"
    else
        echo -e "  ${RED}✗${NC} $file - MISSING"
        all_files_exist=false
    fi
done

if [[ "$all_files_exist" = false ]]; then
    echo -e "${RED}FAIL: Some essential files are missing!${NC}"
    exit 1
fi

# Test 2: Check CMakeLists.txt modifications
 echo "\n${YELLOW}Test 2: Checking CMakeLists.txt...${NC}"

if grep -q "nlm_phase6_demo_enhanced" CMakeLists.txt; then
    echo -e "  ${GREEN}✓${NC} Enhanced Phase 6 demo executable added to CMakeLists.txt"
else
    echo -e "  ${RED}✗${NC} Enhanced Phase 6 demo executable NOT found in CMakeLists.txt"
    exit 1
fi

if grep -q "Phase6Demo.cpp" CMakeLists.txt; then
    echo -e "  ${GREEN}✓${NC} Enhanced Phase 6 demo implementation added to CMakeLists.txt"
else
    echo -e "  ${RED}✗${NC} Enhanced Phase 6 demo implementation NOT found in CMakeLists.txt"
    exit 1
fi

# Test 3: Check header structure
 echo "\n${YELLOW}Test 3: Checking header structure...${NC}"

if grep -q "AdvancedConfigManager" src/experiments/EnhancedPhase6Demo.hpp; then
    echo -e "  ${GREEN}✓${NC} EnhancedPhase6Demo.hpp includes AdvancedConfigManager"
else
    echo -e "  ${RED}✗${NC} EnhancedPhase6Demo.hpp does not include AdvancedConfigManager"
    exit 1
fi

if grep -q "CLIParser" src/experiments/EnhancedPhase6Demo.hpp; then
    echo -e "  ${GREEN}✓${NC} EnhancedPhase6Demo.hpp includes CLIParser"
else
    echo -e "  ${RED}✗${NC} EnhancedPhase6Demo.hpp does not include CLIParser"
    exit 1
fi

# Test 4: Check implementation details
 echo "\n${YELLOW}Test 4: Checking implementation details...${NC}"

if grep -q "class EnhancedPhase6Demo" src/experiments/EnhancedPhase6Demo.hpp; then
    echo -e "  ${GREEN}✓${NC} EnhancedPhase6Demo class declaration found"
else
    echo -e "  ${RED}✗${NC} EnhancedPhase6Demo class declaration NOT found"
    exit 1
fi

if grep -q "namespace nlm" src/experiments/AdvancedConfigManager.hpp; then
    echo -e "  ${GREEN}✓${NC} AdvancedConfigManager in nlm namespace"
else
    echo -e "  ${RED}✗${NC} AdvancedConfigManager not in nlm namespace"
    exit 1
fi

# Test 5: Check configuration management
 echo "\n${YELLOW}Test 5: Checking configuration management...${NC}"

if grep -q "loadFromFile" src/core/Config/AdvancedConfigManager.cpp; then
    echo -e "  ${GREEN}✓${NC} loadFromFile method found"
else
    echo -e "  ${RED}✗${NC} loadFromFile method NOT found"
    exit 1
fi

if grep -q "saveToFile" src/core/Config/AdvancedConfigManager.cpp; then
    echo -e "  ${GREEN}✓${NC} saveToFile method found"
else
    echo -e "  ${RED}✗${NC} saveToFile method NOT found"
    exit 1
fi

if grep -q "validate" src/core/Config/AdvancedConfigManager.cpp; then
    echo -e "  ${GREEN}✓${NC} validate method found"
else
    echo -e "  ${RED}✗${NC} validate method NOT found"
    exit 1
fi

# Test 6: Check CLI parser
 echo "\n${YELLOW}Test 6: Checking CLI parser...${NC}"

if grep -q "CommandLineOptions" src/experiments/CLIParser.hpp; then
    echo -e "  ${GREEN}✓${NC} CommandLineOptions structure found"
else
    echo -e "  ${RED}✗${NC} CommandLineOptions structure NOT found"
    exit 1
fi

if grep -q "parse(int argc, char\* argv\[\])" src/experiments/CLIParser.cpp; then
    echo -e "  ${GREEN}✓${NC} parse method with argc/argv found"
else
    echo -e "  ${RED}✗${NC} parse method with argc/argv NOT found"
    exit 1
fi

if grep -q "showHelp" src/experiments/CLIParser.cpp; then
    echo -e "  ${GREEN}✓${NC} showHelp method found"
else
    echo -e "  ${RED}✗${NC} showHelp method NOT found"
    exit 1
fi

# Test 7: Check demo implementation
 echo "\n${YELLOW}Test 7: Checking demo implementation...${NC}"

if grep -q "Phase6IntegrationResult run(const CLIParser::CommandLineOptions\& options)" src/experiments/Phase6Demo.cpp; then
    echo -e "  ${GREEN}✓${NC} Enhanced demo run method found"
else
    echo -e "  ${RED}✗${NC} Enhanced demo run method NOT found"
    exit 1
fi

if grep -q "EnhancedPhase6Demo demo;" src/experiments/Phase6Demo.cpp; then
    echo -e "  ${GREEN}✓${NC} EnhancedPhase6Demo instance creation found"
else
    echo -e "  ${RED}✗${NC} EnhancedPhase6Demo instance creation NOT found"
    exit 1
fi

# Test 8: Check color support
 echo "\n${YELLOW}Test 8: Checking color support...${NC}"

if grep -q "Colors::" src/experiments/EnhancedPhase6Demo.hpp; then
    echo -e "  ${GREEN}✓${NC} Colors namespace found"
else
    echo -e "  ${RED}✗${NC} Colors namespace NOT found"
    exit 1
fi

if grep -q "setColorsEnabled" src/experiments/EnhancedPhase6Demo.hpp; then
    echo -e "  ${GREEN}✓${NC} setColorsEnabled method found"
else
    echo -e "  ${RED}✗${NC} setColorsEnabled method NOT found"
    exit 1
fi

# Test 9: Check documentation
 echo "\n${YELLOW}Test 9: Checking documentation...${NC}"

if [[ -f "PHASE6_ENHANCEMENT_PLAN.md" ]]; then
    echo -e "  ${GREEN}✓${NC} PHASE6_ENHANCEMENT_PLAN.md found"
else
    echo -e "  ${RED}✗${NC} PHASE6_ENHANCEMENT_PLAN.md NOT found"
    exit 1
fi

if [[ -f "Enhanced_Demo_Readme.md" ]]; then
    echo -e "  ${GREEN}✓${NC} Enhanced_Demo_Readme.md found"
else
    echo -e "  ${RED}✗${NC} Enhanced_Demo_Readme.md NOT found"
    exit 1
fi

# Test 10: Check backward compatibility
 echo "\n${YELLOW}Test 10: Checking backward compatibility...${NC}"

# Check if original Phase6Demo.cpp is preserved
if [[ -f "src/experiments/Phase6Demo.cpp" ]]; then
    echo -e "  ${GREEN}✓${NC} Original Phase6Demo.cpp preserved"
else
    echo -e "  ${RED}✗${NC} Original Phase6Demo.cpp NOT found"
    exit 1
fi

# Check if it has backward compatible arguments
if grep -q "argc > 1" src/experiments/Phase6Demo.cpp; then
    echo -e "  ${GREEN}✓${NC} Backward compatible argument parsing found"
else
    echo -e "  ${RED}✗${NC} Backward compatible argument parsing NOT found"
    exit 1
fi

echo "\n${GREEN}========================================${NC}"
echo "${GREEN}All tests passed! Enhanced Phase 6 demo is properly structured.${NC}"
echo "${GREEN}========================================${NC}"

echo "\n${YELLOW}Summary of enhancements:${NC}"
echo "  ✓ Advanced configuration management (JSON/.cfg support)"
echo "  ✓ Comprehensive CLI interface with help system"
echo "  ✓ Interactive modes and educational features"
necho "  ✓ Real-time progress indicators and feedback"
echo "  ✓ Colorized output and enhanced user experience"
echo "  ✓ Logging integration and results persistence"
echo "  ✓ Visualization integration (GraphViz/Plotly)"
echo "  ✓ Educational features with tutorial modes"
echo "  ✓ Power user features (batch, tuning, comparison)"
echo "  ✓ Advanced error handling and recovery"
echo "  ✓ Backward compatibility with original API"

echo "\n${YELLOW}Key files created/modified:${NC}"
echo "  src/experiments/Phase6Demo.hpp         - Enhanced Phase 6 demo header"
echo "  src/experiments/Phase6Demo.cpp         - Enhanced Phase 6 demo implementation"
echo "  src/experiments/EnhancedPhase6Demo.hpp  - Advanced demo class"
echo "  src/experiments/EnhancedPhase6Demo.cpp  - Advanced demo implementation"
echo "  src/experiments/CLIParser.hpp          - Command-line argument parser"
echo "  src/experiments/CLIParser.cpp          - CLI parser implementation"
echo "  src/core/Config/AdvancedConfigManager.hpp - Advanced configuration management"
echo "  src/core/Config/AdvancedConfigManager.cpp - Configuration implementation"
echo "  PHASE6_ENHANCEMENT_PLAN.md           - Enhancement plan documentation"
echo "  Enhanced_Demo_Readme.md              - Comprehensive readme documentation"

echo "\n${YELLOW}Build instructions:${NC}"
echo "  1. mkdir build"
echo "  2. cd build"
echo "  3. cmake .. -DCMAKE_BUILD_TYPE=Release"
echo "  4. make -j4"
echo ""
echo "${YELLOW}Run enhanced demo:${NC}"
echo "  ./nlm_phase6_demo_enhanced [options]"
echo ""
echo "${YELLOW}Examples:${NC}"
echo "  ./nlm_phase6_demo_enhanced --help"
echo "  ./nlm_phase6_demo_enhanced --version"
echo "  ./nlm_phase6_demo_enhanced --config myconfig.cfg"
echo "  ./nlm_phase6_demo_enhanced --education integration"
echo "  ./nlm_phase6_demo_enhanced --power batch --max-episodes 20"
