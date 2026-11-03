#!/usr/bin/env python3
"""
Test script to verify NLM Phase 6 integration and identify issues.
"""

import subprocess
import sys
import os

def run_cmd(cmd):
    print(f"Running: {cmd}")
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    print(f"Exit code: {result.returncode}")
    if result.stdout:
        print(f"STDOUT:\n{result.stdout}")
    if result.stderr:
        print(f"STDERR:\n{result.stderr}")
    return result.returncode == 0, result.stdout, result.stderr

def test_cmake_config():
    """Test if CMake configuration is working"""
    print("=== Testing CMake Configuration ===")
    
    # Check if build directory exists
    if not os.path.exists("build"):
        print("Creating build directory...")
        os.makedirs("build")
    
    # Configure CMake
    success, stdout, stderr = run_cmd("cd build && cmake .. -DCMAKE_BUILD_TYPE=Release")
    return success

def test_python_bindings():
    """Test if Python bindings can be built"""
    print("\n=== Testing Python Bindings Build ===")
    
    # Check if we can run the Python bindings build
    success, stdout, stderr = run_cmd("cd python && cmake . && make -j4")
    return success

def test_old_demos():
    """Test if old demos compile with new integrated systems"""
    print("\n=== Testing Old Demos ===")
    
    # Try to compile and run the old demos
    demos = [
        ("Phase 3 demo", "nlm_phase3_demo"),
        ("Phase 4 demo", "nlm_phase4_demo"),
        ("Phase 5 demo", "nlm_phase5_demo"),
    ]
    
    results = {}
    for name, exe in demos:
        print(f"\n--- Testing {name} ---")
        
        # Check if executable exists
        if os.path.exists(f"build/{exe}"):
            print(f"Executable found: build/{exe}")
            
            # Try to run with minimal args
            success, stdout, stderr = run_cmd(f"cd build && ./{exe} 1 10 2>&1 | head -50")
            results[name] = success
        else:
            print(f"Executable not found: build/{exe}")
            results[name] = False
    
    return results

def test_new_demos():
    """Test if new Phase 6 demos work"""
    print("\n=== Testing New Phase 6 Demos ===")
    
    # Check Phase 6 demo
    print("\n--- Testing Phase 6 Integration Demo ---")
    if os.path.exists("build/nlm_phase6_demo"):
        print("Phase 6 demo executable found")
        # Phase 6 demo has arguments
        success, stdout, stderr = run_cmd("cd build && ./nlm_phase6_demo 2>&1 | head -100")
        return success
    else:
        print("Phase 6 demo executable not found")
        return False

def main():
    print("NLM Phase 6 Integration Test")
    print("=" * 50)
    
    # Test CMake configuration
    cmake_ok = test_cmake_config()
    
    # Test Python bindings
    python_ok = test_python_bindings()
    
    # Test old demos
    old_demo_results = test_old_demos()
    
    # Test new demos
    new_demo_ok = test_new_demos()
    
    print("\n" + "=" * 50)
    print("SUMMARY:")
    print(f"CMake Configuration: {'✓ PASS' if cmake_ok else '✗ FAIL'}")
    print(f"Python Bindings: {'✓ PASS' if python_ok else '✗ FAIL'}")
    
    print("\nOld Demos:")
    for name, success in old_demo_results.items():
        print(f"  {name}: {'✓ PASS' if success else '✗ FAIL'}")
    
    print(f"\nNew Phase 6 Demo: {'✓ PASS' if new_demo_ok else '✗ FAIL'}")
    
    # Determine if there are issues to fix
    all_issues = not cmake_ok or not python_ok or not all(old_demo_results.values()) or not new_demo_ok
    
    if all_issues:
        print("\n⚠️  ISSUES FOUND - Code improvements needed!")
        return 1
    else:
        print("\n✅ All tests passed!")
        return 0

if __name__ == "__main__":
    sys.exit(main())
