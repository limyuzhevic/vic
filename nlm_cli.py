#!/usr/bin/env python3
"""
Command Line Interface for NLM (Neural Learning Machine)

This script provides command-line access to the NLM simulation framework,
allowing users to run simulations, configure parameters, and save/load states
without writing Python code.

Installation:
    pip install -e .
    python nlm_cli.py --help
"""

import argparse
import json
import sys
import os
import time
import threading
from typing import Optional, Dict, Any

# Add parent directory to path to import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install the NLM Python bindings.")
    print("Run: pip install -e .")
    sys.exit(1)


def create_default_config():
    """Create and return a default configuration"""
    config = pynlm.createDefaultConfig()
    
    # Set some default advanced configuration
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 1)
    config.set("brain.connection_probability", 0.1)
    config.set("stdp_ltp_weight", 0.01f)
    config.set("stdp_ltd_weight", 0.012f)
    config.set("synaptogenesis_rate", 0.0001f)
    config.set("pruning_rate", 0.00001f)
    config.set("simulation_timestep", 0.001)
    
    return config


def load_config_from_file(filepath: str):
    """Load configuration from a file"""
    config = pynlm.createDefaultConfig()
    try:
        config.loadFromFile(filepath)
        print(f"Loaded configuration from {filepath}")
        return config
    except Exception as e:
        print(f"Error loading configuration from {filepath}: {e}")
        return None


def save_config_to_file(config, filepath: str):
    """Save configuration to a file"""
    try:
        config.saveToFile(filepath)
        print(f"Configuration saved to {filepath}")
        return True
    except Exception as e:
        print(f"Error saving configuration to {filepath}: {e}")
        return False


def print_simulation_header(title: str, config, world: bool = False):
    """Print simulation header information"""
    print(f"\n=== {title} ===")
    
    # Get configuration summary
    summary = config.summary() if hasattr(config, 'summary') else "Custom configuration"
    print(f"Configuration: {summary}")
    
    if world:
        print("Note: Using world simulation with agent-environment interaction")
    else:
        print("Note: Running brain-only simulation")


def run_brain_simulation(args):
    """Run a brain-only simulation"""
    # Create and initialize brain
    brain = pynlm.createBrain(args.config)
    brain.initialize()
    
    print_simulation_header(f"NLM Brain Simulation - {args.max_steps} steps", args.config)
    
    # Performance monitoring
    if args.performance_monitor:
        print("\nPerformance monitoring enabled...")
        print("Step | Time(ms) | Neurons | Spikes | FiringRate | E/I")
        print("-----|----------|---------|--------|------------|-----")
    
    start_time = time.time()
    step_times = []
    
    for step in range(args.max_steps):
        loop_start = time.time()
        
        # Run brain step
        brain.step(step)
        
        # Monitor performance
        loop_time = (time.time() - loop_start) * 1000  # Convert to ms
        step_times.append(loop_time)
        
        if args.performance_monitor and step % 100 == 0:
            print(f"{step:4d} | {loop_time:8.1f} | {brain.getTotalNeuronCount():6d} | "
                  f"{brain.getTotalSpikeCount():5d} | {brain.getAverageFiringRate():10.3f} | "
                  f"{brain.getExcitationInhibitionRatio():4.2f}")
    
    # Print performance summary
    end_time = time.time()
    total_time = end_time - start_time
    avg_step_time = total_time / args.max_steps * 1000  # Convert to ms
    
    print(f"\n=== Simulation Complete ===")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Average step time: {avg_step_time:.2f} ms")
    print(f"Simulation rate: {args.max_steps / (total_time/args.max_steps):.1f} steps/second")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return True


def run_world_simulation(args):
    """Run a world simulation with agent"""
    # Create brain
    brain = pynlm.createBrain(args.config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=args.width,
        height=args.height,
        visionWidth=args.vision_width,
        visionHeight=args.vision_height
    )
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems based on arguments
    if args.enable_reward_modulation:
        agent.enableRewardModulation(True)
    if args.enable_structural_plasticity:
        agent.enableStructuralPlasticity(True)
    if args.enable_development:
        agent.enableDevelopment(True)
    if args.enable_curiosity:
        agent.enableCuriosity(True)
    
    print_simulation_header(
        f"NLM World Simulation - {args.max_steps} steps", 
        args.config,
        world=True
    )
    
    print(f"World size: {args.width}x{args.height}")
    print(f"Vision: {args.vision_width}x{args.vision_height}")
    
    # Performance monitoring
    if args.performance_monitor:
        print("\nPerformance monitoring enabled...")
        print("Step | Time(ms) | Neurons | Spikes | FiringRate | E/I | Curiosity | Novelty | DevStage")
        print("-----|----------|---------|--------|------------|----|----------|---------|---------")
    
    start_time = time.time()
    step_times = []
    
    for step in range(args.max_steps):
        loop_start = time.time()
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if enabled
        if args.enable_reward_modulation and step % 50 == 0:
            reward = 1.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development if enabled
        if args.enable_development:
            agent.updateDevelopment(0.1)
        
        # Monitor performance
        loop_time = (time.time() - loop_start) * 1000  # Convert to ms
        step_times.append(loop_time)
        
        if args.performance_monitor and step % 100 == 0:
            print(f"{step:4d} | {loop_time:8.1f} | {brain.getTotalNeuronCount():6d} | "
                  f"{brain.getTotalSpikeCount():5d} | {brain.getAverageFiringRate():10.3f} | "
                  f"{brain.getExcitationInhibitionRatio():4.2f} | {agent.getCuriosityLevel():8.3f} | "
                  f"{agent.getNoveltyLevel():6.3f} | {brain.getDevelopmentalStage():8s}")
    
    # Print performance summary
    end_time = time.time()
    total_time = end_time - start_time
    avg_step_time = total_time / args.max_steps * 1000  # Convert to ms
    
    print(f"\n=== Simulation Complete ===")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Average step time: {avg_step_time:.2f} ms")
    print(f"Simulation rate: {args.max_steps / (total_time/args.max_steps):.1f} steps/second")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Save checkpoint if requested
    if args.checkpoint_file:
        print(f"\nSaving checkpoint to {args.checkpoint_file}...")
        if brain.save(args.checkpoint_file):
            print("Checkpoint saved successfully!")
        else:
            print("Failed to save checkpoint!")
    
    # Load checkpoint if requested
    if args.load_checkpoint:
        print(f"\nLoading checkpoint from {args.load_checkpoint}...")
        if brain.load(args.load_checkpoint):
            print("Checkpoint loaded successfully!")
        else:
            print("Failed to load checkpoint!")
    
    return True


def print_help():
    """Print help information"""
    help_text = """
NLM Command Line Interface

USAGE:
    python nlm_cli.py [OPTIONS]

BASIC OPTIONS:
    --neuron-count N          Number of neurons (default: 1000)
    --region-count R           Number of brain regions (default: 1)
    --connection-prob P        Connection probability (default: 0.1)
    --max-steps S              Maximum simulation steps (default: 1000)
    --world                    Enable world simulation (agent in environment)
    --world-width W            World width (default: 10)
    --world-height H           World height (default: 10)
    --vision-width V           Vision width (default: 8)
    --vision-height H          Vision height (default: 8)

LEARNING SYSTEMS:
    --enable-reward-modulation  Enable reward-based learning
    --enable-structural-plasticity Enable structural plasticity (brain growth)
    --enable-development        Enable developmental stages
    --enable-curiosity         Enable curiosity-driven exploration

PERFORMANCE:
    --performance-monitor      Enable performance monitoring
    --checkpoint-file FILE      Save brain state to file
    --load-checkpoint FILE     Load brain state from file

CONFIGURATION:
    --config-file FILE          Load configuration from file
    --save-config FILE          Save configuration to file

EXAMPLES:
    python nlm_cli.py --world --neuron-count 2000 --max-steps 5000
        Run a world simulation with 2000 neurons for 5000 steps

    python nlm_cli.py --enable-reward-modulation --enable-curiosity --performance-monitor
        Run brain simulation with learning enabled and monitoring

    python nlm_cli.py --world --checkpoint-file brain_state.bin --load-checkpoint saved.bin
        Load a saved brain state and continue simulation

For more information, visit: https://github.com/yourusername/nlm
    """
    print(help_text)


def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
        Examples:
          python nlm_cli.py --world --neuron-count 2000 --max-steps 5000
          python nlm_cli.py --enable-reward-modulation --enable-curiosity --performance-monitor
          python nlm_cli.py --world --checkpoint-file brain_state.bin --load-checkpoint saved.bin
        """
    )
    
    # Basic configuration options
    parser.add_argument("--neuron-count", type=int, default=1000,
                       help="Number of neurons (default: 1000)")
    parser.add_argument("--region-count", type=int, default=1,
                       help="Number of brain regions (default: 1)")
    parser.add_argument("--connection-prob", type=float, default=0.1,
                       help="Connection probability (default: 0.1)")
    parser.add_argument("--max-steps", type=int, default=1000,
                       help="Maximum simulation steps (default: 1000)")
    
    # World options
    parser.add_argument("--world", action="store_true",
                       help="Enable world simulation (agent in environment)")
    parser.add_argument("--width", type=int, default=10,
                       help="World width (default: 10)")
    parser.add_argument("--height", type=int, default=10,
                       help="World height (default: 10)")
    parser.add_argument("--vision-width", type=int, default=8,
                       help="Vision width (default: 8)")
    parser.add_argument("--vision-height", type=int, default=8,
                       help="Vision height (default: 8)")
    
    # Learning systems
    parser.add_argument("--enable-reward-modulation", action="store_true",
                       help="Enable reward-based learning")
    parser.add_argument("--enable-structural-plasticity", action="store_true",
                       help="Enable structural plasticity (brain growth)")
    parser.add_argument("--enable-development", action="store_true",
                       help="Enable developmental stages")
    parser.add_argument("--enable-curiosity", action="store_true",
                       help="Enable curiosity-driven exploration")
    
    # Performance options
    parser.add_argument("--performance-monitor", action="store_true",
                       help="Enable performance monitoring")
    parser.add_argument("--checkpoint-file", type=str,
                       help="Save brain state to file")
    parser.add_argument("--load-checkpoint", type=str,
                       help="Load brain state from file")
    
    # Configuration file options
    parser.add_argument("--config-file", type=str,
                       help="Load configuration from file")
    parser.add_argument("--save-config", type=str,
                       help="Save configuration to file")
    
    # Help option
    parser.add_argument("-h", "--help", action="store_true",
                       help="Show this help message and exit")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle help option
    if args.help or len(sys.argv) == 1:
        parser.print_help()
        return 0
    
    # Load or create configuration
    if args.config_file:
        config = load_config_from_file(args.config_file)
        if not config:
            return 1
    else:
        config = create_default_config()
        
        # Override config from command line arguments
        if args.neuron_count:
            config.set("brain.neuron_count", args.neuron_count)
        if args.region_count:
            config.set("brain.region_count", args.region_count)
        if args.connection_prob:
            config.set("brain.connection_probability", args.connection_prob)
    
    # Save config if requested
    if args.save_config:
        save_config_to_file(config, args.save_config)
    
    # Run simulation based on arguments
    if args.world:
        success = run_world_simulation(args)
    else:
        success = run_brain_simulation(args)
    
    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main())
