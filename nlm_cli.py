#!/usr/bin/env python3
"""
NLM Command Line Interface
Comprehensive command-line interface for NLM (Neural Learning Machine) simulation
"""

import argparse
import sys
import json
import os
from pathlib import Path
from typing import Dict, Any, List, Optional

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Error: pynlm module not found. Please install NLM Python bindings.")
    print("Install with: pip install .")
    sys.exit(1)

# Import the enhanced performance monitor
from performance import BenchmarkResult, PerformanceMonitor

class NLMCLI:
    """Command line interface for NLM simulation"""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def run_simulation(self, args):
        """Run a complete simulation episode"""
        print(f"Running NLM simulation with {args.neurons} neurons for {args.steps} steps...")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", args.neurons)
        config.set("region_count", 1)
        config.set("connection_probability", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems
        if args.enable_reward_modulation:
            agent.enableRewardModulation(True)
        if args.enable_curiosity:
            agent.enableCuriosity(True)
        if args.enable_development:
            agent.enableDevelopment(True)
        if args.enable_structural_plasticity:
            agent.enableStructuralPlasticity(True)
        
        # Run simulation
        for step in range(args.steps):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain
            brain.step(step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward if enabled
            if args.enable_reward_modulation:
                reward = 0.0
                internal = percept.getInternal()
                if internal and len(internal) > 0:
                    reward = internal[0]
                agent.applyRewardModulation(reward, 0.0)
            
            if args.verbose and step % 100 == 0:
                print(f"Step {step}: "
                      f"Firing neurons: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Print statistics
        print("\nSimulation complete!")
        print(f"Final statistics:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
        
        return brain, agent, world
    
    def run_benchmark(self, args):
        """Run performance benchmarks"""
        print(f"Running NLM performance benchmarks...")
        print("This will test performance with different network sizes.\n")
        
        # Use the benchmark functions from pynlm
        if args.benchmark_type == "quick":
            result = pynlm.quickBenchmark(args.neurons)
            print(f"\nQuick benchmark results for {args.neurons} neurons:")
            print(f"  Neurons: {result.get('neurons')}")
            print(f"  Steps/sec: {result.get('steps_per_second'):.1f}")
            print(f"  Total spikes: {result.get('total_spikes')}")
            print(f"  Average firing rate: {result.get('average_firing_rate'):.3f}")
            print(f"  Benchmark time: {result.get('benchmark_time_seconds'):.2f}s")
            
        elif args.benchmark_type == "full":
            results = pynlm.runBenchmarks()
            print(f"\nBenchmark results:")
            for i, result in enumerate(results):
                neurons = result.neurons
                steps_per_sec = result.stepsPerSecond
                spikes_per_step = result.spikesPerStep
                synapses_per_neuron = result.synapsesPerNeuron
                print(f"  Test {i+1}: {neurons} neurons - "
                      f"{steps_per_sec:.0f} steps/sec, "
                      f"{spikes_per_step:.2f} spikes/step, "
                      f"{synapses_per_neuron:.1f} synapses/neuron")
    
    def run_episodes(self, args):
        """Run multiple episodes"""
        print(f"Running {args.num_episodes} episodes...")
        
        # Create environment
        brain, agent, world = self.run_simulation(args)
        
        # If we have more episodes, run additional ones
        episode_stats = []
        for episode in range(1, args.num_episodes):
            print(f"\nEpisode {episode + 1}/{args.num_episodes}")
            
            # Reset for new episode
            brain.reset()
            agent.reset()
            world.reset()
            
            # Run episode with shorter steps
            for step in range(min(args.steps, 500)):  # Limit episode length
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward if enabled
                if args.enable_reward_modulation:
                    reward = 0.0
                    internal = percept.getInternal()
                    if internal and len(internal) > 0:
                        reward = internal[0]
                    agent.applyRewardModulation(reward, 0.0)
                
                agent.updateDevelopment(0.1)
            
            # Collect statistics
            if args.collect_stats:
                stats = {
                    "episode": episode,
                    "final_neurons_firing": brain.getFiringNeuronCount(),
                    "total_spikes": brain.getTotalSpikeCount(),
                    "curiosity_level": agent.getCuriosityLevel(),
                    "novelty_level": agent.getNoveltyLevel(),
                    "prediction_error": agent.getPredictionError()
                }
                episode_stats.append(stats)
                print(f"  Episode stats: {stats}")
        
        print(f"\nCompleted {args.num_episodes} episodes!")
        if episode_stats:
            print(f"Collected statistics for {len(episode_stats)} episodes.")
        
        return episode_stats
    
    def load_config(self, filepath):
        """Load configuration from file"""
        try:
            self.config.loadFromFile(filepath)
            print(f"Loaded configuration from {filepath}")
            return True
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return False
    
    def save_config(self, filepath):
        """Save configuration to file"""
        try:
            self.config.saveToFile(filepath)
            print(f"Saved configuration to {filepath}")
            return True
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    def print_config(self):
        """Print current configuration"""
        print("Current configuration:")
        keys = self.config.getKeys()
        for key in sorted(keys):
            print(f"  {key}")

def main():
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s simulate --neurons 1000 --steps 1000
  %(prog)s benchmark --neurons 500
  %(prog)s episodes --episodes 5 --neurons 200 --steps 500
  %(prog)s config --load config.json
  %(prog)s config --save my_config.json
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Simulate command
    simulate_parser = subparsers.add_parser("simulate", help="Run a single simulation episode")
    simulate_parser.add_argument("--neurons", type=int, default=1000,
                                 help="Number of neurons (default: 1000)")
    simulate_parser.add_argument("--steps", type=int, default=1000,
                                 help="Number of simulation steps (default: 1000)")
    simulate_parser.add_argument("--enable-reward-modulation", action="store_true",
                                 help="Enable reward-based neuromodulation")
    simulate_parser.add_argument("--enable-curiosity", action="store_true",
                                 help="Enable curiosity-driven exploration")
    simulate_parser.add_argument("--enable-development", action="store_true",
                                 help="Enable developmental processes")
    simulate_parser.add_argument("--enable-structural-plasticity", action="store_true",
                                 help="Enable structural plasticity")
    simulate_parser.add_argument("--verbose", action="store_true",
                                 help="Print verbose output during simulation")
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser("benchmark", help="Run performance benchmarks")
    benchmark_parser.add_argument("--neurons", type=int, default=500,
                                 help="Number of neurons for quick benchmark (default: 500)")
    benchmark_parser.add_argument("--type", dest="benchmark_type", choices=["quick", "full"],
                                 default="quick", help="Benchmark type (default: quick)")
    
    # Episodes command
    episodes_parser = subparsers.add_parser("episodes", help="Run multiple episodes")
    episodes_parser.add_argument("--episodes", type=int, default=3,
                                 help="Number of episodes to run (default: 3)")
    episodes_parser.add_argument("--neurons", type=int, default=200,
                                 help="Number of neurons (default: 200)")
    episodes_parser.add_argument("--steps", type=int, default=500,
                                 help="Steps per episode (default: 500)")
    episodes_parser.add_argument("--collect-stats", action="store_true",
                                 help="Collect statistics for each episode")
    episodes_parser.add_argument("--enable-reward-modulation", action="store_true",
                                 help="Enable reward-based neuromodulation")
    episodes_parser.add_argument("--enable-curiosity", action="store_true",
                                 help="Enable curiosity-driven exploration")
    
    # Config command
    config_parser = subparsers.add_parser("config", help="Configuration management")
    config_subparsers = config_parser.add_subparsers(dest="config_command", help="Configuration commands")
    
    load_parser = config_subparsers.add_parser("load", help="Load configuration from file")
    load_parser.add_argument("filepath", help="Configuration file path")
    
    save_parser = config_subparsers.add_parser("save", help="Save configuration to file")
    save_parser.add_argument("filepath", help="Configuration file path")
    
    print_parser = config_subparsers.add_parser("print", help="Print current configuration")
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    # Initialize CLI
    cli = NLMCLI()
    
    # Execute commands
    try:
        if args.command == "simulate":
            cli.run_simulation(args)
        elif args.command == "benchmark":
            cli.run_benchmark(args)
        elif args.command == "episodes":
            cli.run_episodes(args)
        elif args.command == "config":
            if args.config_command == "load":
                cli.load_config(args.filepath)
            elif args.config_command == "save":
                cli.save_config(args.filepath)
            elif args.config_command == "print":
                cli.print_config()
        else:
            print(f"Unknown command: {args.command}")
            parser.print_help()
            return 1
            
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
