#!/usr/bin/env bash
# Kilo Debug Script
# This script collects comprehensive information about the NLM project structure

set -e

echo "=== NLM Project Structure Analysis ==="
echo

# Check main build files
if [ -f "CMakeLists.txt" ]; then
    echo "✓ CMakeLists.txt exists"
    echo "  Lines: $(wc -l < CMakeLists.txt)"
else
    echo "✗ CMakeLists.txt missing"
fi

echo

# Check Phase 6 demo executable in build files
echo "=== Phase 6 Build Analysis ==="
echo "Checking for Phase 6 executable in CMakeLists.txt..."
grep -n "nlm_phase6_demo" CMakeLists.txt || echo "✗ nlm_phase6_demo not found in CMakeLists.txt"

echo
echo "Checking Phase 6 source files..."
if [ -f "src/experiments/Phase6Demo.cpp" ]; then
    echo "✓ src/experiments/Phase6Demo.cpp exists"
    echo "  Lines: $(wc -l < src/experiments/Phase6Demo.cpp)"
else
    echo "✗ src/experiments/Phase6Demo.cpp missing"
fi

if [ -f "src/experiments/Phase6IntegratedExperiment.cpp" ]; then
    echo "✓ src/experiments/Phase6IntegratedExperiment.cpp exists"
    echo "  Lines: $(wc -l < src/experiments/Phase6IntegratedExperiment.cpp)"
else
    echo "✗ src/experiments/Phase6IntegratedExperiment.cpp missing"
fi

echo
echo "=== Phase 5 Analysis ==="
grep -n "nlm_phase5" CMakeLists.txt || echo "✗ nlm_phase5 not found in CMakeLists.txt"

echo
echo "=== Main Executables in Build System ==="
echo "Configured executables:"
grep -A5 "add_executable(" CMakeLists.txt | head -20

echo
echo "=== Documentation Analysis ==="
if [ -f "README.md" ]; then
    echo "✓ README.md exists"
    echo "  Phase mentioned:"
    grep -i "phase" README.md | head -5
fi

if [ -f "docs/PHASE6_FINAL_AUDIT.md" ]; then
    echo "✓ docs/PHASE6_FINAL_AUDIT.md exists"
else
    echo "✗ docs/PHASE6_FINAL_AUDIT.md missing"
fi

echo
echo "=== Build Configuration Check ==="
echo "Checking build configuration..."
if [ -d "build" ]; then
    echo "✓ build directory exists"
else
    echo "✗ build directory missing - may need to run cmake"
fi

echo
echo "=== Integration Analysis ==="
echo "Checking integration consistency..."

# Look for integration issues
missing_configs=0

if [ ! -f "configs/default.cfg" ]; then
    echo "✗ configs/default.cfg missing"
    missing_configs=$((missing_configs + 1))
else
    echo "✓ configs/default.cfg exists"
fi

if [ ! -f "src/brain/Brain.hpp" ]; then
    echo "✗ src/brain/Brain.hpp missing"
    missing_configs=$((missing_configs + 1))
else
    echo "✓ src/brain/Brain.hpp exists"
fi

echo
echo "=== Summary ==="
echo "Phase 6 demo executable: $(grep -q "nlm_phase6_demo" CMakeLists.txt && echo "CONFIGURED" || echo "MISSING FROM BUILD SYSTEM")"
echo "Phase 6 source files: $(find src/experiments -name "Phase6*" -type f | wc -l) files"
echo "Phase 5 experiment files: $(find src/experiments -name "Phase5*" -type f | wc -l) files"

echo
echo "=== Critical Issues Found ==="
if [ $missing_configs -gt 0 ]; then
    echo "✗ $missing_configs configuration files missing"
fi

if ! grep -q "nlm_phase6_demo" CMakeLists.txt; then
    echo "✗ Phase 6 demo executable not in build system (CRITICAL)"
fi

echo
echo "=== Recommended Actions ==="
echo "1. Add Phase 6 demo to CMakeLists.txt build system"
echo "2. Ensure all Phase 6 source files are compiled"
echo "3. Verify integration consistency across all phases"
echo "4. Update documentation to reflect current state"
echo "5. Consider building with cmake and make to verify compilation"
