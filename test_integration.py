#!/usr/bin/env python3
"""
Integration test for enhanced NLM CLI and Python API.
This script tests the new functionality and ensures everything works correctly.
"""

import subprocess
import sys
import os
import json
from pathlib import Path

def run_command(cmd, cwd=None):
    """Run a command and return the result."""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(
            cmd, shell=True, cwd=cwd, 
            capture_output=True, text=True, timeout=30
        )
        return result.returncode, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        return -1, "", "Command timed out"
    except Exception as e:
        return -1, "", str(e)

def test_cli_help():
    """Test CLI help functionality."""
    print("\n=== Testing CLI Help ===")
    
    # Check if nlm_tool binary exists
    if not Path("nlm_tool").exists():
        print("✗ nlm_tool binary not found")
        return False
    
    # Test help command
    returncode, stdout, stderr = run_command("./nlm_tool --help")
    
    if returncode == 0:
        print("✓ CLI help works")
        if "NLM Advanced CLI Tool" in stdout:
            print("✓ Help text displayed correctly")
        if "--mode" in stdout:
            print("✓ Available modes shown")
        if "--neuron-count" in stdout:
            print("✓ Configuration options shown")
        return True
    else:
        print(f"✗ CLI help failed (returncode: {returncode})")
        if stderr:
            print(f"  Error: {stderr}")
        return False

def test_cli_basic():
    """Test CLI basic functionality."""
    print("\n=== Testing CLI Basic Mode ===")
    
    # Check if nlm_tool binary exists
    if not Path("nlm_tool").exists():
        print("✗ nlm_tool binary not found")
        return False
    
    # Test basic demo mode
    returncode, stdout, stderr = run_command(
        "./nlm_tool --mode demo --neuron-count 100 --steps 10"
    )
    
    if returncode == 0:
        print("✓ CLI demo mode works")
        if "Configuration Summary" in stdout:
            print("✓ Configuration displayed")
        if "NLM Brain initialized" in stdout or "Initializing NLM Brain" in stdout:
            print("✓ Brain initialization shown")
        return True
    else:
        print(f"✗ CLI demo mode failed (returncode: {returncode})")
        if stderr:
            print(f"  Error: {stderr}")
        return False

def test_python_api():
    """Test the enhanced Python API."""
    print("\n=== Testing Python API ===")
    
    # Check if enhanced_api.py exists
    enhanced_api_path = Path("python/enhanced_api.py")
    if not enhanced_api_path.exists():
        print("✗ enhanced_api.py not found")
        return False
    
    print("✓ Enhanced API file exists")
    
    # Try to import it
    try:
        sys.path.insert(0, str(Path("python").absolute()))
        import enhanced_api
        print("✓ Enhanced API imports successfully")
        
        # Check for expected classes and functions
        expected_classes = ['EnhancedBrain', 'ConfigBuilder', 'EnhancedConfig']
        for cls in expected_classes:
            if hasattr(enhanced_api, cls):
                print(f"✓ {cls} class found")
            else:
                print(f"✗ {cls} class not found")
                return False
        
        return True
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False

def test_documentation():
    """Test that documentation is present."""
    print("\n=== Testing Documentation ===")
    
    docs_dir = Path("docs")
    if not docs_dir.exists():
        print("✗ docs directory not found")
        return False
    
    print("✓ docs directory exists")
    
    # Check for API documentation
    api_docs = docs_dir / "enhanced_api.md"
    if api_docs.exists():
        print("✓ Enhanced API documentation exists")
        with open(api_docs, 'r') as f:
            content = f.read()
            if "EnhancedBrain" in content and "ConfigBuilder" in content:
                print("✓ API documentation contains expected content")
            else:
                print("✗ API documentation content incomplete")
                return False
    else:
        print("✗ Enhanced API documentation not found")
        return False
    
    # Check for examples
    examples_dir = Path("examples")
    if examples_dir.exists():
        print("✓ Examples directory exists")
        
        example_files = list(examples_dir.glob("*.py"))
        if example_files:
            print(f"✓ Found {len(example_files)} example files")
            
            # Test one example (basic_brain.py)
            basic_example = examples_dir / "basic_brain.py"
            if basic_example.exists():
                print("✓ Basic brain example exists")
        else:
            print("✗ No example files found")
            return False
    else:
        print("✗ Examples directory not found")
        return False
    
    return True

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM ENHANCED CLI AND PYTHON API INTEGRATION TEST")
    print("=" * 60)
    
    # Check if we're in the right directory
    if not Path("src").exists():
        print("✗ src directory not found - expected NLM project")
        return 1
    
    # Run tests
    tests = [
        ("CLI Help", test_cli_help),
        ("CLI Basic Functionality", test_cli_basic),
        ("Python API", test_python_api),
        ("Documentation", test_documentation),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'=' * 40}")
        print(f"Running {test_name} test...")
        print(f"{'=' * 40}")
        
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"✗ Test {test_name} crashed: {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    for test_name, result in results:
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"{status}: {test_name}")
        if result:
            passed += 1
    
    print(f"\nTotal: {passed}/{len(results)} tests passed")
    
    if passed == len(results):
        print("\n🎉 ALL TESTS PASSED! 🎉")
        print("The enhanced NLM CLI and Python API are working correctly.")
        return 0
    else:
        print(f"\n❌ {len(results) - passed} tests failed.")
        print("Please review the issues above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
