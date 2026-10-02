#!/usr/bin/env python3
"""
NLM Command Line Interface

Simple command-line interface for common NLM operations.

Usage:
    nlm-cli <command> [options]

Commands:
    create <brain_name>          Create a new brain
    simulate <brain_name> <steps> Run simulation
    config <file>                Load configuration
    stats <brain_name>           Show brain statistics
    help                         Show this help

Examples:
    nlm-cli create mybrain
    nlm-cli simulate mybrain 1000
    nlm-cli config myconfig.json
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def print_help():
    print(__doc__)

def create_brain(args):
    if len(args) < 1:
        print("Error: brain_name required")
        print("Usage: nlm-cli create <brain_name>")
        return
    
    brain_name = args[0]
    print(f"Creating brain '{brain_name}'...")
    
    # Create brain with default configuration
    brain = pynlm.createBrain()
    
    # In a real implementation, we would save/load brain instances
    print(f"Brain '{brain_name}' created with {brain.getTotalNeuronCount()} neurons")
    print(f"You can now use this brain in Python scripts")

def simulate_brain(args):
    if len(args) < 2:
        print("Error: brain_name and steps required")
        print("Usage: nlm-cli simulate <brain_name> <steps>")
        return
    
    brain_name = args[0]
    try:
        steps = int(args[1])
    except ValueError:
        print(f"Error: steps must be a number, got '{args[1]}'")
        return
    
    print(f"Running simulation on brain '{brain_name}' for {steps} steps...")
    
    # Create brain
    brain = pynlm.createBrain()
    
    # Run simulation
    for i in range(steps):
        brain.step(i)
        
        if (i + 1) % 100 == 0:
            print(f"  Step {i + 1}/{steps}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Simulation complete!")
    print(f"Final stats: {brain.getTotalSpikeCount()} total spikes, {brain.getAverageFiringRate():.2f} avg firing rate")

def load_config(args):
    if len(args) < 1:
        print("Error: config file required")
        print("Usage: nlm-cli config <file>")
        return
    
    config_file = args[0]
    if not os.path.exists(config_file):
        print(f"Error: config file '{config_file}' not found")
        return
    
    print(f"Loading configuration from '{config_file}'...")
    
    try:
        config = pynlm.createConfigFromFile(config_file)
        keys = pynlm.getConfigKeys(config)
        print(f"Configuration loaded successfully with keys: {', '.join(keys)}")
    except Exception as e:
        print(f"Error loading configuration: {e}")

def show_stats(args):
    if len(args) < 1:
        print("Error: brain_name required")
        print("Usage: nlm-cli stats <brain_name>")
        return
    
    brain_name = args[0]
    print(f"Getting statistics for brain '{brain_name}'...")
    
    # Create brain
    brain = pynlm.createBrain()
    
    # Show summary
    summary = pynlm.getBrainSummary(brain)
    print(summary)

def main():
    if len(sys.argv) < 2:
        print("Error: command required")
        print_help()
        return
    
    command = sys.argv[1]
    args = sys.argv[2:]
    
    if command == "help":
        print_help()
    elif command == "create":
        create_brain(args)
    elif command == "simulate":
        simulate_brain(args)
    elif command == "config":
        load_config(args)
    elif command == "stats":
        show_stats(args)
    else:
        print(f"Error: unknown command '{command}'")
        print_help()
        return

if __name__ == "__main__":
    main()