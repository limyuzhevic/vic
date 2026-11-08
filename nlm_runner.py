#!/usr/bin/env python3
"""
NLM Experiment Runner - CLI Interface

This script provides a command-line interface for running NLM (熙然) experiments.
It supports various experiment types and configuration options.

Commands:
  run <experiment_type> [options]  Run an experiment
  config <key=value>               Set configuration value
  list                              List available experiment types
  help                              Show help information

Example:
  ./nlm_runner.py run phase3 --neurons 500 --steps 2000
  ./nlm_runner.py run phase6 --debug --checkpoint
"""

import argparse
import sys
import os
import subprocess
from pathlib import Path

def run_phase3_experiment(neurons=1000, steps=2000):
    """Run Phase 3 experiment."""
    print(f"Running Phase 3 experiment with {neurons} neurons for {steps} steps")
    # Build the C++ executable if needed
    build_cmd = ["cmake", "--build", ".", "--target", "all"]
    print("Building NLM executable...")
    subprocess.run(build_cmd, check=True)
    
    # Run Phase 3 demo
    phase3_exe = "./Phase3Demo"
    if not os.path.exists(phase3_exe):
        print(f"Executable not found: {phase3_exe}")
        print("Trying to build with make...")
        subprocess.run(["make", "-j4"], check=True)
    
    print(f"Running Phase 3 experiment...")
    subprocess.run([phase3_exe], check=True)

def run_phase6_demo(neurons=500, steps=2000, debug=False):
    """Run Phase 6 integration demo."""
    print(f"Running Phase 6 demo with {neurons} neurons for {steps} steps")
    
    # Build the C++ executable if needed
    build_cmd = ["cmake", "--build", ".", "--target", "all"]
    print("Building NLM executable...")
    subprocess.run(build_cmd, check=True)
    
    # Run Phase 6 demo
    phase6_exe = "./Phase6Demo"
    if not os.path.exists(phase6_exe):
        print(f"Executable not found: {phase6_exe}")
        print("Trying to build with make...")
        subprocess.run(["make", "-j4"], check=True)
    
    print(f"Running Phase 6 demo...")
    subprocess.run([phase6_exe], check=True)

def run_phase4_experiment(neurons=500, steps=2000):
    """Run Phase 4 experiment."""
    print(f"Running Phase 4 experiment with {neurons} neurons for {steps} steps")
    # TODO: Implement Phase 4 experiment runner
    print("Phase 4 experiment runner not yet implemented")

def run_phase5_experiment(neurons=500, steps=2000):
    """Run Phase 5 experiment."""
    print(f"Running Phase 5 experiment with {neurons} neurons for {steps} steps")
    # TODO: Implement Phase 5 experiment runner
    print("Phase 5 experiment runner not yet implemented")

def list_experiments():
    """List available experiment types."""
    print("Available experiment types:")
    print("  phase3    - Phase 3 World Interaction Demo")
    print("  phase4    - Phase 4 Structural Plasticity")
    print("  phase5    - Phase 5 Reward Prediction")
    print("  phase6    - Phase 6 Integration Demo")
    print("  ablation  - Ablation studies")

def main():
    parser = argparse.ArgumentParser(description="NLM Experiment Runner")
    parser.add_argument("command", help="Command to execute")
    parser.add_argument("args", nargs="*", help="Additional arguments")
    
    if len(sys.argv) < 2:
        print("NLM Experiment Runner")
        print("Usage: python nlm_runner.py <command> [options]")
        print("")
        print("Available commands:")
        list_experiments()
        print("")
        print("Use 'help' for detailed usage information.")
        sys.exit(1)
    
    command = sys.argv[1]
    args = sys.argv[2:]
    
    if command == "help":
        print("NLM Experiment Runner - Help")
        print("=" * 50)
        print("")
        print("Commands:")
        print("  run <experiment_type> [options]  Run an experiment")
        print("  list                              List available experiment types")
        print("  help                              Show this help")
        print("")
        print("Experiment types:")
        list_experiments()
        print("")
        print("Options for run command:")
        print("  --neurons N        Number of neurons (default: 1000 for phase3)")
        print("  --steps S          Number of steps (default: 2000 for phase3)")
        print("  --debug            Enable debug output")
        print("  --checkpoint       Enable checkpointing")
        sys.exit(0)
    
    elif command == "list":
        list_experiments()
        sys.exit(0)
    
    elif command == "run":
        if len(args) < 1:
            print("Error: Experiment type not specified")
            print("Use 'list' to see available experiment types")
            sys.exit(1)
        
        experiment_type = args[0]
        
        # Parse common options
        neurons = 1000
        steps = 2000
        debug = False
        checkpoint = False
        
        for i, arg in enumerate(args[1:], 1):
            if arg == "--debug":
                debug = True
            elif arg == "--checkpoint":
                checkpoint = True
            elif arg.startswith("--neurons="):
                neurons = int(arg.split("=")[1])
            elif arg.startswith("--steps="):
                steps = int(arg.split("=")[1])
        
        if experiment_type == "phase3":
            run_phase3_experiment(neurons, steps)
        elif experiment_type == "phase6":
            run_phase6_demo(neurons, steps, debug)
        elif experiment_type == "phase4":
            run_phase4_experiment(neurons, steps)
        elif experiment_type == "phase5":
            run_phase5_experiment(neurons, steps)
        else:
            print(f"Error: Unknown experiment type '{experiment_type}'")
            sys.exit(1)
    
    else:
        print(f"Error: Unknown command '{command}'")
        sys.exit(1)

if __name__ == "__main__":
    main()