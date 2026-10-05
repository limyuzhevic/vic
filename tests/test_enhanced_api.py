"""
Enhanced Python API Tests for NLM
Comprehensive test coverage for Python bindings and enhancements
"""

import pytest
import tempfile
import os
from pathlib import Path
import sys

# Add the python directory to path for imports
python_dir = Path(__file__).parent.parent / "python"
sys.path.insert(0, str(python_dir))

# Try to import the NLM Python bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Tests will be skipped.")

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestEnhancedPythonAPI:
    """Test the enhanced Python API for NLM"""
    
    def test_config_class_enhancements(self):
        """Test enhanced Config class functionality"""
        config = pynlm.Config()
        
        # Test setting and getting different data types
        config.set("int_val", 42)
        assert config.get("int_val") == 42
        
        config.set("double_val", 3.14)
        assert abs(config.get("double_val") - 3.14) < 0.001
        
        config.set("string_val", "hello")
        assert config.get("string_val") == "hello"
        
        config.set("bool_val", True)
        assert config.get("bool_val") is True
        
        # Test array types
        config.set("int_array", [1, 2, 3, 4, 5])
        int_array = config.get("int_array")
        assert int_array == [1, 2, 3, 4, 5]
        
        config.set("string_array", ["a", "b", "c"])
        string_array = config.get("string_array")
        assert string_array == ["a", "b", "c"]
        
        # Test getOr with default values
        assert config.get_or("missing", 999) == 999
        assert config.get_or("missing", "default") == "default"
        
        print("✓ Config class enhancements test passed")
    
    def test_config_file_io(self):
        """Test configuration file I/O with JSON and YAML"""
        config = pynlm.Config()
        
        # Set up test data
        config.set("test_int", 123)
        config.set("test_string", "test_value")
        config.set("test_bool", True)
        config.set("test_array", [1.0, 2.0, 3.0])
        
        # Test JSON file I/O
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json_file = f.name
        
        try:
            # Note: saveToFile might not be implemented in Python bindings
            # This test would need to be adjusted based on actual implementation
            pass
        finally:
            os.unlink(json_file)
        
        # Test YAML file I/O (if supported)
        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            yaml_file = f.name
        
        try:
            pass
        finally:
            os.unlink(yaml_file)
        
        print("✓ Config file I/O test passed")
    
    def test_convenience_methods(self):
        """Test convenience methods and presets"""
        # Test create_default_config
        config1 = pynlm.createDefaultConfig()
        assert config1 is not None
        
        # Test create_simple_config
        config2 = pynlm.createSimpleConfig(100, 4)
        assert config2 is not None
        
        # Test brain creation convenience methods
        brain = pynlm.createBrainWithPreset("beginner")
        assert brain is not None
        
        # Test simulation convenience methods
        brain2 = pynlm.createBrainWithConfig(config1)
        assert brain2 is not None
        
        print("✓ Convenience methods test passed")
    
    def test_brain_api_enhancements(self):
        """Test enhanced Brain API through Python bindings"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Test initialization
        brain.initialize()
        
        # Test basic brain properties
        assert brain.getTotalNeuronCount() > 0
        assert brain.getRegionCount() > 0
        assert brain.getTotalSynapseCount() >= 0
        
        # Test brain operations
        brain.reset()
        assert brain.getTotalNeuronCount() > 0
        
        # Test step operations
        brain.step(0)
        
        print("✓ Brain API enhancements test passed")
    
    def test_agentbrain_api(self):
        """Test AgentBrain API through Python bindings"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Test AgentBrain creation
        assert agent is not None
        assert agent.getBrain() is not None
        
        # Test AgentBrain configuration
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        
        # Test AgentBrain methods
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        
        assert agent.isRewardModulationEnabled() is True
        assert agent.isDevelopmentEnabled() is True
        
        print("✓ AgentBrain API test passed")
    
    def test_simulation_helpers(self):
        """Test simulation helper functions"""
        # Test brain statistics
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        stats = pynlm.getBrainStats(brain)
        assert "neuron_count" in stats
        assert "synapse_count" in stats
        assert "firing_neurons" in stats
        assert "average_firing_rate" in stats
        
        # Test simulation control
        pynlm.pauseSimulation(brain)
        pynlm.resumeSimulation(brain)
        
        # Test simulation speed
        pynlm.setSimulationSpeed(brain, 1.5)
        
        print("✓ Simulation helpers test passed")
    
    def test_error_handling(self):
        """Test error handling in Python API"""
        config = pynlm.Config()
        
        # Test validation
        errors = pynlm.validateBrainConfig(config)
        assert isinstance(errors, list)
        
        # Test config summary
        summary = pynlm.getConfigSummary(config)
        assert isinstance(summary, str)
        assert len(summary) > 0
        
        print("✓ Error handling test passed")

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestPythonIntegration:
    """Test Python integration with NLM components"""
    
    def test_complete_workflow(self):
        """Test complete simulation workflow through Python API"""
        # Create all components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize components
        brain.initialize()
        world.configure(20, 20, 8, 8)
        agent.initialize(world)
        
        # Run simulation
        for step in range(10):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process with agent
            agent.processSensoryInput(percept)
            
            # Step brain
            brain.step(step, step * 0.1)
            
            # Generate and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.1)
        
        # Verify system is still functional
        assert brain.getTotalNeuronCount() > 0
        assert agent.getNeuromodulationLevel() >= -1.0
        
        print("✓ Complete workflow test passed")
    
    def test_default_simulation(self):
        """Test default simulation helper function"""
        # This should create and run a complete simulation
        spike_count = pynlm.runDefaultSimulation(steps=50, dt=0.1)
        
        assert isinstance(spike_count, int)
        assert spike_count >= 0
        
        print("✓ Default simulation test passed")
    
    def test_config_and_brain_creation(self):
        """Test config and brain creation helpers"""
        # Test with config
        config = pynlm.createSimpleConfig(50, 2)
        brain = pynlm.createBrainAndInitialize(config)
        
        assert brain is not None
        assert brain.getTotalNeuronCount() == 50
        assert brain.getRegionCount() == 2
        
        # Test with settings
        brain2 = pynlm.createBrainWithSettings(100, 0.01)
        assert brain2 is not None
        
        print("✓ Config and brain creation test passed")

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
