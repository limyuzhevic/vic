"""
pynlm command-line interface.

This module provides the command-line interface for NLM, including
basic commands for running simulations and managing neural networks.
"""

import argparse
import sys
from typing import Optional
import logging

import pynlm

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


def create_default_config_file(filename: str = "config.json") -> None:
    """
    Create default configuration file.
    
    Args:
        filename: Output filename
    """
    import json
    config = pynlm.create_default_config()
    
    with open(filename, "w") as f:
        json.dump(config, f, indent=2)

    logger.info(f"Created default configuration file: {filename}")


def create_brain_from_config(config_file: str = "config.json") -> pynlm.Brain:
    """
    Create brain from configuration file.
    
    Args:
        config_file: Configuration file path
        
    Returns:
        Brain instance
    """
    import json
    
    with open(config_file, "r") as f:
        config = json.load(f)
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    logger.info("Created brain from configuration")
    return brain


def run_simulation(
    num_steps: int = 1000,
    config_file: Optional[str] = None,
    output_file: Optional[str] = None,
) -> None:
    """
    Run simulation.
    
    Args:
        num_steps: Number of simulation steps
        config_file: Configuration file path
        output_file: Output file path
    """
    if config_file:
        brain = create_brain_from_config(config_file)
    else:
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()

    logger.info(f"Running simulation for {num_steps} steps")
    
    for step in range(num_steps):
        brain.step(step)
        
        if step % 100 == 0:
            firing = brain.get_firing_neuron_count()
            logger.info(f"Step {step}: {firing} firing neurons")

    if output_file:
        brain.save(output_file)
        logger.info(f"Saved simulation state to: {output_file}")


def main() -> int:
    """
    Main CLI entry point.
    
    Returns:
        Exit code
    """
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) - Python bindings for spiking neural networks"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Create default config command
    config_parser = subparsers.add_parser("create-config", help="Create default configuration file")
    config_parser.add_argument("--output", "-o", default="config.json", help="Output filename")
    
    # Run simulation command
    sim_parser = subparsers.add_parser("run", help="Run simulation")
    sim_parser.add_argument("--steps", "-s", type=int, default=1000, help="Number of steps")
    sim_parser.add_argument("--config", "-c", help="Configuration file path")
    sim_parser.add_argument("--output", "-o", help="Output file path")
    
    # Version command
    version_parser = subparsers.add_parser("version", help="Show version information")
    
    args = parser.parse_args()
    
    try:
        if args.command == "create-config":
            create_default_config_file(args.output)
        
        elif args.command == "run":
            run_simulation(args.steps, args.config, args.output)
        
        elif args.command == "version":
            print(f"pynlm version {pynlm.__version__}")
        
        else:
            parser.print_help()
            return 1
        
        return 0
    
    except Exception as e:
        logger.error(f"Error: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())