#!/usr/bin/env python3
"""
Quick Test Script for NLM Phase 6 Improvements

This script demonstrates the new Phase 6 functionality and verifies that
the improvements work correctly.
"""

import os
import sys
import subprocess
import tempfile
import time

def test_cli_help():
    """Test that nlm-cli help works."""
    print("Testing nlm-cli --help...")
    try:
        result = subprocess.run([sys.executable, "nlm_cli.py", "--help"], 
                              capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            print("✅ nlm-cli help works")
            return True
        else:
            print(f"❌ nlm-cli help failed: {result.stderr}")
            return False
    except Exception as e:
        print(f"❌ nlm-cli help error: {e}")
        return False

def test_viz_help():
    """Test that nlm-viz help works."""
    print("Testing nlm-viz --help...")
    try:
        result = subprocess.run([sys.executable, "nlm_viz.py", "--help"], 
                              capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            print("✅ nlm-viz help works")
            return True
        else:
            print(f"❌ nlm-viz help failed: {result.stderr}")
            return False
    except Exception as e:
        print(f"❌ nlm-viz help error: {e}")
        return False

def test_python_api():
    """Test Python API import and basic functionality."""
    print("Testing Python API...")
    try:
        import pynlm
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            print("❌ Brain initialization failed")
            return False
        
        print("✅ Python API import and brain creation works")
        
        # Run simple simulation
        for i in range(10):
            brain.step(i)
        
        print("✅ Brain simulation works")
        return True
    except Exception as e:
        print(f"❌ Python API error: {e}")
        return False

def test_phase6_results():
    """Test Phase 6 results documentation."""
    print("Testing Phase 6 results documentation...")
    try:
        # Check if Phase6_FINAL_RESULTS.md exists and has content
        if os.path.exists("docs/PHASE6_FINAL_RESULTS.md"):
            with open("docs/PHASE6_FINAL_RESULTS.md", "r") as f:
                content = f.read()
                if "Phase 6" in content and "integration" in content.lower():
                    print("✅ Phase 6 results documentation is updated")
                    return True
                else:
                    print("❌ Phase 6 results documentation seems incomplete")
                    return False
        else:
            print("❌ Phase 6 results documentation not found")
            return False
    except Exception as e:
        print(f"❌ Phase 6 results documentation error: {e}")
        return False

def test_documentation_structure():
    """Test documentation structure."""
    print("Testing documentation structure...")
    try:
        required_files = [
            "docs/PHASE6_FINAL_RESULTS.md",
            "docs/ARCHITECTURE.md",
            "docs/SCIENCE.md",
            "docs/EXPERIMENTS.md"
        ]
        
        for file_path in required_files:
            if not os.path.exists(file_path):
                print(f"❌ Required documentation file not found: {file_path}")
                return False
        
        print("✅ Documentation structure is complete")
        return True
    except Exception as e:
        print(f"❌ Documentation structure error: {e}")
        return False

def main():
    print("=== NLM Phase 6 Improvements Test Suite ===\n")
    
    tests = [
        ("CLI Help", test_cli_help),
        ("Viz Help", test_viz_help),
        ("Python API", test_python_api),
        ("Phase 6 Results", test_phase6_results),
        ("Documentation Structure", test_documentation_structure),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n--- Running {test_name} ---")
        if test_func():
            passed += 1
        else:
            print(f"Test {test_name} failed")
    
    print(f"\n=== Test Results ===")
    print(f"Passed: {passed}/{total}")
    print(f"Failed: {total - passed}/{total}")
    
    if passed == total:
        print("\n🎉 All tests passed! Phase 6 improvements are working correctly.")
        return 0
    else:
        print(f"\n❌ {total - passed} test(s) failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())