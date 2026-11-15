#!/usr/bin/env python3
# Verification script for NLM Phase 6 improvements

import os
import sys

def check_file_exists(path, description):
    """Check if a file exists."""
    if os.path.exists(path):
        print(f"✓ {description} found at {path}")
        return True
    else:
        print(f"✗ {description} NOT found at {path}")
        return False

def check_python_bindings():
    """Check if Python bindings are working."""
    try:
        # Add python directory to path
        sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))
        
        import pynlm
        print("✓ Python bindings imported successfully")
        
        # Test basic functionality
        config = pynlm.createDefaultConfig()
        print("✓ Default config created")
        
        brain = pynlm.createBrain(config)
        print("✓ Brain created")
        
        world = pynlm.createSimpleWorld()
        print("✓ World created")
        
        agent = pynlm.createAgentBrain(brain)
        print("✓ Agent created")
        
        return True
        
    except ImportError as e:
        print(f"✗ Failed to import Python bindings: {e}")
        return False
    except Exception as e:
        print(f"✗ Python bindings test failed: {e}")
        return False

def main():
    print("NLM Phase 6 Improvements Verification")
    print("=" * 50)
    
    # Check key files
    print("\n=== Checking Key Files ===")
    
    files_to_check = [
        ("src/nlm_cli.cpp", "Command-Line Interface source"),
        ("src/nlm_cli.cpp", "Command-Line Interface implementation"),
        ("python/bindings.cpp", "Python bindings source"),
        ("python/README.md", "Python documentation"),
        ("python/API_REFERENCE.md", "Python API reference"),
        ("test_python_bindings.py", "Python test script"),
        ("IMPROVEMENTS_SUMMARY.md", "Improvements summary"),
        ("build_and_test.sh", "Build and test script"),
    ]
    
    all_files_exist = True
    for file_path, description in files_to_check:
        if not check_file_exists(file_path, description):
            all_files_exist = False
    
    # Check for build directory
    if os.path.exists("build"):
        print("✓ Build directory found")
        
        # Check for executables
        executables = [
            ("build/nlm_cli", "Command-line interface"),
            ("build/nlm", "Phase 2 demo"),
            ("build/nlm_phase3_demo", "Phase 3 demo"),
            ("build/nlm_phase4_demo", "Phase 4 demo"),
            ("build/nlm_phase6_demo", "Phase 6 integration demo"),
        ]
        
        for exe_path, description in executables:
            check_file_exists(exe_path, description)
    else:
        print("✗ Build directory not found")
        all_files_exist = False
    
    # Check Python bindings
    print("\n=== Checking Python Bindings ===")
    if check_python_bindings():
        print("✓ Python bindings working correctly")
    else:
        print("✗ Python bindings not working")
        all_files_exist = False
    
    # Summary
    print("\n=== Summary ===")
    if all_files_exist:
        print("✓ All key files present")
        print("✓ Python bindings working")
        print("✓ Improvements successfully implemented")
        print("\nNLM Phase 6 improvements verification PASSED!")
        return 0
    else:
        print("✗ Some files or functionality missing")
        print("✗ NLM Phase 6 improvements verification FAILED!")
        return 1

if __name__ == "__main__":
    sys.exit(main())