#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

This module provides a command-line interface for common NLM operations,
making it easy to run simulations, analyze results, and manage brain states
from the terminal.
"""

import pynlm
import json
import time
import argparse
import sys
import os
from typing import Optional
from pathlib import Path


def create_parser():
    """Create the main argument parser."""
    parser = argparse.ArgumentParser(
        description="NLM - Neural Learning Machine Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run a simple brain simulation
  pynlm-cli simple --steps 100

  # Run a complete agent simulation with learning
  pynlm-cli agent --steps 500 --enable-learning

  # Run performance benchmark
  pynlm-cli benchmark --steps 1000

  # Save brain state
  pynlm-cli save --filepath my_brain.pkl

  # Load brain state and continue
  pynlm-cli load --filepath my_brain.pkl --steps 200

  # Analyze configuration
  pynlm-cli analyze --config config.json

  # Get configuration presets
  pynlm-cli presets --show learning_agent
        """
    )

    subparsers = parser.add_subparsers(dest='command', help='Command to execute')

    # Simple simulation command
    simple_parser = subparsers.add_parser('simple', help='Run simple brain simulation')
    simple_parser.add_argument('--steps', type=int, default=100,
                              help='Number of simulation steps (default: 100)')
    simple_parser.add_argument('--neurons', type=int, default=1000,
                              help='Number of neurons (default: 1000)')
    simple_parser.add_argument('--synapses', type=float, default=0.1,
                              help='Synapse density (default: 0.1)')
    simple_parser.add_argument('--output', '-o', help='Output file for results')

    # Agent simulation command
    agent_parser = subparsers.add_parser('agent', help='Run agent simulation with world')
    agent_parser.add_argument('--steps', type=int, default=500,
                             help='Number of simulation steps (default: 500)')
    agent_parser.add_argument('--world-width', type=int, default=20,
                             help='World width (default: 20)')
    agent_parser.add_argument('--world-height', type=int, default=20,
                             help='World height (default: 20)')
    agent_parser.add_argument('--world-vision-width', type=int, default=8,
                             help='World vision width (default: 8)')
    agent_parser.add_argument('--world-vision-height', type=int, default=8,
                             help='World vision height (default: 8)')
    agent_parser.add_argument('--enable-learning', action='store_true',
                             help='Enable learning features')
    agent_parser.add_argument('--enable-curiosity', action='store_true',
                             help='Enable curiosity-driven exploration')
    agent_parser.add_argument('--enable-development', action='store_true',
                             help='Enable brain development')
    agent_parser.add_argument('--reward', type=float, default=0.1,
                             help='Reward value per step (default: 0.1)')
    agent_parser.add_argument('--output', '-o', help='Output file for results')
    agent_parser.add_argument('--no-save-stats', action='store_true',
                             help='Do not save statistics to file')

    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmark')
    benchmark_parser.add_argument('--steps', type=int, default=1000,
                                 help='Number of simulation steps (default: 1000)')
    benchmark_parser.add_argument('--output', '-o', help='Output file for benchmark results')

    # Save brain state command
    save_parser = subparsers.add_parser('save', help='Save brain state to file')
    save_parser.add_argument('--filepath', '-f', required=True,
                            help='Filepath to save brain state')
    save_parser.add_argument('--brain', help='Brain object reference (internal use)')

    # Load brain state command
    load_parser = subparsers.add_parser('load', help='Load brain state from file')
    load_parser.add_argument('--filepath', '-f', required=True,
                            help='Filepath to load brain state from')
    load_parser.add_argument('--steps', type=int, default=100,
                            help='Number of steps to run after loading (default: 100)')
    load_parser.add_argument('--output', '-o', help='Output file for results')

    # Analyze configuration command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze configuration or brain')
    analyze_parser.add_argument('--config', help='Configuration file path')
    analyze_parser.add_argument('--brain', help='Brain file path')
    analyze_parser.add_argument('--format', choices=['json', 'text', 'yaml'],
                               default='text', help='Output format (default: text)')
    analyze_parser.add_argument('--output', '-o', help='Output file for analysis')

    # Configuration presets command
    presets_parser = subparsers.add_parser('presets', help='Manage configuration presets')
    presets_subparsers = presets_parser.add_subparsers(dest='preset_action',
                                                     help='Preset action')

    show_parser = presets_subparsers.add_parser('show', help='Show preset configuration')
    show_parser.add_argument('preset_name', help='Name of preset (default, learning_agent, etc.)')

    list_parser = presets_subparsers.add_parser('list', help='List available presets')

    apply_parser = presets_subparsers.add_parser('apply', help='Apply preset configuration')
    apply_parser.add_argument('preset_name', help='Name of preset to apply')
    apply_parser.add_argument('--output-file', '-o', help='Output file for applied config')

    # Statistics command
    stats_parser = subparsers.add_parser('stats', help='Get brain or agent statistics')
    stats_parser.add_argument('--brain', help='Brain file path')
    stats_parser.add_argument('--agent', action='store_true', help='Show agent statistics')
    stats_parser.add_argument('--format', choices=['json', 'text', 'yaml'],
                             default='text', help='Output format (default: text)')
    stats_parser.add_argument('--output', '-o', help='Output file for statistics')

    # Version command
    version_parser = subparsers.add_parser('version', help='Show version information')

    # Help command
    help_parser = subparsers.add_parser('help', help='Show help information')

    return parser


def run_simple_simulation(args):
    """Run a simple brain simulation."""
    print("Running simple brain simulation...")
    
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", str(args.neurons))
        config.set("brain.synapse_density", str(args.synapses))
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        print(f"Running {args.steps} steps with {args.neurons} neurons...")
        start_time = time.time()
        
        for step in range(args.steps):
            brain.step(step)
            
            # Show progress every 10%
            if step % (args.steps // 10) == 0:
                progress = (step / args.steps) * 100
                print(f"  Progress: {progress:.1f}% - "
                      f"Firing neurons: {brain.getFiringNeuronCount()}, "
                      f"Total spikes: {brain.getTotalSpikeCount()}")
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Get statistics
        stats = brain.get_statistics()
        stats['total_time_seconds'] = duration
        stats['steps_per_second'] = args.steps / duration
        
        print(f"\nSimulation completed!")
        print(f"  Total steps: {args.steps}")
        print(f"  Duration: {duration:.2f} seconds")
        print(f"  Speed: {stats['steps_per_second']:.1f} steps/second")
        print(f"  Final firing neurons: {stats['firing_neurons']}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Average firing rate: {stats['average_firing_rate']:.3f}")
        
        # Save results if requested
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(stats, f, indent=2)
            print(f"Results saved to {args.output}")
        
        return True
        
    except Exception as e:
        print(f"Error running simple simulation: {e}")
        return False


def run_agent_simulation(args):
    """Run a complete agent simulation with world."""
    print("Running agent simulation with world...")
    
    try:
        # Create configuration
        config = pynlm.create_standard_config()
        
        # Create brain and initialize
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(
            width=args.world_width,
            height=args.world_height,
            visionWidth=args.world_vision_width,
            visionHeight=args.world_vision_height
        )
        world.reset()
        world.setAgentStart(args.world_width // 2, args.world_height // 2)
        
        # Initialize agent
        agent.initialize(world)
        
        # Enable features based on arguments
        if args.enable_learning:
            agent.enableRewardModulation(True)
        
        if args.enable_curiosity:
            agent.enableCuriosity(True)
        
        if args.enable_development:
            agent.enableDevelopment(True)
        
        print(f"Starting simulation: {args.steps} steps")
        print(f"World size: {args.world_width}x{args.world_height}")
        print(f"Vision: {args.world_vision_width}x{args.world_vision_height}")
        
        # Run simulation
        start_time = time.time()
        
        for step in range(args.steps):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            if args.enable_learning:
                agent.applyRewardModulation(args.reward, 0.0)
            
            # Update development
            if args.enable_development:
                agent.updateDevelopment(0.1)
            
            # Show progress every 10%
            if step % (args.steps // 10) == 0 and step > 0:
                progress = (step / args.steps) * 100
                print(f"  Progress: {progress:.1f}% - "
                      f"Firing neurons: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}")
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Get comprehensive statistics
        brain_stats = brain.get_statistics()
        agent_status = agent.get_agent_status()
        world_state = world.get_world_state()
        
        # Combine all statistics
        all_stats = {
            'simulation_info': {
                'total_steps': args.steps,
                'duration_seconds': duration,
                'steps_per_second': args.steps / duration,
            },
            'brain_statistics': brain_stats,
            'agent_status': agent_status,
            'world_state': world_state,
        }
        
        print(f"\nAgent simulation completed!")
        print(f"  Steps: {args.steps}")
        print(f"  Duration: {duration:.2f} seconds")
        print(f"  Final firing neurons: {brain_stats['firing_neurons']}")
        print(f"  Final curiosity level: {agent_status['curiosity_level']:.3f}")
        print(f"  Final novelty level: {agent_status['novelty_level']:.3f}")
        print(f"  Development stage: {agent_status['developmental_stage']}")
        
        # Save results if requested
        if args.output and not args.no_save_stats:
            with open(args.output, 'w') as f:
                json.dump(all_stats, f, indent=2)
            print(f"Results saved to {args.output}")
        
        # Print final summary
        print(f"\n=== Final Summary ===")
        print(f"  Total neurons created: {brain_stats['total_neurons']}")
        print(f"  Total synapses created: {brain_stats['total_synapses']}")
        print(f"  Average firing rate: {brain_stats['average_firing_rate']:.3f}")
        print(f"  Agent curiosity: {agent_status['curiosity_level']:.3f}")
        print(f"  Agent novelty: {agent_status['novelty_level']:.3f}")
        
        return True
        
    except Exception as e:
        print(f"Error running agent simulation: {e}")
        return False


def run_benchmark(args):
    """Run performance benchmark."""
    print("Running performance benchmark...")
    
    try:
        # Create and initialize brain
        config = pynlm.create_standard_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Running {args.steps} steps for benchmark...")
        
        # Run benchmark
        start_time = time.time()
        
        for step in range(args.steps):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Calculate statistics
        results = brain.benchmark_performance(args.steps)
        results['wall_clock_duration'] = duration
        results['wall_clock_steps_per_second'] = args.steps / duration
        
        print(f"\nBenchmark completed!")
        print(f"  Steps: {args.steps}")
        print(f"  CPU time: {results['total_time_ms']:.1f} ms")
        print(f"  Wall time: {duration:.2f} seconds")
        print(f"  CPU speed: {results['steps_per_second']:.1f} steps/second")
        print(f"  Wall speed: {results['wall_clock_steps_per_second']:.1f} steps/second")
        
        # Save results if requested
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Results saved to {args.output}")
        
        return True
        
    except Exception as e:
        print(f"Error running benchmark: {e}")
        return False


def save_brain_state(args):
    """Save brain state to file."""
    print(f"Saving brain state to {args.filepath}...")
    
    try:
        # Create and initialize a test brain
        config = pynlm.create_standard_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run a few steps to have some state
        for step in range(10):
            brain.step(step)
        
        # Save brain state
        brain.save(args.filepath)
        
        print(f"Brain state saved successfully to {args.filepath}")
        
        # Show file size
        file_size = os.path.getsize(args.filepath)
        print(f"File size: {file_size} bytes")
        
        return True
        
    except Exception as e:
        print(f"Error saving brain state: {e}")
        return False


def load_brain_state(args):
    """Load brain state from file."""
    print(f"Loading brain state from {args.filepath}...")
    
    try:
        # Create brain
        config = pynlm.create_standard_config()
        brain = pynlm.createBrain(config)
        
        # Load brain state
        brain.load(args.filepath)
        
        print("Brain state loaded successfully!")
        
        # Run additional steps if requested
        if args.steps > 0:
            print(f"Running {args.steps} steps with loaded brain...")
            
            for step in range(args.steps):
                brain.step(step)
                
                if (step + 1) % (args.steps // 10) == 0 and step > 0:
                    progress = ((step + 1) / args.steps) * 100
                    print(f"  Progress: {progress:.1f}% - "
                          f"Firing neurons: {brain.getFiringNeuronCount()}")
        
        # Get final statistics
        stats = brain.get_statistics()
        print(f"\nLoaded brain simulation completed!")
        print(f"  Final firing neurons: {stats['firing_neurons']}")
        print(f"  Total spikes: {stats['total_spikes']}")
        
        # Save results if requested
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(stats, f, indent=2)
            print(f"Results saved to {args.output}")
        
        return True
        
    except Exception as e:
        print(f"Error loading brain state: {e}")
        return False


def analyze_config_or_brain(args):
    """Analyze configuration or brain file."""
    print("Analyzing configuration or brain...")
    
    try:
        if args.config:
            # Analyze configuration file
            config = pynlm.createDefaultConfig()
            config.loadFromFile(args.config)
            config.summary()
            
            if args.output:
                with open(args.output, 'w') as f:
                    if args.format == 'json':
                        f.write(json.dumps(dict(config), indent=2))
                    else:
                        f.write(str(config))
                print(f"Analysis saved to {args.output}")
        
        elif args.brain:
            # Analyze brain file
            config = pynlm.create_standard_config()
            brain = pynlm.createBrain(config)
            brain.load(args.brain)
            
            stats = brain.get_statistics()
            print(f"Brain analysis completed:")
            print(f"  Total neurons: {stats['total_neurons']}")
            print(f"  Total synapses: {stats['total_synapses']}")
            print(f"  Firing neurons: {stats['firing_neurons']}")
            print(f"  Average firing rate: {stats['average_firing_rate']:.3f}")
            print(f"  Development stage: {stats['developmental_stage']}")
            
            if args.output:
                with open(args.output, 'w') as f:
                    if args.format == 'json':
                        json.dump(stats, f, indent=2)
                    else:
                        f.write(str(stats))
                print(f"Analysis saved to {args.output}")
        
        else:
            print("Please specify either --config or --brain for analysis")
            return False
        
        return True
        
    except Exception as e:
        print(f"Error analyzing configuration or brain: {e}")
        return False


def show_presets(args):
    """Show preset configuration."""
    print("Showing configuration preset...")
    
    try:
        from python import ConfigPresets
        
        presets = ConfigPresets.get_all_presets()
        
        if args.preset_name in presets:
            preset = presets[args.preset_name]
            
            print(f"Preset: {args.preset_name}")
            preset.summary()
            
            if args.output_file:
                with open(args.output_file, 'w') as f:
                    json.dump(dict(preset), f, indent=2)
                print(f"Preset saved to {args.output_file}")
        
        else:
            print(f"Preset '{args.preset_name}' not found")
            print("Available presets:")
            for name in presets.keys():
                print(f"  - {name}")
            return False
        
        return True
        
    except Exception as e:
        print(f"Error showing preset: {e}")
        return False


def list_presets(args):
    """List available configuration presets."""
    print("Available configuration presets:")
    
    try:
        from python import ConfigPresets
        
        presets = ConfigPresets.get_all_presets()
        
        for name in sorted(presets.keys()):
            print(f"\n{name}:")
            
            # Show key properties
            preset = presets[name]
            neuron_count = preset.get("brain.neuron_count", "default")
            synapse_density = preset.get("brain.synapse_density", "default")
            
            print(f"  Neurons: {neuron_count}")
            print(f"  Synapse density: {synapse_density}")
            
            # Show enabled features
            features = []
            if preset.get("plasticity.stdp.enable") == "true":
                features.append("STDP")
            if preset.get("plasticity.hebbian.enable") == "true":
                features.append("Hebbian")
            if preset.get("neuromod.curiosity.enable") == "true":
                features.append("Curiosity")
            if preset.get("neuromod.novelty.enable") == "true":
                features.append("Novelty")
            
            if features:
                print(f"  Features: {', '.join(features)}")
        
        return True
        
    except Exception as e:
        print(f"Error listing presets: {e}")
        return False


def apply_preset(args):
    """Apply preset configuration."""
    print(f"Applying preset: {args.preset_name}")
    
    try:
        from python import ConfigPresets
        
        presets = ConfigPresets.get_all_presets()
        
        if args.preset_name in presets:
            preset = presets[args.preset_name]
            
            # Save preset
            if args.output_file:
                with open(args.output_file, 'w') as f:
                    json.dump(dict(preset), f, indent=2)
                print(f"Preset applied and saved to {args.output_file}")
            else:
                print("Preset applied (not saved to file)")
                preset.summary()
            
            return True
        
        else:
            print(f"Preset '{args.preset_name}' not found")
            return False
        
    except Exception as e:
        print(f"Error applying preset: {e}")
        return False


def get_stats(args):
    """Get brain or agent statistics."""
    print("Getting statistics...")
    
    try:
        if args.brain:
            # Load brain and get statistics
            config = pynlm.create_standard_config()
            brain = pynlm.createBrain(config)
            brain.load(args.brain)
            
            stats = brain.get_statistics()
            
            print(f"Brain statistics:")
            for key, value in stats.items():
                print(f"  {key}: {value}")
            
        elif args.agent:
            # Create agent and get statistics
            config = pynlm.create_standard_config()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent_stats = agent.get_agent_status()
            
            print(f"Agent statistics:")
            for key, value in agent_stats.items():
                print(f"  {key}: {value}")
        
        else:
            print("Please specify either --brain or --agent for statistics")
            return False
        
        return True
        
    except Exception as e:
        print(f"Error getting statistics: {e}")
        return False


def show_version(args):
    """Show version information."""
    print("NLM Command Line Interface")
    print(f"Version: 0.1.0")
    print(f"Author: NLM Authors")
    print(f"Description: Enhanced Python utilities for NLM neural simulations")
    print(f"Python bindings for: NLM (Neural Learning Machine) C++ framework")
    
    return True


def main():
    """Main CLI entry point."""
    parser = create_parser()
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    # Dispatch to appropriate handler based on command
    commands = {
        'simple': run_simple_simulation,
        'agent': run_agent_simulation,
        'benchmark': run_benchmark,
        'save': save_brain_state,
        'load': load_brain_state,
        'analyze': analyze_config_or_brain,
        'version': show_version,
        'help': lambda args: parser.print_help() or True,
    }
    
    # Handle presets subcommands
    if args.command == 'presets':
        if args.preset_action == 'show':
            return 0 if show_presets(args) else 1
        elif args.preset_action == 'list':
            return 0 if list_presets(args) else 1
        else:
            print("Please specify a preset action (show, list)")
            return 1
    
    # Execute the appropriate command
    if args.command in commands:
        return 0 if commands[args.command](args) else 1
    else:
        print(f"Unknown command: {args.command}")
        return 1


if __name__ == '__main__':
    sys.exit(main())