#!/usr/bin/env python3
"""
Command-line interface for NLM Python bindings
===========================================

This module provides a command-line interface for the NLM Python bindings,
enabling users to run simulations, manage experiments, and visualize results
through the terminal.
"""

import argparse
import json
import logging
import sys
from pathlib import Path
from typing import Optional

import pynlm
def setup_logging(level: str = "INFO"):
    """Set up logging for the application."""
    numeric_level = getattr(logging, level.upper(), None)
    if not isinstance(numeric_level, int):
        raise ValueError(f"Invalid log level: {level}")

    logging.basicConfig(
        level=numeric_level,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S"
    )
class CLIHandler:
    """Handles command-line interface operations."""

    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.brain = None
        self.experiment = None

    def create_brain(self, config_file: Optional[str] = None,
                     neuron_count: Optional[int] = None,
                     duration: Optional[float] = None) -> pynlm.Brain:
        """
        Create a brain instance with optional configuration.

        Args:
            config_file: Path to configuration file
            neuron_count: Number of neurons to create
            duration: Simulation duration in seconds

        Returns:
            Brain instance
        """
        try:
            # Create default configuration
            config = pynlm.Config.create_default()

            # Load configuration from file if provided
            if config_file and Path(config_file).exists():
                config = pynlm.Config.load_from_file(config_file)
                self.logger.info(f"Loaded configuration from {config_file}")

            # Apply command-line overrides
            if neuron_count:
                config.set("simulation.neuron_count", neuron_count)
                self.logger.info(f"Set neuron count to {neuron_count}")

            if duration:
                config.set("simulation.duration", duration)
                self.logger.info(f"Set duration to {duration} seconds")

            # Create brain
            brain = pynlm.create_brain(config)
            self.logger.info("Brain created successfully")

            return brain

        except Exception as e:
            self.logger.error(f"Failed to create brain: {e}")
            raise

    def run_simulation(self, steps: int, dt: float = 0.001,
                       output_file: Optional[str] = None):
        """
        Run a simulation with the given parameters.

        Args:
            steps: Number of simulation steps
            dt: Time step duration in seconds
            output_file: Optional file to save simulation results
        """
        try:
            self.logger.info(f"Starting simulation with {steps} steps, dt={dt}")

            # Initialize brain
            self.brain.initialize()
            self.logger.info("Brain initialized")

            # Run simulation
            for step in range(steps):
                self.brain.step(step, dt)

                # Log progress every 10% of simulation
                if step % max(1, steps // 10) == 0:
                    stats = self.brain.get_stats()
                    self.logger.info(
                        f"Step {step}/{steps} - "
                        f"Spikes: {stats['total_spikes']}, "
                        f"Firing rate: {stats['firing_rate']:.3f} Hz"
                    )

            self.logger.info("Simulation completed")

            # Save results if requested
            if output_file:
                self.save_simulation_results(output_file)
                self.logger.info(f"Results saved to {output_file}")

            return self.brain

        except Exception as e:
            self.logger.error(f"Simulation failed: {e}")
            raise

    def save_simulation_results(self, filepath: str):
        """
        Save simulation results to a file.

        Args:
            filepath: Path to save results
        """
        try:
            if not self.brain:
                raise ValueError("No brain instance available")

            results = {
                "brain_stats": self.brain.get_stats(),
                "simulation_time": self.brain.simulation_time,
                "step_count": self.brain.step_count,
                "initialized": self.brain.initialized
            }

            with open(filepath, 'w') as f:
                json.dump(results, f, indent=2)

        except Exception as e:
            self.logger.error(f"Failed to save results: {e}")
            raise

    def create_experiment(self, experiment_id: str, brain: pynlm.Brain,
                         parameters: Optional[dict] = None):
        """
        Create an experiment with the given parameters.

        Args:
            experiment_id: Unique identifier for the experiment
            brain: Brain instance to use
            parameters: Experiment parameters
        """
        try:
            self.experiment = pynlm.Experiment(
                experiment_id, brain, parameters
            )
            self.logger.info(f"Experiment '{experiment_id}' created")

            return self.experiment

        except Exception as e:
            self.logger.error(f"Failed to create experiment: {e}")
            raise

    def run_experiment(self, run_id: Optional[str] = None):
        """
        Run the experiment.

        Args:
            run_id: Optional run identifier
        """
        try:
            if not self.experiment:
                raise ValueError("No experiment created")

            self.logger.info(f"Running experiment {run_id or 'default'}")

            result = self.experiment.run(run_id)
            self.logger.info(f"Experiment completed in {result.duration:.2f} seconds")

            return result

        except Exception as e:
            self.logger.error(f"Experiment failed: {e}")
            raise

    def create_visualizations(self, brain: pynlm.Brain,
                            output_prefix: str = "visualization"):
        """
        Create visualizations for the brain state.

        Args:
            brain: Brain instance to visualize
            output_prefix: Prefix for output files
        """
        try:
            self.logger.info("Creating visualizations")

            visualizer = pynlm.Visualizer(save_plots=True)

            # Create comprehensive visualization
            visualizer.plot_all(brain, output_prefix=output_prefix)

            self.logger.info(f"Visualizations saved with prefix: {output_prefix}")

        except Exception as e:
            self.logger.error(f"Visualization failed: {e}")
            raise

    def demonstrate_example(self, example_name: str):
        """
        Demonstrate a specific example.

        Args:
            example_name: Name of example to demonstrate
        """
        examples_dir = Path(__file__).parent / "examples"

        example_map = {
            "getting_started": "00_getting_started.py",
            "basic_simulation": "01_basic_simulation.py",
            "learning_experiment": "02_learning_experiment.py",
            "advanced_usage": "03_advanced_usage.py"
        }

        if example_name not in example_map:
            available = ", ".join(example_map.keys())
            raise ValueError(f"Unknown example: {example_name}. Available: {available}")

        example_file = examples_dir / example_map[example_name]

        if not example_file.exists():
            raise FileNotFoundError(f"Example file not found: {example_file}")

        self.logger.info(f"Running example: {example_name}")

        # Import and run the example
        sys.path.insert(0, str(examples_dir.parent))
        exec(open(example_file).read())

        self.logger.info(f"Example {example_name} completed")
def main():
    """Main entry point for the CLI."""
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings - Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run basic simulation
  pynlm --create-brain --simulation-steps 1000

  # Run a specific example
  pynlm --example getting_started

  # Create visualizations
  pynlm --create-visualizations --brain-file my_brain.json

  # Run experiment
  pynlm --run-experiment --experiment-id my_experiment
        """
    )

    # Subparsers for different commands
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")

    # Create brain command
    brain_parser = subparsers.add_parser("create-brain", help="Create a brain instance")
    brain_parser.add_argument("--config-file", help="Path to configuration file")
    brain_parser.add_argument("--neuron-count", type=int, help="Number of neurons")
    brain_parser.add_argument("--duration", type=float, help="Simulation duration (seconds)")

    # Run simulation command
    sim_parser = subparsers.add_parser("run-simulation", help="Run simulation")
    sim_parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    sim_parser.add_argument("--dt", type=float, default=0.001, help="Time step (seconds)")
    sim_parser.add_argument("--output-file", help="Output file for results")

    # Create visualizations command
    viz_parser = subparsers.add_parser("create-visualizations", help="Create visualizations")
    viz_parser.add_argument("--brain-file", help="Path to brain checkpoint file")
    viz_parser.add_argument("--output-prefix", default="visualization", help="Output file prefix")

    # Create experiment command
    exp_parser = subparsers.add_parser("create-experiment", help="Create experiment")
    exp_parser.add_argument("--experiment-id", required=True, help="Experiment identifier")
    exp_parser.add_argument("--config-file", help="Path to configuration file")

    # Run experiment command
    run_exp_parser = subparsers.add_parser("run-experiment", help="Run experiment")
    run_exp_parser.add_argument("--experiment-id", help="Experiment identifier")
    run_exp_parser.add_argument("--run-id", help="Run identifier")

    # Example command
    example_parser = subparsers.add_parser("example", help="Run an example")
    example_parser.add_argument("example_name", choices=["getting_started", "basic_simulation", "learning_experiment", "advanced_usage"], help="Example to run")

    # General options
    parser.add_argument("--log-level", default="INFO", choices=["DEBUG", "INFO", "WARNING", "ERROR"], help="Log level")
    parser.add_argument("--help-examples", action="store_true", help="Show example usage")

    args = parser.parse_args()

    # Set up logging
    setup_logging(args.log_level)

    handler = CLIHandler()

    try:
        if args.command == "create-brain":
            handler.create_brain(
                config_file=args.config_file,
                neuron_count=args.neuron_count,
                duration=args.duration
            )

        elif args.command == "run-simulation":
            brain = handler.create_brain()
            handler.brain = brain
            handler.run_simulation(
                steps=args.steps,
                dt=args.dt,
                output_file=args.output_file
            )

        elif args.command == "create-visualizations":
            if args.brain_file:
                handler.create_brain(config_file=args.brain_file)
                brain = handler.brain
            else:
                brain = handler.create_brain()
                handler.brain = brain
            handler.create_visualizations(brain, args.output_prefix)

        elif args.command == "create-experiment":
            brain = handler.create_brain(config_file=args.config_file)
            handler.create_experiment(args.experiment_id, brain)

        elif args.command == "run-experiment":
            if args.experiment_id:
                brain = handler.create_brain()
                handler.brain = brain
                handler.create_experiment(args.experiment_id, brain)
            handler.run_experiment(args.run_id)

        elif args.command == "example":
            handler.demonstrate_example(args.example_name)

        elif args.help_examples:
            parser.print_help()

        else:
            parser.print_help()

    except Exception as e:
        logging.error(f"Command failed: {e}")
        sys.exit(1)
if __name__ == "__main__":
    main()