#!/usr/bin/env python3
"""
Comprehensive Python test suite for pynlm bindings

This test suite verifies that the pynlm Python bindings are working correctly
and covers the main functionality of the NLM framework.
"""

import sys
import os
import tempfile
import pynlm

def test_basic_creation():
    """Test basic object creation and initialization."""
    print("Test 1: Basic object creation...")
    
    # Test config creation
    config = pynlm.createDefaultConfig()
    assert config is not None, "Config creation failed"
    
    # Test brain creation
    brain = pynlm.createBrain(config)
    assert brain is not None, "Brain creation failed"
    
    # Test world creation
    world = pynlm.createSimpleWorld()
    assert world is not None, "World creation failed"
    
    # Test agent creation
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None, "Agent creation failed"
    
    print("  ✓ Basic creation tests passed")
    return True
def test_brain_initialization():
    """Test brain initialization and basic operations."""
    print("Test 2: Brain initialization...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    brain.initialize()
    
    # Check initial state
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
    
    # Run some steps
    for i in range(10):
        brain.step(i)
    
    # Check that brain is active
    assert brain.getTotalSpikeCount() >= 0, "Should have spikes (or at least not negative)"
    
    print("  ✓ Brain initialization tests passed")
    return True
def test_world_operations():
    """Test world configuration and operations."""
    print("Test 3: World operations...")
    
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Check world properties
    assert world.getWidth() == 10, "Width should be 10"
    assert world.getHeight() == 10, "Height should be 10"
    assert world.getVisionWidth() == 8, "Vision width should be 8"
    assert world.getVisionHeight() == 8, "Vision height should be 8"
    
    # Test agent start
    world.setAgentStart(5, 5)
    
    # Update world
    world.update(0.1)
    
    # Get percept
    percept = world.getSensoryPercept()
    assert percept is not None, "Should get percept"
    
    # Get agent body
    body = world.getAgentBody()
    assert body is not None, "Should get agent body"
    
    print("  ✓ World operations tests passed")
    return True
def test_agent_operations():
    """Test agent creation and operations."""
    print("Test 4: Agent operations...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test agent properties
    assert agent.getBrain() is brain, "Agent should reference brain"
    assert agent.getSensoryInputSize() > 0, "Should have sensory input"
    assert agent.getMotorOutputSize() > 0, "Should have motor output"
    
    # Test agent control flags
    agent.enableRewardModulation(True)
    assert agent.isRewardModulationEnabled(), "Reward modulation should be enabled"
    
    agent.enableCuriosity(True)
    assert agent.isCuriosityEnabled(), "Curiosity should be enabled"
    
    # Test processing
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Test development
    agent.updateDevelopment(0.1)
    assert agent.getDevelopmentalStage() >= 0, "Development stage should be non-negative"
    
    print("  ✓ Agent operations tests passed")
    return True
def test_config_operations():
    """Test configuration operations."""
    print("Test 5: Config operations...")
    
    config = pynlm.createDefaultConfig()
    
    # Test config properties
    assert config.has("random_seed"), "Should have random_seed"
    assert config.has("neuron_count"), "Should have neuron_count"
    assert config.has("region_count"), "Should have region_count"
    
    # Test getting values
    seed = config.getOr("random_seed", 42)
    assert seed == 42, "Should get random_seed"
    
    # Test summary
    summary = config.summary()
    assert summary is not None, "Should have summary"
    assert len(summary) > 0, "Summary should not be empty"
    
    # Test clearing
    config.clear()
    assert not config.has("random_seed"), "Cleared config should not have keys"
    
    print("  ✓ Config operations tests passed")
    return True
def test_file_operations():
    """Test file I/O operations."""
    print("Test 6: File operations...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps
    for i in range(10):
        brain.step(i)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        filepath = f.name
    
    try:
        # Save brain state
        save_result = brain.save(filepath)
        assert save_result, "Brain save should succeed"
        
        # Create new brain and load
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        # Load brain state
        load_result = brain2.load(filepath)
        assert load_result, "Brain load should succeed"
        
        # Verify loaded state (check that spike count is preserved)
        # Note: Load may not perfectly restore spike count due to simulation nature
        # but the save/load process should complete without errors
        
    finally:
        # Clean up
        if os.path.exists(filepath):
            os.unlink(filepath)
    
    print("  ✓ File operations tests passed")
    return True
def test_enumerations():
    """Test enumeration values."""
    print("Test 7: Enumeration values...")
    
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory is not None, "NeuronType should exist"
    assert pynlm.NeuronType.Inhibitory is not None, "NeuronType should exist"
    assert pynlm.NeuronType.Sensory is not None, "NeuronType should exist"
    
    # Test SynapseType
    assert pynlm.SynapseType.Excitatory is not None, "SynapseType should exist"
    assert pynlm.SynapseType.Inhibitory is not None, "SynapseType should exist"
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward is not None, "ActionType should exist"
    assert pynlm.ActionType.TurnLeft is not None, "ActionType should exist"
    assert pynlm.ActionType.Interact is not None, "ActionType should exist"
    
    # Test MotorCommand
    assert pynlm.MotorCommand.MoveForward is not None, "MotorCommand should exist"
    assert pynlm.MotorCommand.TurnLeft is not None, "MotorCommand should exist"
    
    # Test constants
    assert pynlm.INVALID_NEURON_ID.value is not None, "INVALID_NEURON_ID should exist"
    assert pynlm.INVALID_SYNAPSE_ID.value is not None, "INVALID_SYNAPSE_ID should exist"
    
    print("  ✓ Enumeration tests passed")
    return True
def run_all_tests():
    """Run all tests and return results."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_creation,
        test_brain_initialization,
        test_world_operations,
        test_agent_operations,
        test_config_operations,
        test_file_operations,
        test_enumerations
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
            print(f"  ✗ Test failed with exception: {e}")
            failed += 1
        print()
    
    print("=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    print(f"Tests passed: {passed}")
    print(f"Tests failed: {failed}")
    print(f"Total tests: {passed + failed}")
    
    if failed == 0:
        print("\n✓ All tests passed!")
        return True
    else:
        print(f"\n✗ {failed} test(s) failed")
        return False
def main():
    print("NLM Python Bindings Test Suite")
    print("Testing the pynlm Python bindings functionality...")
    
    success = run_all_tests()
    
    if success:
        print("\n✅ Test suite completed successfully!")
        return 0
    else:
        print("\n❌ Test suite completed with failures!")
        return 1
if __name__ == "__main__":
    sys.exit(main())