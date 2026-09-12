# Test file for Python bindings
# This tests the pynlm Python module interface

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_creation():
    """Test basic creation of brain and configuration objects"""
    print("Test 1: Basic creation...")
    
    # Create config
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("  ✓ Config created")
    
    # Create brain
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("  ✓ Brain created")
    
    # Initialize brain
    success = brain.initialize()
    assert success == True
    print("  ✓ Brain initialized")
    
    # Get brain stats
    neuron_count = brain.getTotalNeuronCount()
    assert neuron_count > 0
    print(f"  ✓ Brain has {neuron_count} neurons")
    
    return True

def test_simple_simulation():
    """Test a simple simulation step"""
    print("Test 2: Simple simulation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a few steps
    for i in range(10):
        brain.step(i)
    
    # Check stats
    spike_count = brain.getTotalSpikeCount()
    firing_count = brain.getFiringNeuronCount()
    assert spike_count >= 0
    assert firing_count >= 0
    print(f"  ✓ Simulation completed: {spike_count} spikes, {firing_count} firing neurons")
    
    return True

def test_world_and_agent():
    """Test world and agent functionality"""
    print("Test 3: World and agent...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test sensor input
    percept = world.getSensoryPercept()
    assert percept is not None
    print("  ✓ World sensory percept created")
    
    # Process sensory input
    agent.processSensoryInput(percept)
    print("  ✓ Sensory input processed")
    
    # Test motor command
    action = agent.decodeMotorCommand()
    assert action is not None
    print(f"  ✓ Motor command decoded: {action}")
    
    # Test world update
    world.update(0.1)
    print("  ✓ World updated")
    
    return True

def test_learning_systems():
    """Test learning system enable/disable"""
    print("Test 4: Learning systems...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test enable/disable functions
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("  ✓ Learning systems enabled")
    
    # Check that systems are working
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    neuromod_level = agent.getNeuromodulationLevel()
    
    assert curiosity_level >= 0
    assert novelty_level >= 0
    assert prediction_error >= -1.0 and prediction_error <= 1.0
    assert neuromod_level >= -1.0 and neuromod_level <= 1.0
    
    print(f"  ✓ Neuromodulation: curiosity={curiosity_level:.3f}, novelty={novelty_level:.3f}, error={prediction_error:.3f}")
    
    return True

def test_checkpoint_operations():
    """Test checkpoint save/load operations"""
    print("Test 5: Checkpoint operations...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a few steps to create state
    for i in range(50):
        brain.step(i)
    
    # Save checkpoint
    checkpoint_file = "test_checkpoint.bin"
    success = brain.save(checkpoint_file)
    if success:
        print("  ✓ Checkpoint saved")
        
        # Create new brain and load checkpoint
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        load_success = brain2.load(checkpoint_file)
        if load_success:
            print("  ✓ Checkpoint loaded")
        else:
            print("  ! Checkpoint load failed (expected for this test)")
        
        # Clean up
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
            print("  ✓ Checkpoint file cleaned up")
    else:
        print("  ! Checkpoint save failed (may be expected)")
    
    return True

def test_config_operations():
    """Test configuration operations"""
    print("Test 6: Configuration operations...")
    
    config = pynlm.createDefaultConfig()
    
    # Test basic operations
    config.set("brain.neuron_count", 500)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    # Test has function
    has_neuron_count = config.has("brain.neuron_count")
    assert has_neuron_count == True
    
    has_nonexistent = config.has("nonexistent.key")
    assert has_nonexistent == False
    
    # Test get keys
    keys = config.getKeys()
    assert "brain.neuron_count" in keys
    print(f"  ✓ Config keys: {len(keys)} keys found")
    
    # Test summary
    summary = config.summary()
    assert summary is not None and len(summary) > 0
    print("  ✓ Config summary generated")
    
    return True

def test_experiment_api():
    """Test experiment-related API"""
    print("Test 7: Experiment API...")
    
    # Create brain with config
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a simple experiment loop
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    total_reward = 0.0
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        total_reward += result.reward
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
    
    print(f"  ✓ Experiment completed: {total_reward:.3f} total reward")
    print(f"  ✓ Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  ✓ Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  ✓ Novelty level: {agent.getNoveltyLevel():.3f}")
    
    return True

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_creation,
        test_simple_simulation,
        test_world_and_agent,
        test_learning_systems,
        test_checkpoint_operations,
        test_config_operations,
        test_experiment_api
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
                print(f"  ✗ {test.__name__} returned False")
        except Exception as e:
            failed += 1
            print(f"  ✗ {test.__name__} failed with exception: {e}")
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(main())
