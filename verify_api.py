#!/usr/bin/env python3
"""
Quick verification test for NLM Python API improvements.
This script performs a simple smoke test to ensure the new API works.
"""

import sys

def test_api_availability():
    """Test that the new API functions are available."""
    print("Testing NLM Python API availability...")
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test new convenience functions
        print("\nTesting convenience functions...")
        
        # Test createQuickBrain
        brain = pynlm.createQuickBrain()
        print(f"✓ createQuickBrain() works - {brain.neuron_count} neurons")
        
        # Test configuration presets
        config1 = pynlm.createDefaultConfig()
        print(f"✓ createDefaultConfig() works")
        
        config2 = pynlm.createMinimalConfig()
        print(f"✓ createMinimalConfig() works")
        
        config3 = pynlm.createResearchConfig()
        print(f"✓ createResearchConfig() works")
        
        # Test property access
        brain.initialize()
        brain.step(0)
        
        print(f"✓ Property access works - {brain.neuron_count} neurons, {brain.firing_neuron_count} firing")
        
        # Test world setup
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        brain, agent = pynlm.createAgentSetup(world)
        print(f"✓ createAgentSetup() works")
        
        # Test episode function
        results = pynlm.run_episode(brain, agent, world, num_steps=5, verbose=False)
        avg_reward, max_firing = results
        print(f"✓ run_episode() works - avg reward: {avg_reward:.3f}, max firing: {max_firing}")
        
        print("\n🎉 All API tests passed successfully!")
        return True
        
    except Exception as e:
        print(f"\n❌ API test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    print("=" * 60)
    print("NLM Python API - Quick Verification Test")
    print("=" * 60)
    
    success = test_api_availability()
    
    if success:
        print("\n✅ The NLM Python API improvements are working correctly!")
        sys.exit(0)
    else:
        print("\n❌ The NLM Python API improvements have issues that need to be addressed.")
        sys.exit(1)
