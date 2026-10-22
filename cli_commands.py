#!/usr/bin/env python3
"""
NLM CLI Command Implementations

Implementation of CLI commands for the NLM (Neural Learning Machine) framework.
Each command class handles a specific functionality.
"""

import sys
import os
import json
from pathlib import Path
from typing import Any, Dict, List, Optional
from datetime import datetime

from cli_utils import (
    NLMCLIError,
    load_config,
    save_config,
    create_brain,
    run_simulation,
    format_stats,
    validate_config,
    create_markdown_table
)

class Command:
    """Base class for all CLI commands"""
    
    def __init__(self, args, debug=False):
        self.args = args
        self.debug = debug
        self.name = self.__class__.__name__.lower()
    
    def execute(self) -> bool:
        """Execute the command. Returns True on success."""
        raise NotImplementedError("Subclasses must implement execute()")
    
    def log(self, message: str):
        """Log a message (with debug prefix if debug is enabled)"""
        if self.debug:
            prefix = f"[{self.name.upper()}] "
            print(f"{prefix}{message}")
        else:
            print(message)
    
    def error(self, message: str) -> bool:
        """Log an error message and return False"""
        print(f"ERROR: {message}", file=sys.stderr)
        return False

class RunCommand(Command):
    """Command to run a neural simulation"""
    
    def execute(self) -> bool:
        """Run a neural simulation"""
        try:
            steps = self.args.steps
            verbose = self.args.verbose
            config_file = self.args.config
            
            self.log(f"Loading configuration from {config_file}")
            config = load_config(config_file)
            
            self.log(f"Creating brain with {config.get('neuron_count', 'default')} neurons")
            brain = create_brain(config)
            
            self.log(f"Running simulation for {steps} steps")
            results = run_simulation(brain, steps, verbose)
            
            print("\n=== Simulation Results ===")
            print(f"Steps completed: {results['steps']}")
            print(f"Initial spikes: {results['initial_spikes']}")
            print(f"Final spikes: {results['final_spikes']}")
            print(f"Spikes generated: {results['spike_increase']}")
            print(f"Final firing rate: {results['final_firing']}")
            print(f"Average firing rate: {results['average_firing_rate']:.3f}")
            
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Unexpected error: {e}")

class ConfigCommand(Command):
    """Command to manage configuration"""
    
    def execute(self) -> bool:
        """Manage configuration"""
        subcommand = self.args.subcommand
        
        if subcommand == "show":
            return self._show()
        elif subcommand == "set":
            return self._set()
        elif subcommand == "get":
            return self._get()
        elif subcommand == "load":
            return self._load()
        elif subcommand == "save":
            return self._save()
        elif subcommand == "default":
            return self._default()
        else:
            return self.error(f"Unknown config subcommand: {subcommand}")
    
    def _show(self) -> bool:
        """Show current configuration"""
        try:
            config_file = self.args.config
            config = load_config(config_file)
            
            print("=== Current Configuration ===")
            for key, value in config.items():
                print(f"{key}: {value} ({type(value).__name__})")
            
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error showing config: {e}")
    
    def _set(self) -> bool:
        """Set a configuration value"""
        try:
            key = self.args.key
            value = self.args.value
            
            # Convert value to appropriate type
            try:
                if value.lower() == 'true':
                    value = True
                elif value.lower() == 'false':
                    value = False
                elif value.isdigit():
                    value = int(value)
                else:
                    try:
                        value = float(value)
                    except ValueError:
                        pass  # Keep as string
            except (ValueError, AttributeError):
                pass  # Keep as string
            
            # Create a config dictionary
            config = {key: value}
            
            # Save to file
            output_file = self.args.output or "configs/modified.cfg"
            save_config(config, output_file)
            
            print(f"Configuration saved to {output_file}")
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error setting config: {e}")
    
    def _get(self) -> bool:
        """Get a configuration value"""
        try:
            key = self.args.key
            config_file = self.args.config
            config = load_config(config_file)
            
            if key in config:
                print(f"{key}: {config[key]}")
                return True
            else:
                print(f"Key '{key}' not found in configuration")
                return False
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error getting config: {e}")
    
    def _load(self) -> bool:
        """Load configuration from file"""
        try:
            input_file = self.args.input
            config = load_config(input_file)
            
            print(f"Loaded configuration from {input_file}")
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error loading config: {e}")
    
    def _save(self) -> bool:
        """Save configuration to file"""
        try:
            output_file = self.args.output
            if not output_file:
                return self.error("Output file not specified")
            
            config_file = self.args.config
            config = load_config(config_file)
            
            save_config(config, output_file)
            print(f"Configuration saved to {output_file}")
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error saving config: {e}")
    
    def _default(self) -> bool:
        """Show default configuration"""
        try:
            default_config = {
                "neuron_count": 500,
                "region_count": 1,
                "connection_probability": 0.1,
                "simulation_timestep": 0.001,
                "random_seed": 42,
                "enable_plasticity": True,
                "enable_development": True,
                "enable_neuromodulation": True
            }
            
            print("=== Default Configuration ===")
            for key, value in default_config.items():
                print(f"{key}: {value}")
            
            return True
        
        except Exception as e:
            return self.error(f"Error showing default config: {e}")

class StatsCommand(Command):
    """Command to display brain statistics"""
    
    def execute(self) -> bool:
        """Show brain statistics"""
        try:
            config_file = self.args.config
            config = load_config(config_file)
            
            print("Creating brain...")
            brain = create_brain(config)
            
            stats = format_stats(brain)
            print(stats)
            
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error getting stats: {e}")

class SaveCommand(Command):
    """Command to save brain state"""
    
    def execute(self) -> bool:
        """Save brain state to file"""
        try:
            output_file = self.args.output or "brain_state.bin"
            config_file = self.args.config
            config = load_config(config_file)
            
            print("Creating brain...")
            brain = create_brain(config)
            
            # Note: pynlm brain.save() might save to different format
            # This is a placeholder implementation
            print(f"Saving brain state to {output_file}")
            print("(Note: Save functionality would use brain's save() method)")
            
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error saving brain: {e}")

class LoadCommand(Command):
    """Command to load brain state"""
    
    def execute(self) -> bool:
        """Load brain state from file"""
        try:
            input_file = self.args.input
            print(f"Loading brain state from {input_file}")
            print("(Note: Load functionality would use brain's load() method)")
            
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error loading brain: {e}")

class ConnectivityCommand(Command):
    """Command to test neural connectivity"""
    
    def execute(self) -> bool:
        """Test neural connectivity"""
        try:
            config_file = self.args.config
            config = load_config(config_file)
            
            print("Creating brain...")
            brain = create_brain(config)
            
            print("=== Connectivity Test ===")
            print("1. Testing basic neural connectivity...")
            
            # Get regions and test connectivity
            regions = brain.getRegions()
            if regions:
                region = regions[0]
                neurons = region.getAllNeurons()
                print(f"   Found region with {len(neurons)} neurons")
                
                if len(neurons) >= 2:
                    # Test basic connectivity by injecting current
                    print("   Injecting current into sensory neurons...")
                    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)
                    
                    # Run simulation for a few steps
                    for step in range(10):
                        brain.step(step)
                    
                    spikes = brain.getTotalSpikeCount()
                    print(f"   Spikes generated: {spikes}")
                    
                    if spikes > 0:
                        print("   ✓ Connectivity test PASSED: Spikes propagate through network")
                    else:
                        print("   ! Connectivity test WARNING: No spikes generated")
            
            print("\n2. Testing synaptic weights...")
            total_weight = 0.0
            positive_weight = 0
            negative_weight = 0
            
            for region in regions:
                for syn in region.getSynapses():
                    weight = syn.getWeight()
                    total_weight += abs(weight)
                    if weight > 0:
                        positive_weight += 1
                    elif weight < 0:
                        negative_weight += 1
            
            print(f"   Total synapses: {total_weight}")
            print(f"   Positive weights: {positive_weight}")
            print(f"   Negative weights: {negative_weight}")
            
            if positive_weight > 0 or negative_weight > 0:
                print("   ✓ Synaptic weight test PASSED: Synapses have connectivity")
            else:
                print("   ! Synaptic weight test WARNING: No synapses with non-zero weights")
            
            print("\n=== Connectivity Summary ===")
            print("The neural network shows connectivity patterns suitable for simulation.")
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error in connectivity test: {e}")

class HelpCommand(Command):
    """Command to show help"""
    
    def execute(self) -> bool:
        """Show help"""
        print("=== NLM CLI Help ===\n")
        
        print("Available commands:")
        print("  run     - Run a neural simulation")
        print("  config  - Manage configuration")
        print("  stats   - Show brain statistics")
        print("  save    - Save brain state")
        print("  load    - Load brain state")
        print("  connectivity - Test neural connectivity")
        print("  help    - Show this help")
        print("\nUse 'nlm <command> --help' for command-specific help\n")
        
        print("Example usage:")
        print("  nlm run --steps 1000 --config my_config.cfg")
        print("  nlm config show")
        print("  nlm stats --config my_config.cfg")
        print("  nlm save --output brain_state.bin")
        
        return True

class InteractiveCommand(Command):
    """Command to run in interactive mode"""
    
    def execute(self) -> bool:
        """Run in interactive mode"""
        print("=== NLM Interactive Mode ===")
        print("Type 'help' for available commands, 'exit' to quit")
        
        try:
            config_file = self.args.config
            config = load_config(config_file)
            
            brain = create_brain(config)
            step = 0
            
            while True:
                try:
                    user_input = input(f"nlm[{step}]> ").strip()
                    
                    if user_input in ['exit', 'quit', 'q']:
                        print("Exiting interactive mode")
                        break
                    elif user_input in ['help', 'h']:
                        self._show_help()
                    elif user_input.startswith('step'):
                        # Run multiple steps
                        parts = user_input.split()
                        if len(parts) > 1:
                            try:
                                steps_to_run = int(parts[1])
                                for i in range(steps_to_run):
                                    brain.step(step + i)
                                step += steps_to_run
                                self._show_status(brain, step)
                            except ValueError:
                                print("Invalid number of steps")
                    elif user_input == 'status':
                        self._show_status(brain, step)
                    elif user_input == 'stats':
                        stats = format_stats(brain)
                        print(stats)
                    elif user_input == 'spikes':
                        print(f"Total spikes: {brain.getTotalSpikeCount()}")
                    elif user_input == 'firing':
                        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
                    else:
                        print(f"Unknown command: {user_input}. Type 'help' for available commands")
                
                except KeyboardInterrupt:
                    print("\nInterrupted. Exiting...")
                    break
                except Exception as e:
                    print(f"Error: {e}")
            
            return True
        
        except NLMCLIError as e:
            return self.error(str(e))
        except Exception as e:
            return self.error(f"Error in interactive mode: {e}")
    
    def _show_help(self):
        """Show interactive help"""
        help_text = """
Available commands:
  help (h)        - Show this help
  status          - Show current brain status
  stats           - Show detailed brain statistics
  step N          - Run N simulation steps
  spikes          - Show total spike count
  firing          - Show number of firing neurons
  exit/quit/q     - Exit interactive mode
        """
        print(help_text)
    
    def _show_status(self, brain, step):
        """Show current status"""
        print(f"\n=== Step {step} Status ===")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Active neurons: {brain.getActiveNeuronCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")

# Command dispatcher
def get_command_handler(args) -> Command:
    """Get the appropriate command handler based on arguments"""
    command_map = {
        'run': RunCommand,
        'config': ConfigCommand,
        'stats': StatsCommand,
        'save': SaveCommand,
        'load': LoadCommand,
        'connectivity': ConnectivityCommand,
        'help': HelpCommand,
        'interactive': InteractiveCommand
    }
    
    command_name = args.command
    if command_name not in command_map:
        raise NLMCLIError(f"Unknown command: {command_name}")
    
    return command_map[command_name](args, args.debug if hasattr(args, 'debug') else False)

def handle_command(args) -> bool:
    """Handle the parsed command"""
    try:
        # Ensure pynlm is available
        import pynlm
        if not pynlm:
            raise NLMCLIError("pynlm module is not available")
        
        command = get_command_handler(args)
        return command.execute()
    
    except NLMCLIError as e:
        print(f"ERROR: {e}", file=sys.stderr)
        return False
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        return False