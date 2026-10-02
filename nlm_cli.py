#!/usr/bin/env python3
"""
NLM Command Line Interface

Simple command-line tool for testing and experimenting with NLM brain simulations.

Usage:
    nlm_cli [command] [options]

Commands:
    quick-test          Run a quick brain test
    run-simulation      Run a complete simulation
    run-agent           Run an agent in a world
    stats               Show brain statistics
    help                Show help information

Examples:
    nlm_cli quick-test --steps 100
    nlm_cli run-simulation --brain brain.bin --steps 1000
    nlm_cli run-agent --world-config myworld.json --steps 500
    nlm_cli stats brain.bin
"""

import argparse
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
def quick_test(args):
    """Run a quick brain test with default settings."""
    print("=== NLM Quick Test ===")
    
    # Create brain with default settings
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 1)
    config.set("random_seed", 42)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Region count: {brain.getRegionCount()}")
    
    # Run simulation
    steps = args.steps if hasattr(args, 'steps') else 100
    print(f"Running {steps} simulation steps...")
    
    for step in range(steps):
        brain.step(step)
        
        if step % 50 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # Show final stats
    stats = nlm.computeBrainStats(brain)
    print(f"\n=== Final Stats ===")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    print("=== Quick Test Complete ===")
    return brain
def run_simulation(args):
    """Run a complete simulation from a saved brain or create new one."""
    print("=== NLM Simulation ===")
    
    if args.brain:
        # Load existing brain
        print(f"Loading brain from: {args.brain}")
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.load(args.brain)
    else:
        # Create new brain
        print("Creating new brain...")
        config = nlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.region_count", 2)
        config.set("random_seed", args.seed)
        
        brain = nlm.createBrain(config)
        brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Region count: {brain.getRegionCount()}")
    
    # Run simulation
    steps = args.steps if hasattr(args, 'steps') else 1000
    print(f"Running {steps} simulation steps...")
    
    for step in range(steps):
        brain.step(step, step * 0.001)
        
        if step % 100 == 0:
            firing = brain.getFiringNeuronCount()
            total_spikes = brain.getTotalSpikeCount()
            rate = brain.getAverageFiringRate()
            print(f"  Step {step:4d}: {firing:4d} firing, {total_spikes:6d} total, {rate:6.2f} Hz avg")
    
    # Save if requested
    if args.save:
        print(f"Saving brain to: {args.save}")
        brain.save(args.save)
    
    print("=== Simulation Complete ===")
    return brain
def run_agent(args):
    """Run an agent in a world environment."""
    print("=== NLM Agent Simulation ===")
    
    # Create world
    print("Creating world...")
    world = nlm.createSimpleWorld()
    world.configure(width=args.world_width, height=args.world_height,
                    visionWidth=args.vision_width, visionHeight=args.vision_height)
    world.setAgentStart(args.agent_x, args.agent_y)
    
    # Create brain
    print("Creating brain...")
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("random_seed", args.seed)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    print("Creating agent...")
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features
    if args.enable_features:
        agent.setAllFeatures(True)
    
    # Run episode
    steps = args.steps if hasattr(args, 'steps') else 500
    print(f"Running agent episode for {steps} steps...")
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step, step * 0.1)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        if args.enable_features:
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            firing = brain.getFiringNeuronCount()
            spikes = brain.getTotalSpikeCount()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            prediction_error = agent.getPredictionError()
            print(f"  Step {step:3d}: {firing:3d} firing, {spikes:5d} spikes, "
                  f"curiosity:{curiosity:.2f}, novelty:{novelty:.2f}, pred_err:{prediction_error:.2f}")
    
    print("=== Agent Episode Complete ===")
    return brain, agent, world
def brain_stats(args):
    """Show brain statistics from a file or current brain."""
    print("=== NLM Brain Statistics ===")
    
    if args.brain:
        # Load brain from file
        print(f"Loading brain from: {args.brain}")
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.load(args.brain)
    else:
        # Create default brain for stats
        print("Creating default brain for statistics...")
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
    
    # Format and display stats
    formatted = nlm.formatBrainStats(brain)
    print(formatted)
    
    # Save to file if requested
    if args.output:
        with open(args.output, 'w') as f:
            f.write(formatted)
        print(f"Statistics saved to: {args.output}")
    
    print("=== Statistics Complete ===")
    return brain
def interactive_mode():
    """Start an interactive Python session with NLM imports."""
    print("=== NLM Interactive Mode ===")
    print("Starting Python interpreter with NLM loaded...")
    print("Type 'exit' or 'quit' to return to command line")
    print()
    
    while True:
        try:
            code = input("nlm> ")
            if code.lower() in ('exit', 'quit'):
                break
            elif code.strip():
                # Execute in the current namespace
                exec(code, globals())
        except KeyboardInterrupt:
            print("\nKeyboard interrupt, returning to command line")
            break
        except Exception as e:
            print(f"Error: {e}")
    
    print("Returning to command line mode")
def main():
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Quick test command
    quick_parser = subparsers.add_parser("quick-test", help="Run a quick brain test")
    quick_parser.add_argument("--steps", type=int, default=100,
                             help="Number of simulation steps (default: 100)")
    
    # Simulation command
    sim_parser = subparsers.add_parser("run-simulation", help="Run a complete simulation")
    sim_parser.add_argument("--brain", help="Load brain from file (otherwise create new)")
    sim_parser.add_argument("--seed", type=int, default=42,
                           help="Random seed for new brain (default: 42)")
    sim_parser.add_argument("--steps", type=int, default=1000,
                           help="Number of simulation steps (default: 1000)")
    sim_parser.add_argument("--save", help="Save brain to file after simulation")
    
    # Agent command
    agent_parser = subparsers.add_parser("run-agent", help="Run an agent in a world")
    agent_parser.add_argument("--world-width", type=int, default=20,
                             help="World width (default: 20)")
    agent_parser.add_argument("--world-height", type=int, default=20,
                             help="World height (default: 20)")
    agent_parser.add_argument("--vision-width", type=int, default=8,
                             help="Vision width (default: 8)")
    agent_parser.add_argument("--vision-height", type=int, default=8,
                             help="Vision height (default: 8)")
    agent_parser.add_argument("--agent-x", type=float, default=10.0,
                             help="Agent starting X position (default: 10.0)")
    agent_parser.add_argument("--agent-y", type=float, default=10.0,
                             help="Agent starting Y position (default: 10.0)")
    agent_parser.add_argument("--seed", type=int, default=42,
                             help="Random seed (default: 42)")
    agent_parser.add_argument("--steps", type=int, default=500,
                             help="Number of simulation steps (default: 500)")
    agent_parser.add_argument("--enable-features", action="store_true",
                             help="Enable all learning features (reward, plasticity, development, curiosity)")
    
    # Stats command
    stats_parser = subparsers.add_parser("stats", help="Show brain statistics")
    stats_parser.add_argument("brain", nargs="?", help="Brain file to analyze")
    stats_parser.add_argument("--output", help="Save statistics to file")
    
    # Interactive command
    interactive_parser = subparsers.add_parser("interactive", help="Start interactive Python session")
    
    # Help command
    help_parser = subparsers.add_parser("help", help="Show help information")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Dispatch commands
    if args.command == "quick-test":
        quick_test(args)
    elif args.command == "run-simulation":
        run_simulation(args)
    elif args.command == "run-agent":
        run_agent(args)
    elif args.command == "stats":
        brain_stats(args)
    elif args.command == "interactive":
        interactive_mode()
    elif args.command == "help" or not args.command:
        parser.print_help()
    else:
        parser.print_help()
        sys.exit(1)
if __name__ == "__main__":
    main()
