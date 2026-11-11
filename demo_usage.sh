#!/bin/bash
# NLM Demo Usage Script
# Demonstrates the improved command-line interface for NLM demos

echo "==========================================="
echo "NLM Demo Usage Examples"
echo "==========================================="
echo ""

echo "1. Basic Phase 3 demo with custom parameters:"
echo "./nlm_phase3_demo --episodes 10 --steps 500"
echo ""

echo "2. Phase 3 demo with configuration file:"
echo "./nlm_phase3_demo --config configs/fast_test.cfg --verbose"
echo ""

echo "3. Phase 3 demo with help:"
echo "./nlm_phase3_demo --help"
echo ""

echo "4. Phase 3 demo with version check:"
echo "./nlm_phase3_demo --version"
echo ""

echo "5. Phase 4 demo with custom trials:"
echo "./nlm_phase4_demo 100"
echo ""

echo "6. Phase 4 demo with all options:"
echo "./nlm_phase4_demo --config configs/default.cfg --log-level debug --verbose"
echo ""

echo "7. Phase 5 demo (advanced features):"
echo "./nlm_phase5_demo"
echo ""

echo "8. Phase 6 demo (full integration):"
echo "./nlm_phase6_demo"
echo ""

echo "9. Phase 6 text demo (language learning):"
echo "./nlm_phase6_text_demo"
echo ""

echo "10. Performance benchmark:"
echo "./nlm_benchmark"
echo ""

echo "==========================================="
echo "Configuration Files"
echo "==========================================="
echo "configs/default.cfg   - Default configuration"
echo "configs/fast_test.cfg - Fast configuration for testing"
echo ""

echo "For more information, visit the NLM documentation."