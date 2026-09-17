#!/usr/bin/env python3
"""
NLM Command Line Tool - Advanced Interface for NLM Experiments

This script provides an enhanced command-line interface for NLM experiments,
offering more control and easier access to advanced features.
"""

import argparse
import subprocess
import sys
import os
import json
import time
from pathlib import Path

# Add project root to path
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

def build_nlm():
    """Build NLM from source"""
    print("Building NLM...")
    try:
        # Try to use existing build system
        result = subprocess.run(["mkdir", "-p", "build"], check=True)
        result = subprocess.run(["cd", "build", "&&", "cmake", "..", "-DCMAKE_BUILD_TYPE=Release"], shell=True, check=True)
        result = subprocess.run(["cd", "build", "&&", "make", "-j4"], shell=True, check=True)
        print("✓ Build successful")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ Build failed: {e}")
        return False

def run_experiment(experiment_type, config_file=None, steps=1000, neurons=500, verbose=False):
    """Run specific experiment"""
    print(f"Running {experiment_type} experiment...")
    
    cmd = [
        "./build/nlm"
    ]
    
    if config_file:
        cmd.extend(["--config", config_file])
    
    # Add experiment-specific arguments
    if experiment_type == "phase2":
        cmd.extend(["--phase", "2"])
    elif experiment_type == "phase3":
        cmd.extend(["--phase", "3"])
    elif experiment_type == "phase6":
        cmd.extend(["--phase", "6"])
    
    cmd.extend([
        "--steps", str(steps),
        "--neurons", str(neurons)
    ])
    
    if verbose:
        cmd.append("--verbose")
    
    try:
        # Save current directory
        original_dir = os.getcwd()
        os.chdir("build")
        
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
        
        # Restore directory
        os.chdir(original_dir)
        
        if result.returncode == 0:
            print("✓ Experiment completed successfully")
            if verbose:
                print("Output:", result.stdout)
            return True
        else:
            print("✗ Experiment failed")
            print("Error:", result.stderr)
            return False
            
    except subprocess.TimeoutExpired:
        print("✗ Experiment timed out")
        os.chdir(original_dir)
        return False
    except Exception as e:
        print(f"✗ Error running experiment: {e}")
        os.chdir(original_dir)
        return False

def create_config_file(config_name, template="default"):
    """Create configuration file with preset values"""
    print(f"Creating configuration file: {config_name}.cfg")
    
    configs = {
        "debug": {
            "neuron_count": 100,
            "region_count": 1,
            "connection_probability": 0.05,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "simulation_timestep": 0.001,
            "random_seed": 123,
            "verbose": True,
            "enable_developmental": True
        },
        "production": {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "simulation_timestep": 0.001,
            "random_seed": 42,
            "verbose": False,
            "enable_developmental": True
        },
        "high_performance": {
            "neuron_count": 5000,
            "region_count": 4,
            "connection_probability": 0.15,
            "stdp_ltp_weight": 0.03,
            "stdp_ltd_weight": 0.025,
            "simulation_timestep": 0.002,
            "random_seed": 999,
            "verbose": False,
            "enable_developmental": True,
            "enable_checkpointing": True,
            "checkpoint_interval": 1000
        }
    }
    
    if template not in configs:
        print(f"✗ Unknown template: {template}")
        print(f"Available templates: {', '.join(configs.keys())}")
        return False
    
    config = configs[template]
    
    # Create configs directory if it doesn't exist
    os.makedirs("configs", exist_ok=True)
    
    # Write config file
    with open(f"configs/{config_name}.cfg", "w") as f:
        f.write("# NLM Configuration File\n")
        f.write(f"# Template: {template}\n")
        f.write(f"# Created: {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        
        for key, value in config.items():
            f.write(f"{key} = {value}\n")
    
    print(f"✓ Configuration file created successfully")
    return True

def list_experiments():
    """List available experiments"""
    experiments = [
        {"name": "phase2", "description": "Phase 2: Real Neural Computation (LIF neurons, STDP)", "default_steps": 1000, "default_neurons": 500},
        {"name": "phase3", "description": "Phase 3: World Integration (sensory-motor loop)", "default_steps": 500, "default_neurons": 1000},
        {"name": "phase6", "description": "Phase 6: Final Integration (all systems)", "default_steps": 2000, "default_neurons": 500}
    ]
    
    print("Available NLM Experiments:")
    print("=" * 60)
    for exp in experiments:
        print(f"\nExperiment: {exp['name'].upper()}")
        print(f"  Description: {exp['description']}")
        print(f"  Default steps: {exp['default_steps']}")
        print(f"  Default neurons: {exp['default_neurons']}")
    
    return experiments

def run_batch_experiments(configs):
    """Run multiple experiments with different configurations"""
    print("Running batch experiments...")
    
    for i, config in enumerate(configs):
        print(f"\n--- Experiment {i+1}/{len(configs)} ---")
        print(f"Config: {config.get('name', 'Unnamed')}")
        
        success = run_experiment(
            experiment_type=config.get('type', 'phase2'),
            config_file=config.get('config'),
            steps=config.get('steps', 1000),
            neurons=config.get('neurons', 500),
            verbose=config.get('verbose', False)
        )
        
        if not success:
            print(f"Experiment {i+1} failed, stopping batch")
            return False
    
    print(f"\n✓ All {len(configs)} batch experiments completed")
    return True

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Tool - Advanced Interface for NLM Experiments",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_cli.py build                    Build the project
  nlm_cli.py run phase2 --steps=2000    Run Phase 2 experiment
  nlm_cli.py create-config debug       Create debug config
  nlm_cli.py list-experiments         List all available experiments
  nlm_cli.py run batch --file=experiments.json  Run multiple experiments
        ""
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Build command
    build_parser = subparsers.add_parser("build", help="Build the project")
    
    # Run command
    run_parser = subparsers.add_parser("run", help="Run experiment")
    run_parser.add_argument("type", choices=["phase2", "phase3", "phase6"], help="Experiment type")
    run_parser.add_argument("--config", help="Configuration file path")
    run_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    run_parser.add_argument("--neurons", type=int, default=500, help="Number of neurons")
    run_parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    # Create config command
    create_parser = subparsers.add_parser("create-config", help="Create configuration file")
    create_parser.add_argument("name", help="Configuration name")
    create_parser.add_argument("template", choices=["debug", "production", "high_performance"], 
                               default="production", help="Configuration template")
    
    # List experiments command
    subparsers.add_parser("list-experiments", help="List available experiments")
    
    # Batch run command
    batch_parser = subparsers.add_parser("run-batch", help="Run multiple experiments")
    batch_parser.add_argument("--file", required=True, help="JSON file with experiment configurations")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute commands
    if args.command == "build":
        success = build_nlm()
        sys.exit(0 if success else 1)
    
    elif args.command == "run":
        success = run_experiment(args.type, args.config, args.steps, args.neurons, args.verbose)
        sys.exit(0 if success else 1)
    
    elif args.command == "create-config":
        success = create_config_file(args.name, args.template)
        sys.exit(0 if success else 1)
    
    elif args.command == "list-experiments":
        list_experiments()
        sys.exit(0)
    
    elif args.command == "run-batch":
        try:
            with open(args.file, 'r') as f:
                configs = json.load(f)
            success = run_batch_experiments(configs)
            sys.exit(0 if success else 1)
        except FileNotFoundError:
            print(f"✗ File not found: {args.file}")
            sys.exit(1)
        except json.JSONDecodeError:
            print(f"✗ Invalid JSON file: {args.file}")
            sys.exit(1)
    
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == "__main__":
    main()
