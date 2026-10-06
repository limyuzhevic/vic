#!/usr/bin/env python3
"""
Simple test to verify nlm-config CLI functionality
"""

import subprocess
import sys
import os
import tempfile
import json

def test_cli_help():
    """Test basic help command"""
    print("Testing nlm-config --help...")
    result = subprocess.run([sys.executable, 'scripts/nlm-config', '--help'],
                          capture_output=True, text=True, cwd='/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_5f7d145d-d3ef-44a3-9694-72e4baa627e4')
    
    if result.returncode == 0:
        print("✓ Help command works")
        return True
    else:
        print(f"✗ Help command failed: {result.stderr}")
        return False

def test_cli_list():
    """Test list command with example config"""
    print("\nTesting nlm-config list...")
    
    # Create a simple config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.write("""
neuron_count: 100
region_count: 2
simulation_timestep: 0.01
enable_structural_plasticity: true
""")
        config_file = f.name
    
    try:
        result = subprocess.run([sys.executable, 'scripts/nlm-config', 'list', '--file', config_file],
                              capture_output=True, text=True, cwd='/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_5f7d145d-d3ef-44a3-9694-72e4baa627e4')
        
        if result.returncode == 0 and 'neuron_count' in result.stdout:
            print("✓ List command works")
            return True
        else:
            print(f"✗ List command failed: {result.stderr}")
            return False
    finally:
        os.unlink(config_file)

def test_cli_get():
    """Test get command"""
    print("\nTesting nlm-config get...")
    
    # Create a config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.write("neuron_count: 500\n")
        config_file = f.name
    
    try:
        result = subprocess.run([sys.executable, 'scripts/nlm-config', 'get', '--file', config_file, 'neuron_count'],
                              capture_output=True, text=True, cwd='/workspace/cf14c76c-4b1f-4d-38-9a96-64cd8a400481/sessions/agent_5f7d145d-d3ef-44a3-9694-72e4baa627e4')
        
        if result.returncode == 0 and 'neuron_count: 500' in result.stdout:
            print("✓ Get command works")
            return True
        else:
            print(f"✗ Get command failed: {result.stderr}")
            return False
    finally:
        os.unlink(config_file)

def test_cli_template():
    """Test template command"""
    print("\nTesting nlm-config template...")
    
    result = subprocess.run([sys.executable, 'scripts/nlm-config', 'template', 'debug'],
                          capture_output=True, text=True, cwd='/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_5f7d145d-d3ef-44a3-9694-72e4baa627e4')
    
    if result.returncode == 0 and 'neuron_count' in result.stdout:
        print("✓ Template command works")
        return True
    else:
        print(f"✗ Template command failed: {result.stderr}")
        return False

def test_config_validation_help():
    """Test config_validation.py help"""
    print("\nTesting config_validation.py --help...")
    result = subprocess.run([sys.executable, 'scripts/config_validation.py', '--help'],
                          capture_output=True, text=True, cwd='/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_5f7d145d-d3ef-44a3-9694-72e4baa627e4')
    
    if result.returncode == 0:
        print("✓ Config validation help works")
        return True
    else:
        print(f"✗ Config validation help failed: {result.stderr}")
        return False

def test_config_validation_list():
    """Test config_validation.py list command"""
    print("\nTesting config_validation.py validate...")
    
    # Create a config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.write("neuron_count: 100\n")
        config_file = f.name
    
    try:
        result = subprocess.run([sys.executable, 'scripts/config_validation.py', 'validate', config_file],
                              capture_output=True, text=True, cwd='/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_5f7d145d-d3ef-44a3-9694-72e4baa627e4')
        
        if result.returncode == 0:
            print("✓ Config validation works")
            return True
        else:
            print(f"✗ Config validation failed: {result.stderr}")
            return False
    finally:
        os.unlink(config_file)

def main():
    print("NLM Config CLI Test Suite")
    print("=" * 50)
    
    tests = [
        test_cli_help,
        test_cli_list,
        test_cli_get,
        test_cli_template,
        test_config_validation_help,
        test_config_validation_list,
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
            print(f"✗ Test {test.__name__} failed with exception: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("\n✓ All tests passed!")
        return 0
    else:
        print(f"\n✗ {failed} test(s) failed")
        return 1

if __name__ == '__main__':
    sys.exit(main())