#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Command Line Interface

A comprehensive CLI tool for interacting with the NLM neural simulation framework.
This tool provides access to all NLM functionality from the command line.
"""

import argparse
import sys
import os
from pathlib import Path

# Add the project root to Python path
project_root = Path(__file__).parent
if str(project_root) not in sys.path:
    sys.path.insert(0, str(project_root))

# Import CLI modules
try:
    import pynlm
    from cli_utils import NLMCLIError
    import cli_commands
    NLM_AVAILABLE = True
except ImportError as e:
    NLM_AVAILABLE = False
    NLM_ERROR = str(e)

def print_banner():
    """Print the NLM CLI banner"""
    print("╔══════════════════════════════════════════════════════════════╗")
    print("║                                                              ║")
    print("║     NLM — 熙然                                                ║")
    print("║     Neural Learning Machine                                   ║")
    print("║                                                              ║")
    print("║     Phase 6: Final Integration                                ║")
    print("║     Command Line Interface                                    ║")
    print("║                                                              ║")
    print("╚══════════════════════════════════════════════════════════════╝")
    print()

def print_error(message: str):
    """Print error message to stderr"""
    print(f"ERROR: {message}", file=sys.stderr)

def main():
    """Main CLI entry point"""
    # Print banner
    print_banner()
    
    # Check if pynlm is available
    if not NLM_AVAILABLE:
        print_error(f"Could not import pynlm: {NLM_ERROR}")
        print("Make sure NLM is properly installed: pip install -e .")
        sys.exit(1)
    
    try:
        parser = create_parser()
        args = parser.parse_args()
        
        # Dispatch to command handler
        success = cli_commands.handle_command(args)
        
        if not success:
            sys.exit(1)
    
    except cli_commands.NLMCLIError as e:
        print_error(str(e))
        sys.exit(1)
    except Exception as e:
        print_error(f"Unexpected error: {e}")
        sys.exit(1)

def create_parser():
    """Create the main argument parser"""
    parser = argparse.ArgumentParser(
        prog="nlm",
        description="NLM (Neural Learning Machine) CLI - Command line interface for neural simulation",
        epilog="""Examples:
          nlm run --steps 1000                    # Run a simple simulation
          nlm config show                         # Show current configuration
          nlm config set --key neuron_count --value 500  # Set a config value
          nlm save brain_state.bin                # Save brain state to file
          nlm load brain_state.bin                # Load brain state from file
          nlm stats                                # Show brain statistics
          nlm connectivity                        # Test neural connectivity
          nlm run --interactive                   # Run in interactive mode
        """,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        "-v", "--version",
        action="version",
        version="NLM CLI 0.1.0 (Phase 6)"
    )
    
    parser.add_argument(
        "-d", "--debug",
        action="store_true",
        help="Enable debug output"
    )
    
    parser.add_argument(
        "-c", "--config",
        default="configs/default.cfg",
        help="Path to configuration file (default: configs/default.cfg)"
    )
    
    # Create subparsers for commands
    subparsers = parser.add_subparsers(
        dest="command",
        title="Commands",
        description="Available commands",
        help="Command to execute"
    )
    
    # Add run command
    run_parser = subparsers.add_parser("run", help="Run a neural simulation")
    run_parser.add_argument(
        "--steps", type=int, default=100,
        help="Number of simulation steps to run (default: 100)"
    )
    run_parser.add_argument(
        "--verbose", action="store_true",
        help="Enable verbose output"
    )
    run_parser.add_argument(
        "--interactive", action="store_true",
        help="Run in interactive mode"
    )
    
    # Add config command
    config_parser = subparsers.add_parser("config", help="Manage configuration")
    config_subparsers = config_parser.add_subparsers(
        dest="subcommand",
        title="Config subcommands",
        description="Configuration commands",
        help="Configuration command to execute"
    )
    
    # Config show
    config_show_parser = config_subparsers.add_parser("show", help="Show current configuration")
    
    # Config set
    config_set_parser = config_subparsers.add_parser("set", help="Set a configuration value")
    config_set_parser.add_argument("--key", required=True, help="Configuration key")
    config_set_parser.add_argument("--value", required=True, help="Configuration value")
    config_set_parser.add_argument("--output", help="Output file path")
    
    # Config get
    config_get_parser = config_subparsers.add_parser("get", help="Get a configuration value")
    config_get_parser.add_argument("--key", required=True, help="Configuration key")
    
    # Config load
    config_load_parser = config_subparsers.add_parser("load", help="Load configuration from file")
    config_load_parser.add_argument("--input", required=True, help="Input file path")
    
    # Config save
    config_save_parser = config_subparsers.add_parser("save", help="Save configuration to file")
    config_save_parser.add_argument("--output", required=True, help="Output file path")
    
    # Config default
    config_subparsers.add_parser("default", help="Show default configuration")
    
    # Add other commands
    subparsers.add_parser("stats", help="Show brain statistics")
    subparsers.add_parser("save", help="Save brain state")
    subparsers.add_parser("load", help="Load brain state")
    subparsers.add_parser("connectivity", help="Test neural connectivity")
    subparsers.add_parser("help", help="Show help")
    subparsers.add_parser("interactive", help="Run in interactive mode")
    
    return parser

if __name__ == "__main__":
    main()