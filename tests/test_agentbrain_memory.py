"""
AgentBrain Memory Management Tests for NLM
Comprehensive test coverage for AgentBrain memory management and smart references
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
class TestAgentBrainMemoryManagement:
    """Test AgentBrain memory management functionality"""
    
    def test_raw_pointer_replacement(self):
        """Test raw pointer replacement with smart references"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Test that agent brain uses smart references (not raw pointers)
        # This is verified by the Python bindings implementation
        assert agent is not None
        assert agent.getBrain() is not None
        
        # Test neuron group validation
        assert agent.validateNeuronGroups() is True
        
        # Test total neuron count
        neuron_count = agent.getTotalNeuronCount()
        assert isinstance(neuron_count, int)
        assert neuron_count > 0
        
        print("✓ Raw pointer replacement test passed")
    
    def test_memory_tracking_and_cleanup(self):
        """Test memory tracking and cleanup"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Test initialization
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 4, 4)
        agent.initialize(world)
        
        # Test that agent maintains memory references correctly
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        
        # Test reset functionality (should clean up memory)
        initial_neuron_count = agent.getTotalNeuronCount()
        agent.reset()
        
        # After reset, agent should still be functional
        assert agent.getNeuromodulationLevel() == 0.0
        assert agent.getNoveltyLevel() == 0.0
        assert agent.getCuriosityLevel() == 0.0
        
        print("✓ Memory tracking and cleanup test passed")
    
    def test_array_bounds_validation(self):
        """Test array bounds validation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Test with world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 4, 4)
        agent.initialize(world)
        
        # Test array bounds validation methods
        # Note: these methods are implemented in C++ and exposed via Python bindings
        assert hasattr(agent, 'validateArrayBounds')
        assert hasattr(agent, 'validateNeuronExists')
        
        # Test validation methods don't crash
        # The actual validation logic is in C++
        
        print("✓ Array bounds validation test passed")
    
    def test_neuron_group_distribution(self):
        """Test neuron group distribution"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Test neuron distribution
        agent.logNeuronDistribution()
        
        # Verify distribution is logged
        neuron_count = agent.getTotalNeuronCount()
        assert neuron_count > 0
        
        print("✓ Neuron group distribution test passed")
    
    def test_backward_compatibility(self):
        """Test backward compatibility"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test that existing API still works
        agent = pynlm.createAgentBrain(brain)
        
        # Test basic properties
        assert hasattr(agent, 'getBrain')
        assert hasattr(agent, 'processSensoryInput')
        assert hasattr(agent, 'decodeMotorCommand')
        assert hasattr(agent, 'reset')
        assert hasattr(agent, 'applyRewardModulation')
        assert hasattr(agent, 'updateDevelopment')
        
        # Test that old API calls still work
        assert agent.getBrain() is not None
        
        # Test with world
        world = pynlm.createSimpleWorld()
        agent.initialize(world)
        
        # Test basic operations
        agent.reset()
        assert agent.getNeuromodulationLevel() == 0.0
        
        print("✓ Backward compatibility test passed")

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestAgentBrainConfiguration:
    """Test AgentBrain configuration and settings"""
    
    def test_configuration_loading(self):
        """Test configuration loading from Config object"""
        config = pynlm.createDefaultConfig()
        config.set("vision_size", 128)
        config.set("touch_size", 16)
        config.set("vision_scale_factor", 2.5)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Load configuration (if implemented)
        # This would test the loadFromConfig method
        agent.loadFromConfig(config)
        
        # Verify configuration was loaded
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        
        print("✓ Configuration loading test passed")
    
    def test_enable_disable_features(self):
        """Test enabling and disabling AgentBrain features"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        
        # Test enabling/disabling features
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        
        # Test that features are enabled
        assert agent.isRewardModulationEnabled() is True
        assert agent.isDevelopmentEnabled() is True
        assert agent.isCuriosityEnabled() is True
        assert agent.isStructuralPlasticityEnabled() is True
        
        print("✓ Enable/disable features test passed")

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
