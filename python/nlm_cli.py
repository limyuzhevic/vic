#!/usr/bin/env python3
"""
NLM Command Line Interface

Provides a rich CLI for working with NLM (Neural Learning Machine)
including subcommands for different operations and enhanced configuration.
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Dict, Any, Optional

# Add the python module to path
sys.path.insert(0, str(Path(__file__).parent))

from pynlm import Brain, Config, Experiment, Phase6Integration, Metrics
from pynlm.core import Logger, SimulationClock


def create_parser():
    """Create the main argument parser with subcommands."""
    parser = argparse.ArgumentParser(
        description="NLM - Neural Learning Machine: Experimental Artificial Developmental Brain",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run Phase 6 integration test
  nlm phase6-demo --neurons 1000 --steps 2000

  # Create and simulate a brain
  nlm create --neurons 500 --output brain.bin
  nlm simulate brain.bin --steps 1000

  # Run learning experiment
  nlm experiment --config experiment.json --train

  # List available configurations
  nlm config list

  # Show system information
  nlm info

Configuration files can be provided with --config <file>.
Additional settings can be specified as --key value pairs.
        """
    )
    
    parser.add_argument(
        "--config", "-c",
        help="Configuration file (JSON or .cfg)"
    )
    
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--log-level",
        choices=["debug", "info", "warn", "error"],
        default="info",
        help="Set logging level"
    )
    
    parser.add_argument(
        "--checkpoint",
        help="Checkpoint file for save/load operations"
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(
        dest="command",
        title="Commands",
        description="Available NLM commands",
        help="Command to execute"
    )
    
    # Info command
    add_info_command(subparsers)
    
    # Config command
    add_config_command(subparsers)
    
    # Create command
    add_create_command(subparsers)
    
    # Simulate command
    add_simulate_command(subparsers)
    
    # Phase 6 command
    add_phase6_command(subparsers)
    
    # Experiment command
    add_experiment_command(subparsers)
    
    # Compare command
    add_compare_command(subparsers)
    
    return parser


def add_info_command(subparsers):
    """Add the info command."""
    parser = subparsers.add_parser(
        "info",
        help="Show system information and capabilities",
        description="Display information about NLM installation and system capabilities"
    )
    
    parser.add_argument(
        "--format",
        choices=["text", "json"],
        default="text",
        help="Output format"
    )


def add_config_command(subparsers):
    """Add the config command."""
    parser = subparsers.add_parser(
        "config",
        help="Manage configurations",
        description="Commands for managing NLM configuration files and settings"
    )
    
    config_subparsers = parser.add_subparsers(
        dest="config_command",
        title="Config Commands",
        description="Configuration management commands",
        help="Configuration command to execute"
    )
    
    # List command
    list_parser = config_subparsers.add_parser(
        "list",
        help="List available configurations",
        description="List all available configuration files in the configs directory"
    )
    
    list_parser.add_argument(
        "--all",
        action="store_true",
        help="Show all configurations including system defaults"
    )
    
    # Show command
    show_parser = config_subparsers.add_parser(
        "show",
        help="Show configuration details",
        description="Show detailed information about a specific configuration"
    )
    
    show_parser.add_argument(
        "name",
        help="Configuration name"
    )
    
    # Create command
    create_parser = config_subparsers.add_parser(
        "create",
        help="Create a new configuration",
        description="Create a new configuration file with specified parameters"
    )
    
    create_parser.add_argument(
        "name",
        help="Configuration name"
    )
    
    create_parser.add_argument(
        "--from-template",
        help="Create from a template configuration"
    )


def add_create_command(subparsers):
    """Add the create command."""
    parser = subparsers.add_parser(
        "create",
        help="Create a new brain simulation",
        description="Create a new NLM brain with specified parameters and initialize it"
    )
    
    parser.add_argument(
        "--neurons", "-n",
        type=int,
        default=100,
        help="Number of neurons (default: 100)"
    )
    
    parser.add_argument(
        "--regions", "-r",
        type=int,
        default=1,
        help="Number of neural regions (default: 1)"
    )
    
    parser.add_argument(
        "--probability", "-p",
        type=float,
        default=0.15,
        help="Connection probability between neurons (default: 0.15)"
    )
    
    parser.add_argument(
        "--output", "-o",
        help="Output file path for saving the brain"
    )
    
    parser.add_argument(
        "--seed",
        type=int,
        default=42,
        help="Random seed for reproducible results (default: 42)"
    )


def add_simulate_command(subparsers):
    """Add the simulate command."""
    parser = subparsers.add_parser(
        "simulate",
        help="Simulate brain activity",
        description="Run neural simulation with the specified brain configuration"
    )
    
    parser.add_argument(
        "input",
        help="Input brain file or configuration"
    )
    
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=1000,
        help="Number of simulation steps (default: 1000)"
    )
    
    parser.add_argument(
        "--dt",
        type=float,
        default=0.001,
        help="Simulation timestep in seconds (default: 0.001)"
    )
    
    parser.add_argument(
        "--output", "-o",
        help="Output file path for saving results"
    )
    
    parser.add_argument(
        "--metrics",
        action="store_true",
        help="Collect and display performance metrics"
    )


def add_phase6_command(subparsers):
    """Add the phase6 command."""
    parser = subparsers.add_parser(
        "phase6-demo",
        help="Run Phase 6 integration demo",
        description="Run the Phase 6 final integration test demonstrating all brain systems"
    )
    
    parser.add_argument(
        "--neurons", "-n",
        type=int,
        default=500,
        help="Number of neurons for demo (default: 500)"
    )
    
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=2000,
        help="Maximum simulation steps (default: 2000)"
    )
    
    parser.add_argument(
        "--checkpoint",
        action="store_true",
        help="Enable checkpointing for save/load testing"
    )
    
    parser.add_argument(
        "--replay",
        action="store_true",
        help="Enable replay testing"
    )
    
    parser.add_argument(
        "--development",
        action="store_true",
        help="Enable development system testing"
    )
    
    parser.add_argument(
        "--output", "-o",
        help="Output file for demo results"
    )


def add_experiment_command(subparsers):
    """Add the experiment command."""
    parser = subparsers.add_parser(
        "experiment",
        help="Run learning experiments",
        description="Run learning experiments with the specified configuration"
    )
    
    parser.add_argument(
        "input",
        help="Input configuration file or brain"
    )
    
    parser.add_argument(
        "--type", "-t",
        choices=["learning", "plasticity", "plasticity-stdp", "development"],
        default="learning",
        help="Experiment type (default: learning)"
    )
    
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=1000,
        help="Number of simulation steps (default: 1000)"
    )
    
    parser.add_argument(
        "--iterations", "-i",
        type=int,
        default=1,
        help="Number of experiment iterations (default: 1)"
    )
    
    parser.add_argument(
        "--output", "-o",
        help="Output file for experiment results"
    )
    
    parser.add_argument(
        "--compare",
        action="store_true",
        help="Compare with baseline performance"
    )


def add_compare_command(subparsers):
    """Add the compare command."""
    parser = subparsers.add_parser(
        "compare",
        help="Compare different brain configurations",
        description="Run comparative experiments with different configurations"
    )
    
    parser.add_argument(
        "configs",
        nargs="+",
        help="Configuration files to compare"
    )
    
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=1000,
        help="Number of simulation steps (default: 1000)"
    )
    
    parser.add_argument(
        "--metric", "-m",
        choices=["reward", "spikes", "weight-change", "learning"],
        default="reward",
        help="Primary metric to compare (default: reward)"
    )


def load_config_from_args(config_file, args):
    """Load configuration from file and command line arguments."""
    config = {}
    
    # Load from file if specified
    if config_file:
        try:
            if config_file.endswith('.json'):
                with open(config_file, 'r') as f:
                    config.update(json.load(f))
            elif config_file.endswith('.cfg'):
                # Parse simple .cfg format
                with open(config_file, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if line and not line.startswith('#') and '=' in line:
                            key, value = line.split('=', 1)
                            config[key.strip()] = float(value.strip()) if '.' in value else int(value.strip())
        except Exception as e:
            print(f"Warning: Could not load config file {config_file}: {e}")
    
    return config


def setup_logging(log_level):
    """Set up logging based on log level."""
    level_map = {
        'debug': 0,
        'info': 1,
        'warn': 2,
        'error': 3
    }
    
    logger = Logger()
    logger.set_level(level_map.get(log_level, 1))
    return logger


def main():
    """Main entry point for the NLM CLI."""
    parser = create_parser()
    args = parser.parse_args()
    
    # Set up logging
    logger = setup_logging(args.log_level)
    
    try:
        # Dispatch to command handlers
        if args.command == "info":
            handle_info_command(args)
        
        elif args.command == "config":
            if args.config_command == "list":
                handle_config_list_command(args)
            elif args.config_command == "show":
                handle_config_show_command(args)
            elif args.config_command == "create":
                handle_config_create_command(args)
        
        elif args.command == "create":
            handle_create_command(args)
        
        elif args.command == "simulate":
            handle_simulate_command(args)
        
        elif args.command == "phase6-demo":
            handle_phase6_demo_command(args)
        
        elif args.command == "experiment":
            handle_experiment_command(args)
        
        elif args.command == "compare":
            handle_compare_command(args)
        
        else:
            parser.print_help()
            return 1
    
    except KeyboardInterrupt:
        print("\nOperation interrupted by user")
        return 130
    
    except Exception as e:
        logger.error(f"Error: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1
    
    return 0


def handle_info_command(args):
    """Handle the info command."""
    info = {
        "name": "NLM - Neural Learning Machine",
        "version": "0.1.0",
        "description": "Experimental Artificial Developmental Brain",
        "phase": "6 (Final Integration)",
        "systems": [
            "Memory (Working, Episodic, Associative)",
            "Neuromodulation (Dopamine, Curiosity, Novelty)",
            "Prediction",
            "Cognition (Planning, Concepts, Attention)",
            "Development",
            "Plasticity (STDP, Hebbian, Structural)"
        ],
        "capabilities": [
            "Neural Simulation",
            "Learning from Experience",
            "Memory Consolidation",
            "Predictive Processing",
            "Developmental Plasticity",
            "Checkpoint Persistence"
        ],
        "python_bindings": True,
        "command_line_interface": True,
        "visualization": True
    }
    
    if args.format == "json":
        print(json.dumps(info, indent=2))
    else:
        print("=== NLM System Information ===")
        print(f"Name: {info['name']}")
        print(f"Version: {info['version']}")
        print(f"Phase: {info['phase']}")
        print(f"Description: {info['description']}")
        print("\n=== Integrated Systems ===")
        for system in info['systems']:
            print(f"  ✓ {system}")
        print("\n=== Capabilities ===")
        for capability in info['capabilities']:
            print(f"  ✓ {capability}")
        print(f"\nPython Bindings: {info['python_bindings']}")
        print(f"Command Line Interface: {info['command_line_interface']}")
        print(f"Visualization: {info['visualization']}")


def handle_config_list_command(args):
    """Handle the config list command."""
    config_dir = Path("configs")
    if not config_dir.exists():
        print("No configs directory found")
        return
    
    config_files = list(config_dir.glob("*.cfg")) + list(config_dir.glob("*.json"))
    
    if args.all:
        # Include system defaults
        default_configs = ["default.cfg", "phase6_default.json"]
        print("=== System Default Configurations ===")
        for default in default_configs:
            print(f"  {default}")
    
    if config_files:
        print("=== User Configurations ===")
        for config_file in sorted(config_files):
            print(f"  {config_file.name}")
    else:
        print("No user configurations found")


def handle_config_show_command(args):
    """Handle the config show command."""
    print(f"Showing configuration: {args.name}")
    print("(Configuration system will be implemented)")


def handle_config_create_command(args):
    """Handle the config create command."""
    print(f"Creating configuration: {args.name}")
    print(f"From template: {args.from_template or 'default'}")
    print("(Configuration system will be implemented)")


def handle_create_command(args):
    """Handle the create command."""
    print(f"Creating brain with {args.neurons} neurons...")
    
    config = {
        "neuron_count": args.neurons,
        "region_count": args.regions,
        "connection_probability": args.probability,
        "random_seed": args.seed
    }
    
    if args.output:
        print(f"Saving to: {args.output}")
        # In real implementation, this would call Brain(config).save(args.output)
    
    print(f"Configuration: {json.dumps(config, indent=2)}")
    print("(Brain creation will be implemented in the next phase)")


def handle_simulate_command(args):
    """Handle the simulate command."""
    print(f"Simulating brain from: {args.input}")
    print(f"Steps: {args.steps}")
    print(f"Timestep: {args.dt}")
    
    if args.output:
        print(f"Output: {args.output}")
    
    if args.metrics:
        print("Collecting metrics...")
    
    print("(Simulation will be implemented in the next phase)")


def handle_phase6_demo_command(args):
    """Handle the phase6-demo command."""
    print("=== NLM Phase 6 Integration Demo ===")
    print(f"Neurons: {args.neurons}")
    print(f"Max Steps: {args.steps}")
    print(f"Checkpointing: {args.checkpoint}")
    print(f"Replay: {args.replay}")
    print(f"Development: {args.development}")
    print("(Phase 6 demo will be implemented in the next phase)")


def handle_experiment_command(args):
    """Handle the experiment command."""
    print(f"Running {args.type} experiment")
    print(f"Input: {args.input}")
    print(f"Steps: {args.steps}")
    print(f"Iterations: {args.iterations}")
    
    if args.compare:
        print("Comparing with baseline...")
    
    print("(Experiment will be implemented in the next phase)")


def handle_compare_command(args):
    """Handle the compare command."""
    print(f"Comparing configurations: {', '.join(args.configs)}")
    print(f"Steps: {args.steps}")
    print(f"Primary metric: {args.metric}")
    print("(Comparison will be implemented in the next phase)")


if __name__ == "__main__":
    sys.exit(main())
