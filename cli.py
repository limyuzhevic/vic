#!/usr/bin/env python3
"""
NLM CLI - Command Line Interface for NLM Brain Simulation Project
Main entry point for the NLM CLI application.
"""

import sys
import os

# Add the src directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

from nlm_cli import NLMCLI

def main():
    """Main entry point for the NLM CLI."""
    try:
        cli = NLMCLI()
        cli.run()
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()