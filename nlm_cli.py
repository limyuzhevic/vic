#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI) Enhancement Script

This script enhances the NLM command-line interface to provide:
- Better command organization and discovery
- Advanced command options and flags
- Interactive mode support
- Configuration management
- Performance monitoring
- Help and documentation system

The CLI follows Unix philosophy principles:
- Each command has a clear, single purpose
- Commands can be combined and piped
- Output is machine-parseable where possible
- Help is comprehensive and context-sensitive
"""

import argparse
import json
import os
import sys
import time
from pathlib import Path
from typing import Dict, List, Any, Optional, Callable
import tabulate

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


class NLMCommandError(Exception):
    """Base exception for NLM CLI errors"""
    pass


class CommandNotFoundError(NLMCommandError):
    """Raised when a command is not found"""
    pass


class CommandExecutionError(NLMCommandError):
    """Raised when command execution fails"""
    pass


class NLMCLI:
    """Enhanced NLM Command Line Interface"""
    
    def __init__(self):
        self.version = "Phase 6.2 - Enhanced Integration"
        self.commands = {}
        self.aliases = {}
        self.config_file = "nlm_config.json"
        self.history_file = "nlm_history.txt"
        self.checkpoints_dir = "nlm_checkpoints"
        self.ensure_directories()
        
        # Initialize default configuration
        self.default_config = {
            "neuron_count": 1000,
            "region_count": 2,
            "simulation_timestep": 0.001,
            "connection_probability": 0.1,
            "random_seed": 42,
            "enable_reward_modulation": True,
            "enable_curiosity": True,
            "enable_development": True,
            "enable_structural_plasticity": True,
            "log_level": "INFO"
        }
        
        self.load_config()
    
    def ensure_directories(self):
        """Ensure required directories exist"""
        os.makedirs(self.checkpoints_dir, exist_ok=True)
        os.makedirs("docs", exist_ok=True)
        os.makedirs("configs", exist_ok=True)
    
    def load_config(self):
        """Load configuration from file"""
        if os.path.exists(self.config_file):
            try:
                with open(self.config_file, 'r') as f:
                    file_config = json.load(f)
                self.default_config.update(file_config)
            except Exception as e:
                print(f"Warning: Could not load config file: {e}")
    
    def save_config(self):
        """Save configuration to file"""
        try:
            with open(self.config_file, 'w') as f:
                json.dump(self.default_config, f, indent=2)
        except Exception as e:
            print(f"Warning: Could not save config file: {e}")
    
    def register_command(self, name: str, func: Callable, help_text: str, 
                        category: str = "general", args: Optional[argparse.ArgumentParser] = None):
        """Register a new command"""
        self.commands[name] = {
            'func': func,
            'help': help_text,
            'category': category,
            'parser': args or self._create_default_parser()
        }
    
    def register_alias(self, alias: str, command: str):
        """Register a command alias"""
        self.aliases[alias] = command
    
    def _create_default_parser(self):
        """Create default argument parser"""
        parser = argparse.ArgumentParser(add_help=False)
        parser.add_argument('--help', action='store_true', help='Show this help message')
        parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
        parser.add_argument('--quiet', '-q', action='store_true', help='Quiet output')
        return parser
    
    def get_help(self, command_name: str = None):
        """Get help for a command or list all commands"""
        if command_name:
            # Show help for specific command
            if command_name in self.commands:
                cmd = self.commands[command_name]
                print(f"Command: {command_name}")
                print(f"Category: {cmd['category']}")
                print(f"Description: {cmd['help']}")
                print("\nUsage:")
                cmd['parser'].print_help()
            elif command_name in self.aliases:
                print(f"Alias: {command_name}")
                print(f"Refers to: {self.aliases[command_name]}")
                self.get_help(self.aliases[command_name])
            else:
                print(f"Command not found: {command_name}")
                self.list_categories()
        else:
            # Show all commands
            self.list_commands()
    
    def list_categories(self):
        """List all command categories"""
        categories = set()
        for cmd in self.commands.values():
            categories.add(cmd['category'])
        
        print("Available Categories:")
        for category in sorted(categories):
            print(f"  {category}")
            # Count commands in category
            count = sum(1 for cmd in self.commands.values() if cmd['category'] == category)
            print(f"    ({count} commands)")
        
        print("\nUse: nlm help <category> to see commands in a category")
    
    def list_commands(self, category: str = None):
        """List commands, optionally filtered by category"""
        if category:
            # Filter by category
            matching_commands = [(name, cmd) for name, cmd in self.commands.items() 
                               if cmd['category'] == category]
            if not matching_commands:
                print(f"No commands found in category: {category}")
                return
            
            print(f"Commands in category '{category}':")
            for name, cmd in sorted(matching_commands):
                aliases = [a for a, c in self.aliases.items() if c == name]
                alias_str = f" ({', '.join(aliases)})" if aliases else ""
                print(f"  {name}{alias_str}: {cmd['help']}")
        else:
            # List all commands with categories
            print("All NLM Commands:")
            current_category = None
            for name, cmd in sorted(self.commands.items()):
                if cmd['category'] != current_category:
                    current_category = cmd['category']
                    print(f"\n{current_category.upper()}:")
                aliases = [a for a, c in self.aliases.items() if c == name]
                alias_str = f" ({', '.join(aliases)})" if aliases else ""
                print(f"  {name}{alias_str}: {cmd['help']}")
    
    def execute(self, args: List[str]) -> int:
        """Execute a command"""
        if not args:
            self.get_help()
            return 0
        
        command_name = args[0]
        
        # Handle help requests
        if command_name in ('help', '--help', '-h'):
            if len(args) > 1:
                self.get_help(args[1])
            else:
                self.get_help()
            return 0
        
        # Handle version request
        if command_name == 'version':
            print(f"NLM {self.version}")
            return 0
        
        # Check if it's an alias
        if command_name in self.aliases:
            command_name = self.aliases[command_name]
        
        # Check if command exists
        if command_name not in self.commands:
            print(f"Error: Command not found: {command_name}")
            self.list_categories()
            return 1
        
        try:
            # Get the command parser and parse arguments
            cmd = self.commands[command_name]
            parsed_args = cmd['parser'].parse_args(args[1:])
            
            # Execute command
            result = cmd['func'](parsed_args)
            return result if isinstance(result, int) else 0
            
        except Exception as e:
            print(f"Error executing command '{command_name}': {e}")
            if '--verbose' in args:
                import traceback
                traceback.print_exc()
            return 1
    
    def interactive_mode(self):
        """Enter interactive CLI mode"""
        print("NLM Interactive Mode")
        print("Type 'help' for commands, 'quit' or 'exit' to leave")
        
        history = []
        
        while True:
            try:
                # Get command from user
                user_input = input("nlm> ").strip()
                
                if not user_input:
                    continue
                
                # Check for exit commands
                if user_input.lower() in ('quit', 'exit', 'q'):
                    print("Goodbye!")
                    break
                
                # Parse command
                args = user_input.split()
                command = args[0]
                
                # Check if it's an alias
                if command in self.aliases:
                    command = self.aliases[command]
                    args[0] = command
                
                # Execute command
                print(f"Executing: {' '.join(args)}")
                result = self.execute(args)
                
                if result != 0:
                    print(f"Command failed with exit code: {result}")
                
                # Save to history
                history.append(user_input)
                
            except KeyboardInterrupt:
                print("\nInterrupted. Type 'quit' to exit.")
                continue
            except Exception as e:
                print(f"Error: {e}")
                continue


class CreateBrainCommand:
    """Command for creating and configuring brains"""
    
    def __init__(self, cli: NLMCLI):
        self.cli = cli
    
    def __call__(self, args):
        """Create a brain"""
        verbose = getattr(args, 'verbose', False)
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply command-line or config file settings
        config.set("neuron_count", self.cli.default_config.get("neuron_count", 1000))
        config.set("region_count", self.cli.default_config.get("region_count", 2))
        config.set("connection_probability", 
                  self.cli.default_config.get("connection_probability", 0.1))
        config.set("simulation_timestep", 
                  self.cli.default_config.get("simulation_timestep", 0.001))
        config.set("random_seed", self.cli.default_config.get("random_seed", 42))
        
        # Create brain
        brain = pynlm.createBrain(config)
        
        if verbose:
            print(f"Creating brain with {config.getOr('neuron_count', 1000)} neurons...")
            print(f"Regions: {config.getOr('region_count', 2)}")
            print(f"Connection probability: {config.getOr('connection_probability', 0.1)}")
        
        # Initialize brain
        if not brain.initialize():
            raise CommandExecutionError("Failed to initialize brain")
        
        # Save if requested
        if hasattr(args, 'save') and args.save:
            if brain.save(args.save):
                print(f"✓ Brain saved to: {args.save}")
            else:
                raise CommandExecutionError(f"Failed to save brain to {args.save}")
        
        # Display brain statistics
        self._display_brain_stats(brain, verbose)
        
        return 0
    
    def _display_brain_stats(self, brain, verbose):
        """Display brain statistics"""
        print(f"\nBrain created successfully:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Regions: {brain.getRegionCount()}")
        print(f"  Synapses: {brain.getTotalSynapseCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        
        if verbose:
            print(f"\nAdditional statistics:")
            print(f"  Active neurons: {brain.getActiveNeuronCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Pending spike events: {brain.getPendingSpikeEventCount()}")


class RunSimulationCommand:
    """Command for running simulations"""
    
    def __init__(self, cli: NLMCLI):
        self.cli = cli
    
    def __call__(self, args):
        """Run a simulation"""
        verbose = getattr(args, 'verbose', False)
        steps = args.steps
        
        # Load or create brain
        brain = self._load_or_create_brain(args, verbose)
        
        # Create world if requested
        world = None
        agent = None
        
        if getattr(args, 'with_world', False):
            world = pynlm.createSimpleWorld()
            world.configure(
                getattr(args, 'width', 20),
                getattr(args, 'height', 20),
                getattr(args, 'vision_width', 8),
                getattr(args, 'vision_height', 8)
            )
            world.reset()
            
            if getattr(args, 'with_agent', False):
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                
                # Enable features if requested
                if getattr(args, 'reward_modulation', False):
                    agent.enableRewardModulation(True)
                if getattr(args, 'curiosity', False):
                    agent.enableCuriosity(True)
                if getattr(args, 'development', False):
                    agent.enableDevelopment(True)
                if getattr(args, 'structural_plasticity', False):
                    agent.enableStructuralPlasticity(True)
        
        # Run simulation
        if verbose:
            print(f"\nRunning simulation for {steps} steps...")
            if world:
                print(f"  World: {world.getWidth()}x{world.getHeight()}")
            if agent:
                print(f"  Agent: enabled features: "
                      f"reward={getattr(args, 'reward_modulation', False)}, "
                      f"curiosity={getattr(args, 'curiosity', False)}, "
                      f"development={getattr(args, 'development', False)}, "
                      f"plasticity={getattr(args, 'structural_plasticity', False)}")
        
        start_time = time.time()
        
        for step in range(steps):
            # Update world
            if world:
                world.update(0.1)
                
                if agent:
                    # Get sensory input
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Action selection
            if agent:
                action = agent.decodeMotorCommand()
                if world:
                    world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log progress
            if step % max(1, steps // 10) == 0 or step == steps - 1:
                progress = (step / steps) * 100
                print(f"  Progress: {progress:.0f}% | "
                      f"Spikes: {brain.getTotalSpikeCount()} | "
                      f"Firing: {brain.getFiringNeuronCount()}")
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Display final results
        self._display_simulation_results(brain, agent, duration, verbose)
        
        # Save brain if requested
        if hasattr(args, 'save') and args.save:
            if brain.save(args.save):
                print(f"✓ Brain state saved to: {args.save}")
            else:
                raise CommandExecutionError(f"Failed to save brain to {args.save}")
        
        return 0
    
    def _load_or_create_brain(self, args, verbose):
        """Load existing brain or create new one"""
        if hasattr(args, 'load') and args.load:
            if verbose:
                print(f"Loading brain from: {args.load}")
            # Note: This would require implementing brain loading
            # For now, create a new brain
            print(f"Warning: Loading not fully implemented, creating new brain")
        
        # Create new brain
        config = pynlm.createDefaultConfig()
        
        # Apply settings
        config.set("neuron_count", self.cli.default_config.get("neuron_count", 1000))
        config.set("region_count", self.cli.default_config.get("region_count", 2))
        config.set("random_seed", self.cli.default_config.get("random_seed", 42))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    def _display_simulation_results(self, brain, agent, duration, verbose):
        """Display simulation results"""
        print(f"\n✓ Simulation completed!")
        print(f"  Duration: {duration:.2f} seconds")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        
        if agent:
            print(f"\nAgent state:")
            print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
        
        if verbose:
            print(f"\nDetailed statistics:")
            print(f"  Total neurons: {brain.getTotalNeuronCount()}")
            print(f"  Total synapses: {brain.getTotalSynapseCount()}")
            print(f"  Active neurons: {brain.getActiveNeuronCount()}")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")


class AnalyzeBrainCommand:
    """Command for analyzing brain states"""
    
    def __init__(self, cli: NLMCLI):
        self.cli = cli
    
    def __call__(self, args):
        """Analyze brain state"""
        brain_file = args.file
        output_file = getattr(args, 'output', None)
        
        print(f"Analyzing brain state from: {brain_file}")
        
        # Load brain (this would need to be implemented)
        # For now, show a message
        print("Note: Brain loading and analysis requires implementation")
        print(f"To analyze {brain_file}, implement the loading functionality")
        
        if output_file:
            print(f"To save analysis results, create {output_file}")
        
        return 0


class ConfigCommand:
    """Command for configuration management"""
    
    def __init__(self, cli: NLMCLI):
        self.cli = cli
    
    def __call__(self, args):
        """Handle configuration commands"""
        subcommand = getattr(args, 'subcommand', None)
        
        if not subcommand:
            print("Configuration commands:")
            print("  show    - Show current configuration")
            print("  set     - Set a configuration value")
            print("  unset   - Remove a configuration value")
            print("  save    - Save configuration to file")
            print("  load    - Load configuration from file")
            print("  reset   - Reset to defaults")
            return 0
        
        if subcommand == 'show':
            return self._show_config(args)
        elif subcommand == 'set':
            return self._set_config(args)
        elif subcommand == 'unset':
            return self._unset_config(args)
        elif subcommand == 'save':
            return self._save_config(args)
        elif subcommand == 'load':
            return self._load_config(args)
        elif subcommand == 'reset':
            return self._reset_config(args)
        else:
            print(f"Unknown configuration command: {subcommand}")
            return 1
    
    def _show_config(self, args):
        """Show current configuration"""
        print("Current NLM Configuration:")
        for key, value in self.cli.default_config.items():
            print(f"  {key}: {value}")
        return 0
    
    def _set_config(self, args):
        """Set a configuration value"""
        if not hasattr(args, 'key') or not hasattr(args, 'value'):
            print("Usage: nlm config set <key> <value>")
            return 1
        
        key = args.key
        value = args.value
        
        # Parse value based on key
        if key in ['neuron_count', 'region_count', 'random_seed']:
            try:
                value = int(value)
            except ValueError:
                print(f"Error: {key} must be an integer")
                return 1
        elif key in ['connection_probability', 'simulation_timestep']:
            try:
                value = float(value)
            except ValueError:
                print(f"Error: {key} must be a float")
                return 1
        
        self.cli.default_config[key] = value
        print(f"✓ Set {key} = {value}")
        return 0
    
    def _unset_config(self, args):
        """Remove a configuration value"""
        if not hasattr(args, 'key'):
            print("Usage: nlm config unset <key>")
            return 1
        
        key = args.key
        if key in self.cli.default_config:
            del self.cli.default_config[key]
            print(f"✓ Removed {key}")
        else:
            print(f"Key not found: {key}")
        
        return 0
    
    def _save_config(self, args):
        """Save configuration to file"""
        filename = getattr(args, 'filename', self.cli.config_file)
        
        try:
            with open(filename, 'w') as f:
                json.dump(self.cli.default_config, f, indent=2)
            print(f"✓ Configuration saved to: {filename}")
            return 0
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return 1
    
    def _load_config(self, args):
        """Load configuration from file"""
        filename = getattr(args, 'filename', self.cli.config_file)
        
        try:
            with open(filename, 'r') as f:
                loaded_config = json.load(f)
            
            self.cli.default_config.update(loaded_config)
            print(f"✓ Configuration loaded from: {filename}")
            return 0
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return 1
    
    def _reset_config(self, args):
        """Reset configuration to defaults"""
        self.cli.default_config.clear()
        self.cli.default_config.update({
            "neuron_count": 1000,
            "region_count": 2,
            "simulation_timestep": 0.001,
            "connection_probability": 0.1,
            "random_seed": 42,
            "enable_reward_modulation": True,
            "enable_curiosity": True,
            "enable_development": True,
            "enable_structural_plasticity": True,
            "log_level": "INFO"
        })
        print("✓ Configuration reset to defaults")
        return 0


class ExperimentCommand:
    """Command for running experiments"""
    
    def __init__(self, cli: NLMCLI):
        self.cli = cli
    
    def __call__(self, args):
        """Run an experiment"""
        experiment_name = args.experiment_name
        
        print(f"Running experiment: {experiment_name}")
        print("Note: Experiment execution requires implementation")
        print(f"To run {experiment_name}, implement the experiment framework")
        
        return 0


class InteractiveCommand:
    """Command to enter interactive mode"""
    
    def __init__(self, cli: NLMCLI):
        self.cli = cli
    
    def __call__(self, args):
        """Enter interactive mode"""
        print("Entering NLM Interactive Mode...")
        print("Type 'help' for commands, 'quit' to exit")
        self.cli.interactive_mode()
        return 0


class ListCommandsCommand:
    """Command to list all commands"""
    
    def __init__(self, cli: NLMCLI):
        self.cli = cli
    
    def __call__(self, args):
        """List all commands"""
        self.cli.list_commands()
        return 0


class ListCategoriesCommand:
    """Command to list all categories"""
    
    def __init__(self, cli: NLMCLI):
        self.cli = cli
    
    def __call__(self, args):
        """List all categories"""
        self.cli.list_categories()
        return 0


class VersionCommand:
    """Command to show version"""
    
    def __init__(self, cli: NLMCLI):
        self.cli = cli
    
    def __call__(self, args):
        """Show version information"""
        print(f"NLM {self.cli.version}")
        print("Neural Learning Machine - Phase 6 Enhanced Integration")
        return 0


class NLMCLIBuilder:
    """Builder class for creating NLM CLI with enhanced features"""
    
    def __init__(self):
        self.cli = NLMCLI()
    
    def add_core_commands(self):
        """Add core NLM commands"""
        # Create command parser for core commands
        create_parser = argparse.ArgumentParser(add_help=False, description="Create and initialize a brain")
        create_parser.add_argument('--save', help='Save brain state to file')
        create_parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
        
        self.cli.register_command(
            'create',
            CreateBrainCommand(self.cli),
            "Create and initialize a new brain",
            "brain",
            create_parser
        )
        
        # Simulate command parser
        sim_parser = argparse.ArgumentParser(add_help=False, description="Run a simulation")
        sim_parser.add_argument('steps', type=int, help='Number of simulation steps')
        sim_parser.add_argument('--load', help='Load brain state from file')
        sim_parser.add_argument('--with-world', action='store_true', help='Run with world simulation')
        sim_parser.add_argument('--with-agent', action='store_true', help='Run with agent brain')
        sim_parser.add_argument('--reward-modulation', action='store_true', help='Enable reward modulation')
        sim_parser.add_argument('--curiosity', action='store_true', help='Enable curiosity')
        sim_parser.add_argument('--development', action='store_true', help='Enable development')
        sim_parser.add_argument('--structural-plasticity', action='store_true', help='Enable structural plasticity')
        sim_parser.add_argument('--width', type=int, default=20, help='World width')
        sim_parser.add_argument('--height', type=int, default=20, help='World height')
        sim_parser.add_argument('--vision-width', type=int, default=8, help='Vision width')
        sim_parser.add_argument('--vision-height', type=int, default=8, help='Vision height')
        sim_parser.add_argument('--save', help='Save brain state to file')
        sim_parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
        
        self.cli.register_command(
            'simulate',
            RunSimulationCommand(self.cli),
            "Run a brain simulation",
            "simulation",
            sim_parser
        )
        
        # Analyze command parser
        analyze_parser = argparse.ArgumentParser(add_help=False, description="Analyze brain state")
        analyze_parser.add_argument('file', help='Brain state file to analyze')
        analyze_parser.add_argument('-o', '--output', help='Output report file')
        
        self.cli.register_command(
            'analyze',
            AnalyzeBrainCommand(self.cli),
            "Analyze brain state from checkpoint",
            "analysis",
            analyze_parser
        )
        
        # Configuration command parser
        config_parser = argparse.ArgumentParser(add_help=False, description="Configuration management")
        config_parser.add_argument('subcommand', help='Configuration subcommand')
        config_parser.add_argument('--filename', help='Configuration file name')
        config_parser.add_argument('--key', help='Configuration key')
        config_parser.add_argument('--value', help='Configuration value')
        
        self.cli.register_command(
            'config',
            ConfigCommand(self.cli),
            "Manage NLM configuration",
            "configuration",
            config_parser
        )
        
        # Experiment command parser
        exp_parser = argparse.ArgumentParser(add_help=False, description="Run an experiment")
        exp_parser.add_argument('experiment_name', help='Name of experiment to run')
        
        self.cli.register_command(
            'experiment',
            ExperimentCommand(self.cli),
            "Run a predefined experiment",
            "experiments",
            exp_parser
        )
    
    def add_helper_commands(self):
        """Add helper commands"""
        # Interactive mode
        self.cli.register_command(
            'interactive',
            InteractiveCommand(self.cli),
            "Enter interactive CLI mode",
            "tools"
        )
        
        # List commands
        self.cli.register_command(
            'list',
            ListCommandsCommand(self.cli),
            "List all available commands",
            "tools"
        )
        
        # List categories
        self.cli.register_command(
            'categories',
            ListCategoriesCommand(self.cli),
            "List all command categories",
            "tools"
        )
        
        # Version
        self.cli.register_command(
            'version',
            VersionCommand(self.cli),
            "Show version information",
            "tools"
        )
    
    def add_aliases(self):
        """Add command aliases"""
        # Common aliases
        self.cli.register_alias('cr', 'create')
        self.cli.register_alias('sim', 'simulate')
        self.cli.register_alias('analy', 'analyze')
        self.cli.register_alias('conf', 'config')
        self.cli.register_alias('exp', 'experiment')
        self.cli.register_alias('ls', 'list')
        self.cli.register_alias('cat', 'categories')
        self.cli.register_alias('ver', 'version')
        self.cli.register_alias('i', 'interactive')
    
    def build(self) -> NLMCLI:
        """Build and return the enhanced CLI"""
        self.add_core_commands()
        self.add_helper_commands()
        self.add_aliases()
        return self.cli


def main():
    """Main entry point"""
    print("NLM Enhanced Command Line Interface")
    print("=" * 50)
    
    # Build the enhanced CLI
    cli_builder = NLMCLIBuilder()
    cli = cli_builder.build()
    
    # Get command line arguments
    args = sys.argv[1:]
    
    # Handle special cases
    if not args:
        print("No command provided. Use 'nlm help' for help.")
        cli.get_help()
        return 0
    
    if args[0] in ('--help', '-h', 'help'):
        if len(args) > 1:
            cli.get_help(args[1])
        else:
            cli.get_help()
        return 0
    
    # Execute command
    return cli.execute(args)


if __name__ == '__main__':
    sys.exit(main())
