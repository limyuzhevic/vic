#!/usr/bin/env python3
"""
NLM Command Line Interface

A command-line tool for common NLM (Neural Learning Machine) operations.
Provides easy access to brain simulation, configuration management,
and experiment running capabilities.

Usage:
  nlm-cli <command> [options]

Commands:
  create-brain     Create and initialize a brain
  simulate         Run brain simulation
  agent-sim        Run agent simulation
  config           Configuration management
  save-load        Save and load brain states
  stats            Display brain statistics
  experiment       Run experiments
  benchmark        Performance benchmarking
  help             Show help information

For more help on specific commands:
  nlm-cli help <command>
"""

import argparse
import json
import os
import sys
from typing import Optional, List
import time

# Import NLM module
try:
    import pynlm
    from pynlm import (
        create_default_config, create_brain, create_simple_world, 
        create_agent_brain, Brain, Config, SimpleWorld, AgentBrain
    )
except ImportError as e:
    print(f"Error: Could not import pynlm: {e}")
    print("Please install pynlm: pip install pynlm")
    sys.exit(1)

# Constants
DEFAULT_CONFIG_PATH = "nlm_config.json"
DEFAULT_CHECKPOINT_PATH = "nlm_checkpoint.bin"

class NLMCliError(Exception):
    """Base exception for NLM CLI errors."""
    pass

def create_brain_command(args):
    """Create and initialize a brain."""
    print("Creating brain...")
    
    # Create configuration
    if args.config_file:
        config = Config()
        config.load_from_file(args.config_file)
    else:
        config = create_default_config()
        # Apply command line overrides
        if args.neuron_count:
            config.set("brain.neuron_count", args.neuron_count)
        if args.synapse_density:
            config.set("brain.synapse_density", args.synapse_density)
        if args.region_count:
            config.set("brain.region_count", args.region_count)
    
    # Create and initialize brain
    brain = create_brain(config)
    brain.initialize()
    
    print(f"Brain created successfully!")
    print(f"  Total neurons: {brain.get_total_neuron_count()}")
    print(f"  Total synapses: {brain.get_total_synapse_count()}")
    print(f"  Developmental stage: {brain.get_developmental_stage()}")
    
    # Save config if requested
    if args.save_config:
        config.save_to_file(args.save_config)
        print(f"Configuration saved to: {args.save_config}")
    
    return brain

def simulate_command(args):
    """Run brain simulation."""
    print(f"Running simulation for {args.steps} steps...")
    
    # Create brain
    config = create_default_config()
    if args.config_file:
        config = Config()
        config.load_from_file(args.config_file)
    
    brain = create_brain(config)
    brain.initialize()
    
    # Run simulation
    start_time = time.time()
    
    for step in range(args.steps):
        brain.step(step)
        
        if args.verbose and step % args.verbose == 0:
            print(f"Step {step}: {brain.get_firing_neuron_count()} firing neurons")
        
        if args.max_steps and step >= args.max_steps:
            break
    
    end_time = time.time()
    
    # Display results
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print(f"Average firing rate: {brain.get_average_firing_rate():.3f}")
    print(f"Final developmental stage: {brain.get_developmental_stage()}")
    
    # Save checkpoint if requested
    if args.checkpoint:
        brain.save(args.checkpoint)
        print(f"Checkpoint saved to: {args.checkpoint}")
    
    return brain

def agent_sim_command(args):
    """Run agent simulation."""
    print(f"Running agent simulation for {args.steps} steps...")
    
    # Create brain
    config = create_default_config()
    if args.config_file:
        config = Config()
        config.load_from_file(args.config_file)
    
    brain = create_brain(config)
    brain.initialize()
    
    # Create and configure world
    world = create_simple_world()
    world.configure(
        width=args.world_width,
        height=args.world_height,
        vision_width=args.vision_width,
        vision_height=args.vision_height
    )
    world.reset()
    world.set_agent_start(args.agent_x, args.agent_y)
    
    # Create agent
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable subsystems if requested
    if args.enable_reward_modulation:
        agent.enable_reward_modulation(True)
    if args.enable_structural_plasticity:
        agent.enable_structural_plasticity(True)
    if args.enable_development:
        agent.enable_development(True)
    if args.enable_curiosity:
        agent.enable_curiosity(True)
    
    # Run simulation
    start_time = time.time()
    
    total_reward = 0.0
    
    for step in range(args.steps):
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        world.apply_motor_command(agent.decode_motor_command(), world.get_simulation_time())
        
        # Apply reward modulation if enabled
        if args.enable_reward_modulation:
            # Calculate reward based on world state
            percept = world.get_sensory_percept()
            reward = 0.0
            if percept.get_internal():
                reward = percept.get_internal()[0]
            agent.apply_reward_modulation(reward, 0.0)
        
        if args.enable_development:
            agent.update_development(0.1)
        
        total_reward += reward if args.enable_reward_modulation else 0.0
        
        if args.verbose and step % args.verbose == 0:
            print(f"Step {step}: "
                  f"Action: {agent.decode_motor_command()}, "
                  f"Reward: {reward:.3f}, "
                  f"Curiosity: {agent.get_curiosity_level():.3f}")
    
    end_time = time.time()
    
    # Display results
    print(f"\nAgent simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total reward: {total_reward}")
    print(f"Average reward: {total_reward / args.steps:.3f}")
    print(f"Firing neurons: {brain.get_firing_neuron_count()}")
    print(f"Firing rate: {brain.get_average_firing_rate():.3f}")
    print(f"Final developmental stage: {brain.get_developmental_stage()}")
    print(f"Final curiosity level: {agent.get_curiosity_level():.3f}")
    print(f"Final novelty level: {agent.get_novelty_level():.3f}")
    
    return brain, world, agent

def config_command(args):
    """Configuration management."""
    if args.show:
        # Show configuration
        config = create_default_config()
        if args.file:
            config = Config()
            config.load_from_file(args.file)
        
        print("Current configuration:")
        print(config.summary())
        
    elif args.set:
        # Set configuration value
        config = create_default_config()
        if args.file:
            config = Config()
            config.load_from_file(args.file)
        
        # Parse key-value pair
        key, value = args.set.split('=', 1)
        key = key.strip()
        value = value.strip()
        
        # Try to convert to appropriate type
        try:
            if '.' in value:
                value = float(value)
            else:
                value = int(value)
        except ValueError:
            pass  # Keep as string
        
        config.set(key, value)
        print(f"Set {key} = {value}")
        
        # Save if requested
        if args.file:
            config.save_to_file(args.file)
            print(f"Configuration saved to: {args.file}")
        
    elif args.get:
        # Get configuration value
        config = create_default_config()
        if args.file:
            config = Config()
            config.load_from_file(args.file)
        
        try:
            if args.get_float:
                value = config.get_float(args.get)
                print(f"{args.get} = {value} (float)")
            elif args.get_int:
                value = config.get_int(args.get)
                print(f"{args.get} = {value} (int)")
            else:
                value = config.get(args.get)
                print(f"{args.get} = {value}")
        except KeyError:
            print(f"Error: Configuration key '{args.get}' not found")
        
    elif args.keys:
        # List configuration keys
        config = create_default_config()
        if args.file:
            config = Config()
            config.load_from_file(args.file)
        
        print("Configuration keys:")
        for key in config.get_keys():
            print(f"  {key}")
    
    elif args.clear:
        # Clear configuration
        config = create_default_config()
        config.clear()
        print("Configuration cleared")

def save_load_command(args):
    """Save and load brain states."""
    if args.save:
        # Save brain
        print(f"Saving brain to: {args.save}")
        
        # Create brain (simplified - in reality you'd load existing brain)
        config = create_default_config()
        brain = create_brain(config)
        brain.initialize()
        
        # Run a quick simulation to generate state
        for step in range(100):
            brain.step(step)
        
        brain.save(args.save)
        print("Brain saved successfully")
        
    elif args.load:
        # Load brain
        print(f"Loading brain from: {args.load}")
        
        if not os.path.exists(args.load):
            print(f"Error: File '{args.load}' not found")
            return
        
        config = create_default_config()
        brain = create_brain(config)
        
        if brain.load(args.load):
            print("Brain loaded successfully")
        else:
            print("Error: Failed to load brain")

def stats_command(args):
    """Display brain statistics."""
    if args.brain_file:
        # Load and display brain stats
        print(f"Loading brain from: {args.brain_file}")
        
        config = create_default_config()
        brain = create_brain(config)
        
        if brain.load(args.brain_file):
            print("\nBrain Statistics:")
            print(brain.get_stats())
        else:
            print("Error: Failed to load brain")
    else:
        # Create and display brain stats
        brain = simulate_command(vars(args))
        
        if brain:
            print("\nBrain Statistics:")
            print(brain.get_stats())

def experiment_command(args):
    """Run experiments."""
    print(f"Running experiment: {args.name}")
    
    # Run simulation with experiment settings
    config = create_default_config()
    
    # Apply experiment-specific settings
    if args.neuron_count:
        config.set("brain.neuron_count", args.neuron_count)
    if args.steps:
        steps = args.steps
    else:
        steps = 1000
    
    brain = create_brain(config)
    brain.initialize()
    
    # Run experiment
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 100 == 0:
            print(f"Step {step}/{steps}: "
                  f"Firing: {brain.get_firing_neuron_count()}, "
                  f"Rate: {brain.get_average_firing_rate():.3f}")
    
    end_time = time.time()
    
    print(f"\nExperiment '{args.name}' completed in {end_time - start_time:.2f} seconds")
    print(f"Final stats: {brain.get_stats()}")
    
    # Save results if requested
    if args.results_file:
        results = {
            'name': args.name,
            'steps': steps,
            'duration': end_time - start_time,
            'total_spikes': brain.get_total_spike_count(),
            'avg_firing_rate': brain.get_average_firing_rate(),
            'final_stage': str(brain.get_developmental_stage()),
            'config': config.get_keys()
        }
        
        with open(args.results_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Results saved to: {args.results_file}")

def benchmark_command(args):
    """Performance benchmarking."""
    print("Running performance benchmark...")
    
    import timeit
    
    # Benchmark different operations
    config = create_default_config()
    
    # Time brain creation
    def create_and_init():
        brain = create_brain(config)
        brain.initialize()
        return brain
    
    creation_time = timeit.timeit(create_and_init, number=args.runs)
    print(f"Brain creation + initialization: {creation_time/args.runs*1000:.2f} ms average")
    
    # Time simulation step
    brain = create_and_init()
    
    def simulate_step():
        for step in range(10):
            brain.step(step)
    
    step_time = timeit.timeit(simulate_step, number=args.runs)
    print(f"10 simulation steps: {step_time/args.runs*1000:.2f} ms average")
    
    # Time statistics
    def get_stats():
        return brain.get_stats()
    
    stats_time = timeit.timeit(get_stats, number=args.runs)
    print(f"Get statistics: {stats_time/args.runs*1000:.2f} ms average")

def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Global options
    parser.add_argument(
        "-v", "--verbose", action="store_true",
        help="Enable verbose output"
    )
    parser.add_argument(
        "-h", "--help", action="store_true",
        help="Show help information"
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create brain command
    parser_create = subparsers.add_parser("create-brain", help="Create and initialize a brain")
    parser_create.add_argument("--config-file", help="Load configuration from file")
    parser_create.add_argument("--neuron-count", type=int, help="Set neuron count")
    parser_create.add_argument("--synapse-density", type=float, help="Set synapse density")
    parser_create.add_argument("--region-count", type=int, help="Set region count")
    parser_create.add_argument("--save-config", help="Save configuration to file")
    parser_create.set_defaults(func=create_brain_command)
    
    # Simulate command
    parser_sim = subparsers.add_parser("simulate", help="Run brain simulation")
    parser_sim.add_argument("steps", type=int, help="Number of simulation steps")
    parser_sim.add_argument("--config-file", help="Load configuration from file")
    parser_sim.add_argument("--checkpoint", help="Save checkpoint to file")
    parser_sim.add_argument("--max-steps", type=int, help="Maximum steps to simulate")
    parser_sim.add_argument("--verbose", type=int, default=100, help="Verbose interval")
    parser_sim.set_defaults(func=simulate_command)
    
    # Agent simulation command
    parser_agent = subparsers.add_parser("agent-sim", help="Run agent simulation")
    parser_agent.add_argument("steps", type=int, help="Number of simulation steps")
    parser_agent.add_argument("--config-file", help="Load configuration from file")
    parser_agent.add_argument("--world-width", type=int, default=20, help="World width")
    parser_agent.add_argument("--world-height", type=int, default=20, help="World height")
    parser_agent.add_argument("--vision-width", type=int, default=8, help="Vision width")
    parser_agent.add_argument("--vision-height", type=int, default=8, help="Vision height")
    parser_agent.add_argument("--agent-x", type=float, default=10.0, help="Agent starting X")
    parser_agent.add_argument("--agent-y", type=float, default=10.0, help="Agent starting Y")
    parser_agent.add_argument("--enable-reward-modulation", action="store_true", help="Enable reward modulation")
    parser_agent.add_argument("--enable-structural-plasticity", action="store_true", help="Enable structural plasticity")
    parser_agent.add_argument("--enable-development", action="store_true", help="Enable development")
    parser_agent.add_argument("--enable-curiosity", action="store_true", help="Enable curiosity")
    parser_agent.add_argument("--verbose", type=int, default=100, help="Verbose interval")
    parser_agent.set_defaults(func=agent_sim_command)
    
    # Configuration command
    parser_config = subparsers.add_parser("config", help="Configuration management")
    parser_config_group = parser_config.add_mutually_exclusive_group(required=True)
    parser_config_group.add_argument("--show", action="store_true", help="Show current configuration")
    parser_config_group.add_argument("--set", help="Set configuration value (key=value)")
    parser_config_group.add_argument("--get", help="Get configuration value")
    parser_config_group.add_argument("--keys", action="store_true", help="List configuration keys")
    parser_config_group.add_argument("--clear", action="store_true", help="Clear configuration")
    parser_config.add_argument("--file", help="Configuration file path")
    parser_config.add_argument("--get-float", action="store_true", help="Get as float")
    parser_config.add_argument("--get-int", action="store_true", help="Get as integer")
    parser_config.set_defaults(func=config_command)
    
    # Save/load command
    parser_save = subparsers.add_parser("save-load", help="Save and load brain states")
    save_load_group = parser_save.add_mutually_exclusive_group(required=True)
    save_load_group.add_argument("--save", help="Save brain to file")
    save_load_group.add_argument("--load", help="Load brain from file")
    parser_save.set_defaults(func=save_load_command)
    
    # Stats command
    parser_stats = subparsers.add_parser("stats", help="Display brain statistics")
    parser_stats.add_argument("--brain-file", help="Load brain from file")
    parser_stats.add_argument("--steps", type=int, default=100, help="Number of steps to simulate")
    parser_stats.set_defaults(func=stats_command)
    
    # Experiment command
    parser_exp = subparsers.add_parser("experiment", help="Run experiments")
    parser_exp.add_argument("name", help="Experiment name")
    parser_exp.add_argument("--steps", type=int, help="Number of steps")
    parser_exp.add_argument("--neuron-count", type=int, help="Neuron count")
    parser_exp.add_argument("--results-file", help="Save results to file")
    parser_exp.set_defaults(func=experiment_command)
    
    # Benchmark command
    parser_bench = subparsers.add_parser("benchmark", help="Performance benchmarking")
    parser_bench.add_argument("--runs", type=int, default=5, help="Number of benchmark runs")
    parser_bench.set_defaults(func=benchmark_command)
    
    # Help command
    def help_command(args):
        if args.command:
            # Show help for specific command
            try:
                subparsers.parse_args([args.command, '--help'])
            except SystemExit:
                pass  # Help was displayed
        else:
            # Show general help
            parser.print_help()
    
    parser_help = subparsers.add_parser("help", help="Show help information")
    parser_help.add_argument("command", nargs='?', help="Command to get help for")
    parser_help.set_defaults(func=help_command)
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute command
    if hasattr(args, 'func'):
        try:
            args.func(args)
        except NLMCliError as e:
            print(f"Error: {e}")
            sys.exit(1)
        except Exception as e:
            print(f"Unexpected error: {e}")
            sys.exit(1)
    elif args.help:
        parser.print_help()
    else:
        parser.print_help()

if __name__ == "__main__":
    main()