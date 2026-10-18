#!/usr/bin/env python3
"""
NLM Command Line Interface

This module provides command-line tools for working with NLM (Neural Learning Machine).
"""

import argparse
import sys
import os
from typing import Optional
import traceback
import time

import pynlm


def create_parser() -> argparse.ArgumentParser:
    """Create the main argument parser."""
    parser = argparse.ArgumentParser(
        prog="pynlm-cli",
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  pynlm-cli run --steps 100                    # Run a basic brain simulation
  pynlm-cli create --neurons 1000              # Create a brain with specific size
  pynlm-cli benchmark --brains 10 --steps 100   # Benchmark performance
  pynlm-cli experiment --steps 500 --learning   # Run complete agent experiment
  pynlm-cli config --file config.json          # Load/save configuration
  pynlm-cli help                               # Show help information
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Run command
    parser_run = subparsers.add_parser(
        "run", 
        help="Run a basic brain simulation",
        description="Run a simple brain simulation with the given configuration"
    )
    parser_run.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser_run.add_argument("--neurons", type=int, help="Number of neurons (overrides config)")
    parser_run.add_argument("--synapses", type=float, help="Synapse density (0.0-1.0)")
    parser_run.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    # Create command
    parser_create = subparsers.add_parser(
        "create",
        help="Create a brain with specific configuration",
        description="Create and initialize a brain with custom parameters"
    )
    parser_create.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser_create.add_argument("--density", type=float, default=0.1, help="Synapse density")
    parser_create.add_argument("--learning-rate", type=float, default=0.001, help="Learning rate")
    parser_create.add_argument("--file", help="Save configuration to file")
    parser_create.add_argument("--output", help="Output brain state to file")
    
    # Benchmark command
    parser_benchmark = subparsers.add_parser(
        "benchmark",
        help="Benchmark NLM performance",
        description="Benchmark brain creation and simulation performance"
    )
    parser_benchmark.add_argument("--brains", type=int, default=10, help="Number of brains to create")
    parser_benchmark.add_argument("--steps", type=int, default=100, help="Simulation steps per benchmark")
    parser_benchmark.add_argument("--neurons", type=int, default=500, help="Number of neurons per brain")
    parser_benchmark.add_argument("--type", choices=["creation", "simulation", "both"], 
                                default="both", help="Type of benchmark to run")
    
    # Experiment command
    parser_experiment = subparsers.add_parser(
        "experiment",
        help="Run complete agent experiment",
        description="Run a complete agent simulation with world interaction"
    )
    parser_experiment.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    parser_experiment.add_argument("--width", type=int, default=20, help="World width")
    parser_experiment.add_argument("--height", type=int, default=20, help="World height")
    parser_experiment.add_argument("--learning", action="store_true", help="Enable learning")
    parser_experiment.add_argument("--development", action="store_true", help="Enable development")
    parser_experiment.add_argument("--curiosity", action="store_true", help="Enable curiosity")
    parser_experiment.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    # Config command
    parser_config = subparsers.add_parser(
        "config",
        help="Manage configuration files",
        description="Load, save, and manipulate configuration files"
    )
    parser_config.add_argument("action", choices=["load", "save", "create"], help="Action to perform")
    parser_config.add_argument("--file", help="Configuration file path")
    parser_config.add_argument("--key", help="Configuration key")
    parser_config.add_argument("--value", help="Configuration value")
    parser_config.add_argument("--list", action="store_true", help="List all configuration keys")
    
    # Info command
    parser_info = subparsers.add_parser(
        "info",
        help="Display system information",
        description="Display information about the NLM system and current state"
    )
    
    # Version command
    parser_version = subparsers.add_parser(
        "version",
        help="Display version information",
        description="Display NLM version and system information"
    )
    
    return parser


def handle_run(args):
    """Handle the 'run' command."""
    print("Running NLM brain simulation...")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Override configuration if specified
    if args.neurons:
        config.set("brain.neuron_count", args.neurons)
    if args.synapses:
        config.set("brain.synapse_density", args.synapses)
    
    if args.verbose:
        print(f"Configuration: {config.summary()}")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation
    start_time = time.time()
    
    for step in range(args.steps):
        brain.step(step)
        
        if args.verbose and step % 50 == 0:
            metrics = pynlm.SimulationMetrics.collect(brain)
            print(f"Step {step}: {metrics['firing_neuron_count']} firing neurons, "
                  f"{metrics['average_firing_rate']:.2f} Hz average rate")
    
    end_time = time.time()
    
    # Print final metrics
    metrics = pynlm.SimulationMetrics.collect(brain)
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total steps: {args.steps}")
    print(f"Steps per second: {args.steps / (end_time - start_time):.2f}")
    print(f"Total spikes: {metrics['total_spike_count']}")
    print(f"Final firing rate: {metrics['average_firing_rate']:.2f} Hz")
    print(f"Development stage: {metrics['developmental_stage']}")
    
    # Save brain state if requested
    if args.output:
        brain.save(args.output)
        print(f"Brain state saved to {args.output}")


def handle_create(args):
    """Handle the 'create' command."""
    print("Creating NLM brain with custom configuration...")
    
    # Create configuration with custom parameters
    config = pynlm.BrainConfig.with_parameters(
        neuron_count=args.neurons,
        synapse_density=args.density,
        learning_rate=args.learning_rate
    )
    
    print(f"Created configuration: {config.summary()}")
    
    # Save configuration if requested
    if args.file:
        success = pynlm.BrainConfig.save_to_file(config, args.file)
        if success:
            print(f"Configuration saved to {args.file}")
        else:
            print(f"Failed to save configuration to {args.file}")
            return 1
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Save brain state if requested
    if args.output:
        brain.save(args.output)
        print(f"Brain state saved to {args.output}")
    
    return 0


def handle_benchmark(args):
    """Handle the 'benchmark' command."""
    print(f"Running NLM benchmarks (type: {args.type})...")
    
    # Benchmark brain creation
    if args.type in ["creation", "both"]:
        print("\n=== Brain Creation Benchmark ===")
        results = pynlm.benchmark_brain_creation(
            num_brains=args.brains,
            neuron_count=args.neurons
        )
        
        print(f"Created {args.brains} brains in {results['creation_time']:.2f} seconds")
        print(f"Brains per second: {results['brains_per_second']:.2f}")
        print(f"Average creation time: {results['avg_creation_time']:.4f} seconds")
    
    # Benchmark simulation
    if args.type in ["simulation", "both"]:
        print("\n=== Simulation Benchmark ===")
        
        # Create test configuration
        config = pynlm.BrainConfig.with_parameters(neuron_count=args.neurons)
        
        # Create brain
        brain = pynlm.createBrain(config)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        results = pynlm.benchmark_simulation(brain, world, args.steps)
        
        print(f"Simulated {args.steps} steps in {results['simulation_time']:.2f} seconds")
        print(f"Steps per second: {results['steps_per_second']:.2f}")
        print(f"Average step time: {results['avg_step_time']:.4f} seconds")


def handle_experiment(args):
    """Handle the 'experiment' command."""
    print("Running NLM agent experiment...")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=args.width,
        height=args.height,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(args.width // 2, args.height // 2)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    brain.initialize()
    agent.initialize(world)
    
    # Enable subsystems based on arguments
    if args.learning:
        agent.enableRewardModulation(True)
    if args.development:
        agent.enableDevelopment(True)
    if args.curiosity:
        agent.enableCuriosity(True)
    
    print(f"Starting experiment with {args.steps} steps")
    print(f"Learning enabled: {args.learning}")
    print(f"Development enabled: {args.development}")
    print(f"Curiosity enabled: {args.curiosity}")
    
    # Run experiment
    start_time = time.time()
    
    for step in range(args.steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward if learning is enabled
        if args.learning:
            # Simple reward based on energy
            reward = world.getAgentBody().energy / 100.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if args.development:
            agent.updateDevelopment(0.1)
        
        # Print progress
        if args.verbose and step % 100 == 0:
            metrics = pynlm.SimulationMetrics.collect(brain, world)
            print(f"Step {step}:")
            print(f"  Firing neurons: {metrics['firing_neuron_count']}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {metrics['developmental_stage']}")
    
    end_time = time.time()
    
    # Print final metrics
    final_metrics = pynlm.SimulationMetrics.collect(brain, world)
    
    print(f"\nExperiment completed in {end_time - start_time:.2f} seconds")
    print(f"Total steps: {args.steps}")
    print(f"Steps per second: {args.steps / (end_time - start_time):.2f}")
    print(f"Final firing rate: {final_metrics['average_firing_rate']:.2f} Hz")
    print(f"Final development stage: {final_metrics['developmental_stage']}")
    print(f"Final agent position: ({final_metrics['agent_position'][0]:.1f}, {final_metrics['agent_position'][1]:.1f})")
    print(f"Final agent energy: {final_metrics['agent_energy']:.1f}")


def handle_config(args):
    """Handle the 'config' command."""
    if args.action == "create":
        # Create a default configuration
        config = pynlm.createDefaultConfig()
        print("Created default configuration")
        print(config.summary())
        
        if args.file:
            pynlm.BrainConfig.save_to_file(config, args.file)
            print(f"Saved to {args.file}")
    
    elif args.action == "load":
        if not args.file:
            print("Error: --file is required for load action")
            return 1
        
        config = pynlm.BrainConfig.from_file(args.file)
        print(f"Loaded configuration from {args.file}")
        print(config.summary())
        
        if args.list:
            print(f"\nConfiguration keys: {config.getKeys()}")
        
        if args.key and args.value:
            config.set(args.key, args.value)
            print(f"Set {args.key} = {args.value}")
    
    elif args.action == "save":
        if not args.file:
            print("Error: --file is required for save action")
            return 1
        
        # Use current config if available, otherwise create default
        if hasattr(pynlm, '_default_config') and pynlm._default_config:
            config = pynlm._default_config
        else:
            config = pynlm.createDefaultConfig()
        
        success = pynlm.BrainConfig.save_to_file(config, args.file)
        if success:
            print(f"Saved configuration to {args.file}")
        else:
            print(f"Failed to save configuration to {args.file}")
            return 1
    
    return 0


def handle_info():
    """Handle the 'info' command."""
    print("NLM System Information")
    print("=" * 50)
    print(f"Version: {pynlm.__version__}")
    print(f"Author: {pynlm.__author__}")
    print(f"Description: {pynlm.__doc__}")
    print(f"Registered brains: {len(pynlm.list_brains())}")
    
    if pynlm.list_brains():
        print("\nRegistered brain names:")
        for name in pynlm.list_brains():
            print(f"  - {name}")


def handle_version():
    """Handle the 'version' command."""
    print(f"NLM Python Module")
    print(f"Version: {pynlm.__version__}")
    print(f"Author: {pynlm.__author__}")
    print(f"License: MIT")
    
    # Print Python version
    import platform
    print(f"Python: {platform.python_version()}")
    print(f"Platform: {platform.system()} {platform.release()}")


def main():
    """Main entry point for the CLI."""
    parser = create_parser()
    
    try:
        args = parser.parse_args()
        
        if args.command == "run":
            handle_run(args)
        elif args.command == "create":
            handle_create(args)
        elif args.command == "benchmark":
            handle_benchmark(args)
        elif args.command == "experiment":
            handle_experiment(args)
        elif args.command == "config":
            return handle_config(args)
        elif args.command == "info":
            handle_info()
        elif args.command == "version":
            handle_version()
        else:
            parser.print_help()
            return 1
        
        return 0
        
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        if "-v" in sys.argv or "--verbose" in sys.argv:
            traceback.print_exc()
        else:
            print("Use -v or --verbose for detailed error information")
        return 1


if __name__ == "__main__":
    main()