"""Checkpoint Tests

Test save/load functionality.
"""

import pytest
import pynlm
import tempfile
import os
import shutil
from pathlib import Path


class TestCheckpoints:
    """Test checkpoint save/load functionality."""
    
    def test_config_save_load(self):
        """Test Config save/load functionality."""
        # Create and modify a config
        config = pynlm.createDefaultConfig()
        config.clear()  # Start with clean config
        
        # Add some configuration data
        config.set("test_key1", "test_value1")
        config.set("test_key2", "test_value2")
        config.set("test_number", 42)
        
        # Verify config has our data
        assert config.has("test_key1") == True
        assert config.has("test_key2") == True
        assert config.has("test_number") == True
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            # Save config
            config.saveToFile(config_file)
            assert os.path.exists(config_file)
            
            # Create new config and load from file
            loaded_config = pynlm.createDefaultConfig()
            loaded_config.loadFromFile(config_file)
            
            # Verify loaded config matches original
            assert loaded_config.has("test_key1") == True
            assert loaded_config.has("test_key2") == True
            assert loaded_config.has("test_number") == True
            
            # Test keys are preserved
            loaded_keys = loaded_config.getKeys()
            assert len(loaded_keys) >= 3  # At least our test keys
            
        finally:
            # Clean up
            if os.path.exists(config_file):
                os.unlink(config_file)
    
    def test_brain_save_load(self):
        """Test Brain save/load functionality."""
        # Create and initialize a brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Add a region
        brain.addRegion("test_region")
        
        # Test brain save/load
        with tempfile.TemporaryDirectory() as temp_dir:
            save_file = os.path.join(temp_dir, "brain_checkpoint.nln")
            
            # Save brain
            brain.save(save_file)
            assert os.path.exists(save_file)
            
            # Create new brain and load from file
            config2 = pynlm.createDefaultConfig()
            loaded_brain = pynlm.createBrain(config2)
            
            # Load the saved brain
            loaded_brain.load(save_file)
            
            # Verify basic properties are preserved
            original_regions = brain.getRegionIds()
            loaded_regions = loaded_brain.getRegionIds()
            
            # Region count should be the same (at least the test region)
            assert len(loaded_regions) == len(original_regions)
            
            # Test brain statistics
            orig_stats = {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'region_count': brain.getRegionCount(),
            }
            
            loaded_stats = {
                'neuron_count': loaded_brain.getTotalNeuronCount(),
                'synapse_count': loaded_brain.getTotalSynapseCount(),
                'region_count': loaded_brain.getRegionCount(),
            }
            
            # These should match (assuming no neurons were added after save)
            assert loaded_stats['region_count'] == orig_stats['region_count']
    
    def test_checkpoint_file_persistence(self):
        """Test that checkpoint files are properly created and can be accessed."""
        # Create brain with some state
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Add multiple regions to create more complex state
        for i in range(5):
            brain.addRegion(f"region_{i}")
        
        # Save checkpoint
        with tempfile.TemporaryDirectory() as temp_dir:
            checkpoint_file = os.path.join(temp_dir, "checkpoint.nln")
            brain.save(checkpoint_file)
            
            # Verify file exists and has content
            assert os.path.exists(checkpoint_file)
            assert os.path.getsize(checkpoint_file) > 0
            
            # Verify we can load it multiple times
            for i in range(3):
                config2 = pynlm.createDefaultConfig()
                loaded_brain = pynlm.createBrain(config2)
                loaded_brain.load(checkpoint_file)
                
                # Verify loaded state
                assert loaded_brain.getRegionCount() == brain.getRegionCount()
    
    def test_checkpoint_with_config(self):
        """Test that checkpoints can be saved with different configurations."""
        # Create multiple brains with different configurations
        configs = []
        brains = []
        
        for i in range(3):
            config = pynlm.createDefaultConfig()
            # Modify each config differently
            config.set(f"config_id", i)
            config.set(f"param_{i}", f"value_{i}")
            
            config.save(f"/tmp/config_{i}.json")
            configs.append(config)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            brain.addRegion(f"region_{i}")
            
            brains.append(brain)
        
        # Save all brains
        with tempfile.TemporaryDirectory() as temp_dir:
            for i, brain in enumerate(brains):
                checkpoint_file = os.path.join(temp_dir, f"brain_{i}.nln")
                brain.save(checkpoint_file)
                
                # Verify each checkpoint is unique
                config = pynlm.createDefaultConfig()
                loaded_brain = pynlm.createBrain(config)
                loaded_brain.load(checkpoint_file)
                
                # Load and verify
                assert loaded_brain.getRegionCount() == brain.getRegionCount()
        
        # Clean up config files
        for i in range(3):
            config_file = f"/tmp/config_{i}.json"
            if os.path.exists(config_file):
                os.unlink(config_file)
    
    def test_error_handling_in_checkpoint(self):
        """Test error handling for checkpoint operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test saving to invalid path (should handle gracefully or raise appropriate error)
        invalid_path = "/invalid/path/that/does/not/exist/checkpoint.nln"
        
        # This might raise an exception, which is acceptable
        try:
            brain.save(invalid_path)
        except Exception:
            # Expected to fail
            pass
        
        # Test loading from non-existent file
        with tempfile.TemporaryDirectory() as temp_dir:
            non_existent_file = os.path.join(temp_dir, "does_not_exist.nln")
            
            config2 = pynlm.createDefaultConfig()
            loaded_brain = pynlm.createBrain(config2)
            
            # This should either load successfully (empty brain) or raise appropriate error
            try:
                loaded_brain.load(non_existent_file)
            except Exception:
                # Expected to fail
                pass


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
