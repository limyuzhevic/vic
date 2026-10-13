#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI) - Advanced Usage

This module provides command-line tools for advanced NLM (Neural Learning Machine) usage,
including configuration management, simulation control, and analysis.
"""

import argparse
import sys
import os
import json
import logging
from pathlib import Path
from typing import Dict, Any, Optional, List
import time

# Add the current directory to the path so we can import enhanced_api
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from enhanced_api import (
    NMLEnvironmentConfig, 
    ValidationLevel, 
    create_config, 
    create_session, 
    run_simulation,
    save_session,
    load_session,
    NLMError,
    ConfigValidationError,
    SimulationError,
    ConfigurationValidator
)

class NLMCLI:
    """Command-line interface for NLM (Neural Learning Machine)."""
    
    def __init__(self):
        self.logger = self._setup_logging()
        self.current_session = None
        self.current_config = None
        
    def _setup_logging(self) -> logging.Logger:
        """Setup logging configuration."""
        logger = logging.getLogger("nlm_cli")
        logger.setLevel(logging.INFO)
        
        # Console handler
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.INFO)
        formatter = logging.Formatter(
            '%(asctime)s - NLM CLI - %(levelname)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        console_handler.setFormatter(formatter)
        logger.addHandler(console_handler)
        
        # File handler (only if log file specified)
        return logger
    
    def run_command(self, args: argparse.Namespace) -> bool:
        """Run the appropriate command based on parsed arguments."""
        command_map = {
            'config': self._handle_config_command,
            'session': self._handle_session_command,
            'simulation': self._handle_simulation_command,
            'analyze': self._handle_analyze_command,
            'validate': self._handle_validate_command,
            'benchmark': self._handle_benchmark_command,
            'export': self._handle_export_command,
            'import': self._handle_import_command,
            'presets': self._handle_presets_command,
        }
        
        try:
            command = args.command
            if command in command_map:
                return command_map[command](args)
            else:
                self.logger.error(f"Unknown command: {command}")
                self._print_help()
                return False
        except Exception as e:
            self.logger.error(f"Error executing command: {e}")
            return False
    
    def _handle_config_command(self, args: argparse.Namespace) -> bool:
        """Handle config-related commands."""
        subcommand_map = {
            'create': self._handle_config_create,
            'show': self._handle_config_show,
            'modify': self._handle_config_modify,
            'validate': self._handle_config_validate,
            'save': self._handle_config_save,
            'load': self._handle_config_load,
        }
        
        if not args.subcommand:
            self.logger.error("Subcommand required for config command")
            return False
        
        subcommand = args.subcommand
        if subcommand in subcommand_map:
            return subcommand_map[subcommand](args)
        else:
            self.logger.error(f"Unknown config subcommand: {subcommand}")
            return False
    
    def _handle_session_command(self, args: argparse.Namespace) -> bool:
        """Handle session-related commands."""
        subcommand_map = {
            'create': self._handle_session_create,
            'info': self._handle_session_info,
            'status': self._handle_session_status,
            'delete': self._handle_session_delete,
            'list': self._handle_session_list,
        }
        
        if not args.subcommand:
            self.logger.error("Subcommand required for session command")
            return False
        
        subcommand = args.subcommand
        if subcommand in subcommand_map:
            return subcommand_map[subcommand](args)
        else:
            self.logger.error(f"Unknown session subcommand: {subcommand}")
            return False
    
    def _handle_simulation_command(self, args: argparse.Namespace) -> bool:
        """Handle simulation-related commands."""
        subcommand_map = {
            'run': self._handle_simulation_run,
            'step': self._handle_simulation_step,
            'pause': self._handle_simulation_pause,
            'resume': self._handle_simulation_resume,
            'stop': self._handle_simulation_stop,
            'monitor': self._handle_simulation_monitor,
        }
        
        if not args.subcommand:
            self.logger.error("Subcommand required for simulation command")
            return False
        
        subcommand = args.subcommand
        if subcommand in subcommand_map:
            return subcommand_map[subcommand](args)
        else:
            self.logger.error(f"Unknown simulation subcommand: {subcommand}")
            return False
    
    def _handle_analyze_command(self, args: argparse.Namespace) -> bool:
        """Handle analysis-related commands."""
        subcommand_map = {
            'brain': self._handle_analyze_brain,
            'performance': self._handle_analyze_performance,
            'trajectory': self._handle_analyze_trajectory,
            'correlations': self._handle_analyze_correlations,
            'patterns': self._handle_analyze_patterns,
        }
        
        if not args.subcommand:
            self.logger.error("Subcommand required for analyze command")
            return False
        
        subcommand = args.subcommand
        if subcommand in subcommand_map:
            return subcommand_map[subcommand](args)
        else:
            self.logger.error(f"Unknown analyze subcommand: {subcommand}")
            return False
    
    # Configuration handling methods
    def _handle_config_create(self, args: argparse.Namespace) -> bool:
        """Create a new configuration."""
        preset = getattr(args, 'preset', 'balanced')
        output_file = getattr(args, 'output_file', None)
        validation_level = getattr(args, 'validation_level', 'strict')
        
        try:
            validation_enum = ValidationLevel[validation_level.upper()]
        except KeyError:
            self.logger.error(f"Invalid validation level: {validation_level}")
            return False
        
        self.logger.info(f"Creating configuration with preset: {preset}")
        
        try:
            config = create_config(preset=preset, validation_level=validation_enum)
            self.current_config = config
            
            self.logger.info(f"Configuration created successfully:")
            self.logger.info(f"  Preset: {preset}")
            self.logger.info(f"  Neurons: {config.neuron_count}")
            self.logger.info(f"  Regions: {config.region_count}")
            self.logger.info(f"  Timestep: {config.simulation_timestep}")
            
            if output_file:
                config_dict = config.to_dict()
                with open(output_file, 'w') as f:
                    json.dump(config_dict, f, indent=2)
                self.logger.info(f"Configuration saved to: {output_file}")
            
            return True
        except Exception as e:
            self.logger.error(f"Failed to create configuration: {e}")
            return False
    
    def _handle_config_show(self, args: argparse.Namespace) -> bool:
        """Show configuration details."""
        if not self.current_config:
            self.logger.error("No configuration loaded. Create or load a configuration first.")
            return False
        
        config = self.current_config
        
        print("\n=== NLM Configuration ===\n")
        
        # Basic settings
        print("BASIC SETTINGS:")
        print(f"  Preset: {args.preset if hasattr(args, 'preset') else 'custom'}")
        print(f"  Neurons: {config.neuron_count}")
        print(f"  Regions: {config.region_count}")
        print(f"  Simulation Timestep: {config.simulation_timestep}")
        print(f"  Max Steps: {config.max_simulation_steps}")
        print()
        
        # Connectivity
        print("CONNECTIVITY:")
        print(f"  Connection Probability: {config.connection_probability}")
        print(f"  Initial Weight Mean: {config.initial_weight_mean}")
        print(f"  Initial Weight Std: {config.initial_weight_std}")
        print()
        
        # Neuron dynamics
        print("NEURON DYNAMICS:")
        print(f"  Threshold: {config.v_thresh}")
        print(f"  Rest Potential: {config.v_rest}")
        print(f"  Reset Potential: {config.v_reset}")
        print(f"  Membrane Time Constant: {config.tau_mem}")
        print(f"  Refractory Period: {config.tau_ref}")
        print()
        
        # Plasticity
        print("PLASTICITY:")
        print(f"  STDP Enabled: {config.stdp_enable}")
        print(f"  STDP Learning Rate: {config.stdp_learning_rate}")
        print(f"  STDP Time Constants: {config.stdp_tau_plus}, {config.stdp_tau_minus}")
        print(f"  Hebbian Enabled: {config.hebbian_enable}")
        print(f"  Hebbian Learning Rate: {config.hebbian_learning_rate}")
        print(f"  Structural Plasticity Enabled: {config.structural_enable}")
        print(f"  Synaptogenesis Rate: {config.synaptogenesis_rate}")
        print(f"  Pruning Rate: {config.pruning_rate}")
        print()
        
        # Development
        print("DEVELOPMENT:")
        print(f"  Development Synaptogenesis Rate: {config.development_synaptogenesis_rate}")
        print(f"  Development Pruning Rate: {config.development_pruning_rate}")
        print(f"  Developmental Stage Duration: {config.developmental_stage_duration}")
        print()
        
        # Neuromodulation
        print("NEUROMODULATION:")
        print(f"  Dopamine Baseline: {config.dopamine_baseline}")
        print(f"  Curiosity Enabled: {config.curiosity_enable}")
        print(f"  Curiosity Threshold: {config.curiosity_threshold}")
        print(f"  Novelty Decay Rate: {config.novelty_decay_rate}")
        print()
        
        # Advanced features
        print("ADVANCED FEATURES:")
        print(f"  Curiosity-Driven Exploration: {config.enable_curiosity_driven_exploration}")
        print(f"  Hierarchical Planning: {config.enable_hierarchical_planning}")
        print(f"  Metacognition: {config.enable_metacognition}")
        print(f"  Transfer Learning: {config.enable_transfer_learning}")
        print(f"  Performance Monitoring: {config.enable_performance_monitoring}")
        print(f"  Memory Profiling: {config.enable_memory_profiling}")
        print(f"  Safety Checks: {config.enable_safety_checks}")
        print()
        
        return True
    
    def _handle_config_modify(self, args: argparse.Namespace) -> bool:
        """Modify existing configuration."""
        if not self.current_config:
            self.logger.error("No configuration loaded. Create or load a configuration first.")
            return False
        
        # Parse parameter modifications
        modifications = {}
        if hasattr(args, 'param'):
            for param in args.param:
                if '=' in param:
                    key, value = param.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Try to convert value to appropriate type
                    if key in ['neuron_count', 'region_count', 'max_simulation_steps']:
                        try:
                            value = int(value)
                        except ValueError:
                            self.logger.error(f"Invalid integer value for parameter: {param}")
                            return False
                    elif key in ['simulation_timestep', 'connection_probability', 
                                'initial_weight_mean', 'initial_weight_std', 
                                'v_thresh', 'v_rest', 'v_reset', 
                                'tau_mem', 'tau_ref', 
                                'stdp_learning_rate', 'stdp_tau_plus', 'stdp_tau_minus',
                                'hebbian_learning_rate', 'synaptogenesis_rate', 'pruning_rate',
                                'development_synaptogenesis_rate', 'development_pruning_rate',
                                'dopamine_baseline', 'curiosity_threshold', 'novelty_decay_rate',
                                'reward_discount_factor', 'reward_scaling',
                                'environment_width', 'environment_height',
                                'vision_width', 'vision_height', 'vision_channels',
                                'visualization_update_rate', 'performance_sampling_rate',
                                'energy_decay_rate', 'health_threshold', 'curiosity_exploration_rate']:
                        try:
                            value = float(value)
                        except ValueError:
                            self.logger.error(f"Invalid float value for parameter: {param}")
                            return False
                    elif key in ['stdp_enable', 'hebbian_enable', 'structural_enable',
                                'curiosity_enable', 'enable_curiosity_driven_exploration',
                                'enable_hierarchical_planning', 'enable_metacognition',
                                'enable_transfer_learning', 'visualization_enabled',
                                'enable_performance_monitoring', 'enable_memory_profiling',
                                'enable_safety_checks', 'log_to_file', 'log_file_append']:
                        value = value.lower() in ['true', '1', 'yes', 'on']
                    
                    modifications[key] = value
                else:
                    self.logger.error(f"Invalid parameter format: {param}")
                    return False
        
        # Apply modifications
        for key, value in modifications.items():
            if hasattr(self.current_config, key):
                setattr(self.current_config, key, value)
                self.logger.info(f"Modified parameter {key}: {value}")
            else:
                self.logger.error(f"Unknown parameter: {key}")
                return False
        
        # Validate configuration
        validation_results = ConfigurationValidator.validate_configuration(self.current_config, ValidationLevel.STRICT)
        if validation_results['errors']:
            self.logger.error(f"Configuration validation failed: {validation_results['errors']}")
            return False
        
        self.logger.info("Configuration modified and validated successfully")
        return True
    
    def _handle_config_validate(self, args: argparse.Namespace) -> bool:
        """Validate configuration."""
        if not self.current_config:
            self.logger.error("No configuration loaded. Create or load a configuration first.")
            return False
        
        validation_level = ValidationLevel[args.validation_level.upper()]
        
        self.logger.info(f"Validating configuration with level: {validation_level}")
        
        validation_results = ConfigurationValidator.validate_configuration(self.current_config, validation_level)
        
        if validation_results['errors']:
            print("\n=== VALIDATION ERRORS ===\n")
            for error in validation_results['errors']:
                print(f"  ✗ {error}")
            print()
            
        if validation_results['warnings']:
            print("\n=== VALIDATION WARNINGS ===\n")
            for warning in validation_results['warnings']:
                print(f"  ⚠ {warning}")
            print()
        
        if validation_results['info']:
            print("\n=== VALIDATION INFO ===\n")
            for info in validation_results['info']:
                print(f"  ℹ {info}")
            print()
        
        if not validation_results['errors']:
            self.logger.info("Configuration validation passed")
            return True
        else:
            self.logger.error("Configuration validation failed")
            return False
    
    def _handle_config_save(self, args: argparse.Namespace) -> bool:
        """Save configuration to file."""
        if not self.current_config:
            self.logger.error("No configuration loaded. Create or load a configuration first.")
            return False
        
        output_file = getattr(args, 'output_file', 'config.json')
        
        try:
            config_dict = self.current_config.to_dict()
            with open(output_file, 'w') as f:
                json.dump(config_dict, f, indent=2)
            
            self.logger.info(f"Configuration saved to: {output_file}")
            return True
        except Exception as e:
            self.logger.error(f"Failed to save configuration: {e}")
            return False
    
    def _handle_config_load(self, args: argparse.Namespace) -> bool:
        """Load configuration from file."""
        input_file = getattr(args, 'input_file', 'config.json')
        
        try:
            if not os.path.exists(input_file):
                self.logger.error(f"Configuration file not found: {input_file}")
                return False
            
            with open(input_file, 'r') as f:
                config_dict = json.load(f)
            
            # Create configuration object
            self.current_config = NMLEnvironmentConfig()
            
            # Apply configuration values
            for key, value in config_dict.items():
                if hasattr(self.current_config, key):
                    setattr(self.current_config, key, value)
                else:
                    self.logger.warning(f"Unknown configuration parameter ignored: {key}")
            
            self.logger.info(f"Configuration loaded from: {input_file}")
            return True
        except Exception as e:
            self.logger.error(f"Failed to load configuration: {e}")
            return False
    
    def _handle_session_create(self, args: argparse.Namespace) -> bool:
        """Create a new simulation session."""
        if not self.current_config:
            self.logger.error("No configuration loaded. Create or load a configuration first.")
            return False
        
        self.logger.info("Creating simulation session...")
        
        try:
            self.current_session = create_session(self.current_config)
            
            self.logger.info("Simulation session created successfully")
            self.logger.info(f"  Neurons: {self.current_session.get('total_neurons', 0)}")
            self.logger.info(f"  Regions: {self.current_session.get('region_count', 0)}")
            self.logger.info(f"  Max Steps: {self.current_session.get('max_simulation_steps', 0)}")
            
            return True
        except Exception as e:
            self.logger.error(f"Failed to create simulation session: {e}")
            return False
    
    def _handle_session_info(self, args: argparse.Namespace) -> bool:
        """Show session information."""
        if not self.current_session:
            self.logger.error("No simulation session active. Create a session first.")
            return False
        
        print("\n=== Simulation Session Information ===\n")
        
        print("SESSION STATE:")
        print(f"  Active: {self.current_session.get('active', False)}")
        print(f"  Current Step: {self.current_session.get('step', 0)}")
        print(f"  Simulation Time: {self.current_session.get('simulation_time', 0.0)}")
        print()
        
        print("BRAIN STATISTICS:")
        brain_stats = self.current_session.get('brain_stats', {})
        if brain_stats:
            for key, value in brain_stats.items():
                print(f"  {key}: {value}")
        print()
        
        print("PERFORMANCE DATA:")
        performance_data = self.current_session.get('performance_data', [])
        if performance_data:
            print(f"  Total entries: {len(performance_data)}")
            print(f"  Average firing rate: {sum(d.get('average_firing_rate', 0) for d in performance_data) / len(performance_data):.3f}")
            print(f"  Max firing rate: {max(d.get('average_firing_rate', 0) for d in performance_data):.3f}")
        print()
        
        return True
    
    def _handle_simulation_run(self, args: argparse.Namespace) -> bool:
        """Run simulation."""
        if not self.current_session:
            self.logger.error("No simulation session active. Create a session first.")
            return False
        
        num_steps = getattr(args, 'num_steps', 100)
        
        self.logger.info(f"Running simulation for {num_steps} steps...")
        
        try:
            result = run_simulation(num_steps, self.current_config)
            
            # Update session state
            self.current_session.update(result)
            
            self.logger.info("Simulation completed successfully")
            self.logger.info(f"  Total steps executed: {result.get('final_step', 0)}")
            self.logger.info(f"  Total neurons: {result.get('total_neurons', 0)}")
            self.logger.info(f"  Total synapses: {result.get('total_synapses', 0)}")
            self.logger.info(f"  Total spikes: {result.get('total_spikes', 0)}")
            self.logger.info(f"  Firing neurons: {result.get('firing_neurons', 0)}")
            
            return True
        except Exception as e:
            self.logger.error(f"Simulation failed: {e}")
            return False
    
    def _print_help(self) -> None:
        """Print help information."""
        help_text = """
NLM (Neural Learning Machine) Command Line Interface

USAGE:
  nlm-cli <command> [options]

COMMANDS:
  config     Configuration management
  session    Session management
  simulation Simulation control
  analyze    Analysis and statistics
  validate   Configuration validation
  benchmark  Performance benchmarking
  export     Export data
  import     Import data
  presets    List available presets

CONFIGURATION SUBCOMMANDS:
  config create [--preset PRESET] [--output-file FILE] [--validation-level LEVEL]
  config show
  config modify --param PARAM=value [--param PARAM=value]...
  config validate [--validation-level LEVEL]
  config save [--output-file FILE]
  config load [--input-file FILE]

SESSION SUBCOMMANDS:
  session create
  session info
  session status
  session delete [--session-id ID]
  session list

SIMULATION SUBCOMMANDS:
  simulation run [--num-steps STEPS]
  simulation step [--steps STEPS]
  simulation pause
  simulation resume
  simulation stop
  simulation monitor [--interval SECONDS]

For more detailed help on specific commands, use:
  nlm-cli <command> --help
"""
        print(help_text)

def main():
    """Main entry point for NLM CLI."""
    cli = NLMCLI()
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        epilog="Use 'nlm-cli <command> --help' for detailed help on specific commands."
    )
    
    parser.add_argument(
        'command',
        choices=['config', 'session', 'simulation', 'analyze', 'validate', 'benchmark', 'export', 'import', 'presets'],
        help="Command to execute"
    )
    
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help="Enable verbose output"
    )
    
    parser.add_argument(
        '--quiet', '-q',
        action='store_true',
        help="Suppress non-essential output"
    )
    
    parser.add_argument(
        '--log-file',
        help="Log output to file"
    )
    
    # Global command-level options
    global_group = parser.add_argument_group('Global Options')
    global_group.add_argument('--version', action='store_true', help='Show version and exit')
    global_group.add_argument('--help', action='store_true', help='Show help and exit')
    
    # Parse known arguments first
    args, remaining = parser.parse_known_args()
    
    # Handle global flags
    if args.version:
        print("NLM CLI Version 1.0.0")
        return
    
    if args.help:
        cli._print_help()
        return
    
    if args.verbose:
        cli.logger.setLevel(logging.DEBUG)
    
    if args.quiet:
        cli.logger.setLevel(logging.WARNING)
    
    if args.log_file:
        file_handler = logging.FileHandler(args.log_file)
        file_handler.setLevel(logging.INFO)
        formatter = logging.Formatter('%(asctime)s - NLM CLI - %(levelname)s - %(message)s')
        file_handler.setFormatter(formatter)
        cli.logger.addHandler(file_handler)
    
    # Now parse the rest of arguments for the specific command
    if not remaining:
        cli.logger.error("No command provided")
        cli._print_help()
        return
    
    # Re-parse with the specific command
    sys.argv = [sys.argv[0]] + remaining
    parser = argparse.ArgumentParser()
    
    # Add command-specific arguments
    cli._add_command_arguments(parser, args.command)
    
    try:
        parsed_args = parser.parse_args()
        
        # Add command to args namespace
        parsed_args.command = args.command
        
        # Run the command
        success = cli.run_command(parsed_args)
        
        if not success:
            sys.exit(1)
            
    except SystemExit:
        raise
    except Exception as e:
        cli.logger.error(f"Unexpected error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()