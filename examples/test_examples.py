#!/usr/bin/env python3
"""
Quick test to verify NLM examples work correctly.
"""

import sys
import os

# Add examples directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_quick_start():
    """Test the quick start example"""
    print("Testing quick start example...")
    try:
        import examples.quick_start
        print("✓ Quick start example imported successfully")
        return True
    except Exception as e:
        print(f"✗ Quick start example failed: {e}")
        return False

def test_getting_started():
    """Test the getting started tutorial"""
    print("Testing getting started tutorial...")
    try:
        import examples.getting_started
        print("✓ Getting started tutorial imported successfully")
        return True
    except Exception as e:
        print(f"✗ Getting started tutorial failed: {e}")
        return False

def test_comprehensive_examples():
    """Test the comprehensive examples"""
    print("Testing comprehensive examples...")
    try:
        import examples.comprehensive_examples
        print("✓ Comprehensive examples imported successfully")
        return True
    except Exception as e:
        print(f"✗ Comprehensive examples failed: {e}")
        return False

def main():
    print("=" * 60)
    print("NLM Examples Test Suite")
    print("=" * 60)
    
    # Run tests
    quick_start_ok = test_quick_start()
    getting_started_ok = test_getting_started()
    comprehensive_ok = test_comprehensive_examples()
    
    # Results
    print("\n" + "=" * 60)
    print("Test Results:")
    print("=" * 60)
    print(f"Quick start example: {'✓ PASS' if quick_start_ok else '✗ FAIL'}")
    print(f"Getting started tutorial: {'✓ PASS' if getting_started_ok else '✗ FAIL'}")
    print(f"Comprehensive examples: {'✓ PASS' if comprehensive_ok else '✗ FAIL'}")
    
    if quick_start_ok and getting_started_ok and comprehensive_ok:
        print("\n🎉 All tests passed!")
        print("\nYou can now run the examples with:")
        print("  python examples/quick_start.py")
        print("  python examples/getting_started.py")
        print("  python examples/comprehensive_examples.py")
        return 0
    else:
        print("\n❌ Some tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())