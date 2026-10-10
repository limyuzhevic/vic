#!/usr/bin/env python3
"""
NLM Command Line Interface

Command line tool for running NLM simulations and experiments.
"""

import argparse
import sys
import os
import time
from pathlib import Path

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

import pynlm


def simple_brain(args):
    """Run a simple brain simulation."""
    print(f"Running simple brain simulation...")
    print(f"Neurons: {args.neurons}, Steps: {args.steps}")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    start_time = time.time()
    for step in range(args.steps):
        brain.step(step)
    
    end_time = time.time()
    
    # Print results
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    return True


def brain_in_world(args):
    """Run a brain in a world simulation."""
    print(f"Running brain in world simulation...")
    print(f"World size: {args.width}x{args.height}, Steps: {args.steps}")
    
    # Create brain, world, and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=args.width,
        height=args.height,
        visionWidth=args.vision_width,
        visionHeight=args.vision_height
    )
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features if requested
    if args.reward_modulation:
        agent.enableRewardModulation(True)
    if args.structural_plasticity:
        agent.enableStructuralPlasticity(True)
    if args.development:
        agent.enableDevelopment(True)
    if args.curiosity:
        agent.enableCuriosity(True)
    
    # Run simulation
    start_time = time.time()
    for step in range(args.steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    # Print results
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    return True


def advanced_agent(args):
    """Run an advanced agent with learning capabilities."""
    print(f"Running advanced agent simulation...")
    print(f"Learning features: Reward={args.reward_modulation}, "
          f"Structure={args.structural_plasticity}, "
          f"Development={args.development}, Curiosity={args.curiosity}")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=args.width,
        height=args.height,
        visionWidth=args.vision_width,
        visionHeight=args.vision_height
    )
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    start_time = time.time()
    for step in range(args.steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    # Print results
    print(f"\nAdvanced simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Learning features enabled and active")
    print(f"Final statistics:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    return True


def memory_experiment(args):
    """Run a memory experiment."""
    print(f"Running memory experiment: {args.memory_type}")
    
    # Create brain with appropriate configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable memory features
    if args.memory_type == "working":
        # Configure working memory
        pass
    elif args.memory_type == "episodic":
        # Configure episodic memory
        pass
    elif args.memory_type == "combined":
        # Enable both memory types
        pass
    
    # Run simulation
    start_time = time.time()
    for step in range(args.steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    # Print results
    print(f"\nMemory experiment completed in {end_time - start_time:.2f} seconds")
    print(f"Memory type: {args.memory_type}")
    print(f"Replay interval: {args.replay_interval} steps")
    
    return True


def prediction_test(args):
    """Run a prediction learning test."""
    print(f"Running prediction test...")
    print(f"Prediction horizon: {args.prediction_horizon}, Learning rate: {args.learning_rate}")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=8, height=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation with prediction focus
    start_time = time.time()
    for step in range(args.steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    # Print results
    print(f"\nPrediction test completed in {end_time - start_time:.2f} seconds")
    print(f"Prediction horizon: {args.prediction_horizon}")
    print(f"Learning rate: {args.learning_rate}")
    
    return True


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface for running brain simulations",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s simple --neurons 500 --steps 1000
  %(prog)s brain-in-world --width 10 --height 10 --steps 500
  %(prog)s advanced-agent --width 15 --height 15 --steps 1000
  %(prog)s memory-experiment --memory-type episodic --steps 2000
  %(prog)s prediction-test --prediction-horizon 20 --steps 1000

For more information about specific commands, use:
  %(prog)s <command> --help
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Simple brain command
    simple_parser = subparsers.add_parser("simple", help="Run simple brain simulation")
    simple_parser.add_argument("--neurons", type=int, default=500, help="Number of neurons")
    simple_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    simple_parser.set_defaults(func=simple_brain)
    
    # Brain in world command
    world_parser = subparsers.add_parser("brain-in-world", help="Run brain in world simulation")
    world_parser.add_argument("--width", type=int, default=10, help="World width")
    world_parser.add_argument("--height", type=int, default=10, help="World height")
    world_parser.add_argument("--vision-width", type=int, default=8, help="Agent vision width")
    world_parser.add_argument("--vision-height", type=int, default=8, help="Agent vision height")
    world_parser.add_argument("--steps", type=int, default=500, help="Number of simulation steps")
    world_parser.add_argument("--reward-modulation", action="store_true", help="Enable reward modulation")
    world_parser.add_argument("--structural-plasticity", action="store_true", help="Enable structural plasticity")
    world_parser.add_argument("--development", action="store_true", help="Enable development")
    world_parser.add_argument("--curiosity", action="store_true", help="Enable curiosity")
    world_parser.set_defaults(func=brain_in_world)
    
    # Advanced agent command
    advanced_parser = subparsers.add_parser("advanced-agent", help="Run advanced agent with learning")
    advanced_parser.add_argument("--width", type=int, default=15, help="World width")
    advanced_parser.add_argument("--height", type=int, default=15, help="World height")
    advanced_parser.add_argument("--vision-width", type=int, default=8, help="Agent vision width")
    advanced_parser.add_argument("--vision-height", type=int, default=8, help="Agent vision height")
    advanced_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    advanced_parser.set_defaults(func=advanced_agent)
    
    # Memory experiment command
    memory_parser = subparsers.add_parser("memory-experiment", help="Run memory experiment")
    memory_parser.add_argument(
        "--memory-type", 
        choices=["working", "episodic", "combined"], 
        default="episodic",
        help="Type of memory to experiment with"
    )
    memory_parser.add_argument("--steps", type=int, default=2000, help="Number of simulation steps")
    memory_parser.add_argument(
        "--replay-interval", 
        type=int, 
        default=100, 
        help="Steps between memory replay"
    )
    memory_parser.set_defaults(func=memory_experiment)
    
    # Prediction test command
    prediction_parser = subparsers.add_parser("prediction-test", help="Run prediction learning test")
    prediction_parser.add_argument(
        "--prediction-horizon", 
        type=int, 
        default=20, 
        help="How far ahead to predict"
    )
    prediction_parser.add_argument(
        "--learning-rate", 
        type=float, 
        default=0.01, 
        help="Learning rate for predictions"
    )
    prediction_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    prediction_parser.set_defaults(func=prediction_test)
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute command
    if not hasattr(args, 'func'):
        parser.print_help()
        return 1
    
    try:
        result = args.func(args)
        return 0 if result else 1
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
