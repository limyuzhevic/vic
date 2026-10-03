#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Command Line Interface

A command-line interface for the NLM brain simulator.
Provides basic commands for running simulations and managing brain configurations.
"""

import sys
import os
import argparse
import json
from pathlib import Path

# Add the Python module to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError as e:
    print(f"Error: Could not import pynlm: {e}")
    print("Please install pynlm first:")
    print("  pip install pynlm")
    sys.exit(1)


def print_version():
    """Print NLM version information."""
    print("NLM (Neural Learning Machine) CLI")
    print("Version: 0.1.0")
    print("Built with Python bindings and C++ neural simulation framework")
    print()


def create_brain(args):
    """Create and initialize a brain."""
    print(f"Creating brain with {args.neurons} neurons...")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set configuration parameters
    if args.neurons:
        config.set("brain.neuron_count", args.neurons)
    if args.regions:
        config.set("brain.region_count", args.regions)
    if args.variance:
        config.set("brain.connection_probability", args.variance)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        print("Error: Failed to initialize brain")
        return None
    
    print(f"Brain created successfully:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")
    
    return brain


def run_simulation(brain, args):
    """Run a simulation with the given brain."""
    print(f"Running simulation for {args.steps} steps...")
    
    for step in range(args.steps):
        brain.step(step)
        
        if step % args.verbose == 0 and args.verbose > 0:
            firing = brain.getFiringNeuronCount()
            total_spikes = brain.getTotalSpikeCount()
            print(f"Step {step}: {firing} firing neurons, {total_spikes} total spikes")
    
    print("Simulation complete!")
    print(f"Final stats:")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Firing neurons (last step): {brain.getFiringNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")


def save_brain(brain, filepath):
    """Save brain state to file."""
    print(f"Saving brain state to {filepath}...")
    if brain.save(filepath):
        print("Brain saved successfully!")
    else:
        print("Error: Failed to save brain")


def load_brain(filepath):
    """Load brain state from file."""
    print(f"Loading brain state from {filepath}...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if brain.load(filepath):
        print("Brain loaded successfully!")
        return brain
    else:
        print("Error: Failed to load brain")
        return None


def inspect_brain(brain):
    """Display brain information."""
    print("=== Brain Inspection ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons (current): {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Pending events: {brain.getPendingSpikeEventCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Get development stage
    try:
        stage = brain.getDevelopmentalStage()
        stage_names = {0: "Initial", 1: "CriticalPeriod", 2: "Maturation", 3: "Adult", 4: "Aging"}
        stage_name = stage_names.get(stage, f"Unknown ({stage})")
        print(f"Developmental stage: {stage_name}")
    except:
        print("Developmental stage: N/A")
    
    # Show regions
    print(f"\nRegions ({brain.getRegionCount()}):")
    for i, region_id in enumerate(brain.getRegionIds()):
        region = brain.getRegion(region_id)
        if region:
            neuron_count = region.getTotalNeuronCount() if hasattr(region, 'getTotalNeuronCount') else 0
            synapse_count = region.getSynapseCount() if hasattr(region, 'getSynapseCount') else 0
            print(f"  Region {i+1} (ID: {region_id}): {neuron_count} neurons, {synapse_count} synapses")


def compare_results():
    """Example: Compare results from different brain configurations."""
    print("Example: Comparing different brain configurations...")
    print("This would run multiple simulations with different parameters")
    print("and compare the results (e.g., learning speed, stability, etc.)")
    print("\nTo use this, create a script that runs multiple configurations")
    print("and calls this function with the results.")


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm create --neurons 1000 --steps 100
  nlm run --steps 1000 --verbose 100
  nlm save brain_state.bin
  nlm load brain_state.bin
  nlm inspect
  nlm compare

For more detailed help, use:
  nlm COMMAND --help
        """
    )
    
    # Add subcommands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create command
    create_parser = subparsers.add_parser("create", help="Create a new brain")
    create_parser.add_argument("--neurons", type=int, default=1000,
                               help="Number of neurons (default: 1000)")
    create_parser.add_argument("--regions", type=int, default=1,
                               help="Number of brain regions (default: 1)")
    create_parser.add_argument("--variance", type=float, default=0.1,
                               help="Connection probability/variance (default: 0.1)")
    
    # Run command
    run_parser = subparsers.add_parser("run", help="Run a simulation")
    run_parser.add_argument("--steps", type=int, default=100,
                            help="Number of simulation steps (default: 100)")
    run_parser.add_argument("--verbose", type=int, default=0,
                            help="Print verbose output every N steps (default: 0)")
    
    # Save command
    save_parser = subparsers.add_parser("save", help="Save brain state")
    save_parser.add_argument("filepath", help="File path to save brain state")
    
    # Load command
    load_parser = subparsers.add_parser("load", help="Load brain state")
    load_parser.add_argument("filepath", help="File path to load brain state")
    
    # Inspect command
    inspect_parser = subparsers.add_parser("inspect", help="Inspect current brain state")
    
    # Compare command
    compare_parser = subparsers.add_parser("compare", help="Compare different brain configurations")
    
    # Version command
    version_parser = subparsers.add_parser("version", help="Show version information")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle subcommands
    if args.command == "create":
        brain = create_brain(args)
        if brain:
            print("\nBrain created successfully!")
            print("You can now run simulations with this brain.")
            print("Example commands:")
            print(f"  nlm run --steps 1000")
            print(f"  nlm save your_brain.bin")
    
    elif args.command == "run":
        # For now, create a simple brain and run simulation
        brain = create_brain(type('Args', (), {
            'neurons': 1000,
            'regions': 1,
            'variance': 0.1
        })())
        if brain:
            run_simulation(brain, args)
    
    elif args.command == "save":
        brain = create_brain(type('Args', (), {
            'neurons': 1000,
            'regions': 1,
            'variance': 0.1
        })())
        if brain:
            save_brain(brain, args.filepath)
    
    elif args.command == "load":
        brain = load_brain(args.filepath)
        if brain:
            inspect_brain(brain)
    
    elif args.command == "inspect":
        # Create a default brain for inspection
        brain = create_brain(type('Args', (), {
            'neurons': 1000,
            'regions': 1,
            'variance': 0.1
        })())
        if brain:
            inspect_brain(brain)
    
    elif args.command == "compare":
        compare_results()
    
    elif args.command == "version":
        print_version()
    
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
