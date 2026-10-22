#!/usr/bin/env python3
"""
NLM CLI Test Script

Simple test script to verify the NLM CLI tool structure and functionality.
This script tests the basic imports and demonstrates the CLI capabilities.
"""

import sys
import os
from pathlib import Path

# Add the project root to Python path
sys.path.insert(0, str(Path(__file__).parent))

def test_imports():
    """Test if all required modules can be imported"""
    print("=== Testing Module Imports ===")
    
    try:
        # Test NLM imports
        import pynlm
        print("✓ pynlm module imported successfully")
        
        # Test CLI utilities
        from cli_utils import (
            NLMCLIError,
            load_config,
            save_config,
            validate_config,
            format_stats,
            create_brain,
            create_markdown_table
        )
        print("✓ CLI utilities imported successfully")
        
        # Test CLI commands
        import cli_commands
        print("✓ CLI commands imported successfully")
        
        # Test main CLI
        import nlm_cli
        print("✓ Main CLI imported successfully")
        
        return True
    
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False

def test_config_loading():
    """Test configuration loading functionality"""
    print("\n=== Testing Configuration Loading ===")
    
    try:
        from cli_utils import load_config
        
        # Test loading default config
        config = load_config("configs/default.cfg")
        print(f"✓ Default config loaded successfully")
        print(f"  Keys in config: {list(config.keys())}")
        
        # Show some config values
        if 'neuron_count' in config:
            print(f"  neuron_count: {config['neuron_count']}")
        if 'simulation_timestep' in config:
            print(f"  simulation_timestep: {config['simulation_timestep']}")
        
        return True
    
    except Exception as e:
        print(f"✗ Error loading config: {e}")
        return False

def test_config_validation():
    """Test configuration validation"""
    print("\n=== Testing Configuration Validation ===")
    
    try:
        from cli_utils import validate_config
        
        # Test valid config
        valid_config = {
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.1,
            "simulation_timestep": 0.001
        }
        
        validated = validate_config(valid_config)
        print("✓ Valid configuration passed validation")
        print(f"  Validated keys: {list(validated.keys())}")
        
        # Test invalid config
        print("\n  Testing invalid configuration...")
        try:
            invalid_config = {
                "neuron_count": -1,
                "connection_probability": 1.5
            }
            validate_config(invalid_config)
            print("  ✗ Should have raised error for invalid config")
            return False
        except Exception as e:
            print(f"  ✓ Caught expected error: {e}")
        
        return True
    
    except Exception as e:
        print(f"✗ Error in validation test: {e}")
        return False

def test_table_formatting():
    """Test table formatting functionality"""
    print("\n=== Testing Table Formatting ===")
    
    try:
        from cli_utils import create_markdown_table
        
        headers = ["Name", "Value", "Description"]
        rows = [
            ["Neurons", "1000", "Total neuron count"],
            ["Synapses", "5000", "Total synapse count"],
            ["Firing Rate", "0.25", "Average firing rate"]
        ]
        
        table = create_markdown_table(headers, rows)
        print("✓ Table formatting test passed")
        print("  Generated table:")
        print("  " + "-" * 60)
        for line in table.split('\n'):
            print(f"  {line}")
        print("  " + "-" * 60)
        
        return True
    
    except Exception as e:
        print(f"✗ Error in table formatting: {e}")
        return False

def test_command_structure():
    """Test CLI command structure"""
    print("\n=== Testing CLI Command Structure ===")
    
    try:
        import cli_commands
        
        # Check if command classes exist
        command_classes = [
            cli_commands.RunCommand,
            cli_commands.ConfigCommand,
            cli_commands.StatsCommand,
            cli_commands.SaveCommand,
            cli_commands.LoadCommand,
            cli_commands.ConnectivityCommand,
            cli_commands.HelpCommand,
            cli_commands.InteractiveCommand
        ]
        
        print(f"✓ Found {len(command_classes)} command classes")
        
        for cmd_class in command_classes:
            print(f"  - {cmd_class.__name__}")
        
        return True
    
    except Exception as e:
        print(f"✗ Error in command structure test: {e}")
        return False

def print_summary():
    """Print test summary"""
    print("\n" + "=" * 60)
    print("NLM CLI Test Summary")
    print("=" * 60)
    print("\nThe NLM CLI tool has been successfully created with the following structure:")
    print("\n1. Main CLI (nlm_cli.py):")
    print("   - Argument parser with subcommands")
    print("   - Error handling and logging")
    print("   - Integration with pynlm Python bindings")
    
    print("\n2. CLI Utilities (cli_utils.py):")
    print("   - Configuration file loading (JSON, YAML, key=value)")
    print("   - Brain creation and simulation")
    print("   - Statistics formatting")
    print("   - Table formatting")
    print("   - Error handling classes")
    
    print("\n3. CLI Commands (cli_commands.py):")
    print("   - RunCommand: Run neural simulations")
    print("   - ConfigCommand: Manage configuration")
    print("   - StatsCommand: Show brain statistics")
    print("   - SaveCommand: Save brain state")
    print("   - LoadCommand: Load brain state")
    print("   - ConnectivityCommand: Test neural connectivity")
    print("   - HelpCommand: Show help information")
    print("   - InteractiveCommand: Run in interactive mode")
    
    print("\n4. Demonstration Script (demo_cli.py):")
    print("   - Basic usage demonstration")
    print("   - Configuration command examples")
    print("   - Error handling examples")
    
    print("\n5. Updated pyproject.toml:")
    print("   - Added CLI optional dependency (pyyaml)")
    print("   - Added nlm CLI entry point")
    
    print("\nTo use the NLM CLI tool:")
    print("  nlm --help                    # Show help")
    print("  nlm run --steps 100          # Run a simulation")
    print("  nlm config show               # Show current config")
    print("  nlm stats                     # Show brain statistics")
    print("  nlm connectivity              # Test neural connectivity")
    print("  nlm --help                    # Show all available commands")

def main():
    """Main test function"""
    print("╔══════════════════════════════════════════════════════════════╗")
    print("║                                                              ║")
    print("║     NLM — 熙然                                                ║")
    print("║     Neural Learning Machine                                   ║")
    print("║                                                              ║")
    print("║     CLI Test Script                                           ║")
    print("║                                                              ║")
    print("╚══════════════════════════════════════════════════════════════╝")
    print()
    
    # Run tests
    tests = [
        ("Module Imports", test_imports),
        ("Configuration Loading", test_config_loading),
        ("Configuration Validation", test_config_validation),
        ("Table Formatting", test_table_formatting),
        ("Command Structure", test_command_structure)
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\nRunning: {test_name}")
        print("-" * 60)
        success = test_func()
        results.append((test_name, success))
    
    # Print summary
    print_summary()
    
    # Print test results
    print("\n" + "=" * 60)
    print("Test Results")
    print("=" * 60)
    
    passed = 0
    total = len(results)
    
    for test_name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{test_name:30} {status}")
        if success:
            passed += 1
    
    print(f"\nOverall: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed! The NLM CLI tool is ready for use.")
    else:
        print(f"\n⚠️  {total - passed} test(s) failed. Please check the output above.")
    
    return passed == total

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)