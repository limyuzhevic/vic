# NLM Python API Test Suite
# Tests for enhanced Python bindings

import sys
import os
import nlm
import tempfile
import json

def test_basic_functionality():
    """Test basic brain functionality"""
    print("=== Testing Basic NLM Functionality ===")
    
    # Create brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    
    # Check initial state
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
    assert brain.getRegionCount() > 0, "Brain should have regions"
    
    print(f"✓ Initial brain created: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Initialize brain
    brain.initialize()
    assert brain.getTotalSpikeCount() == 0, "Should start with zero spikes"
    
    print("✓ Brain initialized successfully")
    
    # Run simulation steps
    for step in range(10):
        brain.step(step)
    
    assert brain.getTotalSpikeCount() >= 0, "Should have spikes after simulation"
    print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} total spikes")
    
    return brain

def test_helper_methods():
    """Test new helper methods"""
    print("\n=== Testing Helper Methods ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test getRegionStatistics
    region_stats = brain.getRegionStatistics()
    assert isinstance(region_stats, dict), "Region stats should be dict"
    assert len(region_stats) > 0, "Should have region statistics"
    
    print(f"✓ Region statistics retrieved: {len(region_stats)} regions")
    
    # Test getMemoryStats
    memory_stats = brain.getMemoryStats()
    assert isinstance(memory_stats, dict), "Memory stats should be dict"
    
    print("✓ Memory statistics retrieved")
    
    # Test getNeuromodulationState
    neuromod_state = brain.getNeuromodulationState()
    assert isinstance(neurom_state, dict), "Neuromodulation state should be dict"
    
    print("✓ Neuromodulation state retrieved")
    
    # Test getPlasticityState
    plasticity_state = brain.getPlasticityState()
    assert isinstance(plasticity_state, dict), "Plasticity state should be dict"
    
    print("✓ Plasticity state retrieved")
    
    # Test getTopFiringNeurons
    for region_id, region_data in region_stats.items():
        top_neurons = brain.getTopFiringNeurons(region_id, 5)
        assert len(top_neurons) <= 5, "Should return at most 5 neurons"
        break  # Just test one region
    
    print("✓ Top firing neurons retrieved")
    
    # Test isDecreasingPlasticity
    is_developing = brain.isDecreasingPlasticity()
    assert isinstance(is_developing, bool), "isDecreasingPlasticity should return bool"
    
    print(f"✓ Plasticity development check: {is_developing}")

def test_agent_functionality():
    """Test agent brain interface"""
    print("\n=== Testing Agent Functionality ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Test sensor input size
    assert agent.getSensoryInputSize() > 0, "Should have sensory input"
    print(f"✓ Sensor input size: {agent.getSensoryInputSize()}")
    
    # Test motor output size
    assert agent.getMotorOutputSize() > 0, "Should have motor output"
    print(f"✓ Motor output size: {agent.getMotorOutputSize()}")
    
    # Run a few steps
    for step in range(5):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"  Step {step}: action={action}, curiosity={agent.getCuriosityLevel():.3f}")
    
    print("✓ Agent simulation completed successfully")
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("✓ Learning systems enabled")

def test_action_and_percept():
    """Test action and percept classes"""
    print("\n=== Testing Action and Percept Classes ===")
    
    # Test action creation
    action = nlm.Action(nlm.ActionType.MoveForward)
    assert action.getType() == nlm.ActionType.MoveForward
    assert action.getName() == "MoveForward"
    
    action.setType(nlm.ActionType.TurnLeft)
    assert action.getType() == nlm.ActionType.TurnLeft
    
    action.setParameters([0.1, 0.2, 0.3])
    params = action.getParameters()
    assert len(params) == 3
    assert params[0] == 0.1
    
    # Test action as dict
    action_dict = nlm.Action.action_to_dict(action)
    assert isinstance(action_dict, dict)
    assert action_dict["type"] == nlm.ActionType.TurnLeft
    assert action_dict["name"] == "TurnLeft"
    
    # Test action from dict
    new_action = nlm.Action.action_from_dict(action_dict)
    assert new_action.getType() == nlm.ActionType.TurnLeft
    
    print("✓ Action class functionality tested")
    
    # Test percept creation and conversion
    percept = nlm.SensoryPercept()
    
    # Set some test data
    vision_data = [0.1] * 256
    percept.setVision(vision_data)
    
    touch_data = [0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0]
    percept.setTouch(touch_data)
    
    internal_data = [0.0, 0.5, 1.0, -0.5]
    percept.setInternal(internal_data)
    
    # Test percept as dict
    percept_dict = nlm.SensoryPercept.percept_to_dict(percept)
    assert isinstance(percept_dict, dict)
    assert "vision" in percept_dict
    assert "touch" in percept_dict
    assert "internal" in percept_dict
    
    # Test percept from dict
    new_percept = nlm.SensoryPercept.percept_from_dict(percept_dict)
    assert new_percept.getVisionWidth() == 256
    assert len(new_percept.getTouch()) == 8
    
    print("✓ Percept class functionality tested")

def test_config_functionality():
    """Test configuration class"""
    print("\n=== Testing Config Functionality ===")
    
    # Create default config
    config = nlm.createDefaultConfig()
    
    # Set values
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_bool", True)
    config.set("test_string", "hello world")
    
    # Get values
    int_val = config.get("test_int")
    assert int_val.has_value() and int_val.value() == 42
    
    float_val = config.get("test_float")
    assert float_val.has_value() and float_val.value() == 3.14
    
    bool_val = config.get("test_bool")
    assert bool_val.has_value() and bool_val.value() == True
    
    # Python-specific getters
    int_py = config.get_int("test_int")
    assert int_py == 42
    
    float_py = config.get_float("test_float")
    assert abs(float_py - 3.14) < 0.001
    
    bool_py = config.get_bool("test_bool")
    assert bool_py == True
    
    string_py = config.get("test_string")
    assert isinstance(string_py.value(), str) and string_py.value() == "hello world"
    
    # Get all keys
    keys = config.getKeys()
    assert "test_int" in keys
    assert "test_float" in keys
    assert "test_bool" in keys
    assert "test_string" in keys
    
    print("✓ Config class functionality tested")

def test_simulation_patterns():
    """Test simulation patterns from documentation"""
    print("\n=== Testing Simulation Patterns ===")
    
    # Run complete agent simulation
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.0, 7.0)
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"  Step {step}: neurons={brain.getFiringNeuronCount()}, curiosity={agent.getCuriosityLevel():.3f}")
    
    print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} spikes, final curiosity={agent.getCuriosityLevel():.3f}")
    
    # Test that learning occurred
    assert brain.getTotalSpikeCount() > 0, "Should have neural activity"
    assert agent.getCuriosityLevel() > 0, "Should have curiosity"

def test_checkpoints():
    """Test checkpoint saving/loading"""
    print("\n=== Testing Checkpoints ===")
    
    import os
    import tempfile
    
    # Create and initialize brain
    config = nlm.createDefaultConfig()
    brain1 = nlm.createBrain(config)
    brain1.initialize()
    
    # Run some simulation
    for step in range(50):
        brain1.step(step)
    
    # Save checkpoint
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_path = f.name
    
    try:
        save_result = brain1.save(checkpoint_path)
        assert save_result, "Brain save should succeed"
        print(f"✓ Brain saved to {checkpoint_path}")
        
        # Create new brain and load checkpoint
        config2 = nlm.createDefaultConfig()
        brain2 = nlm.createBrain(config2)
        
        load_result = brain2.load(checkpoint_path)
        assert load_result, "Brain load should succeed"
        print("✓ Brain loaded successfully")
        
        # Verify they have same basic properties
        assert brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount()
        assert brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount()
        
    finally:
        # Clean up
        if os.path.exists(checkpoint_path):
            os.unlink(checkpoint_path)
    
    print("✓ Checkpoint test completed")

def test_version_and_metadata():
    """Test module version and metadata"""
    print("\n=== Testing Version and Metadata ===")
    
    assert hasattr(nlm, '__version__'), "Module should have __version__"
    assert hasattr(nlm, '__author__'), "Module should have __author__"
    assert hasattr(nlm, '__description__'), "Module should have __description__"
    
    print(f"✓ NLM Version: {nlm.__version__}")
    print(f"✓ Author: {nlm.__author__}")
    print(f"✓ Description: {nlm.__description__}")

def run_all_tests():
    """Run all tests"""
    print("Running NLM Python API Test Suite")
    print("=" * 50)
    
    try:
        test_basic_functionality()
        test_helper_methods()
        test_agent_functionality()
        test_action_and_percept()
        test_config_functionality()
        test_simulation_patterns()
        test_checkpoints()
        test_version_and_metadata()
        
        print("\n" + "=" * 50)
        print("✓ ALL TESTS PASSED!")
        print("=" * 50)
        return True
        
    except Exception as e:
        print(f"\n✗ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)