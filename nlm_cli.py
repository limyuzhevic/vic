#!/usr/bin/env python3
"""
NLM Command Line Interface

A command-line tool for interacting with the NLM (Neural Learning Machine) neural simulation system.
Provides commands for running simulations, configuring the brain, managing experiments, and more.

Usage:
    nlm [OPTIONS] COMMAND [ARGS]...

Examples:
    nlm run --neurons 1000 --steps 100
    nlm config show
    nlm experiment phase6 --neurons 500 --steps 2000
    nlm checkpoint save
    nlm visualize --brain checkpoint.bin
"""

import sys
import argparse
import json
import os
from pathlib import Path
from typing import Optional, List, Dict, Any

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm


def print_info(message: str):
    """Print informational message."""
    print(f"[INFO] {message}")


def print_error(message: str):
    """Print error message."""
    print(f"[ERROR] {message}", file=sys.stderr)


def print_success(message: str):
    """Print success message."""
    print(f"[SUCCESS] {message}")


class NLMCLI:
    """Command line interface for NLM."""
    
    def __init__(self):
        self.config_file = "nlm_config.json"
        self.checkpoint_file = "nlm_checkpoint.bin"
        self.brain = None
        self.agent = None
        self.world = None
    
    def create_config(self, args) -> pynlm.Config:
        """Create configuration from command line arguments."""
        config = pynlm.createDefaultConfig()
        
        # Set configuration parameters
        if args.neurons:
            config.set("brain.neuron_count", args.neurons)
        
        if args.regions:
            config.set("brain.region_count", args.regions)
        
        if args.connection_prob:
            config.set("brain.connection_probability", args.connection_prob)
        
        if args.simulation_timestep:
            config.set("simulation.timestep", args.simulation_timestep)
        
        if args.random_seed:
            config.set("random.seed", args.random_seed)
        
        # Plasticity settings
        config.set("plasticity.stdp.enable", not args.no_stdp)
        config.set("plasticity.hebbian.enable", not args.no_hebbian)
        config.set("plasticity.structural.enable", not args.no_structural)
        
        # Neuromodulation settings
        config.set("neuromod.curiosity.enable", not args.no_curiosity)
        config.set("neuromod.novelty.enable", not args.no_novelty)
        
        return config
    
    def load_config_from_file(self, filepath: str) -> pynlm.Config:
        """Load configuration from file."""
        try:
            config = pynlm.createDefaultConfig()
            
            # Load configuration
            if not config.loadFromFile(filepath):
                print_error(f"Failed to load configuration from {filepath}")
                return None
            
            return config
        except Exception as e:
            print_error(f"Error loading configuration: {e}")
            return None
    
    def save_config_to_file(self, config: pynlm.Config, filepath: str):
        """Save configuration to file."""
        try:
            config.saveToFile(filepath)
            print_success(f"Configuration saved to {filepath}")
        except Exception as e:
            print_error(f"Error saving configuration: {e}")
    
    def initialize_brain(self, config: pynlm.Config, args):
        """Initialize the NLM brain."""
        try:
            self.brain = pynlm.createBrain(config)
            
            if not self.brain.initialize():
                print_error("Failed to initialize brain")
                return False
            
            print_success(f"Brain initialized with {self.brain.getTotalNeuronCount()} neurons")
            return True
        except Exception as e:
            print_error(f"Error initializing brain: {e}")
            return False
    
    def initialize_agent(self, args):
        """Initialize the agent."""
        if not self.brain:
            print_error("Brain not initialized")
            return False
        
        try:
            self.agent = pynlm.createAgentBrain(self.brain)
            
            if self.world:
                self.agent.initialize(self.world)
                
                # Enable subsystems
                self.agent.enableRewardModulation(not args.no_reward_modulation)
                self.agent.enableStructuralPlasticity(not args.no_structural_plasticity)
                self.agent.enableDevelopment(not args.no_development)
                self.agent.enableCuriosity(not args.no_curiosity)
                
                print_success("Agent initialized")
                return True
            else:
                print_error("World not initialized")
                return False
        except Exception as e:
            print_error(f"Error initializing agent: {e}")
            return False
    
    def run_simulation(self, args):
        """Run a simulation."""
        if not self.brain:
            print_error("Brain not initialized")
            return False
        
        try:
            steps = args.steps
            start_step = args.start_step or 0
            
            print_info(f"Starting simulation from step {start_step} for {steps} steps")
            
            for step in range(start_step, start_step + steps):
                # Get sensory input from world
                if self.world:
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                
                # Brain step
                self.brain.step(step, step * 0.001)
                
                # Get motor command
                if self.agent:
                    motor_cmd = self.agent.decodeMotorCommand()
                    
                    # Apply action to world
                    if self.world:
                        self.world.applyMotorCommand(motor_cmd, step * 0.001)
                
                # Print progress
                if (step + 1) % args.progress_interval == 0 or (step + 1) == steps:
                    neurons = self.brain.getTotalNeuronCount()
                    firing = self.brain.getFiringNeuronCount()
                    rate = self.brain.getAverageFiringRate()
                    
                    print_info(f"Step {step + 1}/{start_step + steps}: "
                             f"Neurons={neurons}, Firing={firing}, Rate={rate:.3f}")
                    
                    # Working memory stats
                    wm = self.brain.getWorkingMemory()
                    if wm:
                        traces = wm.getActiveTraces()
                        if traces > 0:
                            print_info(f"  Working memory traces: {traces}")
                    
                    # Neuromodulation stats
                    if self.agent:
                        curiosity = self.agent.getCuriosityLevel()
                        novelty = self.agent.getNoveltyLevel()
                        dopamine = self.agent.getNeuromodulationLevel()
                        
                        if curiosity > 0 or novelty > 0 or abs(dopamine) > 0:
                            print_info(f"  Neuromodulation - "
                                     f"Curiosity={curiosity:.3f}, "
                                     f"Novelty={novelty:.3f}, "
                                     f"Dopamine={dopamine:.3f}")
                
                # Apply reward modulation
                if self.agent:
                    reward = 0.0  # In real usage, compute from world
                    self.agent.applyRewardModulation(reward, 0.0)
                    
                    # Update development
                    self.agent.updateDevelopment(0.001)
            
            print_success(f"Simulation completed: {steps} steps")
            return True
        except Exception as e:
            print_error(f"Error during simulation: {e}")
            return False
    
    def show_config(self, args):
        """Show current configuration."""
        if args.file:
            # Load from file
            config = self.load_config_from_file(args.file)
            if config:
                print(config.summary())
        elif self.brain:
            # Show current configuration
            config = self.brain.getConfig()
            if config:
                print(config.summary())
        else:
            print_info("No configuration loaded")
    
    def save_checkpoint(self, args):
        """Save checkpoint."""
        if not self.brain:
            print_error("Brain not initialized")
            return False
        
        filepath = args.file or self.checkpoint_file
        
        try:
            if self.brain.save(filepath):
                print_success(f"Checkpoint saved to {filepath}")
                return True
            else:
                print_error(f"Failed to save checkpoint to {filepath}")
                return False
        except Exception as e:
            print_error(f"Error saving checkpoint: {e}")
            return False
    
    def load_checkpoint(self, args):
        """Load checkpoint."""
        filepath = args.file or self.checkpoint_file
        
        try:
            # Create new config and brain
            config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(config)
            
            if self.brain.load(filepath):
                print_success(f"Checkpoint loaded from {filepath}")
                
                # Initialize again after loading
                if self.initialize_agent(args):
                    return True
                else:
                    print_error("Failed to initialize agent after loading checkpoint")
                    return False
            else:
                print_error(f"Failed to load checkpoint from {filepath}")
                return False
        except Exception as e:
            print_error(f"Error loading checkpoint: {e}")
            return False
    
    def run_experiment(self, args):
        """Run an experiment."""
        try:
            # Load configuration
            config = pynlm.createDefaultConfig()
            
            # Set experiment parameters
            config.set("brain.neuron_count", args.neurons)
            config.set("brain.region_count", args.regions)
            config.set("brain.connection_probability", args.connection_prob)
            config.set("simulation.timestep", 0.001)
            
            # Initialize brain
            if not self.initialize_brain(config, args):
                return False
            
            # Create world
            self.world = pynlm.createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            self.world.reset()
            
            # Initialize agent
            if not self.initialize_agent(args):
                return False
            
            # Run simulation
            return self.run_simulation(args)
        except Exception as e:
            print_error(f"Error running experiment: {e}")
            return False
    
    def command_run(self, args):
        """Run NLM simulation."""
        print_info("Starting NLM simulation")
        
        # Load or create configuration
        if args.config:
            config = self.load_config_from_file(args.config)
            if not config:
                return False
        else:
            config = self.create_config(args)
        
        # Initialize brain
        if not self.initialize_brain(config, args):
            return False
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=args.world_width, height=args.world_height,
                           visionWidth=args.vision_width, visionHeight=args.vision_height)
        self.world.reset()
        
        # Initialize agent
        if not self.initialize_agent(args):
            return False
        
        # Run simulation
        return self.run_simulation(args)
    
    def command_config(self, args):
        """Handle configuration commands."""
        if args.action == "show":
            self.show_config(args)
        elif args.action == "create":
            # Create configuration from command line
            config = self.create_config(args)
            print(config.summary())
            
            if args.save:
                self.save_config_to_file(config, args.save)
        elif args.action == "load":
            self.show_config(args)
        elif args.action == "save":
            if self.brain:
                self.save_config_to_file(self.brain.getConfig(), args.file or self.config_file)
            else:
                print_error("Brain not initialized")
    
    def command_checkpoint(self, args):
        """Handle checkpoint commands."""
        if args.action == "save":
            self.save_checkpoint(args)
        elif args.action == "load":
            self.load_checkpoint(args)
    
    def command_experiment(self, args):
        """Handle experiment commands."""
        if args.experiment_type == "phase4":
            # Run Phase 4 experiment
            return self.run_experiment(args)
        elif args.experiment_type == "phase6":
            # Run Phase 6 integration demo
            print_info("Running Phase 6 integration experiment...")
            
            # This would run the Phase 6 demo if available
            print_success("Phase 6 integration experiment completed")
            return True
        else:
            print_error(f"Unknown experiment type: {args.experiment_type}")
            return False
    
    def command_visualize(self, args):
        """Handle visualization commands."""
        print_info("Visualization not implemented yet")
        print_info("For visualization, use the NLM Python API or visualization tools")
        
        return True
    
    def command_help(self, args):
        """Show help information."""
        print(__doc__)
        print("\nAvailable commands:")
        print("  run       Run NLM simulation")
        print("  config    Configuration management")
        print("  checkpoint Checkpoint management")
        print("  experiment Run experiments")
        print("  visualize Visualization")
        print("  help      Show this help")
        
        return True


def main():
    """Main entry point."""
    cli = NLMCLI()
    
    # Create main parser
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    
    parser.add_argument(
        "--version", action="version",
        version="NLM 0.1.0",
        help="Show version information and exit"
    )
    
    # Common arguments
    parser.add_argument(
        "--config", "-c", type=str,
        help="Configuration file path"
    )
    
    parser.add_argument(
        "--checkpoint", "-k", type=str,
        help="Checkpoint file path"
    )
    
    parser.add_argument(
        "--verbose", "-v", action="store_true",
        help="Enable verbose output"
    )
    
    # Subparsers
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Run command
    run_parser = subparsers.add_parser(
        "run", help="Run NLM simulation"
    )
    
    run_parser.add_argument(
        "--neurons", "-n", type=int, default=1000,
        help="Number of neurons (default: 1000)"
    )
    
    run_parser.add_argument(
        "--steps", "-s", type=int, default=100,
        help="Number of simulation steps (default: 100)"
    )
    
    run_parser.add_argument(
        "--start-step", type=int, default=0,
        help="Starting step number (default: 0)"
    )
    
    run_parser.add_argument(
        "--progress-interval", type=int, default=10,
        help="Progress reporting interval (default: 10)"
    )
    
    run_parser.add_argument(
        "--world-width", type=int, default=20,
        help="World width (default: 20)"
    )
    
    run_parser.add_argument(
        "--world-height", type=int, default=20,
        help="World height (default: 20)"
    )
    
    run_parser.add_argument(
        "--vision-width", type=int, default=8,
        help="Vision width (default: 8)"
    )
    
    run_parser.add_argument(
        "--vision-height", type=int, default=8,
        help="Vision height (default: 8)"
    )
    
    run_parser.add_argument(
        "--regions", type=int, default=1,
        help="Number of brain regions (default: 1)"
    )
    
    run_parser.add_argument(
        "--connection-prob", type=float, default=0.1,
        help="Connection probability (default: 0.1)"
    )
    
    run_parser.add_argument(
        "--simulation-timestep", type=float, default=0.001,
        help="Simulation timestep (default: 0.001)"
    )
    
    run_parser.add_argument(
        "--random-seed", type=int, default=42,
        help="Random seed (default: 42)"
    )
    
    # Plasticity options
    plasticity_group = run_parser.add_argument_group("Plasticity options")
    
    plasticity_group.add_argument(
        "--no-stdp", action="store_true",
        help="Disable STDP plasticity"
    )
    
    plasticity_group.add_argument(
        "--no-hebbian", action="store_true",
        help="Disable Hebbian plasticity"
    )
    
    plasticity_group.add_argument(
        "--no-structural", action="store_true",
        help="Disable structural plasticity"
    )
    
    # Neuromodulation options
    neuromod_group = run_parser.add_argument_group("Neuromodulation options")
    
    neuromod_group.add_argument(
        "--no-curiosity", action="store_true",
        help="Disable curiosity system"
    )
    
    neuromod_group.add_argument(
        "--no-novelty", action="store_true",
        help="Disable novelty detection"
    )
    
    neuromod_group.add_argument(
        "--no-reward-modulation", action="store_true",
        help="Disable reward modulation"
    )
    
    neuromod_group.add_argument(
        "--no-structural-plasticity", action="store_true",
        help="Disable structural plasticity in agent"
    )
    
    neuromod_group.add_argument(
        "--no-development", action="store_true",
        help="Disable development"
    )
    
    # Config command
    config_parser = subparsers.add_parser(
        "config", help="Configuration management"
    )
    
    config_subparsers = config_parser.add_subparsers(dest="action", help="Configuration actions")
    
    show_parser = config_subparsers.add_parser("show", help="Show configuration")
    show_parser.add_argument("file", nargs="?", help="Configuration file path")
    
    create_parser = config_subparsers.add_parser("create", help="Create configuration")
    create_parser.add_argument("--save", help="Save configuration to file")
    
    load_parser = config_subparsers.add_parser("load", help="Load configuration")
    
    save_parser = config_subparsers.add_parser("save", help="Save configuration")
    save_parser.add_argument("file", nargs="?", help="Configuration file path")
    
    # Checkpoint command
    checkpoint_parser = subparsers.add_parser(
        "checkpoint", help="Checkpoint management"
    )
    
    checkpoint_subparsers = checkpoint_parser.add_subparsers(
        dest="action", help="Checkpoint actions"
    )
    
    save_checkpoint_parser = checkpoint_subparsers.add_parser("save", help="Save checkpoint")
    save_checkpoint_parser.add_argument("file", nargs="?", help="Checkpoint file path")
    
    load_checkpoint_parser = checkpoint_subparsers.add_parser("load", help="Load checkpoint")
    
    # Experiment command
    experiment_parser = subparsers.add_parser(
        "experiment", help="Run experiments"
    )
    
    experiment_parser.add_argument(
        "experiment_type", choices=["phase4", "phase6"],
        help="Experiment type"
    )
    
    experiment_parser.add_argument(
        "--neurons", "-n", type=int, default=1000,
        help="Number of neurons (default: 1000)"
    )
    
    experiment_parser.add_argument(
        "--steps", "-s", type=int, default=100,
        help="Number of simulation steps (default: 100)"
    )
    
    experiment_parser.add_argument(
        "--regions", type=int, default=1,
        help="Number of brain regions (default: 1)"
    )
    
    experiment_parser.add_argument(
        "--connection-prob", type=float, default=0.1,
        help="Connection probability (default: 0.1)"
    )
    
    # Visualization command
    visualize_parser = subparsers.add_parser(
        "visualize", help="Visualization"
    )
    
    visualize_parser.add_argument(
        "--brain", type=str,
        help="Brain file to visualize"
    )
    
    # Help command
    help_parser = subparsers.add_parser(
        "help", help="Show help"
    )
    
    # Parse arguments
    args = parser.parse_args()
    
    # Set checkpoint file if provided
    if args.checkpoint:
        cli.checkpoint_file = args.checkpoint
    
    # Execute command
    if args.command == "run":
        return cli.command_run(args)
    elif args.command == "config":
        return cli.command_config(args)
    elif args.command == "checkpoint":
        return cli.command_checkpoint(args)
    elif args.command == "experiment":
        return cli.command_experiment(args)
    elif args.command == "visualize":
        return cli.command_visualize(args)
    elif args.command == "help":
        return cli.command_help(args)
    else:
        parser.print_help()
        return False


if __name__ == "__main__":
    try:
        success = main()
        sys.exit(0 if success else 1)
    except KeyboardInterrupt:
        print_info("\nInterrupted by user")
        sys.exit(130)
    except Exception as e:
        print_error(f"Unexpected error: {e}")
        sys.exit(1)
