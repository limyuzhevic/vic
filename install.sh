#!/bin/bash
# NLM CLI Installer
# This script installs the NLM CLI commands and makes them executable

set -e

PROJECT_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BIN_DIR="$PROJECT_ROOT/bin"

# Create bin directory if it doesn't exist
mkdir -p "$BIN_DIR"

# Make all Python scripts in bin executable
for script in "$BIN_DIR"/*.py; do
    if [ -f "$script" ]; then
        chmod +x "$script"
        echo "Made executable: $script"
    fi
done

# Create symlink in user PATH if available
if [ -d "~/.local/bin" ]; then
    echo "Creating symlinks in ~/.local/bin..."
    for script in "$BIN_DIR"/*; do
        if [ -f "$script" ]; then
            basename=$(basename "$script")
            if [ ! -f "~/.local/bin/$basename" ]; then
                ln -sf "$script" "~/.local/bin/$basename"
                echo "Linked: ~/.local/bin/$basename"
            fi
done
elif [ -d "~/bin" ]; then
    echo "Creating symlinks in ~/bin..."
    for script in "$BIN_DIR"/*; do
        if [ -f "$script" ]; then
            basename=$(basename "$script")
            if [ ! -f "~/bin/$basename" ]; then
                ln -sf "$script" "~/bin/$basename"
                echo "Linked: ~/bin/$basename"
            fi
done
else
    echo "Warning: No standard user bin directory found. Please install commands manually."
    echo "Either create ~/.local/bin or ~/bin and add it to your PATH, or install the scripts directly to those directories."
fi

# Update the main help message
cat > "$BIN_DIR/nlm" << 'EOF'
#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

A comprehensive command-line interface for the NLM (Neural Learning Machine) simulation framework.
Provides commands for exploration, learning, simulation, analysis, benchmarking, debugging, profiling, and export.

Usage:
    nlm-explore [OPTIONS]              Explore NLM brain configurations interactively
    nlm-learn [OPTIONS]                Run learning simulations with presets
    nlm-simulate [OPTIONS]             Run simple simulations
    nlm-analyze [OPTIONS]              Analyze and visualize simulation results
    nlm-benchmark [OPTIONS]            Benchmark performance across configurations
    nlm-debug [OPTIONS]                Advanced debugging with breakpoints
    nlm-profile [OPTIONS]              Profile memory and performance
    nlm-export [OPTIONS]               Export results in various formats
    nlm --help                        Show this help message

For detailed help on any command, use: command --help
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from nlm_cli import main

if __name__ == '__main__':
    main()
EOF

chmod +x "$BIN_DIR/nlm"
echo "Created main NLM CLI entry point: $BIN_DIR/nlm"

echo "\nNLM CLI installation complete!"
echo "\nAvailable commands:"
echo "  nlm-explore  - Interactive exploration of NLM brain configurations"
echo "  nlm-learn    - Run learning simulations with presets"
echo "  nlm-simulate - Run simple simulations"
echo "  nlm-analyze  - Analyze and visualize simulation results"
echo "  nlm-benchmark - Benchmark performance across configurations"
echo "  nlm-debug    - Advanced debugging with breakpoints"
echo "  nlm-profile   - Profile memory and performance"
echo "  nlm-export   - Export results in various formats"
echo "\nTo install globally, add $BIN_DIR to your PATH environment variable."
