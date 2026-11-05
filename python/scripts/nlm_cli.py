#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

A comprehensive command-line tool for controlling NLM simulations,
both simple and advanced, with support for simulation control,
configuration management, and result analysis.

Usage:
    nlm_cli [command] [options]

Commands:
    run          Run a simulation with specified parameters
    config        Configuration management (create, list, edit)
    analyze       Analyze simulation results
    benchmark     Run performance benchmarks
    experiment    Create and run experiments
    validate      Validate simulation setup
    help          Show help information
"""

import argparse
import sys
import json
import time
import logging
from pathlib import Path
from typing import Optional, List

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class NLMCLI:
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.scripts_dir = self.project_root / "scripts"
        
    def create_parser(self):
        """Create the argument parser."""
        parser = argparse.ArgumentParser(
            description="NLM (Neural Learning Machine) Command Line Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter
        )
        
        subparsers = parser.add_subparsers(dest="command", help="Available commands")
        
        # Run command
        self._add_run_command(subparsers)
        
        # Config command
        self._add_config_command(subparsers)
        
        # Analyze command
        self._add_analyze_command(subparsers)
        
        # Benchmark command
        self._add_benchmark_command(subparsers)
        
        # Experiment command
        self._add_experiment_command(subparsers)
        
        # Validate command
        self._add_validate_command(subparsers)
        
        return parser
    
    def _add_run_command(self, subparsers):
        """Add the run command."""
        parser = subparsers.add_parser("run", help="Run a simulation")
        
        parser.add_argument("--brain-mode", choices=["default", "compact", "large"], 
                          default="default", help="Brain configuration mode")
        parser.add_argument("--world-width", type=int, default=20, help="World width")
        parser.add_argument("--world-height", type=int, default=20, help="World height")
        parser.add_argument("--vision-width", type=int, default=8, help="Vision width")
        parser.add_argument("--vision-height", type=int, default=8, help="Vision height")
        parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
        parser.add_argument("--timestep", type=float, default=0.1, help="Simulation timestep")
        parser.add_argument("--enable-learning", action="store_true", help="Enable learning features")
        parser.add_argument("--enable-development", action="store_true", help="Enable development")
        parser.add_argument("--enable-curiosity", action="store_true", help="Enable curiosity")
        parser.add_argument("--log-stats", action="store_true", help="Log statistics")
        parser.add_argument("--output", type=str, help="Output file for results")
        parser.add_argument("--output-format", choices=["json", "csv", "txt"], 
                          default="json", help="Output format")
        parser.add_argument("--seed", type=int, help="Random seed for reproducibility")
        
    def _add_config_command(self, subparsers):
        """Add the config command."""
        parser = subparsers.add_parser("config", help="Configuration management")
        
        config_subparsers = parser.add_subparsers(dest="config_action", help="Config actions")
        
        # Create config
        create_parser = config_subparsers.add_parser("create", help="Create configuration")
        create_parser.add_argument("--name", required=True, help="Configuration name")
        create_parser.add_argument("--file", help="Output file path")
        
        # List config
        list_parser = config_subparsers.add_parser("list", help="List configurations")
        list_parser.add_argument("--all", action="store_true", help="Show all configurations")
        
        # Show config
        show_parser = config_subparsers.add_parser("show", help="Show configuration details")
        show_parser.add_argument("name", help="Configuration name")
        
    def _add_analyze_command(self, subparsers):
        """Add the analyze command."""
        parser = subparsers.add_parser("analyze", help="Analyze simulation results")
        
        parser.add_argument("--input", required=True, help="Input file path")
        parser.add_argument("--input-format", choices=["json", "csv", "txt"], 
                          default="json", help="Input format")
        parser.add_argument("--output", help="Output file path")
        parser.add_argument("--plot", action="store_true", help="Generate plots")
        parser.add_argument("--verbose", action="store_true", help="Verbose output")
        
    def _add_benchmark_command(self, subparsers):
        """Add the benchmark command."""
        parser = subparsers.add_parser("benchmark", help="Run performance benchmarks")
        
        parser.add_argument("--brain-mode", choices=["default", "compact", "large"], 
                          default="default", help="Brain configuration mode")
        parser.add_argument("--steps", type=int, default=100, help="Number of steps for benchmark")
        parser.add_argument("--iterations", type=int, default=5, help="Number of iterations")
        parser.add_argument("--output", help="Output file for benchmark results")
        
    def _add_experiment_command(self, subparsers):
        """Add the experiment command."""
        parser = subparsers.add_parser("experiment", help="Create and run experiments")
        
        parser.add_argument("--name", required=True, help="Experiment name")
        parser.add_argument("--description", help="Experiment description")
        parser.add_argument("--brain-mode", choices=["default", "compact", "large"], 
                          default="default", help="Brain configuration mode")
        parser.add_argument("--steps", type=int, default=500, help="Number of steps")
        parser.add_argument("--timestep", type=float, default=0.1, help="Simulation timestep")
        parser.add_argument("--learning-rate", type=float, default=0.001, help="Learning rate")
        parser.add_argument("--world-size", type=int, default=10, help="World size")
        parser.add_argument("--agent-start", nargs=2, type=float, default=[5.0, 5.0], 
                          help="Agent starting position (x y)")
        parser.add_argument("--output", help="Output file path")
        
    def _add_validate_command(self, subparsers):
        """Add the validate command."""
        parser = subparsers.add_parser("validate", help="Validate simulation setup")
        
        parser.add_argument("--brain", help="Brain configuration file")
        parser.add_argument("--world", help="World configuration file")
        parser.add_argument("--agent", help="Agent configuration file")
        parser.add_argument("--format", choices=["json", "yaml", "ini"], 
                          default="json", help="Configuration format")
        parser.add_argument("--strict", action="store_true", help="Strict validation")
        
    def run_command(self, args):
        """Execute the run command."""
        logger.info("Starting simulation with parameters: %s", args)
        
        try:
            # Import here to avoid import errors if pynlm is not available
            import pynlm
            
            # Create brain configuration
            if args.brain_mode == "compact":
                config = pynlm.createDefaultConfig()
                config.set("brain.neuron_count", 100)
                config.set("brain.connection_probability", 0.05)
            elif args.brain_mode == "large":
                config = pynlm.createDefaultConfig()
                config.set("brain.neuron_count", 5000)
                config.set("brain.connection_probability", 0.15)
            else:
                config = pynlm.createDefaultConfig()
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create world
            world = pynlm.createSimpleWorld()
            world.configure(
                width=args.world_width,
                height=args.world_height,
                visionWidth=args.vision_width,
                visionHeight=args.vision_height
            )
            world.reset()
            world.setAgentStart(args.agent_start[0], args.agent_start[1])
            
            # Set random seed if provided
            if args.seed is not None:
                world.setRandomSeed(args.seed)
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning features
            if args.enable_learning:
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
            if args.enable_development:
                agent.enableDevelopment(True)
            
            # Run simulation
            logger.info("Running simulation for %d steps...", args.steps)
            start_time = time.time()
            
            for step in range(args.steps):
                world.update(args.timestep)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if args.log_stats and step % 100 == 0:
                    stats = agent.getAllStats()
                    logger.info("Step %d: Firing neurons: %d, Curiosity: %.3f", 
                              step, brain.getFiringNeuronCount(), stats["curiosity_level"])
            
            end_time = time.time()
            logger.info("Simulation completed in %.2f seconds", end_time - start_time)
            
            # Collect statistics
            final_stats = agent.getAllStats()
            final_stats.update({
                "total_steps": args.steps,
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "simulation_time": end_time - start_time,
                "brain_mode": args.brain_mode,
                "world_size": f"{args.world_width}x{args.world_height}",
                "final_firing_rate": brain.getAverageFiringRate(),
                "final_e_i_ratio": brain.getExcitationInhibitionRatio()
            })
            
            # Save results
            if args.output:
                self._save_results(final_stats, args.output, args.output_format)
                logger.info("Results saved to %s", args.output)
            
            # Print summary
            print("\n=== Simulation Results ===")
            print(f"Brain mode: {args.brain_mode}")
            print(f"World size: {args.world_width}x{args.world_height}")
            print(f"Steps: {args.steps}")
            print(f"Total neurons: {brain.getTotalNeuronCount()}")
            print(f"Total synapses: {brain.getTotalSynapseCount()}")
            print(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
            print(f"Final E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
            print(f"Curiosity level: {final_stats['curiosity_level']:.3f}")
            print(f"Novelty level: {final_stats['novelty_level']:.3f}")
            print(f"Simulation time: {end_time - start_time:.2f} seconds")
            
            return True
            
        except Exception as e:
            logger.error("Simulation failed: %s", str(e))
            return False
    
    def _save_results(self, stats, output_path, format):
        """Save results to file."""
        output_file = Path(output_path)
        
        if format == "json":
            with open(output_file, 'w') as f:
                json.dump(stats, f, indent=2)
        elif format == "csv":
            import csv
            with open(output_file, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(stats.keys())
                writer.writerow(stats.values())
        elif format == "txt":
            with open(output_file, 'w') as f:
                f.write("Simulation Results\n")
                f.write("=" * 50 + "\n")
                for key, value in stats.items():
                    f.write(f"{key}: {value}\n")
    
    def execute(self, command, args):
        """Execute the requested command."""
        if command == "run":
            return self.run_command(args)
        elif command == "config":
            return self.config_command(args)
        elif command == "analyze":
            return self.analyze_command(args)
        elif command == "benchmark":
            return self.benchmark_command(args)
        elif command == "experiment":
            return self.experiment_command(args)
        elif command == "validate":
            return self.validate_command(args)
        else:
            logger.error("Unknown command: %s", command)
            return False
    
    def config_command(self, args):
        """Execute the config command."""
        if args.config_action == "create":
            logger.info("Creating configuration: %s", args.name)
            # Implementation for creating config
            return True
        elif args.config_action == "list":
            logger.info("Listing configurations")
            # Implementation for listing config
            return True
        elif args.config_action == "show":
            logger.info("Showing configuration: %s", args.name)
            # Implementation for showing config
            return True
        return False
    
    def analyze_command(self, args):
        """Execute the analyze command."""
        logger.info("Analyzing results from %s", args.input)
        # Implementation for analyzing results
        return True
    
    def benchmark_command(self, args):
        """Execute the benchmark command."""
        logger.info("Running benchmark with brain mode: %s", args.brain_mode)
        # Implementation for benchmarking
        return True
    
    def experiment_command(self, args):
        """Execute the experiment command."""
        logger.info("Running experiment: %s", args.name)
        # Implementation for experiments
        return True
    
    def validate_command(self, args):
        """Execute the validate command."""
        logger.info("Validating simulation setup")
        # Implementation for validation
        return True

def main():
    """Main entry point."""
    cli = NLMCLI()
    parser = cli.create_parser()
    
    if len(sys.argv) == 1:
        parser.print_help()
        return 1
    
    try:
        args = parser.parse_args()
        if not args.command:
            parser.print_help()
            return 1
        
        success = cli.execute(args.command, args)
        return 0 if success else 1
        
    except Exception as e:
        logger.error("CLI error: %s", str(e))
        return 1

if __name__ == "__main__":
    sys.exit(main())
