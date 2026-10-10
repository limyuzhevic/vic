#!/usr/bin/env python3
"""
NLM Python Command Line Interface (CLI)
A powerful command-line tool for working with NLM (Neural Learning Machine)
simulations using the pynlm Python bindings.
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Optional, Dict, Any

# Add parent directory to path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

import pynlm
class NLMCLI:
    """Command-line interface for NLM simulations."""

    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.current_step = 0

    def create_brain(self, config_file: Optional[str] = None,
                     neuron_count: int = 1000,
                     connection_probability: float = 0.1) -> None:
        """Create a new brain with specified configuration."""
        print("Creating brain...")
        config = pynlm.createDefaultConfig()

        # Set configuration
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.connection_probability", connection_probability)

        if config_file:
            try:
                config.loadFromFile(config_file)
                print(f"Loaded configuration from {config_file}")
            except Exception as e:
                print(f"Warning: Could not load config file {config_file}: {e}")

        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        print(f"Brain created with {neuron_count} neurons")

    def create_world(self, width: int = 20, height: int = 20,
                     vision_width: int = 8, vision_height: int = 8,
                     max_energy: float = 100.0) -> None:
        """Create a new world for the agent."""
        if not self.brain:
            print("Error: Need to create a brain first!")
            return

        print(f"Creating world {width}x{height}...")
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, vision_width, vision_height)
        self.world.setMaxEnergy(max_energy)
        self.world.reset()

        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        print(f"World created: {width}x{height}, vision: {vision_width}x{vision_height}")

    def run_simulation(self, steps: int, timestep: float = 0.1,
                      log_interval: int = 100,
                      enable_learning: bool = True) -> None:
        """Run the simulation loop."""
        if not self.brain:
            print("Error: No brain created!")
            return

        print(f"Running simulation for {steps} steps...")
        self.current_step = 0

        # Enable learning features if requested
        if enable_learning and self.agent:
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
            print("Learning features enabled: reward modulation, curiosity, development")

        try:
            for step in range(steps):
                # Update world
                self.world.update(timestep)

                # Get sensory input from world
                percept = self.world.getSensoryPercept()

                # Process sensory input in brain
                self.agent.processSensoryInput(percept)

                # Run brain step
                self.brain.step(step)

                # Decode motor command from brain activity
                motor_cmd = self.agent.decodeMotorCommand()

                # Apply motor command to world
                self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())

                # Apply reward modulation (simulate reward from world)
                reward = self._calculate_reward(percept)
                self.agent.applyRewardModulation(reward, 0.0)

                # Update development
                self.agent.updateDevelopment(timestep)

                # Log progress
                if log_interval > 0 and (step + 1) % log_interval == 0:
                    self._log_step(step + 1)

                self.current_step = step + 1

            print("\nSimulation completed!")
            self._print_final_stats()

        except Exception as e:
            print(f"Error during simulation: {e}")
            import traceback
            traceback.print_exc()

    def _calculate_reward(self, percept) -> float:
        """Calculate reward based on sensory input."""
        # Simple reward function: positive for resources, negative for hazards
        vision = percept.getVision()
        if vision:
            # This is a simple example - in reality you'd have more sophisticated reward
            return 0.1  # Default small positive reward
        return 0.0

    def _log_step(self, step: int) -> None:
        """Log simulation progress."""
        if not self.brain:
            return

        print(f"Step {step:5d}: ", end="")
        print(f"Firing: {self.brain.getFiringNeuronCount():4d} ", end="")
        print(f"Rate: {self.brain.getAverageFiringRate():6.2f} ", end="")
        if self.agent:
            print(f"Curiosity: {self.agent.getCuriosityLevel():5.3f} ", end="")
            print(f"Novelty: {self.agent.getNoveltyLevel():5.3f} ", end="")
        print(f"DevStage: {self.brain.getDevelopmentalStage()}")

    def _print_final_stats(self) -> None:
        """Print final simulation statistics."""
        if not self.brain:
            return

        print("\n=== Final Statistics ===")
        print(f"Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        print(f"Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"E/I ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        print(f"Development stage: {self.brain.getDevelopmentalStage()}")

        if self.agent:
            print(f"Neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
            print(f"Curiosity level: {self.agent.getCuriosityLevel():.3f}")
            print(f"Novelty level: {self.agent.getNoveltyLevel():.3f}")
            print(f"Prediction error: {self.agent.getPredictionError():.3f}")

    def save_brain(self, filepath: str) -> None:
        """Save brain state to file."""
        if not self.brain:
            print("Error: No brain to save!")
            return

        try:
            self.brain.save(filepath)
            print(f"Brain saved to {filepath}")
        except Exception as e:
            print(f"Error saving brain: {e}")

    def load_brain(self, filepath: str) -> None:
        """Load brain state from file."""
        if not self.brain:
            print("Error: No brain to load into!")
            return

        try:
            self.brain.load(filepath)
            print(f"Brain loaded from {filepath}")
        except Exception as e:
            print(f"Error loading brain: {e}")

    def create_config(self, config_file: Optional[str] = None) -> None:
        """Create and display configuration."""
        print("Creating configuration...")
        config = pynlm.createDefaultConfig()

        if config_file:
            try:
                config.loadFromFile(config_file)
                print(f"Loaded from {config_file}")
            except Exception as e:
                print(f"Could not load config file: {e}")
                print("Using default configuration")

        print("Current configuration:")
        print(config.summary())

        # Save as JSON if requested
        if config_file and config_file.endswith('.json'):
            try:
                config.saveToFile(config_file)
                print(f"Configuration saved to {config_file}")
            except Exception as e:
                print(f"Could not save configuration: {e}")

    def run_preset(self, preset_name: str, steps: int = 1000) -> None:
        """Run a preset simulation configuration."""
        presets = {
            'minimal': {
                'neuron_count': 100,
                'world_size': 10,
                'enable_learning': False
            },
            'standard': {
                'neuron_count': 1000,
                'world_size': 20,
                'enable_learning': True
            },
            'advanced': {
                'neuron_count': 2000,
                'world_size': 30,
                'enable_learning': True
            }
        }

        if preset_name not in presets:
            print(f"Unknown preset: {preset_name}")
            print(f"Available presets: {', '.join(presets.keys())}")
            return

        preset = presets[preset_name]
        print(f"Running '{preset_name}' preset simulation...")

        self.create_brain(
            neuron_count=preset['neuron_count'],
            connection_probability=0.1
        )

        self.create_world(
            width=preset['world_size'],
            height=preset['world_size'],
            vision_width=min(8, preset['world_size']),
            vision_height=min(8, preset['world_size'])
        )

        self.run_simulation(
            steps=steps,
            enable_learning=preset['enable_learning']
        )
def main():
    """Main entry point for the CLI."""
    cli = NLMCLI()
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Python CLI",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Create a minimal brain and world
  python nlm_cli.py brain create --neuron-count 500
  python nlm_cli.py world create --width 30 --height 30
  python nlm_cli.py sim run --steps 500 --enable-learning

  # Run a preset simulation
  python nlm_cli.py sim preset --name standard --steps 1000

  # Save and load brain states
  python nlm_cli.py brain save brain_state.bin
  python nlm_cli.py brain load saved_brain.bin
        """
    )

    subparsers = parser.add_subparsers(dest='command', help='Available commands')

    # Brain commands
    brain_parser = subparsers.add_parser('brain', help='Brain management commands')
    brain_subparsers = brain_parser.add_subparsers(dest='brain_command')

    brain_create_parser = brain_subparsers.add_parser('create', help='Create a new brain')
    brain_create_parser.add_argument('--neuron-count', type=int, default=1000,
                                     help='Number of neurons (default: 1000)')
    brain_create_parser.add_argument('--connection-probability', type=float, default=0.1,
                                     help='Connection probability (default: 0.1)')
    brain_create_parser.add_argument('--config-file', help='Load configuration from file')
    brain_create_parser.set_defaults(func=lambda args: cli.create_brain(
        config_file=args.config_file,
        neuron_count=args.neuron_count,
        connection_probability=args.connection_probability
    ))

    brain_save_parser = brain_subparsers.add_parser('save', help='Save brain state')
    brain_save_parser.add_argument('filepath', help='File path to save brain state')
    brain_save_parser.set_defaults(func=lambda args: cli.save_brain(args.filepath))

    brain_load_parser = brain_subparsers.add_parser('load', help='Load brain state')
    brain_load_parser.add_argument('filepath', help='File path to load brain state')
    brain_load_parser.set_defaults(func=lambda args: cli.load_brain(args.filepath))

    brain_config_parser = brain_subparsers.add_parser('config', help='Display configuration')
    brain_config_parser.add_argument('--config-file', help='Load configuration from file')
    brain_config_parser.set_defaults(func=lambda args: cli.create_config(args.config_file))

    # World commands
    world_parser = subparsers.add_parser('world', help='World management commands')
    world_subparsers = world_parser.add_subparsers(dest='world_command')

    world_create_parser = world_subparsers.add_parser('create', help='Create a new world')
    world_create_parser.add_argument('--width', type=int, default=20,
                                     help='World width (default: 20)')
    world_create_parser.add_argument('--height', type=int, default=20,
                                     help='World height (default: 20)')
    world_create_parser.add_argument('--vision-width', type=int, default=8,
                                     help='Vision width (default: 8)')
    world_create_parser.add_argument('--vision-height', type=int, default=8,
                                     help='Vision height (default: 8)')
    world_create_parser.add_argument('--max-energy', type=float, default=100.0,
                                     help='Maximum energy (default: 100.0)')
    world_create_parser.set_defaults(func=lambda args: cli.create_world(
        width=args.width,
        height=args.height,
        vision_width=args.vision_width,
        vision_height=args.vision_height,
        max_energy=args.max_energy
    ))

    # Simulation commands
    sim_parser = subparsers.add_parser('sim', help='Simulation commands')
    sim_subparsers = sim_parser.add_subparsers(dest='sim_command')

    sim_run_parser = sim_subparsers.add_parser('run', help='Run simulation')
    sim_run_parser.add_argument('--steps', type=int, default=1000,
                                help='Number of simulation steps (default: 1000)')
    sim_run_parser.add_argument('--timestep', type=float, default=0.1,
                                help='Time step size (default: 0.1)')
    sim_run_parser.add_argument('--log-interval', type=int, default=100,
                                help='Logging interval (default: 100)')
    sim_run_parser.add_argument('--enable-learning', action='store_true',
                                help='Enable learning features')
    sim_run_parser.set_defaults(func=lambda args: cli.run_simulation(
        steps=args.steps,
        timestep=args.timestep,
        log_interval=args.log_interval,
        enable_learning=args.enable_learning
    ))

    sim_preset_parser = sim_subparsers.add_parser('preset', help='Run preset simulation')
    sim_preset_parser.add_argument('--name', required=True, choices=['minimal', 'standard', 'advanced'],
                                   help='Preset name')
    sim_preset_parser.add_argument('--steps', type=int, default=1000,
                                   help='Number of simulation steps (default: 1000)')
    sim_preset_parser.set_defaults(func=lambda args: cli.run_preset(
        preset_name=args.name,
        steps=args.steps
    ))

    # Config file commands
    config_parser = subparsers.add_parser('config', help='Configuration file commands')
    config_subparsers = config_parser.add_subparsers(dest='config_command')

    config_create_parser = config_subparsers.add_parser('create', help='Create configuration file')
    config_create_parser.add_argument('--input-file', help='Load from existing config file')
    config_create_parser.add_argument('--output-file', help='Save to config file')
    config_create_parser.set_defaults(func=lambda args: cli.create_config(
        config_file=args.input_file
    ))

    # Parse arguments and execute command
    if len(sys.argv) == 1:
        parser.print_help()
        return

    args = parser.parse_args()

    if hasattr(args, 'func'):
        args.func(args)
    else:
        parser.print_help()
if __name__ == '__main__':
    main()
