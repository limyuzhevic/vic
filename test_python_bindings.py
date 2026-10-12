#!/usr/bin/env python3
"""
Test script to identify issues with NLM Python bindings and demonstrate improvements needed.
"""

import pynlm
import time

def test_basic_functionality():
    """Test basic Python bindings functionality."""
    print("Testing basic NLM Python bindings...")
    
    # Test 1: Create and initialize brain
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Brain creation and initialization works")
        
        # Test basic stats
        print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
        
    except Exception as e:
        print(f"✗ Brain creation failed: {e}")
        return False
    
    # Test 2: Run simulation steps
    try:
        for i in range(10):
            brain.step(i)
        
        print("✓ Simulation steps work")
        print(f"  - Spike count after 10 steps: {brain.getTotalSpikeCount()}")
        
    except Exception as e:
        print(f"✗ Simulation steps failed: {e}")
        return False
    
    # Test 3: Test AgentBrain integration
    try:
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("✓ AgentBrain integration works")
        
        # Test with world interaction
        for i in range(5):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ World interaction loop works")
        
    except Exception as e:
        print(f"✗ AgentBrain integration failed: {e}")
        return False
    
    return True

def test_api_improvements_needed():
    """Test what API improvements are needed."""
    print("\nTesting API improvements needed...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test 1: Missing convenience methods
    print("Testing missing convenience methods...")
    
    # Try to access agent subsystems
    try:
        agent = pynlm.createAgentBrain(brain)
        
        # Test accessing subsystems (these might be missing or hard to access)
        if hasattr(agent, 'getCuriosityLevel'):
            print(f"✓ curiosity level accessible: {agent.getCuriosityLevel()}")
        else:
            print("✗ curiosity level not accessible")
            
        if hasattr(agent, 'getNoveltyLevel'):
            print(f"✓ novelty level accessible: {agent.getNoveltyLevel()}")
        else:
            print("✗ novelty level not accessible")
            
        if hasattr(agent, 'getPredictionError'):
            print(f"✓ prediction error accessible: {agent.getPredictionError()}")
        else:
            print("✗ prediction error not accessible")
            
    except Exception as e:
        print(f"✗ Subsystem access failed: {e}")
    
    # Test 2: Configuration improvements
    print("\nTesting configuration improvements...")
    
    try:
        # Test if we can get all config keys easily
        keys = config.getKeys()
        print(f"✓ Config keys accessible: {len(keys)} keys")
        
        # Try to get specific values
        if config.has("random_seed"):
            seed = config.get("random_seed")
            print(f"✓ Can get specific config values: {seed}")
        else:
            print("✗ Cannot get specific config values efficiently")
            
    except Exception as e:
        print(f"✗ Configuration access failed: {e}")
    
    # Test 3: Performance improvements
    print("\nTesting performance improvements...")
    
    try:
        # Test multiple brain operations in loop
        start_time = time.time()
        for i in range(100):
            brain.step(i)
        end_time = time.time()
        
        print(f"✓ 100 simulation steps took {end_time - start_time:.3f} seconds")
        
    except Exception as e:
        print(f"✗ Performance test failed: {e}")

def test_error_handling():
    """Test error handling improvements."""
    print("\nTesting error handling...")
    
    try:
        # Test 1: Invalid operations
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Try to access regions before initialization
        try:
            regions = brain.getRegions()
            print(f"✓ getRegions() returned {len(regions)} regions (expected 0)")
        except Exception as e:
            print(f"✓ getRegions() properly handled error: {e}")
            
        # Try invalid operations
        try:
            brain.step(-1)  # Negative step
            print("✗ Negative step should have failed but didn't")
        except Exception:
            print("✓ Negative step properly handled")
            
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")

if __name__ == "__main__":
    print("=== NLM Python Bindings Test ===")
    
    success = test_basic_functionality()
    test_api_improvements_needed()
    test_error_handling()
    
    if success:
        print("\n✓ Basic functionality works!")
        print("\nNext steps for improvements:")
        print("1. Add convenience methods for common operations")
        print("2. Improve configuration access patterns")
        print("3. Add better error handling and validation")
        print("4. Optimize performance for batch operations")
        print("5. Add Pythonic constructors and factory methods")
        print("6. Improve documentation and type hints")
    else:
        print("\n✗ Basic functionality has issues that need fixing")
