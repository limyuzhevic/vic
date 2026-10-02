#!/usr/bin/env python3
"""
Quick Python Test for Enhanced NLM Bindings

This script tests the enhanced Python bindings by importing and using them.
Run with: python test_simple.py
"""

import sys
import os

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_imports():
    """Test if pynlm can be imported and has enhanced features"""
    print("Testing NLM Python Bindings...")
    
    try:
        import pynlm
        print(f"✓ pynlm imported successfully")
        print(f"  Version: {getattr(pynlm, '__version__', 'unknown')}")
        print(f"  Description: {getattr(pynlm, '__description__', 'unknown')}")
        print(f"  Author: {getattr(pynlm, '__author__', 'unknown')}")
        return True
        
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error importing pynlm: {e}")
        return False

def test_basic_functionality():
    """Test basic enhanced functionality"""
    print("\nTesting Basic Enhanced Functionality...")
    
    try:
        import pynlm
        
        # Test configuration
        config = pynlm.createDefaultConfig()
        print(f"✓ Config created with {len(config.getKeys())} keys")
        
        # Test Pythonic access
        config["test.key"] = "value"
        value = config.get_string("test.key", "default")
        print(f"✓ Pythonic config access works: {value}")
        
        # Test dictionary update
        updates = {"key1": "val1", "key2": 123}
        config.update(updates)
        print(f"✓ Dictionary updates work")
        
        # Test save_dict
        config_dict = config.save_dict()
        print(f"✓ save_dict works: {len(config_dict)} items")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_brain_features():
    """Test enhanced brain features"""
    print("\nTesting Enhanced Brain Features...")
    
    try:
        import pynlm
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 50)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        
        # Test simulate()
        brain.simulate(steps=10, verbose=False)
        print(f"✓ simulate() works: {brain.getTotalSpikeCount()} spikes")
        
        # Test get_statistics()
        stats = brain.get_statistics()
        print(f"✓ get_statistics() works: {len(stats)} keys")
        
        # Test __repr__
        repr_str = repr(brain)
        print(f"✓ __repr__ works: {repr_str[:50]}...")
        
        # Test visualize_brain()
        brain.visualize_brain()
        print(f"✓ visualize_brain() works")
        
        # Test context manager
        with pynlm.BrainContext(pynlm.createBrain(config)) as test_brain:
            test_brain.initialize()
            test_brain.simulate(steps=5)
            print(f"✓ Context manager works")
        
        return True
        
    except Exception as e:
        print(f"✗ Brain features test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_agent_features():
    """Test enhanced agent features"""
    print("\nTesting Enhanced Agent Features...")
    
    try:
        import pynlm
        
        # Create agent components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        agent.initialize(world)
        print(f"✓ Agent initialized")
        
        # Test setup_learning()
        agent.setup_learning(reward=True, curiosity=True)
        print(f"✓ setup_learning() works")
        
        # Test get_agent_stats()
        stats = agent.get_agent_stats()
        print(f"✓ get_agent_stats() works: curiosity={stats['curiosity_level']:.3f}")
        
        # Test __repr__
        repr_str = repr(agent)
        print(f"✓ Agent __repr__ works: {repr_str[:50]}...")
        
        return True
        
    except Exception as e:
        print(f"✗ Agent features test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling"""
    print("\nTesting Error Handling...")
    
    try:
        import pynlm
        
        config = pynlm.createDefaultConfig()
        
        # Test safe access
        value = config.get_string("nonexistent", "default")
        print(f"✓ Safe access with default: {value}")
        
        # Test KeyError
        try:
            bad_value = config["nonexistent"]
            print(f"✗ Should have raised KeyError")
            return False
        except KeyError:
            print(f"✓ KeyError raised as expected")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("Enhanced NLM Python Bindings - Quick Test Suite")
    print("=" * 50)
    
    tests = [
        ("Import Test", test_imports),
        ("Basic Functionality", test_basic_functionality),
        ("Brain Features", test_brain_features),
        ("Agent Features", test_agent_features),
        ("Error Handling", test_error_handling),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        if test_func():
            passed += 1
            print(f"✓ {test_name} PASSED")
        else:
            print(f"✗ {test_name} FAILED")
    
    print("\n" + "=" * 50)
    print(f"Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed! Enhanced Python bindings are working correctly.")
        return 0
    else:
        print(f"\n❌ {total - passed} tests failed.")
        return 1

if __name__ == "__main__":
    exit(main())
