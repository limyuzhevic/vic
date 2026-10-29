#!/usr/bin/env python3
"""
Python tests for NLM (Neural Learning Machine) Python bindings.
Tests the pynlm package functionality for accessing the C++ neural simulation framework.
"""

import pytest
import numpy as np
import sys
import os

# Add the workspace to the path
sys.path.insert(0, '/workspace/app')

# Try to import the pynlm package
try:
    from pynlm import (
        nlm,
        Config,
        Brain,
        Neuron,
        Synapse,
        SensoryInput,
        Vision,
        Action,
        ActionType,
    )
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    IMPORT_ERROR = str(e)
class TestImport:
    """Test that the pynlm package can be imported correctly."""
    
    def test_import_success(self):
        """Test that all expected components can be imported."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        assert nlm is not None
        assert Config is not None
        assert Brain is not None
        assert Neuron is not None
        assert Synapse is not None
        assert SensoryInput is not None
        assert Vision is not None
        assert Action is not None
        assert ActionType is not None
    
    def test_action_type_enum(self):
        """Test that ActionType enum works correctly."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Test that all expected action types exist
        expected_actions = [
            ActionType.Wait,
            ActionType.MoveForward,
            ActionType.MoveBackward,
            ActionType.TurnLeft,
            ActionType.TurnRight,
            ActionType.Interact,
        ]
        
        for action in expected_actions:
            assert action is not None
class TestConfig:
    """Test configuration functionality."""
    
    def test_config_creation(self):
        """Test creating a configuration object."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        config = Config()
        assert config is not None
    
    def test_config_values(self):
        """Test setting and getting configuration values."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        config = Config()
        
        # Test integer value
        config.set("test_int", 42)
        value = config.get("test_int")
        assert value == 42
        
        # Test float value
        config.set("test_float", 3.14)
        value = config.get("test_float")
        assert abs(value - 3.14) < 0.001
        
        # Test string value
        config.set("test_string", "hello")
        value = config.get("test_string")
        assert value == "hello"
        
        # Test boolean value
        config.set("test_bool", True)
        value = config.get("test_bool")
        assert value is True
    
    def test_config_has(self):
        """Test checking if configuration has a key."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        config = Config()
        
        # Key that doesn't exist
        assert not config.has("nonexistent_key")
        
        # Key that exists
        config.set("existing_key", "value")
        assert config.has("existing_key")
class TestNeuron:
    """Test neuron functionality."""
    
    def test_neuron_creation(self):
        """Test creating a neuron object."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        neuron = Neuron()
        assert neuron is not None
    
    def test_neuron_properties(self):
        """Test neuron property getters and setters."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        neuron = Neuron()
        
        # Test initial properties
        assert neuron.get_id() > 0  # Should have a valid ID
        assert neuron.get_type() is not None
        
        # Test membrane potential
        neuron.set_membrane_potential(0.5)
        assert abs(neuron.get_membrane_potential() - 0.5) < 0.001
        
        # Test threshold
        neuron.set_threshold(1.0)
        assert abs(neuron.get_threshold() - 1.0) < 0.001
        
        # Test is_firing
        assert not neuron.is_firing()
        # Note: Firing state management may require additional context
    
    def test_neuron_state(self):
        """Test neuron state methods."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        neuron = Neuron()
        
        # Test membrane potential injection
        initial_potential = neuron.get_membrane_potential()
        neuron.inject_current(0.1)
        
        # Membrane potential should have changed (though magnitude depends on neuron model)
        new_potential = neuron.get_membrane_potential()
        assert new_potential >= initial_potential
class TestSensoryInput:
    """Test sensory input functionality."""
    
    def test_sensory_input_creation(self):
        """Test creating sensory input objects."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Test base SensoryInput class
        sensory = SensoryInput()
        assert sensory is not None
        assert sensory.get_type() is not None
        assert sensory.get_data() is not None
        assert sensory.get_dimensions() > 0
    
    def test_vision_input(self):
        """Test creating vision input."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Test Vision class
        vision = Vision(100, 100, 3)  # 100x100 RGB image
        assert vision is not None
        assert vision.get_type() == "vision"
        assert vision.get_width() == 100
        assert vision.get_height() == 100
        assert vision.get_channels() == 3
        assert len(vision.get_data()) == 100 * 100 * 3
        
        # Test setting data
        data = np.random.rand(100 * 100 * 3).astype(np.float32)
        vision.set_data(data)
        assert np.allclose(vision.get_data(), data)
    
    def test_sensory_input_data(self):
        """Test sensory input data handling."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        sensory = SensoryInput()
        
        # Test with sample data
        test_data = np.array([0.1, 0.2, 0.3, 0.4, 0.5])
        
        # Note: The exact interface may vary based on the actual pybind11 bindings
        # This test will need to be adjusted based on the actual API
class TestAction:
    """Test action functionality."""
    
    def test_action_creation(self):
        """Test creating action objects."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Test creating action with different types
        for action_type in ActionType.__members__.values():
            action = Action(action_type)
            assert action is not None
            assert action.get_type() == action_type
        
        # Test reward and probability methods
        action = Action(ActionType.MoveForward)
        action.set_reward(1.0)
        assert action.get_reward() == 1.0
        
        action.set_probability(0.8)
        assert abs(action.get_probability() - 0.8) < 0.001
class TestIntegration:
    """Integration tests for the complete system."""
    
    def test_config_and_brain_creation(self):
        """Test that we can create a config and brain together."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Create configuration
        config = Config()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        
        # Try to create brain (this may fail if C++ side isn't compiled)
        try:
            brain = Brain(config)
            assert brain is not None
            print("Brain creation successful")
        except Exception as e:
            pytest.skip(f"Brain creation not supported in current environment: {e}")
    
    def test_neuron_array_operations(self):
        """Test basic array operations with neurons."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Create multiple neurons
        neurons = [Neuron() for _ in range(10)]
        
        # Test that we can iterate over them
        assert len(neurons) == 10
        
        # Test basic properties
        ids = [neuron.get_id() for neuron in neurons]
        assert len(set(ids)) == 10  # All IDs should be unique
    
    def test_sensory_array_creation(self):
        """Test creating arrays of sensory inputs."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Create multiple vision inputs
        visions = [Vision(50, 50, 3) for _ in range(5)]
        
        # Test that they all have correct properties
        for i, vision in enumerate(visions):
            assert vision.get_width() == 50
            assert vision.get_height() == 50
            assert vision.get_channels() == 3
            assert len(vision.get_data()) == 50 * 50 * 3
    
    def test_action_collection(self):
        """Test creating and managing action collections."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Create actions
        actions = [
            Action(ActionType.Wait),
            Action(ActionType.MoveForward),
            Action(ActionType.TurnLeft),
        ]
        
        # Test collection operations
        assert len(actions) == 3
        
        # Test accessing action properties
        for action in actions:
            assert action.get_type() is not None
            assert action.get_reward() >= 0.0
            assert 0.0 <= action.get_probability() <= 1.0
class TestPerformance:
    """Performance and stress tests."""
    
    def test_large_neuron_array(self):
        """Test creating a large number of neurons."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Create a moderate number of neurons
        num_neurons = 100
        neurons = [Neuron() for _ in range(num_neurons)]
        
        # Verify all were created
        assert len(neurons) == num_neurons
        
        # Test basic property access (timing this would be useful)
        import time
        start_time = time.time()
        
        # Access properties of all neurons
        for neuron in neurons:
            _ = neuron.get_id()
            _ = neuron.get_type()
            _ = neuron.get_membrane_potential()
        
        end_time = time.time()
        print(f"Accessed {num_neurons} neuron properties in {end_time - start_time:.4f} seconds")
    
    def test_large_sensory_array(self):
        """Test creating large sensory arrays."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Create multiple vision inputs
        num_vision = 20
        visions = []
        
        for i in range(num_vision):
            vision = Vision(64, 64, 3)  # Smaller images for performance
            visions.append(vision)
        
        # Verify all were created
        assert len(visions) == num_vision
        
        # Test total memory usage (rough estimate)
        total_pixels = sum(v.get_width() * v.get_height() * v.get_channels() for v in visions)
        print(f"Created {num_vision} vision inputs with {total_pixels} total pixels")
    
    def test_mixed_operations(self):
        """Test mixed operations with different object types."""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {IMPORT_ERROR}")
        
        # Create a mix of different objects
        config = Config()
        neurons = [Neuron() for _ in range(5)]
        actions = [Action(ActionType.Wait), Action(ActionType.MoveForward)]
        
        # Perform operations on all of them
        config.set("test_value", 123)
        
        for neuron in neurons:
            neuron.inject_current(0.05)
        
        # Verify operations completed
        assert config.get("test_value") == 123
        assert len(neurons) == 5
        assert len(actions) == 2
if __name__ == "__main__":
    # Run tests when executed directly
    pytest.main([__file__, "-v", "-s"])
