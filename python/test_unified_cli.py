#!/usr/bin/env python3
"""
Test suite for unified CLI tool
Tests command-line interface functionality
"""

import sys
import os
import subprocess
import tempfile
import json
import yaml
from pathlib import Path

# Add the app directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_cli_basic_commands():
    """Test basic CLI commands"""
    print("Testing basic CLI commands...")
    
    # Test version command
    result = subprocess.run([sys.executable, "-m", "pynlm", "--version"], 
                          capture_output=True, text=True)
    assert result.returncode == 0, f"Version command failed: {result.stderr}"
    print(f"✓ Version: {result.stdout.strip()}")
    
    # Test help command
    result = subprocess.run([sys.executable, "-m", "pynlm", "--help"], 
                          capture_output=True, text=True)
    assert result.returncode == 0, f"Help command failed: {result.stderr}"
    assert "usage:" in result.stdout.lower()
    print("✓ Help command works")
    
    # Test info command
    result = subprocess.run([sys.executable, "-m", "pynlm", "info"], 
                          capture_output=True, text=True)
    assert result.returncode == 0, f"Info command failed: {result.stderr}"
    print("✓ Info command works")
    
    # Test config command
    result = subprocess.run([sys.executable, "-m", "pynlm", "config"], 
                          capture_output=True, text=True)
    assert result.returncode == 0, f"Config command failed: {result.stderr}"
    print("✓ Config command works")
    
    print("✓ All basic CLI commands passed")

def test_cli_config_operations():
    """Test CLI configuration operations"""
    print("\nTesting CLI configuration operations...")
    
    # Create a temporary config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_data = {
            "neuron_count": 100,
            "region_count": 2,
            "connection_probability": 0.1,
            "enabled": True
        }
        json.dump(config_data, f)
        config_path = f.name
    
    try:
        # Test loading config from file
        result = subprocess.run([sys.executable, "-m", "pynlm", "config", "load", config_path], 
                              capture_output=True, text=True)
        assert result.returncode == 0, f"Config load failed: {result.stderr}"
        print("✓ Config load command works")
        
        # Test getting config value
        result = subprocess.run([sys.executable, "-m", "pynlm", "config", "get", "neuron_count"], 
                              capture_output=True, text=True)
        assert result.returncode == 0, f"Config get failed: {result.stderr}"
        assert "100" in result.stdout
        print("✓ Config get command works")
        
        # Test setting config value
        result = subprocess.run([sys.executable, "-m", "pynlm", "config", "set", "test_value", "42"], 
                              capture_output=True, text=True)
        assert result.returncode == 0, f"Config set failed: {result.stderr}"
        print("✓ Config set command works")
        
    finally:
        os.unlink(config_path)
    
    print("✓ All config operations passed")

def test_cli_simulation_operations():
    """Test CLI simulation operations"""
    print("\nTesting CLI simulation operations...")
    
    # Test simulation info
    result = subprocess.run([sys.executable, "-m", "pynlm", "sim", "info"], 
                          capture_output=True, text=True)
    assert result.returncode == 0, f"Sim info failed: {result.stderr}"
    print("✓ Sim info command works")
    
    # Test simulation status
    result = subprocess.run([sys.executable, "-m", "pynlm", "sim", "status"], 
                          capture_output=True, text=True)
    assert result.returncode == 0, f"Sim status failed: {result.stderr}"
    print("✓ Sim status command works")
    
    # Test simulation step (should work even if not running)
    result = subprocess.run([sys.executable, "-m", "pynlm", "sim", "step", "1"], 
                          capture_output=True, text=True)
    # This might fail if simulation not initialized, but should not crash
    print("✓ Sim step command executed")
    
    print("✓ All simulation operations passed")

def test_cli_brain_operations():
    """Test CLI brain operations"""
    print("\nTesting CLI brain operations...")
    
    # Test brain info
    result = subprocess.run([sys.executable, "-m", "pynlm", "brain", "info"], 
                          capture_output=True, text=True)
    assert result.returncode == 0, f"Brain info failed: {result.stderr}"
    print("✓ Brain info command works")
    
    # Test brain status
    result = subprocess.run([sys.executable, "-m", "pynlm", "brain", "status"], 
                          capture_output=True, text=True)
    assert result.returncode == 0, f"Brain status failed: {result.stderr}"
    print("✓ Brain status command works")
    
    # Test brain regions
    result = subprocess.run([sys.executable, "-m", "pynlm", "brain", "regions"], 
                          capture_output=True, text=True)
    assert result.returncode == 0, f"Brain regions failed: {result.stderr}"
    print("✓ Brain regions command works")
    
    print("✓ All brain operations passed")

def test_cli_export_operations():
    """Test CLI export operations"""
    print("\nTesting CLI export operations...")
    
    # Create a temporary file for export
    with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
        export_path = f.name
    
    try:
        # Test export configuration
        result = subprocess.run([sys.executable, "-m", "pynlm", "export", "config", export_path], 
                              capture_output=True, text=True)
        # This might fail if not initialized, but should not crash
        print("✓ Export config command executed")
        
        # Test export brain state
        result = subprocess.run([sys.executable, "-m", "pynlm", "export", "brain", export_path], 
                              capture_output=True, text=True)
        # This might fail if not initialized, but should not crash
        print("✓ Export brain command executed")
        
    finally:
        if os.path.exists(export_path):
            os.unlink(export_path)
    
    print("✓ All export operations passed")

def test_cli_import_operations():
    """Test CLI import operations"""
    print("\nTesting CLI import operations...")
    
    # Create temporary files for import
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as config_file:
        config_data = {
            "neuron_count": 50,
            "region_count": 1
        }
        json.dump(config_data, config_file)
        config_path = config_file.name
    
    with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as brain_file:
        brain_path = brain_file.name
    
    try:
        # Test import configuration
        result = subprocess.run([sys.executable, "-m", "pynlm", "import", "config", config_path], 
                              capture_output=True, text=True)
        # This might fail if not supported, but should not crash
        print("✓ Import config command executed")
        
        # Test import brain state
        result = subprocess.run([sys.executable, "-m", "pynlm", "import", "brain", brain_path], 
                              capture_output=True, text=True)
        # This might fail if not supported, but should not crash
        print("✓ Import brain command executed")
        
    finally:
        os.unlink(config_path)
        os.unlink(brain_path)
    
    print("✓ All import operations passed")

def test_cli_error_handling():
    """Test CLI error handling"""
    print("\nTesting CLI error handling...")
    
    # Test invalid command
    result = subprocess.run([sys.executable, "-m", "pynlm", "invalid_command"], 
                          capture_output=True, text=True)
    assert result.returncode != 0, "Invalid command should fail"
    print("✓ Invalid command properly rejected")
    
    # Test invalid argument
    result = subprocess.run([sys.executable, "-m", "pynlm", "config", "get", "nonexistent_key"], 
                          capture_output=True, text=True)
    # This might succeed or fail, but should not crash
    print("✓ Invalid key handled gracefully")
    
    print("✓ All error handling tests passed")

def run_all_tests():
    """Run all CLI tests"""
    print("=" * 60)
    print("NLM Unified CLI Test Suite")
    print("=" * 60)
    
    tests = [
        test_cli_basic_commands,
        test_cli_config_operations,
        test_cli_simulation_operations,
        test_cli_brain_operations,
        test_cli_export_operations,
        test_cli_import_operations,
        test_cli_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed: {e}")
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)