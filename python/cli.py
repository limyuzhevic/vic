"""
CLI tools for the NLM Python package.

This module provides command-line interfaces for common NLM operations,
making it easy to run simulations, analyze results, and explore the API
without writing Python code.
"""

import sys
import argparse
import logging
from pathlib import Path
from typing import Optional
import json
import time

import pynlm


def setup_logging(level: str = "INFO"):
    """Set up logging configuration."""
    logging.basicConfig(
        level=getattr(logging, level.upper()),
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Python Bindings",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-explore --help                    Show help information
  nlm-sim --config config.json          Run simulation from config
  nlm-analyze --file results.json       Analyze simulation results
        ""
    )
    
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Enable verbose logging'
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Explore command
    explore_parser = subparsers.add_parser(
        'explore',
        help='Explore NLM capabilities and API'
    )
    explore_parser.add_argument(
        '--steps', '-s',
        type=int,
        default=100,
        help='Number of simulation steps'
    )
    explore_parser.add_argument(
        '--neurons', '-n',
        type=int,
        default=1000,
        help='Number of neurons in brain'
    )
    
    # Sim command
    sim_parser = subparsers.add_parser(
        'sim',
        help='Run NLM simulation'
    )
    sim_parser.add_argument(
        'config',
        type=argparse.FileType('r'),
        help='Configuration file (JSON format)'
    )
    sim_parser.add_argument(
        'output',
        nargs='?',
        type=argparse.FileType('w'),
        help='Output file for results'
    )
    sim_parser.add_argument(
        '--steps', '-s',
        type=int,
        help='Number of simulation steps'
    )
    sim_parser.add_argument(
        '--continue', 'continue_',
        action='store_true',
        help='Continue from existing checkpoint'
    )
    
    # Analyze command
    analyze_parser = subparsers.add_parser(
        'analyze',
        help='Analyze simulation results'
    )
    analyze_parser.add_argument(
        'file',
        type=argparse.FileType('r'),
        help='Results file (JSON format)'
    )
    analyze_parser.add_argument(
        '--format', '-f',
        choices=['text', 'json'],
        default='text',
        help='Output format'
    )
    
    args = parser.parse_args()
    
    # Set up logging
    setup_logging('DEBUG' if args.verbose else 'INFO')
    logger = logging.getLogger(__name__)
    
    # Dispatch to command handlers
    if args.command == 'explore':
        explore_command(args)
    elif args.command == 'sim':
        sim_command(args)
    elif args.command == 'analyze':
        analyze_command(args)
    else:
        parser.print_help()
        sys.exit(1)


def explore_command(args):
    """Handle the explore command."""
    logger.info("Starting NLM exploration")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", args.neurons)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"NLM Exploration")
    print(f"===============\n")
    print(f"Brain Configuration:")
    print(f"  Neuron count: {brain.getTotalNeuronCount()}")
    print(f"  Synapse count: {brain.getTotalSynapseCount()}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}\n")
    
    # Run exploration simulation
    print(f"Running {args.steps}-step simulation:")
    
    for step in range(args.steps):
        brain.step(step)
        
        if step % 25 == 0 or step == args.steps - 1:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            spikes = brain.getTotalSpikeCount()
            
            print(f"  Step {step:4d}: {firing:5d} firing, {rate:6.2f} Hz, {spikes:7d} total spikes")
    
    # Print statistics
    print(f"\nExploration Statistics:")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")


def sim_command(args):
    """Handle the sim command."""
    logger.info("Starting NLM simulation")
    
    # Load configuration
    config_data = json.load(args.config)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Apply configuration
    if "brain" in config_data:
        brain_config = config_data["brain"]
        if "neuron_count" in brain_config:
            config.set("brain.neuron_count", brain_config["neuron_count"])
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Determine number of steps
    steps = args.steps if args.steps is not None else 100
    
    print(f"Running NLM simulation ({steps} steps)")
    
    # Run simulation
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 50 == 0:
            print(f"Step {step}/{steps}: {brain.getFiringNeuronCount()} neurons firing")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"Simulation completed in {duration:.2f} seconds")
    
    # Prepare results
    results = {
        "steps": steps,
        "duration": duration,
        "final_stats": {
            "neurons": brain.getTotalNeuronCount(),
            "synapses": brain.getTotalSynapseCount(),
            "firing": brain.getFiringNeuronCount(),
            "spikes": brain.getTotalSpikeCount(),
            "rate": brain.getAverageFiringRate(),
            "ei_ratio": brain.getExcitationInhibitionRatio(),
            "stage": str(brain.getDevelopmentalStage()),
        }
    }
    
    # Save results if output file specified
    if args.output:
        json.dump(results, args.output, indent=2)
        print(f"Results saved to {args.output.name}")


def analyze_command(args):
    """Handle the analyze command."""
    logger.info("Analyzing NLM results")
    
    # Load results
    results = json.load(args.file)
    
    # Extract statistics
    stats = results.get("final_stats", {})
    
    print(f"NLM Results Analysis")
    print(f"==================\n")
    
    if args.format == "json":
        print(json.dumps(results, indent=2))
    else:
        print("Summary Statistics:")
        print(f"  Number of steps: {results.get('steps', 'N/A')}")
        print(f"  Simulation duration: {results.get('duration', 0):.2f} seconds")
        print(f"\nFinal Brain Statistics:")
        
        for key, value in stats.items():
            if isinstance(value, (int, float)):
                print(f"  {key}: {value:.4f}")
            else:
                print(f"  {key}: {value}")
        
        print(f"\nKey Insights:")
        
        # Check developmental stage
        stage = stats.get("stage", "")
        if "Adult" in stage:
            print(f"  ✓ Brain reached adult developmental stage")
        elif "CriticalPeriod" in stage:
            print(f"  ⚠ Brain is in critical period (rapid learning phase)")
        
        # Check firing rate
        rate = stats.get("rate", 0)
        if rate > 1.0:
            print(f"  ✓ Healthy firing rate: {rate:.2f} Hz")
        elif rate > 0.1:
            print(f"  ✓ Moderate firing rate: {rate:.2f} Hz")
        else:
            print(f"  ⚠ Low firing rate: {rate:.2f} Hz (brain may need stimulation)")
        
        # Check E/I balance
        ei_ratio = stats.get("ei_ratio", 1.0)
        if 0.5 <= ei_ratio <= 2.0:
            print(f"  ✓ Good excitation/inhibition balance: {ei_ratio:.3f}")
        else:
            print(f"  ⚠ Unusual excitation/inhibition ratio: {ei_ratio:.3f}")


if __name__ == "__main__":
    main()
