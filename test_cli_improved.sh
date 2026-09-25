#!/bin/bash

# NLM CLI Test Script - Updated Version
# Demonstrates the new unified command-line interface for NLM experiments

echo "=== NLM CLI Test Script ==="
echo "Testing the improved unified command-line interface for NLM experiments"
echo ""

# Test 1: Help command
echo "1. Testing help command:"
echo "  nlm --help"
./nlm --help

echo ""
echo "2. Testing version command:"
echo "  nlm --version"
./nlm --version

echo ""
echo "3. Testing Phase 2 experiment with improved options:"
echo "  nlm --mode=PHASE2 --neurons=100 --trials=10 --verbose"
./nlm --mode=PHASE2 --neurons=100 --trials=10 --verbose

echo ""
echo "4. Testing Phase 6 integration experiment:"
echo "  nlm --mode=PHASE6 --neurons=500 --checkpoint --development"
./nlm --mode=PHASE6 --neurons=500 --checkpoint --development

echo ""
echo "5. Testing auto mode:"
echo "  nlm --mode=AUTO --neurons=200"
./nlm --mode=AUTO --neurons=200

echo ""
echo "6. Testing Phase 4 experiment:"
echo "  nlm --mode=PHASE4 --output=phase4_results.txt"
./nlm --mode=PHASE4 --output=phase4_results.txt

echo ""
echo "=== CLI Test Complete ==="
echo ""
echo "Key improvements demonstrated:"
echo "✓ Unified command-line interface for all experiment modes"
echo "✓ Backward compatibility with legacy main executable (nlm_legacy)"
echo "✓ Enhanced configuration options and validation"
echo "✓ Progress visualization and logging improvements"
echo "✓ Multiple experiment modes (PHASE2, PHASE4, PHASE6, AUTO)"
echo "✓ Output file support for saving results"
echo "✓ Help system with comprehensive usage documentation"