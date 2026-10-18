"""
Test suite for enhanced NLM Python bindings.

This test suite provides comprehensive testing for the enhanced Python API,
including unit tests, integration tests, and performance tests.
"""

import pytest
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

# Try to import NLM bindings
try:
    from nlm import (
        Brain, Config, SimpleWorld, AgentBrain,
        AdvancedActions, ActionSequence, ActionTemplate,
        AdvancedVisualization, VisualizationConfig,
        AdvancedConfig, DynamicConfigManager,
        AdvancedLearning, LearningConfig,
        ExperimentManager
    )
    from nlm import (
        NLMError, ConfigurationError, SimulationError,
        LearningError, VisualizationError
    )
    from nlm import (
        create_brain, create_default_config, create_simple_world,
        create_agent_brain, brain_context, action_context, sequence_context
    )
    from nlm import (
        NeuronType, SynapseType, DevelopmentalStage,
        ActionType, WorldObjectType
    )
    NLM_AVAILABLE = True
except ImportError as e:
    NLM_AVAILABLE = False
    IMPORT_ERROR = e

# Skip all tests if NLM is not available
pytestmark = pytest.mark.skipif(
    not NLM_AVAILABLE,
    reason=f"NLM bindings not available: {IMPORT_ERROR}"
)

if NLM_AVAILABLE:
    import numpy as np
    import time

    class TestCoreBindings:
        """Test core NLM bindings and basic functionality."""
        
        def test_config_creation(self):
            """Test configuration creation and basic operations."""
            config = create_default_config()
            
            # Test that config is created successfully
            assert config is not None
            assert hasattr(config, 'summary')
            
            # Test getting and setting values
            config.set("test.key", "test_value")
            assert config.get("test.key") == "test_value"
            assert "test.key" in config
            
            # Test removing keys
            config.remove("test.key")
            assert "test.key" not in config
        
        def test_brain_creation(self):
            """Test brain creation and basic operations."""
            config = create_default_config()
            brain = create_brain(config)
            
            # Test brain creation
            assert brain is not None
            assert hasattr(brain, 'neuron_count')
            assert hasattr(brain, 'region_count')
            
            # Test brain initialization
            assert brain.initialize()
            
            # Test brain properties
            assert brain.neuron_count >= 0
            assert brain.region_count >= 0
            assert brain.synapse_count >= 0
            assert brain.active_neuron_count >= 0
            assert brain.firing_neuron_count >= 0
            
            # Test reset
            brain.reset()
            assert not brain.initialized
        
        def test_world_creation(self):
            """Test world creation and basic operations."""
            world = create_simple_world()
            
            # Test world creation
            assert world is not None
            assert hasattr(world, 'width')
            assert hasattr(world, 'height')
            
            # Test world configuration
            world.configure(100.0, 100.0, 64, 64)
            assert world.width == 100.0
            assert world.height == 100.0
            
            # Test world reset
            world.reset()
        
        def test_agent_brain_creation(self):
            """Test agent brain creation and basic operations."""
            config = create_default_config()
            brain = create_brain(config)
            world = create_simple_world()
            
            # Create agent brain
            agent_brain = create_agent_brain(brain)
            
            # Test agent brain creation
            assert agent_brain is not None
            assert hasattr(agent_brain, 'sensory_input_size')
            assert hasattr(agent_brain, 'motor_output_size')
            
            # Test agent brain properties
            assert agent_brain.sensory_input_size >= 0
            assert agent_brain.motor_output_size >= 0
            assert agent_brain.neuromodulation_level >= 0.0
            assert agent_brain.curiosity_level >= 0.0
            assert agent_brain.novelty_level >= 0.0
            assert agent_brain.prediction_error >= 0.0
        
        def test_enums(self):
            """Test enumeration types."""
            # Test NeuronType
            assert NeuronType.Excitatory is not None
            assert NeuronType.Inhibitory is not None
            assert NeuronType.Modulatory is not None
            
            # Test SynapseType
            assert SynapseType.Excitatory is not None
            assert SynapseType.Inhibitory is not None
            
            # Test ActionType
            assert ActionType.MoveForward is not None
            assert ActionType.TurnLeft is not None
            assert ActionType.Interact is not None
            
            # Test WorldObjectType
            assert WorldObjectType.Empty is not None
            assert WorldObjectType.Resource is not None
            
            # Test DevelopmentalStage
            assert DevelopmentalStage.Initial is not None
            assert DevelopmentalStage.Adult is not None
        
        def test_exception_hierarchy(self):
            """Test exception hierarchy."""
            # Test base NLM error
            error = NLMError("Test error")
            assert error.message == "Test error"
            assert isinstance(error, Exception)
            
            # Test specific exceptions
            config_error = ConfigurationError("Config error")
            assert isinstance(config_error, NLMError)
            
            sim_error = SimulationError("Simulation error")
            assert isinstance(sim_error, NLMError)
            
            learn_error = LearningError("Learning error")
            assert isinstance(learn_error, NLMError)
            
            viz_error = VisualizationError("Viz error")
            assert isinstance(viz_error, NLMError)
    
    class TestPythonicAPI:
        """Test Pythonic API features including context managers and properties."""
        
        def test_brain_context_manager(self):
            """Test brain context manager."""
            config = create_default_config()
            
            with brain_context(config) as brain:
                assert brain.initialized
                assert brain.neuron_count >= 0
                # Brain should be reset when exiting context
            
            # Brain should be reset after context exit
            assert not brain.initialized
        
        def test_action_context_manager(self):
            """Test action context manager."""
            with action_context(ActionType.MoveForward) as action:
                assert action is not None
                assert action.action_type == ActionType.MoveForward
        
        def test_sequence_context_manager(self):
            """Test sequence context manager."""
            with sequence_context("TestSequence") as sequence:
                assert sequence is not None
                assert sequence.name == "TestSequence"
                assert sequence.action_count == 0
        
        def test_property_access(self):
            """Test property-based access."""
            config = create_default_config()
            brain = create_brain(config)
            
            # Test brain property access
            brain.initialize()
            
            # Access properties multiple times
            count1 = brain.neuron_count
            count2 = brain.neuron_count
            assert count1 == count2
            
            # Test developmental stage property
            stage1 = brain.developmental_stage
            brain.developmental_stage = DevelopmentalStage.Adult
            stage2 = brain.developmental_stage
            assert stage1 != stage2
        
        def test_string_representations(self):
            """Test string representations."""
            config = create_default_config()
            brain = create_brain(config)
            brain.initialize()
            
            # Test __repr__
            repr_str = repr(brain)
            assert isinstance(repr_str, str)
            assert "neuron_count" in repr_str
            
            # Test __str__
            str_str = str(brain)
            assert isinstance(str_str, str)
            assert "Brain" in str_str
            
            # Test config string representation
            config_repr = repr(config)
            assert isinstance(config_repr, str)
            assert "Config:" in config_repr
    
    class TestAdvancedFeatures:
        """Test advanced features like advanced actions and visualization."""
        
        def test_advanced_actions(self):
            """Test advanced actions system."""
            advanced_actions = AdvancedActions()
            
            # Test action template creation
            template = ActionTemplate("TestTemplate", "Test description")
            assert template is not None
            assert template.get_name() == "TestTemplate"
            
            # Test action sequence creation
            sequence = ActionSequence("TestSequence")
            assert sequence is not None
            assert sequence.get_name() == "TestSequence"
            assert sequence.get_action_count() == 0
        
        def test_advanced_visualization(self):
            """Test advanced visualization system."""
            viz_config = VisualizationConfig()
            assert viz_config is not None
            assert hasattr(viz_config, 'backend')
            assert hasattr(viz_config, 'enable_real_time')
            
            # Test visualization config properties
            assert viz_config.backend is not None
            assert isinstance(viz_config.enable_real_time, bool)
        
        def test_advanced_config(self):
            """Test advanced configuration system."""
            config = AdvancedConfig()
            assert config is not None
            assert hasattr(config, 'validate_config')
            assert hasattr(config, 'get_config_errors')
        
        def test_advanced_learning(self):
            """Test advanced learning system."""
            learning_config = LearningConfig()
            assert learning_config is not None
            assert hasattr(learning_config, 'learning_algorithm')
            assert hasattr(learning_config, 'learning_rate')
        
        def test_dynamic_config_manager(self):
            """Test dynamic configuration manager."""
            config_manager = DynamicConfigManager()
            assert config_manager is not None
            assert hasattr(config_manager, 'update_config')
            assert hasattr(config_manager, 'get_changes')
    
    class TestErrorHandling:
        """Test error handling and edge cases."""
        
        def test_invalid_config_key(self):
            """Test handling of invalid configuration keys."""
            config = create_default_config()
            
            # Test getting non-existent key
            result = config.get("non.existent.key", "default")
            assert result == "default"
        
        def test_invalid_brain_operation(self):
            """Test handling of invalid brain operations."""
            config = create_default_config()
            brain = create_brain(config)
            
            # Try to reset uninitialized brain
            brain.reset()  # Should not raise an exception
            
            # Try to get region before adding one
            try:
                from nlm import RegionId
                invalid_region = RegionId(999999)
                brain.get_region(invalid_region)
                # If we get here, the method completed (might return None or throw)
            except Exception:
                pass  # Expected to fail
        
        def test_memory_management(self):
            """Test memory management and resource cleanup."""
            import gc
            
            # Create and delete objects to test memory management
            for i in range(10):
                config = create_default_config()
                brain = create_brain(config)
                del brain
                del config
            
            # Force garbage collection
            gc.collect()
            
            # If we get here without errors, memory management worked
            assert True
    
    class TestIntegration:
        """Test integration between different components."""
        
        def test_brain_world_integration(self):
            """Test integration between brain and world."""
            config = create_default_config()
            brain = create_brain(config)
            world = create_simple_world()
            agent_brain = create_agent_brain(brain)
            
            # Initialize all components
            brain.initialize()
            
            # Test world configuration
            world.configure(100.0, 100.0, 64, 64)
            world.reset()
            
            # Set agent start position
            world.set_agent_start(50.0, 50.0)
            
            # Test that components can interact
            world.update(1)
            
            # Get world state
            body = world.get_agent_body()
            assert body is not None
        
        def test_action_sequence_integration(self):
            """Test action sequence integration with advanced actions."""
            advanced_actions = AdvancedActions()
            
            # Create action sequence
            sequence = ActionSequence("IntegrationTest")
            
            # Add actions to sequence
            for i in range(5):
                action = Action(ActionType.MoveForward)
                sequence.add_action(action)
            
            # Verify sequence
            assert sequence.get_action_count() == 5
            
            # Execute sequence
            results = sequence.execute()
            assert len(results) == 5
    
    class TestPerformance:
        """Performance tests for NLM bindings."""
        
        def test_config_creation_performance(self):
            """Test performance of config creation."""
            import time
            
            start_time = time.time()
            
            # Create multiple configs
            for i in range(100):
                config = create_default_config()
                config.set(f"test.key{i}", f"value{i}")
            
            end_time = time.time()
            elapsed = end_time - start_time
            
            # Should be fast (less than 1 second for 100 configs)
            assert elapsed < 1.0, f"Config creation too slow: {elapsed:.2f}s"
        
        def test_brain_operation_performance(self):
            """Test performance of brain operations."""
            import time
            
            config = create_default_config()
            brain = create_brain(config)
            brain.initialize()
            
            # Test performance of property access
            start_time = time.time()
            
            for i in range(1000):
                _ = brain.neuron_count
                _ = brain.synapse_count
                _ = brain.active_neuron_count
            
            end_time = time.time()
            elapsed = end_time - start_time
            
            # Should be fast (less than 1 second for 3000 property accesses)
            assert elapsed < 1.0, f"Property access too slow: {elapsed:.2f}s"

# Additional test functions for backward compatibility
def test_backward_compatibility():
    """Test backward compatibility with existing NLM usage patterns."""
    if not NLM_AVAILABLE:
        pytest.skip("NLM not available")
    
    # Test that all the original factory functions still work
    config = create_default_config()
    brain = create_brain(config)
    world = create_simple_world()
    
    assert config is not None
    assert brain is not None
    assert world is not None
    
    # Test that basic functionality still works
    brain.initialize()
    brain.reset()
    world.reset()

if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v", "--tb=short"])
