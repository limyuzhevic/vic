#!/usr/bin/env python3
"""
NLM Configuration Utility

Helper script for managing NLM configurations and generating example configurations.

Usage:
    nlm_config [command] [options]

Commands:
    create        Create a new configuration file
    validate      Validate a configuration file
    compare       Compare two configuration files
    show          Show configuration details
    help          Show help information
"""

import argparse
import json
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
def create_config(args):
    """Create a new configuration file with default or custom values."""
    print("=== NLM Configuration Creator ===")
    
    # Create default config
    config = nlm.createDefaultConfig()
    
    # Apply command line overrides if provided
    if args.neuron_count:
        config.set("brain.neuron_count", args.neuron_count)
    if args.region_count:
        config.set("brain.region_count", args.region_count)
    if args.connection_probability:
        config.set("brain.connection_probability", args.connection_probability)
    if args.stdp_ltp_weight:
        config.set("stdp_ltp_weight", args.stdp_ltp_weight)
    if args.stdp_ltd_weight:
        config.set("stdp_ltd_weight", args.stdp_ltd_weight)
    if args.synaptogenesis_rate:
        config.set("synaptogenesis_rate", args.synaptogenesis_rate)
    if args.pruning_rate:
        config.set("pruning_rate", args.pruning_rate)
    if args.random_seed:
        config.set("random_seed", args.random_seed)
    if args.checkpoint_dir:
        config.set("checkpoint_dir", args.checkpoint_dir)
    
    # Save to file
    filename = args.output if args.output else "nlm_config.json"
    config.saveToFile(filename)
    
    print(f"Configuration saved to: {filename}")
    
    # Show summary
    print("\n=== Configuration Summary ===")
    print(config.summary())
    
    # Show Python import example
    print("\n=== Python Usage Example ===")
    print("```python")
    print("import pynlm")
    print("config = pynlm.createDefaultConfig()")
    
    if args.neuron_count:
        print(f"config.set('brain.neuron_count', {args.neuron_count})")
    if args.region_count:
        print(f"config.set('brain.region_count', {args.region_count})")
    if args.connection_probability:
        print(f"config.set('brain.connection_probability', {args.connection_probability})")
    
    print("brain = pynlm.createBrain(config)")
    print("brain.initialize()")
    print("```")
    
    return config
def validate_config(args):
    """Validate a configuration file."""
    print("=== NLM Configuration Validator ===")
    
    filename = args.input if args.input else "nlm_config.json"
    
    try:
        config = nlm.createDefaultConfig()
        if config.loadFromFile(filename):
            print(f"✓ Configuration file '{filename}' is valid")
            
            # Show key information
            print("\n=== Configuration Keys ===")
            keys = config.getKeys()
            for key in sorted(keys):
                print(f"  {key}")
            
            return True
        else:
            print(f"✗ Failed to load configuration from '{filename}'")
            return False
            
    except Exception as e:
        print(f"✗ Error validating configuration: {e}")
        return False
def compare_configs(args):
    """Compare two configuration files."""
    print("=== NLM Configuration Comparator ===")
    
    try:
        # Load first config
        config1 = nlm.createDefaultConfig()
        if not config1.loadFromFile(args.config1):
            print(f"✗ Failed to load '{args.config1}'")
            return
        
        # Load second config
        config2 = nlm.createDefaultConfig()
        if not config2.loadFromFile(args.config2):
            print(f"✗ Failed to load '{args.config2}'")
            return
        
        # Compare keys
        keys1 = set(config1.getKeys())
        keys2 = set(config2.getKeys())
        
        print(f"Config 1 ('{args.config1}'): {len(keys1)} keys")
        print(f"Config 2 ('{args.config2}'): {len(keys2)} keys")
        
        # Find differences
        only_in_first = keys1 - keys2
        only_in_second = keys2 - keys1
        both = keys1 & keys2
        
        print(f"\nKeys only in first config: {len(only_in_first)}")
        for key in sorted(only_in_first):
            val1 = config1.get(key) if config1.has(key) else None
            print(f"  {key}: {val1}")
        
        print(f"\nKeys only in second config: {len(only_in_second)}")
        for key in sorted(only_in_second):
            val2 = config2.get(key) if config2.has(key) else None
            print(f"  {key}: {val2}")
        
        # Compare common keys
        if both:
            print(f"\nCommon keys ({len(both)}):")
            diff_count = 0
            for key in sorted(both):
                val1 = config1.get(key) if config1.has(key) else None
                val2 = config2.get(key) if config2.has(key) else None
                
                if val1 != val2:
                    print(f"  {key}:")
                    print(f"    Config 1: {val1}")
                    print(f"    Config 2: {val2}")
                    diff_count += 1
            
            if diff_count == 0:
                print("  (All common keys have identical values)")
        
    except Exception as e:
        print(f"✗ Error comparing configurations: {e}")
        return
    
    print("\n=== Comparison Complete ===")
def show_config(args):
    """Show configuration details in a formatted way."""
    print("=== NLM Configuration Viewer ===")
    
    filename = args.input if args.input else "nlm_config.json"
    
    try:
        config = nlm.createDefaultConfig()
        if config.loadFromFile(filename):
            print(f"Configuration file: {filename}")
            print()
            
            # Show all keys with values
            keys = config.getKeys()
            if not keys:
                print("  (No configuration keys)")
            else:
                print(f"Total keys: {len(keys)}")
                print()
                
                for key in sorted(keys):
                    value = config.get(key)
                    source = "unknown"
                    # Note: Config doesn't have getSource() method, so we can't show this
                    
                    if isinstance(value, (int, float, bool)):
                        print(f"  {key}: {value} ({type(value).__name__})")
                    elif isinstance(value, str):
                        # Truncate long strings
                        display = value if len(value) < 80 else value[:77] + "..."
                        print(f"  {key}: '{display}' ({type(value).__name__})")
                    elif isinstance(value, list):
                        display = f"[{len(value)} items]"
                        print(f"  {key}: {display} ({type(value).__name__})")
                    else:
                        print(f"  {key}: {value} ({type(value).__name__})")
            
            print()
            print("=== Configuration Summary ===")
            print(config.summary())
            
        else:
            print(f"✗ Failed to load configuration from '{filename}'")
            
    except Exception as e:
        print(f"✗ Error loading configuration: {e}")
        return
    
    print("\n=== Configuration Viewer Complete ===")
def main():
    parser = argparse.ArgumentParser(
        description="NLM Configuration Utility",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Create command
    create_parser = subparsers.add_parser("create", help="Create a new configuration file")
    create_parser.add_argument("--neuron-count", type=int, help="Number of neurons (default: 1000)")
    create_parser.add_argument("--region-count", type=int, help="Number of brain regions (default: 1)")
    create_parser.add_argument("--connection-probability", type=float, help="Synapse connection probability (default: 0.1)")
    create_parser.add_argument("--stdp-ltp-weight", type=float, help="STDP LTP weight (default: 0.01)")
    create_parser.add_argument("--stdp-ltd-weight", type=float, help="STDP LTD weight (default: 0.012)")
    create_parser.add_argument("--synaptogenesis-rate", type=float, help="Synaptogenesis rate (default: 0.0001)")
    create_parser.add_argument("--pruning-rate", type=float, help="Pruning rate (default: 0.00001)")
    create_parser.add_argument("--random-seed", type=int, help="Random seed (default: 42)")
    create_parser.add_argument("--checkpoint-dir", help="Checkpoint directory (default: ./checkpoints)")
    create_parser.add_argument("--output", help="Output filename (default: nlm_config.json)")
    
    # Validate command
    validate_parser = subparsers.add_parser("validate", help="Validate a configuration file")
    validate_parser.add_argument("input", nargs="?", help="Configuration file to validate")
    
    # Compare command
    compare_parser = subparsers.add_parser("compare", help="Compare two configuration files")
    compare_parser.add_argument("config1", help="First configuration file")
    compare_parser.add_argument("config2", help="Second configuration file")
    
    # Show command
    show_parser = subparsers.add_parser("show", help="Show configuration details")
    show_parser.add_argument("input", nargs="?", help="Configuration file to view")
    
    # Help command
    help_parser = subparsers.add_parser("help", help="Show help information")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Dispatch commands
    if args.command == "create":
        create_config(args)
    elif args.command == "validate":
        validate_config(args)
    elif args.command == "compare":
        compare_configs(args)
    elif args.command == "show":
        show_config(args)
    elif args.command == "help" or not args.command:
        parser.print_help()
    else:
        parser.print_help()
        sys.exit(1)
if __name__ == "__main__":
    main()
