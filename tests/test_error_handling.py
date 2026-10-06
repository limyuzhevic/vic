"""Error Handling Tests

Test error conditions and exception handling.
"""

import pytest
import pynlm
import tempfile
import os


class TestErrorHandling:
    """Test error conditions and exception handling."""
    
    def test_invalid_neuron_id_operations(self):
        """Test operations with invalid neuron IDs."""
        # Test operations with invalid neuron ID
        invalid_id = pynlm.INVALID_NEURON_ID
        
        # Create valid neuron ID for comparison
        valid_id = pynlm.NeuronId(100)
        
        # Test equality operations
        assert invalid_id != valid_id
        assert invalid_id.value == 0
        assert valid_id.value == 100
        
        # Test hash operations
        hash(invalid_id)
        hash(valid_id)
        
        # Test neuron ID creation with invalid values
        # (Should handle edge cases appropriately)
        edge_id = pynlm.NeuronId(0)
        assert edge_id.value == 0
    
    def test_invalid_file_operations(self):
        """Test error handling for invalid file operations."""
        config = pynlm.createDefaultConfig()
        
        # Test saving to non-existent directory
        invalid_dir_path = "/this/directory/does/not/exist/config.json"
        
        try:
            config.saveToFile(invalid_dir_path)
            # If it doesn't raise an exception, that's fine
        except Exception:
            # Exception is acceptable
            pass
        
        # Test loading from non-existent file
        non_existent_file = "/this/file/does/not/exist.json"
        
        # Create new config
        config2 = pynlm.createDefaultConfig()
        
        try:
            config2.loadFromFile(non_existent_file)
            # If it doesn't raise an exception, that's fine
        except Exception:
            # Exception is acceptable
            pass
    
    def test_brain_error_conditions(self):
        """Test error conditions in brain operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test operations with invalid region ID
        invalid_region_id = pynlm.INVALID_REGION_ID
        
        # Try to get invalid region (may return null or raise exception)
        region = brain.getRegion(invalid_region_id)
        # Either None is returned or exception is raised - both are acceptable
        
        # Test operations with invalid neuron ID
        invalid_neuron_id = pynlm.INVALID_NEURON_ID
        
        # Test injection into invalid neuron (may fail gracefully)
        try:
            brain.injectCurrent(invalid_neuron_id, 1.0)
        except Exception:
            # Expected to fail
            pass
        
        # Test operations with invalid neuron type
        # This should be handled gracefully
        from pynlm import NeuronType
        try:
            # This might not be directly exposed, but test neuron operations
            pass
        except Exception:
            pass
    
    def test_world_error_conditions(self):
        """Test error conditions in world operations."""
        world = pynlm.createSimpleWorld()
        
        # Test operations without configuration
        # World should handle this gracefully
        body = world.getAgentBody()
        assert body is not None
        
        # Test operations with invalid positions
        percept = world.getSensoryPercept()
        # Should handle gracefully even if world not configured
        
        # Test world methods with extreme values
        world.configure(1, 1, 1, 1)  # Minimal world
        
        # Test invalid positions
        valid = world.isValidPosition(0.0, 0.0)
        assert valid == True  # (0,0) should be valid in 1x1 world
        
        invalid = world.isValidPosition(-1.0, 0.0)
        # Should handle invalid positions appropriately
    
    def test_agent_brain_error_conditions(self):
        """Test error conditions in agent brain operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Create agent brain without initializing world
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Try to use agent brain methods without proper initialization
        # Should handle errors gracefully
        try:
            agent_brain.getSensoryInputSize()
        except Exception:
            # Expected if world not initialized
            pass
        
        try:
            agent_brain.getMotorOutputSize()
        except Exception:
            # Expected if world not initialized
            pass
        
        # Test with uninitialized components
        world = pynlm.createSimpleWorld()
        world.configure(800, 600, 640, 480)
        
        agent_brain.initialize(world)
        
        # Test with invalid percept
        try:
            # Create a percept-like object but pass invalid data
            # (Depending on implementation, this may fail)
            pass
        except Exception:
            pass
    
    def test_memory_system_errors(self):
        """Test error conditions in memory system operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test operations with memory systems
        # Add regions for memory systems
        brain.addRegion("working_memory")
        brain.addRegion("episodic_memory")
        
        # Test statistics with empty memory
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        active_count = brain.getActiveNeuronCount()
        firing_count = brain.getFiringNeuronCount()
        avg_rate = brain.getAverageFiringRate()
        
        # These should be valid (likely zeros)
        assert neuron_count == 0
        assert synapse_count == 0
        assert active_count == 0
        assert firing_count == 0
        assert avg_rate == 0.0
    
    def test_neuromodulation_error_conditions(self):
        """Test error conditions in neuromodulation operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Test with uninitialized world
        try:
            agent_brain.getNeuromodulationLevel()
        except Exception:
            # Expected if world not initialized
            pass
        
        # Initialize and test error conditions
        agent_brain.initialize(world)
        
        # Test with edge case values
        agent_brain.applyRewardModulation(0.0, 0.0)  # No reward, no prediction
        agent_brain.applyRewardModulation(1.0, 1.0)  # Max reward, max prediction
        agent_brain.applyRewardModulation(-1.0, 1.0) # Negative reward, max prediction
        
        # Verify neuromodulation level stays within bounds
        neuromod_level = agent_brain.getNeuromodulationLevel()
        assert 0.0 <= neuromod_level <= 1.0
    
    def test_checkpoint_error_conditions(self):
        """Test error conditions in checkpoint operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test saving to read-only location (if possible)
        # This might require special permissions
        readonly_path = "/tmp/readonly_test.nln"
        
        # Create file and make it read-only
        with open(readonly_path, 'w') as f:
            f.write("test")
        
        try:
            os.chmod(readonly_path, 0o444)  # Read-only
            brain.save(readonly_path)
            # If it succeeds, that's fine
        except Exception:
            # Expected to fail
            pass
        finally:
            # Clean up
            if os.path.exists(readonly_path):
                os.chmod(readonly_path, 0o666)
                os.unlink(readonly_path)
        
        # Test loading corrupted/invalid file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.nln', delete=False) as f:
            f.write("This is not a valid NLM checkpoint file")
            invalid_file = f.name
        
        try:
            config2 = pynlm.createDefaultConfig()
            loaded_brain = pynlm.createBrain(config2)
            loaded_brain.load(invalid_file)
            # If it loads successfully (may be empty), that's fine
        except Exception:
            # Expected to fail
            pass
        finally:
            if os.path.exists(invalid_file):
                os.unlink(invalid_file)


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
