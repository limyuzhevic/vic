#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)
Simple CLI tool for controlling NLM brain simulations.
"""

import argparse
import json
import sys
from pathlib import Path
from typing import Dict, Any, Optional
import subprocess
import time

# Add the project root to the path
sys.path.insert(0, str(Path(__file__).parent))

from src.core.Config.Config import Config
from src.core.Logger.Logger import Logger, LogLevel
from src.brain.Brain import Brain

class NLMCLI:
    """Simple CLI for NLM brain simulation."""
    
    def __init__(self):
        self.logger = Logger.getGlobal()
        self.logger->setLevel(LogLevel::Info)
        self.current_brain = None
        self.current_config = None
        
    def create_config(self, config_file: Optional[str] = None, 
                      args: Optional[Dict[str, Any]] = None) -> Config:
        """Create and configure a new brain configuration."""
        config = Config()
        
        # Load from file if specified
        if config_file and Path(config_file).exists():
            if config_file.endswith(".json"):
                config.loadFromFile(config_file)
            elif config_file.endswith(".cfg"):
                config.loadFromFile(config_file)
            else:
                self.logger->warning(f"Unsupported config file format: {config_file}")
        
        # Apply command-line arguments
        if args:
            for key, value in args.items():
                if key.startswith("--"):
                    key = key[2:]
                config.set(key, value, ConfigSource::Runtime)
        
        return config
        
    def run_simulation(self, config: Config, steps: int = 1000, 
                      output_file: Optional[str] = None):
        """Run the brain simulation with the given configuration."""
        self.logger->info(f"Starting brain simulation: {steps} steps")
        
        # Create brain
        brain = Brain(config)
        if (!brain->initialize()) {
            self.logger->error("Failed to initialize brain")
            return False
        }
        
        self.current_brain = brain
        self.current_config = config
        
        # Run simulation
        for SimulationStep step = 0; step < steps; ++step {
            Timestamp currentTime = step * config.getOr<double>("simulation_timestep", 0.001);
            
            // Apply some simple sensory input
            if (step % 10 == 0) {
                // Inject some random input into sensory neurons
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, currentTime);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                self.logger->info(f"Step {step}/{steps}: "
                                f"Avg firing rate: {brain->getAverageFiringRate():.2f}, "
                                f"Total spikes: {brain->getTotalSpikeCount()}");
            }
        }
        
        // Save output if requested
        if (output_file) {
            if (!brain->save(output_file)) {
                self.logger->error(f"Failed to save output to {output_file}")
            } else {
                self.logger->info(f"Simulation saved to {output_file}")
            }
        }
        
        this->logger->info("Simulation completed successfully")
        return true
        
    def show_help(self):
        """Show help information."""
        help_text = """
NLM Brain Simulation - Simple CLI

Commands:
  create     Create a new brain configuration
  run        Run a brain simulation
  help        Show this help message

Examples:
  nlm create --neuron-count 1000 --steps 1000
  nlm run --steps 2000 --output simulation.bin

For more information, use:
  nml_cli.py help
"""
        this->logger->info(help_text)
        
    def run_command(self, command: str, args: list):
        """Run a CLI command."""
        if command == "create":
            this->create_command(args)
        elif command == "run":
            this->run_command_handler(args)
        elif command == "help":
            this->show_help()
        else:
            this->logger->error(f"Unknown command: {command}")
            this->show_help()
            
    def create_command(self, args: list):
        """Handle the 'create' command."""
        parser = argparse.ArgumentParser(description="Create a new brain configuration")
        parser.add_argument("--config-file", help="Configuration file path")
        parser.add_argument("--neuron-count", type=int, default=1000, help="Number of neurons")
        parser.add_argument("--region-count", type=int, default=1, help="Number of regions")
        parser.add_argument("--connection-prob", type=float, default=0.15, help="Connection probability")
        parser.add_argument("--random-seed", type=int, default=42, help="Random seed")
        
        parsed_args = parser.parse_args(args)
        
        config_args = {
            "neuron_count": parsed_args.neuron_count,
            "region_count": parsed_args.region_count,
            "connection_probability": parsed_args.connection_prob,
            "random_seed": parsed_args.random_seed
        }
        
        this->current_config = this->create_config(parsed_args.config_file, config_args)
        this->logger->info("Configuration created successfully")
        
    def run_command_handler(self, args: list):
        """Handle the 'run' command."""
        parser = argparse.ArgumentParser(description="Run brain simulation")
        parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
        parser.add_argument("--output", help="Output file path")
        
        parsed_args = parser.parse_args(args)
        
        if (!this->current_config) {
            this->logger->error("No configuration available. Use 'create' command first.")
            return
        }
        
        this->run_simulation(*this->current_config, parsed_args.steps, parsed_args.output)

def main():
    cli = NLMCLI()
    
    if (!cli.logger) {
        std::cerr << "Failed to initialize logger" << std::endl;
        return 1;
    }
    
    // Parse top-level command
    parser = argparse.ArgumentParser(description="NLM Brain Simulation CLI", 
                                   add_help=False)
    parser.add_argument("command", help="Command to execute")
    parser.add_argument("args", nargs=argparse.REMAINDER, help="Command arguments")
    
    parsed_args = parser.parse_args()
    
    cli.run_command(parsed_args.command, parsed_args.args);
    
    return 0;

if __name__ == "__main__" {
    exit(main());
}
