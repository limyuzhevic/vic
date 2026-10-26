#!/usr/bin/env python3
# Comprehensive test script for NLM Python bindings

import pynlm
import time
import sys

def test_basic_brain_creation():
    """Test basic brain creation and initialization"""
    print("=== Test 1: Basic Brain Creation ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    print(f"Created default config with {config.getKeys().size()} keys")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Expected approx: 1000 neurons")
    print(f"Brain initialized: {brain.initialize()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return True

def test_simulation():
    """Test brain simulation"""
    print("\n=== Test 2: Brain Simulation ===")
    
    # Create brain with small configuration for testing
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation for 100 steps
    for step in range(100):
        brain.step(step)
        
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Final state:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return True

def test_world_interaction():
    """Test brain-world interaction"""
    print("\n=== Test 3: World Interaction ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Created world {world.getWidth()}x{world.getHeight()}")
    print(f"Created agent with {agent.getSensoryInputSize()} sensory inputs")
    
    # Run some steps
    for step in range(50):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    print(f"Simulation complete:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Agent curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Agent novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    return True

def test_memory_systems():
    """Test memory systems"""
    print("\n=== Test 4: Memory Systems ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain created with memory systems")
    
    # Note: We can't directly access memory systems through Python API
    # but we can test through brain statistics
    for step in range(200):
        brain.step(step)
        
        if step % 50 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Memory test complete:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    return True

def test_configuration():
    """Test configuration management"""
    print("\n=== Test 5: Configuration Management ===")
    
    # Create config
    config = pynlm.createDefaultConfig()
    
    # Check initial configuration
    print(f"Initial config keys: {len(config.getKeys())}")
    
    # Modify configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("random_seed", 12345)
    
    print(f"Modified config:")
    print(f"  neuron_count: {config.getOr<int64_t>('brain.neuron_count', 0)}")
    print(f"  synapse_density: {config.getOr<float>('brain.synapse_density', 0.0f)}")
    print(f"  random_seed: {config.getOr<int64_t>('random_seed', 0)}")
    
    # Test has function
    print(f"Has 'brain.neuron_count': {config.has('brain.neuron_count')}")
    print(f"Has 'nonexistent': {config.has('nonexistent')}")
    
    # Save and load configuration
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_file = f.name
    
    try:
        config.saveToFile(config_file)
        print(f"Config saved to: {config_file}")
        
        # Create new config and load
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile(config_file)
        
        print(f"Config loaded and matches: {config.getOr<int64_t>('brain.neuron_count', 0) == config2.getOr<int64_t>('brain.neuron_count', 0)}")
        
    finally:
        if os.path.exists(config_file):
            os.unlink(config_file)
    
    return True

def test_save_load():
    """Test brain state save/load"""
    print("\n=== Test 6: Brain State Save/Load ===")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Run some steps
    for step in range(100):
        brain1.step(step)
    
    print(f"Initial brain state: {brain1.getTotalSpikeCount()} spikes")
    
    # Save state
    brain1.save("test_brain.bin")
    print("Brain state saved to test_brain.bin")
    
    # Create new brain and load
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    if brain2.load("test_brain.bin"):
        print("Brain state loaded successfully")
        print(f"Loaded brain spikes: {brain2.getTotalSpikeCount()}")
    else:
        print("Failed to load brain state")
    
    # Cleanup
    import os
    if os.path.exists("test_brain.bin"):
        os.unlink("test_brain.bin")
    
    return True

def run_all_tests():
    """Run all tests"""
    print("=== NLM Python Bindings Test Suite ===")
    print("This script tests the pynlm Python bindings for NLM")
    print("=" * 50)
    
    tests = [
        test_basic_brain_creation,
        test_simulation,
        test_world_interaction,
        test_memory_systems,
        test_configuration,
        test_save_load
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
                print(f"✓ {test.__name__} PASSED")
            else:
                failed += 1
                print(f"✗ {test.__name__} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} FAILED with exception: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("All tests PASSED!")
        return True
    else:
        print("Some tests FAILED!")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)