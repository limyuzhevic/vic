#!/usr/bin/env python3
"""
NLM Command Line Tools

This script provides command-line interface for common NLM operations.
"""

import argparse
import json
import sys
import os
from pathlib import Path

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install NLM Python bindings.")
    sys.exit(1)


def create_config(args):
    """Create a configuration file."""
    config = pynlm.createDefaultConfig()
    
    # Apply command line arguments
    if args.neuron_count:
        config.set("brain.neuron_count", args.neuron_count)
    if args.region_count:
        config.set("brain.region_count", args.region_count)
    if args.connection_probability:
        config.set("brain.connection_probability", args.connection_probability)
    if args.stdp_learning_rate:
        config.set("plasticity.stdp.learning_rate", args.stdp_learning_rate)
    if args.curiosity:
        config.set("neuromod.curiosity.enable", args.curiosity)
    if args.novelty:
        config.set("neuromod.novelty.enable", args.novelty)
    if args.working_memory:
        config.set("working_memory.capacity", args.working_memory)
    if args.development:
        config.set("development.enable", args.development)
    
    # Save configuration
    with open(args.output, 'w') as f:
        # For now, we'll just create a simple JSON representation
        config_data = {}
        if args.neuron_count:
            config_data['neuron_count'] = args.neuron_count
        if args.region_count:
            config_data['region_count'] = args.region_count
        if args.connection_probability:
            config_data['connection_probability'] = args.connection_probability
        
        json.dump(config_data, f, indent=2)
    
    print(f"Configuration saved to {args.output}")


def run_simple_test(args):
    """Run a simple test of the brain."""
    print("=== Running Simple Brain Test ===")
    
    config = pynlm.createDefaultConfig()
    
    # Configure based on arguments
    if args.neuron_count:
        config.set("brain.neuron_count", args.neuron_count)
    if args.steps:
        steps = args.steps
    else:
        steps = 100
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
    
    print(f"Simulation completed!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")


def run_plasticity_test(args):
    """Run plasticity learning test."""
    print("=== Running Plasticity Test ===")
    
    config = pynlm.createDefaultConfig()
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Inject current to stimulate learning
    for i in range(min(10, brain.getTotalNeuronCount())):
        brain.injectCurrent(pynlm.NeuronId(i), 30.0)
    
    print(f"Applied current to 10 neurons")
    
    # Run plasticity simulation
    for step in range(200):
        brain.step(step)
        
        if step % 50 == 0:
            print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, "
                  f"Spikes={brain.getTotalSpikeCount()}")
    
    print(f"Plasticity test completed!")


def save_brain(args):
    """Save brain state to file."""
    print(f"=== Saving Brain State to {args.output} ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(args.steps):
        brain.step(step)
    
    # Save brain state
    brain.save(args.output)
    print(f"Brain state saved to {args.output}")


def load_brain(args):
    """Load brain state from file."""
    print(f"=== Loading Brain State from {args.input} ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Load brain state
    brain.load(args.input)
    
    print(f"Brain state loaded successfully!")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")


def benchmark(args):
    """Run benchmark tests."""
    print(f"=== Running Benchmark Tests ===")
    
    import time
    
    config = pynlm.createDefaultConfig()
    
    # Configure benchmark
    if args.neuron_count:
        config.set("brain.neuron_count", args.neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run benchmark
    start_time = time.time()
    steps = args.steps if args.steps else 1000
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"Benchmark completed!")
    print(f"Steps: {steps}")
    print(f"Duration: {duration:.2f} seconds")
    print(f"Steps per second: {steps/duration:.2f}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")


def main():
    parser = argparse.ArgumentParser(
        description="NLM Command Line Tools - Common NLM operations",
        epilog="Use 'nlm <command> --help' for more information about specific commands."
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # create-config command
    parser_create = subparsers.add_parser('create-config', 
                                          help='Create a configuration file')
    parser_create.add_argument('--output', '-o', required=True,
                               help='Output configuration file path')
    parser_create.add_argument('--neuron-count', type=int,
                               help='Number of neurons')
    parser_create.add_argument('--region-count', type=int,
                               help='Number of neural regions')
    parser_create.add_argument('--connection-probability', type=float,
                               help='Synapse connection probability')
    parser_create.add_argument('--stdp-learning-rate', type=float,
                               help='STDP learning rate')
    parser_create.add_argument('--curiosity', action='store_true',
                               help='Enable curiosity modulation')
    parser_create.add_argument('--novelty', action='store_true',
                               help='Enable novelty detection')
    parser_create.add_argument('--working-memory', type=int,
                               help='Working memory capacity')
    parser_create.add_argument('--development', action='store_true',
                               help='Enable development stages')
    parser_create.set_defaults(func=create_config)
    
    # test command
    parser_test = subparsers.add_parser('test', help='Run NLM tests')
    parser_test.add_argument('type', choices=['simple', 'plasticity', 'stdp', 'all'],
                            help='Type of test to run')
    parser_test.add_argument('--neuron-count', type=int,
                            help='Number of neurons (default: 1000)')
    parser_test.add_argument('--steps', type=int,
                            help='Number of simulation steps (default: 100)')
    parser_test.set_defaults(func=lambda args: None)  # Will handle in main
    
    # save command
    parser_save = subparsers.add_parser('save', help='Save brain state')
    parser_save.add_argument('--output', '-o', required=True,
                            help='Output brain state file path')
    parser_save.add_argument('--steps', type=int, default=100,
                            help='Number of simulation steps (default: 100)')
    parser_save.set_defaults(func=save_brain)
    
    # load command
    parser_load = subparsers.add_parser('load', help='Load brain state')
    parser_load.add_argument('--input', '-i', required=True,
                            help='Input brain state file path')
    parser_load.set_defaults(func=load_brain)
    
    # benchmark command
    parser_bench = subparsers.add_parser('benchmark', help='Run benchmark tests')
    parser_bench.add_argument('--neuron-count', type=int,
                             help='Number of neurons (default: 1000)')
    parser_bench.add_argument('--steps', type=int, default=1000,
                             help='Number of simulation steps (default: 1000)')
    parser_bench.set_defaults(func=benchmark)
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        sys.exit(1)
    
    try:
        # Handle test commands
        if args.command == 'test':
            if args.type == 'simple':
                args.func = run_simple_test
            elif args.type == 'plasticity':
                args.func = run_plasticity_test
            elif args.type == 'stdp':
                # For now, just run plasticity test as placeholder
                args.func = run_plasticity_test
            elif args.type == 'all':
                print("Running all tests...")
                run_simple_test(args)
                run_plasticity_test(args)
                print("All tests completed!")
                return
            
            args.func(args)
        else:
            # Handle other commands
            args.func(args)
    
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == '__main__':
    main()