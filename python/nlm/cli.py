#!/usr/bin/env python3
"""
Command-line interface for NLM (Neural Learning Machine)

Provides commands for:
- Running simulations
- Configuration management
- Performance benchmarking
- Advanced features
"""

import argparse
import sys
import json
import time
from pathlib import Path
from typing import Optional

# Add the parent directory to sys.path to import nlm
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

import nlm
from nlm import Config, createBrain, createSimpleWorld, createAgentBrain


def run_simulation(args):
    """Run a simulation with specified parameters."""
    print(f"Running simulation with {args.steps} steps...")
    
    # Create configuration
    config = Config()
    
    # Apply user settings
    if args.neurons:
        config.set("neuron_count", args.neurons)
    if args.timestep:
        config.set("simulation_timestep", args.timestep)
    
    # Create brain
    brain = createBrain(config)
    brain.initialize()
    
    # Run simulation
    start_time = time.time()
    for step in range(args.steps):
        brain.step(step)
        
        # Progress reporting
        if args.verbose and step % 100 == 0:
            print(f"Step {step}/{args.steps}, Spikes: {brain.getTotalSpikeCount()}")
    
    end_time = time.time()
    
    # Report results
    total_time = end_time - start_time
    print(f"\nSimulation Complete:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"  Total simulation time: {total_time:.2f}s")
    print(f"  Steps per second: {args.steps / total_time:.0f}")


def run_agent_simulation(args):
    """Run a complete agent simulation with world interaction."""
    print(f"Running agent simulation...")
    
    # Create components
    config = Config()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(args.width, args.height, args.vision_width, args.vision_height)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems if requested
    if args.reward_modulation:
        agent.enableRewardModulation(True)
    if args.curiosity:
        agent.enableCuriosity(True)
    if args.development:
        agent.enableDevelopment(True)
    
    # Run simulation loop
    start_time = time.time()
    for step in range(args.steps):
        world.update(args.timestep)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Progress reporting
        if args.verbose and step % 100 == 0:
            print(f"Step {step}/{args.steps}")
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print("\nAgent simulation complete!")
    print(f"  Simulation time: {total_time:.2f}s")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")


def configure_command(args):
    """Manage configuration files."""
    if args.save:
        # Save current configuration to file
        config = Config()
        config.saveToFile(args.config_file)
        print(f"Configuration saved to {args.config_file}")
    
    elif args.load:
        # Load configuration from file
        config = Config()
        if config.loadFromFile(args.config_file):
            print(f"Configuration loaded from {args.config_file}")
        else:
            print(f"Failed to load configuration from {args.config_file}")
            return 1
    
    elif args.show:
        # Show current configuration
        config = Config()
        print("Current configuration:")
        print(config.summary())


def benchmark_command(args):
    """Run performance benchmarks."""
    print("Running NLM performance benchmarks...")
    
    # Create different benchmark configurations
    benchmarks = [
        {
            'config': Config(),
            'steps': 1000,
            'description': 'Small Network (100 neurons)'
        },
        {
            'config': Config(),
            'steps': 500,
            'description': 'Medium Network (500 neurons)'
        },
        {
            'config': Config(),
            'steps': 200,
            'description': 'Large Network (1000 neurons)'
        }
    ]
    
    # Set different neuron counts for each benchmark
    neuron_counts = [100, 500, 1000]
    for i, neuron_count in enumerate(neuron_counts):
        benchmarks[i]['config'].set("neuron_count", neuron_count)
        benchmarks[i]['description'] = f"{benchmarks[i]['description'].split('(')[0].strip()}, {neuron_count} neurons)"
    
    # Run benchmarks
    results = []
    for benchmark in benchmarks:
        config = benchmark['config']
        steps = benchmark['steps']
        description = benchmark['description']
        
        print(f"\nRunning benchmark: {description}")
        print(f"  Neurons: {config.getOr('neuron_count', 0)}")
        print(f"  Steps: {steps}")
        
        # Create brain
        brain = createBrain(config)
        brain.initialize()
        
        # Run simulation and measure performance
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        
        total_time = end_time - start_time
        steps_per_second = steps / total_time
        
        result = {
            'description': description,
            'neuron_count': config.getOr('neuron_count', 0),
            'steps': steps,
            'total_time': total_time,
            'steps_per_second': steps_per_second,
            'spikes_per_step': brain.getTotalSpikeCount() / steps,
            'total_spikes': brain.getTotalSpikeCount(),
        }
        
        print(f"  Time: {total_time:.2f}s")
        print(f"  Steps/sec: {steps_per_second:.0f}")
        print(f"  Spikes/step: {result['spikes_per_step']:.2f}")
        
        results.append(result)
    
    # Summary
    print("\nBenchmark Summary")
    print("=================")
    print(f"{'Description':<30} {'Neurons':<10} {'Steps/sec':<12} {'Spikes/step':<12} {'Time (s)':<10}")
    print("-" * 85)
    
    for result in results:
        print(f"{result['description']:<30} {result['neuron_count']:<10} "
              f"{result['steps_per_second']:<12.0f} "
              f"{result['spikes_per_step']:<12.2f} "
              f"{result['total_time']:<10.2f}")
    
    return results


def visualization_command(args):
    """Generate simple text-based visualization."""
    print("Generating visualization...")
    
    # Create configuration
    config = Config()
    config.set("neuron_count", args.neurons)
    
    # Create brain
    brain = createBrain(config)
    brain.initialize()
    
    # Run a short simulation
    for step in range(args.steps):
        brain.step(step)
    
    # Generate simple visualization
    print(f"\nNLM Brain Activity Visualization")
    print(f"====================================")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Simple activity graph
    if args.graph:
        print(f"\nActivity Graph (last {args.graph} steps):")
        # Generate spike history
        spike_history = []
        # Simulate to get spike history (simplified)
        for step in range(args.steps - args.graph, args.steps):
            brain.step(step)
        
        # Create simple bar chart
        max_spikes = max(1, brain.getTotalSpikeCount() // args.graph)
        for i in range(0, args.steps, args.graph // 10):
            segment_steps = min(args.graph // 10, args.steps - i)
            if segment_steps > 0:
                print(f"  Step {i:6d}-{i+segment_steps-1:6d}: {'█' * (brain.getTotalSpikeCount() // segment_steps // 10)}")


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) - Computational Brain Simulator",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm simulate --steps 1000 --neurons 1000
  nlm agent --steps 500 --width 50 --height 50
  nlm benchmark
  nlm visualize --steps 100 --neurons 500
  nlm config --show
        """
    )
    
    # Global options
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    parser.add_argument(
        "-q", "--quiet",
        action="store_true",
        help="Suppress output (except errors)"
    )
    parser.add_argument(
        "--log-level",
        choices=["DEBUG", "INFO", "WARNING", "ERROR"],
        default="INFO",
        help="Set logging level"
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Simulation command
    sim_parser = subparsers.add_parser(
        "simulate",
        help="Run a brain simulation"
    )
    sim_parser.add_argument(
        "--steps", type=int, default=1000,
        help="Number of simulation steps (default: 1000)"
    )
    sim_parser.add_argument(
        "--neurons", type=int,
        help="Number of neurons to create"
    )
    sim_parser.add_argument(
        "--timestep", type=float, default=0.001,
        help="Simulation timestep in seconds (default: 0.001)"
    )
    
    # Agent command
    agent_parser = subparsers.add_parser(
        "agent",
        help="Run an agent in a world"
    )
    agent_parser.add_argument(
        "--steps", type=int, default=500,
        help="Number of simulation steps (default: 500)"
    )
    agent_parser.add_argument(
        "--width", type=int, default=20,
        help="World width (default: 20)"
    )
    agent_parser.add_argument(
        "--height", type=int, default=20,
        help="World height (default: 20)"
    )
    agent_parser.add_argument(
        "--vision-width", type=int, default=8,
        help="Vision width (default: 8)"
    )
    agent_parser.add_argument(
        "--vision-height", type=int, default=8,
        help="Vision height (default: 8)"
    )
    agent_parser.add_argument(
        "--timestep", type=float, default=0.1,
        help="Timestep duration (default: 0.1)"
    )
    agent_parser.add_argument(
        "--reward-modulation", action="store_true",
        help="Enable reward modulation"
    )
    agent_parser.add_argument(
        "--curiosity", action="store_true",
        help="Enable curiosity-driven exploration"
    )
    agent_parser.add_argument(
        "--development", action="store_true",
        help="Enable developmental processes"
    )
    
    # Configuration command
    config_parser = subparsers.add_parser(
        "config",
        help="Manage configuration"
    )
    config_group = config_parser.add_mutually_exclusive_group()
    config_group.add_argument(
        "--save", action="store_true",
        help="Save current configuration"
    )
    config_group.add_argument(
        "--load", action="store_true",
        help="Load configuration from file"
    )
    config_group.add_argument(
        "--show", action="store_true",
        help="Show current configuration"
    )
    config_parser.add_argument(
        "config_file",
        nargs="?",
        default="nlm_config.json",
        help="Configuration file path (default: nlm_config.json)"
    )
    
    # Benchmark command
    subparsers.add_parser(
        "benchmark",
        help="Run performance benchmarks"
    )
    
    # Visualization command
    viz_parser = subparsers.add_parser(
        "visualize",
        help="Generate brain activity visualization"
    )
    viz_parser.add_argument(
        "--steps", type=int, default=100,
        help="Number of simulation steps (default: 100)"
    )
    viz_parser.add_argument(
        "--neurons", type=int, default=500,
        help="Number of neurons (default: 500)"
    )
    viz_parser.add_argument(
        "--graph", type=int,
        help="Generate activity graph for last N steps"
    )
    
    # Parse arguments
    args = parser.parse_args()
    
    # Set global logging level
    if args.verbose:
        args.log_level = "DEBUG"
    elif args.quiet:
        args.log_level = "WARNING"
    
    # Route to appropriate handler
    if args.command == "simulate":
        run_simulation(args)
    elif args.command == "agent":
        run_agent_simulation(args)
    elif args.command == "config":
        configure_command(args)
    elif args.command == "benchmark":
        benchmark_command(args)
    elif args.command == "visualize":
        visualization_command(args)
    else:
        parser.print_help()
        return 1
    
    return 0


if __name__ == "__main__":
    sys.exit(main())