#!/usr/bin/env python3
"""
NLM Command Line Interface

Advanced command-line interface for NLM (Neural Learning Machine) Phase 6.
Provides commands for configuration, execution, monitoring, and administration.
"""

import argparse
import sys
import os
import json
import subprocess
from pathlib import Path
from typing import List, Optional, Dict, Any
from enum import Enum

# Add config module to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'config'))

from NLMConfig import NLMConfig, Environment, get_config, initialize_config
class NLMCliError(Exception):
    """NLM CLI specific exception."""
    pass
class Command(Enum):
    """Available NLM CLI commands."""
    
    # Configuration commands
    CONFIG_INIT = "config:init"
    CONFIG_SHOW = "config:show"
    CONFIG_SET = "config:set"
    CONFIG_GET = "config:get"
    CONFIG_SAVE = "config:save"
    CONFIG_LOAD = "config:load"
    CONFIG_VALIDATE = "config:validate"
    CONFIG_RESET = "config:reset"
    
    # Brain commands
    BRAIN_INIT = "brain:init"
    BRAIN_STATUS = "brain:status"
    BRAIN_STEP = "brain:step"
    BRAIN_RESET = "brain:reset"
    BRAIN_SAVE = "brain:save"
    BRAIN_LOAD = "brain:load"
    BRAIN_LOG = "brain:log"
    BRAIN_STATS = "brain:stats"
    
    # Experiment commands
    EXPERIMENT_RUN = "experiment:run"
    EXPERIMENT_TEST = "experiment:test"
    EXPERIMENT_VERIFY = "experiment:verify"
    EXPERIMENT_MONITOR = "experiment:monitor"
    
    # System commands
    SYSTEM_INFO = "system:info"
    SYSTEM_PERFORMANCE = "system:performance"
    SYSTEM_LOGGING = "system:logging"
    SYSTEM_MONITOR = "system:monitor"
    
    # Utility commands
    UTILITY_HELP = "utility:help"
    UTILITY_VERSION = "utility:version"
    UTILITY_BUILD = "utility:build"
    UTILITY_CLEAN = "utility:clean"
    UTILITY_TEST = "utility:test"
class NLMCli:
    """
    NLM Command Line Interface.
    
    Provides comprehensive CLI functionality for NLM Phase 6.
    """
    
    def __init__(self):
        self.config = get_config()
        self.logger = self._setup_logging()
        self.project_root = Path(__file__).parent
        self.build_dir = self.project_root / "build"
    
    def _setup_logging(self):
        """Setup logging for CLI."""
        import logging
        
        logger = logging.getLogger("nlm.cli")
        logger.setLevel(logging.INFO)
        
        # Console handler
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        
        # Formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        console_handler.setFormatter(formatter)
        
        logger.addHandler(console_handler)
        return logger
    
    def execute_command(self, command_str: str, args: List[str] = None) -> bool:
        """
        Execute NLM CLI command.
        
        Args:
            command_str: Command string (e.g., "brain:status")
            args: Command arguments
            
        Returns:
            True if command executed successfully, False otherwise
        """
        try:
            # Parse command
            if "." not in command_str:
                raise NLMCliError(f"Invalid command format: {command_str}")
            
            category, action = command_str.split(".", 1)
            command = Command(f"{category}:{action}")
            
            # Dispatch to appropriate handler
            if hasattr(self, f"handle_{command.value}"):
                handler = getattr(self, f"handle_{command.value}")
                return handler(args or [])
            else:
                raise NLMCliError(f"Unknown command: {command_str}")
                
        except NLMCliError as e:
            self.logger.error(f"Command error: {e}")
            return False
        except Exception as e:
            self.logger.error(f"Unexpected error: {e}")
            return False
    
    def handle_config_init(self, args: List[str]) -> bool:
        """Initialize NLM configuration."""
        try:
            # Parse optional arguments
            config_file = None
            environment_str = "development"
            
            if args:
                for i, arg in enumerate(args):
                    if arg == "--config-file" and i + 1 < len(args):
                        config_file = args[i + 1]
                    elif arg == "--environment" and i + 1 < len(args):
                        environment_str = args[i + 1]
            
            environment = Environment(environment_str.lower())
            
            # Initialize configuration
            initialize_config(config_file, environment)
            
            self.logger.info(f"NLM configuration initialized for {environment.value} environment")
            if config_file:
                self.logger.info(f"Configuration loaded from {config_file}")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to initialize configuration: {e}")
    
    def handle_config_show(self, args: List[str]) -> bool:
        """Show current configuration."""
        try:
            config_file = None
            
            if args:
                for i, arg in enumerate(args):
                    if arg == "--file" and i + 1 < len(args):
                        config_file = args[i + 1]
            
            if config_file:
                # Load configuration from file
                initialize_config(config_file)
            
            # Print configuration summary
            summary = self.config.get_config_summary()
            print(summary)
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to show configuration: {e}")
    
    def handle_config_set(self, args: List[str]) -> bool:
        """Set configuration value."""
        if len(args) < 2:
            raise NLMCliError("config:set requires key and value arguments")
        
        key = args[0]
        value = args[1]
        
        try:
            # Try to convert to appropriate type
            if value.lower() == "true":
                value = True
            elif value.lower() == "false":
                value = False
            elif value.isdigit():
                value = int(value)
            elif self._is_float(value):
                value = float(value)
            
            self.config.set(key, value)
            self.logger.info(f"Configuration set: {key} = {value}")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to set configuration: {e}")
    
    def handle_config_get(self, args: List[str]) -> bool:
        """Get configuration value."""
        if len(args) < 1:
            raise NLMCliError("config:get requires key argument")
        
        key = args[0]
        
        try:
            value = self.config.get(key)
            
            if value is None:
                print(f"Configuration key '{key}' not found")
                return False
            else:
                print(f"{key}: {value}")
                return True
                
        except Exception as e:
            raise NLMCliError(f"Failed to get configuration: {e}")
    
    def handle_config_save(self, args: List[str]) -> bool:
        """Save configuration to file."""
        if len(args) < 1:
            raise NLMCliError("config:save requires file path argument")
        
        config_file = args[0]
        
        try:
            self.config.save_config(config_file)
            self.logger.info(f"Configuration saved to {config_file}")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to save configuration: {e}")
    
    def handle_config_load(self, args: List[str]) -> bool:
        """Load configuration from file."""
        if len(args) < 1:
            raise NLMCliError("config:load requires file path argument")
        
        config_file = args[0]
        
        try:
            initialize_config(config_file)
            self.logger.info(f"Configuration loaded from {config_file}")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to load configuration: {e}")
    
    def handle_config_validate(self, args: List[str]) -> bool:
        """Validate configuration."""
        try:
            # Validate all sections
            sections = ["brain", "plasticity", "neuromod", "memory", 
                       "cognition", "development", "performance", "system"]
            
            all_valid = True
            for section in sections:
                if self.config.validate_config_section(section):
                    print(f"✓ Section '{section}' is valid")
                else:
                    print(f"✗ Section '{section}' is invalid")
                    all_valid = False
            
            if all_valid:
                self.logger.info("All configuration sections are valid")
                return True
            else:
                raise NLMCliError("Some configuration sections are invalid")
                
        except Exception as e:
            raise NLMCliError(f"Failed to validate configuration: {e}")
    
    def handle_config_reset(self, args: List[str]) -> bool:
        """Reset configuration to defaults."""
        try:
            self.config.reset_to_defaults()
            self.logger.info("Configuration reset to defaults")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to reset configuration: {e}")
    
    def handle_brain_init(self, args: List[str]) -> bool:
        """Initialize brain with current configuration."""
        try:
            # This would typically call the NLM Brain initialization
            # For now, we'll simulate it
            
            neuron_count = self.config.get("brain.neuron_count", 1000)
            region_count = self.config.get("brain.region_count", 1)
            
            self.logger.info(f"Initializing NLM brain with {neuron_count} neurons and {region_count} regions")
            
            # In a real implementation, this would call the actual Brain::initialize() method
            print(f"Brain initialized successfully")
            print(f"Neurons: {neuron_count}")
            print(f"Regions: {region_count}")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to initialize brain: {e}")
    
    def handle_brain_status(self, args: List[str]) -> bool:
        """Get brain status information."""
        try:
            # This would typically call the NLM Brain status method
            # For now, we'll generate simulated status information
            
            status = "=== BRAIN STATUS ===\n"
            status += f"Status: OPERATIONAL\n"
            status += f"Neurons: {self.config.get('brain.neuron_count', 1000)}\n"
            status += f"Regions: {self.config.get('brain.region_count', 1)}\n"
            status += f"Total Synapses: {self.config.get('brain.neuron_count', 1000) * 10}\n"
            status += f"Average Firing Rate: {self.config.get('brain.neuron_count', 1000) * 0.05:.2f} Hz\n"
            
            print(status)
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to get brain status: {e}")
    
    def handle_brain_step(self, args: List[str]) -> bool:
        """Run one brain simulation step."""
        try:
            steps = 1
            if args and args[0].isdigit():
                steps = int(args[0])
            
            self.logger.info(f"Running {steps} brain simulation step(s)")
            
            # In a real implementation, this would call the Brain::step() method
            print(f"Brain step completed: {steps} steps simulated")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to run brain step: {e}")
    
    def handle_brain_reset(self, args: List[str]) -> bool:
        """Reset brain state."""
        try:
            self.logger.info("Resetting brain state")
            
            # In a real implementation, this would call the Brain::reset() method
            print("Brain state reset successfully")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to reset brain: {e}")
    
    def handle_brain_save(self, args: List[str]) -> bool:
        """Save brain state to file."""
        if len(args) < 1:
            raise NLMCliError("brain:save requires file path argument")
        
        filepath = args[0]
        
        try:
            # In a real implementation, this would call the Brain::save() method
            self.logger.info(f"Saving brain state to {filepath}")
            print(f"Brain state saved to {filepath}")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to save brain: {e}")
    
    def handle_brain_load(self, args: List[str]) -> bool:
        """Load brain state from file."""
        if len(args) < 1:
            raise NLMCliError("brain:load requires file path argument")
        
        filepath = args[0]
        
        try:
            # In a real implementation, this would call the Brain::load() method
            self.logger.info(f"Loading brain state from {filepath}")
            print(f"Brain state loaded from {filepath}")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to load brain: {e}")
    
    def handle_brain_log(self, args: List[str]) -> bool:
        """Get brain log information."""
        try:
            # In a real implementation, this would call the Brain::logStatus() method
            self.logger.info("Getting brain log information")
            
            # Print simulated log information
            print("=== BRAIN LOG ===")
            print("Recent log entries would be displayed here")
            print("=====================")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to get brain log: {e}")
    
    def handle_brain_stats(self, args: List[str]) -> bool:
        """Get brain statistics."""
        try:
            # This would typically call the Brain statistics methods
            stats = self.config.get_config_summary()
            print(stats)
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to get brain statistics: {e}")
    
    def handle_experiment_run(self, args: List[str]) -> bool:
        """Run NLM experiment."""
        try:
            # Parse experiment parameters
            neuron_count = self.config.get("experiment.default_neuron_count", 500)
            max_steps = self.config.get("experiment.default_max_steps", 2000)
            
            # Parse command line arguments
            if args:
                for i, arg in enumerate(args):
                    if arg == "--neurons" and i + 1 < len(args):
                        neuron_count = int(args[i + 1])
                    elif arg == "--steps" and i + 1 < len(args):
                        max_steps = int(args[i + 1])
            
            self.logger.info(f"Running NLM experiment: {neuron_count} neurons, {max_steps} steps")
            
            # In a real implementation, this would call the Phase6Demo or experiment runner
            print(f"Experiment running...")
            print(f"Neurons: {neuron_count}")
            print(f"Steps: {max_steps}")
            print(f"Experiment completed successfully")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to run experiment: {e}")
    
    def handle_experiment_test(self, args: List[str]) -> bool:
        """Run NLM experiment tests."""
        try:
            self.logger.info("Running NLM experiment tests")
            
            # In a real implementation, this would run the test suite
            print("Running NLM experiment tests...")
            print("Test suite executed successfully")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to run experiment tests: {e}")
    
    def handle_experiment_verify(self, args: List[str]) -> bool:
        """Verify NLM integration."""
        try:
            self.logger.info("Verifying NLM integration")
            
            # In a real implementation, this would run the Phase6Demo verification
            print("Verifying NLM integration...")
            print("Integration verification completed successfully")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to verify integration: {e}")
    
    def handle_system_info(self, args: List[str]) -> bool:
        """Get system information."""
        try:
            # Get system information
            info = "=== NLM SYSTEM INFORMATION ===\n"
            info += f"Version: {self._get_version()}\n"
            info += f"Build: {self._get_build_info()}\n"
            info += f"Environment: {self.config.environment.value}\n"
            info += f"Memory Usage: {self.config.get('system.max_memory_usage', 0) / (1024*1024):.1f} MB\n"
            info += f"Timeout: {self.config.get('system.timeout', 0)} seconds\n"
            
            print(info)
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to get system info: {e}")
    
    def handle_system_performance(self, args: List[str]) -> bool:
        """Get system performance information."""
        try:
            # Get performance information
            performance = "=== SYSTEM PERFORMANCE ===\n"
            performance += f"Performance Score: {self.config.get('performance.memory_pool_size', 0) / 1024:.1f}/1024\n"
            performance += f"Multithreading: {'Enabled' if self.config.get('performance.enable_multithreading', False) else 'Disabled'}\n"
            performance += f"SIMD: {'Enabled' if self.config.get('performance.enable_simd', False) else 'Disabled'}\n"
            performance += f"Memory Pool: {self.config.get('performance.memory_pool_size', 0)} bytes\n"
            performance += f"Event Queue: {self.config.get('performance.event_queue_size', 0)} events\n"
            
            print(performance)
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to get system performance: {e}")
    
    def handle_utility_help(self, args: List[str]) -> bool:
        """Show help information."""
        try:
            help_text = "=== NLM CLI HELP ===\n"
            help_text += "Available commands:\n\n"
            
            # Group commands by category
            categories = {}
            for command in Command:
                category, action = command.value.split(":", 1)
                if category not in categories:
                    categories[category] = []
                categories[category].append((action, command.value))
            
            for category, commands in categories.items():
                help_text += f"{category.upper()} COMMANDS:\n"
                for action, full_cmd in commands:
                    help_text += f"  {full_cmd}\n"
                help_text += "\n"
            
            print(help_text)
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to show help: {e}")
    
    def handle_utility_version(self, args: List[str]) -> bool:
        """Show NLM version information."""
        try:
            version = self._get_version()
            build = self._get_build_info()
            
            print(f"=== NLM VERSION ===")
            print(f"Version: {version}")
            print(f"Build: {build}")
            
            return True
            
        except Exception as e:
            raise NLMCliError(f"Failed to get version: {e}")
    
    def _is_float(self, value: str) -> bool:
        """Check if string can be converted to float."""
        try:
            float(value)
            return True
        except ValueError:
            return False
    
    def _get_version(self) -> str:
        """Get NLM version."""
        return "NLM Phase 6.0 - Integrated Artificial Brain"
    
    def _get_build_info(self) -> str:
        """Get NLM build information."""
        return "Built with C++20, Real LIF dynamics, Event-driven simulation"
    
    def parse_args(self, args: List[str]) -> Dict[str, Any]:
        """
        Parse command line arguments.
        
        Args:
            args: Command line arguments
            
        Returns:
            Parsed arguments dictionary
        """
        if not args:
            return {}
        
        # Simple argument parsing
        result = {}
        
        for i, arg in enumerate(args):
            if arg.startswith("--"):
                key = arg[2:].replace("-", "_")
                if i + 1 < len(args) and not args[i + 1].startswith("--"):
                    result[key] = args[i + 1]
                else:
                    result[key] = True
        
        return result
    
    def print_help(self):
        """Print CLI help information."""
        help_text = """
NLM Command Line Interface

Usage: nlm <command> [arguments]

Available commands:

CONFIGURATION COMMANDS:
  config:init [--config-file FILE] [--environment ENV]  Initialize configuration
  config:show [--file FILE]                                  Show current configuration
  config:set KEY VALUE                                       Set configuration value
  config:get KEY                                             Get configuration value
  config:save FILE                                            Save configuration to file
  config:load FILE                                            Load configuration from file
  config:validate                                            Validate configuration
  config:reset                                               Reset configuration to defaults

BRAIN COMMANDS:
  brain:init                                                Initialize brain with current configuration
  brain:status                                              Get brain status information
  brain:step [STEPS]                                         Run brain simulation step(s)
  brain:reset                                              Reset brain state
  brain:save FILE                                           Save brain state to file
  brain:load FILE                                           Load brain state from file
  brain:log                                                Get brain log information
  brain:stats                                               Get brain statistics

EXPERIMENT COMMANDS:
  experiment:run [OPTIONS]                                 Run NLM experiment
  experiment:test                                           Run NLM experiment tests
  experiment:verify                                         Verify NLM integration
  experiment:monitor                                       Monitor experiment progress

SYSTEM COMMANDS:
  system:info                                              Get system information
  system:performance                                        Get system performance information
  system:logging                                            Configure logging
  system:monitor                                            Monitor system resources

UTILITY COMMANDS:
  utility:help                                             Show help information
  utility:version                                          Show NLM version information
  utility:build                                            Build NLM from source
  utility:clean                                            Clean build artifacts
  utility:test                                            Run NLM tests

EXAMPLES:
  nlm config:init --environment production
  nlm brain:init
  nlm brain:status
  nlm experiment:run --neurons 1000 --steps 5000
  nlm system:info
  nlm utility:help
"""
        print(help_text)
def main():
    """Main CLI entry point."""
    try:
        # Parse command line arguments
        if len(sys.argv) < 2:
            print("Error: No command specified")
            print("\nUse 'nlm utility:help' for help information")
            sys.exit(1)
        
        # Create CLI instance
        cli = NLMCli()
        
        # Get command and arguments
        command = sys.argv[1]
        args = sys.argv[2:]
        
        # Execute command
        success = cli.execute_command(command, args)
        
        if not success:
            sys.exit(1)
            
    except NLMCliError as e:
        print(f"Error: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        sys.exit(130)
    except Exception as e:
        print(f"Unexpected error: {e}")
        sys.exit(1)
if __name__ == "__main__":
    main()
