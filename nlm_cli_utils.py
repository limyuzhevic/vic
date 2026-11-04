#!/usr/bin/env python3
"""
Unified CLI utilities for NLM demos.

This script provides common command-line argument parsing, configuration handling,
and logging setup that can be used across all NLM demo executables.
"""

import argparse
import json
import os
import sys
from typing import Dict, Any, Optional

# Try to import NLM Python bindings
try:
    import pynlm
    HAS_PYNLm = True
except ImportError:
    HAS_PYNLm = False
    print("Warning: pynlm not available. Some features will be disabled.")


def create_argument_parser(description: str) -> argparse.ArgumentParser:
    """
    Create a standard argument parser with common NLM options.
    
    Args:
        description: Description for the demo
        
    Returns:
        Configured argument parser
    """
    parser = argparse.ArgumentParser(
        description=description,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                    # Run with default settings
  %(prog)s --episodes 10     # Run 10 episodes
  %(prog)s --steps 500        # Run 500 steps per episode
  %(prog)s --brain-size 2000   # Create brain with 2000 neurons
  %(prog)s --config config.json # Load configuration from JSON file
  %(prog)s --log-level DEBUG   # Set logging level to DEBUG

To see all available options:
  %(prog)s --help
        """
    )
    
    # Core simulation parameters
    parser.add_argument("--episodes", "-e", type=int, default=5,
                       help="Number of episodes to run (default: 5)")
    parser.add_argument("--steps", "-s", type=int, default=200,
                       help="Steps per episode (default: 200)")
    parser.add_argument("--timestep", type=float, default=0.1,
                       help="World timestep in seconds (default: 0.1)")
    
    # Brain configuration
    parser.add_argument("--brain-size", type=int, default=500,
                       help="Number of neurons in the brain (default: 500)")
    parser.add_argument("--regions", type=int, default=2,
                       help="Number of brain regions (default: 2)")
    parser.add_argument("--connection-prob", type=float, default=0.1,
                       help="Connection probability for random connectivity (default: 0.1)")
    parser.add_argument("--seed", type=int, default=42,
                       help="Random seed for reproducible experiments (default: 42)")
    
    # World configuration
    parser.add_argument("--world-width", type=float, default=20.0,
                       help="World width in units (default: 20.0)")
    parser.add_argument("--world-height", type=float, default=20.0,
                       help="World height in units (default: 20.0)")
    parser.add_argument("--vision-width", type=int, default=16,
                       help="Vision sensor width (default: 16)")
    parser.add_argument("--vision-height", type=int, default=16,
                       help="Vision sensor height (default: 16)")
    
    # Feature toggles
    parser.add_argument("--reward-modulation", action="store_true", default=True,
                       help="Enable reward modulation (default: enabled)")
    parser.add_argument("--structural-plasticity", action="store_true", default=True,
                       help="Enable structural plasticity (default: enabled)")
    parser.add_argument("--development", action="store_true", default=True,
                       help="Enable development system (default: enabled)")
    parser.add_argument("--curiosity", action="store_true", default=True,
                       help="Enable curiosity-driven exploration (default: enabled)")
    
    # Logging and output
    parser.add_argument("--log-level", choices=["DEBUG", "INFO", "WARN", "ERROR"],
                       default="INFO", help="Logging level (default: INFO)")
    parser.add_argument("--verbose", "-v", action="store_true",
                       help="Enable verbose output")
    parser.add_argument("--quiet", "-q", action="store_true",
                       help="Suppress non-essential output")
    
    # Configuration file
    parser.add_argument("--config", "-c", type=str,
                       help="Load configuration from JSON or key=value file")
    parser.add_argument("--save-config", type=str,
                       help="Save configuration to file after experiment")
    
    # Advanced options
    parser.add_argument("--enable-checkpointing", action="store_true",
                       help="Enable checkpoint saving/loading")
    parser.add_argument("--checkpoint-path", type=str, default="/tmp/nlm_checkpoint",
                       help="Path for checkpoint files (default: /tmp/nlm_checkpoint)")
    
    # Performance
    parser.add_argument("--neural-steps", type=int, default=10,
                       help="Number of neural simulation steps per action (default: 10)")
    
    return parser


def load_configuration(args) -> Dict[str, Any]:
    """
    Load configuration from command line, file, or defaults.
    
    Args:
        args: Parsed command line arguments
        
    Returns:
        Dictionary of configuration values
    """
    config = {}
    
    # Load from config file if specified
    if args.config:
        config.update(load_config_file(args.config))
    
    # Override with command line arguments (if explicitly set)
    cli_args = {
        'episodes': args.episodes if args.episodes != 5 else None,
        'steps': args.steps if args.steps != 200 else None,
        'timestep': args.timestep if args.timestep != 0.1 else None,
        'brain_size': args.brain_size if args.brain_size != 500 else None,
        'regions': args.regions if args.regions != 2 else None,
        'connection_prob': args.connection_prob if args.connection_prob != 0.1 else None,
        'seed': args.seed if args.seed != 42 else None,
        'world_width': args.world_width if args.world_width != 20.0 else None,
        'world_height': args.world_height if args.world_height != 20.0 else None,
        'vision_width': args.vision_width if args.vision_width != 16 else None,
        'vision_height': args.vision_height if args.vision_height != 16 else None,
        'neural_steps': args.neural_steps if args.neural_steps != 10 else None,
    }
    
    # Add non-default values to config
    for key, value in cli_args.items():
        if value is not None:
            config[key] = value
    
    # Set defaults for any missing keys
    default_config = {
        'episodes': 5,
        'steps': 200,
        'timestep': 0.1,
        'brain_size': 500,
        'regions': 2,
        'connection_prob': 0.1,
        'seed': 42,
        'world_width': 20.0,
        'world_height': 20.0,
        'vision_width': 16,
        'vision_height': 16,
        'neural_steps': 10,
        'reward_modulation': True,
        'structural_plasticity': True,
        'development': True,
        'curiosity': True,
    }
    
    for key, value in default_config.items():
        if key not in config:
            config[key] = value
    
    return config


def load_config_file(filepath: str) -> Dict[str, Any]:
    """
    Load configuration from a file (JSON or key=value format).
    
    Args:
        filepath: Path to configuration file
        
    Returns:
        Dictionary of configuration values
    """
    config = {}
    
    if not os.path.exists(filepath):
        print(f"Warning: Configuration file '{filepath}' not found")
        return config
    
    # Try to parse as JSON first
    if filepath.endswith('.json'):
        try:
            with open(filepath, 'r') as f:
                file_config = json.load(f)
            config.update(file_config)
            print(f"Loaded configuration from JSON file: {filepath}")
        except json.JSONDecodeError as e:
            print(f"Warning: Failed to parse JSON config file: {e}")
    
    # Parse as key=value format (for backward compatibility)
    else:
        try:
            with open(filepath, 'r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#') and not line.startswith('//'):
                        if '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            
                            # Try to convert to appropriate type
                            config[key] = convert_config_value(key, value)
        except Exception as e:
            print(f"Warning: Failed to parse config file: {e}")
    
    return config


def convert_config_value(key: str, value: str) -> Any:
    """
    Convert configuration value to appropriate type based on key.
    
    Args:
        key: Configuration key
        value: String value
        
    Returns:
        Converted value
    """
    # Try to convert to boolean
    if value.lower() in ('true', 'false'):
        return value.lower() == 'true'
    
    # Try to convert to integer
    try:
        return int(value)
    except ValueError:
        pass
    
    # Try to convert to float
    try:
        return float(value)
    except ValueError:
        pass
    
    # Return as string
    return value


def setup_logging(log_level: str, verbose: bool, quiet: bool):
    """
    Setup logging configuration.
    
    Args:
        log_level: Logging level
        verbose: Enable verbose output
        quiet: Suppress non-essential output
    """
    if quiet:
        print("Output suppressed (quiet mode)")
    elif verbose:
        print(f"Log level: {log_level}")
        print("Verbose mode enabled")
    
    # In a real implementation, this would configure Python's logging module
    # For now, just print some basic info
    if verbose:
        print(f"Logging configured at level: {log_level}")


def save_configuration(config: Dict[str, Any], filepath: str):
    """
    Save configuration to file.
    
    Args:
        config: Configuration dictionary
        filepath: Path to save configuration
    """
    # Create directory if it doesn't exist
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    
    # Save as JSON
    if filepath.endswith('.json'):
        with open(filepath, 'w') as f:
            json.dump(config, f, indent=2)
    
    # Save as key=value format
    else:
        with open(filepath, 'w') as f:
            for key, value in config.items():
                if isinstance(value, str):
                    f.write(f"{key} = \"{value}\"")
                elif isinstance(value, bool):
                    f.write(f"{key} = {'true' if value else 'false'}")
                else:
                    f.write(f"{key} = {value}")
                f.write("\n")
    
    print(f"Configuration saved to: {filepath}")


def print_demo_header(title: str, config: Dict[str, Any]):
    """
    Print a standardized header for demo output.
    
    Args:
        title: Demo title
        config: Configuration dictionary
    """
    print("=" * 60)
    print(f"  {title}")
    print("=" * 60)
    
    if 'brain_size' in config:
        print(f"  Brain size: {config['brain_size']} neurons")
        print(f"  Regions: {config['regions']}")
    
    if 'world_width' in config and 'world_height' in config:
        print(f"  World size: {config['world_width']} x {config['world_height']} units")
    
    if 'vision_width' in config and 'vision_height' in config:
        print(f"  Vision: {config['vision_width']} x {config['vision_height']}")
    
    print()


def create_config_object(config: Dict[str, Any]):
    """
    Create a NLM Config object from configuration dictionary.
    
    Args:
        config: Configuration dictionary
        
    Returns:
        NLM Config object or None if pynlm is not available
    """
    if not HAS_PYNLm:
        return None
    
    py_config = pynlm.createDefaultConfig()
    
    # Set configuration values
    for key, value in config.items():
        # Convert key from snake_case to the format expected by NLM
        nlm_key = key.replace('_', '-')
        
        # Try to set the value using appropriate method
        try:
            if isinstance(value, bool):
                py_config.set(nlm_key, value)
            elif isinstance(value, int):
                py_config.set(nlm_key, value)
            elif isinstance(value, float):
                py_config.set(nlm_key, value)
            elif isinstance(value, str):
                py_config.set(nlm_key, value)
        except Exception:
            # If setting fails, continue with other values
            pass
    
    return py_config


if __name__ == "__main__":
    parser = create_argument_parser("NLM Demo Utilities")
    args = parser.parse_args()
    
    config = load_configuration(args)
    setup_logging(args.log_level, args.verbose, args.quiet)
    
    print_demo_header("NLM DEMO UTILITIES", config)
    print(f"Configuration loaded with {len(config)} parameters")
    print("Use this script as a base for creating unified NLM demos!")
    
    # Example usage
    if args.save_config:
        save_configuration(config, args.save_config)
    
    print("Demo utilities completed successfully!")