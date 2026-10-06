import pynlm as nlm
import tempfile
import os

def test_basic_bindings():
    """Test basic Python bindings functionality."""
    print("Testing basic Python bindings...")
    
    # Test 1: Create default config
    config = nlm.createDefaultConfig()
    assert config is not None
    print("✓ Default config creation successful")
    
    # Test 2: Configure and create brain
    config.set("neuron_count", 50)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    
    brain = nlm.createBrain(config)
    assert brain is not None
    print("✓ Brain creation successful")
    
    # Test 3: Initialize brain
    success = brain.initialize()
    assert success, "Brain initialization failed"
    print("✓ Brain initialization successful")
    
    # Test 4: Get brain properties
    assert brain.getTotalNeuronCount() == 50
    assert brain.getRegionCount() == 2
    print("✓ Brain properties correct")
    
    # Test 5: Run simulation steps
    for step in range(10):
        brain.step(step)
    
    assert brain.getTotalNeuronCount() == 50
    print("✓ Simulation steps successful")
    
    # Test 6: Action production
    action = brain.produceAction()
    assert action is not None
    print("✓ Action production successful")
    
    # Test 7: Save and load brain
    with tempfile.NamedTemporaryFile(delete=False, suffix='.nlm') as f:
        filepath = f.name
    
    try:
        brain.save(filepath)
        assert os.path.exists(filepath)
        
        # Create new brain and load
        brain2 = nlm.createBrain(nlm.createDefaultConfig())
        brain2.load(filepath)
        assert brain2.getTotalNeuronCount() == 50
        
        print("✓ Save/load functionality successful")
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)
    
    # Test 8: Reset functionality
    brain.reset()
    assert brain.getTotalNeuronCount() == 50
    print("✓ Reset functionality successful")
    
    print("Basic bindings tests PASSED!\n")

def test_world_bindings():
    """Test world-related Python bindings."""
    print("Testing world bindings...")
    
    # Create world and agent
    world = nlm.createSimpleWorld()
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    agent = nlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(10, 10, 5, 5)
    assert world.getWidth() == 10
    assert world.getHeight() == 10
    print("✓ World configuration successful")
    
    # Initialize systems
    brain.initialize()
    agent.initialize(world)
    print("✓ World system initialization successful")
    
    # Test agent body
    agent_body = world.getAgentBody()
    agent_body.x = 5.0
    agent_body.y = 5.0
    assert agent_body.x == 5.0
    assert agent_body.y == 5.0
    print("✓ Agent body manipulation successful")
    
    # Test sensory percept
    percept = world.getSensoryPercept()
    assert percept is not None
    print("✓ Sensory percept access successful")
    
    # Test world update
    world.update(0.001)
    assert world.getSimulationTime() > 0
    print("✓ World update successful")
    
    # Test motor command application
    cmd = nlm.MotorCommand.TurnLeft
    world.applyMotorCommand(cmd, 0.0)
    print("✓ Motor command application successful")
    
    # Test world reset
    world.reset()
    assert world.getSimulationTime() == 0.0
    print("✓ World reset successful")
    
    print("World bindings tests PASSED!\n")

def test_agent_bindings():
    """Test agent-related Python bindings."""
    print("Testing agent bindings...")
    
    # Create full agent system
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    
    # Configure and initialize
    world.configure(20, 20, 10, 10)
    world.setAgentStart(10, 10)
    
    brain.initialize()
    agent.initialize(world)
    print("✓ Agent system initialization successful")
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    print("✓ Sensory processing successful")
    
    # Test motor decoding
    cmd = agent.decodeMotorCommand()
    assert cmd is not None
    print("✓ Motor command decoding successful")
    
    # Test neuromodulation
    reward = 5.0
    predicted_reward = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted_reward)
    print("✓ Neuromodulation application successful")
    
    # Test development update
    agent.updateDevelopment(0.001)
    stage = agent.getDevelopmentalStage()
    assert stage is not None
    print("✓ Development update successful")
    
    # Test agent reset
    agent.reset()
    print("✓ Agent reset successful")
    
    # Test feature toggles
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isCuriosityEnabled()
    assert agent.isDevelopmentEnabled()
    print("✓ Feature toggles successful")
    
    print("Agent bindings tests PASSED!\n")

def test_error_handling():
    """Test Python error handling."""
    print("Testing error handling...")
    
    try:
        # Test invalid neuron ID
        neuron_id = nlm.NeuronId(-1)
        print("✗ Should have raised exception for invalid neuron ID")
        assert False
    except nlm.InvalidNeuronIdError as e:
        print("✓ Invalid neuron ID correctly rejected")
    
    try:
        # Test invalid synapse ID
        synapse_id = nlm.SynapseId(-100)
        print("✗ Should have raised exception for invalid synapse ID")
        assert False
    except nlm.InvalidSynapseIdError as e:
        print("✓ Invalid synapse ID correctly rejected")
    
    try:
        # Test invalid region ID
        region_id = nlm.RegionId(-50)
        print("✗ Should have raised exception for invalid region ID")
        assert False
    except nlm.InvalidRegionIdError as e:
        print("✓ Invalid region ID correctly rejected")
    
    try:
        # Test parameter range validation
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        # Try to step with negative step (should be caught by validation)
        brain.step(-1)
        print("✗ Should have raised exception for invalid step")
        assert False
    except nlm.InvalidParameterError as e:
        print("✓ Invalid parameter correctly rejected")
    
    print("Error handling tests PASSED!\n")

def test_config_bindings():
    """Test configuration-related Python bindings."""
    print("Testing configuration bindings...")
    
    # Test default config
    config = nlm.createDefaultConfig()
    assert config is not None
    
    # Test setting different value types
    config.set("int_value", 42)
    config.set("float_value", 3.14)
    config.set("string_value", "test")
    config.set("bool_value", True)
    config.set("double_value", 2.71828)
    
    assert config.has("int_value")
    assert config.has("float_value")
    assert config.has("string_value")
    assert config.has("bool_value")
    assert config.has("double_value")
    
    # Test getOr with default
    int_val = config.getOr("int_value", 0)
    assert int_val == 42
    
    string_val = config.getOr("nonexistent", "default")
    assert string_val == "default"
    
    # Test configuration summary
    summary = config.summary()
    assert isinstance(summary, str)
    assert len(summary) > 0
    print("✓ Configuration summary generation successful")
    
    # Test clear
    config.clear()
    assert not config.has("int_value")
    assert not config.has("float_value")
    print("✓ Configuration clear successful")
    
    print("Configuration bindings tests PASSED!\n")

def test_sensory_bindings():
    """Test sensory input bindings."""
    print("Testing sensory input bindings...")
    
    # Test Vision
    vision = nlm.Vision(10, 10, 3)
    assert vision.getWidth() == 10
    assert vision.getHeight() == 10
    assert vision.getChannels() == 3
    
    # Test setting vision data
    test_data = [0.1] * (10 * 10 * 3)
    vision.setData(test_data)
    print("✓ Vision input successful")
    
    # Test Audio
    audio = nlm.Audio(44100, 1024)
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 1024
    
    audio.setSampleRate(48000)
    assert audio.getSampleRate() == 48000
    print("✓ Audio input successful")
    
    # Test InternalSignals
    internal = nlm.InternalSignals()
    internal.addSignal(0.5)
    internal.addSignal(-0.3)
    print("✓ Internal signals successful")
    
    # Test SensoryInput base class
    sensory = nlm.SensoryInput()
    sensory.setTimestamp(123.45)
    assert sensory.getTimestamp() == 123.45
    print("✓ SensoryInput base class successful")
    
    print("Sensory bindings tests PASSED!\n")

def test_action_bindings():
    """Test action-related Python bindings."""
    print("Testing action bindings...")
    
    # Test ActionType enum
    action_type = nlm.ActionType.MoveForward
    assert action_type == nlm.ActionType.MoveForward
    print("✓ ActionType enum successful")
    
    # Test Action creation from type
    action = nlm.Action(nlm.ActionType.MoveForward)
    assert action.getType() == nlm.ActionType.MoveForward
    assert action.getName() == "MoveForward"
    print("✓ Action creation from type successful")
    
    # Test Action with parameters
    action2 = nlm.Action(nlm.ActionType.Custom, [0.1, 0.5, 0.9])
    assert action2.getType() == nlm.ActionType.Custom
    params = action2.getParameters()
    assert params[0] == 0.1
    assert params[1] == 0.5
    assert params[2] == 0.9
    print("✓ Action with parameters successful")
    
    # Test action cloning
    action3 = action2.clone()
    assert action3.getType() == action2.getType()
    assert action3.getParameters() == action2.getParameters()
    print("✓ Action cloning successful")
    
    print("Action bindings tests PASSED!\n")

def test_memory_bindings():
    """Test memory-related Python bindings (if available)."""
    print("Testing memory bindings...")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test working memory (may be None if not implemented)
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print("✓ Working memory available")
        
        # Test episodic memory
        episodic_memory = brain.getEpisodicMemory()
        if episodic_memory:
            print("✓ Episodic memory available")
            
            # Test semantic memory
            semantic_memory = brain.getSemanticMemory()
            if semantic_memory:
                print("✓ Semantic memory available")
                
                # Test procedural memory
                procedural_memory = brain.getProceduralMemory()
                if procedural_memory:
                    print("✓ Procedural memory available")
    
    print("Memory bindings tests completed (partial support)")

def test_performance_bindings():
    """Test performance-related Python bindings (if available)."""
    print("Testing performance bindings...")
    
    # Test checkpoint system
    checkpoint_system = nlm.CheckpointSystem()
    if checkpoint_system:
        print("✓ Checkpoint system available")
        
        # Test metrics collection
        metrics = nlm.Metrics()
        if metrics:
            print("✓ Metrics collection available")
    
    # Test visualization interface
    viz_interface = nlm.VisualizationInterface()
    if viz_interface:
        print("✓ Visualization interface available")
    
    print("Performance bindings tests completed (partial support)")

def main():
    """Run all Python binding tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    print()
    
    try:
        test_basic_bindings()
        test_config_bindings()
        test_sensory_bindings()
        test_action_bindings()
        test_world_bindings()
        test_agent_bindings()
        test_error_handling()
        test_memory_bindings()
        test_performance_bindings()
        
        print("=" * 60)
        print("ALL PYTHON BINDINGS TESTS PASSED!")
        print("=" * 60)
        print()
        print("Summary:")
        print("- Basic bindings: ✓ Working")
        print("- Configuration: ✓ Working")
        print("- Sensory inputs: ✓ Working")
        print("- Actions: ✓ Working")
        print("- World simulation: ✓ Working")
        print("- Agent brain: ✓ Working")
        print("- Error handling: ✓ Working")
        print("- Memory systems: ✓ Partially available")
        print("- Performance features: ✓ Partially available")
        
    except Exception as e:
        print("=" * 60)
        print(f"TEST SUITE FAILED: {e}")
        print("=" * 60)
        raise

if __name__ == "__main__":
    main()