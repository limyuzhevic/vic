"""
Main entry point for the enhanced NLM CLI.
This module provides a unified command-line interface for NLM operations.
"""

import sys
from pathlib import Path

# Add the nlm module to the path
sys.path.insert(0, str(Path(__file__).parent))

from nlm.advanced_cli import cli as advanced_cli
from nlm.cli import cli as basic_cli

# Create a unified CLI that combines both
import click

@click.group()
@click.version_option("0.1.0-enhanced")
@click.option("--verbose", "-v", is_flag=True, help="Enable verbose output")
@click.option("--log-file", "-l", help="Log output to file")
def main(verbose, log_file):
    """NLM - Neural Learning Machine Simulation Framework
    
    Enhanced CLI with advanced features for running neural simulations.
    
    Examples:
        nlm-sim 1000                          # Run simple simulation
        nlm-sim 1000 --world --learn          # Run with world and learning
        nlm-explore                           # Run exploration demo
        nlm-learn 500 --world-size 15        # Run learning experiment
    """
    pass

# Add basic CLI commands
main.add_command(basic_cli, name="basic")
main.add_command(advanced_cli, name="advanced")

# Add aliases for convenience
main.add_command(basic_cli, name="sim")
main.add_command(basic_cli, name="explore")
main.add_command(basic_cli, name="learn")

if __name__ == "__main__":
    main()
