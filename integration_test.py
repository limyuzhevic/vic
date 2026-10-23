#!/usr/bin/env python3
"""
Simple Integration Test for NLM Improvements

This script tests the basic functionality of the NLM improvements
to ensure everything is working correctly.
"""

import sys
import os

def test_basic_imports():
    """Test basic imports work."""
    print("Testing basic imports...")
    try:
        import pynlm
        print("✓ pynlm imported successfully")
        
        # Test basic functionality
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("✓ Basic brain creation and initialization works")
        return True
    except Exception as e:
        print(f"✗ Basic import test failed: {e}")
        return False

def test_advanced_functions():
    """Test advanced convenience functions."""
    print("\nTesting advanced convenience functions...")
    try:
        from pynlm_advanced import create_advanced_brain
        
        brain, config = create_advanced_brain(
            neuron_count=50,
            enable_learning=False,
            enable_memory=False
        )
        
        print("✓ create_advanced_brain works")
        
        # Test agent-world creation
        from pynlm_advanced import create_agent_world
        brain2, agent2, world2 = create_agent_world(
            neuron_count=30,
            world_width=10,
            world_height=10
        )
        
        print("✓ create_agent_world works")
        
        return True
    except Exception as e:
        print(f"✗ Advanced functions test failed: {e}")
        return False

def test_cli_import():
    """Test CLI import."""
    print("\nTesting CLI import...")
    try:
        import nlm_cli
        print("✓ nlm_cli imported successfully")
        
        # Check main function exists
        if hasattr(nlm_cli, 'main') and callable(nlm_cli.main):
            print("✓ CLI main function exists")
        else:
            print("✗ CLI main function missing")
            return False
            
        return True
    except Exception as e:
        print(f"✗ CLI import test failed: {e}")
        return False

def test_examples_import():
    """Test examples import."""
    print("\nTesting examples import...")
    try:
        import nlm_examples
        print("✓ nlm_examples imported successfully")
        return True
    except Exception as e:
        print(f"✗ Examples import test failed: {e}")
        return False

def test_pyproject():
    """Test pyproject.toml exists."""
    print("\nTesting pyproject.toml...")
    try:
        if os.path.exists("pyproject.toml"):
            print("✓ pyproject.toml exists")
            
            # Check it's not empty
            with open("pyproject.toml", "r") as f:
                content = f.read()
                if len(content) > 100:
                    print("✓ pyproject.toml has content")
                else:
                    print("⚠ pyproject.toml seems empty")
            return True
        else:
            print("✗ pyproject.toml does not exist")
            return False
    except Exception as e:
        print(f"✗ pyproject.toml test failed: {e}")
        return False

def main():
    """Run integration test."""
    print("NLM Improvements - Integration Test")
    print("=" * 50)
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Advanced Functions", test_advanced_functions),
        ("CLI Import", test_cli_import),
        ("Examples Import", test_examples_import),
        ("pyproject.toml", test_pyproject),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                passed += 1
                print(f"✅ {test_name} PASSED")
            else:
                print(f"❌ {test_name} FAILED")
        except Exception as e:
            print(f"❌ {test_name} ERROR: {e}")
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All integration tests passed!")
        print("\nThe NLM improvements are working correctly.")
        print("\nNext steps:")
        print("1. Run full test suite: pytest")
        print("2. Run examples: python nlm_examples.py")
        print("3. Use CLI: nlm-cli --help")
        print("4. View documentation: IMPROVEMENTS_SUMMARY.md")
        return 0
    else:
        print(f"\n⚠️  {total - passed} test(s) failed.")
        print("Please check the output above for details.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
