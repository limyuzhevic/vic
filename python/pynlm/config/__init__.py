"""
Configuration utilities for NLM Python package

This module provides configuration management and validation for neural simulations.
"""

from typing import Dict, Any, Optional, Union
from .utils import validate_config, load_config, save_config


def create_default_config() -> Dict[str, Any]:
    """
    Create a default configuration for NLM brain.

    Returns:
        dict: Default configuration
    """
    return {
        # Core simulation parameters
        'random_seed': 42,
        'simulation_timestep': 0.001,
        'neuron_count': 1000,
        'region_count': 1,
        'connection_probability': 0.1,

        # Plasticity settings
        'stdp_ltp_weight': 0.01,
        'stdp_ltd_weight': 0.012,
        'stdp_time_constant': 20.0,
        'plasticity_learning_rate': 0.01,

        # Development settings
        'development_synaptogenesis_rate': 0.001,
        'development_pruning_rate': 0.0001,

        # Neuromodulation settings
        'dopamine_baseline': 0.1,
        'curiosity_baseline': 0.0,
        'novelty_sensitivity': 0.5,

        # Reward settings
        'reward_discount_factor': 0.99,
        'reward_scale': 1.0,

        # Environment settings
        'environment_name': 'SimpleWorld',
        'environment_width': 10,
        'environment_height': 10,
        'vision_width': 8,
        'vision_height': 8,

        # Logging settings
        'log_level': 'INFO',
        'log_to_file': False,
        'log_filename': 'nlm.log',

        # Simulation settings
        'max_simulation_steps': 10000,
        'simulation_time_limit': 0.0,

        # Memory settings
        'working_memory_capacity': 100,
        'episodic_memory_max_episodes': 1000,
        'consolidation_interval': 1000,

        # Development settings
        'developmental_stage': 'Adult',
        'plasticity_modulation': True,

        # Visualization
        'visualization_enabled': False,
        'visualization_update_rate': 30.0,
    }


def apply_command_line_args(config: Dict[str, Any], args: list) -> Dict[str, Any]:
    """
    Apply command-line arguments to configuration.

    Args:
        config: Base configuration
        args: Command-line arguments (list of strings)

    Returns:
        dict: Updated configuration
    """
    import sys
    if args:
        arg_list = args
    else:
        arg_list = sys.argv[1:]

    updated_config = config.copy()

    for arg in arg_list:
        if arg.startswith('--config='):
            config_file = arg[8:]
            loaded = load_config(config_file)
            if loaded:
                updated_config.update(loaded)

        elif arg == '--help' or arg == '-h':
            print_help()
            sys.exit(0)

        elif arg.startswith('--') and '=' in arg:
            key, value = arg[2:].split('=', 1)
            if key in updated_config:
                try:
                    # Try to infer type from current value
                    current_type = type(updated_config[key])
                    if current_type == int:
                        updated_config[key] = int(value)
                    elif current_type == float:
                        updated_config[key] = float(value)
                    elif current_type == bool:
                        updated_config[key] = value.lower() == 'true'
                    else:
                        updated_config[key] = value
                except ValueError:
                    print(f"Warning: Could not convert {value} to {current_type}")

        elif arg in ['--verbose', '--quiet', '--debug']:
            updated_config['log_level'] = 'DEBUG' if arg == '--debug' else 'INFO'

    return updated_config


def validate_config_for_phase(phase: str, config: Dict[str, Any]) -> tuple[bool, str]:
    """
    Validate configuration for a specific NLM phase.

    Args:
        phase: NLM phase name (e.g., 'Phase 2', 'Phase 3', 'Phase 6')
        config: Configuration dictionary

    Returns:
        tuple: (is_valid, error_message)
    """
    # Common validation
    is_valid, error = validate_config(config)
    if not is_valid:
        return False, error

    # Phase-specific validation
    if phase == 'Phase 2':
        # Phase 2: Real neural computation
        if config['neuron_count'] < 100:
            return False, "Phase 2 requires at least 100 neurons"
        if config['simulation_timestep'] > 0.01:
            return False, "Phase 2 timestep should be <= 0.01s for stability"

    elif phase == 'Phase 6':
        # Phase 6: Full integration - more neurons possible
        if config['neuron_count'] > 10000:
            return False, "Phase 6 neuron_count exceeds recommended maximum of 10000"
        if config['region_count'] > 10:
            return False, "Phase 6 region_count exceeds recommended maximum of 10"

    return True, "Configuration valid for " + phase


def print_help():
    """Print help message for NLM Python package."""
    help_text = """
NLM (Neural Learning Machine) Python Interface
===============================================

Usage: python -m pynlm [OPTIONS]

Basic Options:
  --help, -h              Show this help message
  --version               Show version information

Configuration:
  --config=FILE           Load configuration from JSON file
  --output=FILE           Save results to file

Simulation Control:
  --steps=NUMBER          Number of simulation steps
  --time=SECONDS          Simulation duration in seconds
  --neurons=NUMBER        Number of neurons (default: 1000)

Logging:
  --verbose               Enable verbose logging
  --quiet                 Suppress output
  --debug                 Enable debug logging

Phase Selection:
  --phase=PHASE_NAME      Select NLM phase (Phase 2, Phase 3, Phase 4, Phase 6)

Examples:
  python -m pynlm --neurons=500 --steps=1000 --phase=Phase 6
  python -m pynlm --config=my_config.json --verbose
  python -m pynlm --phase=Phase 2 --output=results.json
"""
    print(help_text)