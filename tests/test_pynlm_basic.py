"""
NLM Python Bindings - Basic Tests

Essential tests for NLM Python bindings functionality.
"""

import pytest
import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

def test_import():
    """Test that pynlm can be imported."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"

def test_version():
    """Test that __version__ attribute exists."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    assert hasattr(pynlm, '__version__'), "pynlm should have __version__ attribute"
    assert isinstance(pynlm.__version__, str), "__version__ should be a string"
    assert len(pynlm.__version__) > 0, "__version__ should not be empty"
    print(f"NLM version: {pynlm.__version__}")

def test_config_creation():
    """Test basic config creation."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    summary = config.summary()
    assert isinstance(summary, str)
    assert "Configuration" in summary

def test_brain_creation():
    """Test basic brain creation and initialization."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    brain.initialize()
    
    neuron_count = brain.getTotalNeuronCount()
    assert isinstance(neuron_count, int)
    assert neuron_count > 0
    
    # Test a step
    brain.step(0)
    
    total_spikes = brain.getTotalSpikeCount()
    assert isinstance(total_spikes, int)
    assert total_spikes >= 0

def test_world_creation():
    """Test basic world creation."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    world.configure(10, 10, 8, 8)
    world.reset()
    
    width = world.getWidth()
    height = world.getHeight()
    assert width == 10
    assert height == 10
    
    # Test update
    world.update(0.1)
    time = world.getSimulationTime()
    assert time == 0.1

def test_agent_creation():
    """Test basic agent creation."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    assert agent is not None
    
    # Test feature enabling
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isCuriosityEnabled()
    
    # Test sensory processing (with dummy data)
    from pynlm import Vision
    vision = Vision(8, 8, 3)
    agent.processSensoryInput(vision)
    
    # Test motor decoding
    from pynlm import MotorCommand
    cmd = agent.decodeMotorCommand()
    assert cmd in pynlm.MotorCommand
    
    # Test neuromodulation access
    neuromod = agent.getNeuromodulationLevel()
    assert isinstance(neuromod, (int, float))
    
    curiosity = agent.getCuriosityLevel()
    assert isinstance(curiosity, (int, float))

def test_invalid_ids():
    """Test invalid ID constants."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    assert pynlm.INVALID_NEURON_ID is not None
    assert pynlm.INVALID_SYNAPSE_ID is not None
    assert pynlm.INVALID_REGION_ID is not None
    assert pynlm.INVALID_POPULATION_ID is not None

if __name__ == '__main__':
    pytest.main([__file__, '-v'])
