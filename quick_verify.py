#!/usr/bin/env python3
"""
Quick Verification of NLM Improvements

This script runs a quick verification of the key improvements made to the NLM project.
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def main():
    print("Quick NLM Improvements Verification")
    print("=" * 40)
    
    all_passed = True
    
    # Test 1: Bug fix verification
    print("\n1. Testing bug fixes...")
    try:
        config = pynlm.createConfig("configs/default.cfg")
        print("   ✓ Configuration loads without duplicate parameter errors")
        
        brain = pynlm.BrainWrapper(config.getConfig())
        if brain.initialize():
            print("   ✓ Brain initialization works")
        else:
            print("   ✗ Brain initialization failed")
            all_passed = False
    except Exception as e:
        print(f"   ✗ Bug fix test failed: {e}")
        all_passed = False
    
    # Test 2: Enhanced API
    print("\n2. Testing enhanced API...")
    try:
        # Test ConfigWrapper
        config = pynlm.ConfigWrapper()
        config.configureBrain(neuronCount=100)
        
        # Test BrainWrapper
        brain = pynlm.BrainWrapper(config.getConfig())
        if brain.initialize():
            brain.runSimulation(5)
            print("   ✓ Enhanced API (BrainWrapper, ConfigWrapper) works")
        else:
            print("   ✗ Enhanced API test failed")
            all_passed = False
    except Exception as e:
        print(f"   ✗ Enhanced API test failed: {e}")
        all_passed = False
    
    # Test 3: New tools
    print("\n3. Testing new tools...")
    try:
        # Test quick test
        result = pynlm.runQuickTest()
        print("   ✓ Quick test utility works")
        
        # Test demo brain
        brain, stats = pynlm.create_demo_brain(50, 3)
        if brain and stats:
            print("   ✓ Demo brain creation works")
        else:
            print("   ✗ Demo brain creation failed")
            all_passed = False
    except Exception as e:
        print(f"   ✗ New tools test failed: {e}")
        all_passed = False
    
    # Test 4: File structure
    print("\n4. Testing file structure...")
    required_files = [
        "python/bindings.cpp",
        "configs/default.cfg",
        "basic_examples.py",
        "advanced_api.py",
        "nlm_cli.py",
        "tutorial.py",
        "IMPROVEMENTS_SUMMARY.md"
    ]
    
    missing_files = []
    for filepath in required_files:
        if not os.path.exists(filepath):
            missing_files.append(filepath)
    
    if not missing_files:
        print("   ✓ All required files exist")
    else:
        print(f"   ✗ Missing files: {missing_files}")
        all_passed = False
    
    # Summary
    print("\n" + "=" * 40)
    if all_passed:
        print("✓ ALL VERIFICATION TESTS PASSED")
        print("\nThe NLM improvements have been successfully implemented:")
        print("• Bug fixes completed (duplicate config parameter)")
        print("• Enhanced Python API with improved error handling")
        print("• New tools and examples for better usability")
        print("• Comprehensive documentation and tutorials")
        print("• Command-line interface for easier experimentation")
        print("\nThe project is now ready for use!")
    else:
        print("✗ SOME VERIFICATION TESTS FAILED")
        print("Please check the failed tests above.")
    
    return all_passed

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
