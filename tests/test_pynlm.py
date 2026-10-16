"""
Python tests for NLM (Neural Learning Machine) bindings.

These tests verify that the Python bindings work correctly with the
C++ neural simulation framework.
"""

import pynlm
import pytest
import tempfile
import os


def test_version():
    """Test that version information is available."""
    assert hasattr(pynlm, '__version__')
    assert isinstance(pynlm.__version__, str)
    assert pynlm.__version__ == "0.1.0"
    
    # Test version constants
    assert hasattr(pynlm, 'VERSION')
    assert hasattr(pynlm, 'NLM_VERSION_STRING')
    assert hasattr(pynlm, 'NLM_VERSION_MAJOR')
    assert hasattr(pynlm, 'NLM_VERSION_MINOR')
    assert hasattr(pynlm, 'NLM_VERSION_PATCH')


def test_factory_functions():
    """Test factory functions create objects correctly."""
    # Test createDefaultConfig
    config = pynlm.createDefaultConfig()
    assert config is not None
    assert config.has("neuronCount")
    
    # Test createBrain
    brain = pynlm.createBrain(config)
    assert brain is not None
    assert brain.getRegionCount() >= 0
    
    # Test createSimpleWorld
    world = pynlm.createSimpleWorld()
    assert world is not None
    assert world.getWidth() > 0
    assert world.getHeight() > 0
    
    # Test createAgentBrain
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0


def test_brain_operations():
    """Test basic brain operations."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    brain.initialize()
    
    # Test step operations
    brain.step(0)  # Step 0, default time
    brain.step(1, 0.1)  # Step 1, time 0.1
    
    # Test basic statistics
    assert brain.getTotalNeuronCount() >= 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getActiveNeuronCount() >= 0
    
    # Test developmental stage
    stage = brain.getDevelopmentalStage()
    assert stage in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    # Test reset
    brain.reset()
    
    # Test logging
    brain.logStatus()


def test_world_operations():
    """Test world operations."""
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    
    # Test agent start
    world.setAgentStart(10, 10)
    
    # Test basic properties
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    
    # Test sensory percept
    world.reset()
    percept = world.getSensoryPercept()
    assert percept is not None
    
    # Test world objects
    obj = pynlm.WorldObject()
    obj.x = 5.0
    obj.y = 5.0
    obj.type = pynlm.WorldObjectType.Resource
    obj.value = 10.0
    
    world.addObject(obj)
    assert world.isValidPosition(5, 5)
    
    # Test energy settings
    world.setMaxEnergy(200.0)
    assert world.getMaxEnergy() == 200.0
    
    world.setEnergyDecayRate(0.98)
    assert world.getEnergyDecayRate() == 0.98
    
    # Test simulation time
    initial_time = world.getSimulationTime()
    world.update(0.1)
    assert world.getSimulationTime() > initial_time


def test_agent_brain_operations():
    """Test agent brain operations."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 4, 4)
    world.setAgentStart(5, 5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test basic properties
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0
    
    # Test neuromodulation
    agent.applyRewardModulation(1.0, 0.5)
    assert agent.getNeuromodulationLevel() >= -1.0
    assert agent.getNeuromodulationLevel() <= 1.0
    
    # Test curiosity
    assert agent.getCuriosityLevel() >= 0.0
    assert agent.getCuriosityLevel() <= 1.0
    
    # Test novelty
    assert agent.getNoveltyLevel() >= 0.0
    
    # Test prediction error
    assert agent.getPredictionError() <= 1.0
    
    # Test development
    agent.updateDevelopment(1.0)
    stage = agent.getDevelopmentalStage()
    assert stage in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    # Test enable/disable features
    agent.enableRewardModulation(False)
    agent.enableStructuralPlasticity(False)
    agent.enableDevelopment(False)
    agent.enableCuriosity(False)
    
    assert not agent.isRewardModulationEnabled()
    assert not agent.isStructuralPlasticityEnabled()
    assert not agent.isDevelopmentEnabled()
    assert not agent.isCuriosityEnabled()
    
    # Reset agent
    agent.reset()


def test_config_operations():
    """Test configuration operations."""
    config = pynlm.createDefaultConfig()
    
    # Test default values
    assert config.has("neuronCount")
    assert config.has("synapseDensity")
    assert config.has("connectionProbability")
    
    # Test getting keys
    keys = config.getKeys()
    assert isinstance(keys, list)
    assert len(keys) > 0
    
    # Test save/load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        filepath = f.name
    
    try:
        config.saveToFile(filepath)
        
        # Create new config and load
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(filepath)
        
        # Load should not crash
        assert new_config is not None
    finally:
        os.unlink(filepath)
    
    # Test has operator
    assert "neuronCount" in config
    assert "nonexistentKey" not in config
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str)
    assert len(summary) > 0


def test_sensory_percept():
    """Test sensory percept operations."""
    percept = pynlm.SensoryPercept()
    
    # Test default creation
    assert percept is not None
    
    # Test vision
    vision = pynlm.Vision()
    vision.setData([0.0] * 64)
    assert vision.getWidth() == 8
    assert vision.getHeight() == 8
    
    percept.setVision(vision)
    assert percept.getVision() is not None
    
    # Test audio
    audio = pynlm.Audio()
    audio.setSampleRate(44100)
    audio.setData([0.0] * 1024)
    assert audio.getSampleRate() == 44100
    
    percept.setAudio(audio)
    assert percept.getAudio() is not None
    
    # Test internal signals
    percept.setInternal([0.1, 0.2, 0.3])
    assert percept.getInternal() == [0.1, 0.2, 0.3]
    
    # Test all signals
    signals = percept.getAllSignals()
    assert isinstance(signals, dict)
    
    # Test timestamp
    import time
    ts = time.time()
    percept.setTimestamp(ts)
    assert percept.getTimestamp() == ts


def test_action():
    """Test action operations."""
    # Create action with type only
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action1.getType() == pynlm.ActionType.MoveForward
    assert action1.getParameters() == []
    
    # Create action with parameters
    action2 = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.7, 0.3])
    assert action2.getType() == pynlm.ActionType.Custom
    assert action2.getParameters() == [0.5, 0.7, 0.3]
    
    # Test name
    assert action1.getName() == "Move Forward"
    
    # Test set parameters
    action1.setParameters([1.0, 2.0])
    assert action1.getParameters() == [1.0, 2.0]
    
    # Test clone
    action3 = action1.clone()
    assert action3.getType() == action1.getType()
    assert action3.getParameters() == action1.getParameters()
    
    # Test copy semantics
    action4 = pynlm.Action()
    action4.setType(pynlm.ActionType.Interact)
    assert action4.getType() == pynlm.ActionType.Interact


def test_constants_and_utils():
    """Test constants and utility functions."""
    # Test constants
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    
    # Test version constants
    assert hasattr(pynlm, 'SIMULATION_TIME_STEP')
    assert hasattr(pynlm, 'MAX_ENERGY')
    assert hasattr(pynlm, 'ENERGY_DECAY_RATE')
    
    # Test utility functions
    version_info = pynlm.getVersionInfo()
    assert isinstance(version_info, dict)
    assert 'version' in version_info
    assert 'description' in version_info
    
    # Test example brain
    example_brain = pynlm.createExampleBrain()
    assert example_brain is not None
    assert example_brain.getTotalNeuronCount() >= 0


def test_agent_example():
    """Test complete agent example from documentation."""
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.setAgentStart(5, 5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Verify results
    assert brain.getTotalSpikeCount() >= 0
    assert brain.getFiringNeuronCount() >= 0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])