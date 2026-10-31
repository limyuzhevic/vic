#!/usr/bin/env python3
"""
Test script to verify NLM Python bindings are working.
This script tests the basic functionality documented in easy_usage.md.
"""

import sys

def test_basic_imports():
    """Test that all basic imports work."""
    print("Testing basic imports...")
    
    try:
        import pynlm
        print(f"✅ pynlm imported successfully, version: {getattr(pynlm, '__version__', 'unknown')}")
    except ImportError as e:
        print(f"❌ Failed to import pynlm: {e}")
        return False
    
    return True

def test_create_default_config():
    """Test creating a default configuration."""
    print("\nTesting createDefaultConfig()...")
    
    try:
        import pynlm
        config = pynlm.createDefaultConfig()
        print(f"✅ Default config created successfully")
        return True
    except Exception as e:
        print(f"❌ Failed to create default config: {e}")
        return False

def test_create_brain():
    """Test creating a brain from config."""
    print("\nTesting createBrain()...")
    
    try:
        import pynlm
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        print(f"✅ Brain created successfully")
        
        # Test brain methods
        brain.initialize()
        print(f"✅ Brain initialized, total neurons: {brain.getTotalNeuronCount()}")
        
        return True
    except Exception as e:
        print(f"❌ Failed to create brain: {e}")
        return False

def test_simple_simulation():
    """Test running a simple brain simulation."""
    print("\nTesting brain step()...")
    
    try:
        import pynlm
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run a few steps
        for step in range(10):
            brain.step(step)
        
        print(f"✅ Brain simulation completed")
        print(f"   Total spikes: {brain.getTotalSpikeCount()}")
        print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
        
        return True
    except Exception as e:
        print(f"❌ Failed to run simulation: {e}")
        return False

def test_create_world():
    """Test creating a simple world."""
    print("\nTesting createSimpleWorld()...")
    
    try:
        import pynlm
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        print(f"✅ World created and configured successfully")
        print(f"   World size: {world.getWidth()}x{world.getHeight()}")
        print(f"   Vision size: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
        return True
    except Exception as e:
        print(f"❌ Failed to create world: {e}")
        return False

def test_create_agent():
    """Test creating an agent brain."""
    print("\nTesting createAgentBrain()...")
    
    try:
        import pynlm
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        print(f"✅ Agent created successfully")
        print(f"   Agent has brain with {brain.getTotalNeuronCount()} neurons")
        return True
    except Exception as e:
        print(f"❌ Failed to create agent: {e}")
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_imports,
        test_create_default_config,
        test_create_brain,
        test_simple_simulation,
        test_create_world,
        test_create_agent,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"❌ Test {test.__name__} crashed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed! Python bindings are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())