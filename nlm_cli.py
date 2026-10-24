#!/usr/bin/env python3
"""
NLM Command-Line Interface

A command-line tool for running NLM (Neural Learning Machine) simulations.

Features:
- Run simulations with different configurations
- Support for JSON, YAML, and simple config files
- Multiple simulation modes (basic, exploration, memory)
- Performance benchmarking
- Configuration management
- Export results and statistics

Usage:
    nlm [options] [command]

Commands:
    run           Run a simulation
    benchmark      Run a performance benchmark
    config         Configuration management
    help          Show help information

Options:
    --help, -h     Show help
    --version      Show version
    --verbose, -v  Enable verbose output
    --quiet, -q    Quiet mode
"""

import argparse
import sys
import os
from pathlib import Path
from typing import Optional, Dict, Any

# Add the parent directory to the path so we can import pynlm
import sys
sys.path.insert(0, str(Path(__file__).parent.parent))

import pynlm

class NLMCLI:
    """Command-line interface for NLM."""
    
    def __init__(self):
        self.version = "0.1.0"
        self.verbose = False
    
    def run_simulation(self, args):
        """Run a simulation with the specified configuration."""
        print(f"Running NLM simulation with {args.steps} steps...")
        
        # Load configuration
        config = self.load_configuration(args.config)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
        print(f"Total synapses: {brain.getTotalSynapseCount()}")
        
        # Run simulation
        for step in range(args.steps):
            brain.step(step)
            
            if args.verbose and (step + 1) % 10 == 0:
                print(f"Step {step + 1}: {brain.getFiringNeuronCount()} firing neurons")
            elif (step + 1) % 100 == 0 and not args.verbose:
                print(f"Progress: {((step + 1) / args.steps) * 100:.1f}%")
        
        # Print results
        print(f"\nSimulation completed!")
        print(f"Total steps: {args.steps}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Active neurons: {brain.getActiveNeuronCount()}")
        print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
        
        # Save results if requested
        if args.output:
            self.save_results(brain, args.output)
        
        # Clean up
        brain.reset()
        
        return True
    
    def benchmark_brain(self, args):
        """Run a performance benchmark."""
        print(f"Running NLM benchmark with {args.steps} steps...")
        
        # Load configuration
        config = self.load_configuration(args.config)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run benchmark
        import time
        start_time = time.time()
        
        for step in range(args.steps):
            brain.step(step)
        
        end_time = time.time()
        
        # Calculate statistics
        duration = end_time - start_time
        steps_per_second = args.steps / duration if duration > 0 else 0
        
        # Print results
        print(f"\nBenchmark Results:")
        print(f"  Configuration: {args.config}")
        print(f"  Total steps: {args.steps}")
        print(f"  Duration: {duration:.2f} seconds")
        print(f"  Steps per second: {steps_per_second:.2f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Time per step: {duration / args.steps * 1000:.2f} ms")
        
        # Save benchmark results if requested
        if args.output:
            self.save_benchmark_results(brain, duration, steps_per_second, args.output)
        
        # Clean up
        brain.reset()
        
        return True
    
    def manage_configuration(self, args):
        """Manage configuration files."""
        if args.action == "create":
            self.create_configuration(args.name, args.type)
        elif args.action == "list":
            self.list_configurations()
        elif args.action == "show":
            self.show_configuration(args.name)
        elif args.action == "delete":
            self.delete_configuration(args.name)
        else:
            print(f"Unknown configuration action: {args.action}")
            return False
        
        return True
    
    def create_configuration(self, name, config_type):
        """Create a new configuration."""
        configs_dir = Path("configs")
        configs_dir.mkdir(exist_ok=True)
        
        if config_type == "minimal":
            config = pynlm.Config()
            config.set("random_seed", 42)
            config.set("simulation_timestep", 0.001)
            config.set("neuron_count", 100)
            config.set("region_count", 1)
            config.set("connection_probability", 0.2)
            
        elif config_type == "exploration":
            config = pynlm.Config()
            config.set("random_seed", 42)
            config.set("neuron_count", 2000)
            config.set("region_count", 2)
            config.set("connection_probability", 0.05)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.novelty.enable", True)
            
        elif config_type == "memory":
            config = pynlm.Config()
            config.set("random_seed", 42)
            config.set("neuron_count", 1500)
            config.set("region_count", 3)
            config.set("connection_probability", 0.08)
            config.set("memory.episodic.max_episodes", 2000)
            
        elif config_type == "standard":
            config = pynlm.createDefaultConfig()
            
        else:
            print(f"Unknown configuration type: {config_type}")
            return False
        
        # Save configuration
        filepath = configs_dir / f"{name}.json"
        config.saveToJSON(str(filepath))
        
        print(f"Created configuration '{name}' at {filepath}")
        
        return True
    
    def list_configurations(self):
        """List available configurations."""
        configs_dir = Path("configs")
        
        if not configs_dir.exists():
            print("No configurations found.")
            return True
        
        print("Available configurations:")
        for filepath in configs_dir.glob("*.json"):
            print(f"  {filepath.stem}")
            
            # Try to load and show summary
            try:
                config = pynlm.Config()
                config.loadFromFile(str(filepath))
                print(f"    Neurons: {config.get('neuron_count', 0)}")
                print(f"    Regions: {config.get('region_count', 0)}")
            except:
                print(f"    (Unable to load)")
        
        return True
    
    def show_configuration(self, name):
        """Show configuration details."""
        configs_dir = Path("configs")
        filepath = configs_dir / f"{name}.json"
        
        if not filepath.exists():
            print(f"Configuration '{name}' not found.")
            return False
        
        try:
            config = pynlm.Config()
            config.loadFromFile(str(filepath))
            
            print(f"Configuration: {name}")
            print("=" * 50)
            
            for key in config.getKeys():
                value = config.get(key)
                print(f"{key}: {value}")
                
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return False
        
        return True
    
    def delete_configuration(self, name):
        """Delete a configuration."""
        configs_dir = Path("configs")
        filepath = configs_dir / f"{name}.json"
        
        if not filepath.exists():
            print(f"Configuration '{name}' not found.")
            return False
        
        try:
            filepath.unlink()
            print(f"Deleted configuration '{name}'")
            return True
        except Exception as e:
            print(f"Error deleting configuration: {e}")
            return False
    
    def load_configuration(self, config_file):
        """Load configuration from file."""
        if not config_file:
            return pynlm.createDefaultConfig()
        
        # Check if file exists
        if not Path(config_file).exists():
            print(f"Warning: Configuration file '{config_file}' not found. Using default.")
            return pynlm.createDefaultConfig()
        
        try:
            config = pynlm.Config()
            
            # Determine file type from extension
            if config_file.endswith('.json'):
                config.loadFromJSON(config_file)
            elif config_file.endswith('.yaml') or config_file.endswith('.yml'):
                config.loadFromYAML(config_file)
            else:
                config.loadFromSimple(config_file)
            
            print(f"Loaded configuration from {config_file}")
            return config
            
        except Exception as e:
            print(f"Error loading configuration: {e}")
            print("Using default configuration.")
            return pynlm.createDefaultConfig()
    
    def save_results(self, brain, output_file):
        """Save simulation results to file."""
        try:
            results = {
                "total_spikes": brain.getTotalSpikeCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "active_neurons": brain.getActiveNeuronCount(),
                "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
                "total_neuron_count": brain.getTotalNeuronCount(),
                "total_synapse_count": brain.getTotalSynapseCount()
            }
            
            with open(output_file, 'w') as f:
                import json
                json.dump(results, f, indent=2)
            
            print(f"Results saved to {output_file}")
            
        except Exception as e:
            print(f"Error saving results: {e}")
    
    def save_benchmark_results(self, brain, duration, steps_per_second, output_file):
        """Save benchmark results to file."""
        try:
            results = {
                "duration_seconds": duration,
                "steps_per_second": steps_per_second,
                "total_spikes": brain.getTotalSpikeCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "time_per_step_ms": (duration / args.steps * 1000) if args.steps > 0 else 0
            }
            
            with open(output_file, 'w') as f:
                import json
                json.dump(results, f, indent=2)
            
            print(f"Benchmark results saved to {output_file}")
            
        except Exception as e:
            print(f"Error saving benchmark results: {e}")
    
    def run(self, args):
        """Main entry point."""
        if args.command == "run":
            return self.run_simulation(args)
        elif args.command == "benchmark":
            return self.benchmark_brain(args)
        elif args.command == "config":
            return self.manage_configuration(args)
        elif args.command == "help":
            self.show_help()
            return True
        else:
            print(f"Unknown command: {args.command}")
            return False
    
    def show_help(self):
        """Show help information."""
        print(__doc__)

def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="NLM Command-Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Global options
    parser.add_argument(
        "--help", "-h",
        action="store_true",
        help="Show help information"
    )
    parser.add_argument(
        "--version",
        action="store_true",
        help="Show version information"
    )
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )
    parser.add_argument(
        "--quiet", "-q",
        action="store_true",
        help="Quiet mode"
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Run command
    run_parser = subparsers.add_parser("run", help="Run a simulation")
    run_parser.add_argument(
        "--config", "-c",
        help="Configuration file (JSON, YAML, or simple format)"
    )
    run_parser.add_argument(
        "--steps", "-s",
        type=int,
        default=100,
        help="Number of simulation steps (default: 100)"
    )
    run_parser.add_argument(
        "--output", "-o",
        help="Output file for results"
    )
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser("benchmark", help="Run a performance benchmark")
    benchmark_parser.add_argument(
        "--config", "-c",
        help="Configuration file (JSON, YAML, or simple format)"
    )
    benchmark_parser.add_argument(
        "--steps", "-s",
        type=int,
        default=1000,
        help="Number of benchmark steps (default: 1000)"
    )
    benchmark_parser.add_argument(
        "--output", "-o",
        help="Output file for benchmark results"
    )
    
    # Configuration command
    config_parser = subparsers.add_parser("config", help="Configuration management")
    config_subparsers = config_parser.add_subparsers(dest="action", help="Available actions")
    
    # Create configuration
    create_parser = config_subparsers.add_parser("create", help="Create a new configuration")
    create_parser.add_argument(
        "name",
        help="Configuration name"
    )
    create_parser.add_argument(
        "type",
        choices=["minimal", "exploration", "memory", "standard"],
        default="standard",
        help="Configuration type (default: standard)"
    )
    
    # List configurations
    config_subparsers.add_parser("list", help="List available configurations")
    
    # Show configuration
    show_parser = config_subparsers.add_parser("show", help="Show configuration details")
    show_parser.add_argument(
        "name",
        help="Configuration name"
    )
    
    # Delete configuration
    delete_parser = config_subparsers.add_parser("delete", help="Delete a configuration")
    delete_parser.add_argument(
        "name",
        help="Configuration name"
    )
    
    # Parse arguments
    args = parser.parse_args()
    
    # Create CLI instance
    cli = NLMCLI()
    cli.verbose = args.verbose
    
    # Show help if requested
    if args.help or not args.command:
        cli.show_help()
        return 0
    
    # Show version if requested
    if args.version:
        print(f"NLM CLI version {cli.version}")
        return 0
    
    # Run command
    try:
        success = cli.run(args)
        return 0 if success else 1
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())