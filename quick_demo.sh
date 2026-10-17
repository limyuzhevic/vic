#!/usr/bin/env bash

# Quick NLM Demonstration Script
# 
# This script runs a very short demonstration of NLM's integrated systems.
# It's designed to be quick and show users what NLM can do.

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
print_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Check if nlm exists
if ! command -v nlm &> /dev/null; then
    print_error "NLM executable not found!"
    print_info "Please build NLM first:"
    echo "  mkdir build && cd build && cmake .. && make -j$(nproc)"
    exit 1
fi

print_info "Starting NLM Quick Demonstration..."
echo ""

# Run a short demo
print_info "Running: nlm demo --steps 50 --quiet"
echo ""

if nlm demo --steps 50 --quiet 2>&1; then
    echo ""
    print_success "Demo completed successfully!"
    echo ""
    print_info "What you just saw:"
    echo "  ✓ NLM verified all systems are integrated"
    echo "  ✓ Memory systems are working"
    echo "  ✓ Neuromodulation systems are active"
    echo "  ✓ Brain is learning from experience"
    echo "  ✓ All cognitive systems are connected"
    echo ""
    print_info "Next steps for learning more:"
    echo "  ./nlm_demo.sh --getting-started  # Learn how to use NLM"
    echo "  nlm verify                      # Check integration status"
    echo "  nlm status                      # View current brain state"
    echo "  nlm experiment --steps 1000     # Run full experiment"
    echo ""
else
    print_error "Demo failed!"
    echo ""
    print_info "Troubleshooting tips:"
    echo "  1. Make sure NLM was built with experiment support:"
    echo "     cmake .. -DNLM_ENABLE_EXPERIMENTS=ON"
    echo "  2. Check for build errors in the output above"
    echo "  3. Try a smaller simulation: nlm demo --steps 20"
    echo ""
fi

echo ""
print_info "Demonstration complete!"