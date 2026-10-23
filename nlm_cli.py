#!/usr/bin/env python3
"""
NLM Python Command Line Interface

A powerful command-line tool for NLM (Neural Learning Machine) simulations.
Provides a comprehensive interface for running simulations, managing experiments,
and exploring advanced NLM features.

Usage: python nlm_cli.py [command] [options]

Commands:
  create          Create different types of brains and configurations
  run             Run simulations with various settings
  experiment      Run complete experimental protocols
  manage          Manage saved brain states and configurations
  explore          Explore brain properties and statistics
  visualize       Generate visualizations and reports
  benchmark       Run performance benchmarks
  config          Configure global settings
  help            Show this help message
"""

import argparse
import json
import sys
import os
import time
from pathlib import Path
from typing import Dict, Any, Optional, List

try:
    import pynlm
    from pynlm_advanced import (
        create_advanced_brain,
        create_agent_world,
        create_development_experiment,
        create_memory_experiment,
        create_curiosity_experiment,
        create_prediction_experiment,
        run_simulation_with_logging,
        save_brain_state,
        load_or_create_brain
    )
except ImportError as e:
    print(f"Error importing NLM: {e}")
    print("Please ensure pynlm is properly installed")
    sys.exit(1)


class NLMCommandError(Exception):
    """Custom exception for NLM CLI errors."""
    pass


def print_error(message: str, exit_code: int = 1):
    """Print error message and exit."""
    print(f"ERROR: {message}", file=sys.stderr)
    sys.exit(exit_code)


def print_success(message: str):
    """Print success message."""
    print(f"SUCCESS: {message}")


def create_brain_command(args):
    """Create a brain with specified configuration."""
    try:
        if args.advanced:
            brain, config = create_advanced_brain(
                neuron_count=args.neurons,
                connection_probability=args.density,
                enable_learning=args.learning,
                enable_memory=args.memory,
                enable_development=args.development,
                enable_curiosity=args.curiosity,
                enable_novelty=args.novelty,
                enable_prediction=args.prediction,
                enable_planning=args.planning
            )
        elif args.agent:
            brain, agent, world = create_agent_world(
                neuron_count=args.neurons,
                world_width=args.world_width,
                world_height=args.world_height,
                vision_width=args.vision_width,
                vision_height=args.vision_height,
                enable_rl=args.rl,
                enable_curiosity=args.curiosity,
                enable_rewards=args.rewards
            )
        elif args.development:
            brain, config, stages = create_development_experiment(
                neuron_count=args.neurons,
                development_stages=args.stages,
                enable_stdp=args.stdp,
                enable_hebbian=args.hebbian,
                enable_structural=args.structural
            )
        elif args.memory:
            brain, config = create_memory_experiment(
                episodes=args.episodes,
                memory_capacity=args.capacity,
                replay_frequency=args.replay
            )
        elif args.curiosity:
            brain, config = create_curiosity_experiment(
                exploration_rate=args.exploration,
                novelty_sensitivity=args.novelty_sens,
                reward_scaling=args.reward_scale
            )
        elif args.prediction:
            brain, config = create_prediction_experiment(
                prediction_horizon=args.horizon,
                prediction_error_threshold=args.error_threshold
            )
        else:
            # Basic brain
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", args.neurons)
            config.set("brain.connection_probability", args.density)
            brain = pynlm.createBrain(config)
        
        brain.initialize()
        
        print_success(f"Created {args.brain_type} brain with {args.neurons} neurons")
        print(f"Brain stats: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
        
        if args.save:
            filepath = save_brain_state(brain, args.save)
            print_success(f"Brain saved to: {filepath}")
        
        return 0
        
    except Exception as e:
        return print_error(f"Failed to create brain: {e}")


def run_simulation_command(args):
    """Run a simulation with specified settings."""
    try:
        # Load or create brain
        if args.load:
            brain, _ = load_or_create_brain(args.load, args.neurons)
        else:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", args.neurons)
            config.set("brain.connection_probability", args.density)
            brain = pynlm.createBrain(config)
        
        # Create agent if requested
        agent = None
        world = None
        if args.agent:
            world = pynlm.createSimpleWorld()
            world.configure(args.world_width, args.world_height, 
                          args.vision_width, args.vision_height)
            world.reset()
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            if args.rl:
                agent.enableRewardModulation(True)
            if args.curiosity:
                agent.enableCuriosity(True)
        
        print_success(f"Running simulation: {args.steps} steps")
        start_time = time.time()
        
        if args.agent:
            # Run agent-world simulation
            for step in range(args.steps):
                world.update(args.world_timestep)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Log progress
                if step % args.log_interval == 0:
                    print(f"Step {step}: "
                          f"Firing: {brain.getFiringNeuronCount()}, "
                          f"Action: {action.getType()}, "
                          f"Curiosity: {agent.getCuriosityLevel():.3f}")
        else:
            # Run simple brain simulation
            brain.initialize()
            
            if args.logging:
                stats = run_simulation_with_logging(
                    brain, args.steps, args.log_interval,
                    enable_progress=not args.quiet,
                    enable_detailed_stats=args.detailed_stats
                )
            else:
                for step in range(args.steps):
                    brain.step(step)
                    
                    if step % args.log_interval == 0 and not args.quiet:
                        print(f"Step {step}: "
                              f"Firing: {brain.getFiringNeuronCount()}, "
                              f"Rate: {brain.getAverageFiringRate():.3f}")
        
        end_time = time.time()
        duration = end_time - start_time
        
        print_success(f"Simulation completed in {duration:.2f}s")
        print(f"Final stats:")
        print(f"  Total steps: {args.steps}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Active neurons: {brain.getActiveNeuronCount()}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        
        if args.save:
            filepath = save_brain_state(brain, args.save)
            print_success(f"Brain saved to: {filepath}")
        
        if args.export_stats:
            stats = {
                'simulation_params': {
                    'steps': args.steps,
                    'neurons': args.neurons,
                    'density': args.density,
                    'agent_mode': args.agent,
                    'world_size': args.world_width
                },
                'final_stats': {
                    'total_spikes': brain.getTotalSpikeCount(),
                    'avg_firing_rate': brain.getAverageFiringRate(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'active_neurons': brain.getActiveNeuronCount(),
                    'e_i_ratio': brain.getExcitationInhibitionRatio(),
                    'development_stage': int(brain.getDevelopmentalStage()),
                    'duration_seconds': duration
                },
                'timestamp': time.time()
            }
            
            with open(args.export_stats, 'w') as f:
                json.dump(stats, f, indent=2)
            print_success(f"Statistics exported to: {args.export_stats}")
        
        return 0
        
    except Exception as e:
        return print_error(f"Simulation failed: {e}")


def explore_brain_command(args):
    """Explore brain properties and statistics."""
    try:
        if args.load:
            brain, _ = load_or_create_brain(args.load, args.neurons)
        else:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", args.neurons)
            brain = pynlm.createBrain(config)
        
        brain.initialize()
        
        # Run exploration steps
        exploration_steps = args.steps if args.steps else 100
        for step in range(exploration_steps):
            brain.step(step)
        
        print("=== BRAIN EXPLORATION ===")
        print(f"Neuron count: {brain.getTotalNeuronCount()}")
        print(f"Synapse count: {brain.getTotalSynapseCount()}")
        print(f"Active neurons: {brain.getActiveNeuronCount()}")
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"Development stage: {brain.getDevelopmentalStage()}")
        
        # Regional analysis
        print("\n=== REGIONAL ANALYSIS ===")
        for region in brain.getRegions():
            print(f"Region {region.getId().index()} ({region.getName()}):")
            print(f"  Neurons: {region.getTotalNeuronCount()}")
            print(f"  Synapses: {region.getSynapseCount()}")
            print(f"  Avg synaptic weight: {region.getAverageSynapticWeight():.3f}")
        
        # Memory system status
        print("\n=== MEMORY SYSTEMS ===")
        if brain.getWorkingMemory():
            print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
        if brain.getEpisodicMemory():
            print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
        if brain.getAssociativeMemory():
            print(f"Associative memory patterns: {brain.getAssociativeMemory().getPatternCount()}")
        
        # Neuromodulation status
        print("\n=== NEUROMODULATION ===")
        if brain.getDopamine():
            print(f"Dopamine level: {brain.getDopamine().getLevel():.3f}")
        if brain.getCuriosity():
            print(f"Curiosity level: {brain.getCuriosity().getLevel():.3f}")
        if brain.getNovelty():
            print(f"Novelty level: {brain.getNovelty().getLevel():.3f}")
        
        # Prediction system
        print("\n=== PREDICTION SYSTEM ===")
        if brain.getPredictionSystem():
            print(f"Prediction system active")
        
        # Cognition systems
        print("\n=== COGNITION SYSTEMS ===")
        if brain.getPlanner():
            print(f"Planner: active")
        if brain.getConceptFormation():
            print(f"Concept formation: active")
        if brain.getAttention():
            print(f"Attention system: active")
        
        return 0
        
    except Exception as e:
        return print_error(f"Exploration failed: {e}")


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Python Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create brain and configurations')
    create_parser.add_argument('--advanced', action='store_true', help='Create advanced brain with all features')
    create_parser.add_argument('--agent', action='store_true', help='Create agent-world setup')
    create_parser.add_argument('--development', action='store_true', help='Create development experiment setup')
    create_parser.add_argument('--memory', action='store_true', help='Create memory experiment setup')
    create_parser.add_argument('--curiosity', action='store_true', help='Create curiosity experiment setup')
    create_parser.add_argument('--prediction', action='store_true', help='Create prediction experiment setup')
    create_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons (default: 1000)')
    create_parser.add_argument('--density', type=float, default=0.1, help='Connection density (default: 0.1)')
    create_parser.add_argument('--brain-type', default='basic', help='Brain type (basic, advanced, agent, etc.)')
    create_parser.add_argument('--save', help='Save brain state to file')
    create_parser.add_argument('--stages', type=int, default=5, help='Number of developmental stages')
    create_parser.add_argument('--episodes', type=int, default=100, help='Number of memory episodes')
    create_parser.add_argument('--capacity', type=int, default=1000, help='Memory capacity')
    create_parser.add_argument('--exploration', type=float, default=0.3, help='Exploration rate')
    create_parser.add_argument('--novelty-sens', type=float, default=0.5, help='Novelty sensitivity')
    create_parser.add_argument('--reward-scale', type=float, default=1.0, help='Reward scaling factor')
    create_parser.add_argument('--horizon', type=int, default=10, help='Prediction horizon')
    create_parser.add_argument('--error-threshold', type=float, default=0.1, help='Prediction error threshold')
    create_parser.add_argument('--world-width', type=int, default=20, help='World width')
    create_parser.add_argument('--world-height', type=int, default=20, help='World height')
    create_parser.add_argument('--vision-width', type=int, default=8, help='Vision width')
    create_parser.add_argument('--vision-height', type=int, default=8, help='Vision height')
    create_parser.add_argument('--rl', action='store_true', help='Enable reward learning')
    create_parser.add_argument('--rewards', action='store_true', help='Enable world rewards')
    create_parser.add_argument('--learning', action='store_true', help='Enable learning systems')
    create_parser.add_argument('--memory', action='store_true', help='Enable memory systems')
    create_parser.add_argument('--development', action='store_true', help='Enable development systems')
    create_parser.add_argument('--curiosity', action='store_true', help='Enable curiosity systems')
    create_parser.add_argument('--novelty', action='store_true', help='Enable novelty detection')
    create_parser.add_argument('--prediction', action='store_true', help='Enable prediction systems')
    create_parser.add_argument('--planning', action='store_true', help='Enable planning systems')
    create_parser.add_argument('--stdp', action='store_true', help='Enable STDP')
    create_parser.add_argument('--hebbian', action='store_true', help='Enable Hebbian learning')
    create_parser.add_argument('--structural', action='store_true', help='Enable structural plasticity')
    create_parser.add_argument('--replay', type=int, default=10, help='Replay frequency')
    create_parser.set_defaults(func=create_brain_command)
    
    # Run command
    run_parser = subparsers.add_parser('run', help='Run simulations')
    run_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps (default: 100)')
    run_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons (default: 1000)')
    run_parser.add_argument('--density', type=float, default=0.1, help='Connection density (default: 0.1)')
    run_parser.add_argument('--agent', action='store_true', help='Run agent-world simulation')
    run_parser.add_argument('--world-width', type=int, default=20, help='World width (default: 20)')
    run_parser.add_argument('--world-height', type=int, default=20, help='World height (default: 20)')
    run_parser.add_argument('--vision-width', type=int, default=8, help='Vision width (default: 8)')
    run_parser.add_argument('--vision-height', type=int, default=8, help='Vision height (default: 8)')
    run_parser.add_argument('--world-timestep', type=float, default=0.1, help='World timestep (default: 0.1)')
    run_parser.add_argument('--rl', action='store_true', help='Enable reward learning')
    run_parser.add_argument('--curiosity', action='store_true', help='Enable curiosity')
    run_parser.add_argument('--logging', action='store_true', help='Enable detailed logging')
    run_parser.add_argument('--detailed-stats', action='store_true', help='Collect detailed statistics')
    run_parser.add_argument('--quiet', action='store_true', help='Suppress output')
    run_parser.add_argument('--load', help='Load brain state from file')
    run_parser.add_argument('--save', help='Save brain state to file')
    run_parser.add_argument('--log-interval', type=int, default=50, help='Logging interval (default: 50)')
    run_parser.add_argument('--export-stats', help='Export statistics to JSON file')
    run_parser.set_defaults(func=run_simulation_command)
    
    # Explore command
    explore_parser = subparsers.add_parser('explore', help='Explore brain properties')
    explore_parser.add_argument('--steps', type=int, help='Number of exploration steps')
    explore_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons (default: 1000)')
    explore_parser.add_argument('--load', help='Load brain state from file')
    explore_parser.set_defaults(func=explore_brain_command)
    
    # Manage command
    manage_parser = subparsers.add_parser('manage', help='Manage saved brain states')
    manage_parser_subparsers = manage_parser.add_subparsers(dest='action', help='Management actions')
    
    save_parser = manage_parser_subparsers.add_parser('save', help='Save brain state')
    save_parser.add_argument('filename', help='Filename to save')
    save_parser.set_defaults(func=lambda args: print_error("Save command not implemented yet"))
    
    load_parser = manage_parser_subparsers.add_parser('load', help='Load brain state')
    load_parser.add_argument('filename', help='Filename to load')
    load_parser.set_defaults(func=lambda args: print_error("Load command not implemented yet"))
    
    list_parser = manage_parser_subparsers.add_parser('list', help='List saved brain states')
    list_parser.set_defaults(func=lambda args: print_error("List command not implemented yet"))
    
    # Experiment command (placeholder for advanced experiments)
    experiment_parser = subparsers.add_parser('experiment', help='Run experimental protocols')
    experiment_parser.set_defaults(func=lambda args: print_error("Experiment command not implemented yet"))
    
    # Visualize command (placeholder for visualization)
    visualize_parser = subparsers.add_parser('visualize', help='Generate visualizations')
    visualize_parser.set_defaults(func=lambda args: print_error("Visualize command not implemented yet"))
    
    # Benchmark command (placeholder for performance benchmarks)
    benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmarks')
    benchmark_parser.set_defaults(func=lambda args: print_error("Benchmark command not implemented yet"))
    
    # Config command (placeholder for configuration management)
    config_parser = subparsers.add_parser('config', help='Configure global settings')
    config_parser.set_defaults(func=lambda args: print_error("Config command not implemented yet"))
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    # Execute command
    return args.func(args)


if __name__ == '__main__':
    sys.exit(main())
