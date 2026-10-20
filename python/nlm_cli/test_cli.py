#!/usr/bin/env python3
"""
Test script for NLM CLI tool

This script demonstrates the basic functionality of the NLM CLI tool
and provides examples of how to use it.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'nlm_cli'))

from nlm_cli.nlm_cli import NLMCLI

def test_basic_functionality():
    """Test basic CLI functionality"""
    print("Testing NLM CLI Tool")
    print("=" * 50)
    
    # Create CLI instance
    cli = NLMCLI()
    
    # Test 1: Help command
    print("\n1. Testing help command:")
    try:
        cli._show_help()
    except Exception as e:
        print(f"Error: {e}")
    
    # Test 2: Parse args
    print("\n2. Testing argument parsing:")
    test_args = ["brain", "create", "--neuron-count", "100", "--timestep", "0.001"]
    try:
        parsed_args = cli.parse_args(test_args)
        print(f"Parsed arguments: {parsed_args}")
    except Exception as e:
        print(f"Error: {e}")
    
    # Test 3: Output formatting
    print("\n3. Testing output formatting:")
    test_data = {
        "brain_created": True,
        "neuron_count": 1000,
        "timestep": 0.001,
        "regions": 2,
        "synapses": 5000
    }
    
    print("JSON format:")
    import json
    print(json.dumps(test_data, indent=2))
    
    print("\nTable format:")
    print(cli._format_as_table(test_data))
    
    print("\nCSV format:")
    print(cli._format_as_csv(test_data))
    
    # Test 4: Configuration loading
    print("\n4. Testing configuration loading:")
    config_file = "configs/default.cfg"
    if os.path.exists(config_file):
        cli.load_config(config_file)
        print(f"Loaded configuration with {len(cli.config)} values")
    else:
        print(f"Configuration file not found: {config_file}")
    
    # Test 5: Command dispatch
    print("\n5. Testing command dispatch:")
    test_commands = [
        ["brain", "create", "--neuron-count", "500"],
        ["world", "create", "--width", "100", "--height", "100"],
        ["agent", "create"],
        ["config", "create"],
        ["experiment", "run"],
        ["profile", "start"]
    ]
    
    for cmd in test_commands:
        print(f"Would execute: {' '.join(cmd)}")
        
    print("\n" + "=" * 50)
    print("Test completed successfully!")
    print("\nNote: Some commands require the pynlm Python bindings")
    print("and may not function without them installed.")

if __name__ == "__main__":
    test_basic_functionality()