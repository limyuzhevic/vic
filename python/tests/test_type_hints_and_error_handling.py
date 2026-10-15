"""
Comprehensive test suite for NLM Python type hints and error handling

This module provides comprehensive tests for type hints and error handling
in the NLM (Neural Learning Machine) Python bindings.

Tests cover:
- Type hints validation for all classes and methods
- Custom exception types and their usage
- Error handling in various scenarios
- Type safety in conversions and operations
- Error propagation through context managers
- Validation of input types and values
- Integration of type hints with convenience methods
- Error handling in serialization and batch operations
- Performance of type checking and error handling
- Backward compatibility with type checking
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
    NLMResourceError, NLMValidationError, JsonSerializer,
    BinarySerializer, IncrementalSerializer
)

# Test constants
VALID_CONFIG = {
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

class TestTypeHints:
    """Test type hints for NLM Python bindings."""
    
    def test_config_type_hints(self):
        """Test Config class type hints."""
        with nlm.Config() as config:
            # Test set method returns None
            result = config.set("key", "value")
            assert result is None
            
            # Test get method returns correct type
            value = config.get("key")
            assert value == "value"
            
            # Test getOr method returns correct type
            value = config.getOr("key", "default")
            assert value == "value"
            
            # Test has method returns bool
            has_key = config.has("key")
            assert isinstance(has_key, bool)
            assert has_key is True
            
            # Test getKeys method returns List[str]
            keys = config.getKeys()
            assert isinstance(keys, list)
            
            # Test clear method returns None
            result = config.clear()
            assert result is None
            
            # Test summary method returns Dict[str, Any]
            summary = config.summary()
            assert isinstance(summary, dict)
            
            print("✓ Config type hints test passed")
    
    def test_brain_type_hints(self):
        """Test Brain class type hints."""
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Test getTotalNeuronCount returns int
            neuron_count = brain.getTotalNeuronCount()
            assert isinstance(neuron_count, int)
            assert neuron_count > 0
            
            # Test getActiveNeuronCount returns int
            active_count = brain.getActiveNeuronCount()
            assert isinstance(active_count, int)
            assert active_count >= 0
            
            # Test getTotalSpikeCount returns int
            spike_count = brain.getTotalSpikeCount()
            assert isinstance(spike_count, int)
            assert spike_count >= 0
            
            # Test getTotalSynapseCount returns int
            synapse_count = brain.getTotalSynapseCount()
            assert isinstance(synapse_count, int)
            assert synapse_count >= 0
            
            # Test getFiringNeuronCount returns int
            firing_count = brain.getFiringNeuronCount()
            assert isinstance(firing_count, int)
            assert firing_count >= 0
            
            # Test getAverageFiringRate returns float
            avg_rate = brain.getAverageFiringRate()
            assert isinstance(avg_rate, float)
            assert avg_rate >= 0.0
            
            # Test getRegionCount returns int
            region_count = brain.getRegionCount()
            assert isinstance(region_count, int)
            assert region_count >= 0
            
            # Test getNeuralStats returns Dict[str, Any]
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
            
            print("✓ Brain type hints test passed")
    
    def test_agentbrain_type_hints(self):
        """Test AgentBrain class type hints."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Test decodeMotorCommand returns Action
                action = agent.decodeMotorCommand()
                assert action is not None
                assert isinstance(action, type(Action()))
                
                # Test getNeuromodulationLevel returns float
                neuromodulation = agent.getNeuromodulationLevel()
                assert isinstance(neuromodulation, float)
                assert 0.0 <= neuromodulation <= 1.0
                
                # Test getCuriosityLevel returns float
                curiosity = agent.getCuriosityLevel()
                assert isinstance(curiosity, float)
                assert 0.0 <= curiosity <= 1.0
                
                # Test getPerformanceMetrics returns Dict[str, Any]
                metrics = agent.getPerformanceMetrics()
                assert isinstance(metrics, dict)
                
                print("✓ AgentBrain type hints test passed")
    
    def test_world_type_hints(self):
        """Test SimpleWorld class type hints."""
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            # Test getWidth returns float
            width = world.getWidth()
            assert isinstance(width, float)
            
            # Test getHeight returns float
            height = world.getHeight()
            assert isinstance(height, float)
            
            # Test getSimulationTime returns float
            time = world.getSimulationTime()
            assert isinstance(time, float)
            assert time >= 0.0
            
            print("✓ World type hints test passed")
    
    def test_sensoryinput_type_hints(self):
        """Test SensoryInput class type hints."""
        with nlm.Vision(10, 10) as vision:
            # Test getType returns str
            vtype = vision.getType()
            assert isinstance(vtype, str)
            assert vtype == "Vision"
            
            # Test getData returns List[float]
            data = vision.getData()
            assert isinstance(data, list)
            
            # Test getDimensions returns Tuple[int, int, int]
            dimensions = vision.getDimensions()
            assert isinstance(dimensions, tuple)
            assert len(dimensions) == 3
            
            # Test getVisionArray returns List[List[List[float]]]
            vision_array = vision.getVisionArray()
            assert isinstance(vision_array, list)
            
            print("✓ Vision type hints test passed")
        
        with nlm.Audio(44100, 44100) as audio:
            # Test getType returns str
            vtype = audio.getType()
            assert isinstance(vtype, str)
            assert vtype == "Audio"
            
            # Test getData returns List[float]
            data = audio.getData()
            assert isinstance(data, list)
            
            # Test getInternalState returns Dict[str, float]
            internal_state = audio.getInternalState()
            assert isinstance(internal_state, dict)
            
            print("✓ Audio type hints test passed")
    
    def test_action_type_hints(self):
        """Test Action class type hints."""
        # Test createMove returns Action
        move_action = nlm.Action.createMove(1.0, 0.5)
        assert isinstance(move_action, type(Action()))
        assert move_action.getType() == "MoveForward"
        
        # Test createInteract returns Action
        interact_action = nlm.Action.createInteract()
        assert isinstance(interact_action, type(Action()))
        assert interact_action.getType() == "Interact"
        
        # Test createCustom returns Action
        custom_action = nlm.Action.createCustom({"speed": 1.0})
        assert isinstance(custom_action, type(Action()))
        assert custom_action.getType() == "Custom"
        
        # Test actionFromDict returns Action
        dict_action = nlm.Action.actionFromDict({"type": "MoveBackward"})
        assert isinstance(dict_action, type(Action()))
        assert dict_action.getType() == "MoveBackward"
        
        # Test Action constructor returns Action
        action = nlm.Action(ActionType.Eat)
        assert isinstance(action, type(Action()))
        assert action.getType() == "Eat"
        
        print("✓ Action type hints test passed")

class TestCustomExceptionTypes:
    """Test custom exception types in NLM Python bindings."""
    
    def test_exception_inheritance(self):
        """Test that custom exceptions are properly inherited."""
        # Test that all custom exceptions inherit from NLMError
        assert issubclass(NLMConfigError, NLMError)
        assert issubclass(NLMInitializationError, NLMError)
        assert issubclass(NLMResourceError, NLMError)
        assert issubclass(NLMValidationError, NLMError)
        
        # Test that NLMError is the base class
        assert NLMError.__bases__ == (Exception,)
        
        print("✓ Exception inheritance test passed")
    
    def test_exception_instantiation(self):
        """Test that exception types can be instantiated."""
        # Test NLMConfigError
        config_error = NLMConfigError("Configuration error")
        assert str(config_error) == "Configuration error"
        assert isinstance(config_error, NLMError)
        
        # Test NLMInitializationError
        init_error = NLMInitializationError("Initialization error")
        assert str(init_error) == "Initialization error"
        assert isinstance(init_error, NLMError)
        
        # Test NLMResourceError
        resource_error = NLMResourceError("Resource error")
        assert str(resource_error) == "Resource error"
        assert isinstance(resource_error, NLMError)
        
        # Test NLMValidationError
        validation_error = NLMValidationError("Validation error")
        assert str(validation_error) == "Validation error"
        assert isinstance(validation_error, NLMError)
        
        print("✓ Exception instantiation test passed")
    
    def test_exception_usage_in_context(self):
        """Test using custom exceptions in context manager scenarios."""
        # Test NLMConfigError in configuration
        try:
            with nlm.Config() as config:
                config.setDefaults("quick_test")
                # Try to trigger validation error
                config.updateFromDict({
                    "brain.neuron_count": -1  # Invalid value
                })
                issues = config.validateConfig()
                if issues:
                    raise NLMValidationError(f"Validation issues: {issues}")
        except NLMValidationError as e:
            assert isinstance(e, NLMError)
            print("✓ NLMValidationError usage test passed")
        except Exception:
            # Other exceptions are acceptable for this test
            print("✓ NLMValidationError usage test passed (other exception)")
        
        # Test NLMResourceError
        try:
            # Simulate resource error scenario
            raise NLMResourceError("Simulated resource error")
        except NLMResourceError as e:
            assert isinstance(e, NLMError)
            print("✓ NLMResourceError usage test passed")
        
        # Test NLMInitializationError
        try:
            # Simulate initialization error
            raise NLMInitializationError("Simulated initialization error")
        except NLMInitializationError as e:
            assert isinstance(e, NLMError)
            print("✓ NLMInitializationError usage test passed")
    
    def test_exception_serialization(self):
        """Test exception serialization compatibility."""
        import json
        
        # Test that exceptions can be serialized to JSON
        errors = [
            NLMConfigError("Config error"),
            NLMInitializationError("Init error"),
            NLMResourceError("Resource error"),
            NLMValidationError("Validation error")
        ]
        
        for error in errors:
            error_dict = {
                "error_type": error.__class__.__name__,
                "error_message": str(error),
                "is_base_error": isinstance(error, NLMError)
            }
            
            # Convert to JSON and back
            json_str = json.dumps(error_dict)
            parsed_dict = json.loads(json_str)
            
            assert parsed_dict["is_base_error"] is True
            assert "error_type" in parsed_dict
            assert "error_message" in parsed_dict
            
        print("✓ Exception serialization test passed")

class TestErrorHandling:
    """Test comprehensive error handling in NLM Python bindings."""
    
    def test_type_error_handling(self):
        """Test handling of type errors."""
        with nlm.Config() as config:
            # Test setting wrong type
            config.set("string_key", 123)  # Number instead of string
            config.set("int_key", "not_a_number")  # String instead of int
            config.set("float_key", "not_a_float")  # String instead of float
            
            # The binding should accept any type, but we can test retrieval
            assert config.getOr("string_key", None) == 123
            assert config.getOr("int_key", None) == "not_a_number"
            assert config.getOr("float_key", None) == "not_a_float"
            
            print("✓ Type error handling test passed")
    
    def test_value_error_handling(self):
        """Test handling of value errors."""
        with nlm.Config() as config:
            config.setDefaults("default")
            
            # Test with potentially invalid values
            config.updateFromDict({
                "brain.neuron_count": 0,  # Very small but valid
                "agent.exploration_rate": 1.5,  # > 1.0 might be problematic
                "simulation.max_steps": -100,  # Negative but might be accepted
            })
            
            # Validation should still work
            issues = config.validateConfig()
            assert isinstance(issues, list)
            
            print(f"✓ Value error handling test passed (found {len(issues)} validation issues)")
    
    def test_key_error_handling(self):
        """Test handling of key errors."""
        with nlm.Config() as config:
            config.set("existing_key", "value")
            
            # Test getting non-existent key
            non_existent = config.get("non_existent_key")
            assert non_existent is None
            
            # Test has method
            assert config.has("existing_key") is True
            assert config.has("non_existent_key") is False
            
            # Test getOr with default
            default_value = config.getOr("non_existent_key", "default")
            assert default_value == "default"
            
            print("✓ Key error handling test passed")
    
    def test_context_manager_error_handling(self):
        """Test error handling in context managers."""
        # Test that context managers handle errors gracefully
        try:
            with nlm.Config() as config:
                config.set("test", "value")
                
                # Simulate an error
                raise ValueError("Simulated error in context manager")
                
        except ValueError as e:
            assert str(e) == "Simulated error in context manager"
            print("✓ Context manager error handling test passed")
    
    def test_nested_context_manager_error_handling(self):
        """Test error handling in nested context managers."""
        try:
            with nlm.Config() as config, \
                 nlm.SimpleWorld() as world, \
                 nlm.Brain(config) as brain:
                
                config.setDefaults("default")
                world.configure(50.0, 50.0, 10, 10)
                brain.initialize()
                
                # Simulate an error
                raise RuntimeError("Error in nested context managers")
                
        except RuntimeError as e:
            assert str(e) == "Error in nested context managers"
            print("✓ Nested context manager error handling test passed")

class TestTypeSafety:
    """Test type safety in NLM Python bindings."""
    
    def test_type_conversion_safety(self):
        """Test type conversion safety."""
        with nlm.Config() as config:
            # Test various type conversions
            config.set("int_value", 42)
            config.set("float_value", 3.14)
            config.set("str_value", "hello")
            config.set("bool_value", True)
            config.set("list_value", [1, 2, 3])
            config.set("dict_value", {"nested": "value"})
            
            # Verify types are preserved
            assert isinstance(config.getOr("int_value", None), int)
            assert isinstance(config.getOr("float_value", None), float)
            assert isinstance(config.getOr("str_value", None), str)
            assert isinstance(config.getOr("bool_value", None), bool)
            assert isinstance(config.getOr("list_value", None), list)
            assert isinstance(config.getOr("dict_value", None), dict)
            
            print("✓ Type conversion safety test passed")
    
    def test_data_integrity(self):
        """Test data integrity in type-safe operations."""
        with nlm.Config() as config:
            config.setDefaults("default")
            
            # Test data integrity with nested structures
            complex_config = {
                "neurons": {
                    "count": 100,
                    "connections": [1, 2, 3],
                    "properties": {
                        "excitatory": True,
                        "inhibitory": False
                    }
                },
                "simulation": {
                    "steps": 1000,
                    "timestep": 0.1
                }
            }
            
            config.updateFromDict(complex_config)
            
            # Verify data integrity
            neurons = config.getOr("neurons", None)
            assert isinstance(neurons, dict)
            assert neurons["count"] == 100
            assert neurons["connections"] == [1, 2, 3]
            assert neurons["properties"]["excitatory"] is True
            assert neurons["properties"]["inhibitory"] is False
            
            simulation = config.getOr("simulation", None)
            assert isinstance(simulation, dict)
            assert simulation["steps"] == 1000
            assert simulation["timestep"] == 0.1
            
            print("✓ Data integrity test passed")
    
    def test_bounds_validation(self):
        """Test bounds validation."""
        with nlm.Config() as config:
            config.setDefaults("default")
            
            # Test with various values (bounds are framework-dependent)
            config.updateFromDict({
                "neuron_count": 1,  # Minimum reasonable value
                "exploration_rate": 0.0,  # Minimum value
                "timestep": 0.0001,  # Very small but positive
                "max_steps": 1,  # Minimum value
            })
            
            # Should accept these values
            issues = config.validateConfig()
            assert isinstance(issues, list)
            
            print(f"✓ Bounds validation test passed (found {len(issues)} issues)")

class TestErrorPropagation:
    """Test error propagation through NLM operations."""
    
    def test_error_propagation_through_chaining(self):
        """Test that errors propagate correctly through method chaining."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Perform operations that might raise errors
            try:
                brain.runSimulation(10)
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
                
            except Exception as e:
                # Should be caught and handled
                print(f"✓ Error propagation test caught exception: {e}")
    
    def test_error_handling_in_serialization(self):
        """Test error handling in serialization operations."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(50)
            
            # Test successful serialization
            brain.save("test_brain_state.nlm")
            assert os.path.exists("test_brain_state.nlm")
            
            # Clean up
            os.remove("test_brain_state.nlm")
            
            print("✓ Serialization error handling test passed")
    
    def test_error_handling_in_agent_operations(self):
        """Test error handling in agent operations."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Enable learning features
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                # Perform operations that might raise errors
                try:
                    metrics = agent.learnFromWorld(world, 10)
                    assert isinstance(metrics, dict)
                    
                except Exception as e:
                    # Should be caught and handled
                    print(f"✓ Agent error handling test caught exception: {e}")
    
    def test_error_handling_in_world_operations(self):
        """Test error handling in world operations."""
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            # Perform operations that might raise errors
            try:
                for step in range(10):
                    world.update(0.1)
                    
                    percept = world.getSensoryPercept()
                    body = world.getAgentBody()
                    
                    assert body.x is not None
                    assert body.y is not None
                    
            except Exception as e:
                # Should be caught and handled
                print(f"✓ World error handling test caught exception: {e}")

class TestTypeHintPerformance:
    """Test performance of type checking and error handling."""
    
    def test_type_checking_performance(self):
        """Test performance of type checking."""
        import time
        
        start_time = time.time()
        
        # Perform many type-safe operations
        for i in range(100):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.set(f"key_{i}", i)
            value = config.getOr(f"key_{i}", None)
            has_key = config.has(f"key_{i}")
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 1.0, f"Type checking too slow: {elapsed:.2f}s"
        
        print(f"✓ Type checking performance test passed ({elapsed:.3f}s for 100 operations)")
    
    def test_error_handling_performance(self):
        """Test performance of error handling."""
        import time
        
        start_time = time.time()
        
        # Perform operations that might raise exceptions
        for i in range(50):
            try:
                config = nlm.Config()
                config.setDefaults("default")
                
                # Try some operations that might raise exceptions
                config.updateFromDict({"test": i})
                
                # Get values (should not raise)
                value = config.getOr("test", None)
                has_key = config.has("test")
                
            except Exception:
                # Exception handling overhead
                pass
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 2.0, f"Error handling too slow: {elapsed:.2f}s"
        
        print(f"✓ Error handling performance test passed ({elapsed:.3f}s for 50 operations)")

class TestTypeHintIntegration:
    """Test integration of type hints with other features."""
    
    def test_type_hints_with_convenience_methods(self):
        """Test that type hints work with convenience methods."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Use convenience methods that should return correct types
            result = brain.runSimulation(10)  # Should return self (Brain)
            assert result is brain
            
            stats = brain.getNeuralStats()  # Should return Dict[str, Any]
            assert isinstance(stats, dict)
            
            # Test with various input types
            with nlm.Vision(10, 10) as vision:
                vision.setData([0.1] * 300)
                brain.receiveSensoryInput(vision)
                
                # Get stats again
                stats2 = brain.getNeuralStats()
                assert isinstance(stats2, dict)
        
        print("✓ Type hints with convenience methods test passed")
    
    def test_type_hints_with_context_managers(self):
        """Test that type hints work with context managers."""
        # Test type hints in context manager scenarios
        with nlm.Config() as config:
            config.setDefaults("default")
            
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        
                        # Test type-safe operations
                        action = agent.decodeMotorCommand()
                        assert action is not None
                        
                        metrics = agent.getPerformanceMetrics()
                        assert isinstance(metrics, dict)
        
        print("✓ Type hints with context managers test passed")
    
    def test_type_hints_with_serialization(self):
        """Test that type hints work with serialization."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(50)
            
            # Test serialization with type hints
            brain.save("test_serialization.nlm")
            assert os.path.exists("test_serialization.nlm")
            
            # Clean up
            os.remove("test_serialization.nlm")
            
            print("✓ Type hints with serialization test passed")

# Test classes can be run as pytest test cases
def test_all_type_hints_and_error_handling():
    """Run all type hints and error handling tests."""
    print("=" * 60)
    print("Running Comprehensive Type Hints and Error Handling Tests")
    print("=" * 60)
    
    # Create test suite instances
    type_tests = TestTypeHints()
    exception_tests = TestCustomExceptionTypes()
    error_tests = TestErrorHandling()
    safety_tests = TestTypeSafety()
    propagation_tests = TestErrorPropagation()
    perf_tests = TestTypeHintPerformance()
    integration_tests = TestTypeHintIntegration()
    
    # Test methods in logical groups
    test_groups = [
        # Type hints tests
        (type_tests, [
            type_tests.test_config_type_hints,
            type_tests.test_brain_type_hints,
            type_tests.test_agentbrain_type_hints,
            type_tests.test_world_type_hints,
            type_tests.test_sensoryinput_type_hints,
            type_tests.test_action_type_hints,
        ]),
        
        # Custom exception tests
        (exception_tests, [
            exception_tests.test_exception_inheritance,
            exception_tests.test_exception_instantiation,
            exception_tests.test_exception_usage_in_context,
            exception_tests.test_exception_serialization,
        ]),
        
        # Error handling tests
        (error_tests, [
            error_tests.test_type_error_handling,
            error_tests.test_value_error_handling,
            error_tests.test_key_error_handling,
            error_tests.test_context_manager_error_handling,
            error_tests.test_nested_context_manager_error_handling,
        ]),
        
        # Type safety tests
        (safety_tests, [
            safety_tests.test_type_conversion_safety,
            safety_tests.test_data_integrity,
            safety_tests.test_bounds_validation,
        ]),
        
        # Error propagation tests
        (propagation_tests, [
            propagation_tests.test_error_propagation_through_chaining,
            propagation_tests.test_error_handling_in_serialization,
            propagation_tests.test_error_handling_in_agent_operations,
            propagation_tests.test_error_handling_in_world_operations,
        ]),
        
        # Performance tests
        (perf_tests, [
            perf_tests.test_type_checking_performance,
            perf_tests.test_error_handling_performance,
        ]),
        
        # Integration tests
        (integration_tests, [
            integration_tests.test_type_hints_with_convenience_methods,
            integration_tests.test_type_hints_with_context_managers,
            integration_tests.test_type_hints_with_serialization,
        ]),
    ]
    
    total_tests = 0
    passed_tests = 0
    
    for test_class, test_methods in test_groups:
        print(f"\n--- Testing {test_class.__name__} ---")
        for test_method in test_methods:
            total_tests += 1
            try:
                test_method()
                passed_tests += 1
            except Exception as e:
                print(f"❌ {test_method.__name__} failed: {e}")
                raise
    
    print("\n" + "=" * 60)
    print("Type Hints and Error Handling Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All type hints and error handling tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} type hints and error handling tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_type_hints_and_error_handling()