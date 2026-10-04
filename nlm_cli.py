#!/usr/bin/env python3
"""
NLM - Neural Learning Machine CLI Interface

Command-line interface for controlling the NLM brain system.
Provides configuration management, experiment running, and status monitoring.
"""

import sys
import os
import json
import argparse
import logging
from pathlib import Path
from typing import Dict, Any, Optional

# Add the pynlm module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '.'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some features may not work.")

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)


class NLMCLI:
    """Command-line interface for NLM system control."""

    def __init__(self):
        self.brain = None
        self.world = None
        self.config = None

    def create_default_config(self, output_file: str = "nlm_config.json"):
        """Create a default configuration file."""
        config = {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.15,
            "vision_width": 16,
            "vision_height": 16,
            "world_width": 20.0,
            "world_height": 20.0,
            "max_energy": 100.0,
            "energy_decay_rate": 0.01,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001,
            "enable_reward_modulation": True,
            "enable_structural_plasticity": True,
            "enable_development": True,
            "enable_curiosity": True,
            "simulation_duration": 10.0,
            "output_file": "nlm_checkpoint.pkl"
        }

        with open(output_file, 'w') as f:
            json.dump(config, f, indent=2)

        print(f"Default configuration saved to: {output_file}")
        logger.info(f"Created default configuration file: {output_file}")

    def load_config(self, config_file: str) -> Dict[str, Any]:
        """Load configuration from file."""
        try:
            with open(config_file, 'r') as f:
                config = json.load(f)
            print(f"Configuration loaded from: {config_file}")
            logger.info(f"Loaded configuration from: {config_file}")
            return config
        except FileNotFoundError:
            print(f"Error: Configuration file '{config_file}' not found!")
            return {}
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON in configuration file '{config_file}': {e}")
            return {}

    def create_brain(self, config_file: Optional[str] = None) -> bool:
        """Create and initialize the NLM brain."""
        if not PYNLM_AVAILABLE:
            print("Error: pynlm module not available!")
            return False

        try:
            if config_file:
                config = self.load_config(config_file)
                if not config:
                    return False

                # Convert config to appropriate types
                for key, value in config.items():
                    if isinstance(value, str) and value.lower() in ['true', 'false']:
                        config[key] = value.lower() == 'true'

                # Create config object
                self.config = pynlm.Config()
                # Note: This assumes pynlm.Config has a way to load from dict
                # You might need to implement a custom config loader
            else:
                self.config = pynlm.Config()

            # Create brain
            self.brain = pynlm.Brain(self.config)

            # Create world
            self.world = pynlm.SimpleWorld()

            # Configure world
            if self.config:
                self.world.configure(
                    getattr(self.config, 'world_width', 20.0),
                    getattr(self.config, 'world_height', 20.0),
                    getattr(self.config, 'vision_width', 16),
                    getattr(self.config, 'vision_height', 16)
                )

            # Initialize brain
            self.brain.initialize()

            # Initialize world
            self.world.reset()

            print("NLM brain created and initialized successfully!")
            logger.info("NLM brain created and initialized")
            return True

        except Exception as e:
            print(f"Error creating brain: {e}")
            logger.error(f"Error creating brain: {e}")
            return False

    def run_simulation(self, duration: float, output_file: str = "nlm_checkpoint.pkl"):
        """Run simulation for specified duration."""
        if not self.brain or not self.world:
            print("Error: Brain or world not initialized!")
            return False

        try:
            print(f"Starting simulation for {duration} seconds...")
            logger.info(f"Starting simulation for {duration} seconds")

            start_time = self.world.getSimulationTime()
            end_time = start_time + duration

            while self.world.getSimulationTime() < end_time:
                # Get current sensory percept
                percept = self.world.getSensoryPercept()

                # Process sensory input through agent brain if available
                # Note: This requires a proper AgentBrain instance

                # Update world
                self.world.update(duration / 1000.0)  # Convert to seconds

                # Log progress every second
                current_time = self.world.getSimulationTime()
                if int(current_time * 1000) % 1000 == 0:  # Every second
                    print(f"Time: {current_time:.2f}s | "
                          f"Energy: {self.world.getAgentBody().energy:.1f} | "
                          f"Active neurons: {self.brain.getActiveNeuronCount()}")

            # Save checkpoint if requested
            if output_file:
                self.brain.save(output_file)
                print(f"Checkpoint saved to: {output_file}")
                logger.info(f"Checkpoint saved to: {output_file}")

            print("Simulation completed successfully!")
            logger.info("Simulation completed successfully")
            return True

        except Exception as e:
            print(f"Error during simulation: {e}")
            logger.error(f"Error during simulation: {e}")
            return False

    def run_learning_experiment(self, config_file: Optional[str] = None):
        """Run a learning experiment."""
        if not self.brain or not self.world:
            print("Error: Brain or world not initialized!")
            return False

        try:
            print("Starting learning experiment...")
            logger.info("Starting learning experiment")

            # Run a simple learning loop
            for step in range(1000):
                # Get sensory percept
                percept = self.world.getSensoryPercept()

                # Apply random actions
                if step % 10 == 0:  # Every 10 steps, perform an action
                    action_type = pynlm.MotorCommand.Wait
                    if step % 50 == 0:  # Move forward occasionally
                        action_type = pynlm.MotorCommand.MoveForward
                    elif step % 33 == 0:  # Turn left occasionally
                        action_type = pynlm.MotorCommand.TurnLeft
                    elif step % 25 == 0:  # Turn right occasionally
                        action_type = pynlm.MotorCommand.TurnRight

                    # Apply action
                    result = self.world.applyMotorCommand(action_type, self.world.getSimulationTime())

                    # Log result
                    if result.success:
                        print(f"Step {step}: Action '{action_type}' succeeded, Reward: {result.reward}")

                # Update world
                self.world.update(0.001)

                # Log progress periodically
                if step % 100 == 0:
                    print(f"Step {step}: Energy: {self.world.getAgentBody().energy:.1f}, "
                          f"Active neurons: {self.brain.getActiveNeuronCount()}")

            print("Learning experiment completed successfully!")
            logger.info("Learning experiment completed successfully")
            return True

        except Exception as e:
            print(f"Error during learning experiment: {e}")
            logger.error(f"Error during learning experiment: {e}")
            return False

    def get_status(self):
        """Get current system status."""
        if not self.brain or not self.world:
            print("Error: Brain or world not initialized!")
            return False

        try:
            print("=== NLM System Status ===")
            print(f"Simulation Time: {self.world.getSimulationTime():.2f}s")
            print(f"Agent Energy: {self.world.getAgentBody().energy:.1f}")
            print(f"Agent Position: ({self.world.getAgentBody().x:.1f}, {self.world.getAgentBody().y:.1f})")
            print(f"Active Neurons: {self.brain.getActiveNeuronCount()}")
            print(f"Total Neurons: {self.brain.getTotalNeuronCount()}")
            print(f"Total Synapses: {self.brain.getTotalSynapseCount()}")
            print(f"Firing Rate: {self.brain.getAverageFiringRate():.3f}")
            print(f"Excitatory/Inhibitory Ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
            print(f"Simulation Time: {self.world.getSimulationTime():.2f}s")
            
            if self.config:
                print("Configuration:")
                print(f"  Random Seed: {getattr(self.config, 'random_seed', 'N/A')}")
                print(f"  Neuron Count: {getattr(self.config, 'neuron_count', 'N/A')}")
                print(f"  Vision Size: {getattr(self.config, 'vision_width', 'N/A')}x{getattr(self.config, 'vision_height', 'N/A')}")
            
            print("========================")
            return True

        except Exception as e:
            print(f"Error getting status: {e}")
            logger.error(f"Error getting status: {e}")
            return False

    def help(self):
        """Show help information."""
        print("NLM - Neural Learning Machine CLI Interface")
        print("=" * 50)
        print("Commands:")
        print("  create-config <file>      Create a default configuration file")
        print("  load-config <file>        Load configuration file")
        print("  create-brain [config]     Create and initialize NLM brain")
        print("  run-simulation <duration> Run simulation for specified duration (seconds)")
        print("  run-experiment            Run a learning experiment")
        print("  status                    Display current system status")
        print("  help                      Show this help message")
        print("  exit                      Exit the CLI")
        print()
        print("Examples:")
        print("  create-config nlm_config.json")
        print("  create-brain")
        print("  run-simulation 10.0")
        print("  run-experiment")
        print("  status")


def main():
    """Main CLI entry point."""
    cli = NLMCLI()
    
    print("NLM - Neural Learning Machine CLI")
    print("=" * 40)
    
    while True:
        try:
            command = input("\nEnter command (help for help): ").strip().lower()
            
            if command == "exit":
                print("Goodbye!")
                break
            elif command == "help":
                cli.help()
            elif command.startswith("create-config"):
                if len(command.split()) > 1:
                    output_file = command.split()[1]
                    cli.create_default_config(output_file)
                else:
                    print("Error: Please specify output file name!")
                    print("Usage: create-config <file>")
            elif command.startswith("load-config"):
                if len(command.split()) > 1:
                    config_file = command.split()[1]
                    cli.load_config(config_file)
                else:
                    print("Error: Please specify configuration file!")
                    print("Usage: load-config <file>")
            elif command == "create-brain":
                config_file = None
                if len(sys.argv) > 1 and not sys.argv[-1].startswith("-"):
                    config_file = sys.argv[-1]
                cli.create_brain(config_file)
            elif command.startswith("run-simulation"):
                if len(command.split()) > 1:
                    try:
                        duration = float(command.split()[1])
                        cli.run_simulation(duration)
                    except ValueError:
                        print("Error: Invalid duration!")
                        print("Usage: run-simulation <duration>")
                else:
                    print("Error: Please specify simulation duration!")
                    print("Usage: run-simulation <duration>")
            elif command == "run-experiment":
                cli.run_learning_experiment()
            elif command == "status":
                cli.get_status()
            else:
                print(f"Unknown command: {command}")
                print("Type 'help' for available commands.")
                
        except KeyboardInterrupt:
            print("\nInterrupted by user. Exiting...")
            break
        except Exception as e:
            print(f"Error: {e}")
            logger.error(f"Error in CLI: {e}")


if __name__ == "__main__":
    main()
