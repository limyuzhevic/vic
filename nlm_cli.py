#!/usr/bin/env python3

"""
NLM Command Line Interface

A user-friendly command-line interface for the Neural Learning Machine (NLM)
neural simulation framework. This CLI provides common operations for creating,
running, and managing neural simulations.

Usage:
    python nlm_cli.py <command> [options]

Commands:
    create <name>              Create a new brain simulation
    run <name> [steps]         Run an existing simulation
    config <name> [file]       Manage configuration files
    benchmark <neurons> [steps] Run performance benchmark
    help                       Show help information
    version                    Show version information
"""

import sys
import os
import json
import argparse
from pathlib import Path
from typing import Optional, List, Dict, Any

# Add current directory to Python path for NLM imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    NLM_AVAILABLE = True
except ImportError as e:
    print(f"Error: Could not import NLM: {e}")
    print("Please ensure NLM is properly installed and available in your Python path.")
    NLM_AVAILABLE = False

class NLMCLI:
    """Command-line interface for NLM operations."""
    
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.brain_dir = self.project_root / "brains"
        self.config_dir = self.project_root / "configs"
        
        # Create directories if they don't exist
        self.brain_dir.mkdir(exist_ok=True)
        self.config_dir.mkdir(exist_ok=True)
    
    def create_brain(self, name: str, neurons: int = 500, steps: int = 100) -> None:
        """Create a new brain simulation."""
        if not NLM_AVAILABLE:
            print("NLM is not available. Please check your installation.")
            return
        
        print(f"Creating brain simulation '{name}' with {neurons} neurons for {steps} steps...")
        
        try:
            # Create configuration
            config = pynlm.create_default_config()
            config.set("neuron_count", neurons)
            config.set("region_count", 1)
            config.set("connection_probability", 0.1f)
            
            # Save configuration
            config_file = self.config_dir / f"{name}_config.json"
            pynlm.save_config(config, str(config_file))
            print(f"  Configuration saved to: {config_file}")
            
            # Create and run brain
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            for i in range(steps):
                brain.step(i, i * 0.001)
            
            # Save brain state
            brain_file = self.brain_dir / f"{name}.bin"
            brain.save(str(brain_file))
            print(f"  Brain state saved to: {brain_file}")
            
            # Print statistics
            print(f"  Statistics:")
            print(f"    Neurons: {brain.get_total_neuron_count()}")
            print(f"    Synapses: {brain.get_total_synapse_count()}")
            print(f"    Firing neurons: {brain.get_firing_neuron_count()}")
            print(f"    Total spikes: {brain.get_total_spike_count()}")
            print(f"    Average firing rate: {brain.get_average_firing_rate():.2f}")
            
            print(f"✓ Brain simulation '{name}' completed successfully!")
            
        except Exception as e:
            print(f"✗ Error creating brain simulation '{name}': {e}")
            raise
    
    def run_simulation(self, name: str, steps: Optional[int] = None, use_saved: bool = False) -> None:
        """Run an existing simulation or continue from saved state."""
        if not NLM_AVAILABLE:
            print("NLM is not available. Please check your installation.")
            return
        
        print(f"Running simulation '{name}'...")
        
        try:
            # Load existing brain or create new
            brain_file = self.brain_dir / f"{name}.bin"
            config_file = self.config_dir / f"{name}_config.json"
            
            if use_saved and brain_file.exists() and config_file.exists():
                print(f"  Loading saved brain from: {brain_file}")
                
                # Load configuration
                config = pynlm.load_config(str(config_file))
                
                # Create brain and load state
                brain = pynlm.create_brain(config)
                brain.initialize()
                brain.load(str(brain_file))
                
                if steps is not None:
                    print(f"  Running {steps} additional steps...")
                    for i in range(steps):
                        brain.step(i, i * 0.001)
            else:
                print(f"  Creating new brain simulation...")
                brain = pynlm.create_brain()
                brain.initialize()
                
                if steps is None:
                    steps = 100
                
                print(f"  Running {steps} steps...")
                for i in range(steps):
                    brain.step(i, i * 0.001)
                
                # Save configuration and brain
                config_file.write_text(json.dumps({
                    "neuron_count": brain.get_total_neuron_count(),
                    "region_count": 1,
                    "connection_probability": 0.1f,
                    "stdp_ltp_weight": 0.01f,
                    "stdp_ltd_weight": 0.012f,
                    "random_seed": 42
                }, indent=2))
                
                config_file.write_text(json.dumps({
                    "neuron_count": brain.get_total_neuron_count(),
                    "region_count": 1,
                    "connection_probability": 0.1f,
                    "stdp_ltp_weight": 0.01f,
                    "stdp_ltd_weight": 0.012f,
                    "random_seed": 42
                }, indent=2))
                
                brain.save(str(brain_file))
                
                config_file.write_text(json.dumps({
                    "neuron_count": brain.get_total_neuron_count(),
                    "region_count": 1,
                    "connection_probability": 0.1f,
                    "stdp_ltp_weight": 0.01f,
                    "stdp_ltd_weight": 0.012f,
                    "random_seed": 42
                }, indent=2))
            
            # Print final statistics
            print(f"  Final statistics:")
            print(f"    Neurons: {brain.get_total_neuron_count()}")
            print(f"    Synapses: {brain.get_total_synapse_count()}")
            print(f"    Firing neurons: {brain.get_firing_neuron_count()}")
            print(f"    Total spikes: {brain.get_total_spike_count()}")
            print(f"    Average firing rate: {brain.get_average_firing_rate():.2f}")
            print(f"    Excitation/Inhibition ratio: {brain.get_excitation_inhibition_ratio():.3f}")
            
            print(f"✓ Simulation '{name}' completed successfully!")
            
        except Exception as e:
            print(f"✗ Error running simulation '{name}': {e}")
            raise
    
    def manage_config(self, name: str, action: str, filepath: Optional[str] = None) -> None:
        """Manage configuration files."""
        if not NLM_AVAILABLE:
            print("NLM is not available. Please check your installation.")
            return
        
        print(f"Managing configuration '{name}'...")
        
        try:
            if action == "create":
                if filepath is None:
                    filepath = str(self.config_dir / f"{name}.json")
                
                config = pynlm.create_default_config()
                config.set("neuron_count", 500)
                config.set("region_count", 1)
                config.set("connection_probability", 0.1f)
                config.set("stdp_ltp_weight", 0.01f)
                config.set("stdp_ltd_weight", 0.012f)
                
                pynlm.save_config(config, filepath)
                print(f"✓ Configuration created: {filepath}")
                
            elif action == "load":
                if filepath is None:
                    filepath = str(self.config_dir / f"{name}.json")
                
                config = pynlm.load_config(filepath)
                print(f"✓ Configuration loaded: {filepath}")
                print(f"  Keys: {config.get_keys()}")
                
            elif action == "show":
                if filepath is None:
                    filepath = str(self.config_dir / f"{name}.json")
                
                config = pynlm.load_config(filepath)
                print(f"Configuration from {filepath}:")
                for key in config.get_keys():
                    print(f"  {key}")
                    
            elif action == "validate":
                if filepath is None:
                    filepath = str(self.config_dir / f"{name}.json")
                
                config = pynlm.load_config(filepath)
                warnings = pynlm.utils.validate_config(config)
                
                if warnings:
                    print(f"Warnings for configuration:")
                    for warning in warnings:
                        print(f"  - {warning}")
                else:
                    print(f"✓ Configuration is valid")
                    
            else:
                print(f"✗ Unknown action: {action}")
                
        except Exception as e:
            print(f"✗ Error managing configuration '{name}': {e}")
            raise
    
    def benchmark(self, neurons: int, steps: int = 1000) -> None:
        """Run performance benchmark."""
        if not NLM_AVAILABLE:
            print("NLM is not available. Please check your installation.")
            return
        
        print(f"Running benchmark: {neurons} neurons, {steps} steps...")
        
        try:
            config = pynlm.create_default_config()
            config.set("neuron_count", neurons)
            
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            # Run benchmark
            import time
            start_time = time.time()
            
            for i in range(steps):
                brain.inject_current_to_neurons(pynlm.constants.NEURON_TYPES.SENSORY, 10.0f)
                brain.step(i, i * 0.001)
            
            end_time = time.time()
            total_time = end_time - start_time
            
            # Calculate metrics
            steps_per_second = steps / total_time
            spikes_per_step = brain.get_total_spike_count() / steps if steps > 0 else 0
            synapses_per_neuron = brain.get_total_synapse_count() / brain.get_total_neuron_count() if brain.get_total_neuron_count() > 0 else 0
            
            print(f"\nBenchmark Results:")
            print(f"  Neurons: {brain.get_total_neuron_count()}")
            print(f"  Synapses: {brain.get_total_synapse_count()}")
            print(f"  Total spikes: {brain.get_total_spike_count()}")
            print(f"  Total time: {total_time:.2f} seconds")
            print(f"  Steps per second: {steps_per_second:.0f}")
            print(f"  Spikes per step: {spikes_per_step:.2f}")
            print(f"  Synapses per neuron: {synapses_per_neuron:.2f}")
            
            print(f"\n✓ Benchmark completed successfully!")
            
        except Exception as e:
            print(f"✗ Error running benchmark: {e}")
            raise
    
    def help(self, command: Optional[str] = None) -> None:
        """Show help information."""
        if command is None:
            print(__doc__)
            return
        
        commands = {
            "create": """
Create a new brain simulation.

Usage: nlm create <name> [options]

Options:
  --neurons N     Number of neurons (default: 500)
  --steps S       Number of simulation steps (default: 100)

Example:
  nlm create my_brain --neurons 1000 --steps 500
""",
            "run": """
Run an existing simulation.

Usage: nlm run <name> [options]

Options:
  --steps S       Number of steps to run (default: 100)
  --load          Load from saved state instead of creating new

Example:
  nlm run my_brain --steps 200
  nlm run my_brain --load --steps 500
""",
            "config": """
Manage configuration files.

Usage: nlm config <name> <action> [options]

Actions:
  create [file]   Create new configuration file
  load [file]     Load existing configuration file
  show [file]     Display configuration contents
  validate [file] Validate configuration

Options:
  --file F        Configuration file path (default: configs/<name>.json)

Example:
  nlm config my_config create
  nlm config my_config show
  nlm config my_config validate
""",
            "benchmark": """
Run performance benchmark.

Usage: nlm benchmark <neurons> [options]

Options:
  --steps S       Number of steps (default: 1000)

Example:
  nlm benchmark 1000
  nlm benchmark 500 --steps 2000
"""
        }
        
        if command in commands:
            print(commands[command])
        else:
            print(f"Unknown command: {command}")
            print("\nAvailable commands:")
            for cmd in commands.keys():
                print(f"  {cmd}")
    
    def version(self) -> None:
        """Show version information."""
        print("NLM Command Line Interface v1.0.0")
        print("Neural Learning Machine - CLI Tools")
        print("© 2026 NLM Project")
        print("")
        print("For more information, visit: https://github.com/nlm-project/nlm")

def main() -> None:
    """Main entry point for the NLM CLI."""
    cli = NLMCLI()
    
    # Create argument parser
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    
    # Add subcommands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create command
    create_parser = subparsers.add_parser("create", help="Create a new brain simulation")
    create_parser.add_argument("name", help="Name of the brain simulation")
    create_parser.add_argument("--neurons", type=int, default=500, help="Number of neurons")
    create_parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    
    # Run command
    run_parser = subparsers.add_parser("run", help="Run an existing simulation")
    run_parser.add_argument("name", help="Name of the simulation")
    run_parser.add_argument("--steps", type=int, help="Number of steps to run")
    run_parser.add_argument("--load", action="store_true", help="Load from saved state")
    
    # Config command
    config_parser = subparsers.add_parser("config", help="Manage configuration files")
    config_parser.add_argument("name", help="Name of the configuration")
    config_parser.add_argument("action", help="Action to perform (create, load, show, validate)")
    config_parser.add_argument("--file", help="Configuration file path")
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser("benchmark", help="Run performance benchmark")
    benchmark_parser.add_argument("neurons", type=int, help="Number of neurons")
    benchmark_parser.add_argument("--steps", type=int, default=1000, help="Number of steps")
    
    # Help command
    help_parser = subparsers.add_parser("help", help="Show help information")
    help_parser.add_argument("command", nargs="?", help="Specific command to get help for")
    
    # Version command
    version_parser = subparsers.add_parser("version", help="Show version information")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute command
    try:
        if args.command == "create":
            cli.create_brain(args.name, args.neurons, args.steps)
        elif args.command == "run":
            cli.run_simulation(args.name, args.steps, args.load)
        elif args.command == "config":
            cli.manage_config(args.name, args.action, args.file)
        elif args.command == "benchmark":
            cli.benchmark(args.neurons, args.steps)
        elif args.command == "help":
            cli.help(args.command)
        elif args.command == "version":
            cli.version()
        else:
            parser.print_help()
            return
            
    except KeyboardInterrupt:
        print("\nOperation cancelled by user.")
        return
    except Exception as e:
        print(f"\nError: {e}")
        return
if __name__ == "__main__":
    main()