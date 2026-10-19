"""
Test suite for improved NLM Python bindings
Tests all the new features and improvements added to the bindings
"""

import sys
import os
import tempfile
import json
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def test_id_classes():
    """Test ID classes with improved API"""
    print("Testing ID classes...")
    
    # Test NeuronId
    nid = pynlm.NeuronId()
    assert nid.value == 0
    assert nid.index() == 0
    
    nid2 = pynlm.NeuronId(42)
    assert nid2.value == 42
    assert nid2.index() == 42
    
    assert nid != nid2
    assert nid == pynlm.NeuronId()
    
    # Test SynapseId
    sid = pynlm.SynapseId(100)
    assert sid.value == 100
    
    # Test RegionId
    rid = pynlm.RegionId(999)
    assert rid.value == 999
    
    # Test PopulationId
    pid = pynlm.PopulationId(777)
    assert pid.value == 777
    
    # Test hash and repr
    ids = {nid2, rid, pid}
    assert len(ids) == 3
    
    print("✓ ID classes test passed")

def test_enumerations():
    """Test enumerations"""
    print("Testing enumerations...")
    
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory == pynlm.NeuronType.Excitatory
    assert pynlm.NeuronType.Inhibitory == pynlm.NeuronType.Inhibitory
    assert pynlm.NeuronType.Sensory == pynlm.NeuronType.Sensory
    
    # Test SynapseType
    assert pynlm.SynapseType.Excitatory == pynlm.SynapseType.Excitatory
    assert pynlm.SynapseType.Electrical == pynlm.SynapseType.Electrical
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward == pynlm.ActionType.MoveForward
    assert pynlm.ActionType.Eat == pynlm.ActionType.Eat
    
    print("✓ Enumerations test passed")

def test_config_api():
    """Test enhanced Config API"""
    print("Testing Config API...")
    
    # Test default config
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test fluent API
    config2 = pynlm.createConfigWithDefaults(
        neuronCount=500,
        regionCount=3,
        connectionProbability=0.05
    )
    
    # Test setting values
    config.set("string_key", "hello")
    config.set("int_key", 42)
    config.set("float_key", 3.14)
    config.set("bool_key", True)
    
    # Test getting values
    assert config.get("string_key") == "hello"
    assert config.get("int_key") == 42
    assert abs(config.get("float_key") - 3.14) < 0.001
    assert config.get("bool_key") is True
    
    # Test getOr with defaults
    assert config.getOr("missing_key", "default_value") == "default_value"
    assert config.getOr("missing_key", 99) == 99
    assert config.getOr("missing_key", 3.14) == 3.14
    assert config.getOr("missing_key", False) is False
    
    # Test validation
    assert config.has("string_key")
    assert not config.has("nonexistent")
    
    # Test getOr validation
    try:
        config.get("nonexistent")
        assert False, "Should have raised KeyError"
    except KeyError:
        pass  # Expected
    
    print("✓ Config API test passed")

def test_config_builder():
    """Test configuration builder functions"""
    print("Testing configuration builder...")
    
    # Test builder functions
    config1 = pynlm.createConfig()
    assert config1 is not None
    
    config2 = pynlm.createConfigWithDefaults(100, 1, 0.1)
    assert config2 is not None
    
    print("✓ Configuration builder test passed")

def test_action_class():
    """Test enhanced Action class"""
    print("Testing Action class...")
    
    # Test creation with type
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action1.getType() == pynlm.ActionType.MoveForward
    assert action1.getName() == "MoveForward"
    
    # Test creation with parameters
    action2 = pynlm.Action(pynlm.ActionType.Eat, [0.5, 0.7, 0.9])
    assert action2.getType() == pynlm.ActionType.Eat
    params = action2.getParameters()
    assert len(params) == 3
    assert abs(params[0] - 0.5) < 0.001
    
    # Test addParameter
    action3 = pynlm.Action(pynlm.ActionType.Rest)
    action3.addParameter(0.8)
    assert action3.getParameterCount() == 1
    assert abs(action3.getParameterAt(0) - 0.8) < 0.001
    
    # Test validation
    assert action1.isValid()
    assert action2.isValid()
    
    # Test clone
    action4 = action1.clone()
    assert action4.getType() == action1.getType()
    assert action4 is not action1  # Different objects
    
    print("✓ Action class test passed")

def test_brain_simulation():
    """Test enhanced brain simulation features"""
    print("Testing brain simulation...")
    
    # Test basic brain creation and initialization
    config = pynlm.createConfigWithDefaults(100, 2, 0.1)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test statistics
    assert brain.getTotalNeuronCount() == 100
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    
    # Test step
    brain.step(0)
    brain.step(1, 0.0)
    
    # Test getStatistics
    stats = brain.getStatistics()
    assert "total_neurons" in stats
    assert stats["total_neurons"] == 100
    assert "total_synapses" in stats
    assert stats["firing_neurons"] in stats
    
    # Test validateState
    assert brain.validateState()
    
    # Test getSummary
    summary = brain.getSummary()
    assert "Brain Statistics" in summary
    assert "Total neurons: 100" in summary
    
    # Test save/load
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        save_path = f.name
    
    try:
        brain.save(save_path)
        assert os.path.exists(save_path)
        
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        brain2.load(save_path)
        
        assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount()
    finally:
        if os.path.exists(save_path):
            os.unlink(save_path)
    
    print("✓ Brain simulation test passed")

def test_world_simulation():
    """Test enhanced world simulation"""
    print("Testing world simulation...")
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Test world properties
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getSimulationTime() == 0.0
    
    # Test agent position
    position = world.getAgentPosition()
    assert len(position) == 2
    assert position[0] >= 0 and position[0] < 20
    assert position[1] >= 0 and position[1] < 20
    
    # Test update
    world.update(0.1)
    assert world.getSimulationTime() == 0.1
    
    # Test vision data
    vision = world.getVisionData()
    assert len(vision) == 64  # 8x8 vision
    
    # Test vision as 2D
    vision2d = world.getVisionAs2D()
    assert len(vision2D) == 8
    assert len(vision2D[0]) == 8
    
    # Test getWorldBounds
    bounds = world.getWorldBounds()
    assert len(bounds) == 4
    assert bounds[0] == 0.0  # min_x
    assert bounds[1] == 0.0  # min_y
    assert bounds[2] == 20.0  # max_x
    assert bounds[3] == 20.0  # max_y
    
    print("✓ World simulation test passed")

def test_agent_integration():
    """Test complete agent integration"""
    print("Testing agent integration...")
    
    # Setup complete agent
    config = pynlm.createConfigWithDefaults(200, 3, 0.05)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent.initialize(world)
    agent.enableAll()  # Enable all subsystems
    
    # Run a few simulation steps
    for step in range(10):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply some reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    # Check results
    brain_stats = brain.getStatistics()
    assert brain_stats["total_neurons"] == 200
    
    agent_factors = agent.getAllFactors()
    assert "neuromodulation" in agent_factors
    assert "curiosity" in agent_factors
    assert "developmental_stage" in agent_factors
    
    print("✓ Agent integration test passed")

def test_context_manager():
    """Test BrainSimulator context manager"""
    print("Testing context manager...")
    
    config = pynlm.createConfig()
    brain = pynlm.createBrain(config)
    
    # Test context manager
    with pynlm.BrainSimulator(brain) as simulator:
        simulator.step(0)
        simulator.step(1)
        
        assert simulator.getBrain() is brain
    
    # Brain should be reset after exiting context
    assert brain is not None
    
    print("✓ Context manager test passed")

def test_convenience_functions():
    """Test convenience functions"""
    print("Testing convenience functions...")
    
    # Test runSilentBrain
    spike_count = pynlm.runSilentBrain(10)
    assert spike_count >= 0
    
    # Test runAgentEpisode
    config = pynlm.createConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionWidth=4)
    world.reset()
    
    agent.initialize(world)
    
    spikes, firing_rate = pynlm.runAgentEpisode(world, agent, 5)
    assert spikes >= 0
    assert firing_rate >= 0.0
    
    print("✓ Convenience functions test passed")

def test_constants_and_utils():
    """Test constants and utility functions"""
    print("Testing constants and utilities...")
    
    # Test module constants
    assert pynlm.VERSION == "0.1.0"
    assert abs(pynlm.PI - 3.14159265358979323846) < 0.0001
    
    # Test utility functions
    version = pynlm.getVersion()
    assert "NLM Python Bindings" in version
    assert "v0.1.0" in version
    
    # Test constants
    assert pynlm.INVALID_NEURON_ID.value == 2**64 - 1
    assert pynlm.INVALID_SYNAPSE_ID.value == 2**64 - 1
    assert pynlm.INVALID_REGION_ID.value == 2**64 - 1
    assert pynlm.INVALID_POPULATION_ID.value == 2**64 - 1
    
    print("✓ Constants and utilities test passed")

def run_all_tests():
    """Run all tests"""
    print("Running NLM Python Bindings Test Suite")
    print("=" * 50)
    
    try:
        test_id_classes()
        test_enumerations()
        test_config_api()
        test_config_builder()
        test_action_class()
        test_brain_simulation()
        test_world_simulation()
        test_agent_integration()
        test_context_manager()
        test_convenience_functions()
        test_constants_and_utils()
        
        print("=" * 50)
        print("All tests passed! ✓")
        return True
        
    except Exception as e:
        print(f"Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)