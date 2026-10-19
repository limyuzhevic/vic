"""
Main entry point for the NLM CLI.

This module provides the command-line interface for the NLM system,
including basic commands and advanced development commands.
"""

import sys
from pathlib import Path

# Add the advanced module to the path
sys.path.insert(0, str(Path(__file__).parent))

from advanced.cli import AdvancedCLI
from basic.cli import BasicCLI

def main():
    """Main entry point for the CLI."""
    # Check if we're running advanced commands
    if "--advanced" in sys.argv or "advanced" in sys.argv:
        cli = AdvancedCLI()
        sys.exit(cli.run(sys.argv[1:]))
    else:
        # Run basic CLI for backwards compatibility
        cli = BasicCLI()
        sys.exit(cli.run(sys.argv[1:]))

if __name__ == "__main__":
    main()
