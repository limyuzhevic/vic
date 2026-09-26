#!/usr/bin/env python3
"""
NLM Command Line Interface
Advanced command-line tool for NLM (Neural Learning Machine) simulations.

This provides a comprehensive CLI interface for running simulations,
configuration management, and performance analysis.
"""

import sys
import os
import json
import argparse
import subprocess
from pathlib import Path
from typing import Dict, List, Optional, Any
import yaml

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

try:
    import nlm
except ImportError:
    print("Error: NLM library not found. Please build the project first.")
    sys.exit(1)

class NLMCLI:
    """Main CLI class for NLM operations."""
    
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.configs_dir = self.project_root / "configs"
        self.benchmarks_dir = self.project_root / "benchmarks"
        
    def create_brain_from_config(self, config_path: Optional[str] = None) -> Any:
        """Create a brain from configuration file."""
        if config_path is None:
            config_path = str(self.configs_dir / "default.cfg")
            
        if not Path(config_path).exists():
            self.print_error(f"Configuration file not found: {config_path}")
            return None
            
        try:
            # Load configuration
            if config_path.endswith('.json'):
                with open(config_path, 'r') as f:
                    config_data = json.load(f)
                config = nlm.Config.from_dict(config_data)
            else:
                config = nlm.Config()
                config.load_from_file(config_path)
                
            return nlm.createBrain(config)
        except Exception as e:
            self.print_error(f"Failed to create brain from config: {e}")
            return None
    
    def run_simulation(self, brain: Any, steps: int, output_file: Optional[str] = None) -> Dict[str, Any]:
        """Run simulation for specified number of steps."""
        try:
            results = {
                'steps_completed': 0,
                'final_spike_count': 0,
                'avg_firing_rate': 0.0,
                'memory_usage': 0,
                'completed': False
            }
            
            # Run simulation
            for step in range(steps):
                brain.step(step)
                
                if output_file and step % 100 == 0:
                    self.log_simulation_progress(step, brain)
                    
            # Collect results
            results['steps_completed'] = steps
            results['final_spike_count'] = brain.getTotalSpikeCount()
            results['avg_firing_rate'] = brain.getAverageFiringRate()
            results['memory_usage'] = self.get_memory_usage()
            results['completed'] = True
            
            return results
            
        except Exception as e:
            self.print_error(f"Simulation failed: {e}")
            return {'completed': False, 'error': str(e)}
    
    def log_simulation_progress(self, step: int, brain: Any):
        """Log simulation progress."""
        print(f"\rStep {step}: "
              f"Firing: {brain.getFiringNeuronCount()}, "
              f"Spikes: {brain.getTotalSpikeCount()}, "
              f"Rate: {brain.getAverageFiringRate():.2f}", end='')
    
    def get_memory_usage(self) -> int:
        """Get current memory usage in KB."""
        try:
            import psutil
            process = psutil.Process(os.getpid())
            return process.memory_info().rss // 1024
        except:
            return 0
    
    def create_custom_config(self, args: argparse.Namespace) -> Any:
        """Create custom configuration from command line arguments."""
        config = nlm.Config()
        
        # Set basic parameters
        config.set("neuron_count", args.neurons)
        config.set("region_count", args.regions)
        config.set("connection_probability", args.connection_prob)
        
        # Set performance parameters
        config.set("simulation_timestep", args.timestep)
        config.set("random_seed", args.seed)
        
        # Set learning parameters
        if args.plasticity:
            config.set("plasticity_learning_rate", 0.01f)
            config.set("stdp_ltp_weight", 0.02f)
            config.set("stdp_ltd_weight", 0.015f)
        
        # Set development parameters
        if args.development:
            config.set("development_stage", 3)
            config.set("synaptogenesis_rate", 0.001f)
            config.set("pruning_rate", 0.0001f)
        
        # Set neuromodulation parameters
        if args.neuromodulation:
            config.set("dopamine_baseline", 0.1f)
            config.set("curiosity_strength", 0.5f)
        
        # Set memory parameters
        if args.memory:
            config.set("working_memory_capacity", 100)
            config.set("episodic_memory_span", 50)
        
        return config
    
    def save_results(self, results: Dict[str, Any], output_file: str):
        """Save simulation results to file."""
        try:
            if output_file.endswith('.json'):
                with open(output_file, 'w') as f:
                    json.dump(results, f, indent=2)
            elif output_file.endswith('.yaml') or output_file.endswith('.yml'):
                with open(output_file, 'w') as f:
                    yaml.dump(results, f, default_flow_style=False)
            else:
                self.print_error(f"Unsupported output format: {output_file}")
                return
                
            print(f"Results saved to: {output_file}")
        except Exception as e:
            self.print_error(f"Failed to save results: {e}")
    
    def run_benchmarks(self, args: argparse.Namespace):
        """Run performance benchmarks."""
        print("Running NLM Performance Benchmarks...")
        
        benchmark_configs = [
            {"name": "Small Brain", "neurons": 100, "regions": 1},
            {"name": "Medium Brain", "neurons": 1000, "regions": 2},
            {"name": "Large Brain", "neurons": 5000, "regions": 3}
        ]
        
        for bench_config in benchmark_configs:
            print(f"\n--- {bench_config['name']} ---")
            config = nlm.Config()
            config.set("neuron_count", bench_config["neurons"])
            config.set("region_count", bench_config["regions"])
            
            brain = nlm.createBrain(config)
            brain.initialize()
            
            steps = args.benchmark_steps
            results = self.run_simulation(brain, steps)
            
            print(f"\nResults for {bench_config['name']}:")
            print(f"  Steps: {results['steps_completed']}")
            print(f"  Final spikes: {results['final_spike_count']}")
            print(f"  Average firing rate: {results['avg_firing_rate']:.2f}")
            print(f"  Memory usage: {results['memory_usage']} KB")
            
            if args.output:
                bench_results = {
                    'benchmark_name': bench_config['name'],
                    'config': bench_config,
                    'results': results
                }
                self.save_results(bench_results, args.output)
    
    def print_error(self, message: str):
        """Print error message to stderr."""
        print(f"ERROR: {message}", file=sys.stderr)
    
    def print_info(self, message: str):
        """Print info message to stdout."""
        print(f"INFO: {message}")


def create_argument_parser() -> argparse.ArgumentParser:
    """Create and configure argument parser."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run basic simulation
  nlm run --neurons 1000 --steps 1000
  
  # Run with custom configuration
  nlm run --config myconfig.json --steps 5000
  
  # Run learning experiment
  nlm run --neurons 500 --steps 2000 --plasticity --development
  
  # Run performance benchmarks
  nlm benchmark --steps 500
  
  # Create custom configuration
  nlm config create --name mybrain --neurons 2000
  
  # List available configurations
  nlm config list
        ""
    )
    
    # Main commands
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Run simulation command
    run_parser = subparsers.add_parser('run', help='Run NLM simulation')
    run_parser.add_argument('--config', '-c', help='Configuration file path')
    run_parser.add_argument('--neurons', type=int, default=100, help='Number of neurons')
    run_parser.add_argument('--regions', type=int, default=1, help='Number of brain regions')
    run_parser.add_argument('--connection-prob', type=float, default=0.1, help='Connection probability')
    run_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    run_parser.add_argument('--timestep', type=float, default=0.001, help='Simulation timestep')
    run_parser.add_argument('--seed', type=int, default=42, help='Random seed')
    run_parser.add_argument('--output', '-o', help='Output file for results')
    run_parser.add_argument('--plasticity', action='store_true', help='Enable plasticity')
    run_parser.add_argument('--development', action='store_true', help='Enable development')
    run_parser.add_argument('--neuromodulation', action='store_true', help='Enable neuromodulation')
    run_parser.add_argument('--memory', action='store_true', help='Enable memory systems')
    
    # Benchmark command
    bench_parser = subparsers.add_parser('benchmark', help='Run performance benchmarks')
    bench_parser.add_argument('--steps', type=int, default=1000, help='Number of steps per benchmark')
    bench_parser.add_argument('--output', '-o', help='Output file for benchmark results')
    
    # Configuration commands
    config_parser = subparsers.add_parser('config', help='Configuration management')
    config_subparsers = config_parser.add_subparsers(dest='config_command', help='Configuration command')
    
    # Create config command
    create_config_parser = config_subparsers.add_parser('create', help='Create configuration file')
    create_config_parser.add_argument('--name', required=True, help='Configuration name')
    create_config_parser.add_argument('--neurons', type=int, default=100, help='Number of neurons')
    create_config_parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    create_config_parser.add_argument('--output', '-o', help='Output file path')
    
    # List config command
    config_subparsers.add_parser('list', help='List available configurations')
    
    # Show config command
    show_config_parser = config_subparsers.add_parser('show', help='Show configuration details')
    show_config_parser.add_argument('name', help='Configuration name')
    
    # Experiment commands
    exp_parser = subparsers.add_parser('experiment', help='Run predefined experiments')
    exp_parser.add_argument('type', choices=['learning', 'plasticity', 'development', 'cognition'],
                          help='Experiment type')
    exp_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    exp_parser.add_argument('--steps', type=int, default=2000, help='Number of simulation steps')
    exp_parser.add_argument('--output', '-o', help='Output file for results')
    
    return parser


def main():
    """Main entry point for NLM CLI."""
    parser = create_argument_parser()
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    cli = NLMCLI()
    
    try:
        if args.command == 'run':
            # Run simulation
            if args.config:
                brain = cli.create_brain_from_config(args.config)
            else:
                config = cli.create_custom_config(args)
                brain = nlm.createBrain(config)
            
            if brain is None:
                return 1
                
            brain.initialize()
            print(f"Starting simulation with {args.neurons} neurons for {args.steps} steps...")
            
            results = cli.run_simulation(brain, args.steps, args.output)
            
            if results['completed']:
                print(f"\nSimulation completed successfully!")
                print(f"Results: {results}")
                
                if args.output:
                    cli.save_results(results, args.output)
            else:
                print(f"\nSimulation failed: {results.get('error', 'Unknown error')}")
                return 1
                
        elif args.command == 'benchmark':
            # Run benchmarks
            cli.run_benchmarks(args)
            
        elif args.command == 'config':
            if args.config_command == 'create':
                # Create configuration
                print(f"Creating configuration '{args.name}'...")
                
                config = nlm.Config()
                config.set("neuron_count", args.neurons)
                config.set("region_count", args.regions)
                config.set("connection_probability", 0.1f)
                
                # Save configuration
                output_file = args.output or f"{args.name}.cfg"
                config.saveToFile(output_file)
                print(f"Configuration saved to: {output_file}")
                
            elif args.config_command == 'list':
                # List configurations
                if cli.configs_dir.exists():
                    config_files = list(cli.configs_dir.glob("*.cfg"))
                    if config_files:
                        print("Available configurations:")
                        for config_file in config_files:
                            print(f"  {config_file.name}")
                    else:
                        print("No configuration files found.")
                else:
                    print("Configurations directory does not exist.")
                    
            elif args.config_command == 'show':
                # Show configuration
                config_path = cli.configs_dir / f"{args.name}.cfg"
                if config_path.exists():
                    cli.create_brain_from_config(str(config_path))
                else:
                    print(f"Configuration '{args.name}' not found.")
                    
        elif args.command == 'experiment':
            # Run experiment
            print(f"Running {args.type} experiment with {args.neurons} neurons...")
            
            config = cli.create_custom_config(args)
            brain = nlm.createBrain(config)
            brain.initialize()
            
            # Configure experiment-specific settings
            if args.type == 'learning':
                # Enable learning components
                config.set("plasticity_learning_rate", 0.01f)
                config.set("stdp_ltp_weight", 0.02f)
            elif args.type == 'plasticity':
                # Focus on plasticity
                config.set("synaptogenesis_rate", 0.001f)
                config.set("pruning_rate", 0.0001f)
            elif args.type == 'development':
                # Focus on development
                config.set("development_stage", 3)
                config.set("maturation_rate", 0.1f)
            elif args.type == 'cognition':
                # Focus on cognitive functions
                config.set("working_memory_capacity", 200)
                config.set("episodic_memory_span", 100)
            
            results = cli.run_simulation(brain, args.steps, args.output)
            
            if results['completed']:
                print(f"Experiment {args.type} completed successfully!")
                print(f"Results: {results}")
                
                if args.output:
                    cli.save_results(results, args.output)
            else:
                print(f"Experiment failed: {results.get('error', 'Unknown error')}")
                return 1
                
        else:
            parser.print_help()
            return 1
            
    except KeyboardInterrupt:
        print("\nOperation interrupted by user.")
        return 1
    except Exception as e:
        cli.print_error(f"Unexpected error: {e}")
        return 1
    
    return 0


if __name__ == '__main__':
    sys.exit(main())