#!/usr/bin/env python3
"""
NLM Brain Command Line Interface

Command-line interface for brain operations.
"""

import argparse
import sys
import os
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, str(Path(__file__).parent))

import pynlm
from pynlm import create_default_config, create_brain, create_simple_world, create_agent_brain
def create_brain_command(args):
    """Create and initialize a brain."""
    print("Creating brain...")
    
    # Create configuration
    if args.config_file:
        config = pynlm.Config()
        config.load_from_file(args.config_file)
    else:
        config = create_default_config()
        # Apply command line overrides
        if args.neuron_count:
            config.set("brain.neuron_count", args.neuron_count)
        if args.synapse_density:
            config.set("brain.synapse_density", args.synapse_density)
    
    # Create and initialize brain
    brain = create_brain(config)
    brain.initialize()
    
    print(f"Brain created successfully!")
    print(f"  Total neurons: {brain.get_total_neuron_count()}")
    print(f"  Total synapses: {brain.get_total_synapse_count()}")
    print(f"  Developmental stage: {brain.get_developmental_stage()}")
    
    return brain
def run_simulation_command(args):
    """Run brain simulation."""
    print(f"Running simulation for {args.steps} steps...")
    
    # Create brain
    config = create_default_config()
    if args.config_file:
        config = pynlm.Config()
        config.load_from_file(args.config_file)
    
    brain = create_brain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(args.steps):
        brain.step(step)
        
        if args.verbose and step % args.verbose == 0:
            print(f"Step {step}: {brain.get_firing_neuron_count()} firing neurons")
    
    # Display results
    print(f"\nSimulation completed!")
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print(f"Average firing rate: {brain.get_average_firing_rate():.3f}")
    
    # Save checkpoint if requested
    if args.checkpoint:
        brain.save(args.checkpoint)
        print(f"Checkpoint saved to: {args.checkpoint}")
    
    return brain
def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Brain Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create brain command
    parser_create = subparsers.add_parser("create-brain", help="Create and initialize a brain")
    parser_create.add_argument("--config-file", help="Load configuration from file")
    parser_create.add_argument("--neuron-count", type=int, help="Set neuron count")
    parser_create.add_argument("--synapse-density", type=float, help="Set synapse density")
    parser_create.add_argument("--output", help="Save configuration to file")
    parser_create.set_defaults(func=create_brain_command)
    
    # Run simulation command
    parser_sim = subparsers.add_parser("run-simulation", help="Run brain simulation")
    parser_sim.add_argument("steps", type=int, help="Number of simulation steps")
    parser_sim.add_argument("--config-file", help="Load configuration from file")
    parser_sim.add_argument("--checkpoint", help="Save checkpoint to file")
    parser_sim.add_argument("--verbose", type=int, default=100, help="Verbose interval")
    parser_sim.set_defaults(func=run_simulation_command)
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute command
    if hasattr(args, 'func'):
        try:
            args.func(args)
        except Exception as e:
            print(f"Error: {e}")
            sys.exit(1)
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == "__main__":
    main()