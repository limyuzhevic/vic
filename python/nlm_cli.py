#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Python Command Line Interface

This module provides a command-line interface for NLM simulations,
benchmarking, and analysis using the pynlm Python bindings.
"""

import argparse
import sys
import time
import json
import os
from pathlib import Path

import numpy as np
import pandas as pd

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM Python CLI - Neural Learning Machine Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-console --help                    Show this help message
  nlm-console demo --neurons 500        Run demo with 500 neurons
  nlm-console benchmark --steps 1000     Benchmark simulation performance
  nlm-console config --save config.json  Save configuration to file
  nlm-console analyze data.csv          Analyze existing data

For more information, visit: https://github.com/nlm-project/nlm
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Demo command
    demo_parser = subparsers.add_parser("demo", help="Run NLM demonstration")
    demo_parser.add_argument("--neurons", type=int, default=500,
                             help="Number of neurons (default: 500)")
    demo_parser.add_argument("--steps", type=int, default=1000,
                             help="Number of simulation steps (default: 1000)")
    demo_parser.add_argument("--regions", type=int, default=2,
                             help="Number of brain regions (default: 2)")
    demo_parser.add_argument("--output", type=str, default="demo_results.json",
                             help="Output file for results (default: demo_results.json)")
    demo_parser.add_argument("--seed", type=int, default=42,
                             help="Random seed (default: 42)")
    demo_parser.add_argument("--verbose", action="store_true",
                             help="Enable verbose output")
    
    # Benchmark command
    bench_parser = subparsers.add_parser("benchmark", help="Run performance benchmark")
    bench_parser.add_argument("--steps", type=int, default=5000,
                              help="Number of simulation steps (default: 5000)")
    bench_parser.add_argument("--neurons", type=int, default=1000,
                              help="Number of neurons (default: 1000)")
    bench_parser.add_argument("--repeats", type=int, default=3,
                              help="Number of repetitions for averaging (default: 3)")
    bench_parser.add_argument("--output", type=str, default="benchmark_results.csv",
                              help="Output CSV file (default: benchmark_results.csv)")
    bench_parser.add_argument("--detailed", action="store_true",
                              help="Enable detailed benchmarking with memory profiling")
    
    # Config command
    config_parser = subparsers.add_parser("config", help="Manage NLM configuration")
    config_subparsers = config_parser.add_subparsers(dest="config_command", help="Configuration commands")
    
    # Config save
    save_parser = config_subparsers.add_parser("save", help="Save configuration to file")
    save_parser.add_argument("file", type=str, help="Configuration file path")
    
    # Config load
    load_parser = config_subparsers.add_parser("load", help="Load configuration from file")
    load_parser.add_argument("file", type=str, help="Configuration file path")
    
    # Config list
    config_subparsers.add_parser("list", help="List available configuration templates")
    
    # Analysis command
    analysis_parser = subparsers.add_parser("analyze", help="Analyze NLM data")
    analysis_parser.add_argument("file", type=str, help="Input data file (CSV or JSON)")
    analysis_parser.add_argument("--format", type=str, choices=["csv", "json", "hdf5"], default="csv",
                                help="Input file format (default: csv)")
    analysis_parser.add_argument("--output", type=str, default="analysis_results.json",
                                help="Output file for analysis results (default: analysis_results.json)")
    analysis_parser.add_argument("--plot", action="store_true",
                                help="Generate analysis plots")
    
    # Export command
    export_parser = subparsers.add_parser("export", help="Export NLM data")
    export_parser.add_argument("--brain", type=str, help="Brain instance path")
    export_parser.add_argument("--file", type=str, required=True, help="Output file path")
    export_parser.add_argument("--format", type=str, choices=["csv", "json", "hdf5", "npz"], default="csv",
                              help="Export format (default: csv)")
    export_parser.add_argument("--include-neurons", action="store_true",
                              help="Include neuron data")
    export_parser.add_argument("--include-synapses", action="store_true",
                              help="Include synapse data")
    
    # Help command
    subparsers.add_parser("help", help="Show detailed help information")
    
    # Version command
    subparsers.add_parser("version", help="Show NLM version information")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Route to appropriate command handler
    if args.command == "demo":
        run_demo(args)
    elif args.command == "benchmark":
        run_benchmark(args)
    elif args.command == "config":
        handle_config_command(args)
    elif args.command == "analyze":
        run_analysis(args)
    elif args.command == "export":
        run_export(args)
    elif args.command == "help":
        show_detailed_help()
    elif args.command == "version":
        show_version_info()
    else:
        parser.print_help()
        return 1
    
    return 0


def run_demo(args):
    """Run NLM demonstration"""
    print(f"Running NLM demo with {args.neurons} neurons for {args.steps} steps...")
    
    try:
        # Create configuration
        config = pynlm.create_default_config()
        config.set("neuron_count", args.neurons)
        config.set("region_count", args.regions)
        config.set("random_seed", args.seed)
        
        if args.verbose:
            print("Configuration:")
            print(f"  Neurons: {args.neurons}")
            print(f"  Regions: {args.regions}")
            print(f"  Seed: {args.seed}")
        
        # Create and initialize brain
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        print(f"Brain created with {brain.get_total_neuron_count()} neurons")
        print(f"Synapses: {brain.get_total_synapse_count()}")
        
        # Run simulation
        start_time = time.time()
        for step in range(args.steps):
            brain.step(step)
            
            if args.verbose and step % 100 == 0:
                print(f"Step {step}: {brain.get_firing_neuron_count()} firing neurons")
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"\nDemo completed in {duration:.2f} seconds")
        print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
        print(f"Total spikes: {brain.get_total_spike_count()}")
        
        # Save results
        results = {
            "neurons": args.neurons,
            "regions": args.regions,
            "steps": args.steps,
            "seed": args.seed,
            "duration": duration,
            "firing_rate": brain.get_average_firing_rate(),
            "total_spikes": brain.get_total_spike_count(),
            "final_neurons": brain.get_total_neuron_count(),
            "final_synapses": brain.get_total_synapse_count()
        }
        
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Results saved to {args.output}")
        
    except Exception as e:
        print(f"Error running demo: {e}")
        return 1


def run_benchmark(args):
    """Run performance benchmark"""
    print(f"Running benchmark with {args.neurons} neurons for {args.steps} steps (x{args.repeats} repeats)...")
    
    try:
        results = []
        
        for repeat in range(args.repeats):
            print(f"\nRepeat {repeat + 1}/{args.repeats}")
            
            # Create fresh configuration
            config = pynlm.create_default_config()
            config.set("neuron_count", args.neurons)
            config.set("random_seed", 42 + repeat)
            
            # Create and initialize brain
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            # Run simulation
            start_time = time.time()
            start_memory = get_memory_usage()
            
            for step in range(args.steps):
                brain.step(step)
            
            end_time = time.time()
            end_memory = get_memory_usage()
            
            duration = end_time - start_time
            memory_used = end_memory - start_memory
            
            # Collect metrics
            result = {
                "repeat": repeat + 1,
                "neurons": args.neurons,
                "steps": args.steps,
                "duration": duration,
                "steps_per_second": args.steps / duration,
                "memory_used_mb": memory_used,
                "firing_rate": brain.get_average_firing_rate(),
                "total_spikes": brain.get_total_spike_count(),
                "neurons_firing": brain.get_firing_neuron_count()
            }
            
            results.append(result)
            
            if args.detailed:
                print(f"  Duration: {duration:.2f}s")
                print(f"  Steps/sec: {result['steps_per_second']:.0f}")
                print(f"  Memory: {memory_used:.2f} MB")
                print(f"  Firing rate: {result['firing_rate']:.2f} Hz")
        
        # Calculate averages
        avg_duration = np.mean([r["duration"] for r in results])
        avg_speed = np.mean([r["steps_per_second"] for r in results])
        avg_memory = np.mean([r["memory_used_mb"] for r in results])
        
        # Create DataFrame and save
        df = pd.DataFrame(results)
        df.to_csv(args.output, index=False)
        
        print(f"\nBenchmark completed. Results saved to {args.output}")
        print(f"Average steps/sec: {avg_speed:.0f}")
        print(f"Average duration: {avg_duration:.2f}s")
        print(f"Average memory usage: {avg_memory:.2f} MB")
        
    except Exception as e:
        print(f"Error running benchmark: {e}")
        return 1


def handle_config_command(args):
    """Handle configuration commands"""
    if not hasattr(args, 'config_command'):
        print("Please specify a configuration command: save, load, or list")
        return 1
    
    if args.config_command == "save":
        save_config(args.file)
    elif args.config_command == "load":
        load_config(args.file)
    elif args.config_command == "list":
        list_configs()
    
    return 0


def save_config(filepath):
    """Save default configuration to file"""
    try:
        config = pynlm.create_default_config()
        
        # Save in JSON format
        config_data = {
            "neuron_count": config.getOr("neuron_count", 1000),
            "region_count": config.getOr("region_count", 1),
            "connection_probability": config.getOr("connection_probability", 0.1),
            "random_seed": config.getOr("random_seed", 42),
            "simulation_timestep": config.getOr("simulation_timestep", 0.001),
            "stdp_ltp_weight": config.getOr("stdp_ltp_weight", 0.01),
            "stdp_ltd_weight": config.getOr("stdp_ltd_weight", 0.012),
            "stdp_tau": config.getOr("stdp_tau", 20.0),
            "synaptogenesis_rate": config.getOr("synaptogenesis_rate", 0.0001),
            "pruning_rate": config.getOr("pruning_rate", 0.00001)
        }
        
        with open(filepath, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        print(f"Configuration saved to {filepath}")
        
    except Exception as e:
        print(f"Error saving configuration: {e}")
        return 1


def load_config(filepath):
    """Load configuration from file"""
    try:
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        print("Loaded configuration:")
        for key, value in config_data.items():
            print(f"  {key}: {value}")
        
        print(f"To use this configuration, create a config using pynlm.create_default_config()")
        print(f"and set each parameter using config.set('parameter_name', value)")
        
    except Exception as e:
        print(f"Error loading configuration: {e}")
        return 1


def list_configs():
    """List available configuration templates"""
    configs = {
        "minimal": {
            "neuron_count": 100,
            "region_count": 1,
            "description": "Small simulation for quick testing"
        },
        "standard": {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "description": "Standard medium-sized simulation"
        },
        "large": {
            "neuron_count": 5000,
            "region_count": 3,
            "connection_probability": 0.05,
            "description": "Large-scale simulation"
        },
        "research": {
            "neuron_count": 10000,
            "region_count": 4,
            "connection_probability": 0.02,
            "stdp_ltp_weight": 0.005,
            "stdp_ltd_weight": 0.004,
            "stdp_tau": 30.0,
            "description": "Research-grade simulation with custom plasticity"
        }
    }
    
    print("Available NLM configuration templates:")
    print("=" * 50)
    
    for name, config in configs.items():
        print(f"\n{name.upper()}:")
        print(f"  {config['description']}")
        for key, value in config.items():
            if key != "description":
                print(f"    {key}: {value}")
    
    print(f"\nTo use a template, create a config and set the parameters.")
    print(f"Example: config = pynlm.create_default_config()")
    print(f"Then set parameters: config.set('neuron_count', {configs['standard']['neuron_count']})")


def run_analysis(args):
    """Analyze NLM data"""
    print(f"Analyzing data from {args.file}...")
    
    try:
        if args.format == "csv":
            data = pd.read_csv(args.file)
        elif args.format == "json":
            with open(args.file, 'r') as f:
                data = pd.json_normalize(json.load(f))
        else:
            print(f"Format {args.format} not yet implemented")
            return 1
        
        print(f"Loaded {len(data)} rows of data")
        print(f"Columns: {list(data.columns)}")
        
        # Basic analysis
        analysis_results = {}
        
        # Statistical summary
        numeric_cols = data.select_dtypes(include=[np.number]).columns
        analysis_results["statistics"] = {}
        
        for col in numeric_cols:
            analysis_results["statistics"][col] = {
                "mean": float(data[col].mean()),
                "std": float(data[col].std()),
                "min": float(data[col].min()),
                "max": float(data[col].max()),
                "median": float(data[col].median()),
                "q25": float(data[col].quantile(0.25)),
                "q75": float(data[col].quantile(0.75))
            }
        
        # Basic insights
        analysis_results["insights"] = []
        
        if "firing_rate" in numeric_cols:
            avg_firing = data["firing_rate"].mean()
            analysis_results["insights"].append(
                f"Average firing rate: {avg_firing:.2f} Hz"
            )
        
        if "total_spikes" in numeric_cols:
            total_spikes = data["total_spikes"].sum()
            analysis_results["insights"].append(
                f"Total spikes across all simulations: {total_spikes}"
            )
        
        # Save analysis results
        with open(args.output, 'w') as f:
            json.dump(analysis_results, f, indent=2)
        
        print(f"Analysis completed and saved to {args.output}")
        
        if args.plot:
            try:
                import matplotlib.pyplot as plt
                
                fig, axes = plt.subplots(2, 2, figsize=(12, 10))
                fig.suptitle("NLM Data Analysis", fontsize=16)
                
                # Plot 1: Firing rate distribution
                if "firing_rate" in numeric_cols:
                    axes[0, 0].hist(data["firing_rate"], bins=20, alpha=0.7)
                    axes[0, 0].set_title("Firing Rate Distribution")
                    axes[0, 0].set_xlabel("Firing Rate (Hz)")
                    axes[0, 0].set_ylabel("Frequency")
                
                # Plot 2: Spike count distribution
                if "total_spikes" in numeric_cols:
                    axes[0, 1].hist(data["total_spikes"], bins=20, alpha=0.7)
                    axes[0, 1].set_title("Total Spikes Distribution")
                    axes[0, 1].set_xlabel("Total Spikes")
                    axes[0, 1].set_ylabel("Frequency")
                
                # Plot 3: Scatter plot (first two numeric columns)
                if len(numeric_cols) >= 2:
                    col1, col2 = numeric_cols[0], numeric_cols[1]
                    axes[1, 0].scatter(data[col1], data[col2], alpha=0.5)
                    axes[1, 0].set_title(f"{col1} vs {col2}")
                    axes[1, 0].set_xlabel(col1)
                    axes[1, 0].set_ylabel(col2)
                
                # Plot 4: Time series (if data has time/index column)
                if "step" in data.columns and "firing_rate" in numeric_cols:
                    axes[1, 1].plot(data["step"], data["firing_rate"])
                    axes[1, 1].set_title("Firing Rate over Time")
                    axes[1, 1].set_xlabel("Step")
                    axes[1, 1].set_ylabel("Firing Rate (Hz)")
                
                plt.tight_layout()
                plt.savefig(f"analysis_plot.png", dpi=150)
                print("Plots saved to analysis_plot.png")
                
            except ImportError:
                print("Matplotlib not available. Skipping plot generation.")
        
    except Exception as e:
        print(f"Error during analysis: {e}")
        return 1


def run_export(args):
    """Export NLM data"""
    print("Exporting NLM data...")
    
    try:
        if args.format not in ["csv", "json", "hdf5", "npz"]:
            print(f"Format {args.format} not yet implemented")
            return 1
        
        print(f"Export format: {args.format}")
        print(f"Output file: {args.file}")
        
        # Note: This would require an actual brain instance
        # For now, provide a template implementation
        print("\nExport functionality requires a brain instance.")
        print("Example usage:")
        print("  brain = pynlm.create_brain(config)")
        print(f"  pynlm.export_brain_data(brain, '{args.file}', '{args.format}')")
        
    except Exception as e:
        print(f"Error during export: {e}")
        return 1


def show_detailed_help():
    """Show detailed help information"""
    print("""
NLM Python CLI - Detailed Help
================================

This interface provides comprehensive tools for NLM simulation, benchmarking,
configuration management, and data analysis.

COMMANDS:

1. DEMO
   Run a NLM demonstration with customizable parameters
   Usage: nlm-console demo [options]
   Options:
     --neurons N     Number of neurons (default: 500)
     --steps S       Simulation steps (default: 1000)
     --regions R     Brain regions (default: 2)
     --seed S        Random seed (default: 42)
     --output FILE    Save results to FILE (default: demo_results.json)
     --verbose        Show detailed progress

2. BENCHMARK
   Run performance benchmarks
   Usage: nlm-console benchmark [options]
   Options:
     --steps S        Steps per run (default: 5000)
     --neurons N      Neurons per run (default: 1000)
     --repeats R      Number of repetitions (default: 3)
     --output FILE     Save results to FILE (default: benchmark_results.csv)
     --detailed       Include memory profiling

3. CONFIG
   Configuration management
   Usage: nlm-console config save|load|list [options]
   Subcommands:
     save FILE    Save default configuration
     load FILE    Load configuration from file
     list         List configuration templates

4. ANALYZE
   Analyze existing NLM data
   Usage: nlm-console analyze FILE [options]
   Options:
     --format FORMAT Input format (csv, json) (default: csv)
     --output FILE    Save results (default: analysis_results.json)
     --plot          Generate analysis plots

5. EXPORT
   Export NLM data
   Usage: nlm-console export [options]
   Options:
     --brain FILE     Brain instance file (if applicable)
     --file FILE      Output file path
     --format FORMAT  Export format: csv, json, hdf5, npz (default: csv)

6. HELP
   Show detailed help information

7. VERSION
   Show NLM version and build information

FILES GENERATED:

- demo_results.json: Demo simulation results
- benchmark_results.csv: Benchmark performance data
- config_*.json: Saved configuration files
- analysis_results.json: Data analysis results
- analysis_plot.png: Analysis visualization (if --plot used)

TROUBLESHOOTING:

- Missing dependencies: Install pynlm with all dependencies
- File not found: Check file paths and permissions
- Python errors: Ensure Python environment is properly set up
- Memory errors: Reduce --neurons or --steps parameters

For more information, visit: https://github.com/nlm-project/nlm
""")


def show_version_info():
    """Show version information"""
    print("NLM Python Bindings - Version 0.1.0")
    print("Build Information:")
    print("  Python version:", sys.version)
    print("  Platform:", sys.platform)
    print("  Compiler: Built with pybind11 and scikit-build")
    print("  Features:")
    print("    - Full Brain simulation")
    print("    - Memory and Neuromodulation systems")
    print("    - Agent-World interface")
    print("    - Configuration management")
    print("    - Data export and analysis tools")
    print("    - Performance benchmarking")
    print("    - Command-line interface")
    print("\nFor documentation, visit: https://nlm.readthedocs.io")


def get_memory_usage():
    """Get current memory usage in MB"""
    try:
        import psutil
        return psutil.virtual_memory().used / (1024 * 1024)
    except ImportError:
        # Fallback: return 0 if psutil not available
        return 0


if __name__ == "__main__":
    try:
        exit_code = main()
        sys.exit(exit_code)
    except KeyboardInterrupt:
        print("\nOperation interrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"Fatal error: {e}")
        sys.exit(1)
