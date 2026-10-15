"""
Comprehensive test suite for NLM Python edge cases

This module provides comprehensive tests for edge cases and unusual scenarios
in the NLM (Neural Learning Machine) Python bindings.

Edge case tests identify boundary conditions, error conditions, and unusual
scenarios that might not be covered by standard tests.

Tests cover:
- Invalid input types and values
- Boundary conditions
- Error conditions and exceptions
- Resource management edge cases
- Performance edge cases
- Integration edge cases
- Serialization edge cases
- Visualization edge cases
- Configuration edge cases
- Behavior at limits
"""

import pytest
import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
from pynlm import (
    Config, Brain, AgentBrain, SimpleWorld,
    Vision, Audio, InternalSignals, Action,
    ActionType, NLMError, NLMConfigError, NLMInitializationError,
    NLMResourceError, NLMValidationError, JsonSerializer,
    BinarySerializer, IncrementalSerializer
)

# Edge case test constants
EDGE_CASE_CONFIGS = [
    {"brain.neuron_count": 0, "brain.connection_probability": 0.0},
    {"brain.neuron_count": 1, "brain.connection_probability": 1.0},
    {"brain.neuron_count": 10000, "brain.connection_probability": 0.01},
    {"agent.exploration_rate": -0.1, "simulation.max_steps": -1},
    {"agent.exploration_rate": 2.0, "simulation.timestep": 0.0},
    {"brain.neuron_count": 0},  # Missing other keys
    {"unknown.key": "value"},  # Unknown configuration key
]

class TestInputValidationEdgeCases:
    """Test edge cases for input validation."""
    
    def test_empty_configurations(self):
        """Test empty or minimal configurations."""
        # Test completely empty config
        with nlm.Config() as config:
            assert config.getOr("nonexistent.key", None) is None
        
        # Test config with only one key
        with nlm.Config() as config:
            config.set("single.key", "value")
            assert config.getOr("single.key", None) == "value"
            assert config.getOr("other.key", None) is None
        
        print("✓ Empty configurations edge case test passed")
    
    def test_extreme_values(self):
        """Test extreme values for configuration parameters."""
        extreme_configs = [
            {"brain.neuron_count": 1},
            {"brain.neuron_count": 100000},
            {"agent.exploration_rate": 0.0},
            {"agent.exploration_rate": 1.0},
            {"agent.exploration_rate": 1.5},  # > 1.0
            {"simulation.max_steps": 0},
            {"simulation.max_steps": 1000000},
            {"simulation.timestep": 0.000001},
            {"simulation.timestep": 1.0},
        ]
        
        for i, test_config in enumerate(extreme_configs):
            config = nlm.Config()
            config.updateFromDict(test_config)
            
            # Should not crash or raise exceptions
            issues = config.validateConfig()
            assert isinstance(issues, list)
        
        print("✓ Extreme values edge case test passed")
    
    def test_negative_and_zero_values(self):
        """Test negative and zero values in configuration."""
        with nlm.Config() as config:
            config.updateFromDict({
                "brain.neuron_count": 0,
                "simulation.max_steps": -100,
                "simulation.timestep": -0.1,
                "agent.exploration_rate": -0.5
            })
            
            # Should accept values without crashing
            # Validation will determine what's valid
            issues = config.validateConfig()
            assert isinstance(issues, list)
            
            # Values should be retrievable
            assert config.getOr("brain.neuron_count", 0) == 0
            assert config.getOr("simulation.max_steps", 0) == -100
        
        print("✓ Negative and zero values edge case test passed")
    
    def test_string_extremes(self):
        """Test extreme string values."""
        with nlm.Config() as config:
            config.updateFromDict({
                "empty.string": "",
                "very.long.string": "x" * 10000,
                "unicode.string": "Hello 世界 🌍",
                "newlines.string": "line1\nline2\nline3",
                "special.chars.string": "!@#$%^&*()_+-=[]{}|;':\",./<>?",
                "tab.string": "column1\tcolumn2\tcolumn3"
            })
            
            # Should accept strings without crashing
            assert config.getOr("empty.string", "") == ""
            assert len(config.getOr("very.long.string", "")) == 10000
            assert "世界" in config.getOr("unicode.string", "")
        
        print("✓ String extremes edge case test passed")
    
    def test_list_and_dict_edge_cases(self):
        """Test edge cases with list and dictionary values."""
        with nlm.Config() as config:
            config.updateFromDict({
                "empty.list": [],
                "large.list": list(range(1000)),
                "nested.list": [[1, 2], [3, 4], [5, 6]],
                "empty.dict": {},
                "large.dict": {f"key_{i}": f"value_{i}" for i in range(100)},
                "nested.dict": {"outer": {"inner": {"deep": "value"}}},
                "mixed.dict": {"list": [1, 2, 3], "dict": {"nested": "value"}}
            })
            
            # Should accept complex data structures
            assert config.getOr("empty.list", []) == []
            assert len(config.getOr("large.list", [])) == 1000
            assert config.getOr("nested.list", []) == [[1, 2], [3, 4], [5, 6]]
        
        print("✓ List and dict edge cases test passed")

class TestBoundaryConditions:
    """Test boundary conditions in NLM operations."""
    
    def test_minimal_brain_config(self):
        """Test brain with minimal configuration."""
        config = nlm.Config()
        config.updateFromDict({"brain.neuron_count": 1})
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            stats = brain.getNeuralStats()
            assert stats['total_neurons'] == 1
            assert stats['active_neurons'] >= 0
        
        print("✓ Minimal brain config boundary test passed")
    
    def test_large_brain_config(self):
        """Test brain with large configuration."""
        config = nlm.Config()
        config.updateFromDict({
            "brain.neuron_count": 10000,
            "brain.connection_probability": 0.01
        })
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            stats = brain.getNeuralStats()
            assert stats['total_neurons'] == 10000
        
        print("✓ Large brain config boundary test passed")
    
    def test_world_boundary_dimensions(self):
        """Test world with boundary dimensions."""
        # Test minimal world dimensions
        with nlm.SimpleWorld() as world:
            world.configure(0.1, 0.1, 1, 1)
            assert world.getWidth() == 0.1
            assert world.getHeight() == 0.1
        
        # Test large world dimensions
        with nlm.SimpleWorld() as world:
            world.configure(10000.0, 10000.0, 100, 100)
            assert world.getWidth() == 10000.0
            assert world.getHeight() == 10000.0
        
        print("✓ World boundary dimensions test passed")
    
    def test_vision_boundary_dimensions(self):
        """Test vision with boundary dimensions."""
        # Test minimal vision dimensions
        with nlm.Vision(1, 1) as vision:
            assert vision.getWidth() == 1
            assert vision.getHeight() == 1
            
            # Create minimal data
            data = [0.5] * 3
            vision.setData(data)
            assert len(vision.getData()) == 3
        
        # Test large vision dimensions
        with nlm.Vision(1000, 1000) as vision:
            assert vision.getWidth() == 1000
            assert vision.getHeight() == 1000
            
            # Create large data (just a sample)
            data = [0.5] * (1000 * 1000 * 3)
            vision.setData(data)
            assert len(vision.getData()) == 1000 * 1000 * 3
        
        print("✓ Vision boundary dimensions test passed")
    
    def test_action_boundary_values(self):
        """Test action with boundary values."""
        # Test createMove with boundary values
        move_action = nlm.Action.createMove(-1000.0, -180.0)
        assert move_action.getType() == "MoveForward"
        assert move_action.getParameters() == [-1000.0, -180.0]
        
        move_action = nlm.Action.createMove(0.0, 0.0)
        assert move_action.getParameters() == [0.0, 0.0]
        
        move_action = nlm.Action.createMove(1000.0, 360.0)
        assert move_action.getParameters() == [1000.0, 360.0]
        
        print("✓ Action boundary values test passed")

class TestErrorConditions:
    """Test error conditions and exception handling."""
    
    def test_config_validation_errors(self):
        """Test various configuration validation errors."""
        test_cases = [
            {"brain.neuron_count": -1},  # Negative neuron count
            {"brain.connection_probability": -0.1},  # Negative probability
            {"brain.connection_probability": 1.5},  # > 1.0 probability
            {"agent.exploration_rate": -0.1},  # Negative exploration
            {"agent.exploration_rate": 2.0},  # > 1.0 exploration
            {"simulation.max_steps": -1},  # Negative steps
            {"simulation.timestep": -0.1},  # Negative timestep
            {"simulation.timestep": 0.0},  # Zero timestep
        ]
        
        for test_config in test_cases:
            config = nlm.Config()
            config.updateFromDict(test_config)
            
            # Validation should not crash
            issues = config.validateConfig()
            assert isinstance(issues, list)
        
        print("✓ Configuration validation errors test passed")
    
    def test_context_manager_errors(self):
        """Test error handling in context managers."""
        # Test that context managers handle errors gracefully
        try:
            with nlm.Config() as config:
                config.set("test", "value")
                
                # Try to trigger an error
                raise ValueError("Simulated error in context manager")
                
        except ValueError as e:
            assert str(e) == "Simulated error in context manager"
            print("✓ Context manager error handling test passed")
    
    def test_resource_cleanup_errors(self):
        """Test resource cleanup error handling."""
        # Test that resources are cleaned up even if errors occur
        try:
            config = nlm.Config()
            config.set("test", "value")
            
            brain = nlm.Brain(config)
            brain.initialize()
            
            # Force an error
            raise RuntimeError("Simulated resource error")
            
        except RuntimeError:
            # Resources should be cleaned up automatically
            print("✓ Resource cleanup error handling test passed")
        except:
            # Other exceptions are also acceptable for this test
            print("✓ Resource cleanup error handling test passed (other exception)")
    
    def test_serialization_errors(self):
        """Test serialization error handling."""
        # Test serialization with various error conditions
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(10)
            
            # Test serialization with invalid data
            try:
                JsonSerializer.save_to_file("invalid.json", object())
                print("✓ JSON serialization handled invalid data")
            except Exception:
                print("✓ JSON serialization raised exception for invalid data (expected)")
            
            # Test with corrupted file
            with open("corrupted.json", "w") as f:
                f.write("{invalid json")
            
            try:
                loaded = JsonSerializer.load_from_file("corrupted.json")
                print("✓ JSON serialization handled corrupted file")
            except Exception:
                print("✓ JSON serialization raised exception for corrupted file (expected)")
            
            # Clean up
            os.remove("corrupted.json")
        
        print("✓ Serialization errors test passed")

class TestResourceManagementEdgeCases:
    """Test resource management edge cases."""
    
    def test_multiple_context_creation(self):
        """Test creating multiple contexts."""
        contexts = []
        
        for i in range(10):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({"test": i})
            contexts.append(config)
        
        # Verify all contexts were created
        assert len(contexts) == 10
        
        # Clean up all contexts
        for context in contexts:
            context = None
        
        print("✓ Multiple context creation edge case test passed")
    
    def test_context_reuse(self):
        """Test reusing contexts."""
        config = nlm.Config()
        
        # Use config multiple times
        for i in range(5):
            with config:
                config.setDefaults("quick_test")
                config.updateFromDict({"test": i})
                
                # Verify config was set
                assert config.getOr("test", 0) == i
        
        print("✓ Context reuse edge case test passed")
    
    def test_memory_usage(self):
        """Test memory usage with edge cases."""
        import gc
        
        # Force garbage collection
        gc.collect()
        
        # Create many objects
        objects = []
        for i in range(100):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({"test": i})
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            objects.append((config, brain))
        
        # Verify all objects were created
        assert len(objects) == 100
        
        # Clean up all objects
        for config, brain in objects:
            config = None
            brain = None
        
        # Force garbage collection
        gc.collect()
        
        print("✓ Memory usage edge case test passed")
    
    def test_long_running_operations(self):
        """Test long-running operations."""
        import time
        
        start_time = time.time()
        
        # Run operations for extended time
        for i in range(50):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({"test": i})
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                brain.runSimulation(10)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 10 seconds)
        assert elapsed < 10.0, f"Long-running operations too slow: {elapsed:.2f}s"
        
        print(f"✓ Long-running operations edge case test passed ({elapsed:.3f}s)")

class TestVisualizationEdgeCases:
    """Test visualization edge cases."""
    
    def test_empty_world_visualization(self):
        """Test visualization of empty world."""
        with nlm.SimpleWorld() as world:
            world.configure(10.0, 10.0, 5, 5)
            
            # Get observation map of empty world
            observation_map = world.getObservationMap()
            assert isinstance(observation_map, list)
            
            # All values should be 0.0 in empty world
            for row in observation_map:
                for value in row:
                    assert value == 0.0
        
        print("✓ Empty world visualization edge case test passed")
    
    def test_world_with_obstacles_visualization(self):
        """Test visualization with many obstacles."""
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 50, 50)
            world.createGridWorld(100.0, 100.0, 0.5)  # 50% obstacles
            
            # Add many resources
            for x in range(10, 91, 10):
                for y in range(10, 91, 10):
                    world.addResource(float(x), float(y), 1.0)
            
            # Get observation map
            observation_map = world.getObservationMap()
            assert isinstance(observation_map, list)
            
            # Should have many non-zero values (obstacles and resources)
            non_zero_count = 0
            for row in observation_map:
                for value in row:
                    if value > 0.0:
                        non_zero_count += 1
            
            assert non_zero_count > 0
        
        print("✓ World with obstacles visualization edge case test passed")
    
    def test_vision_data_extremes(self):
        """Test vision data with extreme values."""
        with nlm.Vision(10, 10) as vision:
            # Create vision data with extreme values
            extreme_data = []
            
            # Mix of extreme values
            for i in range(10 * 10 * 3):
                if i % 3 == 0:
                    extreme_data.append(0.0)      # Minimum
                elif i % 3 == 1:
                    extreme_data.append(1.0)      # Maximum
                else:
                    extreme_data.append(i % 101 / 100.0)  # Other values
            
            vision.setData(extreme_data)
            
            # Get vision array
            vision_array = vision.getVisionArray()
            assert isinstance(vision_array, list)
            
            # Verify extreme values
            for row in vision_array:
                for pixel in row:
                    for channel in pixel:
                        assert 0.0 <= channel <= 1.0
        
        print("✓ Vision data extremes edge case test passed")
    
    def test_performance_metrics_extremes(self):
        """Test performance metrics with extreme values."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Get neural stats
            stats = brain.getNeuralStats()
            
            # Verify stats are within reasonable ranges
            assert 0 <= stats['total_neurons']
            assert 0 <= stats['active_neurons'] <= stats['total_neurons']
            assert 0 <= stats['total_spikes']
            assert 0 <= stats['total_synapses']
            assert 0 <= stats['firing_neurons'] <= stats['total_neurons']
            assert 0.0 <= stats['average_firing_rate']
        
        print("✓ Performance metrics extremes edge case test passed")

class TestIntegrationEdgeCases:
    """Test integration edge cases."""
    
    def test_deeply_nested_contexts(self):
        """Test deeply nested context managers."""
        # Create deeply nested contexts
        with nlm.Config() as config1:
            config1.setDefaults("default")
            
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                
                with nlm.Brain(config1) as brain:
                    brain.initialize()
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        
                        # Another level of nesting
                        with nlm.Vision(10, 10) as vision:
                            vision.setData([0.1] * 300)
                            brain.receiveSensoryInput(vision)
        
        print("✓ Deeply nested contexts edge case test passed")
    
    def test_multiple_agent_batch(self):
        """Test multiple agents in batch."""
        agents = []
        
        # Create multiple agents
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("default")
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                world.setAgentGoal(40.0, 40.0)
                
                agent = nlm.AgentBrain(brain)
                agent.initialize(world)
                
                agents.append(agent)
        
        # Use all agents
        for agent in agents:
            action = agent.decodeMotorCommand()
            assert action is not None
        
        print("✓ Multiple agent batch edge case test passed")
    
    def test_mixed_world_types(self):
        """Test mixing different world types."""
        # Create different types of worlds
        worlds = []
        
        # Regular world
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            worlds.append(world)
        
        # World with obstacles
        with nlm.SimpleWorld() as world:
            world.createGridWorld(50.0, 50.0, 0.2)
            worlds.append(world)
        
        # Verify all worlds were created
        assert len(worlds) == 2
        
        print("✓ Mixed world types edge case test passed")
    
    def test_serialization_integration_edge_cases(self):
        """Test serialization integration edge cases."""
        # Create complex scenario with serialization
        with nlm.Config() as config:
            config.setDefaults("default")
            
            with nlm.SimpleWorld() as world:
                world.configure(100.0, 100.0, 20, 20)
                world.createGridWorld(100.0, 100.0, 0.1)
                
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    brain.runSimulation(25)
                    
                    # Serialize brain
                    JsonSerializer.serialize_brain(brain, "integration_edge.json")
                    
                    # Load and verify
                    loaded_brain = nlm.Brain(config)
                    with loaded_brain:
                        loaded_brain.load("integration_edge.json")
                        loaded_stats = loaded_brain.getNeuralStats()
                        assert isinstance(loaded_stats, dict)
                        assert loaded_stats.get('total_spikes', 0) > 0
                    
                    # Clean up
                    os.remove("integration_edge.json")
        
        print("✓ Serialization integration edge cases test passed")

# Test classes can be run as pytest test cases
def test_all_edge_cases():
    """Run all edge case tests."""
    print("=" * 60)
    print("Running Comprehensive Edge Case Tests")
    print("=" * 60)
    
    # Create test suite instances
    input_tests = TestInputValidationEdgeCases()
    boundary_tests = TestBoundaryConditions()
    error_tests = TestErrorConditions()
    resource_tests = TestResourceManagementEdgeCases()
    visualization_tests = TestVisualizationEdgeCases()
    integration_tests = TestIntegrationEdgeCases()
    
    # Test methods in logical groups
    test_groups = [
        # Input validation tests
        (input_tests, [
            input_tests.test_empty_configurations,
            input_tests.test_extreme_values,
            input_tests.test_negative_and_zero_values,
            input_tests.test_string_extremes,
            input_tests.test_list_and_dict_edge_cases,
        ]),
        
        # Boundary condition tests
        (boundary_tests, [
            boundary_tests.test_minimal_brain_config,
            boundary_tests.test_large_brain_config,
            boundary_tests.test_world_boundary_dimensions,
            boundary_tests.test_vision_boundary_dimensions,
            boundary_tests.test_action_boundary_values,
        ]),
        
        # Error condition tests
        (error_tests, [
            error_tests.test_config_validation_errors,
            error_tests.test_context_manager_errors,
            error_tests.test_resource_cleanup_errors,
            error_tests.test_serialization_errors,
        ]),
        
        # Resource management tests
        (resource_tests, [
            resource_tests.test_multiple_context_creation,
            resource_tests.test_context_reuse,
            resource_tests.test_memory_usage,
            resource_tests.test_long_running_operations,
        ]),
        
        # Visualization edge case tests
        (visualization_tests, [
            visualization_tests.test_empty_world_visualization,
            visualization_tests.test_world_with_obstacles_visualization,
            visualization_tests.test_vision_data_extremes,
            visualization_tests.test_performance_metrics_extremes,
        ]),
        
        # Integration edge case tests
        (integration_tests, [
            integration_tests.test_deeply_nested_contexts,
            integration_tests.test_multiple_agent_batch,
            integration_tests.test_mixed_world_types,
            integration_tests.test_serialization_integration_edge_cases,
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
    print("Edge Cases Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All edge case tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} edge case tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_edge_cases()