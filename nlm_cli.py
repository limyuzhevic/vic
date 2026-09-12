#!/usr/bin/env python3
"""
NLM Command Line Tools
======================

Command-line tools for working with the NLM (Neural Learning Machine) brain simulation.

Tools available:
- nlm-analyze    : Analyze brain connectivity and statistics
- nlm-experiment : Run predefined experiments
- nlm-visualize  : Visualize brain states and neural activity

Each tool provides command-line interface for common NLM operations.
"""

import argparse
import sys
from pathlib import Path

def main():
    parser = argparse.ArgumentParser(
        description="NLM Command Line Tools - Neural Learning Machine utilities",
        epilog="Use 'nlm-analyze --help', 'nlm-experiment --help', or 'nlm-visualize --help' for tool-specific help."
    )
    
    parser.add_argument('--version', action='version', version='NLM Command Line Tools 0.1.0')
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # nlm-analyze tool
    setup_analyze_tool(subparsers)
    
    # nlm-experiment tool  
    setup_experiment_tool(subparsers)
    
    # nlm-visualize tool
    setup_visualize_tool(subparsers)
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        sys.exit(1)
    
    # Dispatch to tool implementation
    if args.command == 'analyze':
        from .tools.analyze import main as analyze_main
        sys.exit(analyze_main())
    elif args.command == 'experiment':
        from .tools.experiment import main as experiment_main
        sys.exit(experiment_main())
    elif args.command == 'visualize':
        from .tools.visualize import main as visualize_main
        sys.exit(visualize_main())

if __name__ == '__main__':
    main()
