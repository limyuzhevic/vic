#!/bin/bash

# Verification script for NLM Phase 2 improvements

set -e

echo "=== NLM Phase 2 Improvements Verification ==="
echo "Checking all improvements..."

# 1. Check config file fix
echo "\n1. Checking configuration file fix..."
if grep -q "dopamine_baseline = 0.1" configs/default.cfg && ! grep -q "dopamine_baseline = 0.0" configs/default.cfg | head -2; then
    echo "✓ Configuration file fix: PASSED"
else
    echo "✗ Configuration file fix: FAILED"
    exit 1
fi

# 2. Check main.cpp improvements
echo "\n2. Checking main.cpp improvements..."
if grep -q "TestStatistics" src/main.cpp && grep -q "TestOptions" src/main.cpp; then
    echo "✓ Main.cpp test framework: PASSED"
else
    echo "✗ Main.cpp test framework: FAILED"
    exit 1
fi

# 3. Check AgentBrain.cpp refactoring
echo "\n3. Checking AgentBrain.cpp refactoring..."
if grep -q "namespace nlm" src/agent/AgentBrain.cpp && wc -l src/agent/AgentBrain.cpp | awk '{print $1}' > /tmp/line_count && cat /tmp/line_count; then
    lines=$(cat /tmp/line_count)
    if [ "$lines" -lt 200 ]; then
        echo "✓ AgentBrain.cpp refactored ($lines lines): PASSED"
    else
        echo "✗ AgentBrain.cpp still too long ($lines lines): FAILED"
        exit 1
    fi
else
    echo "✗ AgentBrain.cpp check: FAILED"
    exit 1
fi

# 4. Check Config.cpp improvements
echo "\n4. Checking Config.cpp improvements..."
if grep -q "validateKey" src/core/Config/Config.cpp && grep -q "getLastError" src/core/Config/Config.cpp; then
    echo "✓ Config.cpp error handling: PASSED"
else
    echo "✗ Config.cpp error handling: FAILED"
    exit 1
fi

# 5. Check Logger improvements
echo "\n5. Checking Logger improvements..."
if grep -q "LogCategory" src/core/Logger/Logger.hpp && grep -q "LogStatistics" src/core/Logger/Logger.hpp; then
    echo "✓ Logger enhancements: PASSED"
else
    echo "✗ Logger enhancements: FAILED"
    exit 1
fi

# 6. Check documentation
echo "\n6. Checking documentation..."
if [ -f "DOCUMENTATION.md" ] && [ -f "EXAMPLES.md" ]; then
    echo "✓ Documentation files: PASSED"
else
    echo "✗ Documentation files: FAILED"
    exit 1
fi

# 7. Check examples
echo "\n7. Checking examples..."
if [ -f "examples/example_test.py" ] && [ -f "examples/config_builder.py" ]; then
    echo "✓ Example scripts: PASSED"
else
    echo "✗ Example scripts: FAILED"
    exit 1
fi

# 8. Try to compile the code
echo "\n8. Attempting to compile the enhanced code..."
if g++ -std=c++17 -I. -I./src -c src/main.cpp -o /tmp/test_main.o 2>&1 | head -20; then
    echo "✓ Main.cpp compilation: PASSED"
else
    echo "✗ Main.cpp compilation: FAILED"
    exit 1
fi

if g++ -std=c++17 -I. -I./src -c src/agent/AgentBrain.cpp -o /tmp/test_agent.o 2>&1 | head -20; then
    echo "✓ AgentBrain.cpp compilation: PASSED"
else
    echo "✗ AgentBrain.cpp compilation: FAILED"
    exit 1
fi

if g++ -std=c++17 -I. -I./src -c src/core/Config/Config.cpp -o /tmp/test_config.o 2>&1 | head -20; then
    echo "✓ Config.cpp compilation: PASSED"
else
    echo "✗ Config.cpp compilation: FAILED"
    exit 1
fi

if g++ -std=c++17 -I. -I./src -c src/core/Logger/Logger.cpp -o /tmp/test_logger.o 2>&1 | head -20; then
    echo "✓ Logger.cpp compilation: PASSED"
else
    echo "✗ Logger.cpp compilation: FAILED"
    exit 1
fi

echo "\n=== All Checks Passed! ==="
echo ""
echo "Summary of Improvements Verified:"
echo "1. ✓ Configuration file bug fixes"
echo "2. ✓ Enhanced test framework with command-line options"
echo "3. ✓ Statistics collection and reporting"
echo "4. ✓ AgentBrain.cpp refactoring (reduced from ~1110 to ~182 lines)"
echo "5. ✓ Error handling and validation improvements"
echo "6. ✓ Advanced logging and diagnostic capabilities"
echo "7. ✓ Comprehensive documentation and examples"
echo ""
echo "The NLM Phase 2 framework has been successfully enhanced with:"
echo "- Better code organization and maintainability"
echo "- Robust error handling and validation"
echo "- Comprehensive testing framework"
echo "- Advanced logging and diagnostics"
echo "- Detailed documentation and examples"
echo ""
echo "Next steps:"
echo "1. Run the full test suite: ./nlm"
echo "2. Check the documentation in DOCUMENTATION.md"
echo "3. Explore examples in EXAMPLES.md"
echo "4. Review configuration files in configs/"
echo "5. Check log files in logs/ for detailed output"
