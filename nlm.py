#!/usr/bin/env python3
"""
Command-line interface for NLM (Neural Learning Machine)

This CLI provides command-line access to NLM functionality for:
- Running simulations
- Managing configurations
- Performance benchmarking
- Brain saving/loading
- Experiment management
"""

import sys
import os
import argparse
import json
import time
from pathlib import Path

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pynlm
from pynlm.enhanced import ConfigPresets, ExperimentManager, BenchmarkSuite, SerializationUtils

def create_parser():
    """Create the argument parser"""
    parser = argparse.ArgumentParser(
        description="Command-line interface for NLM (Neural Learning Machine)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run a simple simulation
  nlm run --neurons 1000 --steps 500
  
  # Run with a preset configuration
  nlm run --preset full --steps 1000
  
  # Benchmark brain creation
  nlm benchmark creation --count 100
  
  # Run agent experiment
  nlm experiment agent --neurons 2000 --world-width 30 --world-height 30
  
  # Save brain state
  nlm brain save brain_state.bin --neurons 1000
  
  # Load brain state
  nlm brain load brain_state.bin
  
  # List available commands
  nlm help
        ""
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run simulation command
    run_parser = subparsers.add_parser('run', help='Run a simulation')
    run_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    run_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    run_parser.add_argument('--preset', choices=['minimal', 'full', 'development'], 
                          help='Use a preset configuration')
    run_parser.add_argument('--output', '-o', help='Output file for results')
    
    # Benchmark command
    bench_parser = subparsers.add_parser('benchmark', help='Run benchmarks')
    bench_subparsers = bench_parser.add_subparsers(dest='benchmark_type')
    
    creation_parser = bench_subparsers.add_parser('creation', help='Benchmark brain creation')
    creation_parser.add_argument('--count', type=int, default=100, help='Number of brains to create')
    
    simulation_parser = bench_subparsers.add_parser('simulation', help='Benchmark simulation performance')
    simulation_parser.add_argument('--steps', type=int, default=1000, help='Number of steps per simulation')
    simulation_parser.add_argument('--runs', type=int, default=10, help='Number of simulation runs')
    
    # Brain command
    brain_parser = subparsers.add_parser('brain', help='Brain operations')
    brain_subparsers = brain_parser.add_subparsers(dest='brain_action')
    
    save_parser = brain_subparsers.add_parser('save', help='Save brain state')
    save_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    save_parser.add_argument('filepath', help='Output file path')
    
    load_parser = brain_subparsers.add_parser('load', help='Load brain state')
    load_parser.add_argument('filepath', help='Input file path')
    
    # Experiment command
    exp_parser = subparsers.add_parser('experiment', help='Run experiments')
    exp_subparsers = exp_parser.add_subparsers(dest='experiment_type')
    
    agent_parser = exp_subparsers.add_parser('agent', help='Run agent experiment')
    agent_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    agent_parser.add_argument('--world-width', type=float, default=20.0, help='World width')
    agent_parser.add_argument('--world-height', type=float, default=20.0, help='World height')
    agent_parser.add_argument('--steps', type=int, default=1000, help='Number of steps')
    agent_parser.add_argument('--enable-learning', action='store_true', help='Enable reward modulation')
    agent_parser.add_argument('--enable-development', action='store_true', help='Enable development system')
    agent_parser.add_argument('--enable-curiosity', action='store_true', help='Enable curiosity system')
    
    # List command
    list_parser = subparsers.add_parser('list', help='List available presets or help')
    list_parser.add_argument('type', nargs='?', choices=['presets', 'help'], default='presets', 
                           help='What to list')
    
    return parser

def run_simulation(args):
    """Run a simulation"""
    print(f"Running simulation with {args.neurons} neurons for {args.steps} steps...")
    
    # Get configuration
    if args.preset:
        config = getattr(ConfigPresets, args.preset)()
        print(f"Using {args.preset} preset configuration")
    else:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", args.neurons)
        print(f"Using custom configuration with {args.neurons} neurons")
    
    # Create and run brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(args.steps):
        brain.step(step)
    end_time = time.time()
    
    # Collect results
    results = {
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'final_spike_count': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'simulation_steps': args.steps,
        'simulation_time': end_time - start_time,
        'spikes_per_second': brain.getTotalSpikeCount() / (end_time - start_time) if (end_time - start_time) > 0 else 0,
        'configuration': {
            'neuron_count': args.neurons,
            'preset': args.preset,
        }
    }
    
    # Print results
    print("\n=== Simulation Results ===")
    print(f"Neurons: {results['neuron_count']}")
    print(f"Synapses: {results['synapse_count']}")
    print(f"Final Spike Count: {results['final_spike_count']}")
    print(f"Average Firing Rate: {results['average_firing_rate']:.2f} Hz")
    print(f"Simulation Time: {results['simulation_time']:.2f} seconds")
    print(f"Spikes per Second: {results['spikes_per_second']:.2f}")
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to {args.output}")
    
    return results

def run_benchmark_creation(args):
    """Run benchmark for brain creation"""
    print(f"Benchmarking creation of {args.count} brains...")
    
    results = BenchmarkSuite.benchmark_creation(args.count, 100)
    
    print("\n=== Brain Creation Benchmark Results ===")
    print(f"Brains tested: {results['brains_tested']}")
    print(f"Total time: {results['total_creation_time']:.2f} seconds")
    print(f"Average time per brain: {results['average_creation_time_per_brain']:.4f} seconds")
    print(f"Brains per second: {results['brains_per_second']:.2f}")
    
    return results

def run_benchmark_simulation(args):
    """Run benchmark for simulation performance"""
    print(f"Benchmarking {args.runs} simulations with {args.steps} steps each...")
    
    config = pynlm.createDefaultConfig()
    results = BenchmarkSuite.benchmark_simulation(config, args.steps, args.runs)
    
    print("\n=== Simulation Benchmark Results ===")
    print(f"Total simulations: {results['total_simulations']}")
    print(f"Steps per simulation: {results['steps_per_simulation']}")
    print(f"Average time per simulation: {results['average_simulation_time']:.4f} seconds")
    
    return results

def save_brain(args):
    """Save brain state"""
    print(f"Saving brain state with {args.neurons} neurons to {args.filepath}...")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", args.neurons)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a few steps to generate some activity
    for step in range(100):
        brain.step(step)
    
    success = SerializationUtils.save_brain_to_file(brain, args.filepath)
    
    if success:
        print(f"Brain state saved successfully to {args.filepath}")
    else:
        print(f"Failed to save brain state to {args.filepath}")
    
    return success

def load_brain(args):
    """Load brain state"""
    print(f"Loading brain state from {args.filepath}...")
    
    if not os.path.exists(args.filepath):
        print(f"Error: File {args.filepath} does not exist")
        return False
    
    config = pynlm.createDefaultConfig()
    
    brain = SerializationUtils.load_brain_from_file(args.filepath, config)
    
    if brain:
        print("Brain state loaded successfully")
        print(f"Loaded brain has {brain.getTotalNeuronCount()} neurons")
        print(f"Loaded brain has {brain.getTotalSynapseCount()} synapses")
        return True
    else:
        print("Failed to load brain state")
        return False

def run_experiment_agent(args):
    """Run agent experiment"""
    print(f"Running agent experiment...")
    
    world_config = {
        'width': args.world_width,
        'height': args.world_height,
        'visionWidth': 16,
        'visionHeight': 16,
    }
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", args.neurons)
    
    manager = ExperimentManager()
    
    world_config_dict = {
        'width': args.world_width,
        'height': args.world_height,
        'visionWidth': 16,
        'visionHeight': 16,
    }
    
    results = manager.run_agent_experiment(
        config, 
        world_config_dict,
        num_steps=args.steps,
        enable_learning=args.enable_learning,
        enable_development=args.enable_development,
        enable_curiosity=args.enable_curiosity
    )
    
    print("\n=== Agent Experiment Results ===")
    print(f"Total reward: {results['total_reward']:.1f}")
    print(f"Average firing rate: {results['avg_firing_rate']:.2f} Hz")
    print(f"Average curiosity: {results['avg_curiosity']:.3f}")
    print(f"Development stage: {results['development_stage']}")
    print(f"Learned preference: {results['learned_preference']:.3f}")
    
    return results

def list_presets():
    """List available configuration presets"""
    print("Available configuration presets:")
    print()
    
    presets = [
        ("minimal", "Minimal configuration for basic testing"),
        ("full", "Full-featured configuration for complex simulations"),
    ]
    
    for name, description in presets:
        print(f"  {name:20} - {description}")
    
    print()
    print("To use a preset, add --preset <name> to any command that accepts it")

def show_help():
    """Show help information"""
    print("NLM Command-Line Interface")
    print("=" * 50)
    print()
    print("Available commands:")
    print("  nlm run          Run a simulation")
    print("  nlm benchmark    Run benchmarks")
    print("  nlm brain        Brain operations")
    print("  nlm experiment   Run experiments")
    print("  nlm list        List presets or help")
    print()
    print("For more information about a specific command:")
    print("  nlm help <command>")

def main():
    """Main entry point"""
    parser = create_parser()
    args = parser.parse_args()
    
    # Map commands to functions
    command_handlers = {
        'run': run_simulation,
        'benchmark': {
            'creation': run_benchmark_creation,
            'simulation': run_benchmark_simulation,
        },
        'brain': {
            'save': save_brain,
            'load': load_brain,
        },
        'experiment': {
            'agent': run_experiment_agent,
        },
        'list': list_presets,
    }
    
    # Handle nested commands (e.g., benchmark creation)
    try:
        if args.command == 'benchmark' and args.benchmark_type:
            handler = command_handlers[args.command][args.benchmark_type]
            return handler(args)
        elif args.command == 'brain' and args.brain_action:
            handler = command_handlers[args.command][args.brain_action]
            return handler(args)
        elif args.command == 'experiment' and args.experiment_type:
            handler = command_handlers[args.command][args.experiment_type]
            return handler(args)
        elif args.command in command_handlers:
            handler = command_handlers[args.command]
            return handler(args)
        elif args.command == 'help':
            show_help()
            return 0
        else:
            print(f"Unknown command: {args.command}")
            parser.print_help()
            return 1
    except KeyError:
        print(f"Unknown command: {args.command}")
        parser.print_help()
        return 1

if __name__ == "__main__":
    sys.exit(main())
