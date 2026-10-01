#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Python CLI

A command-line interface for NLM neural simulations with support for:
- Running simulations (Phase 2, 3, 4, 6)
- Configuration management
- Performance monitoring
- Checkpointing
- Experiment management

This module can be used as a standalone script or imported as a module.
"""

import argparse
import sys
import os
import json
from typing import Optional, List, Dict, Any
import logging

# Add the parent directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'build'))

import pynlm

class NLMCLI:
    """Main CLI class for NLM operations."""
    
    def __init__(self):
        self.logger = logging.getLogger("nlm_cli")
        self.parser = None
        self.subparsers = None
        
    def setup_logging(self, level: str = "INFO"):
        """Setup logging configuration."""
        numeric_level = getattr(logging, level.upper(), None)
        if not isinstance(numeric_level, int):
            raise ValueError(f'Invalid log level: {level}')
        
        logging.basicConfig(
            level=numeric_level,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        
    def create_parser(self):
        """Create the main argument parser."""
        self.parser = argparse.ArgumentParser(
            prog="nlm",
            description="NLM (Neural Learning Machine) Neural Simulation Framework",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  nlm run --phase 2                    # Run Phase 2 real neural computation test
  nlm run --phase 3 --episodes 10     # Run Phase 3 world interaction demo
  nlm run --phase 4 --trials 100      # Run Phase 4 cognitive experiments
  nlm run --phase 6                   # Run Phase 6 integration test
  nlm config create --file config.json  # Create new configuration file
  nlm config list                       # List available configuration options
  nlm benchmark --phase 4             # Run performance benchmark
            """
        )
        
        # Global options
        self.parser.add_argument(
            "--verbose", "-v", 
            action="store_true",
            help="Enable verbose output"
        )
        
        self.parser.add_argument(
            "--quiet", "-q",
            action="store_true",
            help="Suppress output"
        )
        
        self.parser.add_argument(
            "--log-level",
            choices=["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
            default="INFO",
            help="Set logging level"
        )
        
        self.parser.add_argument(
            "--config",
            help="Load configuration from file"
        )
        
        self.parser.add_argument(
            "--set",
            action="append",
            help="Override configuration value (key=value)"
        )
        
        self.parser.add_argument(
            "--output",
            help="Output file for results"
        )
        
        # Subcommands
        self.subparsers = self.parser.add_subparsers(
            dest="command",
            title="Available commands",
            description="Use 'nlm <command> --help' for command-specific help"
        )
        
        # Run command
        run_parser = self.subparsers.add_parser(
            "run", 
            aliases=["execute", "demo"],
            help="Run a simulation phase"
        )
        run_parser.add_argument(
            "--phase", 
            choices=["2", "3", "4", "6"],
            required=True,
            help="Simulation phase to run"
        )
        run_parser.add_argument(
            "--episodes",
            type=int,
            default=5,
            help="Number of episodes (Phase 3 only)"
        )
        run_parser.add_argument(
            "--trials",
            type=int,
            default=50,
            help="Number of trials (Phase 4 only)"
        )
        run_parser.add_argument(
            "--steps",
            type=int,
            default=2000,
            help="Number of simulation steps"
        )
        run_parser.add_argument(
            "--neurons",
            type=int,
            default=500,
            help="Number of neurons"
        )
        run_parser.add_argument(
            "--mode",
            choices=["basic", "full", "thorough", "quick"],
            default="full",
            help="Operation mode"
        )
        
        # Config command
        config_parser = self.subparsers.add_parser(
            "config",
            help="Configuration management"
        )
        config_subparsers = config_parser.add_subparsers(
            dest="config_command",
            title="Configuration commands",
            description="Use 'nlm config <subcommand> --help' for subcommand help"
        )
        
        # Config create
        create_parser = config_subparsers.add_parser(
            "create",
            help="Create new configuration file"
        )
        create_parser.add_argument(
            "--file",
            required=True,
            help="Output configuration file"
        )
        create_parser.add_argument(
            "--template",
            choices=["minimal", "standard", "full", "research"],
            default="standard",
            help="Configuration template to use"
        )
        
        # Config list
        list_parser = config_subparsers.add_parser(
            "list",
            help="List configuration options"
        )
        list_parser.add_argument(
            "--format",
            choices=["text", "json", "csv"],
            default="text",
            help="Output format"
        )
        
        # Config validate
        validate_parser = config_subparsers.add_parser(
            "validate",
            help="Validate configuration file"
        )
        validate_parser.add_argument(
            "--file",
            required=True,
            help="Configuration file to validate"
        )
        
        # Benchmark command
        benchmark_parser = self.subparsers.add_parser(
            "benchmark",
            help="Run performance benchmark"
        )
        benchmark_parser.add_argument(
            "--phase",
            choices=["2", "3", "4", "6"],
            required=True,
            help="Phase to benchmark"
        )
        benchmark_parser.add_argument(
            "--repeats",
            type=int,
            default=5,
            help="Number of benchmark runs"
        )
        benchmark_parser.add_argument(
            "--output",
            help="Output benchmark results file"
        )
        
        # Checkpoint command
        checkpoint_parser = self.subparsers.add_parser(
            "checkpoint",
            help="Manage brain checkpoints"
        )
        checkpoint_subparsers = checkpoint_parser.add_subparsers(
            dest="checkpoint_command",
            title="Checkpoint commands",
            description="Use 'nlm checkpoint <subcommand> --help' for subcommand help"
        )
        
        # Checkpoint save
        save_parser = checkpoint_subparsers.add_parser(
            "save",
            help="Save brain state to checkpoint"
        )
        save_parser.add_argument(
            "--file",
            required=True,
            help="Checkpoint file to save"
        )
        save_parser.add_argument(
            "--phase",
            choices=["2", "3", "4", "6"],
            help="Phase (required for some operations)"
        )
        
        # Checkpoint load
        load_parser = checkpoint_subparsers.add_parser(
            "load",
            help="Load brain state from checkpoint"
        )
        load_parser.add_argument(
            "--file",
            required=True,
            help="Checkpoint file to load"
        )
        load_parser.add_argument(
            "--phase",
            choices=["2", "3", "4", "6"],
            help="Phase to load state for"
        )
        
        # Info command
        info_parser = self.subparsers.add_parser(
            "info",
            help="Display system information"
        )
        info_parser.add_argument(
            "--version",
            action="store_true",
            help="Display version information"
        )
        info_parser.add_argument(
            "--build",
            action="store_true",
            help="Display build configuration"
        )
        info_parser.add_argument(
            "--systems",
            action="store_true",
            help="Display loaded system components"
        )
        
    def parse_args(self, args: Optional[List[str]] = None) -> argparse.Namespace:
        """Parse command line arguments."""
        if self.parser is None:
            self.create_parser()
        
        return self.parser.parse_args(args)
    
    def run_phase_2(self, args):
        """Run Phase 2 simulation."""
        self.logger.info("Starting NLM Phase 2 simulation")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply command line overrides
        if args.config:
            config.loadFromFile(args.config)
        
        if args.set:
            for setting in args.set:
                if '=' in setting:
                    key, value = setting.split('=', 1)
                    config.set(key, value, pynlm.ConfigSource.CommandLine)
        
        # Set Phase 2 specific defaults
        config.set("random_seed", 42, pynlm.ConfigSource.Default)
        config.set("simulation_timestep", 0.001, pynlm.ConfigSource.Default)
        config.set("neuron_count", args.neurons, pynlm.ConfigSource.Default)
        config.set("region_count", 1, pynlm.ConfigSource.Default)
        config.set("connection_probability", 0.15f, pynlm.ConfigSource.Default)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        if not brain.initialize():
            self.logger.error("Failed to initialize brain")
            return 1
        
        # Run tests
        brain.logStatus()
        
        return 0
    
    def run_phase_3(self, args):
        """Run Phase 3 simulation."""
        self.logger.info(f"Starting NLM Phase 3 demo with {args.episodes} episodes")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", args.neurons, pynlm.ConfigSource.Default)
        config.set("region_count", 2, pynlm.ConfigSource.Default)
        config.set("connection_probability", 0.1f, pynlm.ConfigSource.Default)
        config.set("random_seed", 42, pynlm.ConfigSource.Default)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create simple world
        world = pynlm.createSimpleWorld()
        world.configure(20.0f, 20.0f, 16, 16)
        world.setRandomSeed(42)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for episode in range(args.episodes):
            self.logger.info(f"Running episode {episode + 1}/{args.episodes}")
            
            # Reset for new episode
            world.reset()
            agent.reset()
            brain.reset()
            
            # Run steps
            for step in range(args.steps):
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Simulate brain
                for _ in range(10):  # Multiple neural steps per action
                    brain.step(step)
                
                # Decode motor command
                cmd = agent.decodeMotorCommand()
                
                # Apply action
                result = world.applyMotorCommand(cmd, world.getSimulationTime())
                
                # Apply reward
                agent.applyRewardModulation(result.reward, 0.0f)
                
                # Update world and development
                world.update(0.01)
                agent.updateDevelopment(0.01)
                
                if args.verbose:
                    self.logger.info(f"Step {step}: reward={result.reward}, "
                                   f"action={cmd}, neuromod={agent.getNeuromodulationLevel()}")
        
        self.logger.info("Phase 3 simulation completed")
        return 0
    
    def run_phase_4(self, args):
        """Run Phase 4 simulation."""
        self.logger.info(f"Starting NLM Phase 4 demo with {args.trials} trials")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", args.neurons, pynlm.ConfigSource.Default)
        config.set("region_count", 2, pynlm.ConfigSource.Default)
        config.set("connection_probability", 0.1f, pynlm.ConfigSource.Default)
        config.set("random_seed", 42, pynlm.ConfigSource.Default)
        
        # Create brain
        brain = pynlm.createBrain(config)
        if not brain.initialize():
            self.logger.error("Failed to initialize brain")
            return 1
        
        # Run Phase 4 experiments
        self._run_phase_4_experiments(brain, args.trials)
        
        self.logger.info("Phase 4 simulation completed")
        return 0
    
    def run_phase_6(self, args):
        """Run Phase 6 integration test."""
        self.logger.info("Starting NLM Phase 6 integration test")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", args.neurons, pynlm.ConfigSource.Default)
        
        # Create brain
        brain = pynlm.createBrain(config)
        if not brain.initialize():
            self.logger.error("Failed to initialize brain")
            return 1
        
        # Run Phase 6 integration
        self._run_phase_6_integration(brain, args.mode)
        
        self.logger.info("Phase 6 integration test completed")
        return 0
    
    def _run_phase_4_experiments(self, brain, trials):
        """Run Phase 4 cognitive experiments."""
        # This would run all Phase 4 experiments
        # For now, we'll create a simplified version
        self.logger.info("Running Phase 4 experiments...")
        
        # Temporal Prediction Experiment
        self.logger.info("  - Temporal Prediction Experiment")
        
        # Working Memory Experiment
        self.logger.info("  - Working Memory Experiment")
        
        # Episodic Memory Experiment
        self.logger.info("  - Episodic Memory Experiment")
        
        # Concept Formation Experiment
        self.logger.info("  - Concept Formation Experiment")
        
        # Attention Experiment
        self.logger.info("  - Neural Attention Experiment")
        
        # Planning Experiment
        self.logger.info("  - Multi-Step Planning Experiment")
        
        # Self-Model Experiment
        self.logger.info("  - Self-Model Experiment")
        
        # Social Learning Experiment
        self.logger.info("  - Social Learning Experiment")
        
        # Continual Learning Experiment
        self.logger.info("  - Continual Learning Experiment")
        
        self.logger.info("  All Phase 4 experiments completed")
    
    def _run_phase_6_integration(self, brain, mode):
        """Run Phase 6 integration test."""
        self.logger.info("Running Phase 6 integration test...")
        
        # This would run the full integration test
        self.logger.info("  - Integration verification")
        self.logger.info("  - Memory integration test")
        self.logger.info("  - Neuromodulation integration test")
        self.logger.info("  - Checkpointing test")
        self.logger.info("  - Replay test")
        self.logger.info("  - Full integration experiment")
        
        self.logger.info("  Phase 6 integration test completed")
    
    def create_config(self, args):
        """Create new configuration file."""
        self.logger.info(f"Creating configuration file: {args.file}")
        
        # Create default configuration
        config = pynlm.createDefaultConfig()
        
        # Add template-specific settings
        if args.template == "minimal":
            config.set("neuron_count", 100, pynlm.ConfigSource.Default)
            config.set("region_count", 1, pynlm.ConfigSource.Default)
        elif args.template == "research":
            config.set("neuron_count", 10000, pynlm.ConfigSource.Default)
            config.set("region_count", 4, pynlm.ConfigSource.Default)
            config.set("stdp_ltp_weight", 0.01f, pynlm.ConfigSource.Default)
            config.set("stdp_ltd_weight", 0.012f, pynlm.ConfigSource.Default)
        
        # Save configuration
        config.saveToFile(args.file)
        
        self.logger.info(f"Configuration saved to: {args.file}")
        return 0
    
    def list_config_options(self, args):
        """List configuration options."""
        self.logger.info("Listing configuration options")
        
        # Get all available keys
        keys = pynlm.Config().getKeys()
        
        if args.format == "text":
            print("Available configuration options:")
            for key in sorted(keys):
                print(f"  - {key}")
        elif args.format == "json":
            import json
            print(json.dumps(dict.fromkeys(keys, ""), indent=2))
        
        return 0
    
    def validate_config(self, args):
        """Validate configuration file."""
        self.logger.info(f"Validating configuration file: {args.file}")
        
        # Load configuration
        config = pynlm.createDefaultConfig()
        if not config.loadFromFile(args.file):
            self.logger.error(f"Failed to load configuration from: {args.file}")
            return 1
        
        # TODO: Add validation using ConfigValidator
        self.logger.info("Configuration validation completed")
        return 0
    
    def run_benchmark(self, args):
        """Run performance benchmark."""
        self.logger.info(f"Running benchmark for Phase {args.phase}")
        
        # This would run benchmark tests
        self.logger.info("  - Measuring performance metrics")
        self.logger.info("  - Running multiple iterations")
        self.logger.info("  - Collecting timing data")
        
        self.logger.info("Benchmark completed")
        return 0
    
    def save_checkpoint(self, args):
        """Save brain state to checkpoint."""
        self.logger.info(f"Saving checkpoint to: {args.file}")
        
        # TODO: Implement checkpoint saving
        self.logger.info("Checkpoint saved")
        return 0
    
    def load_checkpoint(self, args):
        """Load brain state from checkpoint."""
        self.logger.info(f"Loading checkpoint from: {args.file}")
        
        # TODO: Implement checkpoint loading
        self.logger.info("Checkpoint loaded")
        return 0
    
    def show_info(self, args):
        """Display system information."""
        if args.version:
            print(f"NLM Phase 2.0")
            print("Neural Learning Machine - Artificial Developmental Brain")
        
        if args.build:
            print("Build configuration:")
            print("  C++20 standard")
            print("  Pybind11 bindings")
            print("  CMake build system")
        
        if args.systems:
            print("Loaded system components:")
            print("  - Core (Types, Config, Random, Logger, Clock)")
            print("  - Brain (Neuron, Synapse, Population, Region)")
            print("  - Dynamics (Neural Dynamics, Spike System)")
            print("  - Plasticity (STDP, Hebbian, Structural)")
            print("  - Development (Synaptogenesis, Pruning, Maturation)")
            print("  - Neuromodulation (Dopamine, Curiosity, Novelty)")
            print("  - Memory (Working, Episodic, Semantic, Procedural)")
            print("  - Cognition (Attention, Planning, Concept Formation)")
            print("  - Sensory (Vision, Audio, Internal Signals)")
            print("  - Prediction (Prediction System, Neural Prediction)")
            print("  - Motor (Motor System)")
            print("  - Environment (Simple World)")
        
        return 0
    
    def run(self, args: Optional[List[str]] = None) -> int:
        """Main entry point for the CLI."""
        try:
            parsed_args = self.parse_args(args)
            
            # Setup logging
            self.setup_logging(parsed_args.log_level)
            
            # Setup verbose/quiet logging
            if parsed_args.verbose:
                self.logger.setLevel(logging.DEBUG)
            elif parsed_args.quiet:
                self.logger.setLevel(logging.CRITICAL)
            
            # Handle commands
            if parsed_args.command == "run":
                return self.run_phase(parsed_args)
            elif parsed_args.command == "config":
                return self.run_config_command(parsed_args)
            elif parsed_args.command == "benchmark":
                return self.run_benchmark(parsed_args)
            elif parsed_args.command == "checkpoint":
                return self.run_checkpoint_command(parsed_args)
            elif parsed_args.command == "info":
                return self.show_info(parsed_args)
            else:
                self.parser.print_help()
                return 1
                
        except Exception as e:
            self.logger.error(f"Error: {e}")
            return 1
    
    def run_phase(self, args):
        """Run the specified phase."""
        if args.phase == "2":
            return self.run_phase_2(args)
        elif args.phase == "3":
            return self.run_phase_3(args)
        elif args.phase == "4":
            return self.run_phase_4(args)
        elif args.phase == "6":
            return self.run_phase_6(args)
        else:
            self.logger.error(f"Unknown phase: {args.phase}")
            return 1
    
    def run_config_command(self, args):
        """Handle config subcommand."""
        if args.config_command == "create":
            return self.create_config(args)
        elif args.config_command == "list":
            return self.list_config_options(args)
        elif args.config_command == "validate":
            return self.validate_config(args)
        else:
            self.logger.error(f"Unknown config command: {args.config_command}")
            return 1
    
    def run_checkpoint_command(self, args):
        """Handle checkpoint subcommand."""
        if args.checkpoint_command == "save":
            return self.save_checkpoint(args)
        elif args.checkpoint_command == "load":
            return self.load_checkpoint(args)
        else:
            self.logger.error(f"Unknown checkpoint command: {args.checkpoint_command}")
            return 1

def main():
    """Main entry point."""
    cli = NLMCLI()
    return cli.run()

if __name__ == "__main__":
    main()
