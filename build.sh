#!/usr/bin/env bash
# NLM Project Build Script
# This script provides easy access to common development tasks

set -e

echo "=== NLM Project Build Script ==="
echo "Available commands:"

echo ""
echo "Building:"
echo "  ./build.sh           Build all targets"
echo "  ./build.sh debug     Build with debug flags"
echo "  ./build.sh clean     Clean build artifacts"

echo ""
echo "Running demos:"
echo "  ./run.sh demo        Run Phase 2 demo"
echo "  ./run.sh phase3      Run Phase 3 demo"
echo "  ./run.sh phase6      Run Phase 6 integration test"
echo "  ./run.sh benchmark   Run benchmark tests"

echo ""
echo "Testing:"
echo "  ./run.sh tests       Run all unit tests"
echo "  ./run.sh pytest      Run Python tests (if any)"

echo ""
echo "Development tools:"
echo "  ./run.sh analyze     Run code analysis"
echo "  ./run.sh docs        Build documentation"
echo "  ./run.sh profile     Run performance profiling"

echo ""
echo "Utilities:"
echo "  ./run.sh status      Show build status"
echo "  ./run.sh config      Show configuration"
echo "  ./run.sh help        Show this help"
