#!/usr/bin/env python3
"""
NLM Configuration Manager CLI - Manage configuration files and parameters.

This tool provides command-line access to configuration management
operations including loading, saving, querying, and modifying NLM configuration.
"""

import argparse
import json
import logging
import os
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional, Union

# Add parent directory to path for pynlm import
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import pynlm
except ImportError as e:
    print(f"ERROR: Could not import pynlm: {e}")
    sys.exit(1)


def detect_file_format(filepath: str) -> str:
    """Detect file format based on extension and content."""
    # Check file extension
    if filepath.lower().endswith('.json'):
        return 'json'
    elif filepath.lower().endswith(('.yaml', '.yml')):
        return 'yaml'
    elif filepath.lower().endswith(('.cfg', '.ini', '.txt')):
        return 'key_value'
    else:
        # Try to detect from content
        try:
            with open(filepath, 'r') as f:
                first_line = f.readline().strip()
                if first_line.startswith('{') or first_line.startswith('['):
                    return 'json'
                elif ':' in first_line and ' ' in first_line:
                    return 'yaml'
                else:
                    return 'key_value'
        except:
            return 'key_value'


def create_config(args: argparse.Namespace) -> None:
    """Create a new configuration file."""
    logger = logging.getLogger("config_manager")
    
    if not args.output_file:
        logger.error("Output file path is required for config creation")
        sys.exit(1)
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Apply custom parameters if specified
    if args.params:
        for param in args.params:
            if '=' in param:
                key, value = param.split('=', 1)
                try:
                    # Try to parse as int/float/bool
                    if value.lower() == 'true':
                        val = True
                    elif value.lower() == 'false':
                        val = False
                    elif value.isdigit():
                        val = int(value)
                    elif '.' in value and value.replace('.', '').isdigit():
                        val = float(value)
                    else:
                        val = value
                    config.set(key.strip(), val)
                except ValueError:
                    config.set(key.strip(), value)
    
    # Determine output format
    output_format = 'key_value'
    if args.format:
        output_format = args.format.lower()
    elif detect_file_format(args.output_file) in ['json', 'yaml']:
        output_format = detect_file_format(args.output_file)
    
    # Save configuration
    success = False
    if output_format == 'json':
        success = config.saveToJsonFile(args.output_file)
    elif output_format == 'yaml':
        success = config.saveToYamlFile(args.output_file)
    else:
        success = config.saveToFile(args.output_file)
    
    if success:
        if args.json:
            result = {
                "success": True,
                "message": f"Configuration saved successfully to {args.output_file}",
                "file_path": args.output_file,
                "config_summary": config.summary()
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"Configuration saved successfully to {args.output_file}")
            print(f"Summary: {config.summary()}")
    else:
        if args.json:
            result = {
                "success": False,
                "message": f"Failed to save configuration to {args.output_file}",
                "file_path": args.output_file
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"Failed to save configuration to {args.output_file}")


def load_config(args: argparse.Namespace) -> None:
    """Load configuration from file."""
    logger = logging.getLogger("config_manager")
    
    if not args.config_file:
        logger.error("Config file path is required for load operation")
        sys.exit(1)
    
    if not os.path.exists(args.config_file):
        logger.error(f"Config file does not exist: {args.config_file}")
        sys.exit(1)
    
    config = pynlm.createDefaultConfig()
    
    # Determine input format
    input_format = 'auto'
    if args.format:
        input_format = args.format.lower()
    
    success = False
    if input_format == 'auto' or input_format == 'json':
        success = config.loadFromJsonFile(args.config_file)
    elif input_format == 'yaml':
        success = config.loadFromYamlFile(args.config_file)
    else:
        success = config.loadFromFile(args.config_file)
    
    if success:
        if args.json:
            # Get all configuration keys and values
            keys = config.getKeys()
            config_data = {}
            
            for key in keys:
                value = config.get(key)
                config_data[key] = value
            
            result = {
                "success": True,
                "message": f"Configuration loaded successfully from {args.config_file}",
                "file_path": args.config_file,
                "config": config_data,
                "config_summary": config.summary()
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"Configuration loaded successfully from {args.config_file}")
            print(f"Summary: {config.summary()}")
            print(f"Keys: {', '.join(config.getKeys())}")
    else:
        if args.json:
            result = {
                "success": False,
                "message": f"Failed to load configuration from {args.config_file}",
                "file_path": args.config_file
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"Failed to load configuration from {args.config_file}")


def inspect_config(args: argparse.Namespace) -> None:
    """Inspect configuration properties."""
    logger = logging.getLogger("config_manager")
    
    config = pynlm.createDefaultConfig()
    
    # Load custom config file if specified
    if args.config_file and os.path.exists(args.config_file):
        config.loadFromFile(args.config_file)
    
    if args.json:
        keys = config.getKeys()
        config_data = {}
        
        for key in keys:
            value = config.get(key)
            config_data[key] = value
        
        result = {
            "config_info": {
                "total_keys": len(keys),
                "keys": keys,
                "summary": config.summary()
            },
            "config_data": config_data
        }
        json.dump(result, sys.stdout, indent=2)
    else:
        print(f"Configuration Summary:")
        print(f"  Total Keys: {len(config.getKeys())}")
        print(f"  Keys: {', '.join(config.getKeys())}")
        print(f"  Full Summary: {config.summary()}")
        
        # Show sample of key values
        print(f"\nSample Key-Value Pairs:")
        sample_keys = list(config.getKeys())[:10]
        for key in sample_keys:
            value = config.get(key)
            print(f"  {key}: {value} ({type(value).__name__})")


def set_param(args: argparse.Namespace) -> None:
    """Set a configuration parameter."""
    logger = logging.getLogger("config_manager")
    
    if not args.key:
        logger.error("Config key is required for set operation")
        sys.exit(1)
    
    if not args.value:
        logger.error("Config value is required for set operation")
        sys.exit(1)
    
    config = pynlm.createDefaultConfig()
    
    # Load existing config if specified
    if args.config_file and os.path.exists(args.config_file):
        config.loadFromFile(args.config_file)
    
    # Parse value type
    try:
        if args.value.lower() == 'true':
            val = True
        elif args.value.lower() == 'false':
            val = False
        elif args.value.isdigit():
            val = int(args.value)
        elif '.' in args.value and args.value.replace('.', '').isdigit():
            val = float(args.value)
        else:
            val = args.value
    except ValueError:
        val = args.value
    
    config.set(args.key, val)
    
    # Save if output file specified
    if args.output_file:
        # Determine output format
        output_format = 'key_value'
        if args.format:
            output_format = args.format.lower()
        elif detect_file_format(args.output_file) in ['json', 'yaml']:
            output_format = detect_file_format(args.output_file)
        
        success = False
        if output_format == 'json':
            success = config.saveToJsonFile(args.output_file)
        elif output_format == 'yaml':
            success = config.saveToYamlFile(args.output_file)
        else:
            success = config.saveToFile(args.output_file)
        
        if success:
            message = f"Parameter set successfully: {args.key} = {val}, saved to {args.output_file}"
        else:
            message = f"Parameter set but failed to save to {args.output_file}"
    else:
        message = f"Parameter set successfully: {args.key} = {val}"
    
    if args.json:
        result = {
            "success": True,
            "message": message,
            "config": {
                "key": args.key,
                "value": val,
                "value_type": type(val).__name__
            },
            "config_summary": config.summary()
        }
        json.dump(result, sys.stdout, indent=2)
    else:
        print(message)


def get_param(args: argparse.Namespace) -> None:
    """Get a configuration parameter value."""
    logger = logging.getLogger("config_manager")
    
    if not args.key:
        logger.error("Config key is required for get operation")
        sys.exit(1)
    
    config = pynlm.createDefaultConfig()
    
    # Load existing config if specified
    if args.config_file and os.path.exists(args.config_file):
        config.loadFromFile(args.config_file)
    
    if config.has(args.key):
        value = config.get(args.key)
        
        if args.json:
            result = {
                "success": True,
                "config": {
                    "key": args.key,
                    "value": value,
                    "value_type": type(value).__name__ if value is not None else None,
                    "exists": True
                }
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"{args.key}: {value} ({type(value).__name__ if value is not None else 'None'})")
    else:
        if args.json:
            result = {
                "success": False,
                "error": f"Key '{args.key}' not found in configuration",
                "config": {
                    "key": args.key,
                    "exists": False
                }
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"ERROR: Key '{args.key}' not found in configuration")


def remove_param(args: argparse.Namespace) -> None:
    """Remove a configuration parameter."""
    logger = logging.getLogger("config_manager")
    
    if not args.key:
        logger.error("Config key is required for remove operation")
        sys.exit(1)
    
    config = pynlm.createDefaultConfig()
    
    # Load existing config if specified
    if args.config_file and os.path.exists(args.config_file):
        config.loadFromFile(args.config_file)
    
    if config.has(args.key):
        config.remove(args.key)
        
        # Save if output file specified
        if args.output_file:
            # Determine output format
            output_format = 'key_value'
            if args.format:
                output_format = args.format.lower()
            elif detect_file_format(args.output_file) in ['json', 'yaml']:
                output_format = detect_file_format(args.output_file)
            
            success = False
            if output_format == 'json':
                success = config.saveToJsonFile(args.output_file)
            elif output_format == 'yaml':
                success = config.saveToYamlFile(args.output_file)
            else:
                success = config.saveToFile(args.output_file)
            
            if success:
                message = f"Parameter removed successfully: {args.key}"
            else:
                message = f"Parameter removed but failed to save to {args.output_file}"
        else:
            message = f"Parameter removed successfully: {args.key}"
        
        if args.json:
            result = {
                "success": True,
                "message": message,
                "config": {
                    "removed_key": args.key,
                    "remaining_keys": config.getKeys()
                }
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(message)
    else:
        if args.json:
            result = {
                "success": False,
                "error": f"Key '{args.key}' not found in configuration",
                "config": {
                    "key": args.key,
                    "exists": False
                }
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"ERROR: Key '{args.key}' not found in configuration")


def validate_config(args: argparse.Namespace) -> None:
    """Validate configuration file."""
    logger = logging.getLogger("config_manager")
    
    if not args.config_file:
        logger.error("Config file path is required for validation")
        sys.exit(1)
    
    if not os.path.exists(args.config_file):
        logger.error(f"Config file does not exist: {args.config_file}")
        sys.exit(1)
    
    config = pynlm.createDefaultConfig()
    
    # Determine input format
    input_format = 'auto'
    if args.format:
        input_format = args.format.lower()
    
    success = False
    if input_format == 'auto' or input_format == 'json':
        success = config.loadFromJsonFile(args.config_file)
    elif input_format == 'yaml':
        success = config.loadFromYamlFile(args.config_file)
    else:
        success = config.loadFromFile(args.config_file)
    
    if success:
        if args.json:
            result = {
                "success": True,
                "message": f"Configuration validation passed: {args.config_file}",
                "config_summary": config.summary(),
                "key_count": len(config.getKeys()),
                "validated_file": args.config_file
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"Configuration validation passed: {args.config_file}")
            print(f"Summary: {config.summary()}")
            print(f"Key count: {len(config.getKeys())}")
    else:
        if args.json:
            result = {
                "success": False,
                "message": f"Configuration validation failed: {args.config_file}",
                "validated_file": args.config_file
            }
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"Configuration validation failed: {args.config_file}")


def main(args: argparse.Namespace) -> int:
    """Main entry point for Configuration Manager CLI."""
    logger = logging.getLogger("config_manager")
    
    if args.verbose:
        setup_logging(logging.DEBUG)
    else:
        setup_logging(logging.INFO)
    
    # Execute command based on subcommand
    if args.command == 'create':
        create_config(args)
    elif args.command == 'load':
        load_config(args)
    elif args.command == 'inspect':
        inspect_config(args)
    elif args.command == 'set':
        set_param(args)
    elif args.command == 'get':
        get_param(args)
    elif args.command == 'remove':
        remove_param(args)
    elif args.command == 'validate':
        validate_config(args)
    else:
        logger.error(f"Unknown command: {args.command}")
        return 1
    
    return 0


def setup_logging(level: int = logging.INFO) -> None:
    """Setup logging configuration."""
    logging.basicConfig(
        level=level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )


def create_parser() -> argparse.ArgumentParser:
    """Create argument parser for Configuration Manager CLI."""
    parser = argparse.ArgumentParser(
        prog='nlm config',
        description='NLM Configuration Manager - Manage configuration files and parameters',
        epilog='Examples:\n'
              '  nlm config create --output-file config.json --params "neuron_count=500" "region_count=3"\n'
              '  nlm config load --config-file config.json --json\n'
              '  nlm config get --key neuron_count\n'
              '  nlm config set --key plasticity.stdp.enable --value true --output-file config.json\n'
              '  nlm config validate --config-file config.json'
    )
    
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Enable verbose logging'
    )
    
    parser.add_argument(
        '--json',
        action='store_true',
        help='Output results as JSON'
    )
    
    parser.add_argument(
        '--format',
        choices=['auto', 'json', 'yaml', 'key_value'],
        default='auto',
        help='Configuration file format (auto, json, yaml, or key_value)'
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create a new configuration file')
    create_parser.add_argument('--output-file', required=True, help='Output configuration file path')
    create_parser.add_argument('--params', nargs='+', help='Configuration parameters in key=value format')
    create_parser.set_defaults(func=create_config)
    
    # Load command
    load_parser = subparsers.add_parser('load', help='Load configuration from file')
    load_parser.add_argument('--config-file', required=True, help='Configuration file path')
    load_parser.set_defaults(func=load_config)
    
    # Inspect command
    inspect_parser = subparsers.add_parser('inspect', help='Inspect configuration properties')
    inspect_parser.add_argument('--config-file', help='Configuration file path (optional)')
    inspect_parser.set_defaults(func=inspect_config)
    
    # Set command
    set_parser = subparsers.add_parser('set', help='Set a configuration parameter')
    set_parser.add_argument('--key', required=True, help='Configuration key')
    set_parser.add_argument('--value', required=True, help='Configuration value')
    set_parser.add_argument('--config-file', help='Input configuration file (optional)')
    set_parser.add_argument('--output-file', help='Output configuration file (optional)')
    set_parser.set_defaults(func=set_param)
    
    # Get command
    get_parser = subparsers.add_parser('get', help='Get a configuration parameter value')
    get_parser.add_argument('--key', required=True, help='Configuration key')
    get_parser.add_argument('--config-file', help='Input configuration file (optional)')
    get_parser.set_defaults(func=get_param)
    
    # Remove command
    remove_parser = subparsers.add_parser('remove', help='Remove a configuration parameter')
    remove_parser.add_argument('--key', required=True, help='Configuration key')
    remove_parser.add_argument('--config-file', help='Input configuration file (optional)')
    remove_parser.add_argument('--output-file', help='Output configuration file (optional)')
    remove_parser.set_defaults(func=remove_param)
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate configuration file')
    validate_parser.add_argument('--config-file', required=True, help='Configuration file path')
    validate_parser.set_defaults(func=validate_config)
    
    return parser


if __name__ == '__main__':
    parser = create_parser()
    args = parser.parse_args()
    
    # Setup logging
    setup_logging(logging.DEBUG if args.verbose else logging.INFO)
    
    # Execute command
    try:
        if hasattr(args, 'func'):
            sys.exit(args.func(args))
        else:
            parser.print_help()
            sys.exit(0)
    except Exception as e:
        logging.error(f"Command failed: {e}")
        if args.json:
            json.dump({"success": False, "error": str(e)}, sys.stdout, indent=2)
        else:
            print(f"ERROR: {e}")
        sys.exit(1)
