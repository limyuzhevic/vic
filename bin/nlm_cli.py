#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) CLI Interface
===========================================

Command-line interface for controlling NLM simulations, managing configurations,
running experiments, and analyzing results.

Usage: nlm [OPTIONS] COMMAND [ARGS]...

Examples:
    nlm run --steps 1000
    nlm run --config my_config.json --output results.json
    nlm run --agents --world-size 20 --learning
    nlm status --verbose
    nlm checkpoint save --filepath brain_state.bin
    nlm checkpoint load --filepath brain_state.bin
"""

import argparse
import json
import sys
import time
from pathlib import Path
from typing import Optional, Dict, Any

import pynlm
class NLMCLI:
    """Command-line interface for NLM operations."""
    
    def __init__(self):
        self.parser = self.create_parser()
    
    def create_parser(self) -> argparse.ArgumentParser:
        """Create the argument parser with all subcommands."""
        parser = argparse.ArgumentParser(
            prog="nlm",
            description="NLM - Neural Learning Machine Simulation Framework",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog=self.get_epilog()
        )
        
        parser.add_argument(
            "--version",
            action="version",
            version="NLM Python Bindings 0.1.0"
        )
        
        parser.add_argument(
            "--verbose",
            "-v",
            action="store_true",
            help="Enable verbose output"
        )
        
        parser.add_argument(
            "--quiet",
            "-q",
            action="store_true",
            help="Suppress non-essential output"
        )
        
        subparsers = parser.add_subparsers(dest="command", help="Available commands")
        
        # Run command
        self.add_run_command(subparsers)
        
        # Status command
        self.add_status_command(subparsers)
        
        # Checkpoint commands
        self.add_checkpoint_commands(subparsers)
        
        # Configuration commands
        self.add_config_commands(subparsers)
        
        # Agent commands
        self.add_agent_commands(subparsers)
        
        # World commands
        self.add_world_commands(subparsers)
        
        # Export/Import commands
        self.add_export_commands(subparsers)
        
        # Performance commands
        self.add_performance_commands(subparsers)
        
        return parser
    
    def get_epilog(self) -> str:
        """Get the epilog text with examples."""
        return """
Quick Start Examples:
  nlm run --steps 1000                    Run a simple simulation
  nlm run --agents --learning             Run with agent and learning
  nlm checkpoint save --filepath brain.bin  Save brain state
  nlm checkpoint load --filepath brain.bin  Load brain state
  nlm status                               Show system status
  nlm config list                          List all configuration options
  nlm config set brain.neuron_count 500    Set configuration value

For detailed help on any command:
  nlm <command> --help
"""
    
    def add_run_command(self, subparsers):
        """Add the run simulation command."""
        run_parser = subparsers.add_parser(
            "run",
            help="Run a simulation",
            description="Run NLM simulation with various modes"
        )
        
        run_parser.add_argument(
            "--steps",
            "-s",
            type=int,
            default=1000,
            help="Number of simulation steps (default: 1000)"
        )
        
        run_parser.add_argument(
            "--config",
            "-c",
            type=str,
            help="Configuration file path (JSON)"
        )
        
        run_parser.add_argument(
            "--timestep",
            type=float,
            default=0.1,
            help="World timestep duration (default: 0.1)"
        )
        
        # Mode options
        mode_group = run_parser.add_mutually_exclusive_group()
        mode_group.add_argument(
            "--simple",
            action="store_true",
            help="Simple brain-only simulation"
        )
        mode_group.add_argument(
            "--agents",
            action="store_true",
            help="Agent in world simulation"
        )
        mode_group.add_argument(
            "--experiment",
            action="store_true",
            help="Run complete experiment with learning"
        )
        
        # Feature flags
        run_parser.add_argument(
            "--learning",
            action="store_true",
            help="Enable reward-based learning"
        )
        
        run_parser.add_argument(
            "--curiosity",
            action="store_true",
            help="Enable curiosity-driven exploration"
        )
        
        run_parser.add_argument(
            "--development",
            action="store_true",
            help="Enable brain development"
        )
        
        run_parser.add_argument(
            "--plasticity",
            action="store_true",
            help="Enable structural plasticity"
        )
        
        run_parser.add_argument(
            "--verbose",
            action="store_true",
            help="Verbose output during simulation"
        )
        
        run_parser.add_argument(
            "--output",
            "-o",
            type=str,
            help="Output file for results (JSON)"
        )
        
        run_parser.add_argument(
            "--world-width",
            type=int,
            default=20,
            help="World width (default: 20)"
        )
        
        run_parser.add_argument(
            "--world-height",
            type=int,
            default=20,
            help="World height (default: 20)"
        )
        
        run_parser.add_argument(
            "--vision-width",
            type=int,
            default=8,
            help="Vision width (default: 8)"
        )
        
        run_parser.add_argument(
            "--vision-height",
            type=int,
            default=8,
            help="Vision height (default: 8)"
        )
    
    def add_status_command(self, subparsers):
        """Add the status command."""
        status_parser = subparsers.add_parser(
            "status",
            help="Show system status",
            description="Display current system status and statistics"
        )
        
        status_parser.add_argument(
            "--verbose",
            "-v",
            action="store_true",
            help="Show detailed status information"
        )
        
        status_parser.add_argument(
            "--brain",
            action="store_true",
            help="Show brain status only"
        )
        
        status_parser.add_argument(
            "--world",
            action="store_true",
            help="Show world status only"
        )
        
        status_parser.add_argument(
            "--agent",
            action="store_true",
            help="Show agent status only"
        )
    
    def add_checkpoint_commands(self, subparsers):
        """Add checkpoint subcommands."""
        checkpoint_parser = subparsers.add_parser(
            "checkpoint",
            help="Manage checkpoints",
            description="Save and load brain state checkpoints"
        )
        
        checkpoint_subparsers = checkpoint_parser.add_subparsers(
            dest="action",
            help="Checkpoint actions"
        )
        
        # Save command
        save_parser = checkpoint_subparsers.add_parser(
            "save",
            help="Save brain state to file",
            description="Save current brain state to a checkpoint file"
        )
        
        save_parser.add_argument(
            "--filepath",
            "-f",
            required=True,
            help="Filepath to save checkpoint"
        )
        
        save_parser.add_argument(
            "--compression",
            choices=["none", "gzip", "lz4"],
            default="gzip",
            help="Compression level (default: gzip)"
        )
        
        # Load command
        load_parser = checkpoint_subparsers.add_parser(
            "load",
            help="Load brain state from file",
            description="Load brain state from a checkpoint file"
        )
        
        load_parser.add_argument(
            "--filepath",
            "-f",
            required=True,
            help="Filepath to load checkpoint from"
        )
        
        # List command
        list_parser = checkpoint_subparsers.add_parser(
            "list",
            help="List available checkpoints",
            description="List available checkpoint files in directory"
        )
        
        list_parser.add_argument(
            "--directory",
            "-d",
            default=".",
            help="Directory to search for checkpoints (default: .)"
        )
    
    def add_config_commands(self, subparsers):
        """Add configuration subcommands."""
        config_parser = subparsers.add_parser(
            "config",
            help="Manage configuration",
            description="Manage NLM configuration settings"
        )
        
        config_subparsers = config_parser.add_subparsers(
            dest="action",
            help="Configuration actions"
        )
        
        # List command
        list_parser = config_subparsers.add_parser(
            "list",
            help="List all configuration keys",
            description="List all available configuration keys"
        )
        
        # Get command
        get_parser = config_subparsers.add_parser(
            "get",
            help="Get configuration value",
            description="Get value of a specific configuration key"
        )
        
        get_parser.add_argument(
            "key",
            help="Configuration key (e.g., brain.neuron_count)"
        )
        
        # Set command
        set_parser = config_subparsers.add_parser(
            "set",
            help="Set configuration value",
            description="Set value of a configuration key"
        )
        
        set_parser.add_argument(
            "key",
            help="Configuration key"
        )
        
        set_parser.add_argument(
            "value",
            help="Value to set (string)"
        )
        
        # Load command
        load_parser = config_subparsers.add_parser(
            "load",
            help="Load configuration from file",
            description="Load configuration from JSON file"
        )
        
        load_parser.add_argument(
            "--filepath",
            "-f",
            required=True,
            help="Configuration file path"
        )
        
        # Save command
        save_parser = config_subparsers.add_parser(
            "save",
            help="Save configuration to file",
            description="Save current configuration to JSON file"
        )
        
        save_parser.add_argument(
            "--filepath",
            "-f",
            required=True,
            help="Filepath to save configuration"
        )
    
    def add_agent_commands(self, subparsers):
        """Add agent subcommands."""
        agent_parser = subparsers.add_parser(
            "agent",
            help="Manage agent systems",
            description="Control agent behavior and learning"
        )
        
        agent_subparsers = agent_parser.add_subparsers(
            dest="action",
            help="Agent actions"
        )
        
        # Enable subsystems
        for subsystem in ["reward", "curiosity", "development", "plasticity"]:
            enable_parser = agent_subparsers.add_parser(
                f"enable-{subsystem}",
                help=f"Enable {subsystem} learning",
                description=f"Enable {subsystem}-driven learning"
            )
            enable_parser.add_argument(
                "value",
                choices=["true", "false"],
                required=True,
                help="Enable or disable"
            )
        
        # Get stats
        stats_parser = agent_subparsers.add_parser(
            "stats",
            help="Get agent statistics",
            description="Display agent learning statistics"
        )
        
        stats_parser.add_argument(
            "--brain",
            action="store_true",
            help="Include brain statistics"
        )
    
    def add_world_commands(self, subparsers):
        """Add world subcommands."""
        world_parser = subparsers.add_parser(
            "world",
            help="Manage world simulation",
            description="Control world simulation parameters"
        )
        
        world_subparsers = world_parser.add_subparsers(
            dest="action",
            help="World actions"
        )
        
        # Configure command
        config_parser = world_subparsers.add_parser(
            "configure",
            help="Configure world",
            description="Configure world dimensions and parameters"
        )
        
        config_parser.add_argument(
            "--width",
            type=int,
            required=True,
            help="World width"
        )
        
        config_parser.add_argument(
            "--height",
            type=int,
            required=True,
            help="World height"
        )
        
        config_parser.add_argument(
            "--vision-width",
            type=int,
            required=True,
            help="Vision width"
        )
        
        config_parser.add_argument(
            "--vision-height",
            type=int,
            required=True,
            help="Vision height"
        )
        
        # Reset command
        reset_parser = world_subparsers.add_parser(
            "reset",
            help="Reset world",
            description="Reset world to initial state"
        )
        
        # Update command
        update_parser = world_subparsers.add_parser(
            "update",
            help="Update world",
            description="Update world state by timestep"
        )
        
        update_parser.add_argument(
            "--timestep",
            type=float,
            required=True,
            help="Timestep duration"
        )
    
    def add_export_commands(self, subparsers):
        """Add export/import subcommands."""
        export_parser = subparsers.add_parser(
            "export",
            help="Export data",
            description="Export simulation data and results"
        )
        
        export_subparsers = export_parser.add_subparsers(
            dest="action",
            help="Export actions"
        )
        
        # Statistics command
        stats_parser = export_subparsers.add_parser(
            "stats",
            help="Export simulation statistics",
            description="Export simulation statistics to file"
        )
        
        stats_parser.add_argument(
            "--filepath",
            "-f",
            required=True,
            help="Output file path"
        )
        
        stats_parser.add_argument(
            "--format",
            choices=["json", "csv", "txt"],
            default="json",
            help="Output format (default: json)"
        )
        
        # Events command
        events_parser = export_subparsers.add_parser(
            "events",
            help="Export events",
            description="Export simulation events"
        )
        
        events_parser.add_argument(
            "--filepath",
            "-f",
            required=True,
            help="Output file path"
        )
    
    def add_performance_commands(self, subparsers):
        """Add performance monitoring commands."""
        perf_parser = subparsers.add_parser(
            "perf",
            help="Performance monitoring",
            description="Monitor and analyze performance"
        )
        
        perf_subparsers = perf_parser.add_subparsers(
            dest="action",
            help="Performance actions"
        )
        
        # Profile command
        profile_parser = perf_subparsers.add_parser(
            "profile",
            help="Start performance profiling",
            description="Start performance profiling during simulation"
        )
        
        profile_parser.add_argument(
            "--output",
            "-o",
            help="Profile output file"
        )
        
        profile_parser.add_argument(
            "--steps",
            type=int,
            default=100,
            help="Number of steps to profile (default: 100)"
        )
        
        # Benchmark command
        benchmark_parser = perf_subparsers.add_parser(
            "benchmark",
            help="Run benchmarks",
            description="Run performance benchmarks"
        )
        
        benchmark_parser.add_argument(
            "--config",
            help="Configuration file for benchmark"
        )
        
        benchmark_parser.add_argument(
            "--output",
            "-o",
            help="Benchmark results file"
        )
    
    def run_command(self, args):
        """Execute the run simulation command."""
        print(f"Running NLM simulation for {args.steps} steps...")
        
        # Load or create configuration
        if args.config:
            config = pynlm.createDefaultConfig()
            try:
                with open(args.config, 'r') as f:
                    config_data = json.load(f)
                for key, value in config_data.items():
                    config.set(key, value)
            except Exception as e:
                print(f"Error loading config file: {e}")
                return
        else:
            config = pynlm.createDefaultConfig()
        
        # Configure based on mode
        if args.simple:
            print("Running simple brain simulation...")
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(args.steps):
                brain.step(step)
                
                if args.verbose and step % 100 == 0:
                    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        elif args.agents:
            print("Running agent in world simulation...")
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(
                width=args.world_width,
                height=args.world_height,
                visionWidth=args.vision_width,
                visionHeight=args.vision_height
            )
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable features
            if args.learning:
                agent.enableRewardModulation(True)
            if args.curiosity:
                agent.enableCuriosity(True)
            if args.development:
                agent.enableDevelopment(True)
            if args.plasticity:
                agent.enableStructuralPlasticity(True)
            
            for step in range(args.steps):
                world.update(args.timestep)
                
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if args.verbose and step % 100 == 0:
                    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
                    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        
        elif args.experiment:
            print("Running complete experiment...")
            # Use the complete experiment function
            import experiments.Phase6IntegratedExperiment as Phase6Experiment
            import experiments.Phase6Config as Phase6Config
            
            phase6_config = Phase6Config.Phase6Config()
            phase6_config.neuronCount = config.getOr("brain.neuron_count", 1000)
            phase6_config.maxSteps = args.steps
            phase6_config.enableCheckpointing = True
            phase6_config.enableReplay = True
            phase6_config.enableDevelopment = args.development
            
            experiment = Phase6Experiment.Phase6IntegratedExperiment()
            result = experiment.run(phase6_config)
            
            print(f"Experiment completed in {result.totalWallClockTime:.2f}s")
            print(f"Total reward: {result.totalReward}")
            print(f"Average firing rate: {result.avgFiringRate}")
        
        # Save results if requested
        if args.output:
            results = {
                'steps_completed': args.steps,
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'learning_enabled': args.learning,
                'curiosity_enabled': args.curiosity
            }
            
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            
            print(f"Results saved to {args.output}")
        
        print("Simulation completed successfully!")
    
    def status_command(self, args):
        """Execute the status command."""
        if args.brain:
            # Show brain status
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            print("=== BRAIN STATUS ===")
            print(f"Neurons: {brain.getTotalNeuronCount()}")
            print(f"Synapses: {brain.getTotalSynapseCount()}")
            print(f"Active neurons: {brain.getActiveNeuronCount()}")
            print(f"Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            print(f"Average firing rate: {brain.getAverageFiringRate()}")
            print(f"E/I ratio: {brain.getExcitationInhibitionRatio()}")
            print(f"Developmental stage: {brain.getDevelopmentalStage()}")
        
        elif args.world:
            # Show world status
            world = pynlm.createSimpleWorld()
            print("=== WORLD STATUS ===")
            print(f"Width: {world.getWidth()}")
            print(f"Height: {world.getHeight()}")
            print(f"Simulation time: {world.getSimulationTime()}")
        
        elif args.agent:
            # Show agent status
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            print("=== AGENT STATUS ===")
            print(f"Neuromodulation level: {agent.getNeuromodulationLevel()}")
            print(f"Curiosity level: {agent.getCuriosityLevel()}")
            print(f"Novelty level: {agent.getNoveltyLevel()}")
            print(f"Prediction error: {agent.getPredictionError()}")
            print(f"Development stage: {agent.getDevelopmentalStage()}")
        
        else:
            # Show general status
            print("=== NLM SYSTEM STATUS ===")
            print("Python Bindings: 0.1.0")
            print("C++ Backend: Phase 6 - Final Integration")
            print("Core Features: All systems integrated")
    
    def run(self) -> int:
        """Main entry point for the CLI."""
        args = self.parser.parse_args()
        
        try:
            if args.command == "run":
                self.run_command(args)
            elif args.command == "status":
                self.status_command(args)
            else:
                self.parser.print_help()
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
def main():
    """Entry point for the CLI."""
    cli = NLMCLI()
    return cli.run()
if __name__ == "__main__":
    sys.exit(main())
