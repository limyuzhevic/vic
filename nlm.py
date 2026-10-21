#!/usr/bin/env python3
"""
NLM Command Line Interface

Enhanced command-line interface for NLM (Neural Learning Machine) with comprehensive
experiment management, configuration tools, and analysis capabilities.
"""

import argparse
import sys
import os
import json
from pathlib import Path

def print_banner():
    print("=== NLM - Neural Learning Machine ===")
    print("An experimental artificial developmental brain")
    print("Phase 6: Final Integration")
    print()

def run_phase6_demo(args):
    """Run the Phase 6 integration demo"""
    print(f"Running NLM Phase 6 Integration Demo...")
    
    # Check if nlm_phase6_demo executable exists
    demo_path = "build/nlm_phase6_demo"
    if not os.path.exists(demo_path):
        print(f"ERROR: Demo executable not found at {demo_path}")
        print("Please build the project first: mkdir build && cd build && cmake .. && make")
        return False
    
    # Run the demo
    import subprocess
    try:
        result = subprocess.run([demo_path], capture_output=True, text=True, timeout=args.timeout)
        print(result.stdout)
        if result.stderr:
            print("STDERR:", result.stderr)
        return result.returncode == 0
    except subprocess.TimeoutExpired:
        print(f"Demo timed out after {args.timeout} seconds")
        return False
    except Exception as e:
        print(f"Error running demo: {e}")
        return False

def create_experiment(args):
    """Create and run a custom experiment"""
    print(f"Creating experiment: {args.experiment_name}")
    print(f"Configuration: neurons={args.neurons}, steps={args.steps}")
    
    # This would create a Python-based experiment runner
    # For now, provide guidance on how to create experiments
    print("\nTo create experiments with the enhanced experiment framework:")
    print("1. Use the NLM C++ API in your experiment code")
    print("2. Create Brain, Environment, and Experiment objects")
    print("3. Use ExperimentRunner to manage and run experiments")
    print("4. Use saveResults() to export experiment data")
    
    return True

def run_analysis(args):
    """Analyze experiment results"""
    print(f"Analyzing experiment results from: {args.results_file}")
    
    if not os.path.exists(args.results_file):
        print(f"ERROR: Results file not found: {args.results_file}")
        return False
    
    # Read and analyze results
    try:
        # This is a simple analysis - in practice, you'd use more sophisticated tools
        print("\nExperiment Results Analysis:")
        print("=" * 50)
        
        if args.results_file.endswith('.csv'):
            # CSV analysis
            import pandas as pd
            df = pd.read_csv(args.results_file)
            print(f"Total steps recorded: {len(df)}")
            print(f"Neurons range: {df['NeuronCount'].min()} - {df['NeuronCount'].max()}")
            print(f"Average firing rate: {df['AvgFiringRate'].mean():.4f}")
            print(f"Synapse count range: {df['SynapseCount'].min()} - {df['SynapseCount'].max()}")
        
        elif args.results_file.endswith('.json'):
            # JSON analysis
            with open(args.results_file, 'r') as f:
                data = json.load(f)
            print(f"Experiment: {data.get('name', 'Unknown')}")
            print(f"Seed: {data.get('seed', 'Unknown')}")
            print(f"Total metrics: {len(data.get('metrics', {}))}")
        
        else:
            print(f"Unknown file format: {args.results_file}")
            return False
            
        return True
        
    except Exception as e:
        print(f"Error analyzing results: {e}")
        return False

def create_config(args):
    """Create a custom configuration file"""
    print(f"Creating configuration file: {args.output}")
    
    config = {
        "random_seed": args.seed,
        "simulation_timestep": args.timestep,
        "neuron_count": args.neurons,
        "region_count": args.regions,
        "connection_probability": args.connection_prob,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "replay_interval": 100,
        "consolidation_interval": 1000,
        "checkpoint_dir": "./checkpoints",
        "checkpoint_interval": 1000,
        "max_checkpoints": 10
    }
    
    try:
        with open(args.output, 'w') as f:
            json.dump(config, f, indent=2)
        print(f"Configuration saved to: {args.output}")
        return True
    except Exception as e:
        print(f"Error creating configuration: {e}")
        return False

def list_experiments(args):
    """List available experiments"""
    print("Available NLM Experiments:")
    print("=" * 50)
    
    experiments = [
        ("nlm_phase6_demo", "Phase 6 Integration Test - Complete brain verification"),
        ("nlm_phase3_demo", "Phase 3 World Interaction - Basic learning"),
        ("nlm_phase4_demo", "Phase 4 Cognitive Integration - Higher cognition"),
        ("nlm_main", "Basic Neural Computation - Phase 2 demonstration"),
    ]
    
    for name, description in experiments:
        print(f"\n{name}:")
        print(f"  {description}")
    
    return True

def build_project(args):
    """Build the NLM project"""
    print("Building NLM project...")
    
    if not os.path.exists("CMakeLists.txt"):
        print("ERROR: CMakeLists.txt not found. Are you in the NLM project root?")
        return False
    
    try:
        # Create build directory
        os.makedirs("build", exist_ok=True)
        
        # Configure with CMake
        print("Configuring with CMake...")
        result = subprocess.run(
            ["cmake", "..", f"-DCMAKE_BUILD_TYPE={'Release' if not args.debug else 'Debug'}"],
            cwd="build",
            capture_output=True,
            text=True
        )
        
        if result.returncode != 0:
            print(f"CMake configuration failed:")
            print(result.stdout)
            print(result.stderr)
            return False
        
        # Build with make
        print("Building with make...")
        if args.parallel:
            result = subprocess.run(
                ["make", "-j4"],
                cwd="build",
                capture_output=True,
                text=True
            )
        else:
            result = subprocess.run(
                ["make"],
                cwd="build",
                capture_output=True,
                text=True
            )
        
        if result.returncode != 0:
            print(f"Build failed:")
            print(result.stdout)
            print(result.stderr)
            return False
        
        print("Build completed successfully!")
        print("\nTo run experiments:")
        print("  ./build/nlm_phase6_demo          # Phase 6 integration test")
        print("  ./build/nlm_phase3_demo          # Phase 3 world interaction")
        print("  ./build/nlm_phase4_demo          # Phase 4 cognitive integration")
        print("  ./build/nlm                        # Basic neural computation")
        
        return True
        
    except Exception as e:
        print(f"Build error: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(
        description="NLM - Neural Learning Machine Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm.py run                    # Run Phase 6 integration demo
  nlm.py build                   # Build the project
  nlm.py experiment --name myexp  # Create and run a custom experiment
  nlm.py analyze results.csv      # Analyze experiment results
  nlm.py config --output mycfg.json  # Create configuration file
  nlm.py list                    # List available experiments
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Run demo command
    parser_run = subparsers.add_parser("run", help="Run NLM demo")
    parser_run.add_argument("--timeout", type=int, default=300, help="Timeout in seconds")
    
    # Experiment command
    parser_exp = subparsers.add_parser("experiment", help="Create and run experiments")
    parser_exp.add_argument("--name", required=True, help="Experiment name")
    parser_exp.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser_exp.add_argument("--steps", type=int, default=10000, help="Number of simulation steps")
    parser_exp.add_argument("--regions", type=int, default=1, help="Number of brain regions")
    parser_exp.add_argument("--connection-prob", type=float, default=0.1, help="Connection probability")
    parser_exp.add_argument("--seed", type=int, default=42, help="Random seed")
    
    # Analysis command
    parser_ana = subparsers.add_parser("analyze", help="Analyze experiment results")
    parser_ana.add_argument("results_file", help="Path to results file (CSV or JSON)")
    
    # Configuration command
    parser_cfg = subparsers.add_parser("config", help="Create configuration file")
    parser_cfg.add_argument("--output", required=True, help="Output configuration file path")
    parser_cfg.add_argument("--seed", type=int, default=42, help="Random seed")
    parser_cfg.add_argument("--timestep", type=float, default=0.001, help="Simulation timestep")
    parser_cfg.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser_cfg.add_argument("--regions", type=int, default=1, help="Number of regions")
    parser_cfg.add_argument("--connection-prob", type=float, default=0.1, help="Connection probability")
    
    # List command
    subparsers.add_parser("list", help="List available experiments")
    
    # Build command
    parser_build = subparsers.add_parser("build", help="Build the NLM project")
    parser_build.add_argument("--no-parallel", action="store_true", help="Disable parallel build")
    parser_build.add_argument("--debug", action="store_true", help="Build in debug mode")
    
    # Help command
    subparsers.add_parser("help", help="Show help information")
    
    # Version command
    subparsers.add_parser("version", help="Show NLM version information")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Print banner
    print_banner()
    
    # Execute command
    if args.command == "run":
        success = run_phase6_demo(args)
    elif args.command == "experiment":
        success = create_experiment(args)
    elif args.command == "analyze":
        success = run_analysis(args)
    elif args.command == "config":
        success = create_config(args)
    elif args.command == "list":
        success = list_experiments(args)
    elif args.command == "build":
        success = build_project(args)
    elif args.command == "help":
        parser.print_help()
        success = True
    elif args.command == "version":
        print("NLM - Neural Learning Machine")
        print("Version: 0.1.0 (Phase 6)")
        print("Author: NLM Research Team")
        print("License: MIT")
        success = True
    else:
        parser.print_help()
        success = False
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
