#!/usr/bin/env python3
"""
NLM Advanced Command-Line Tools
Advanced command-line interface for NLM with enhanced functionality
"""

import sys
import argparse
import json
import time
import os

# Add the NLM module to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pynlm

def create_advanced_brain(args):
    """Create an advanced brain with custom configuration"""
    print("Creating advanced brain...")
    
    config = pynlm.createDefaultConfig()
    
    # Basic configuration
    config.set("neuron_count", args.neuron_count)
    config.set("region_count", args.region_count)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", args.random_seed)
    
    # Advanced connectivity patterns
    config.set("connection_probability", args.connectivity)
    config.set("excitatory_proportion", args.excitatory_proportion)
    
    # Neuromodulation settings
    config.set("reward_modulation_enabled", True)
    config.set("curiosity_enabled", args.curiosity)
    config.set("structural_plasticity_rate", args.plasticity_rate)
    
    # Development settings
    config.set("development_enabled", args.development)
    config.set("developmental_stage", "adult" if args.adult else "critical_period")
    
    # Prediction system
    config.set("prediction_horizon", 5)
    config.set("prediction_confidence_threshold", 0.8)
    
    # Save configuration
    config_path = f"{args.output_dir}/advanced_config_{int(time.time())}.json"
    config.saveToFile(config_path)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Advanced brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Configuration saved to: {config_path}")
    
    return brain

def run_advanced_simulation(args):
    """Run an advanced simulation with monitoring"""
    print("Running advanced simulation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=args.world_width, height=args.world_height, 
                    visionWidth=args.vision_width, visionHeight=args.vision_height)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(args.development)
    agent.enableCuriosity(args.curiosity)
    
    # Run simulation with advanced monitoring
    episode_rewards = []
    episode_steps = []
    working_memory_traces = []
    
    for episode in range(args.episodes):
        world.reset()
        agent.reset()
        agent.initialize(world)
        
        episode_reward = 0.0
        episode_step_count = 0
        
        for step in range(args.steps_per_episode):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward based on action
            if action.getType() == pynlm.ActionType.Interact:
                reward = 2.0
            elif action.getType() == pynlm.ActionType.Look:
                reward = 1.0
            else:
                reward = 0.1
            
            episode_reward += reward
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            episode_step_count += 1
        
        # Collect metrics
        episode_rewards.append(episode_reward / episode_step_count)
        episode_steps.append(episode_step_count)
        working_memory_traces.append(brain.getWorkingMemoryTraces())
        
        if episode % args.monitor_interval == 0:
            print(f"Episode {episode}: "
                  f"Avg Reward={episode_rewards[-1]:.3f}, "
                  f"Steps={episode_steps[-1]}, "
                  f"WorkingMem={working_memory_traces[-1]}")
    
    # Calculate statistics
    avg_reward = sum(episode_rewards) / len(episode_rewards)
    avg_steps = sum(episode_steps) / len(episode_steps)
    avg_working_memory = sum(working_memory_traces) / len(working_memory_traces)
    
    print(f"\nSimulation Results:")
    print(f"Average reward per episode: {avg_reward:.3f}")
    print(f"Average steps per episode: {avg_steps:.1f}")
    print(f"Average working memory traces: {avg_working_memory:.1f}")
    print(f"Total episodes: {args.episodes}")
    
    # Save results
    results = {
        'avg_reward': avg_reward,
        'avg_steps': avg_steps,
        'avg_working_memory': avg_working_memory,
        'episode_rewards': episode_rewards,
        'episode_steps': episode_steps,
        'working_memory_traces': working_memory_traces,
        'total_episodes': args.episodes
    }
    
    results_path = f"{args.output_dir}/simulation_results_{int(time.time())}.json"
    with open(results_path, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"Results saved to: {results_path}")
    
    return results

def analyze_brain_state(args):
    """Analyze and visualize brain state"""
    print("Analyzing brain state...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Load brain state if provided
    if args.load_checkpoint:
        if brain.load(args.load_checkpoint):
            print(f"Brain state loaded from: {args.load_checkpoint}")
        else:
            print(f"Failed to load checkpoint: {args.load_checkpoint}")
    
    # Analyze brain statistics
    print("\n=== Brain State Analysis ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spike count: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Excitatory/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Get advanced metrics
    print("\n=== Advanced Metrics ===")
    if brain.getWorkingMemory():
        print(f"Working memory capacity: {brain.getWorkingMemory().getCapacity()}")
        print(f"Working memory active traces: {brain.getWorkingMemory().getActiveTraces()}")
    
    if brain.getEpisodicMemory():
        print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    # Check integrated systems
    print("\n=== Integrated Systems Status ===")
    print(f"Working memory integrated: {brain.getWorkingMemory() != nullptr}")
    print(f"Episodic memory integrated: {brain.getEpisodicMemory() != nullptr}")
    print(f"Neuromodulation integrated: {brain.getDopamine() != nullptr}")
    print(f"Prediction system integrated: {brain.getPredictionSystem() != nullptr}")
    print(f"Development system integrated: {brain.getDevelopmentSystem() != nullptr}")
    
    # Save analysis
    analysis = {
        'brain_statistics': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio()
        },
        'integrated_systems': {
            'working_memory': brain.getWorkingMemory() != nullptr,
            'episodic_memory': brain.getEpisodicMemory() != nullptr,
            'neuromodulation': brain.getDopamine() != nullptr,
            'prediction_system': brain.getPredictionSystem() != nullptr,
            'development_system': brain.getDevelopmentSystem() != nullptr
        }
    }
    
    analysis_path = f"{args.output_dir}/brain_analysis_{int(time.time())}.json"
    with open(analysis_path, 'w') as f:
        json.dump(analysis, f, indent=2)
    
    print(f"\nAnalysis saved to: {analysis_path}")
    
    return analysis

def create_experimental_setup(args):
    """Create an experimental setup with specific parameters"""
    print("Creating experimental setup...")
    
    config = pynlm.createDefaultConfig()
    
    # Experimental parameters
    config.set("neuron_count", args.neuron_count)
    config.set("region_count", args.region_count)
    config.set("connection_probability", args.connection_probability)
    
    # Development stages
    config.set("development_enabled", True)
    config.set("developmental_stage", args.development_stage)
    
    # Memory systems
    config.set("working_memory_capacity", args.working_memory_capacity)
    config.set("episodic_memory_max_episodes", args.episodic_memory_max_episodes)
    
    # Learning parameters
    config.set("plasticity_rate", args.plasticity_rate)
    config.set("reward_modulation_strength", args.reward_modulation_strength)
    config.set("curiosity_threshold", args.curiosity_threshold)
    
    # Prediction system
    config.set("prediction_horizon", args.prediction_horizon)
    config.set("prediction_confidence_threshold", args.prediction_confidence_threshold)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Experimental brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Development stage: {args.development_stage}")
    print(f"Working memory capacity: {args.working_memory_capacity}")
    print(f"Episodic memory max episodes: {args.episodic_memory_max_episodes}")
    
    # Save experiment configuration
    exp_config = {
        'neuron_count': args.neuron_count,
        'region_count': args.region_count,
        'connection_probability': args.connection_probability,
        'development_stage': args.development_stage,
        'working_memory_capacity': args.working_memory_capacity,
        'episodic_memory_max_episodes': args.episodic_memory_max_episodes,
        'plasticity_rate': args.plasticity_rate,
        'reward_modulation_strength': args.reward_modulation_strength,
        'curiosity_threshold': args.curiosity_threshold,
        'prediction_horizon': args.prediction_horizon,
        'prediction_confidence_threshold': args.prediction_confidence_threshold
    }
    
    config_path = f"{args.output_dir}/experimental_config_{int(time.time())}.json"
    with open(config_path, 'w') as f:
        json.dump(exp_config, f, indent=2)
    
    print(f"Experimental configuration saved to: {config_path}")
    
    return brain

def main():
    """Main command-line interface"""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Command-Line Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python nlm_cli.py create --neuron_count 1000 --region_count 3
  python nlm_cli.py simulate --episodes 50 --steps_per_episode 200
  python nlm_cli.py analyze --world_width 40 --world_height 40
  python nlm_cli.py experiment --neuron_count 500 --development_stage critical_period
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create an advanced brain')
    create_parser.add_argument('--neuron_count', type=int, default=1000, help='Number of neurons')
    create_parser.add_argument('--region_count', type=int, default=3, help='Number of regions')
    create_parser.add_argument('--connectivity', type=float, default=0.15, help='Connection probability')
    create_parser.add_argument('--excitatory_proportion', type=float, default=0.9, help='Excitatory neuron proportion')
    create_parser.add_argument('--plasticity_rate', type=float, default=0.0001, help='Structural plasticity rate')
    create_parser.add_argument('--curiosity', action='store_true', help='Enable curiosity')
    create_parser.add_argument('--development', action='store_true', help='Enable development')
    create_parser.add_argument('--adult', action='store_true', help='Use adult development stage instead of critical period')
    create_parser.add_argument('--random_seed', type=int, default=12345, help='Random seed for reproducibility')
    create_parser.add_argument('--output_dir', type=str, default='./nlm_output', help='Output directory')
    
    # Simulate command
    simulate_parser = subparsers.add_parser('simulate', help='Run an advanced simulation')
    simulate_parser.add_argument('--episodes', type=int, default=20, help='Number of episodes')
    simulate_parser.add_argument('--steps_per_episode', type=int, default=100, help='Steps per episode')
    simulate_parser.add_argument('--world_width', type=int, default=30, help='World width')
    simulate_parser.add_argument('--world_height', type=int, default=30, help='World height')
    simulate_parser.add_argument('--vision_width', type=int, default=16, help='Vision width')
    simulate_parser.add_argument('--vision_height', type=int, default=16, help='Vision height')
    simulate_parser.add_argument('--curiosity', action='store_true', help='Enable curiosity')
    simulate_parser.add_argument('--development', action='store_true', help='Enable development')
    simulate_parser.add_argument('--monitor_interval', type=int, default=5, help='Monitoring interval')
    simulate_parser.add_argument('--output_dir', type=str, default='./nlm_output', help='Output directory')
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze brain state')
    analyze_parser.add_argument('--world_width', type=int, default=20, help='World width')
    analyze_parser.add_argument('--world_height', type=int, default=20, help='World height')
    analyze_parser.add_argument('--load_checkpoint', type=str, help='Load brain from checkpoint')
    analyze_parser.add_argument('--output_dir', type=str, default='./nlm_output', help='Output directory')
    
    # Experiment command
    experiment_parser = subparsers.add_parser('experiment', help='Create experimental setup')
    experiment_parser.add_argument('--neuron_count', type=int, default=1000, help='Number of neurons')
    experiment_parser.add_argument('--region_count', type=int, default=3, help='Number of regions')
    experiment_parser.add_argument('--connection_probability', type=float, default=0.15, help='Connection probability')
    experiment_parser.add_argument('--development_stage', type=str, choices=['initial', 'critical_period', 'maturation', 'adult', 'aging'], default='critical_period', help='Development stage')
    experiment_parser.add_argument('--working_memory_capacity', type=int, default=100, help='Working memory capacity')
    experiment_parser.add_argument('--episodic_memory_max_episodes', type=int, default=1000, help='Maximum episodic memory episodes')
    experiment_parser.add_argument('--plasticity_rate', type=float, default=0.0001, help='Plasticity rate')
    experiment_parser.add_argument('--reward_modulation_strength', type=float, default=1.0, help='Reward modulation strength')
    experiment_parser.add_argument('--curiosity_threshold', type=float, default=0.5, help='Curiosity threshold')
    experiment_parser.add_argument('--prediction_horizon', type=int, default=5, help='Prediction horizon')
    experiment_parser.add_argument('--prediction_confidence_threshold', type=float, default=0.8, help='Prediction confidence threshold')
    experiment_parser.add_argument('--output_dir', type=str, default='./nlm_output', help='Output directory')
    
    args = parser.parse_args()
    
    # Create output directory
    os.makedirs(args.output_dir, exist_ok=True)
    
    # Execute command
    if args.command == 'create':
        create_advanced_brain(args)
    elif args.command == 'simulate':
        run_advanced_simulation(args)
    elif args.command == 'analyze':
        analyze_brain_state(args)
    elif args.command == 'experiment':
        create_experimental_setup(args)
    else:
        parser.print_help()

if __name__ == '__main__':
    main()