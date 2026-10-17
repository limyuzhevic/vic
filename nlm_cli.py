#!/usr/bin/env python3
"""
NLM Command Line Interface

A collection of command-line tools for NLM (Neural Learning Machine).

Usage examples:
    python nlm_cli.py --help              # Show help
    python nlm_cli.py brain --create      # Create a new brain
    python nlm_cli.py brain --simulate     # Run a simulation
    python nlm_cli.py config --create      # Create configuration
"""

import argparse
import sys
import json
import os
from pathlib import Path

# Add the python directory to the path for local imports
sys.path.insert(0, str(Path(__file__).parent / "python"))

import pynlm_enhanced as nlm

def create_brain(args):
    """Create a new brain with specified configuration"""
    if args.basic:
        brain = nlm.createSimpleBrain(neuron_count=100)
    elif args.advanced:
        brain = nlm.createSimpleBrain(neuron_count=500)
    else:
        brain = nlm.createSimpleBrain(neuron_count=args.neurons)
    
    # Run simulation if requested
    if args.steps > 0:
        brain.simulate(args.steps)
    
    # Save brain if requested
    if args.save:
        brain.save(args.save)
        print(f"Brain saved to {args.save}")
    
    # Print brain status
    status = brain.getStatus()
    print(f"Brain created with {status['neuron_count']} neurons")
    print(f"Simulation steps: {args.steps}")
    print(f"Total spikes: {status['total_spikes']}")
    print(f"Average firing rate: {status['firing_rate']:.2f} Hz")
    print(f"Learning detected: {status['learning_detected']}")
    
    return True

def simulate_brain(args):
    """Run a simulation on an existing brain"""
    if args.file:
        # Load brain from file
        brain = nlm.createSimpleBrain()
        if not brain.load(args.file):
            print(f"Failed to load brain from {args.file}")
            return False
    else:
        # Create new brain
        brain = nlm.createSimpleBrain(neuron_count=args.neurons)
    
    # Run simulation
    brain.simulate(args.steps)
    
    # Print results
    status = brain.getStatus()
    print(f"Simulation completed:")
    print(f"  Neurons: {status['neuron_count']}")
    print(f"  Steps: {args.steps}")
    print(f"  Total spikes: {status['total_spikes']}")
    print(f"  Average firing rate: {status['firing_rate']:.2f} Hz")
    print(f"  Learning detected: {status['learning_detected']}")
    
    # Save if requested
    if args.save:
        brain.save(args.save)
        print(f"Brain saved to {args.save}")
    
    return True

def create_config(args):
    """Create configuration"""
    if args.basic:
        config = nlm.EnhancedConfig.createBasicConfig(neuron_count=args.neurons)
    elif args.advanced:
        config = nlm.EnhancedConfig.createAdvancedConfig(neuron_count=args.neurons)
    else:
        config = nlm.EnhancedConfig()
        config.set("neuron_count", args.neurons)
        config.set("simulation_timestep", args.timestep)
    
    if args.output:
        with open(args.output, 'w') as f:
            f.write(config.toJson())
        print(f"Configuration saved to {args.output}")
    else:
        print(config.toJson())
    
    return True

def network_tools(args):
    """Neural network tools"""
    if args.create:
        # Create a neural network
        network = nlm.createNeuralNetwork(
            input_size=args.input_size,
            hidden_size=args.hidden_size,
            output_size=args.output_size
        )
        print(f"Neural network created:")
        print(f"  Input size: {args.input_size}")
        print(f"  Hidden size: {args.hidden_size}")
        print(f"  Output size: {args.output_size}")
        
        # Get network statistics
        stats = network.getStats()
        print(f"  Total weights: {stats['total_weights']}")
        print(f"  Learning rate: {stats['learning_rate']}")
    
    return True

def main():
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    %(prog)s brain --create --basic --steps 100
    %(prog)s brain --simulate --file mybrain.nlm --steps 500
    %(prog)s config --create --basic --neurons 200 --output config.json
    %(prog)s network --create --input_size 10 --hidden_size 20 --output_size 1
    """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Brain commands
    brain_parser = subparsers.add_parser('brain', help='Brain management commands')
    brain_subparsers = brain_parser.add_subparsers(dest='brain_command', help='Brain subcommands')
    
    create_brain_parser = brain_subparsers.add_parser('create', help='Create a new brain')
    create_brain_parser.add_argument('--neurons', type=int, default=100,
                                     help='Number of neurons (default: 100)')
    create_brain_parser.add_argument('--steps', type=int, default=0,
                                     help='Number of simulation steps (default: 0)')
    create_brain_parser.add_argument('--save', help='Save brain to file')
    create_brain_parser.add_argument('--basic', action='store_true',
                                     help='Use basic configuration')
    create_brain_parser.add_argument('--advanced', action='store_true',
                                     help='Use advanced configuration')
    create_brain_parser.set_defaults(func=create_brain)
    
    simulate_brain_parser = brain_subparsers.add_parser('simulate', help='Run simulation')
    simulate_brain_parser.add_argument('--neurons', type=int, default=100,
                                       help='Number of neurons (default: 100)')
    simulate_brain_parser.add_argument('--steps', type=int, required=True,
                                       help='Number of simulation steps')
    simulate_brain_parser.add_argument('--file', help='Load brain from file')
    simulate_brain_parser.add_argument('--save', help='Save brain to file')
    simulate_brain_parser.set_defaults(func=simulate_brain)
    
    # Config commands
    config_parser = subparsers.add_parser('config', help='Configuration commands')
    config_subparsers = config_parser.add_subparsers(dest='config_command', help='Config subcommands')
    
    create_config_parser = config_subparsers.add_parser('create', help='Create configuration')
    create_config_parser.add_argument('--neurons', type=int, default=100,
                                     help='Number of neurons (default: 100)')
    create_config_parser.add_argument('--timestep', type=float, default=0.001,
                                     help='Simulation timestep (default: 0.001)')
    create_config_parser.add_argument('--output', help='Save configuration to file')
    create_config_parser.add_argument('--basic', action='store_true',
                                     help='Use basic configuration')
    create_config_parser.add_argument('--advanced', action='store_true',
                                     help='Use advanced configuration')
    create_config_parser.set_defaults(func=create_config)
    
    # Network commands
    network_parser = subparsers.add_parser('network', help='Neural network commands')
    network_subparsers = network_parser.add_subparsers(dest='network_command', help='Network subcommands')
    
    create_network_parser = network_subparsers.add_parser('create', help='Create neural network')
    create_network_parser.add_argument('--input_size', type=int, required=True,
                                      help='Input layer size')
    create_network_parser.add_argument('--hidden_size', type=int, required=True,
                                      help='Hidden layer size')
    create_network_parser.add_argument('--output_size', type=int, required=True,
                                      help='Output layer size')
    create_network_parser.set_defaults(func=network_tools)
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute command
    if hasattr(args, 'func'):
        return args.func(args)
    else:
        parser.print_help()
        return 1

if __name__ == "__main__":
    sys.exit(main())
