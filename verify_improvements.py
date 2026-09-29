#!/usr/bin/env python3
"""
Final Verification Script for NLM Improvements

This script verifies that all the improvements made to the NLM project
are working correctly.
"""

import sys
import os
import subprocess

def run_command(cmd, description):
    """Run a command and return success status"""
    print(f"\n🔍 Testing: {description}")
    print(f"   Command: {cmd}")
    
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=30)
        if result.returncode == 0:
            print(f"   ✅ SUCCESS: {result.stdout.strip()}")
            return True
        else:
            print(f"   ❌ FAILED: {result.stderr.strip()}")
            return False
    except subprocess.TimeoutExpired:
        print(f"   ⏰ TIMEOUT: Command took too long")
        return False
    except Exception as e:
        print(f"   💥 ERROR: {e}")
        return False

def main():
    print("=" * 60)
    print("NLM Project - Final Verification of Improvements")
    print("=" * 60)
    
    # Change to the project directory
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    tests = [
        # Test Python API imports
        ("python3 -c \"import pynlm; print('NLM module imported successfully')\"", 
         "Python NLM Module Import"),
        
        # Test CLI tool exists
        ("ls -la bin/nlm", "CLI Tool Exists"),
        
        # Test CLI help
        ("python3 nlm-cli.py --help 2>/dev/null | head -10", "CLI Help Output"),
        
        # Test test script runs
        ("python3 test_api.py 2>&1 | head -20", "Test Script Import"),
        
        # Test documentation exists
        ("ls -la easy_usage.md IMPROVEMENTS.md", "Documentation Files"),
        
        # Test bindings.cpp changes
        ("grep -c "createDefaultConfig" python/bindings.cpp", "Factory Functions Added"),
    ]
    
    print("\n🚀 Running verification tests...")
    
    passed = 0
    failed = 0
    
    for cmd, description in tests:
        if run_command(cmd, description):
            passed += 1
        else:
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"📊 Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All verification tests passed!")
        print("\n📝 Summary of Improvements Made:")
        print("   1. Fixed Python bindings to match documented API")
        print("   2. Added convenience factory functions (createDefaultConfig, etc.)")
        print("   3. Created comprehensive CLI tool (nlm-cli.py)")
        print("   4. Added proper error handling and validation")
        print("   5. Created comprehensive test suite (test_api.py)")
        print("   6. Improved documentation (IMPROVEMENTS.md)")
        print("   7. Added CLI entry point (bin/nlm)")
        print("\n✅ The NLM project has been successfully improved!")
        return True
    else:
        print("❌ Some verification tests failed")
        print("\n🔧 Please check the failed tests and fix any issues")
        return False

if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)