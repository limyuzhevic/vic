#!/usr/bin/env python3
"""
NLM Command Line Interface

Advanced command-line tool for running NLM simulations with various options.
Supports different simulation modes, configurations, and output formats.
"""

import argparse
import sys
import json
import time
import os
from pathlib import Path
from typing import Optional, Dict, Any

import pynlm

def create_parser() -> argparse.ArgumentParser:
    """Create and configure the argument parser."""
    parser = argparse.ArgumentParser(
        prog="nlm",
        description="Neural Learning Machine (NLM) Simulation Framework",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm run --brain-config config.json --world-size 50 --steps 1000
  nlm demo --mode phase6 --fast --output demo_output.json
  nlm benchmark --brain-type "large" --parallel 4 --repeats 5
  nlm config create --output custom_config.json
  nlm explore --scene "maze" --size 30 --enable-learning

For more information, visit: https://github.com/nlm-project/nlm
        """
    )
    
    parser.add_argument(
        "--version", 
        action="version", 
        version="NLM 0.1.0 (Phase 6 Final Integration)",
        help="Show version information and exit"
    )
    
    parser.add_argument(
        "--verbose", 
        "-v", 
        action="store_true",
        help="Enable verbose output with detailed statistics"
    )
    
    parser.add_argument(
        "--quiet", 
        "-q", 
        action="store_true",
        help="Suppress non-essential output"
    )
    
    parser.add_argument(
        "--log-level", 
        choices=["debug", "info", "warning", "error"],
        default="info",
        help="Set logging level (default: info)"
    )
    
    # Main commands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Run command
    run_parser = subparsers.add_parser("run", help="Run a simulation")
    run_parser.add_argument(
        "--brain-config", 
        default="configs/default.cfg",
        help="Path to brain configuration file (default: configs/default.cfg)"
    )
    run_parser.add_argument(
        "--world-width", 
        type=int, 
        default=30,
        help="World width in cells (default: 30)"
    )
    run_parser.add_argument(
        "--world-height", 
        type=int, 
        default=30,
        help="World height in cells (default: 30)"
    )
    run_parser.add_argument(
        "--world-vision-width", 
        type=int, 
        default=10,
        help="Vision width in cells (default: 10)"
    )
    run_parser.add_argument(
        "--world-vision-height", 
        type=int, 
        default=10,
        help="Vision height in cells (default: 10)"
    )
    run_parser.add_argument(
        "--steps", 
        type=int, 
        default=1000,
        help="Number of simulation steps (default: 1000)"
    )
    run_parser.add_argument(
        "--timestep", 
        type=float, 
        default=0.1,
        help="Simulation timestep in seconds (default: 0.1)"
    )
    run_parser.add_argument(
        "--output", 
        "-o",
        help="Output file path for results (JSON format)"
    )
    run_parser.add_argument(
        "--checkpoint", 
        help="Checkpoint file path for loading initial state"
    )
    run_parser.add_argument(
        "--save-checkpoint", 
        action="store_true",
        help="Enable checkpoint saving"
    )
    run_parser.add_argument(
        "--load-checkpoint", 
        help="Load checkpoint file path"
    )
    
    # Demo command
    demo_parser = subparsers.add_parser("demo", help="Run a demonstration with presets")
    demo_parser.add_argument(
        "--mode", 
        choices=["phase2", "phase3", "phase4", "phase5", "phase6"],
        default="phase6",
        help="Demo mode (default: phase6)"
    )
    demo_parser.add_argument(
        "--fast", 
        action="store_true",
        help="Run in fast mode with reduced settings"
    )
    demo_parser.add_argument(
        "--detailed", 
        action="store_true",
        help="Run in detailed mode with comprehensive output"
    )
    demo_parser.add_argument(
        "--output", 
        "-o",
        help="Output file path for demo results"
    )
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser("benchmark", help="Run performance benchmarks")
    benchmark_parser.add_argument(
        "--brain-types", 
        nargs="+",
        choices=["small", "medium", "large", "xlarge"],
        default=["small", "medium", "large"],
        help="Brain configurations to benchmark (default: small medium large)"
    )
    benchmark_parser.add_argument(
        "--repeats", 
        type=int, 
        default=3,
        help="Number of repeats for each configuration (default: 3)"
    )
    benchmark_parser.add_argument(
        "--parallel", 
        type=int, 
        default=1,
        help="Number of parallel threads (default: 1)"
    )
    benchmark_parser.add_argument(
        "--metrics", 
        nargs="+",
        choices=["speed", "memory", "spikes", "learning"],
        default=["speed", "memory", "spikes"],
        help="Metrics to collect (default: speed memory spikes)"
    )
    benchmark_parser.add_argument(
        "--output", 
        "-o",
        help="Output file path for benchmark results"
    )
    
    # Config command
    config_parser = subparsers.add_parser("config", help="Configuration management")
    config_subparsers = config_parser.add_subparsers(dest="config_command", help="Config commands")
    
    create_parser = config_subparsers.add_parser("create", help="Create a new configuration")
    create_parser.add_argument(
        "--output", 
        "-o",
        required=True,
        help="Output configuration file path"
    )
    create_parser.add_argument(
        "--brain-count", 
        type=int, 
        default=1000,
        help="Number of neurons (default: 1000)"
    )
    create_parser.add_argument(
        "--region-count", 
        type=int, 
        default=1,
        help="Number of regions (default: 1)"
    )
    create_parser.add_argument(
        "--connection-probability", 
        type=float, 
        default=0.1,
        help="Connection probability (default: 0.1)"
    )
    
    list_parser = config_subparsers.add_parser("list", help="List all configuration options")
    
    # Explore command
    explore_parser = subparsers.add_parser("explore", help="Explore different simulation scenarios")
    explore_parser.add_argument(
        "--scene", 
        choices=["maze", "open", "complex", "social"],
        default="open",
        help="Simulation scene type (default: open)"
    )
    explore_parser.add_argument(
        "--size", 
        type=int, 
        default=30,
        help="World size (default: 30)"
    )
    explore_parser.add_argument(
        "--enable-learning", 
        action="store_true",
        help="Enable all learning subsystems"
    )
    explore_parser.add_argument(
        "--steps", 
        type=int, 
        default=500,
        help="Number of steps to run (default: 500)"
    )
    
    # Analysis command
    analysis_parser = subparsers.add_parser("analyze", help="Analyze simulation results")
    analysis_parser.add_argument(
        "--input", 
        "-i",
        required=True,
        help="Input results file path"
    )
    analysis_parser.add_argument(
        "--type", 
        choices=["performance", "behavior", "learning"],
        default="performance",
        help="Analysis type (default: performance)"
    )
    analysis_parser.add_argument(
        "--output", 
        "-o",
        help="Output file path for analysis results"
    )
    
    return parser
def load_config(config_path: str) -> pynlm.Config:
    """Load configuration from file."""
    try:
        config = pynlm.createDefaultConfig()
        if os.path.exists(config_path):
            config.loadFromFile(config_path)
            print(f"Loaded configuration from: {config_path}")
        else:
            print(f"Warning: Configuration file not found: {config_path}")
            print("Using default configuration")
        return config
    except Exception as e:
        print(f"Error loading configuration: {e}")
        return pynlm.createDefaultConfig()
def create_config(args) -> pynlm.Config:
    """Create configuration based on command line arguments."""
    config = pynlm.createDefaultConfig()
    
    # Set basic parameters
    config.set("brain.neuron_count", args.brain_count)
    config.set("brain.synapse_density", args.connection_probability)
    config.set("region_count", args.region_count)
    
    # Set simulation parameters
    config.set("simulation_timestep", 0.001)  # Fixed timestep
    config.set("random_seed", 42)  # Deterministic for reproducibility
    
    # Enable subsystems based on available config options
    try:
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("development.enable", True)
    except:
        pass  # Config options may not be available
    
    return config
def run_simulation(args):
    """Run a simulation with the given arguments."""
    print(f"Starting simulation with {args.steps} steps")
    
    # Load or create configuration
    config = load_config(args.brain_config)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
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
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Load checkpoint if specified
    if args.load_checkpoint:
        print(f"Loading checkpoint: {args.load_checkpoint}")
        # Note: This would require a save/load API in the Python bindings
        # For now, we'll just log that it's not implemented
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    start_time = time.time()
    
    for step in range(args.steps):
        # Update world
        world.update(args.timestep)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(args.timestep)
        
        # Print progress
        if not args.quiet and (step % 100 == 0 or step == args.steps - 1):
            print(f"Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\nSimulation completed in {duration:.2f} seconds")
    print(f"Average speed: {args.steps / duration:.1f} steps/second")
    
    # Collect results
    results = {
        "steps": args.steps,
        "duration": duration,
        "speed": args.steps / duration,
        "final_firing_rate": brain.getAverageFiringRate(),
        "total_spikes": brain.getTotalSpikeCount(),
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel(),
        "development_stage": brain.getDevelopmentalStage(),
        "working_memory_traces": brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
        "episodes_stored": brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
        "avg_reward": sum([world.getSensoryPercept().getInternal()[0] for _ in range(args.steps)]) / args.steps if world.getSensoryPercept().getInternal() else 0.0
    }
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to: {args.output}")
    
    return results
def run_demo(args):
    """Run a demonstration with preset settings."""
    print(f"Running {args.mode} demo")
    
    # Configuration presets
    presets = {
        "phase2": {
            "neuron_count": 500,
            "steps": 500,
            "fast": True,
            "description": "Phase 2: Basic neural computation and learning"
        },
        "phase3": {
            "neuron_count": 1000,
            "steps": 1000,
            "fast": False,
            "description": "Phase 3: World interaction and behavior"
        },
        "phase4": {
            "neuron_count": 2000,
            "steps": 2000,
            "fast": False,
            "description": "Phase 4: Emerging cognition and memory"
        },
        "phase5": {
            "neuron_count": 3000,
            "steps": 3000,
            "fast": False,
            "description": "Phase 5: Performance optimizations and scaling"
        },
        "phase6": {
            "neuron_count": 5000,
            "steps": 5000,
            "fast": args.fast,
            "description": "Phase 6: Final integration and all systems"
        }
    }
    
    preset = presets[args.mode]
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", preset["neuron_count"])
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(
        width=50,
        height=50,
        visionWidth=15,
        visionHeight=15
    )
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Preset: {preset['description']}")
    print(f"Configuration: {preset['neuron_count']} neurons, {preset['steps']} steps")
    
    # Run demonstration
    for step in range(preset["steps"]):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if args.detailed and (step % 500 == 0 or step == preset["steps"] - 1):
            print(f"Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Action: {action}")
    
    print(f"\nDemo completed successfully!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    return {
        "mode": args.mode,
        "steps": preset["steps"],
        "neuron_count": preset["neuron_count"],
        "total_spikes": brain.getTotalSpikeCount(),
        "avg_firing_rate": brain.getAverageFiringRate()
    }
def run_benchmark(args):
    """Run performance benchmarks."""
    print("Running benchmarks...")
    
    results = {}
    
    for brain_type in args.brain_types:
        print(f"\nBenchmarking {brain_type} brain configuration...")
        
        # Set neuron count based on brain type
        neuron_counts = {
            "small": 500,
            "medium": 1000,
            "large": 2000,
            "xlarge": 5000
        }
        
        neuron_count = neuron_counts.get(brain_type, 1000)
        
        # Run multiple repeats
        times = []
        firing_rates = []
        spike_counts = []
        
        for repeat in range(args.repeats):
            print(f"  Repeat {repeat + 1}/{args.repeats}...")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create world and agent
            world = pynlm.createSimpleWorld()
            world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable subsystems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            start_time = time.time()
            
            for step in range(200):  # Fixed benchmark length
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
            
            end_time = time.time()
            duration = end_time - start_time
            
            times.append(duration)
            firing_rates.append(brain.getAverageFiringRate())
            spike_counts.append(brain.getTotalSpikeCount())
        
        # Calculate statistics
        avg_time = sum(times) / len(times)
        std_time = (sum([(t - avg_time) ** 2 for t in times]) / len(times)) ** 0.5
        avg_firing = sum(firing_rates) / len(firing_rates)
        avg_spikes = sum(spike_counts) / len(spike_counts)
        
        results[brain_type] = {
            "repeats": args.repeats,
            "avg_duration": avg_time,
            "duration_std": std_time,
            "steps_per_second": 200 / avg_time,
            "avg_firing_rate": avg_firing,
            "avg_spike_count": avg_spikes,
            "min_duration": min(times),
            "max_duration": max(times)
        }
        
        print(f"  {brain_type}: {avg_time:.3f}s ± {std_time:.3f}s, "
              f"{200/avg_time:.1f} steps/s, "
              f"{avg_firing:.3f} avg firing rate")
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Benchmark results saved to: {args.output}")
    
    return results
def run_config_command(args):
    """Handle configuration management commands."""
    if args.config_command == "create":
        print(f"Creating configuration with {args.brain_count} neurons...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", args.brain_count)
        config.set("brain.synapse_density", args.connection_probability)
        config.set("region_count", args.region_count)
        
        # Save configuration
        with open(args.output, 'w') as f:
            f.write(f"# NLM Configuration\n")
            f.write(f"brain.neuron_count = {args.brain_count}\n")
            f.write(f"brain.synapse_density = {args.connection_probability}\n")
            f.write(f"region_count = {args.region_count}\n")
            f.write(f"# Created by NLM CLI\n")
        
        print(f"Configuration saved to: {args.output}")
        
    elif args.config_command == "list":
        print("Available configuration options:")
        config = pynlm.createDefaultConfig()
        
        # Try to list common configuration keys
        common_keys = [
            "brain.neuron_count",
            "brain.synapse_density",
            "brain.connection_probability",
            "region_count",
            "plasticity.stdp.enable",
            "plasticity.hebbian.enable",
            "neuromod.curiosity.enable",
            "neuromod.novelty.enable",
            "development.enable",
            "simulation_timestep",
            "random_seed"
        ]
        
        for key in common_keys:
            if config.has(key):
                print(f"  {key}: {config.get(key)}")
            else:
                print(f"  {key}: (not available)")
        
        print("\nUse 'run --help' for command-line options")
def run_explore(args):
    """Explore different simulation scenarios."""
    print(f"Exploring {args.scene} scenario with size {args.size}")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(
        width=args.size,
        height=args.size,
        visionWidth=args.size // 3,
        visionHeight=args.size // 3
    )
    world.reset()
    world.setAgentStart(args.size // 2, args.size // 2)
    
    agent = pynlm.createAgentBrain()
    brain = agent.getBrain()
    agent.initialize(world)
    
    # Enable learning subsystems
    if args.enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    
    print(f"Scene: {args.scene}")
    print(f"World size: {args.size}x{args.size}")
    print(f"Vision range: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Run exploration
    for step in range(args.steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if (step + 1) % 100 == 0:
            print(f"Step {step + 1}/{args.steps}: "
                  f"Position: ({world.getAgentBody().getX():.1f}, {world.getAgentBody().getY():.1f}), "
                  f"Action: {action}")
    
    print(f"\nExploration complete!")
    print(f"Final position: ({world.getAgentBody().getX():.1f}, {world.getAgentBody().getY():.1f})")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return {
        "scene": args.scene,
        "size": args.size,
        "steps": args.steps,
        "final_position": (world.getAgentBody().getX(), world.getAgentBody().getY()),
        "total_spikes": brain.getTotalSpikeCount()
    }
def main():
    """Main entry point."""
    parser = create_parser()
    args = parser.parse_args()
    
    try:
        if args.command == "run":
            result = run_simulation(args)
            if args.verbose:
                print("\nDetailed results:")
                for key, value in result.items():
                    print(f"  {key}: {value}")
                    
        elif args.command == "demo":
            result = run_demo(args)
            if args.verbose:
                print("\nDemo results:")
                for key, value in result.items():
                    print(f"  {key}: {value}")
                    
        elif args.command == "benchmark":
            result = run_benchmark(args)
            if args.verbose:
                print("\nBenchmark results:")
                for brain_type, metrics in result.items():
                    print(f"\n{brain_type}:")
                    for key, value in metrics.items():
                        print(f"  {key}: {value}")
                        
        elif args.command == "config":
            run_config_command(args)
            
        elif args.command == "explore":
            result = run_explore(args)
            if args.verbose:
                print("\nExploration results:")
                for key, value in result.items():
                    print(f"  {key}: {value}")
        
        else:
            parser.print_help()
            return 1
            
        return 0
        
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1
if __name__ == "__main__":
    sys.exit(main())