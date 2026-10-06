#!/usr/bin/env python3
"""
Simple test for NLM Python bindings improvements
"""

import pynlm
import sys

def test_basic_improvements():
    print("Testing NLM Python bindings improvements...")
    
    # Test 1: Enhanced module documentation
    print("1. Testing enhanced module documentation...")
    module_doc = pynlm.__doc__
    if module_doc and "Quick Start" in module_doc:
        print("   ✓ Module documentation includes examples")
    else:
        print("   ✗ Module documentation not enhanced")
    
    # Test 2: Enhanced class documentation
    print("2. Testing enhanced class documentation...")
    brain_doc = pynlm.Brain.__doc__
    if brain_doc and "neural simulation" in brain_doc.lower():
        print("   ✓ Brain class documentation enhanced")
    else:
        print("   ✗ Brain class documentation not enhanced")
    
    config_doc = pynlm.Config.__doc__
    if config_doc and "configuration" in config_doc.lower():
        print("   ✓ Config class documentation enhanced")
    else:
        print("   ✗ Config class documentation not enhanced")
    
    # Test 3: Helper functions
    print("3. Testing helper functions...")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("   ✓ Helper functions working")
    except Exception as e:
        print(f"   ✗ Helper functions failed: {e}")
    
    # Test 4: Enhanced API
    print("4. Testing enhanced API...")
    try:
        # Test create_brain_safely
        brain2, neurons = pynlm.create_brain_safely(config, "test")
        print("   ✓ create_brain_safely working")
        
        # Test create_world_safely  
        world = pynlm.create_world_safely(20.0, 20.0, 8, 8)
        print("   ✓ create_world_safely working")
        
    except Exception as e:
        print(f"   ✗ Enhanced API failed: {e}")
    
    # Test 5: Error handling
    print("5. Testing error handling...")
    try:
        errors = pynlm.validate_parameters(-1.0, 20.0, 8, 8, 0.1, 100)
        if errors:
            print(f"   ✓ Parameter validation working: {len(errors)} errors")
        else:
            print("   ✗ Parameter validation not working")
    except Exception as e:
        print(f"   ✗ Error handling test failed: {e}")
    
    # Test 6: Convenience methods
    print("6. Testing convenience methods...")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10.0, 10.0, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        result = pynlm.run_simulation(brain, world, agent, 5, 0.1)
        print("   ✓ run_simulation convenience method working")
        
    except Exception as e:
        print(f"   ✗ Convenience methods test failed: {e}")
    
    # Test 7: Scientific integration
    print("7. Testing scientific integration...")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(15.0, 15.0, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        viz_data = pynlm.export_visualization_data(brain, world)
        analysis = pynlm.analyze_brain_dynamics(brain, 5.0)
        scientific_data = pynlm.export_for_scientific_analysis(brain, world)
        
        print("   ✓ Scientific integration methods working")
        print(f"     - Visualization data: {len(viz_data)} categories")
        print(f"     - Analysis metrics: {len(analysis)}")
        print(f"     - Scientific data: {len(scientific_data)} categories")
        
    except Exception as e:
        print(f"   ✗ Scientific integration test failed: {e}")
    
    print("\nTest completed!")

if __name__ == "__main__":
    test_basic_improvements()
