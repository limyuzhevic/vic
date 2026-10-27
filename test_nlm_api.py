#!/usr/bin/env python3
"""
Test suite to verify NLM Python API functionality.
This script tests the core functionality of the NLM Python bindings.
"""

import sys
import os

# Add project root to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

def test_imports():
    """Test that pynlm module imports correctly"""
    print("Testing pynlm module import...")
    try:
        import pynlm
        print(f"✓ Successfully imported pynlm version {pynlm.__version__}")
        
        # Test basic functions
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        print("✓ Basic API functions work")
        return True
        
    except Exception as e:
        print(f"✗ Import test failed: {e}")
        return False

def test_brain_creation():
    """Test brain creation and basic operations"""
    print("\nTesting brain creation and operations...")
    try:
        import pynlm
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Initialize
        if not brain.initialize():
            print("✗ Brain initialization failed")
            return False
        
        print("✓ Brain created and initialized")
        
        # Basic operations
        for i in range(10):
            brain.step(i)
        
        print(f"✓ Brain executed {10} steps")
        
        # Check statistics
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        firing_count = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        
        print(f"  Statistics: {total_neurons} neurons, {total_synapses} synapses")
        print(f"  Current state: {firing_count} firing, {total_spikes} total spikes")
        
        # Check developmental stage
        stage = brain.getDevelopmentalStage()
        print(f"  Developmental stage: {stage}")
        
        return True
        
    except Exception as e:
        print(f"✗ Brain creation test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_world_simulation():
    """Test world simulation with agent"""
    print("\nTesting world simulation with agent...")
    try:
        import pynlm
        
        # Create brain and world
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("✓ World and agent created")
        
        # Run simulation
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"✓ Simulation completed {20} steps")
        
        # Check agent state
        pos = world.getAgentBody()
        print(f"  Agent position: ({pos.x:.1f}, {pos.y:.1f})")
        print(f"  Agent moving: {pos.isMoving}")
        
        return True
        
    except Exception as e:
        print(f"✗ World simulation test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_learning_systems():
    """Test learning systems"""
    print("\nTesting learning systems...")
    try:
        import pynlm
        
        # Setup with learning
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print("✓ Learning systems enabled")
        
        # Run with learning
        for step in range(30):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(result.reward, 0.0)
            agent.updateDevelopment(0.1)
        
        print(f"✓ Learning simulation completed {30} steps")
        
        # Check learning metrics
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        dev_stage = brain.getDevelopmentalStage()
        
        print(f"  Curiosity: {curiosity:.3f}")
        print(f"  Novelty: {novelty:.3f}")
        print(f"  Development stage: {dev_stage}")
        
        return True
        
    except Exception as e:
        print(f"✗ Learning systems test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_config_management():
    """Test configuration management"""
    print("\nTesting configuration management...")
    try:
        import pynlm
        
        # Create default config
        config = pynlm.createDefaultConfig()
        
        # Check default values
        neuron_count = config.getOr('brain.neuron_count', 1000)
        print(f"✓ Default neuron count: {neuron_count}")
        
        # Check if key exists
        has_key = config.has('brain.synapse_density')
        print(f"✓ Has 'brain.synapse_density' key: {has_key}")
        
        # Get all keys
        keys = config.getKeys()
        print(f"✓ Config has {len(keys)} keys: {keys}")
        
        # Modify config
        config.set('brain.neuron_count', 2000)
        new_count = config.getOr('brain.neuron_count', 1000)
        print(f"✓ Modified neuron count: {new_count}")
        
        return True
        
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    print("=" * 70)
    print("NLM Python API - Test Suite")
    print("=" * 70)
    
    # Run all tests
    tests = [
        ("Module Import", test_imports),
        ("Brain Creation", test_brain_creation),
        ("World Simulation", test_world_simulation),
        ("Learning Systems", test_learning_systems),
        ("Configuration Management", test_config_management),
    ]
    
    results = []
    for test_name, test_func in tests:
        result = test_func()
        results.append((test_name, result))
    
    # Summary
    print("\n" + "=" * 70)
    print("TEST SUMMARY")
    print("=" * 70)
    
    passed = 0
    for test_name, result in results:
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"{test_name:.<40} {status}")
        if result:
            passed += 1
    
    print(f"\nOverall: {passed}/{len(tests)} tests passed")
    
    if passed == len(tests):
        print("\n🎉 All tests passed!")
        print("The NLM Python API is working correctly.")
        return 0
    else:
        print(f"\n❌ {len(tests) - passed} tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())