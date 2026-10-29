# NLM Python CLI Wrapper
# Simple command-line interface for the NLM Python package

import sys
import json
import argparse
from pathlib import Path

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Error: pynlm package not found. Please install the NLM Python bindings.", file=sys.stderr)
    sys.exit(1)

def print_help():
    """Print help information"""
    help_text = """
NLM Python CLI - Neural Learning Machine Interface

Usage: python -m pynlm [command] [options]

Commands:
  demo             Run a simple demonstration
  help, --help      Show this help message
  version, --version Show version information
  create           Create NLM components

Examples:
  python -m pynlm demo --episodes 10 --steps 200
  python -m pynlm help
  python -m pynlm version

For more detailed documentation, visit: https://github.com/nlm-project/nlm
"""
    print(help_text)

def print_version():
    """Print version information"""
    try:
        version = getattr(pynlm, '__version__', '0.1.0')
        print(f"NLM Python Bindings {version}")
        print("Python interface for Neural Learning Machine neural simulation framework")
    except Exception:
        print("NLM Python Bindings 0.1.0")

def create_components(args):
    """Create and demonstrate NLM components"""
    print("=== NLM Component Creation Demo ===")
    
    # Create default configuration
    print("Creating default configuration...")
    config = pynlm.createDefaultConfig()
    
    # Create brain
    print("Creating neural brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    print("Creating environment world...")
    world = pynlm.createSimpleWorld()
    world.configure(20.0, 20.0, 16, 16)
    
    # Create agent brain interface
    print("Creating agent brain interface...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    episodes = getattr(args, 'episodes', 5)
    steps = getattr(args, 'steps', 200)
    
    print(f"\nRunning simulation: {episodes} episodes, {steps} steps each")
    print("This demonstrates the agent-world interaction loop.")
    
    for episode in range(episodes):
        print(f"\n--- Episode {episode + 1}/{episodes} ---")
        
        # Reset world and agent
        world.reset()
        agent.reset()
        brain.reset()
        brain.initialize()
        
        episode_reward = 0.0
        
        for step in range(steps):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Simulate brain
            for neural_step in range(10):
                brain.step(neural_step)
            
            # Get motor command
            cmd = agent.decodeMotorCommand()
            
            # Apply action
            result = world.applyMotorCommand(cmd, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0)
            
            # Update world
            world.update(0.01)
            
            # Update development
            agent.updateDevelopment(0.01)
            
            episode_reward += result.reward
            
            if step % 50 == 0:
                print(f"  Step {step}: reward={result.reward:.3f}, "
                      f"action={cmd}, neuromod={agent.getNeuromodulationLevel():.3f}")
        
        print(f"  Episode reward: {episode_reward:.3f}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
    
    print("\n=== Simulation Complete ===")
    print("The NLM brain learned from experience through the agent-world interaction loop.")
    print("Sensory input → Neural processing → Action → Consequence → Learning")

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="NLM Python CLI - Neural Learning Machine Interface",
        prog="pynlm"
    )
    
    parser.add_argument(
        'command',
        nargs='?',
        default='help',
        help='Command to execute (demo, help, version, create)'
    )
    
    parser.add_argument(
        '--episodes',
        type=int,
        default=5,
        help='Number of episodes for simulation (default: 5)'
    )
    
    parser.add_argument(
        '--steps',
        type=int,
        default=200,
        help='Steps per episode (default: 200)'
    )
    
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    parser.add_argument(
        '--output',
        type=str,
        help='Output file for results'
    )
    
    parser.add_argument(
        '--seed',
        type=int,
        default=42,
        help='Random seed for reproducible results (default: 42)'
    )
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm package not available.", file=sys.stderr)
        sys.exit(1)
    
    if args.command in ['help', '--help']:
        print_help()
        return
    
    elif args.command in ['version', '--version']:
        print_version()
        return
    
    elif args.command == 'demo':
        create_components(args)
    
    elif args.command == 'create':
        create_components(args)
    
    else:
        print(f"Error: Unknown command '{args.command}'", file=sys.stderr)
        print("Use 'pynlm help' for available commands.", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()