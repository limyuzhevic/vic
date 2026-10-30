#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

Advanced command-line interface for NLM (Neural Learning Machine) simulations.
Provides enhanced functionality beyond the basic Python API for advanced users.
"""

import sys
import argparse
import json
import time
import os
from pathlib import Path
from typing import Optional, List, Dict, Any
import logging

# Import the NLM modules
import pynlm
from python.nlm import NLMBrain, NLMEnvironment, create_simple_brain, create_agent_environment

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

class NLMCLI:
    """NLM Command Line Interface."""
    
    def __init__(self):
        self.current_brain = None
        self.current_world = None
        self.current_agent = None
        self.current_environment = None
        self.session_log = []
    
    def create_brain(self, args):
        """Create a new brain."""
        logger.info("Creating new brain...")
        
        # Get configuration from file or args
        config = None
        if args.config_file:
            config = self._load_config_from_file(args.config_file)
        
        # Override with command line args
        if args.neurons:
            config = config or {}
            config.setdefault("brain.neuron_count", args.neurons)
        
        if args.regions:
            config = config or {}
            config.setdefault("brain.region_count", args.regions)
        
        if args.conn_prob:
            config = config or {}
            config.setdefault("brain.connection_probability", args.conn_prob)
        
        # Create brain
        self.current_brain = NLMBrain(config, args.name)
        
        # Initialize if requested
        if args.auto_init:
            self.current_brain.initialize(
                neuron_count=args.neurons if args.neurons else None,
                region_count=args.regions if args.regions else None,
                connection_probability=args.conn_prob if args.conn_prob else None
            )
        
        logger.info(f"Brain '{args.name}' created successfully")
        logger.info(f"  Neurons: {self.current_brain._brain.getTotalNeuronCount() if self.current_brain._brain else 'Not initialized'}")
        
        return self.current_brain
    
    def create_world(self, args):
        """Create a new world."""
        logger.info("Creating new world...")
        
        self.current_world = pynlm.createSimpleWorld()
        self.current_world.configure(
            args.width, args.height, args.vision_width, args.vision_height
        )
        
        if args.agent_start_x is not None and args.agent_start_y is not None:
            self.current_world.setAgentStart(args.agent_start_x, args.agent_start_y)
        
        logger.info(f"World created: {args.width}x{args.height}, vision: {args.vision_width}x{args.vision_height}")
        
        return self.current_world
    
    def create_agent(self, args):
        """Create a new agent."""
        if not self.current_brain:
            logger.error("No brain created yet. Create a brain first.")
            return None
        
        logger.info("Creating agent...")
        
        self.current_agent = pynlm.createAgentBrain(self.current_brain._brain)
        self.current_agent.initialize(self.current_world._world if self.current_world else None)
        
        # Enable subsystems based on args
        if args.enable_reward:
            self.current_agent.enableRewardModulation(True)
        
        if args.enable_plasticity:
            self.current_agent.enableStructuralPlasticity(True)
        
        if args.enable_development:
            self.current_agent.enableDevelopment(True)
        
        if args.enable_curiosity:
            self.current_agent.enableCuriosity(True)
        
        logger.info("Agent created successfully")
        
        return self.current_agent
    
    def run_simulation(self, args):
        """Run a simulation."""
        if not self.current_brain:
            logger.error("No brain created yet. Create a brain first.")
            return
        
        logger.info(f"Starting simulation: {args.steps} steps")
        
        start_time = time.time()
        
        for step in range(args.steps):
            # Step the brain
            self.current_brain.step(1)
            
            # Update world if available
            if self.current_world and args.world_update:
                self.current_world.update(0.1)
            
            # Get agent action if available
            if self.current_agent and args.use_agent:
                # For CLI, we just show the stats
                pass
            
            # Log progress periodically
            if args.verbose and step % args.log_interval == 0:
                stats = self.current_brain.get_stats()
                logger.info(f"Step {step}: {stats['firing_neurons']} firing neurons, "
                           f"{stats['total_spikes']} total spikes")
        
        elapsed_time = time.time() - start_time
        logger.info(f"Simulation completed in {elapsed_time:.2f} seconds")
        
        # Save if requested
        if args.save:
            filename = args.save if args.save else f"simulation_step_{args.steps}.bin"
            self.current_brain.save(filename)
            logger.info(f"Simulation saved to: {filename}")
    
    def run_agent_episode(self, args):
        """Run an agent episode."""
        logger.info(f"Starting agent episode: {args.steps} steps")
        
        if not self.current_agent or not self.current_world:
            logger.error("Agent or world not created yet. Create both first.")
            return
        
        # Create environment for episode
        env = NLMEnvironment(
            brain_config={},
            world_config={},
            agent_config={}
        )
        
        # Override with current objects
        env.brain = self.current_brain
        env.agent = self.current_agent
        env.world = self.current_world
        
        # Run episode
        stats = env.run_episode(
            steps=args.steps,
            timestep=args.timestep
        )
        
        logger.info("Episode completed")
        logger.info(f"  Time steps: {stats['brain']['simulation_step']}")
        logger.info(f"  Total neurons: {stats['brain']['total_neurons']}")
        logger.info(f"  Total synapses: {stats['brain']['total_synapses']}")
        logger.info(f"  Average firing rate: {stats['brain']['average_firing_rate']:.2f}")
    
    def get_stats(self, _args):
        """Get current statistics."""
        if self.current_brain:
            stats = self.current_brain.get_stats()
            print(json.dumps(stats, indent=2))
        else:
            logger.error("No brain created yet.")
    
    def save_state(self, args):
        """Save current state."""
        if not self.current_brain:
            logger.error("No brain created yet.")
            return
        
        filename = args.file if args.file else f"nlm_state_{int(time.time())}.bin"
        
        if self.current_brain.save(filename):
            logger.info(f"State saved to: {filename}")
        else:
            logger.error(f"Failed to save state to: {filename}")
    
    def load_state(self, args):
        """Load state from file."""
        if not args.file:
            logger.error("No file specified.")
            return
        
        if not self.current_brain:
            logger.error("No brain created yet. Create a brain first.")
            return
        
        if self.current_brain.load(args.file):
            logger.info(f"State loaded from: {args.file}")
        else:
            logger.error(f"Failed to load state from: {args.file}")
    
    def list_configs(self, _args):
        """List available configuration presets."""
        from python.nlm import list_available_configs
        configs = list_available_configs()
        
        print("Available configuration presets:")
        for config in configs:
            print(f"  - {config}")
        
        print("\nExample usage:")
        from python.nlm import get_example_config
        print(f"  create_simple_brain(**get_example_config('default'))")
    
    def run_example(self, args):
        """Run a built-in example."""
        from python.nlm import get_example_config
        
        if args.example == "silent":
            logger.info("Running silent brain example...")
            brain = NLMBrain(get_example_config("minimal"), "silent_brain")
            brain.initialize()
            for i in range(10):
                brain.step(1)
            logger.info("Silent brain example completed")
        
        elif args.example == "watching":
            logger.info("Running world watching example...")
            brain = NLMBrain(get_example_config("default"), "watching_brain")
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain._brain)
            agent.initialize(world._world if hasattr(world, '_world') else world)
            
            for i in range(30):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(i)
            
            logger.info("World watching example completed")
    
    def _load_config_from_file(self, filepath: str) -> Dict[str, Any]:
        """Load configuration from JSON or INI file."""
        try:
            if filepath.endswith('.json'):
                with open(filepath, 'r') as f:
                    return json.load(f)
            else:
                # Simple INI-like parser
                config = {}
                with open(filepath, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if line and not line.startswith('#') and '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            config[key] = value
                return config
        except Exception as e:
            logger.error(f"Failed to load config file {filepath}: {e}")
            return {}
    
    def run_command(self, command: str, args: argparse.Namespace):
        """Run a CLI command."""
        commands = {
            'create-brain': self.create_brain,
            'create-world': self.create_world,
            'create-agent': self.create_agent,
            'run-simulation': self.run_simulation,
            'run-episode': self.run_agent_episode,
            'get-stats': self.get_stats,
            'save-state': self.save_state,
            'load-state': self.load_state,
            'list-configs': self.list_configs,
            'run-example': self.run_example
        }
        
        if command in commands:
            commands[command](args)
        else:
            logger.error(f"Unknown command: {command}")


def main():
    """Main CLI entry point."""
    cli = NLMCLI()
    
    # Create main parser
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface - Advanced NLM brain simulation",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Create and initialize a brain
  nlm create-brain --name mybrain --neurons 1000 --regions 2 --auto-init
  
  # Run a simulation
  nlm run-simulation --steps 500 --save simulation.bin
  
  # Create agent-environment
  nlm create-world --width 30 --height 30
  nlm create-agent --enable-reward --enable-curiosity
  nlm run-episode --steps 200
  
  # Load and save states
  nlm load-state --file brain_state.bin
  nlm save-state --file brain_state.bin
  
  # View statistics
  nlm get-stats
        ""
    )
    
    # Subparsers for commands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create brain command
    brain_parser = subparsers.add_parser('create-brain', help='Create a new brain')
    brain_parser.add_argument('--name', default='brain', help='Brain name (default: brain)')
    brain_parser.add_argument('--neurons', type=int, help='Number of neurons')
    brain_parser.add_argument('--regions', type=int, help='Number of regions')
    brain_parser.add_argument('--conn-prob', type=float, help='Connection probability')
    brain_parser.add_argument('--config-file', help='Configuration file (JSON or INI format)')
    brain_parser.add_argument('--auto-init', action='store_true', help='Automatically initialize brain')
    
    # Create world command
    world_parser = subparsers.add_parser('create-world', help='Create a new world')
    world_parser.add_argument('--width', type=int, default=20, help='World width (default: 20)')
    world_parser.add_argument('--height', type=int, default=20, help='World height (default: 20)')
    world_parser.add_argument('--vision-width', type=int, default=8, help='Vision sensor width (default: 8)')
    world_parser.add_argument('--vision-height', type=int, default=8, help='Vision sensor height (default: 8)')
    world_parser.add_argument('--agent-start-x', type=float, help='Agent starting X position')
    world_parser.add_argument('--agent-start-y', type=float, help='Agent starting Y position')
    
    # Create agent command
    agent_parser = subparsers.add_parser('create-agent', help='Create an agent')
    agent_parser.add_argument('--enable-reward', action='store_true', help='Enable reward modulation')
    agent_parser.add_argument('--enable-plasticity', action='store_true', help='Enable structural plasticity')
    agent_parser.add_argument('--enable-development', action='store_true', help='Enable development')
    agent_parser.add_argument('--enable-curiosity', action='store_true', help='Enable curiosity')
    
    # Run simulation command
    sim_parser = subparsers.add_parser('run-simulation', help='Run a brain simulation')
    sim_parser.add_argument('--steps', type=int, default=100, help='Number of steps (default: 100)')
    sim_parser.add_argument('--save', help='Save file path')
    sim_parser.add_argument('--verbose', action='store_true', help='Verbose output')
    sim_parser.add_argument('--log-interval', type=int, default=50, help='Log interval (default: 50)')
    sim_parser.add_argument('--world-update', action='store_true', help='Update world during simulation')
    sim_parser.add_argument('--use-agent', action='store_true', help='Use agent for actions')
    
    # Run episode command
    episode_parser = subparsers.add_parser('run-episode', help='Run an agent episode')
    episode_parser.add_argument('--steps', type=int, default=100, help='Number of steps (default: 100)')
    episode_parser.add_argument('--timestep', type=float, default=0.1, help='Time step (default: 0.1)')
    
    # Get stats command
    subparsers.add_parser('get-stats', help='Get current statistics')
    
    # Save state command
    save_parser = subparsers.add_parser('save-state', help='Save current state')
    save_parser.add_argument('--file', help='Output file path')
    
    # Load state command
    load_parser = subparsers.add_parser('load-state', help='Load state from file')
    load_parser.add_argument('--file', required=True, help='Input file path')
    
    # List configs command
    subparsers.add_parser('list-configs', help='List available configuration presets')
    
    # Run example command
    example_parser = subparsers.add_parser('run-example', help='Run a built-in example')
    example_parser.add_argument('--example', choices=['silent', 'watching'], default='silent', help='Example to run (default: silent)')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Run command
    if args.command:
        cli.run_command(args.command, args)
    else:
        parser.print_help()


if __name__ == '__main__':
    main()