#!/usr/bin/env python3
"""
NLM Command Line Interface

A comprehensive command-line tool for running NLM simulations,
experiments, and analysis.
"""

import argparse
import json
import sys
import time
import os
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm


def create_parser():
    """Create the argument parser for the CLI."""
    parser = argparse.ArgumentParser(
        description="NLM - Neural Learning Machine Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm basic --neurons 1000 --steps 5000
  nlm learning --episodes 20 --curiosity
  nlm multi-agent --agents 5 --world-size 40
  nlm benchmark --duration 60
  nlm analyze --file brain_state.json --metrics
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Basic simulation command
    basic_parser = subparsers.add_parser(
        'basic', 
        help='Run a basic brain simulation'
    )
    basic_parser.add_argument('--neurons', type=int, default=500,
                             help='Number of neurons (default: 500)')
    basic_parser.add_argument('--steps', type=int, default=1000,
                             help='Number of simulation steps (default: 1000)')
    basic_parser.add_argument('--config', type=str, default=None,
                             help='Configuration file path')
    basic_parser.add_argument('--output', type=str, default=None,
                             help='Output file for brain state')
    basic_parser.add_argument('--verbose', action='store_true',
                             help='Enable verbose output')
    
    # Learning experiment command
    learning_parser = subparsers.add_parser(
        'learning',
        help='Run a learning experiment'
    )
    learning_parser.add_argument('--episodes', type=int, default=10,
                                help='Number of learning episodes (default: 10)')
    learning_parser.add_argument('--world-width', type=int, default=25,
                                help='World width (default: 25)')
    learning_parser.add_argument('--world-height', type=int, default=25,
                                help='World height (default: 25)')
    learning_parser.add_argument('--curiosity', action='store_true',
                                help='Enable curiosity-driven exploration')
    learning_parser.add_argument('--reward-modulation', action='store_true',
                                help='Enable reward modulation (default: True)')
    learning_parser.add_argument('--structural-plasticity', action='store_true',
                                help='Enable structural plasticity (default: True)')
    learning_parser.add_argument('--development', action='store_true',
                                help='Enable development (default: True)')
    learning_parser.add_argument('--save-brain', type=str, default=None,
                                help='Save best brain to file')
    learning_parser.add_argument('--load-brain', type=str, default=None,
                                help='Load initial brain from file')
    
    # Multi-agent command
    multi_agent_parser = subparsers.add_parser(
        'multi-agent',
        help='Run multi-agent simulation'
    )
    multi_agent_parser.add_argument('--agents', type=int, default=3,
                                   help='Number of agents (default: 3)')
    multi_agent_parser.add_argument('--world-size', type=int, default=30,
                                   help='World size (default: 30)')
    multi_agent_parser.add_argument('--steps', type=int, default=200,
                                   help='Number of simulation steps (default: 200)')
    multi_agent_parser.add_argument('--cooperation', action='store_true',
                                   help='Enable cooperative behavior')
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser(
        'benchmark',
        help='Run performance benchmark'
    )
    benchmark_parser.add_argument('--duration', type=int, default=30,
                                 help='Benchmark duration in seconds (default: 30)')
    benchmark_parser.add_argument('--neurons', type=int, default=1000,
                                 help='Number of neurons (default: 1000)')
    benchmark_parser.add_argument('--steps', type=int, default=5000,
                                 help='Number of simulation steps (default: 5000)')
    benchmark_parser.add_argument('--output', type=str, default='benchmark_results.json',
                                 help='Output file for benchmark results')
    
    # Analysis command
    analyze_parser = subparsers.add_parser(
        'analyze',
        help='Analyze brain state or simulation results'
    )
    analyze_parser.add_argument('--file', type=str, required=True,
                               help='Input file to analyze')
    analyze_parser.add_argument('--metrics', action='store_true',
                               help='Calculate performance metrics')
    analyze_parser.add_argument('--neural-activity', action='store_true',
                               help='Analyze neural activity patterns')
    analyze_parser.add_argument('--plasticity', action='store_true',
                               help='Analyze plasticity changes')
    analyze_parser.add_argument('--output', type=str, default=None,
                               help='Output file for analysis results')
    
    # Evolution command
    evolution_parser = subparsers.add_parser(
        'evolution',
        help='Run evolutionary algorithm with NLM brains'
    )
    evolution_parser.add_argument('--generations', type=int, default=5,
                                 help='Number of generations (default: 5)')
    evolution_parser.add_argument('--population', type=int, default=10,
                                 help='Population size (default: 10)')
    evolution_parser.add_argument('--neuron-count', type=int, default=300,
                                 help='Number of neurons per brain (default: 300)')
    evolution_parser.add_argument('--save-best', type=str, default=None,
                                 help='Save best genome to file')
    evolution_parser.add_argument('--load-best', type=str, default=None,
                                 help='Load best genome from file')
    
    return parser


def run_basic_simulation(args):
    """Run a basic brain simulation."""
    print(f"Running basic simulation with {args.neurons} neurons for {args.steps} steps")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", args.neurons)
    
    # Load custom config if specified
    if args.config:
        if config.loadFromFile(args.config):
            print(f"Loaded configuration from: {args.config}")
        else:
            print(f"Warning: Could not load configuration from {args.config}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    if args.verbose:
        print("Initial brain state:")
        stats = pynlm.getBrainStats(brain)
        for key, value in stats.items():
            print(f"  {key}: {value}")
    
    # Run simulation
    for step in range(args.steps):
        brain.step(step)
        
        if args.verbose and step % 100 == 0:
            print(f"Step {step}/{args.steps}")
    
    # Get final statistics
    final_stats = pynlm.getBrainStats(brain)
    
    print(f"\nSimulation completed!")
    print(f"Final statistics:")
    for key, value in final_stats.items():
        print(f"  {key}: {value}")
    
    # Save brain state if requested
    if args.output:
        brain.save(args.output)
        print(f"Brain state saved to: {args.output}")
    
    return final_stats


def run_learning_experiment(args):
    """Run a learning experiment."""
    print(f"Starting learning experiment with {args.episodes} episodes")
    print(f"World: {args.world_width}x{args.world_height}")
    
    total_reward = 0.0
    all_stats = []
    
    for episode in range(args.episodes):
        print(f"\n--- Episode {episode + 1}/{args.episodes} ---")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 800)
        config.set("connection_probability", 0.12)
        
        # Create brain
        brain = pynlm.createBrain(config)
        
        # Load saved brain if specified
        if args.load_brain and episode == 0:
            if brain.load(args.load_brain):
                print(f"Loaded brain from: {args.load_brain}")
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=args.world_width, height=args.world_height, 
                       visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(args.world_width//2, args.world_height//2)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features based on arguments
        agent.enableRewardModulation(args.reward_modulation if hasattr(args, 'reward_modulation') else True)
        agent.enableStructuralPlasticity(args.structural_plasticity if hasattr(args, 'structural_plasticity') else True)
        agent.enableDevelopment(args.development if hasattr(args, 'development') else True)
        
        if args.curiosity:
            agent.enableCuriosity(True)
        
        # Run episode
        episode_reward = 0.0
        for step in range(300):  # Fixed episode length for learning
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(action_result.reward, agent.getPredictionError())
            episode_reward += action_result.reward
        
        total_reward += episode_reward
        
        # Get episode statistics
        stats = pynlm.getBrainStats(brain)
        stats['episode'] = episode + 1
        stats['total_reward'] = episode_reward
        stats['learning_rate'] = agent.getNeuromodulationLevel()
        stats['curiosity'] = agent.getCuriosityLevel()
        stats['development_stage'] = agent.getDevelopmentalStage()
        
        all_stats.append(stats)
        
        print(f"Episode completed in 300 steps")
        print(f"Episode reward: {episode_reward:.2f}")
        print(f"Learning rate: {stats['learning_rate']:.3f}")
        print(f"Curiosity level: {stats['curiosity']:.3f}")
        print(f"Development stage: {stats['development_stage']}")
        
        # Save best brain if requested
        if args.save_brain and episode == 0:
            brain.save(args.save_brain)
            print(f"Saved initial brain to: {args.save_brain}")
    
    # Calculate overall statistics
    avg_reward = total_reward / args.episodes
    
    print(f"\n=== LEARNING EXPERIMENT RESULTS ===")
    print(f"Total episodes: {args.episodes}")
    print(f"Average reward per episode: {avg_reward:.2f}")
    print(f"Best episode reward: {max(s['total_reward'] for s in all_stats):.2f}")
    print(f"Average learning rate: {sum(s['learning_rate'] for s in all_stats) / args.episodes:.3f}")
    print(f"Average curiosity level: {sum(s['curiosity'] for s in all_stats) / args.episodes:.3f}")
    
    return all_stats


def run_multi_agent_simulation(args):
    """Run multi-agent simulation."""
    print(f"Starting multi-agent simulation with {args.agents} agents")
    print(f"World size: {args.world_size}x{args.world_size}")
    print(f"Steps: {args.steps}")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=args.world_size, height=args.world_size,
                   visionWidth=6, visionHeight=6)
    world.reset()
    
    # Create agents
    agents = []
    brains = []
    
    for i in range(args.agents):
        # Create configuration with slight variations
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 400)
        config.set("region_count", 2)
        config.set("connection_probability", 0.08 + (i * 0.005))  # Varying connectivity
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableAllFeatures()
        
        # Position agents randomly
        start_x = (i * args.world_size // (args.agents + 1))
        start_y = args.world_size // 2
        world.setAgentStart(start_x, start_y)
        
        agents.append(agent)
        brains.append(brain)
    
    print("Agents initialized and positioned")
    
    # Run simulation
    for step in range(args.steps):
        world.update(0.1)
        
        # Each agent processes its own percept
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brains[i].step(step)
            action = agent.decodeMotorCommand()
            
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learn from interaction
            agent.applyRewardModulation(0.01, agent.getPredictionError())
        
        if args.verbose and step % 50 == 0:
            print(f"Step {step}/{args.steps}")
    
    # Print final statistics
    print(f"\n=== MULTI-AGENT SIMULATION RESULTS ===")
    total_spikes = sum(brain.getTotalSpikeCount() for brain in brains)
    total_neurons = sum(brain.getTotalNeuronCount() for brain in brains)
    
    print(f"Total agents: {args.agents}")
    print(f"Total neurons: {total_neurons}")
    print(f"Total spikes: {total_spikes}")
    print(f"Average firing rate: {sum(brain.getAverageFiringRate() for brain in brains) / args.agents:.3f}")
    
    return brains


def run_benchmark(args):
    """Run performance benchmark."""
    print(f"Starting benchmark for {args.duration} seconds")
    print(f"Configuration: {args.neurons} neurons, {args.steps} steps")
    
    results = {
        'duration': args.duration,
        'neurons': args.neurons,
        'steps': args.steps,
        'start_time': time.time(),
        'end_time': None,
        'total_steps': 0,
        'steps_per_second': 0,
        'average_step_time': 0,
    }
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", args.neurons)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    end_time = start_time + args.duration
    
    print(f"Benchmark started at {time.ctime(start_time)}")
    print(f"Benchmark will end at {time.ctime(end_time)}")
    
    step_count = 0
    while time.time() < end_time:
        brain.step(step_count)
        step_count += 1
        
        if step_count % 1000 == 0:
            elapsed = time.time() - start_time
            print(f"Progress: {step_count} steps, {elapsed:.1f}s elapsed")
    
    end_time = time.time()
    results['end_time'] = end_time
    results['total_steps'] = step_count
    results['steps_per_second'] = step_count / (end_time - start_time)
    results['average_step_time'] = (end_time - start_time) / step_count if step_count > 0 else 0
    
    print(f"\n=== BENCHMARK RESULTS ===")
    print(f"Total steps completed: {step_count}")
    print(f"Total time: {end_time - start_time:.2f} seconds")
    print(f"Steps per second: {results['steps_per_second']:.1f}")
    print(f"Average step time: {results['average_step_time'] * 1000:.3f} ms")
    print(f"Final brain stats: {pynlm.getBrainStats(brain)}")
    
    # Save results
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to: {args.output}")
    
    return results


def run_analysis(args):
    """Analyze brain state or simulation results."""
    print(f"Analyzing file: {args.file}")
    
    # Create brain for analysis
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Load brain state
    if brain.load(args.file):
        print(f"Successfully loaded brain from: {args.file}")
    else:
        print(f"Error: Could not load brain from {args.file}")
        return None
    
    results = {
        'file': args.file,
        'load_time': time.time(),
    }
    
    if args.metrics:
        print("\n=== PERFORMANCE METRICS ===")
        stats = pynlm.getBrainStats(brain)
        for key, value in stats.items():
            print(f"  {key}: {value}")
        results['metrics'] = stats
    
    if args.neural_activity:
        print("\n=== NEURAL ACTIVITY ANALYSIS ===")
        activity_stats = {
            'total_spikes': brain.getTotalSpikeCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        }
        for key, value in activity_stats.items():
            print(f"  {key}: {value}")
        results['neural_activity'] = activity_stats
    
    if args.plasticity:
        print("\n=== PLASTICITY ANALYSIS ===")
        # Note: Plasticity analysis would require access to synapse information
        # This is a placeholder for more detailed analysis
        results['plasticity'] = {
            'note': 'Detailed plasticity analysis requires additional data structures',
            'synapse_count': brain.getTotalSynapseCount(),
        }
    
    # Save results
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Analysis results saved to: {args.output}")
    
    return results


def run_evolution(args):
    """Run evolutionary algorithm with NLM brains."""
    print(f"Starting evolutionary algorithm: {args.generations} generations")
    print(f"Population size: {args.population}")
    
    generation_stats = []
    
    for generation in range(args.generations):
        print(f"\n=== Generation {generation + 1}/{args.generations} ===")
        
        # Create population
        population = []
        for i in range(args.population):
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", args.neuron_count)
            config.set("connection_probability", 0.08 + (i * 0.002))
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Add genetic variation
            if i % 3 == 0:
                # Higher development
                agent = pynlm.createAgentBrain(brain)
                agent.enableDevelopment(True)
                agent.enableCuriosity(True)
            
            population.append(brain)
        
        # Evaluate each brain
        generation_scores = []
        for i, brain in enumerate(population):
            # Create world and agent for evaluation
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableAllFeatures()
            
            # Run evaluation episode
            score = 0
            for step in range(100):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Simple score: based on actions taken and learning
                score += agent.getNeuromodulationLevel() * 10
                
                # Add curiosity bonus for exploration
                if agent.getCuriosityLevel() > 0.5:
                    score += 50
            
            generation_scores.append(score)
            print(f"  Agent {i}: Score = {score}")
        
        # Create next generation (simple reproduction)
        avg_score = sum(generation_scores) / len(generation_scores)
        print(f"  Average score: {avg_score:.1f}")
        
        generation_stats.append({
            'generation': generation + 1,
            'avg_score': avg_score,
            'best_score': max(generation_scores),
            'scores': generation_scores,
        })
        
        # Save best brain for potential later use
        best_idx = generation_scores.index(max(generation_scores))
        population[best_idx].save(f"best_brain_gen{generation + 1}.json")
        print(f"Saved best brain to: best_brain_gen{generation + 1}.json")
    
    return generation_stats


def main():
    """Main CLI entry point."""
    parser = create_parser()
    
    try:
        args = parser.parse_args()
        
        if not args.command:
            parser.print_help()
            return 1
        
        # Set verbose attribute if not present (for multi-agent command)
        if not hasattr(args, 'verbose'):
            args.verbose = False
        
        # Run the requested command
        if args.command == 'basic':
            return run_basic_simulation(args)
        elif args.command == 'learning':
            return run_learning_experiment(args)
        elif args.command == 'multi-agent':
            return run_multi_agent_simulation(args)
        elif args.command == 'benchmark':
            return run_benchmark(args)
        elif args.command == 'analyze':
            return run_analysis(args)
        elif args.command == 'evolution':
            return run_evolution(args)
        else:
            print(f"Unknown command: {args.command}")
            return 1
            
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == "__main__":
    exit(main())
