#!/usr/bin/env python3
"""
NLM CLI - Command Line Interface for NLM Brain Simulation Project
Main CLI implementation with comprehensive brain simulation and experiment management.

This module provides a full-featured command-line interface for the NLM (Neural Learning Machine) 
project, integrating with the C++ NLM brain simulation framework and Python bindings.
"""

import sys
import os
import json
import yaml
import argparse
import time
import subprocess
import threading
from pathlib import Path
from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass, field
from enum import Enum

# Add src directory to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

# Import NLM modules
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

class LogLevel(Enum):
    """Log levels for colored output."""
    ERROR = "ERROR"
    WARNING = "WARNING"
    INFO = "INFO"
    SUCCESS = "SUCCESS"
    DEBUG = "DEBUG"

class Color:
    """ANSI color codes for terminal output."""
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"
    DIM = "\033[2m"

    # Background colors
    BG_BLACK = "\033[40m"
    BG_RED = "\033[41m"
    BG_GREEN = "\033[42m"
    BG_YELLOW = "\033[43m"
    BG_BLUE = "\033[44m"
    BG_MAGENTA = "\033[45m"
    BG_CYAN = "\033[46m"
    BG_WHITE = "\033[47m"

    # Reset all attributes
    RESET_ALL = "\033[0m"
    
    # Combined colors
    GREEN = OKGREEN
    RED = FAIL
    YELLOW = WARNING
    BLUE = OKBLUE
    CYAN = OKCYAN
    MAGENTA = HEADER
    WHITE = "\033[37m"
    BLACK = "\033[30m"

@dataclass
class CommandResult:
    """Result of a command execution."""
    success: bool = True
    message: str = ""
    data: Dict[str, Any] = field(default_factory=dict)
    error: Optional[str] = None

class BaseCommand:
    """Base class for all CLI commands."""
    
    def __init__(self, name: str, description: str):
        self.name = name
        self.description = description
        self.args = argparse.ArgumentParser(description=description)
        self.setup_args()
    
    def setup_args(self):
        """Setup command arguments. Override in subclasses."""
        pass
    
    def execute(self, args: argparse.Namespace) -> CommandResult:
        """Execute the command. Override in subclasses."""
        return CommandResult(success=True, message="Command executed successfully")
    
    def get_help(self) -> str:
        """Get help text for the command."""
        help_text = f"{self.name}: {self.description}\n"
        help_text += f"Usage: nlm {self.name} [options]\n\n"
        
        # Add argument help
        try:
            help_text += self.args.format_help()
        except:
            help_text += "No additional options."
        
        return help_text

class BrainCommand(BaseCommand):
    """Brain simulation and control commands."""
    
    def __init__(self):
        super().__init__("brain", "Brain simulation and control commands")
        self.setup_args()
    
    def setup_args(self):
        """Setup brain command subcommands."""
        subparsers = self.args.add_subparsers(dest='brain_subcommand', help='Brain subcommand')
        
        # Create subcommand
        create_parser = subparsers.add_parser('create', help='Create a new brain simulation')
        create_parser.add_argument('--name', required=True, help='Name for the brain')
        create_parser.add_argument('--config', help='Configuration file path')
        
        # Load subcommand
        load_parser = subparsers.add_parser('load', help='Load a saved brain state')
        load_parser.add_argument('filepath', help='Path to saved brain file')
        
        # Save subcommand
        save_parser = subparsers.add_parser('save', help='Save brain state to file')
        save_parser.add_argument('filepath', help='Path to save brain file')
        
        # Run subcommand
        run_parser = subparsers.add_parser('run', help='Run simulation with parameters')
        run_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
        run_parser.add_argument('--config', help='Configuration file path')
        
        # Status subcommand
        status_parser = subparsers.add_parser('status', help='Show brain status and statistics')
        
        # Stats subcommand
        stats_parser = subparsers.add_parser('stats', help='Detailed statistics and metrics')
        stats_parser.add_argument('--format', choices=['json', 'table', 'text'], default='text', help='Output format')
        
        # Compare subcommand
        compare_parser = subparsers.add_parser('compare', help='Compare multiple brain states')
        compare_parser.add_argument('filepaths', nargs='+', help='Paths to brain files to compare')
    
    def execute(self, args: argparse.Namespace) -> CommandResult:
        """Execute brain command."""
        if not PYNLM_AVAILABLE:
            return CommandResult(
                success=False,
                error="pynlm is not available. Please install it first."
            )
        
        if not hasattr(args, 'brain_subcommand') or not args.brain_subcommand:
            return CommandResult(
                success=False,
                error="Please specify a brain subcommand (create, load, save, run, status, stats, compare)"
            )
        
        subcommand = args.brain_subcommand
        
        if subcommand == 'create':
            return self._brain_create(args)
        elif subcommand == 'load':
            return self._brain_load(args)
        elif subcommand == 'save':
            return self._brain_save(args)
        elif subcommand == 'run':
            return self._brain_run(args)
        elif subcommand == 'status':
            return self._brain_status(args)
        elif subcommand == 'stats':
            return self._brain_stats(args)
        elif subcommand == 'compare':
            return self._brain_compare(args)
        else:
            return CommandResult(
                success=False,
                error=f"Unknown brain subcommand: {subcommand}"
            )
    
    def _brain_create(self, args: argparse.Namespace) -> CommandResult:
        """Create a new brain simulation."""
        try:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            
            # Store brain in config
            config = {
                'brain_name': args.name,
                'brain': brain,
                'created_at': time.time()
            }
            
            # Save config to file if needed
            if args.config:
                with open(args.config, 'w') as f:
                    json.dump(config, f, indent=2)
            
            return CommandResult(
                success=True,
                message=f"Created brain '{args.name}' successfully"
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to create brain: {str(e)}"
            )
    
    def _brain_load(self, args: argparse.Namespace) -> CommandResult:
        """Load a saved brain state."""
        try:
            if not os.path.exists(args.filepath):
                return CommandResult(
                    success=False,
                    error=f"Brain file not found: {args.filepath}"
                )
            
            # Load brain from file
            brain = pynlm.loadBrain(args.filepath)
            
            return CommandResult(
                success=True,
                message=f"Loaded brain from '{args.filepath}' successfully"
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to load brain: {str(e)}"
            )
    
    def _brain_save(self, args: argparse.Namespace) -> CommandResult:
        """Save brain state to file."""
        try:
            # Save brain to file
            pynlm.saveBrain(args.filepath)
            
            return CommandResult(
                success=True,
                message=f"Saved brain to '{args.filepath}' successfully"
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to save brain: {str(e)}"
            )
    
    def _brain_run(self, args: argparse.Namespace) -> CommandResult:
        """Run simulation with parameters."""
        try:
            brain = pynlm.getBrain()
            if not brain:
                return CommandResult(
                    success=False,
                    error="No brain available. Create a brain first."
                )
            
            # Run simulation
            pynlm.runSimulation(brain, args.steps)
            
            return CommandResult(
                success=True,
                message=f"Ran simulation for {args.steps} steps successfully"
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to run simulation: {str(e)}"
            )
    
    def _brain_status(self, args: argparse.Namespace) -> CommandResult:
        """Show brain status and statistics."""
        try:
            brain = pynlm.getBrain()
            if not brain:
                return CommandResult(
                    success=False,
                    error="No brain available. Create a brain first."
                )
            
            # Get brain status
            status = pynlm.getBrainStatus(brain)
            
            # Format status output
            output = "Brain Status:\n"
            for key, value in status.items():
                output += f"  {key}: {value}\n"
            
            return CommandResult(
                success=True,
                message=output.strip()
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to get brain status: {str(e)}"
            )
    
    def _brain_stats(self, args: argparse.Namespace) -> CommandResult:
        """Get detailed statistics and metrics."""
        try:
            brain = pynlm.getBrain()
            if not brain:
                return CommandResult(
                    success=False,
                    error="No brain available. Create a brain first."
                )
            
            # Get brain statistics
            stats = pynlm.getBrainStats(brain)
            
            if args.format == 'json':
                return CommandResult(
                    success=True,
                    message=json.dumps(stats, indent=2)
                )
            else:
                # Format as table or text
                output = "Brain Statistics:\n"
                for category, data in stats.items():
                    output += f"\n{category.upper()}:\n"
                    if isinstance(data, dict):
                        for key, value in data.items():
                            output += f"  {key}: {value}\n"
                    elif isinstance(data, list):
                        for i, item in enumerate(data):
                            output += f"  [{i}]: {item}\n"
                
                return CommandResult(
                    success=True,
                    message=output.strip()
                )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to get brain statistics: {str(e)}"
            )
    
    def _brain_compare(self, args: argparse.Namespace) -> CommandResult:
        """Compare multiple brain states."""
        try:
            if len(args.filepaths) < 2:
                return CommandResult(
                    success=False,
                    error="Please provide at least 2 brain files to compare"
                )
            
            # Load all brains
            brains = []
            for filepath in args.filepaths:
                if not os.path.exists(filepath):
                    return CommandResult(
                        success=False,
                        error=f"Brain file not found: {filepath}"
                    )
                
                brain = pynlm.loadBrain(filepath)
                brains.append(brain)
            
            # Compare brains
            comparison = pynlm.compareBrains(brains)
            
            # Format comparison output
            output = "Brain Comparison Results:\n"
            for key, value in comparison.items():
                output += f"  {key}: {value}\n"
            
            return CommandResult(
                success=True,
                message=output.strip()
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to compare brains: {str(e)}"
            )
class ConfigCommand(BaseCommand):
    """Configuration management commands."""
    
    def __init__(self):
        super().__init__("config", "Configuration management commands")
        self.setup_args()
    
    def setup_args(self):
        """Setup config command subcommands."""
        subparsers = self.args.add_subparsers(dest='config_subcommand', help='Config subcommand')
        
        # List subcommand
        list_parser = subparsers.add_parser('list', help='List all configuration options')
        list_parser.add_argument('--format', choices=['json', 'table', 'text'], default='text', help='Output format')
        
        # Get subcommand
        get_parser = subparsers.add_parser('get', help='Get configuration value')
        get_parser.add_argument('key', help='Configuration key')
        
        # Set subcommand
        set_parser = subparsers.add_parser('set', help='Set configuration value')
        set_parser.add_argument('key', help='Configuration key')
        set_parser.add_argument('value', help='Configuration value')
        
        # Save subcommand
        save_parser = subparsers.add_parser('save', help='Save current configuration')
        save_parser.add_argument('filepath', help='Path to save configuration file')
        
        # Load subcommand
        load_parser = subparsers.add_parser('load', help='Load configuration from file')
        load_parser.add_argument('filepath', help='Path to configuration file')
        
        # Validate subcommand
        validate_parser = subparsers.add_parser('validate', help='Validate current configuration')
    
    def execute(self, args: argparse.Namespace) -> CommandResult:
        """Execute config command."""
        if not PYNLM_AVAILABLE:
            return CommandResult(
                success=False,
                error="pynlm is not available. Please install it first."
            )
        
        if not hasattr(args, 'config_subcommand') or not args.config_subcommand:
            return CommandResult(
                success=False,
                error="Please specify a config subcommand (list, get, set, save, load, validate)"
            )
        
        subcommand = args.config_subcommand
        
        if subcommand == 'list':
            return self._config_list(args)
        elif subcommand == 'get':
            return self._config_get(args)
        elif subcommand == 'set':
            return self._config_set(args)
        elif subcommand == 'save':
            return self._config_save(args)
        elif subcommand == 'load':
            return self._config_load(args)
        elif subcommand == 'validate':
            return self._config_validate(args)
        else:
            return CommandResult(
                success=False,
                error=f"Unknown config subcommand: {subcommand}"
            )
    
    def _config_list(self, args: argparse.Namespace) -> CommandResult:
        """List all configuration options."""
        try:
            config = pynlm.getConfig()
            
            if args.format == 'json':
                return CommandResult(
                    success=True,
                    message=json.dumps(dict(config), indent=2)
                )
            else:
                # Format as table or text
                output = "Configuration Options:\n"
                for key, value in dict(config).items():
                    output += f"  {key}: {value}\n"
                
                return CommandResult(
                    success=True,
                    message=output.strip()
                )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to list configuration options: {str(e)}"
            )
    
    def _config_get(self, args: argparse.Namespace) -> CommandResult:
        """Get configuration value."""
        try:
            config = pynlm.getConfig()
            
            if args.key not in dict(config):
                return CommandResult(
                    success=False,
                    error=f"Configuration key not found: {args.key}"
                )
            
            value = dict(config)[args.key]
            
            return CommandResult(
                success=True,
                message=f"{args.key}: {value}"
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to get configuration value: {str(e)}"
            )
    
    def _config_set(self, args: argparse.Namespace) -> CommandResult:
        """Set configuration value."""
        try:
            # Try to parse value as int, float, or string
            try:
                value = int(args.value)
            except ValueError:
                try:
                    value = float(args.value)
                except ValueError:
                    value = args.value
            
            pynlm.setConfig(args.key, value)
            
            return CommandResult(
                success=True,
                message=f"Set configuration {args.key} to {value} successfully"
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to set configuration value: {str(e)}"
            )
    
    def _config_save(self, args: argparse.Namespace) -> CommandResult:
        """Save current configuration."""
        try:
            config = pynlm.getConfig()
            
            with open(args.filepath, 'w') as f:
                json.dump(dict(config), f, indent=2)
            
            return CommandResult(
                success=True,
                message=f"Saved configuration to '{args.filepath}' successfully"
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to save configuration: {str(e)}"
            )
    
    def _config_load(self, args: argparse.Namespace) -> CommandResult:
        """Load configuration from file."""
        try:
            if not os.path.exists(args.filepath):
                return CommandResult(
                    success=False,
                    error=f"Configuration file not found: {args.filepath}"
                )
            
            # Load configuration from file
            with open(args.filepath, 'r') as f:
                if args.filepath.endswith('.json'):
                    config_data = json.load(f)
                elif args.filepath.endswith('.yaml') or args.filepath.endswith('.yml'):
                    config_data = yaml.safe_load(f)
                else:
                    return CommandResult(
                        success=False,
                        error="Unsupported configuration file format. Use .json or .yaml/.yml"
                    )
            
            pynlm.loadConfig(config_data)
            
            return CommandResult(
                success=True,
                message=f"Loaded configuration from '{args.filepath}' successfully"
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Failed to load configuration: {str(e)}"
            )
    
    def _config_validate(self, args: argparse.Namespace) -> CommandResult:
        """Validate current configuration."""
        try:
            pynlm.validateConfig()
            
            return CommandResult(
                success=True,
                message="Configuration is valid"
            )
        except Exception as e:
            return CommandResult(
                success=False,
                error=f"Configuration validation failed: {str(e)}"
            )
class ExploreCommand(BaseCommand):
    """Brain state exploration and analysis commands."""
    
    def __init__(self):
        super().__init__("explore", "Brain state exploration and analysis commands")
        self.setup_args()
    
    def setup_args(self):
        """Setup explore command subcommands."""
        subparsers = self.args.add_subparsers(dest='explore_subcommand', help='Explore subcommand')
        
        # Visualize subcommand
        visualize_parser = subparsers.add_parser('visualize', help='Generate visualizations of brain state')
        visualize_parser.add_argument('--type', choices=['neurons', 'synapses', 'memory', 'all'], default='all', help='Visualization type')
        visualize_parser.add_argument('--output', help='Output file path')
        visualize_parser.add_argument('--format', choices=['png', 'svg', 'pdf'], default='png', help='Output format')
        
        # Neurons subcommand
        neurons_parser = subparsers.add_parser('neurons', help='Analyze neuron population statistics')
        neurons_parser.add_argument('--type', choices=['all', 'sensory', 'motor', 'interneuron'], default='all', help='Neuron type')
        neurons_parser.add_argument('--format', choices=['json', 'table', 'text'], default='text', help='Output format')
        
        # Synapses subcommand
        synapses_parser = subparsers.add_parser('synapses', help='Analyze synapse connectivity and weights')
        synapses_parser.add_argument('--type', choices=['all', 'excitatory', 'inhibitory'], default='all', help='Synapse type')
        synapses_parser.add_argument('--format', choices=['json', 'table', 'text'], default='text', help='Output format')
        
        # Memory subcommand
        memory_parser = subparsers.add_parser('memory', help='Analyze memory system contents')
        memory_parser.add_argument('--type', choices=['all', 'working', 'episodic', 'associative'], default='all', help='Memory type')
        memory_parser.add_argument('--format', choices=['json', 'table', 'text'], default='text', help='Output format')
        
        # Simulate subcommand
        simulate_parser = subparsers.add_parser('simulate', help='Run simulation with analysis')
        simulate_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
        simulate_parser.add_argument('--analyze', action='store_true', help='Enable analysis during simulation')
        simulate_parser.add_argument('--output', help='Output file path')
class ExperimentCommand(BaseCommand):
    """Experiment execution and management commands."""
    
    def __init__(self):
        super().__init__("experiment", "Experiment execution and management commands")
        self.setup_args()
    
    def setup_args(self):
        """Setup experiment command subcommands."""
        subparsers = self.args.add_subparsers(dest='experiment_subcommand', help='Experiment subcommand')
        
        # Run subcommand
        run_parser = subparsers.add_parser('run', help='Run a named experiment')
        run_parser.add_argument('name', help='Name of the experiment to run')
        run_parser.add_argument('--config', help='Experiment configuration file path')
        
        # List subcommand
        list_parser = subparsers.add_parser('list', help='List available experiments')
        list_parser.add_argument('--format', choices=['json', 'table', 'text'], default='text', help='Output format')
        
        # Create subcommand
        create_parser = subparsers.add_parser('create', help='Create new experiment config')
        create_parser.add_argument('name', help='Name for the new experiment')
        create_parser.add_argument('--config', help='Configuration file path')
        
        # Export subcommand
        export_parser = subparsers.add_parser('export', help='Export experiment to file')
        export_parser.add_argument('name', help='Name of the experiment to export')
        export_parser.add_argument('filepath', help='Path to export file')
        
        # Import subcommand
        import_parser = subparsers.add_parser('import', help='Import experiment from file')
        import_parser.add_argument('filepath', help='Path to import file')
class PythonCommand(BaseCommand):
    """Python scripting commands for pynlm."""
    
    def __init__(self):
        super().__init__("python", "Python scripting commands for pynlm")
        self.setup_args()
    
    def setup_args(self):
        """Setup python command subcommands."""
        subparsers = self.args.add_subparsers(dest='python_subcommand', help='Python subcommand')
        
        # Run subcommand
        run_parser = subparsers.add_parser('run', help='Execute Python script with pynlm')
        run_parser.add_argument('script', help='Path to Python script')
        run_parser.add_argument('--args', help='Additional arguments for the script')
        
        # Interactive subcommand
        interactive_parser = subparsers.add_parser('interactive', help='Start Python interactive session')
        
        # Benchmark subcommand
        benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmarks')
        benchmark_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
        benchmark_parser.add_argument('--repeats', type=int, default=5, help='Number of benchmark runs')
class ToolsCommand(BaseCommand):
    """Utility and debugging tools."""
    
    def __init__(self):
        super().__init__("tools", "Utility and debugging tools")
        self.setup_args()
    
    def setup_args(self):
        """Setup tools command subcommands."""
        subparsers = self.args.add_subparsers(dest='tools_subcommand', help='Tools subcommand')
        
        # Benchmark subcommand
        benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmarks')
        benchmark_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
        benchmark_parser.add_argument('--format', choices=['json', 'table', 'text'], default='text', help='Output format')
        
        # Profile subcommand
        profile_parser = subparsers.add_parser('profile', help='Profile simulation performance')
        profile_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
        
        # Compare subcommand
        compare_parser = subparsers.add_parser('compare', help='Compare different configurations')
        compare_parser.add_argument('configs', nargs='+', help='Paths to configuration files to compare')
        
        # Analyze subcommand
        analyze_parser = subparsers.add_parser('analyze', help='Analyze simulation results')
        analyze_parser.add_argument('filepaths', nargs='+', help='Paths to simulation result files')
        
        # Replay subcommand
        replay_parser = subparsers.add_parser('replay', help='Replay saved simulation data')
        replay_parser.add_argument('filepath', help='Path to simulation data file')
        replay_parser.add_argument('--steps', type=int, default=-1, help='Number of steps to replay (default: all)')
        
        # Stats subcommand
        stats_parser = subparsers.add_parser('stats', help='Generate summary statistics')
        stats_parser.add_argument('--input', help='Input file path')
        stats_parser.add_argument('--output', help='Output file path')
        stats_parser.add_argument('--format', choices=['json', 'table', 'text'], default='text', help='Output format')
class HelpCommand(BaseCommand):
    """Help and documentation commands."""
    
    def __init__(self):
        super().__init__("help", "Show help and documentation")
        self.setup_args()
    
    def setup_args(self):
        """Setup help command."""
        self.args.add_argument('topic', nargs='?', help='Help topic')
        self.args.add_argument('--format', choices=['text', 'json'], default='text', help='Output format')
    
    def execute(self, args: argparse.Namespace) -> CommandResult:
        """Execute help command."""
        if args.topic:
            # Show help for specific topic
            topic = args.topic.lower()
            
            if topic == "brain":
                return CommandResult(
                    success=True,
                    message=BrainCommand("brain", "Brain simulation and control commands").get_help()
                )
            elif topic == "config":
                return CommandResult(
                    success=True,
                    message=ConfigCommand("config", "Configuration management commands").get_help()
                )
            elif topic == "explore":
                return CommandResult(
                    success=True,
                    message=ExploreCommand("explore", "Brain state exploration and analysis commands").get_help()
                )
            elif topic == "experiment":
                return CommandResult(
                    success=True,
                    message=ExperimentCommand("experiment", "Experiment execution and management commands").get_help()
                )
            elif topic == "python":
                return CommandResult(
                    success=True,
                    message=PythonCommand("python", "Python scripting commands for pynlm").get_help()
                )
            elif topic == "tools":
                return CommandResult(
                    success=True,
                    message=ToolsCommand("tools", "Utility and debugging tools").get_help()
                )
            elif topic == "help":
                return CommandResult(
                    success=True,
                    message=HelpCommand("help", "Show help and documentation").get_help()
                )
            else:
                return CommandResult(
                    success=False,
                    error=f"Unknown help topic: {topic}"
                )
        else:
            # Show general help
            help_text = "NLM CLI - Command Line Interface for NLM Brain Simulation Project\n\n"
            help_text += "Available commands:\n"
            help_text += "  brain     - Brain simulation and control commands\n"
            help_text += "  config    - Configuration management commands\n"
            help_text += "  experiment - Experiment execution and management commands\n"
            help_text += "  explore   - Brain state exploration and analysis commands\n"
            help_text += "  python    - Python scripting commands for pynlm\n"
            help_text += "  tools     - Utility and debugging tools\n"
            help_text += "  help      - Show help and documentation\n\n"
            help_text += "For more information about a specific command, use:\n"
            help_text += "  nlm help <command>\n"
            
            return CommandResult(
                success=True,
                message=help_text
            )
class NLMCLI:
    """Main NLM CLI application."""
    
    def __init__(self):
        self.commands = {
            'brain': BrainCommand(),
            'config': ConfigCommand(),
            'experiment': ExperimentCommand(),
            'explore': ExploreCommand(),
            'python': PythonCommand(),
            'tools': ToolsCommand(),
            'help': HelpCommand()
        }
        self.verbose = False
        self.quiet = False
    
    def run(self):
        """Run the CLI application."""
        # Parse command line arguments
        parser = argparse.ArgumentParser(
            description="NLM CLI - Command Line Interface for NLM Brain Simulation Project",
            formatter_class=argparse.RawDescriptionHelpFormatter
        )
        
        parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose output')
        parser.add_argument('--quiet', '-q', action='store_true', help='Suppress non-essential output')
        parser.add_argument('--config', help='Configuration file path')
        parser.add_argument('--output', help='Output file path')
        
        parser.add_argument('command', nargs='?', help='Command to execute')
        parser.add_argument('args', nargs='*', help='Additional arguments')
        
        args = parser.parse_args()
        
        # Set verbose and quiet flags
        self.verbose = args.verbose
        self.quiet = args.quiet
        
        # If no command provided, show help
        if not args.command:
            self._show_help()
            return
        
        # Check if command exists
        if args.command not in self.commands:
            self._print_error(f"Unknown command: {args.command}")
            self._show_help()
            return
        
        # Execute the command
        command = self.commands[args.command]
        
        # Parse subcommand arguments
        try:
            subcommand_args = command.args.parse_args(args.args)
        except SystemExit:
            return
        
        # Execute command with additional context
        result = command.execute(subcommand_args)
        
        # Print result
        if result.success:
            if self.verbose or not self.quiet:
                self._print_success(result.message)
        else:
            self._print_error(result.error if result.error else result.message)
    
    def _print_success(self, message: str):
        """Print success message."""
        print(f"{Color.GREEN}✓ {message}{Color.ENDC}")
    
    def _print_error(self, message: str):
        """Print error message."""
        print(f"{Color.RED}✗ {message}{Color.ENDC}")
    
    def _print_warning(self, message: str):
        """Print warning message."""
        print(f"{Color.YELLOW}⚠ {message}{Color.ENDC}")
    
    def _print_info(self, message: str):
        """Print info message."""
        print(f"{Color.CYAN}ℹ {message}{Color.ENDC}")
    
    def _show_help(self):
        """Show general help."""
        help_text = """
NLM CLI - Command Line Interface for NLM Brain Simulation Project

Usage: nlm [options] <command> [args]

Options:
  --verbose, -v    Enable verbose output
  --quiet, -q      Suppress non-essential output
  --config <file>  Configuration file path
  --output <file>  Output file path

Commands:
  brain     Brain simulation and control commands
  config    Configuration management commands
  experiment Experiment execution and management commands
  explore   Brain state exploration and analysis commands
  python    Python scripting commands for pynlm
  tools     Utility and debugging tools
  help      Show help and documentation

For more information about a specific command, use:
  nlm help <command>

Examples:
  nlm brain create --name mybrain
  nlm brain run --steps 1000
  nlm config list
  nlm explore neurons
  nlm experiment run learning_test
"""
        print(help_text)
if __name__ == "__main__":
    cli = NLMCLI()
    cli.run()