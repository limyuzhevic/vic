#!/usr/bin/env python3
"""
Test script for NLM CLI Tool
"""

import os
import json
from pathlib import Path

# Add the app directory to the path
app_dir = Path("/workspace/app")
if str(app_dir) not in os.sys.path:
    os.sys.path.insert(0, str(app_dir))

def test_cli_help():
    """Test CLI help functionality."""
    try:
        import subprocess
        result = subprocess.run(
            [sys.executable, str(app_dir / "nlm_cli.py"), "--action", "help"],
            capture_output=True,
            text=True,
            timeout=30
        )
        
        print("=== CLI Help Test ===")
        print("Return code:", result.returncode)
        print("STDOUT:", result.stdout[:500] + "..." if len(result.stdout) > 500 else result.stdout)
        print("STDERR:", result.stderr[:500] + "..." if len(result.stderr) > 500 else result.stderr)
        print()
        
        return result.returncode == 0
        
    except Exception as e:
        print(f"Error running CLI help test: {e}")
        return False

def test_cli_init():
    """Test CLI initialization."""
    try:
        import subprocess
        result = subprocess.run(
            [sys.executable, str(app_dir / "nlm_cli.py"), "--action", "init"],
            capture_output=True,
            text=True,
            timeout=60
        )
        
        print("=== CLI Init Test ===")
        print("Return code:", result.returncode)
        print("STDOUT:", result.stdout[:500] + "..." if len(result.stdout) > 500 else result.stdout)
        print("STDERR:", result.stderr[:500] + "..." if len(result.stderr) > 500 else result.stderr)
        print()
        
        return result.returncode == 0
        
    except Exception as e:
        print(f"Error running CLI init test: {e}")
        return False

def test_cli_config_file():
    """Test CLI with configuration file."""
    # Create a sample configuration file
    config_data = {
        "brain_neurons": 500,
        "brain_regions": 5,
        "world_width": 50,
        "world_height": 50,
        "max_steps_per_episode": 20,
        "episode_count": 3,
        "learning_rate": 0.005,
        "exploration_rate": 0.1,
        "enable_profiling": False,
        "enable_debug": True
    }
    
    config_file = app_dir / "test_config.json"
    with open(config_file, 'w') as f:
        json.dump(config_data, f, indent=2)
    
    try:
        import subprocess
        result = subprocess.run(
            [sys.executable, str(app_dir / "nlm_cli.py"), "--action", "init", "--config-file", str(config_file)],
            capture_output=True,
            text=True,
            timeout=60
        )
        
        print("=== CLI Config File Test ===")
        print("Return code:", result.returncode)
        print("STDOUT:", result.stdout[:500] + "..." if len(result.stdout) > 500 else result.stdout)
        print("STDERR:", result.stderr[:500] + "..." if len(result.stderr) > 500 else result.stderr)
        print()
        
        # Clean up
        if config_file.exists():
            config_file.unlink()
        
        return result.returncode == 0
        
    except Exception as e:
        print(f"Error running CLI config file test: {e}")
        return False

def test_cli_version():
    """Test CLI version functionality."""
    try:
        import subprocess
        result = subprocess.run(
            [sys.executable, str(app_dir / "nlm_cli.py"), "--version"],
            capture_output=True,
            text=True,
            timeout=30
        )
        
        print("=== CLI Version Test ===")
        print("Return code:", result.returncode)
        print("STDOUT:", result.stdout)
        print("STDERR:", result.stderr)
        print()
        
        return result.returncode == 0
        
    except Exception as e:
        print(f"Error running CLI version test: {e}")
        return False

def test_cli_output_formats():
    """Test different output formats."""
    try:
        import subprocess
        
        # Test human-readable format
        result = subprocess.run(
            [sys.executable, str(app_dir / "nlm_cli.py"), "--action", "init"],
            capture_output=True,
            text=True,
            timeout=60
        )
        
        print("=== CLI Output Format Test ===")
        print("Human-readable output sample:")
        if result.stdout:
            lines = result.stdout.split('\n')
            for line in lines[:10]:  # Show first 10 lines
                print(f"  {line}")
        print()
        
        return result.returncode == 0
        
    except Exception as e:
        print(f"Error running CLI output format test: {e}")
        return False

def main():
    """Run all tests."""
    print("Testing NLM CLI Tool")
    print("=" * 50)
    
    tests = [
        ("Help", test_cli_help),
        ("Init", test_cli_init),
        ("Config File", test_cli_config_file),
        ("Output Formats", test_cli_output_formats),
        ("Version", test_cli_version),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"Running {test_name} test...")
        try:
            success = test_func()
            results.append((test_name, success))
            print(f"{test_name} test: {'PASSED' if success else 'FAILED'}")
        except Exception as e:
            print(f"{test_name} test: ERROR - {e}")
            results.append((test_name, False))
        print()
    
    # Summary
    print("=== Test Summary ===")
    passed = sum(1 for _, success in results if success)
    total = len(results)
    print(f"Passed: {passed}/{total}")
    
    for test_name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"  {status} {test_name}")
    
    return passed == total

if __name__ == "__main__":
    import sys
    success = main()
    sys.exit(0 if success else 1)
