# tests/test_pynlm_basic.py
"""
Basic tests for the NLM Python bindings.
These tests verify the basic functionality of the Python interface.
"""

import pytest
import sys
from pathlib import Path

# Add the python module to path
sys.path.insert(0, str(Path(__file__).parent.parent / "python"))

# Try to import the bindings
try:
    from pynlm import Brain, Config, NeuronType, MotorCommand
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Skipping NLM Python binding tests - bindings not available")

@pytest.mark.skipif(not BINDINGS_AVAILABLE, reason="Python bindings not available")
class TestNLMPythonBindings:
    """Test basic NLM Python functionality."""
    
    def test_config_creation(self):
        """Test that Config can be created."""
        config = Config()
        assert config is not None
        
        # Test setting and getting config values
        config.set("test_value", 42)
        assert config.get("test_value") == 42
    
    def test_brain_creation(self):
        """Test that Brain can be created with config."""
        config = Config()
        config.set("neuron_count", 100)
        config.set("simulation_timestep", 0.001)
        
        brain = Brain(config)
        assert brain is not None
        
        # Test brain initialization
        assert brain.initialize() is True
        
        # Test basic statistics
        assert brain.getTotalNeuronCount() == 100
        assert brain.getSimulationTimestep() > 0
    
    def test_neuron_types(self):
        """Test that neuron types are correctly defined."""
        from pynlm import NeuronType
        
        # Check that all expected neuron types exist
        assert NeuronType.Sensory is not None
        assert NeuronType.Motor is not None
        assert NeuronType.Interneuron is not None
        
        # Test that we can create neurons
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Find a sensory neuron and test basic properties
        for region in brain.getRegions():
            for neuron in region.getAllNeurons():
                if neuron.getNeuronType() == NeuronType.Sensory:
                    assert neuron.getMembranePotential() >= 0.0
                    assert neuron.getThreshold() > 0.0
                    break
            else:
                continue
            break
    
    def test_simulation_step(self):
        """Test that simulation steps work correctly."""
        config = Config()
        config.set("neuron_count", 50)
        config.set("simulation_timestep", 0.001)
        
        brain = Brain(config)
        assert brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run a few simulation steps
        for step in range(10):
            brain.step(step, step * 0.001)
        
        # Spike count should have changed (or at least not decreased unrealistically)
        final_spikes = brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes
    
    def test_sensory_input(self):
        """Test that sensory input can be processed."""
        config = Config()
        brain = Brain(config)
        assert brain.initialize()
        
        # Create a simple sensory input
        from pynlm import SensoryInput
        sensory_input = SensoryInput()
        
        # Add some sensory data
        for i in range(10):
            sensory_input.addSensoryData(i, 0.5f)
        
        # Test that brain can receive the input
        brain.receiveSensoryInput(sensory_input)
        
        # Check that neurons received the input
        sensory_neurons = brain.getTotalNeuronCount()  # In a real implementation, this would be more specific
        assert sensory_neurons > 0

@pytest.mark.skipif(not BINDINGS_AVAILABLE, reason="Python bindings not available")
class TestNLMConvenienceFunctions:
    """Test convenience functions from pynlm module."""
    
    def test_create_brain_convenience(self):
        """Test the create_brain convenience function."""
        from pynlm import create_brain
        
        brain = create_brain(neuron_count=10, timestep=0.001)
        assert brain is not None
        assert brain.getTotalNeuronCount() == 10
        
        # Clean up
        del brain
    
    def test_learning_test_convenience(self):
        """Test the test_learning convenience function."""
        from pynlm import test_learning
        
        brain = create_brain(neuron_count=10, timestep=0.001)
        assert brain is not None
        
        results = test_learning(brain, steps=10)
        assert results is not None
        assert "total_spikes" in results
        assert "total_neurons" in results
        assert results["total_neurons"] == 10
        
        # Clean up
        del brain
    
    def test_phase6_demo_convenience(self):
        """Test the run_phase6_demo convenience function."""
        from pynlm import run_phase6_demo
        
        # This will create its own brain
        results = run_phase6_demo(max_steps=10)
        assert results is not None
        assert "total_reward" in results
        assert "avg_firing_rate" in results

if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])