#!/usr/bin/env python3
"""NLM Command Line Interface

This module provides a command-line interface for the NLM framework.
"""

import argparse
import logging
import sys
import json
from typing import Optional

from . import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
from .helpers import (
    createStandardConfig, runCompleteSimulation, benchmarkPerformance,
    runEpisode, validateConfiguration, saveBrainState, loadBrainState,
    collectBrainStatistics, collectAgentStatistics
)

logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)
def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Framework",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s create --config neurons=1000 learning_rate=0.01
  %(prog)s simulate --steps 1000
  %(prog)s benchmark --brain mybrain.nml
  %(prog)s episode --max-steps 200
  %(prog)s validate --brain mybrain.nml
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Create configuration command
    create_parser = subparsers.add_parser('create', help='Create NLM components')
    create_subparsers = create_parser.add_subparsers(dest='create_command', help='Create operation')
    
    config_parser = create_subparsers.add_parser('config', help='Create configuration')
    config_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    config_parser.add_argument('--learning-rate', type=float, default=0.01, help='Learning rate')
    config_parser.add_argument('--synapse-density', type=float, default=0.1, help='Synapse density')
    config_parser.add_argument('--time-step', type=float, default=0.1, help='Time step')
    config_parser.add_argument('--world-width', type=float, default=100.0, help='World width')
    config_parser.add_argument('--world-height', type=float, default=100.0, help='World height')
    config_parser.add_argument('--save', help='Save configuration to file')
    
    brain_parser = create_subparsers.add_parser('brain', help='Create brain')
    brain_parser.add_argument('--config-file', help='Configuration file')
    brain_parser.add_argument('--save', help='Save brain to file')
    
    world_parser = create_subparsers.add_parser('world', help='Create world')
    world_parser.add_argument('--width', type=int, default=100, help='World width')
    world_parser.add_argument('--height', type=int, default=100, help='World height')
    world_parser.add_argument('--vision-width', type=int, default=84, help='Vision width')
    world_parser.add_argument('--vision-height', type=int, default=84, help='Vision height')
    world_parser.add_argument('--save', help='Save world to file')
    
    agent_parser = create_subparsers.add_parser('agent', help='Create agent')
    agent_parser.add_argument('--brain-file', help='Brain file')
    
    # Simulate command
    simulate_parser = subparsers.add_parser('simulate', help='Run simulation')
    simulate_parser.add_argument('--steps', type=int, default=1000, help='Number of steps')
    simulate_parser.add_argument('--config-file', help='Configuration file')
    simulate_parser.add_argument('--world-width', type=int, default=100, help='World width')
    simulate_parser.add_argument('--world-height', type=int, default=100, help='World height')
    simulate_parser.add_argument('--output-stats', help='Save statistics to file')
    
    # Episode command
    episode_parser = subparsers.add_parser('episode', help='Run episode')
    episode_parser.add_argument('--max-steps', type=int, default=100, help='Maximum steps')
    episode_parser.add_argument('--brain-file', help='Brain file')
    episode_parser.add_argument('--world-width', type=int, default=100, help='World width')
    episode_parser.add_argument('--world-height', type=int, default=100, help='World height')
    episode_parser.add_argument('--output-stats', help='Save statistics to file')
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Benchmark performance')
    benchmark_parser.add_argument('--brain-file', help='Brain file')
    benchmark_parser.add_argument('--steps', type=int, default=1000, help='Number of steps')
    benchmark_parser.add_argument('--warmup-steps', type=int, default=100, help='Warmup steps')
    benchmark_parser.add_argument('--output', help='Save benchmark results to file')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate configuration')
    validate_parser.add_argument('--brain-file', help='Brain file')
    validate_parser.add_argument('--config-file', help='Configuration file')
    
    # Save/Load commands
    save_parser = subparsers.add_parser('save', help='Save brain state')
    save_parser.add_argument('--brain', help='Brain file')
    save_parser.add_argument('--output', help='Output file')
    
    load_parser = subparsers.add_parser('load', help='Load brain state')
    load_parser.add_argument('--input', help='Input file')
    load_parser.add_argument('--save', help='Save configuration')
    
    # Stats command
    stats_parser = subparsers.add_parser('stats', help='Show brain statistics')
    stats_parser.add_argument('--brain-file', help='Brain file')
    
    # Execute command
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
        
    try:
        if args.command == 'create':
            handle_create_command(args)
        elif args.command == 'simulate':
            handle_simulate_command(args)
        elif args.command == 'episode':
            handle_episode_command(args)
        elif args.command == 'benchmark':
            handle_benchmark_command(args)
        elif args.command == 'validate':
            handle_validate_command(args)
        elif args.command == 'save':
            handle_save_command(args)
        elif args.command == 'load':
            handle_load_command(args)
        elif args.command == 'stats':
            handle_stats_command(args)
        else:
            print(f"Unknown command: {args.command}")
            return 1
            
    except Exception as e:
        logger.error(f"Command failed: {e}")
        return 1
        
    return 0
def handle_create_command(args):
    """Handle create command."""
    if args.create_command == 'config':
        config = createStandardConfig(
            neuron_count=args.neurons,
            learning_rate=args.learning_rate,
            synapse_density=args.synapse_density,
            time_step=args.time_step
        )
        
        print(f"Created configuration: {config}")
        
        if args.save:
            config.save_to_file(args.save)
            print(f"Configuration saved to: {args.save}")
            
    elif args.create_command == 'brain':
        if args.config_file:
            config = createStandardConfig()
            config.load_from_file(args.config_file)
        else:
            config = createDefaultConfig()
            
        brain = createBrain(config)
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        if args.save:
            brain.save(args.save)
            print(f"Brain saved to: {args.save}")
            
    elif args.create_command == 'world':
        world = createSimpleWorld()
        world.configure(args.width, args.height, args.vision_width, args.vision_height)
        
        print(f"Created world: {args.width}x{args.height}, vision: {args.vision_width}x{args.vision_height}")
        
        if args.save:
            # Note: SimpleWorld doesn't have a save method, so this is for future use
            print(f"World configuration can be saved to: {args.save}")
            
    elif args.create_command == 'agent':
        if args.brain_file:
            print(f"Loading agent from brain file: {args.brain_file}")
            # Note: This would need implementation for loading brain from file
        else:
            config = createDefaultConfig()
            brain = createBrain(config)
            agent = createAgentBrain(brain)
            print("Created agent brain with default configuration")
def handle_simulate_command(args):
    """Handle simulate command."""
    if args.config_file:
        config = createStandardConfig()
        config.load_from_file(args.config_file)
    else:
        config = createDefaultConfig()
        
    brain = createBrain(config)
    world = createSimpleWorld()
    world.configure(args.world_width, args.world_height, 84, 84)
    
    agent = createAgentBrain(brain)
    
    results = runCompleteSimulation(brain, world, agent, steps=args.steps)
    
    print(f"Simulation completed: {results['stats']['steps_completed']} steps")
    print(f"Final spike count: {results['stats']['total_spikes']}")
    print(f"Simulation time: {results['stats']['simulation_time']:.2f}s")
    
    if args.output_stats:
        with open(args.output_stats, 'w') as f:
            json.dump(results['stats'], f, indent=2)
        print(f"Statistics saved to: {args.output_stats}")
def handle_episode_command(args):
    """Handle episode command."""
    if args.brain_file:
        print(f"Loading episode from brain file: {args.brain_file}")
        # Note: This would need implementation for loading brain from file
    else:
        config = createDefaultConfig()
        brain = createBrain(config)
        world = createSimpleWorld()
        world.configure(args.world_width, args.world_height, 84, 84)
        agent = createAgentBrain(brain)
        
    results = runEpisode(agent, world, max_steps=args.max_steps)
    
    print(f"Episode completed: {results['steps_completed']} steps")
    print(f"Total spikes: {results['total_spikes']}")
    print(f"Duration: {results['simulation_time']:.2f}s")
    
    if args.output_stats:
        with open(args.output_stats, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Statistics saved to: {args.output_stats}")
def handle_benchmark_command(args):
    """Handle benchmark command."""
    if args.brain_file:
        print(f"Loading benchmark from brain file: {args.brain_file}")
        # Note: This would need implementation for loading brain from file
    else:
        config = createDefaultConfig()
        brain = createBrain(config)
        
    results = benchmarkPerformance(brain, steps=args.steps, warmup_steps=args.warmup_steps)
    
    total_time, steps_per_second, neurons_per_second = results
    
    print(f"Benchmark completed:")
    print(f"  Total time: {total_time:.2f}s")
    print(f"  Steps per second: {steps_per_second:.2f}")
    print(f"  Neurons processed per second: {neurons_per_second:.2f}")
    
    if args.output:
        with open(args.output, 'w') as f:
            json.dump({
                'total_time': total_time,
                'steps_per_second': steps_per_second,
                'neurons_per_second': neurons_per_second,
                'steps': args.steps,
                'warmup_steps': args.warmup_steps
            }, f, indent=2)
        print(f"Benchmark results saved to: {args.output}")
def handle_validate_command(args):
    """Handle validate command."""
    if args.brain_file and args.config_file:
        print(f"Error: Please provide either --brain-file or --config-file, not both")
        return
        
    if args.brain_file:
        print(f"Validating brain file: {args.brain_file}")
        # Note: This would need implementation for validating brain from file
    else:
        config = createDefaultConfig()
        config.load_from_file(args.config_file)
        brain = createBrain(config)
        world = createSimpleWorld()
        agent = createAgentBrain(brain)
        
    is_valid = validateConfiguration(brain, world, agent)
    
    if is_valid:
        print("Configuration is valid")
    else:
        print("Configuration is invalid")
        return 1
def handle_save_command(args):
    """Handle save command."""
    if not args.brain:
        print("Error: Please provide --brain file")
        return 1
        
    print(f"Saving brain state: {args.brain}")
    # Note: This would need implementation for saving brain
    print("Brain save functionality requires additional implementation")
    
def handle_load_command(args):
    """Handle load command."""
    if not args.input:
        print("Error: Please provide --input file")
        return 1
        
    print(f"Loading brain state: {args.input}")
    # Note: This would need implementation for loading brain
    print("Brain load functionality requires additional implementation")
    
def handle_stats_command(args):
    """Handle stats command."""
    if not args.brain_file:
        print("Error: Please provide --brain-file")
        return 1
        
    print(f"Getting statistics for brain file: {args.brain_file}")
    # Note: This would need implementation for getting brain statistics
    print("Statistics functionality requires additional implementation")
if __name__ == '__main__':
    sys.exit(main())