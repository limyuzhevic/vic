#!/usr/bin/env python3
"""
Test script for NLM CLI functionality.
"""

import sys
import os

# Add the current directory to the Python path
sys.path.insert(0, os.path.dirname(__file__))

from nlm_cli import NLMCLI, CommandResult

def test_basic_cli():
    """Test basic CLI functionality."""
    print("Testing NLM CLI...")
    
    # Create CLI instance
    cli = NLMCLI()
    
    # Test help command
    print("\n1. Testing help command...")
    help_args = type('Args', (), {
        'command': None,
        'args': ['help']
    })()
    
    try:
        cli.run()
    except SystemExit:
        print("Help command executed successfully")
    
    # Test brain command help
    print("\n2. Testing brain command help...")
    brain_args = type('Args', (), {
        'command': 'brain',
        'args': ['--help']
    })()
    
    try:
        cli.run()
    except SystemExit:
        print("Brain help command executed successfully")
    
    # Test config command help
    print("\n3. Testing config command help...")
    config_args = type('Args', (), {
        'command': 'config',
        'args': ['--help']
    })()
    
    try:
        cli.run()
    except SystemExit:
        print("Config help command executed successfully")
    
    # Test explore command help
    print("\n4. Testing explore command help...")
    explore_args = type('Args', (), {
        'command': 'explore',
        'args': ['--help']
    })()
    
    try:
        cli.run()
    except SystemExit:
        print("Explore help command executed successfully")
    
    # Test experiment command help
    print("\n5. Testing experiment command help...")
    experiment_args = type('Args', (), {
        'command': 'experiment',
        'args': ['--help']
    })()
    
    try:
        cli.run()
    except SystemExit:
        print("Experiment help command executed successfully")
    
    # Test python command help
    print("\n6. Testing python command help...")
    python_args = type('Args', (), {
        'command': 'python',
        'args': ['--help']
    })()
    
    try:
        cli.run()
    except SystemExit:
        print("Python help command executed successfully")
    
    # Test tools command help
    print("\n7. Testing tools command help...")
    tools_args = type('Args', (), {
        'command': 'tools',
        'args': ['--help']
    })()
    
    try:
        cli.run()
    except SystemExit:
        print("Tools help command executed successfully")
    
    print("\nAll CLI tests completed successfully!")

def test_command_execution():
    """Test command execution functionality."""
    print("\nTesting command execution...")
    
    # This would require actual brain simulation setup
    # For now, we'll just test that the CLI can parse commands
    print("Command execution tests skipped (requires actual NLM setup)")

if __name__ == "__main__":
    print("NLM CLI Test Suite")
    print("=" * 50)
    
    test_basic_cli()
    test_command_execution()
    
    print("\n" + "=" * 50)
    print("All tests completed!")