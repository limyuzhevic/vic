#!/usr/bin/env bash

# NLM Command Line Interface
# This script provides a unified interface to NLM's various modes and features

echo "NLM Command Line Interface"
echo "==========================="

echo "Usage: nlm [mode] [options]"
echo ""
echo "Modes:"
echo "  demo    Run predefined demonstrations"
echo "  experiment Run custom experiments"
echo "  interactive   Start interactive mode"
echo "  benchmark  Run performance benchmarks"
echo "  monitor    Monitor running simulation"
echo "  config     Configuration management"
echo ""
echo "Options:"
echo "  --help     Show this help message"
echo "  --version  Show version information"
echo ""
echo "Examples:"
echo "  nlm demo --phase2           # Run Phase 2 demonstration (classic neural computation)"
echo "  nlm demo --phase6          # Run Phase 6 integration demo"
echo "  nlm experiment --create     # Create a new experiment configuration"
echo "  nlm config --set neuron_count=2000  # Set configuration parameter"
echo ""
echo "For more information about a specific mode, use:"
echo "  nlm <mode> --help"