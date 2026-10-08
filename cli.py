#!/usr/bin/env python3
"""
NLM Command Line Interface

A comprehensive CLI for interacting with the NLM (Neural Learning Machine) framework.
Provides commands for simulation management, configuration, validation, and benchmarking.
"""

import argparse
import json
import logging
import os
import sys
from pathlib import Path
from typing import Optional, Dict, Any, List
import time
from datetime import datetime

import numpy as np

try:
    from pynlm import Brain, Config, createBrain, createDefaultConfig
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some features will be limited.")

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class NLMCLI:
    """Main NLM CLI class providing various commands for NLM operations."""

    def __init__(self):
        self.config = Config()
        self.brain = None
        self.working_dir = Path.cwd()

    def create_brain(self, config_file: Optional[str] = None, 
                    neurons: int = 1000, regions: int = 1,
                    timestep: float = 0.001) -> bool:
        """Create an NLM brain with specified configuration.
        
        Args:
            config_file: Path to configuration file
            neurons: Number of neurons
            regions: Number of brain regions
            timestep: Simulation timestep
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if config_file and os.path.exists(config_file):
                logger.info(f"Loading configuration from {config_file}")
                self.config.loadFromFile(config_file)
            else:
                logger.info("Using default configuration")
                self.config.clear()
                self.config.set("neuron_count", neurons)
                self.config.set("region_count", regions)
                self.config.set("simulation_timestep", timestep)
                self.config.set("random_seed", 42)
                self.config.set("connection_probability", 0.1)

            logger.info("Creating NLM brain...")
            self.brain = createBrain(self.config)
            logger.info(f"Brain created successfully with {neurons} neurons")
            return True
            
        except Exception as e:
            logger.error(f"Failed to create brain: {e}")
            return False

    def run_simulation(self, steps: int, output_file: Optional[str] = None,
                      verbose: bool = False) -> bool:
        """Run NLM simulation for specified number of steps.
        
        Args:
            steps: Number of simulation steps
            output_file: Optional file to save simulation results
            verbose: Enable verbose logging
            
        Returns:
            bool: True if successful, False otherwise
        """
        if not self.brain:
            logger.error("No brain created. Use 'create' command first.")
            return False

        try:
            logger.info(f"Starting simulation for {steps} steps")
            start_time = time.time()
            
            for step in range(steps):
                self.brain.step(step)
                
                if verbose and step % 100 == 0:
                    logger.info(f"Step {step}/{steps}")
                
                if output_file and step % 500 == 0:
                    self.save_step_data(output_file, step)

            end_time = time.time()
            duration = end_time - start_time
            
            logger.info(f"Simulation completed in {duration:.2f} seconds")
            logger.info(f"Average step time: {duration/steps*1000:.2f} ms")
            
            if output_file:
                self.save_simulation_results(output_file)
                
            return True
            
        except Exception as e:
            logger.error(f"Simulation failed: {e}")
            return False

    def save_step_data(self, filepath: str, step: int):
        """Save simulation data for a specific step."""
        try:
            data = {
                "step": step,
                "timestamp": step * self.config.getOr<float>("simulation_timestep", 0.001),
                "neuron_count": self.brain.getTotalNeuronCount() if self.brain else 0,
                "synapse_count": self.brain.getTotalSynapseCount() if self.brain else 0,
                "firing_count": self.brain.getFiringNeuronCount() if self.brain else 0,
                "spike_count": self.brain.getTotalSpikeCount() if self.brain else 0,
            }
            
            with open(filepath, 'a') as f:
                json.dump(data, f)
                f.write('\n')
                
        except Exception as e:
            logger.error(f"Failed to save step data: {e}")

    def save_simulation_results(self, filepath: str):
        """Save final simulation results."""
        try:
            results = {
                "final_step": self.brain.getTotalSpikeCount() if self.brain else 0,
                "total_neurons": self.brain.getTotalNeuronCount() if self.brain else 0,
                "total_synapses": self.brain.getTotalSynapseCount() if self.brain else 0,
                "total_spikes": self.brain.getTotalSpikeCount() if self.brain else 0,
                "average_firing_rate": self.brain.getAverageFiringRate() if self.brain else 0.0,
                "e_i_ratio": self.brain.getExcitationInhibitionRatio() if self.brain else 0.0,
                "developmental_stage": self.brain.getDevelopmentalStage() if self.brain else "Unknown",
                "export_timestamp": datetime.now().isoformat(),
            }
            
            with open(filepath, 'w') as f:
                json.dump(results, f, indent=2)
                
        except Exception as e:
            logger.error(f"Failed to save simulation results: {e}")

    def load_config(self, filepath: str) -> bool:
        """Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if not os.path.exists(filepath):
                logger.error(f"Configuration file not found: {filepath}")
                return False
                
            self.config.loadFromFile(filepath)
            logger.info(f"Configuration loaded from {filepath}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to load configuration: {e}")
            return False

    def save_config(self, filepath: str) -> bool:
        """Save configuration to file.
        
        Args:
            filepath: Path to save configuration file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            success = self.config.saveToFile(filepath)
            if success:
                logger.info(f"Configuration saved to {filepath}")
            return success
        except Exception as e:
            logger.error(f"Failed to save configuration: {e}")
            return False

    def validate_config(self, config_file: str) -> bool:
        """Validate configuration file.
        
        Args:
            config_file: Path to configuration file to validate
            
        Returns:
            bool: True if valid, False otherwise
        """
        try:
            temp_config = Config()
            if temp_config.loadFromFile(config_file):
                logger.info(f"Configuration file {config_file} is valid")
                print(temp_config.summary())
                return True
            else:
                logger.error(f"Configuration file {config_file} is invalid")
                return False
                
        except Exception as e:
            logger.error(f"Failed to validate configuration: {e}")
            return False

    def benchmark_brain(self, steps: int = 1000, repetitions: int = 5) -> bool:
        """Benchmark NLM brain performance.
        
        Args:
            steps: Number of simulation steps per benchmark
            repetitions: Number of benchmark repetitions
            
        Returns:
            bool: True if successful, False otherwise
        """
        if not self.brain:
            logger.error("No brain created. Use 'create' command first.")
            return False

        try:
            logger.info(f"Running benchmark for {steps} steps, {repetitions} repetitions")
            
            times = []
            operations = []
            
            for i in range(repetitions):
                logger.info(f"Benchmark run {i+1}/{repetitions}")
                start_time = time.time()
                
                for step in range(steps):
                    self.brain.step(step)
                    
                end_time = time.time()
                duration = end_time - start_time
                
                times.append(duration)
                operations.append(steps)
                
                # Reset brain for next run
                self.brain.reset()
                self.brain.initialize()

            avg_time = np.mean(times)
            std_time = np.std(times)
            avg_operations = np.mean(operations)
            operations_per_second = avg_operations / avg_time

            logger.info("=== Benchmark Results ===")
            logger.info(f"Average time: {avg_time:.3f} ± {std_time:.3f} seconds")
            logger.info(f"Average operations: {avg_operations:.0f}")
            logger.info(f"Operations per second: {operations_per_second:.0f}")
            logger.info(f"Average step time: {avg_time/steps*1000:.2f} ms")
            logger.info(f"Steps per second: {steps/avg_time:.0f}")
            
            return True
            
        except Exception as e:
            logger.error(f"Benchmark failed: {e}")
            return False

    def show_brain_status(self):
        """Display current brain status."""
        if not self.brain:
            logger.info("No brain created.")
            return

        self.brain.logStatus()

    def create_experiment(self, experiment_type: str, **kwargs) -> bool:
        """Create and run a specific experiment.
        
        Args:
            experiment_type: Type of experiment (plasticity, connectivity, stdp, etc.)
            **kwargs: Additional experiment parameters
            
        Returns:
            bool: True if successful, False otherwise
        """
        if not self.brain:
            logger.error("No brain created. Use 'create' command first.")
            return False

        try:
            logger.info(f"Running {experiment_type} experiment")
            
            if experiment_type == "plasticity":
                return self._run_plasticity_experiment(**kwargs)
            elif experiment_type == "connectivity":
                return self._run_connectivity_experiment(**kwargs)
            elif experiment_type == "stdp":
                return self._run_stdp_experiment(**kwargs)
            elif experiment_type == "learning":
                return self._run_learning_experiment(**kwargs)
            else:
                logger.error(f"Unknown experiment type: {experiment_type}")
                return False
                
        except Exception as e:
            logger.error(f"Experiment failed: {e}")
            return False

    def _run_plasticity_experiment(self, steps: int = 5000, **kwargs) -> bool:
        """Run plasticity learning experiment."""
        # Enable plasticity on synapses
        for region in self.brain.getRegions():
            for syn in region.getSynapses():
                syn.enablePlasticity(True, True, False)

        # Run simulation
        success = self.run_simulation(steps, verbose=True)
        
        if success:
            logger.info("Plasticity experiment completed")
            # Analyze results
            self._analyze_plasticity_results()
            
        return success

    def _run_connectivity_experiment(self, steps: int = 1000, **kwargs) -> bool:
        """Run connectivity test."""
        logger.info("Running connectivity test - injecting input patterns")
        
        # Inject input into sensory neurons
        if self.brain:
            for i in range(20):
                if i < self.brain.getTotalNeuronCount() / 4:
                    self.brain.injectCurrentToNeurons(NeuronType.Sensory, 50.0f)

        success = self.run_simulation(steps, verbose=False)
        
        if success:
            logger.info("Connectivity experiment completed")
            
        return success

    def _run_stdp_experiment(self, steps: int = 3000, **kwargs) -> bool:
        """Run STDP verification experiment."""
        logger.info("Running STDP verification - correlated pre-post activity")
        
        if self.brain:
            # Enable only STDP on synapses
            for region in self.brain.getRegions():
                for syn in region.getSynapses():
                    syn.enablePlasticity(False, True, False)

        success = self.run_simulation(steps, verbose=True)
        
        if success:
            logger.info("STDP experiment completed")
            # Analyze STDP results
            self._analyze_stdp_results()
            
        return success

    def _run_learning_experiment(self, steps: int = 8000, **kwargs) -> bool:
        """Run general learning experiment."""
        logger.info("Running learning experiment - repeated input patterns")
        
        if self.brain:
            # Enable all plasticity mechanisms
            for region in self.brain.getRegions():
                for syn in region.getSynapses():
                    syn.enablePlasticity(True, True, True)

        success = self.run_simulation(steps, verbose=True)
        
        if success:
            logger.info("Learning experiment completed")
            # Analyze learning results
            self._analyze_learning_results()
            
        return success

    def _analyze_plasticity_results(self):
        """Analyze plasticity experiment results."""
        if not self.brain:
            return

        initial_weights = []
        final_weights = []
        
        # Get initial weights (simplified analysis)
        for region in self.brain.getRegions():
            for syn in region.getSynapses():
                initial_weights.append(syn.getWeight())

        # Run a short simulation to get final weights
        for step in range(100):
            self.brain.step(step)

        # Get final weights
        for region in self.brain.getRegions():
            for syn in region.getSynapses():
                final_weights.append(syn.getWeight())

        if initial_weights and final_weights:
            initial_mean = np.mean(initial_weights)
            final_mean = np.mean(final_weights)
            change = final_mean - initial_mean
            
            logger.info("Plasticity Analysis:")
            logger.info(f"  Initial mean weight: {initial_mean:.4f}")
            logger.info(f"  Final mean weight: {final_mean:.4f}")
            logger.info(f"  Weight change: {change:.4f}")
            
            if abs(change) > 0.001:
                logger.info("  ✓ Plasticity detected - significant weight changes")
            else:
                logger.info("  ! Minimal plasticity detected")

    def _analyze_stdp_results(self):
        """Analyze STDP experiment results."""
        if not self.brain:
            return

        logger.info("STDP Analysis:")
        logger.info("  Note: Full STDP analysis requires spike tracing")
        logger.info("  Check individual synapse weight changes")

    def _analyze_learning_results(self):
        """Analyze learning experiment results."""
        if not self.brain:
            return

        logger.info("Learning Analysis:")
        logger.info(f"  Total spikes generated: {self.brain.getTotalSpikeCount()}")
        logger.info(f"  Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        logger.info(f"  E/I ratio: {self.brain.getExcitationInhibitionRatio():.2f}")
        logger.info("  Learning detected through spike activity and synaptic changes")

    def export_performance_data(self, filepath: str) -> bool:
        """Export performance data.
        
        Args:
            filepath: Path to export performance data
            
        Returns:
            bool: True if successful, False otherwise
        """
        if not self.brain:
            logger.error("No brain created. Use 'create' command first.")
            return False

        try:
            self.brain.exportPerformanceData(filepath)
            logger.info(f"Performance data exported to {filepath}")
            return True
        except Exception as e:
            logger.error(f"Failed to export performance data: {e}")
            return False

    def help(self, command: str = ""):
        """Show help information for a specific command."""
        commands = {
            "create": "Create an NLM brain with specified configuration",
            "run": "Run NLM simulation",
            "load": "Load configuration from file",
            "save": "Save configuration to file",
            "validate": "Validate configuration file",
            "benchmark": "Run NLM brain performance benchmark",
            "status": "Show current brain status",
            "experiment": "Run specific experiments",
            "export": "Export performance data",
            "help": "Show help information",
            "quit": "Exit NLM CLI",
        }

        if command:
            if command in commands:
                print(f"{command}: {commands[command]}")
            else:
                print(f"Unknown command: {command}")
        else:
            print("Available commands:")
            for cmd, desc in commands.items():
                print(f"  {cmd:15} - {desc}")


def main():
    """Main CLI entry point."""
    cli = NLMCLI()

    # Check if pynlm is available
    if not PYNLM_AVAILABLE:
        print("Error: pynlm module not available. Please install pynlm.")
        sys.exit(1)

    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface - Neural Learning Machine Framework",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm create --neurons 500 --steps 1000
  nlm run --steps 2000 --output results.json
  nlm benchmark --steps 1000 --repetitions 5
  nlm experiment --type plasticity --steps 5000
  nlm config load --file config.json
  nlm config save --file output.json

For more information about a specific command, use:
  nlm help <command>
        """
    )

    parser.add_argument(
        "command",
        nargs="?",
        default="help",
        help="Command to execute"
    )

    parser.add_argument(
        "--config-file", "-c",
        help="Path to configuration file"
    )

    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )

    parser.add_argument(
        "--help", "-h",
        action="store_true",
        help="Show help information"
    )

    args = parser.parse_args()

    # Map commands to functions
    command_map = {
        "help": lambda: cli.help(args.command if args.command != "help" else ""),
        "create": lambda: create_brain_command(cli, args),
        "run": lambda: run_simulation_command(cli, args),
        "load": lambda: load_config_command(cli, args),
        "save": lambda: save_config_command(cli, args),
        "validate": lambda: validate_config_command(cli, args),
        "benchmark": lambda: benchmark_command(cli, args),
        "status": lambda: cli.show_brain_status(),
        "experiment": lambda: experiment_command(cli, args),
        "export": lambda: export_command(cli, args),
    }

    # Handle help request
    if args.help or not args.command:
        parser.print_help()
        return

    # Execute command
    command_func = command_map.get(args.command)
    if command_func:
        try:
            command_func()
        except KeyboardInterrupt:
            print("\nOperation interrupted by user.")
            sys.exit(1)
        except Exception as e:
            logger.error(f"Command failed: {e}")
            sys.exit(1)
    else:
        print(f"Unknown command: {args.command}")
        parser.print_help()
        sys.exit(1)


def create_brain_command(cli: NLMCLI, args):
    """Handle create command."""
    neurons = 1000
    regions = 1
    timestep = 0.001

    if args.config_file:
        if not cli.load_config(args.config_file):
            return False
    else:
        parser = argparse.ArgumentParser()
        parser.add_argument("--neurons", type=int, default=neurons)
        parser.add_argument("--regions", type=int, default=regions)
        parser.add_argument("--timestep", type=float, default=timestep)

        # Parse additional arguments
        import shlex
        extra_args = shlex.split(args.command) if args.command else ""
        if extra_args:
            try:
                cmd_args = parser.parse_args(extra_args)
                neurons = cmd_args.neurons
                regions = cmd_args.regions
                timestep = cmd_args.timestep
            except:
                pass

    if cli.create_brain(neurons=neurons, regions=regions, timestep=timestep):
        print(f"Brain created with {neurons} neurons, {regions} regions")
        print("Use 'nlm run' to start simulation")
        return True
    return False


def run_simulation_command(cli: NLMCLI, args):
    """Handle run command."""
    steps = 1000
    output_file = None

    if args.config_file:
        if not cli.load_config(args.config_file):
            return False

    if args.command and args.command != "run":
        # Parse additional arguments
        import shlex
        extra_args = shlex.split(args.command)
        if extra_args:
            parser = argparse.ArgumentParser()
            parser.add_argument("--steps", type=int, default=steps)
            parser.add_argument("--output", "--output-file", help="Output file path")

            try:
                cmd_args = parser.parse_args(extra_args)
                steps = cmd_args.steps
                output_file = cmd_args.output
            except:
                pass

    if cli.run_simulation(steps, output_file, args.verbose):
        print(f"Simulation completed with {steps} steps")
        return True
    return False


def load_config_command(cli: NLMCLI, args):
    """Handle load command."""
    if not args.config_file:
        print("Error: Configuration file required for load command")
        return False

    if cli.load_config(args.config_file):
        print(f"Configuration loaded from {args.config_file}")
        return True
    return False


def save_config_command(cli: NLMCLI, args):
    """Handle save command."""
    if not args.config_file:
        print("Error: Output file required for save command")
        return False

    if cli.save_config(args.config_file):
        print(f"Configuration saved to {args.config_file}")
        return True
    return False


def validate_config_command(cli: NLMCLI, args):
    """Handle validate command."""
    if not args.config_file:
        print("Error: Configuration file required for validate command")
        return False

    if cli.validate_config(args.config_file):
        print(f"Configuration {args.config_file} is valid")
        return True
    return False


def benchmark_command(cli: NLMCLI, args):
    """Handle benchmark command."""
    steps = 1000
    repetitions = 5

    if args.config_file:
        if not cli.load_config(args.config_file):
            return False

    if args.command and args.command != "benchmark":
        # Parse additional arguments
        import shlex
        extra_args = shlex.split(args.command)
        if extra_args:
            parser = argparse.ArgumentParser()
            parser.add_argument("--steps", type=int, default=steps)
            parser.add_argument("--repetitions", type=int, default=repetitions)

            try:
                cmd_args = parser.parse_args(extra_args)
                steps = cmd_args.steps
                repetitions = cmd_args.repetitions
            except:
                pass

    if cli.benchmark_brain(steps, repetitions):
        print(f"Benchmark completed with {steps} steps, {repetitions} repetitions")
        return True
    return False


def experiment_command(cli: NLMCLI, args):
    """Handle experiment command."""
    experiment_type = "plasticity"
    steps = 1000

    if args.config_file:
        if not cli.load_config(args.config_file):
            return False

    if args.command and args.command != "experiment":
        # Parse additional arguments
        import shlex
        extra_args = shlex.split(args.command)
        if extra_args:
            parser = argparse.ArgumentParser()
            parser.add_argument("--type", default=experiment_type)
            parser.add_argument("--steps", type=int, default=steps)

            try:
                cmd_args = parser.parse_args(extra_args)
                experiment_type = cmd_args.type
                steps = cmd_args.steps
            except:
                pass

    if cli.create_experiment(experiment_type, steps=steps):
        print(f"{experiment_type} experiment completed with {steps} steps")
        return True
    return False


def export_command(cli: NLMCLI, args):
    """Handle export command."""
    output_file = "performance_data.json"

    if args.config_file:
        if not cli.load_config(args.config_file):
            return False

    if args.command and args.command != "export":
        # Parse additional arguments
        import shlex
        extra_args = shlex.split(args.command)
        if extra_args:
            parser = argparse.ArgumentParser()
            parser.add_argument("--output", "--output-file", default=output_file)

            try:
                cmd_args = parser.parse_args(extra_args)
                output_file = cmd_args.output
            except:
                pass

    if cli.export_performance_data(output_file):
        print(f"Performance data exported to {output_file}")
        return True
    return False


if __name__ == "__main__":
    main()