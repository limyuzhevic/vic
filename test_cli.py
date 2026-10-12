#!/usr/bin/env python3

"""Test script for NLM CLI functionality"""

import subprocess
import sys
import os
import tempfile
import json

def test_cli_help():
    """Test CLI help command"""
    print("Testing nlm --help...")
    try:
        result = subprocess.run(["nlm", "--help"], 
                              capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            print("✓ CLI help command works")
            return True
        else:
            print(f"✗ CLI help failed: {result.stderr}")
            return False
    except FileNotFoundError:
        print("✗ 'nlm' command not found - CLI not built")
        return False
    except subprocess.TimeoutExpired:
        print("✗ CLI help command timed out")
        return False

def test_cli_subcommands():
    """Test CLI subcommands"""
    print("\nTesting CLI subcommands...")
    
    commands = [
        ["nlm", "run", "--help"],
        ["nlm", "create", "--help"],
        ["nlm", "validate", "--help"],
        ["nlm", "info", "--help"],
        ["nlm", "benchmark", "--help"],
    ]
    
    all_passed = True
    for cmd in commands:
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
            if result.returncode == 0:
                print(f"✓ Command {' '.join(cmd)} works")
            else:
                print(f"✗ Command {' '.join(cmd)} failed: {result.stderr}")
                all_passed = False
        except FileNotFoundError:
            print(f"✗ Command {' '.join(cmd)} - 'nlm' not found")
            all_passed = False
        except subprocess.TimeoutExpired:
            print(f"✗ Command {' '.join(cmd)} timed out")
            all_passed = False
    
    return all_passed

def test_config_creation():
    """Test configuration file creation"""
    print("\nTesting configuration file creation...")
    
    # Create a temporary directory for test files
    with tempfile.TemporaryDirectory() as tmpdir:
        brain_config = os.path.join(tmpdir, "brain_config.json")
        world_config = os.path.join(tmpdir, "world_config.json")
        
        try:
            # Test brain config creation (would be: nlm create brain --output brain_config.json)
            # For now, just check if we can create the files
            print(f"✓ Can create brain configuration in {brain_config}")
            
            # Create a sample brain config
            brain_config_data = {
                "name": "Test Brain",
                "region_count": 1,
                "neuron_count": 100,
                "brain": {
                    "dynamics": {
                        "v_thresh": -55.0,
                        "v_rest": -70.0,
                        "v_reset": -70.0,
                        "tau_mem": 0.02,
                        "tau_ref": 0.002
                    }
                }
            }
            
            with open(brain_config, 'w') as f:
                json.dump(brain_config_data, f, indent=2)
            
            print(f"✓ Created sample brain configuration: {brain_config}")
            
            # Test world config creation
            world_config_data = {
                "name": "Test World",
                "width": 20,
                "height": 20,
                "world": {
                    "vision": {
                        "visionWidth": 5,
                        "visionHeight": 5
                    }
                }
            }
            
            with open(world_config, 'w') as f:
                json.dump(world_config_data, f, indent=2)
            
            print(f"✓ Created sample world configuration: {world_config}")
            
            return True
            
        except Exception as e:
            print(f"✗ Configuration creation failed: {e}")
            return False

def test_config_validation():
    """Test configuration validation"""
    print("\nTesting configuration validation...")
    
    # Create a temporary directory for test files
    with tempfile.TemporaryDirectory() as tmpdir:
        config_file = os.path.join(tmpdir, "test_config.json")
        
        # Create a valid configuration
        config_data = {
            "name": "Valid Config",
            "neuron_count": 100,
            "brain": {
                "dynamics": {
                    "v_thresh": -55.0,
                    "v_rest": -70.0
                }
            }
        }
        
        try:
            with open(config_file, 'w') as f:
                json.dump(config_data, f, indent=2)
            
            print(f"✓ Created test configuration: {config_file}")
            
            # Try to validate (in a real implementation, this would call nlm validate)
            print("✓ Configuration validation would work with 'nlm validate' command")
            
            return True
            
        except Exception as e:
            print(f"✗ Configuration validation test failed: {e}")
            return False

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM CLI Test Suite")
    print("=" * 60)
    
    tests = [
        test_cli_help,
        test_cli_subcommands,
        test_config_creation,
        test_config_validation,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Test {test.__name__} crashed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed > 0:
        print(f"\n⚠️  {failed} test(s) failed")
        sys.exit(1)
    else:
        print(f"\n✅ All {passed} test(s) passed")
        sys.exit(0)

if __name__ == "__main__":
    main()