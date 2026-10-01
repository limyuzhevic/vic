#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

A command-line interface for the NLM (Neural Learning Machine) project
that provides easy access to brain operations, simulation control, and system management.
"""

import argparse
import sys
import json
from pathlib import Path
from typing import Optional

# Import NLM modules
try:
    import pynlm
    print("NLM CLI loaded successfully")
except ImportError as e:
    print(f"Error importing pynlm: {e}")
    sys.exit(1)


def create_brain_from_config(config_file: Optional[str] = None) -> pynlm.Brain:
    """Create a brain from a configuration file or default config."""
    config = pynlm.createDefaultConfig()
    
    if config_file:
        try:
            config.loadFromFile(config_file)
            print(f"Loaded configuration from {config_file}")
        except Exception as e:
            print(f"Warning: Could not load config from {config_file}: {e}")
            print("Using default configuration")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain


def print_brain_status(brain: pynlm.Brain):
    """Print detailed brain status information."""
    print("\n=== NLM Brain Status ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"Pending spike events: {brain.getPendingSpikeEventCount()}")
    
    # Memory system status
    if brain.getWorkingMemory():
        print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
    if brain.getEpisodicMemory():
        print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    # Neuromodulation status
    if brain.getDopamine():
        print(f"Dopamine level: {brain.getDopamine().getLevel():.3f}")
    
    # Development status
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("=== End Brain Status ===\n")


def main():
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli simulate --steps 1000                    Run a simple brain simulation
  nlm-cli agent --width 30 --height 30             Run an agent in a world
  nlm-cli status --file brain_state.bin             Print brain status from saved state
  nlm-cli config --set "brain.neuron_count" 2000    Set configuration parameter
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Simulate command
    simulate_parser = subparsers.add_parser("simulate", help="Run a brain simulation")
    simulate_parser.add_argument("--steps", type=int, default=100,
                                help="Number of simulation steps (default: 100)")
    simulate_parser.add_argument("--config", help="Configuration file path")
    simulate_parser.add_argument("--output", help="Output file for brain state")
    
    # Agent command
    agent_parser = subparsers.add_parser("agent", help="Run an agent simulation")
    agent_parser.add_argument("--width", type=int, default=20, help="World width (default: 20)")
    agent_parser.add_argument("--height", type=int, default=20, help="World height (default: 20)")
    agent_parser.add_argument("--steps", type=int, default=500, help="Number of simulation steps (default: 500)")
    
    # Status command
    status_parser = subparsers.add_parser("status", help="Print brain status")
    status_parser.add_argument("--file", help="Load brain state from file before showing status")
    
    # Config command
    config_parser = subparsers.add_parser("config", help="Manage configuration")
    config_subparsers = config_parser.add_subparsers(dest="config_command", help="Configuration commands")
    
    config_list_parser = config_subparsers.add_parser("list", help="List configuration values")
    config_list_parser.add_argument("--file", help="Configuration file to load")
    
    config_set_parser = config_subparsers.add_parser("set", help="Set configuration value")
    config_set_parser.add_argument("key", help="Configuration key (e.g., 'brain.neuron_count')")
    config_set_parser.add_argument("value", help="Configuration value")
    config_set_parser.add_argument("--file", help="Configuration file to load first")
    
    config_get_parser = config_subparsers.add_parser("get", help="Get configuration value")
    config_get_parser.add_argument("key", help="Configuration key")
    config_get_parser.add_argument("--file", help="Configuration file to load")
    
    # Parse arguments
    args = parser.parse_args()
    
    if args.command == "simulate":
        brain = create_brain_from_config(args.config)
        
        print(f"Running simulation for {args.steps} steps...")
        for step in range(args.steps):
            brain.step(step)
            
            if (step + 1) % 100 == 0:
                print(f"Step {step + 1}/{args.steps}")
        
        print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
        
        if args.output:
            brain.save(args.output)
            print(f"Brain state saved to {args.output}")
    
    elif args.command == "agent":
        print("Creating agent simulation...")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(args.width, args.height, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(args.width // 2, args.height // 2)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print(f"Running agent simulation for {args.steps} steps...")
        
        for step in range(args.steps):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}")
        
        print("Agent simulation completed!")
        print(f"Final state - Firing neurons: {brain.getFiringNeuronCount()}, "
              f"Total spikes: {brain.getTotalSpikeCount()}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    elif args.command == "status":
        brain = create_brain_from_config()
        if args.file:
            try:
                brain.load(args.file)
                print(f"Loaded brain state from {args.file}")
            except Exception as e:
                print(f"Warning: Could not load brain state: {e}")
        print_brain_status(brain)
    
    elif args.command == "config":
        if not hasattr(args, 'config_command') or not args.config_command:
            parser.print_help()
            return
        
        config = pynlm.createDefaultConfig()
        
        if hasattr(args, 'file') and args.file:
            try:
                config.loadFromFile(args.file)
                print(f"Loaded configuration from {args.file}")
            except Exception as e:
                print(f"Error loading config file: {e}")
                return
        
        if args.config_command == "list":
            print("Current configuration:")
            keys = config.getKeys()
            for key in keys:
                value = config.get(key)
                if value:
                    print(f"  {key} = {value}")
                else:
                    print(f"  {key} = <not set>")
        
        elif args.config_command == "set":
            try:
                config.set(args.key, args.value)
                print(f"Set {args.key} = {args.value}")
            except Exception as e:
                print(f"Error setting configuration: {e}")
        
        elif args.config_command == "get":
            value = config.get(args.key)
            if value:
                print(f"{args.key} = {value}")
            else:
                print(f"{args.key} is not set")
    
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
