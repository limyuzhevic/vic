"""
Command-line interface for NLM Python package.
Provides command-line access to NLM neural simulation functionality.
"""

import argparse
import json
import sys
from typing import List, Optional
from pathlib import Path

from . import createDefaultConfig, createBrain, createSimpleWorld
from .exceptions import NLMError, NLMConfigurationError, NLMValidationError
from .validation import InputValidator, validate_world_parameters, validate_simulation_parameters

__all__ = ['main', 'create_cli', 'run_command']

class NLMCLI:
    """
    Command-line interface for NLM.
    
    Provides comprehensive command-line access to NLM functionality.
    """
    
    def __init__(self):
        """Initialize CLI."""
        self.validator = InputValidator()
    
    def create_config(self, args) -> None:
        """Create and save a configuration file."""
        from .bindings import Config
        
        config = Config()
        
        # Apply command line overrides
        if args.learning_rate:
            config.set('learning_rate', args.learning_rate)
        
        if args.neuron_count:
            config.set('neuron_count', args.neuron_count)
        
        if args.region_count:
            config.set('region_count', args.region_count)
        
        if args.world_width:
            config.set('world_width', args.world_width)
        
        if args.world_height:
            config.set('world_height', args.world_height)
        
        if args.simulation_dt:
            config.set('simulation_dt', args.simulation_dt)
        
        # Save configuration
        if args.output:
            config.saveToFile(args.output)
            print(f"Configuration saved to: {args.output}")
        else:
            # Print configuration summary
            print("Configuration:")
            print(config.summary())
    
    def run_simulation(self, args) -> None:
        """Run a simulation."""
        try:
            # Create configuration
            config = createDefaultConfig()
            
            # Apply command line overrides
            if args.learning_rate:
                config.set('learning_rate', args.learning_rate)
            
            if args.neuron_count:
                config.set('neuron_count', args.neuron_count)
            
            # Create brain and world
            brain = createBrain(config)
            world = createSimpleWorld()
            
            # Configure world
            world.configure(
                args.world_width or 100,
                args.world_height or 100,
                args.vision_width or 20,
                args.vision_height or 20
            )
            
            # Validate parameters
            world_errors = validate_world_parameters(
                args.world_width or 100,
                args.world_height or 100,
                args.vision_width or 20,
                args.vision_height or 20
            )
            
            simulation_errors = validate_simulation_parameters(
                args.simulation_dt or 0.01,
                args.max_steps or 1000,
                args.checkpoint_interval or 100
            )
            
            if world_errors or simulation_errors:
                print("Validation errors:")
                for error in world_errors + simulation_errors:
                    print(f"  - {error}")
                return
            
            # Initialize systems
            brain.initialize()
            world.reset()
            
            # Run simulation
            print(f"Running simulation for {args.max_steps or 1000} steps...")
            
            for step in range(args.max_steps or 1000):
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process with brain
                brain.processSensoryInput(percept)
                
                # Get action
                action = brain.produceAction()
                
                # Apply action in world
                world.applyMotorCommand(action)
                
                # Update world
                world.update(args.simulation_dt or 0.01)
                
                # Print progress occasionally
                if (step + 1) % (args.max_steps // 10) == 0:
                    status = brain.logStatus()
                    print(f"Step {step + 1}/{args.max_steps or 1000}: {status}")
                
                # Save checkpoint
                if (step + 1) % (args.checkpoint_interval or 100) == 0:
                    checkpoint_file = f"checkpoint_step_{step + 1}.json"
                    brain.save(checkpoint_file)
                    print(f"Checkpoint saved: {checkpoint_file}")
            
            print("Simulation completed successfully!")
            
        except NLMError as e:
            print(f"NLM Error: {e}")
            sys.exit(1)
        except Exception as e:
            print(f"Error: {e}")
            sys.exit(1)
    
    def inspect_brain(self, args) -> None:
        """Inspect brain state."""
        try:
            # Load brain from file
            from .bindings import Brain, Config
            config = Config()
            brain = Brain(config)
            
            if args.file:
                brain.load(args.file)
            else:
                print("No brain file specified")
                return
            
            # Print brain statistics
            print(f"Brain Statistics:")
            print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
            print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
            print(f"  - Active neurons: {brain.getActiveNeuronCount()}")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Average firing rate: {brain.getAverageFiringRate():.4f}")
            print(f"  - Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.4f}")
            print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  - Developmental stage: {brain.getDevelopmentalStage()}")
            
            # Print region information
            regions = brain.getRegions()
            print(f"\nBrain Regions ({len(regions)}):")
            for region in regions:
                print(f"  - {region}")
            
        except NLMError as e:
            print(f"NLM Error: {e}")
            sys.exit(1)
        except Exception as e:
            print(f"Error: {e}")
            sys.exit(1)
    
    def create_cli(self) -> argparse.ArgumentParser:
        """Create command-line interface parser."""
        parser = argparse.ArgumentParser(
            description="NLM Neural Simulation Framework Command Line Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter
        )
        
        parser.add_argument(
            '--version', action='version',
            version='NLM Python Bindings 0.1.0'
        )
        
        subparsers = parser.add_subparsers(dest='command', help='Available commands')
        
        # Create config command
        config_parser = subparsers.add_parser('config', help='Create configuration file')
        config_parser.add_argument('--learning-rate', type=float, help='Learning rate')
        config_parser.add_argument('--neuron-count', type=int, help='Number of neurons')
        config_parser.add_argument('--region-count', type=int, help='Number of regions')
        config_parser.add_argument('--world-width', type=float, help='World width')
        config_parser.add_argument('--world-height', type=float, help='World height')
        config_parser.add_argument('--simulation-dt', type=float, help='Simulation time step')
        config_parser.add_argument('--output', help='Output configuration file path')
        
        # Run simulation command
        sim_parser = subparsers.add_parser('run', help='Run simulation')
        sim_parser.add_argument('--learning-rate', type=float, help='Learning rate')
        sim_parser.add_argument('--neuron-count', type=int, help='Number of neurons')
        sim_parser.add_argument('--region-count', type=int, help='Number of regions')
        sim_parser.add_argument('--world-width', type=float, default=100, help='World width')
        sim_parser.add_argument('--world-height', type=float, default=100, help='World height')
        sim_parser.add_argument('--vision-width', type=float, default=20, help='Vision width')
        sim_parser.add_argument('--vision-height', type=float, default=20, help='Vision height')
        sim_parser.add_argument('--simulation-dt', type=float, default=0.01, help='Simulation time step')
        sim_parser.add_argument('--max-steps', type=int, default=1000, help='Maximum simulation steps')
        sim_parser.add_argument('--checkpoint-interval', type=int, default=100, help='Checkpoint interval')
        
        # Inspect command
        inspect_parser = subparsers.add_parser('inspect', help='Inspect brain state')
        inspect_parser.add_argument('--file', help='Brain state file path')
        
        return parser
    
    def run(self, args: List[str]) -> int:
        """
        Run CLI with given arguments.
        
        Args:
            args: Command-line arguments.
            
        Returns:
            Exit code.
        """
        parser = self.create_cli()
        parsed_args = parser.parse_args(args)
        
        if not parsed_args.command:
            parser.print_help()
            return 0
        
        try:
            if parsed_args.command == 'config':
                self.create_config(parsed_args)
            elif parsed_args.command == 'run':
                self.run_simulation(parsed_args)
            elif parsed_args.command == 'inspect':
                self.inspect_brain(parsed_args)
            else:
                print(f"Unknown command: {parsed_args.command}")
                return 1
            
            return 0
            
        except KeyboardInterrupt:
            print("\nOperation cancelled by user")
            return 130
        except Exception as e:
            print(f"Error: {e}")
            return 1
def main():
    """Main entry point for CLI."""
    cli = NLMCLI()
    sys.exit(cli.run(sys.argv[1:]))
def create_cli():
    """Create CLI instance."""
    return NLMCLI()
def run_command(args: List[str]) -> int:
    """
    Run NLM command with given arguments.
    
    Args:
        args: Command-line arguments.
        
    Returns:
        Exit code.
    """
    cli = NLMCLI()
    return cli.run(args)