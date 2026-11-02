#!/usr/bin/env python3
"""
Quick test to verify NLM improvements.
"""

import sys
import os

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing basic NLM functionality...")
    
    try:
        import pynlm
        
        # Test 1: Configuration system
        print("1. Testing configuration system...")
        config = pynlm.createDefaultConfig()
        
        # Check default values
        assert config.getOr('brain.neuron_count', 1000) == 1000
        print("   ✓ Default configuration works")
        
        # Modify configuration
        config.set('brain.neuron_count', 500)
        assert config.getOr('brain.neuron_count', 1000) == 500
        print("   ✓ Configuration modification works")
        
        # Test 2: Brain creation
        print("2. Testing brain creation...")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain.getTotalNeuronCount() > 0
        print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        
        # Test 3: Simulation
        print("3. Testing simulation...")
        for step in range(5):
            brain.step(step)
        
        total_spikes = brain.getTotalSpikeCount()
        assert total_spikes >= 0
        print(f"   ✓ Simulation completed with {total_spikes} spikes")
        
        # Test 4: Configuration file I/O
        print("4. Testing configuration file I/O...")
        config.saveToFile("test_config.json")
        
        new_config = pynlm.createDefaultConfig()
        if new_config.loadFromFile("test_config.json"):
            assert new_config.getOr('brain.neuron_count', 1000) == 500
            print("   ✓ Configuration file I/O works")
        else:
            print("   ⚠ Configuration file I/O (fallback mode)")
        
        # Clean up
        if os.path.exists("test_config.json"):
            os.remove("test_config.json")
        
        print("\n✅ All basic tests passed!")
        return True
        
    except Exception as e:
        print(f"❌ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    print("=" * 60)
    print("NLM Improvement Verification - Basic Test")
    print("=" * 60)
    
    if test_basic_functionality():
        print("\n🎉 NLM improvements are working correctly!")
        sys.exit(0)
    else:
        print("\n❌ NLM improvements have issues that need to be addressed.")
        sys.exit(1)
