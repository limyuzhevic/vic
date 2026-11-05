#!/usr/bin/env python3
"""
Unified CLI entry point for NLM (Neural Learning Machine) command-line tools.

This module provides a unified interface for all NLM CLI tools with common
arguments like --help, --version, --verbose, and JSON output support.
"""

import argparse
import json
import logging
import sys
import time
from pathlib import Path
from typing import Any, Dict, List, Optional

# Add the current directory to Python path for pynlm import
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
except ImportError:
    print("ERROR: pynlm module not found. Please ensure the package is installed.")
    sys.exit(1)


def setup_logging(verbose: bool = False) -> None:
    """Configure logging based on verbosity level."""
    level = logging.DEBUG if verbose else logging.INFO
    logging.basicConfig(
        level=level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )


def output_json(data: Any, pretty: bool = False) -> None:
    """Output data as JSON with optional pretty formatting."""
    try:
        if pretty:
            json.dump(data, sys.stdout, indent=2, default=str)
        else:
            json.dump(data, sys.stdout, default=str)
        sys.stdout.write('\n')
        sys.stdout.flush()
    except Exception as e:
        logging.error(f"Failed to output JSON: {e}")
        sys.exit(1)


def error_json(message: str, error_code: int = 1) -> None:
    """Output error as JSON."""
    error_data = {
        "success": False,
        "error": message,
        "error_code": error_code
    }
    output_json(error_data)
    sys.exit(error_code)


def success_json(data: Any, message: Optional[str] = None) -> None:
    """Output success as JSON."""
    result = {"success": True}
    if message:
        result["message"] = message
    result.update(data)
    output_json(result)


def get_version() -> str:
    """Get version information."""
    try:
        import pynlm
        version = getattr(pynlm, '__version__', '0.1.0')
        return version
    except:
        return '0.1.0'


def create_subparsers() -> argparse.ArgumentParser:
    """Create the main argument parser with subcommands."""
    parser = argparse.ArgumentParser(
        prog='nlm',
        description='NLM (Neural Learning Machine) Command Line Interface',
        epilog='Use "nlm <command> --help" for detailed help on specific commands\n'
               'Available commands: brain, experiment, performance, config, visualize'
    )
    
    # Global arguments
    parser.add_argument(
        '--version',
        action='version',
        version=f'NLM CLI version {get_version()}'
    )
    
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    parser.add_argument(
        '--json',
        action='store_true',
        help='Output results as JSON'
    )
    
    parser.add_argument(
        '--pretty',
        action='store_true',
        help='Pretty-print JSON output (only with --json)'
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Brain management subcommand
    brain_parser = subparsers.add_parser('brain', help='Brain management operations')
    brain_parser.set_defaults(func=lambda args: brain_parser.print_help())
    
    # Experiment management subcommand
    experiment_parser = subparsers.add_parser('experiment', help='Experiment operations')
    experiment_parser.set_defaults(func=lambda args: experiment_parser.print_help())
    
    # Performance management subcommand
    performance_parser = subparsers.add_parser('performance', help='Performance profiling and benchmarking')
    performance_parser.set_defaults(func=lambda args: performance_parser.print_help())
    
    # Configuration management subcommand
    config_parser = subparsers.add_parser('config', help='Configuration management')
    config_parser.set_defaults(func=lambda args: config_parser.print_help())
    
    # Visualization subcommand
    visualize_parser = subparsers.add_parser('visualize', help='Visualization and analysis')
    visualize_parser.set_defaults(func=lambda args: visualize_parser.print_help())
    
    return parser, subparsers


def main() -> int:
    """Main entry point for the NLM CLI."""
    parser, subparsers = create_subparsers()
    
    # Parse arguments
    args = parser.parse_args()
    
    # Setup logging based on verbosity
    setup_logging(args.verbose if 'verbose' in args.__dict__ else False)
    
    # Execute command
    if hasattr(args, 'command') and args.command:
        # Dispatch to appropriate subcommand module
        try:
            module = __import__(f"nlm_cli.{args.command}", fromlist=[args.command])
            return module.main(args)
        except ImportError:
            error_json(f"Unknown command: {args.command}", 2)
    else:
        parser.print_help()
        return 0


if __name__ == '__main__':
    sys.exit(main())