#!/bin/bash
# examples/quick_reference.sh

# NLM Quick Reference Guide

echo "NLM CLI Quick Reference Guide"
echo "================================"
echo ""
echo "This script provides quick access to all NLM modes with common configurations."
echo ""

# Demo mode - Basic integration tests
echo "=== DEMO MODE (Integration Tests) ==="
echo "Run comprehensive integration tests for Phase 6:"
echo ""
echo "1. Basic demo:"
./nlm --mode demo
echo ""
echo "2. Demo with validation:"
./nlm --mode demo --validate
echo ""
echo "3. Demo with verbose output:"
./nlm --mode demo --verbose
echo ""

# Learning mode - Plasticity experiments
echo "=== LEARNING MODE (Plasticity Experiments) ==="
echo "Run plasticity learning experiments with different patterns:"
echo ""
echo "4. Learning with random pattern:"
./nlm --mode learning --pattern random --iterations 500
echo ""
echo "5. Learning with sinusoidal pattern:"
./nlm --mode learning --pattern sinusoidal --iterations 500
echo ""
echo "6. Learning with correlated pattern:"
./nlm --mode learning --pattern correlated --iterations 500
echo ""
echo "7. Learning with output file:"
./nlm --mode learning --pattern random --iterations 500 --output learning_results.txt
echo ""

# Benchmark mode - Performance testing
echo "=== BENCHMARK MODE (Performance Testing) ==="
echo "Run performance benchmarks for different network sizes:"
echo ""
echo "8. Standard benchmark suite:"
./nlm --mode benchmark
echo ""
echo "9. Custom benchmark with 20 iterations:"
./nlm --mode benchmark --iterations 20
echo ""
echo "10. Benchmark with validation:"
./nlm --mode benchmark --validate
echo ""

# Explore mode - Curiosity-driven exploration
echo "=== EXPLORE MODE (Curiosity-Driven Exploration) ==="
echo "Run open-ended exploration with adaptive input patterns:"
echo ""
echo "11. Random exploration (60 seconds):"
./nlm --mode explore --pattern random --duration 60
echo ""
echo "12. Sinusoidal exploration (60 seconds):"
./nlm --mode explore --pattern sinusoidal --duration 60
echo ""
echo "13. Timeout test (10 seconds):"
./nlm --mode explore --pattern random --duration 10
echo ""
echo "14. Batch exploration (300 seconds):"
./nlm --mode explore --batch --duration 300 --pattern random
echo ""

# Compare mode - Parameter comparison
echo "=== COMPARE MODE (Parameter Comparison) ==="
echo "Compare different parameter sets for sensitivity analysis:"
echo ""
echo "15. Parameter comparison:"
./nlm --mode compare
echo ""
echo "16. Parameter comparison with output:"
./nlm --mode compare --output parameter_comparison.txt
echo ""
echo "17. Sensitivity analysis:"
./nlm --mode compare --pattern sensitivity
echo ""

# Quick help
echo "=== QUICK HELP ==="
echo "Get help for specific modes:"
echo ""
echo "18. Show mode descriptions:"
./nlm --help-modes
echo ""
echo "19. Show option descriptions:"
./nlm --help-options
echo ""
echo "20. Show complete usage:"
./nlm --help-usage
echo ""
echo "21. Show version information:"
./nlm --version
echo ""

# Configuration examples
echo "=== CONFIGURATION EXAMPLES ==="
echo "Check available configuration files:"
echo ""
echo "22. List configuration files:"
./nlm --list-configs
echo ""
echo "23. Validate specific configuration:"
./nlm --validate --config examples/config.cfg
echo ""

echo "Quick reference completed successfully!"
echo ""
echo "For more detailed information, see:"
echo "- easy_usage.md (beginner-friendly guide)"
echo "- HOW_TO_USE.md (comprehensive documentation)"
echo "- TEST_FRAMEWORK.md (testing framework documentation)"
