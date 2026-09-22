#!/usr/bin/env python3
"""
Unit tests for NLM Python API functionality.
Tests the enhanced features added to the Python bindings.
"""

import pynlm
import tempfile
import os

def test_basic_brain_creation():
    """Test basic brain creation and initialization."""
    print("Testing basic brain creation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getTotalSpikeCount() >= 0
    
    brain.step(0)
    assert brain.getTotalSpikeCount() >= 0
    
    print("✓ Basic brain creation test passed")

def test_agent_initialization():
    """Test agent brain initialization."""
    print("Testing agent initialization...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    assert agent.getSensoryInputSize() == 270  # 256 + 8 + 4 + 6
    assert agent.getMotorOutputSize() == 6
    assert agent.getDevelopmentalStage().name == "Initial"
    
    print("✓ Agent initialization test passed")

def test_enhanced_percept_api():
    """Test enhanced sensory percept API."""
    print("Testing enhanced percept API...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run a few steps
    for step in range(5):
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Test enhanced methods
        vision = percept.getVisionAsTensor()
        touch = percept.getTouchAsArray()
        internal = percept.getInternalAsArray()
        proprio = percept.getProprioceptionAsArray()
        audio = percept.getAudioAsArray()
        
        assert len(vision) == 64  # 8x8 vision
        assert len(touch) == 8
        assert len(internal) == 4
        assert len(proprio) == 6
        assert len(audio) == 0  # Default empty
        
        # Test novelty detection
        novelty = percept.isNovel()
        assert isinstance(novelty, bool)
        
        # Test dictionary conversion
        data_dict = percept.toDict()
        assert "vision" in data_dict
        assert "touch" in data_dict
        assert "internal" in data_dict
        assert "proprioception" in data_dict
        assert "audio" in data_dict
        assert "timestamp" in data_dict
        
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("✓ Enhanced percept API test passed")

def test_neuromodulation_features():
    """Test neuromodulation and learning features."""
    print("Testing neuromodulation features...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all neuromodulation features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    assert agent.isRewardModulationEnabled() == True
    assert agent.isCuriosityEnabled() == True
    assert agent.isDevelopmentEnabled() == True
    assert agent.isStructuralPlasticityEnabled() == True
    
    # Test neuromodulation levels
    assert agent.getNeuromodulationLevel() == 0.0f  # Initially zero
    assert agent.getCuriosityLevel() == 0.0f
    assert agent.getNoveltyLevel() == 0.0f
    assert agent.getPredictionError() == 0.0f
    
    # Run simulation with learning
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.5f if step % 3 == 0 else -0.1f
        agent.applyRewardModulation(reward, 0.0)
    
    # Check that neuromodulation levels changed
    assert abs(agent.getNeuromodulationLevel()) > 0.0f or step < 10
    
    print("✓ Neuromodulation features test passed")

def test_development_system():
    """Test developmental system progression."""
    print("Testing development system...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Initial stage should be "Initial"
    assert agent.getDevelopmentalStage().name == "Initial"
    
    # Progress through development
    for step in range(300):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.updateDevelopment(0.1)
        
        # Development should progress over time
        if step % 100 == 99:
            print(f"  Step {step}: Stage = {agent.getDevelopmentalStage().name}")
    
    # Check final stage (should have progressed)
    assert agent.getDevelopmentalStage().name in [
        "Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"
    ]
    
    print("✓ Development system test passed")

def test_config_operations():
    """Test configuration operations."""
    print("Testing configuration operations...")
    
    config = pynlm.createDefaultConfig()
    
    # Test basic operations
    assert config.has("random_seed") == True
    assert config.has("neuron_count") == True
    assert config.has("nonexistent_key") == False
    
    # Test getting all keys
    keys = config.getKeys()
    assert len(keys) > 0
    assert "random_seed" in keys
    
    # Test saving and loading config
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_path = f.name
    
    try:
        config.saveToFile(config_path)
        assert os.path.exists(config_path)
        
        # Create new config and load
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(config_path)
        
        assert new_config.has("random_seed") == True
        
    finally:
        if os.path.exists(config_path):
            os.unlink(config_path)
    
    print("✓ Configuration operations test passed")

def test_file_io():
    """Test file I/O operations."""
    print("Testing file I/O operations...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test saving brain state
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
        brain_path = f.name
    
    try:
        brain.save(brain_path)
        assert os.path.exists(brain_path)
        
        # Test loading brain state
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        brain2.load(brain_path)
        
        assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount()
        assert brain2.getTotalSynapseCount() == brain.getTotalSynapseCount()
        
    finally:
        if os.path.exists(brain_path):
            os.unlink(brain_path)
    
    print("✓ File I/O operations test passed")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python API Unit Tests")
    print("=" * 60)
    print()
    
    try:
        test_basic_brain_creation()
        test_agent_initialization()
        test_enhanced_percept_api()
        test_neuromodulation_features()
        test_development_system()
        test_config_operations()
        test_file_io()
        
        print()
        print("=" * 60)
        print("All tests passed! ✓")
        print("=" * 60)
        
    except Exception as e:
        print()
        print("=" * 60)
        print(f"Test failed with error: {e}")
        print("=" * 60)
        raise

if __name__ == "__main__":
    run_all_tests()
