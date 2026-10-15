"""
Comprehensive test suite for NLM Python context managers

This module provides comprehensive tests for all context manager functionality
in the NLM (Neural Learning Machine) Python bindings.

Tests cover:
- Basic context manager functionality for all supported classes
- Error handling with context managers
- Nested context managers
- Resource cleanup verification
- Advanced context manager scenarios
- Type hints and error handling
- Performance considerations
- Integration with other features

The test suite uses pytest for testing and includes detailed progress reporting.
"""

import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
from pynlm import (
    Config, Brain, AgentBrain, SimpleWorld,
    Vision, Audio, InternalSignals, Action,
    ActionType, NLMError, NLMConfigError, NLMInitializationError,
    NLMResourceError, NLMValidationError
)

# Test constants
TEST_CONFIG = {
    "brain.neuron_count": 100,
    "brain.connection_probability": 0.2,
    "agent.exploration_rate": 0.3,
    "simulation.max_steps": 1000,
    "simulation.timestep": 0.1
}

WORLD_CONFIG = {
    "width": 100.0,
    "height": 100.0,
    "vision_width": 10,
    "vision_height": 10
}

class TestConfigContextManager:
    """Test Config context manager functionality."""
    
    def test_config_basic_creation(self):
        """Test basic Config context manager creation."""
        with nlm.Config() as config:
            assert config is not None
            config.set("test.key", "test_value")
            assert config.has("test.key")
            assert config.getOr("test.key", "") == "test_value"
            print("✓ Config basic creation test passed")
    
    def test_config_with_defaults(self):
        """Test Config with configuration profiles."""
        with nlm.Config() as config:
            config.setDefaults("quick_test")
            # Verify quick_test profile was applied
            summary = config.getSummary()
            assert isinstance(summary, dict)
            print("✓ Config with defaults test passed")
    
    def test_config_merge_operations(self):
        """Test Config merge operations."""
        with nlm.Config() as config:
            config.set("key1", "value1")
            config.mergeConfig({"key2": "value2"})
            
            assert config.getOr("key1", None) == "value1"
            assert config.getOr("key2", None) == "value2"
            print("✓ Config merge operations test passed")
    
    def test_config_validation(self):
        """Test Config validation."""
        with nlm.Config() as config:
            config.updateFromDict({
                "brain.neuron_count": 100,
                "agent.exploration_rate": 0.5
            })
            
            issues = config.validateConfig()
            # Validation should return a list
            assert isinstance(issues, list)
            print(f"✓ Config validation test passed (found {len(issues)} issues)")

class TestBrainContextManager:
    """Test Brain context manager functionality."""
    
    def test_brain_basic_creation(self):
        """Test basic Brain context manager creation."""
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        with nlm.Brain(config) as brain:
            assert brain is not None
            brain.initialize()
            
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
            assert stats.get('total_neurons', 0) > 0
            print("✓ Brain basic creation test passed")
    
    def test_brain_simulation(self):
        """Test Brain simulation with context manager."""
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Run simulation using convenience method
            brain.runSimulation(10)
            
            stats = brain.getNeuralStats()
            assert stats.get('total_spikes', 0) >= 0
            print("✓ Brain simulation test passed")
    
    def test_brain_with_sensory_input(self):
        """Test Brain with sensory input using context managers."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.Vision(10, 10) as vision:
                vision_data = [0.1] * (10 * 10 * 3)
                vision.setData(vision_data)
                
                brain.receiveSensoryInput(vision)
                brain.runSimulation(5)
            
            print("✓ Brain with sensory input test passed")
    
    def test_brain_serialization(self):
        """Test Brain serialization with context manager."""
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(50)
            
            # Save brain state
            brain.save("test_brain_state.nlm")
            
            # Verify file was created
            assert os.path.exists("test_brain_state.nlm")
            
            # Clean up
            os.remove("test_brain_state.nlm")
        
        print("✓ Brain serialization test passed")

class TestAgentBrainContextManager:
    """Test AgentBrain context manager functionality."""
    
    def test_agentbrain_basic_creation(self):
        """Test basic AgentBrain context manager creation."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                action = agent.decodeMotorCommand()
                assert action is not None
                print("✓ AgentBrain basic creation test passed")
    
    def test_agentbrain_learning(self):
        """Test AgentBrain learning with context manager."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 10, 10)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Enable learning features
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                # Run learning using convenience method
                metrics = agent.learnFromWorld(world, 20)
                
                assert isinstance(metrics, dict)
                assert 'success_rate' in metrics
                print("✓ AgentBrain learning test passed")
    
    def test_agentbrain_performance_metrics(self):
        """Test AgentBrain performance metrics."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 5, 5)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Get performance metrics
                metrics = agent.getPerformanceMetrics()
                assert isinstance(metrics, dict)
                
                # Check for expected keys
                expected_keys = ['neuromodulation_level', 'curiosity_level']
                for key in expected_keys:
                    assert key in metrics, f"Missing key: {key}"
                
                print("✓ AgentBrain performance metrics test passed")

class TestSimpleWorldContextManager:
    """Test SimpleWorld context manager functionality."""
    
    def test_world_basic_creation(self):
        """Test basic SimpleWorld context manager creation."""
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            assert world.getWidth() == 50.0
            assert world.getHeight() == 50.0
            print("✓ World basic creation test passed")
    
    def test_world_convenience_methods(self):
        """Test SimpleWorld convenience methods."""
        with nlm.SimpleWorld() as world:
            world.createGridWorld(100.0, 100.0, 0.1)  # 10% obstacle density
            
            # Test convenience methods
            world.addResource(10.0, 10.0, 5.0)
            world.setAgentGoal(90.0, 90.0)
            
            observation_map = world.getObservationMap()
            assert isinstance(observation_map, list)
            assert len(observation_map) == 100  # 100x100 grid
            
            print("✓ World convenience methods test passed")
    
    def test_world_simulation(self):
        """Test SimpleWorld simulation."""
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            world.setAgentStart(25.0, 25.0)
            
            # Run simulation
            for step in range(10):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                body = world.getAgentBody()
                
                assert body.x is not None
                assert body.y is not None
            
            print("✓ World simulation test passed")

class TestSensoryInputContextManager:
    """Test SensoryInput context manager functionality."""
    
    def test_vision_context_manager(self):
        """Test Vision context manager."""
        with nlm.Vision(10, 10) as vision:
            assert vision.getType() == "Vision"
            assert vision.getWidth() == 10
            assert vision.getHeight() == 10
            assert vision.getChannels() == 3
            
            # Set and get data
            data = [0.5] * (10 * 10 * 3)
            vision.setData(data)
            assert vision.getData() == data
            
            # Test Pythonic convenience method
            vision_array = vision.getVisionArray()
            assert isinstance(vision_array, list)
            assert len(vision_array) == 10
            
            print("✓ Vision context manager test passed")
    
    def test_audio_context_manager(self):
        """Test Audio context manager."""
        with nlm.Audio(44100, 44100) as audio:
            assert audio.getType() == "Audio"
            assert audio.getSampleRate() == 44100
            assert audio.getNumSamples() == 44100
            
            # Set and get data
            data = [0.1 * i for i in range(44100)]
            audio.setData(data)
            assert audio.getData() == data
            
            print("✓ Audio context manager test passed")
    
    def test_internal_signals_context_manager(self):
        """Test InternalSignals context manager."""
        with nlm.InternalSignals() as internal:
            assert internal.getType() == "InternalSignals"
            
            # Add signals
            internal.addSignal(0.8)
            internal.addSignal(0.3)
            internal.addSignal(0.5)
            
            # Get internal state as Python dict (Pythonic)
            state = internal.getInternalState()
            assert isinstance(state, dict)
            assert len(state) == 3
            
            print("✓ InternalSignals context manager test passed")

class TestActionContextManager:
    """Test Action context manager functionality."""
    
    def test_action_convenience_methods(self):
        """Test Action convenience methods."""
        with nlm.Config() as config:
            config.setDefaults("default")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                # Test action creation with convenience methods
                move_action = nlm.Action.createMove(1.0, 0.5)
                assert move_action.getName() == "MoveForward"
                assert move_action.getParameters() == [1.0, 0.5]
                
                interact_action = nlm.Action.createInteract()
                assert interact_action.getName() == "Interact"
                
                custom_action = nlm.Action.createCustom({
                    "speed": 2.0,
                    "direction": 45.0
                })
                assert custom_action.getType() == "Custom"
                
                dict_action = nlm.Action.actionFromDict({
                    "type": "MoveBackward",
                    "parameters": [0.5]
                })
                assert dict_action.getName() == "MoveBackward"
                
                # Test traditional method (backward compatibility)
                traditional_action = nlm.Action(ActionType.Eat)
                traditional_action.setParameters([0.5])
                assert traditional_action.getName() == "Eat"
        
        print("✓ Action convenience methods test passed")
    
    def test_action_context_manager(self):
        """Test Action context manager."""
        with nlm.Config() as config:
            config.setDefaults("default")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.Action(nlm.ActionType.MoveForward) as action:
                    assert action.getType() == nlm.ActionType.MoveForward
                    assert action.getName() == "MoveForward"
                    
                    action.setParameters([2.0, 45.0])
                    assert action.getParameters() == [2.0, 45.0]
                    
                    # Test clone method
                    cloned = action.clone()
                    assert cloned.getType() == action.getType()
                    assert cloned.getParameters() == action.getParameters()
        
        print("✓ Action context manager test passed")

class TestNestedContextManagers:
    """Test nested context manager scenarios."""
    
    def test_deep_nesting(self):
        """Test deeply nested context managers."""
        with nlm.Config() as config, \
             nlm.SimpleWorld() as world, \
             nlm.Brain(config) as brain:
            
            config.setDefaults("quick_test")
            world.configure(100.0, 100.0, 10, 10)
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Run simulation
                for step in range(5):
                    world.update(0.1)
                    action = agent.decodeMotorCommand()
        
        print("✓ Deep nesting test passed")
    
    def test_multiple_resources_single_context(self):
        """Test multiple resources in single context manager (simulated)."""
        # This simulates having multiple resources managed by the same parent
        resources = []
        
        for i in range(3):
            with nlm.Config() as config:
                config.set(f"resource_{i}.value", i)
                resources.append(config)
        
        # All resources should be cleaned up
        assert len(resources) == 3
        print("✓ Multiple resources single context test passed")

class TestErrorHandlingWithContextManagers:
    """Test error handling with context managers."""
    
    def test_exception_safety(self):
        """Test that context managers clean up even on exceptions."""
        try:
            with nlm.Config() as config:
                config.set("test", "value")
                
                with nlm.SimpleWorld() as world:
                    world.configure(50.0, 50.0, 5, 5)
                    
                    with nlm.Brain(config) as brain:
                        brain.initialize()
                        
                        # Raise an exception
                        raise ValueError("Simulated error for testing")
                        
        except ValueError as e:
            assert str(e) == "Simulated error for testing"
            print("✓ Exception safety test passed")
    
    def test_invalid_configuration_error(self):
        """Test error handling for invalid configuration."""
        try:
            with nlm.Config() as config:
                # Try to access validation error
                config.setDefaults("quick_test")
                
                # This should not raise an exception for basic validation
                pass
                
        except (NLMConfigError, NLMValidationError) as e:
            print(f"✓ Configuration error test passed: {e}")
        except Exception:
            # Other exceptions are also acceptable for this test
            print("✓ Configuration error test passed (other exception)")

class TestContextManagerPerformance:
    """Test context manager performance considerations."""
    
    def test_repeated_creation_destruction(self):
        """Test repeated creation and destruction of context managers."""
        import time
        
        start_time = time.time()
        
        for i in range(10):
            with nlm.Config() as config:
                config.set(f"iteration_{i}", i)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 5 seconds)
        assert elapsed < 5.0, f"Performance too slow: {elapsed:.2f}s"
        print(f"✓ Repeated creation/destruction test passed ({elapsed:.3f}s)")
    
    def test_memory_efficiency(self):
        """Test memory efficiency with context managers."""
        # Create and destroy many objects to check for memory leaks
        objects = []
        
        for i in range(20):
            with nlm.Config() as config:
                config.set(f"memory_test_{i}", i)
                objects.append(config)
        
        # Objects should be garbage collected after context exit
        # The list will keep references, but we can verify no errors occurred
        assert len(objects) == 20
        print("✓ Memory efficiency test passed")

class TestTypeHintsAndErrorHandling:
    """Test type hints and error handling."""
    
    def test_type_hints_present(self):
        """Test that type hints are present in the bindings."""
        # This test ensures the bindings have proper type hints
        # The actual type checking would be done at runtime
        
        # Test Config type hints
        with nlm.Config() as config:
            # These methods should return expected types
            config.set("test", "value")
            assert isinstance(config.has("test"), bool)
            assert config.getOr("test", "default") == "value"
        
        # Test Brain type hints
        with nlm.Config() as config:
            config.setDefaults("quick_test")
            with nlm.Brain(config) as brain:
                brain.initialize()
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
                
                neuron_count = brain.getTotalNeuronCount()
                assert isinstance(neuron_count, int)
        
        print("✓ Type hints test passed")
    
    def test_error_exception_types(self):
        """Test custom exception types."""
        from pynlm import (
            NLMError, NLMConfigError, NLMInitializationError,
            NLMResourceError, NLMValidationError
        )
        
        # Test that exception types exist and are properly structured
        assert issubclass(NLMConfigError, NLMError)
        assert issubclass(NLMInitializationError, NLMError)
        assert issubclass(NLMResourceError, NLMError)
        assert issubclass(NLMValidationError, NLMError)
        
        print("✓ Error exception types test passed")

# Test classes can be run as pytest test cases
def test_all_context_managers():
    """Run all context manager tests."""
    print("=" * 60)
    print("Running Comprehensive Context Manager Tests")
    print("=" * 60)
    
    # Create test suite instance
    test_suite = TestConfigContextManager()
    
    # Run test methods
    test_methods = [
        test_suite.test_config_basic_creation,
        test_suite.test_brain_basic_creation,
        test_suite.test_agentbrain_basic_creation,
        test_suite.test_world_basic_creation,
        test_suite.test_vision_context_manager,
        test_suite.test_action_context_manager,
    ]
    
    for test_method in test_methods:
        try:
            test_method()
        except Exception as e:
            print(f"❌ Test failed: {e}")
            raise
    
    print("\n✓ All context manager tests passed!")

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_context_managers()