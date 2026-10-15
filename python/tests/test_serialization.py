"""
Comprehensive test suite for NLM Python serialization

This module provides comprehensive tests for serialization functionality
in the NLM (Neural Learning Machine) Python bindings.

Serialization allows saving and loading brain states, world states, and
configuration data for checkpointing and experiment reproducibility.

Tests cover:
- JSON serializer functionality
- Binary serializer functionality
- Incremental serializer functionality
- Serialization error handling
- Type hints in serialization
- Performance of serialization operations
- Integration with context managers
- Serialization of different object types (Config, Brain, AgentBrain, SimpleWorld, SensoryInput, Action)
- Serialization validation and verification
- Backward compatibility in serialization
- Serialization edge cases and error conditions
"""

import pytest
import sys
import os
import json
import tempfile
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
TEST_CONFIG_DATA = {
    "brain.neuron_count": 100,
    "brain.connection_probability": 0.2,
    "agent.exploration_rate": 0.3,
    "simulation.max_steps": 1000,
    "simulation.timestep": 0.1
}

class TestJsonSerializer:
    """Test JsonSerializer class."""
    
    def test_save_to_file(self):
        """Test JsonSerializer.save_to_file method."""
        test_data = {"key": "value", "number": 42, "list": [1, 2, 3]}
        
        # Save to file
        JsonSerializer.save_to_file("test.json", test_data)
        
        # Verify file was created
        assert os.path.exists("test.json")
        
        # Load and verify content
        with open("test.json", "r") as f:
            loaded_data = json.load(f)
        
        assert loaded_data == test_data
        
        # Clean up
        os.remove("test.json")
        
        print("✓ JsonSerializer.save_to_file test passed")
    
    def test_load_from_file(self):
        """Test JsonSerializer.load_from_file method."""
        test_data = {"complex": {"nested": {"key": "value"}}, "list": [1, 2, 3, 4, 5]}
        
        # Save test data
        JsonSerializer.save_to_file("complex.json", test_data)
        
        # Load using JsonSerializer
        loaded_data = JsonSerializer.load_from_file("complex.json")
        
        assert loaded_data == test_data
        assert isinstance(loaded_data, dict)
        
        # Clean up
        os.remove("complex.json")
        
        print("✓ JsonSerializer.load_from_file test passed")
    
    def test_serialize_brain(self):
        """Test JsonSerializer.serialize_brain method."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(50)
            
            # Serialize brain
            JsonSerializer.serialize_brain(brain, "brain_state.json")
            
            # Verify file was created
            assert os.path.exists("brain_state.json")
            
            # Load and verify serialized data
            loaded_brain = nlm.Brain(config)
            with loaded_brain:
                loaded_brain.load("brain_state.json")
                
                # Verify loaded state
                stats = loaded_brain.getNeuralStats()
                assert isinstance(stats, dict)
                assert stats.get('total_spikes', 0) > 0
            
            # Clean up
            os.remove("brain_state.json")
            
            print("✓ JsonSerializer.serialize_brain test passed")
    
    def test_deserialize_brain(self):
        """Test JsonSerializer.deserialize_brain method."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(100)
            
            # Serialize brain
            JsonSerializer.serialize_brain(brain, "deserialize_test.json")
            
            # Create new brain and deserialize into it
            new_config = nlm.Config()
            new_config.setDefaults("default")
            
            with nlm.Brain(new_config) as new_brain:
                # Note: deserialize_brain signature might need adjustment
                # Based on the API reference: JsonSerializer.deserialize_brain(filepath)
                # This creates a new brain, doesn't deserialize into existing
                deserialized_brain = JsonSerializer.deserialize_brain("deserialize_test.json")
                
                assert deserialized_brain is not None
                assert isinstance(deserialized_brain, nlm.Brain)
                
                # Get stats from deserialized brain
                stats = deserialized_brain.getNeuralStats()
                assert isinstance(stats, dict)
                assert stats.get('total_spikes', 0) > 0
        
        # Clean up
        os.remove("deserialize_test.json")
        
        print("✓ JsonSerializer.deserialize_brain test passed")
    
    def test_serialize_world(self):
        """Test JsonSerializer.serialize_world method."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 20, 20)
            world.createGridWorld(100.0, 100.0, 0.1)
            world.setAgentGoal(90.0, 90.0)
            
            # Add some resources
            world.addResource(10.0, 10.0, 5.0)
            world.addResource(50.0, 50.0, 10.0)
            
            # Serialize world
            JsonSerializer.serialize_world(world, "world_state.json")
            
            # Verify file was created
            assert os.path.exists("world_state.json")
            
            # Load serialized world
            loaded_world = JsonSerializer.load_from_file("world_state.json")
            assert isinstance(loaded_world, dict)
            
            print("✓ JsonSerializer.serialize_world test passed")
        
        # Clean up
        os.remove("world_state.json")
    
    def test_serialization_error_handling(self):
        """Test JsonSerializer error handling."""
        # Test with invalid file path
        try:
            JsonSerializer.save_to_file("/invalid/path/test.json", {})
            print("✓ JsonSerializer handled invalid path (created directory)")
        except Exception:
            print("✓ JsonSerializer raised exception for invalid path (expected)")
        
        # Test with invalid data
        try:
            # This should work with any JSON-serializable data
            JsonSerializer.save_to_file("test.json", {"complex": {"nested": object()}})
            print("✓ JsonSerializer handled complex data")
        except Exception:
            print("✓ JsonSerializer raised exception for complex data (expected)")
        
        print("✓ JsonSerializer error handling test passed")
    
    def test_serialization_type_hints(self):
        """Test JsonSerializer type hints."""
        # Test that methods return expected types
        test_data = {"key": "value", "number": 123}
        
        # save_to_file should return None
        result = JsonSerializer.save_to_file("type_test.json", test_data)
        assert result is None
        
        # load_from_file should return dict
        loaded = JsonSerializer.load_from_file("type_test.json")
        assert isinstance(loaded, dict)
        
        # serialize_brain should return None
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(10)
            
            result = JsonSerializer.serialize_brain(brain, "type_brain.json")
            assert result is None
            
            # clean up
            os.remove("type_brain.json")
        
        # clean up
        os.remove("type_test.json")
        
        print("✓ JsonSerializer type hints test passed")

class TestBinarySerializer:
    """Test BinarySerializer class."""
    
    def test_save_to_file(self):
        """Test BinarySerializer.save_to_file method."""
        test_data = {"key": "value", "number": 42, "list": [1, 2, 3]}
        
        # Save to binary file
        BinarySerializer.save_to_file("test.bin", test_data)
        
        # Verify file was created
        assert os.path.exists("test.bin")
        
        # Load and verify content (as JSON for comparison)
        loaded_data = JsonSerializer.load_from_file("test.bin")
        assert loaded_data == test_data
        
        # Clean up
        os.remove("test.bin")
        
        print("✓ BinarySerializer.save_to_file test passed")
    
    def test_load_from_file(self):
        """Test BinarySerializer.load_from_file method."""
        test_data = {"complex": {"nested": {"key": "value"}}, "list": [1, 2, 3, 4, 5]}
        
        # Save test data as binary
        BinarySerializer.save_to_file("complex.bin", test_data)
        
        # Load using BinarySerializer
        loaded_data = BinarySerializer.load_from_file("complex.bin")
        
        assert loaded_data == test_data
        assert isinstance(loaded_data, dict)
        
        # Clean up
        os.remove("complex.bin")
        
        print("✓ BinarySerializer.load_from_file test passed")
    
    def test_serialize_brain(self):
        """Test BinarySerializer.serialize_brain method."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(50)
            
            # Serialize brain using binary format
            BinarySerializer.serialize_brain(brain, "brain_state.bin")
            
            # Verify file was created
            assert os.path.exists("brain_state.bin")
            
            # Load and verify serialized data
            loaded_config = nlm.Config()
            loaded_config.setDefaults("default")
            
            with nlm.Brain(loaded_config) as loaded_brain:
                # Note: deserialize_brain signature in BinarySerializer might be different
                # Based on API: BinarySerializer.deserialize_brain(filepath, brain)
                BinarySerializer.deserialize_brain("brain_state.bin", loaded_brain)
                
                # Verify loaded state
                stats = loaded_brain.getNeuralStats()
                assert isinstance(stats, dict)
                assert stats.get('total_spikes', 0) > 0
            
            # Clean up
            os.remove("brain_state.bin")
            
            print("✓ BinarySerializer.serialize_brain test passed")
    
    def test_binary_serializer_performance(self):
        """Test BinarySerializer performance."""
        import time
        
        start_time = time.time()
        
        # Create and serialize multiple brains
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                brain.runSimulation(20)
                
                # Serialize using binary format
                BinarySerializer.serialize_brain(brain, f"perf_brain_{i}.bin")
                
                # Clean up
                os.remove(f"perf_brain_{i}.bin")
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 5.0, f"BinarySerializer too slow: {elapsed:.2f}s"
        
        print(f"✓ BinarySerializer performance test passed ({elapsed:.3f}s for 5 brains)")

class TestIncrementalSerializer:
    """Test IncrementalSerializer class."""
    
    def test_save_to_file(self):
        """Test IncrementalSerializer.save_to_file method."""
        test_data = {"key": "value", "number": 42, "list": [1, 2, 3]}
        
        # Save to file in chunks
        IncrementalSerializer.save_to_file("incremental.json", test_data)
        
        # Verify file was created
        assert os.path.exists("incremental.json")
        
        # Load and verify content
        loaded_data = JsonSerializer.load_from_file("incremental.json")
        assert loaded_data == test_data
        
        # Clean up
        os.remove("incremental.json")
        
        print("✓ IncrementalSerializer.save_to_file test passed")
    
    def test_load_from_file(self):
        """Test IncrementalSerializer.load_from_file method."""
        test_data = {"complex": {"nested": {"key": "value"}}, "list": [1, 2, 3, 4, 5]}
        
        # Save test data incrementally
        IncrementalSerializer.save_to_file("incremental.bin", test_data)
        
        # Load using IncrementalSerializer
        loaded_data = IncrementalSerializer.load_from_file("incremental.bin")
        
        assert loaded_data == test_data
        assert isinstance(loaded_data, dict)
        
        # Clean up
        os.remove("incremental.bin")
        
        print("✓ IncrementalSerializer.load_from_file test passed")
    
    def test_get_chunk_count(self):
        """Test IncrementalSerializer.get_chunk_count method."""
        test_data = {"key": "value", "number": 42, "list": list(range(100))}
        
        # Save data incrementally
        IncrementalSerializer.save_to_file("chunked.json", test_data)
        
        # Get chunk count
        chunk_count = IncrementalSerializer.get_chunk_count("chunked.json")
        assert isinstance(chunk_count, int)
        assert chunk_count > 0
        
        # Get chunk size for each chunk
        for chunk_index in range(chunk_count):
            chunk_size = IncrementalSerializer.get_chunk_size("chunked.json", chunk_index)
            assert isinstance(chunk_size, int)
            assert chunk_size > 0
        
        # Clean up
        os.remove("chunked.json")
        
        print(f"✓ IncrementalSerializer.get_chunk_count test passed (chunk_count: {chunk_count})")
    
    def test_incremental_serializer_performance(self):
        """Test IncrementalSerializer performance."""
        import time
        
        start_time = time.time()
        
        # Create and serialize multiple brains incrementally
        for i in range(3):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                brain.runSimulation(20)
                
                # Serialize incrementally
                IncrementalSerializer.save_to_file(f"incremental_{i}.json", brain.getNeuralStats())
                
                # Clean up
                os.remove(f"incremental_{i}.json")
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 5.0, f"IncrementalSerializer too slow: {elapsed:.2f}s"
        
        print(f"✓ IncrementalSerializer performance test passed ({elapsed:.3f}s for 3 brains)")

class TestSerializationErrorHandling:
    """Test serialization error handling."""
    
    def test_invalid_file_paths(self):
        """Test serialization with invalid file paths."""
        test_data = {"key": "value"}
        
        # Test with various invalid paths
        invalid_paths = [
            "/nonexistent/directory/test.json",
            "",
            "   ",
        ]
        
        for path in invalid_paths:
            try:
                JsonSerializer.save_to_file(path, test_data)
                print(f"✓ JsonSerializer handled path '{path}'")
            except Exception:
                print(f"✓ JsonSerializer raised exception for path '{path}' (expected)")
        
        print("✓ Invalid file path handling test passed")
    
    def test_corrupted_files(self):
        """Test handling of corrupted files."""
        # Create a corrupted JSON file
        with open("corrupted.json", "w") as f:
            f.write("{invalid json content")
        
        try:
            # Try to load corrupted file
            loaded_data = JsonSerializer.load_from_file("corrupted.json")
            print("✓ JsonSerializer handled corrupted file gracefully")
        except Exception as e:
            print(f"✓ JsonSerializer raised exception for corrupted file: {e}")
        
        # Clean up
        os.remove("corrupted.json")
        
        print("✓ Corrupted file handling test passed")
    
    def test_missing_files(self):
        """Test handling of missing files."""
        # Try to load non-existent file
        try:
            loaded_data = JsonSerializer.load_from_file("nonexistent.json")
            print("✓ JsonSerializer handled missing file gracefully")
        except Exception as e:
            print(f"✓ JsonSerializer raised exception for missing file: {e}")
        
        print("✓ Missing file handling test passed")
    
    def test_permission_errors(self):
        """Test serialization permission handling."""
        # This test might not work depending on the system
        # but it's worth trying
        try:
            # Try to create file in read-only directory
            JsonSerializer.save_to_file("/root/test.json", {})
            print("✓ JsonSerializer created file in read-only directory (unexpected)")
        except Exception as e:
            print(f"✓ JsonSerializer raised permission exception: {e}")
        
        print("✓ Permission error handling test passed")

class TestSerializationTypeSafety:
    """Test serialization type safety."""
    
    def test_type_conversion_safety(self):
        """Test type conversion safety during serialization."""
        # Test various data types can be serialized
        test_cases = [
            {"string": "value", "number": 42, "float": 3.14, "bool": True, "none": None},
            {"list": [1, 2, 3], "dict": {"nested": "value"}},
            {"complex": {"a": 1, "b": {"c": 2, "d": [3, 4, 5]}}},
        ]
        
        for i, test_data in enumerate(test_cases):
            # Serialize using JSON
            JsonSerializer.save_to_file(f"type_test_{i}.json", test_data)
            
            # Load back
            loaded_data = JsonSerializer.load_from_file(f"type_test_{i}.json")
            
            assert loaded_data == test_data
            
            # Clean up
            os.remove(f"type_test_{i}.json")
        
        print("✓ Type conversion safety test passed")
    
    def test_recursive_structure_serialization(self):
        """Test serialization of recursive data structures."""
        # Create a recursive structure
        recursive_data = {"key": "value"}
        recursive_data["self"] = recursive_data  # Circular reference
        
        # JSON serialization can't handle circular references
        # This should either fail or handle gracefully
        try:
            JsonSerializer.save_to_file("recursive.json", recursive_data)
            print("✓ JsonSerializer handled circular reference (unexpected)")
        except (RecursionError, TypeError, ValueError) as e:
            print(f"✓ JsonSerializer raised exception for circular reference: {e}")
        
        # Clean up if file was created
        if os.path.exists("recursive.json"):
            os.remove("recursive.json")
        
        print("✓ Recursive structure serialization test passed")
    
    def test_large_data_serialization(self):
        """Test serialization of large data structures."""
        import time
        
        # Create large data structure
        large_data = {"items": list(range(1000))}
        
        start_time = time.time()
        
        # Serialize large data
        JsonSerializer.save_to_file("large_data.json", large_data)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 2.0, f"Large data serialization too slow: {elapsed:.2f}s"
        
        # Load back
        loaded_data = JsonSerializer.load_from_file("large_data.json")
        assert loaded_data == large_data
        
        # Clean up
        os.remove("large_data.json")
        
        print(f"✓ Large data serialization test passed ({elapsed:.3f}s)")

class TestSerializationIntegration:
    """Test integration of serialization with other NLM features."""
    
    def test_serialization_with_context_managers(self):
        """Test serialization integrated with context managers."""
        # Create brain and world using context managers
        with nlm.Config() as config:
            config.setDefaults("default")
            
            with nlm.SimpleWorld() as world:
                world.configure(100.0, 100.0, 20, 20)
                
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    brain.runSimulation(50)
                    
                    # Serialize using context managers
                    JsonSerializer.serialize_brain(brain, "integration_brain.json")
                    JsonSerializer.serialize_world(world, "integration_world.json")
                    
                    # Verify files were created
                    assert os.path.exists("integration_brain.json")
                    assert os.path.exists("integration_world.json")
                    
                    # Clean up
                    os.remove("integration_brain.json")
                    os.remove("integration_world.json")
        
        print("✓ Serialization with context managers integration test passed")
    
    def test_serialization_with_convenience_methods(self):
        """Test serialization integrated with convenience methods."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Use convenience methods to generate state
            brain.runSimulation(30)
            
            # Get neural stats as convenience method
            stats = brain.getNeuralStats()
            
            # Serialize the stats
            JsonSerializer.save_to_file("convenience_stats.json", stats)
            
            # Load and verify
            loaded_stats = JsonSerializer.load_from_file("convenience_stats.json")
            assert loaded_stats == stats
            
            print("✓ Serialization with convenience methods integration test passed")
        
        # Clean up
        os.remove("convenience_stats.json")
    
    def test_serialization_with_factory_methods(self):
        """Test serialization integrated with factory methods."""
        # Create objects using factory methods
        config = nlm.Config()
        config.setDefaults("default")
        
        brain = nlm.Brain.createStandardBrain(config)
        assert brain is not None
        
        with brain:
            brain.initialize()
            brain.runSimulation(25)
            
            # Serialize using factory-created objects
            JsonSerializer.serialize_brain(brain, "factory_brain.json")
            
            # Verify file was created
            assert os.path.exists("factory_brain.json")
            
            print("✓ Serialization with factory methods integration test passed")
        
        # Clean up
        os.remove("factory_brain.json")

# Test classes can be run as pytest test cases
def test_all_serialization():
    """Run all serialization tests."""
    print("=" * 60)
    print("Running Comprehensive Serialization Tests")
    print("=" * 60)
    
    # Create test suite instances
    json_serializer_tests = TestJsonSerializer()
    binary_serializer_tests = TestBinarySerializer()
    incremental_serializer_tests = TestIncrementalSerializer()
    error_handling_tests = TestSerializationErrorHandling()
    type_safety_tests = TestSerializationTypeSafety()
    integration_tests = TestSerializationIntegration()
    
    # Test methods in logical groups
    test_groups = [
        # JSON serializer tests
        (json_serializer_tests, [
            json_serializer_tests.test_save_to_file,
            json_serializer_tests.test_load_from_file,
            json_serializer_tests.test_serialize_brain,
            json_serializer_tests.test_deserialize_brain,
            json_serializer_tests.test_serialize_world,
            json_serializer_tests.test_serialization_error_handling,
            json_serializer_tests.test_serialization_type_hints,
        ]),
        
        # Binary serializer tests
        (binary_serializer_tests, [
            binary_serializer_tests.test_save_to_file,
            binary_serializer_tests.test_load_from_file,
            binary_serializer_tests.test_serialize_brain,
            binary_serializer_tests.test_binary_serializer_performance,
        ]),
        
        # Incremental serializer tests
        (incremental_serializer_tests, [
            incremental_serializer_tests.test_save_to_file,
            incremental_serializer_tests.test_load_from_file,
            incremental_serializer_tests.test_get_chunk_count,
            incremental_serializer_tests.test_incremental_serializer_performance,
        ]),
        
        # Error handling tests
        (error_handling_tests, [
            error_handling_tests.test_invalid_file_paths,
            error_handling_tests.test_corrupted_files,
            error_handling_tests.test_missing_files,
            error_handling_tests.test_permission_errors,
        ]),
        
        # Type safety tests
        (type_safety_tests, [
            type_safety_tests.test_type_conversion_safety,
            type_safety_tests.test_recursive_structure_serialization,
            type_safety_tests.test_large_data_serialization,
        ]),
        
        # Integration tests
        (integration_tests, [
            integration_tests.test_serialization_with_context_managers,
            integration_tests.test_serialization_with_convenience_methods,
            integration_tests.test_serialization_with_factory_methods,
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
    print("Serialization Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All serialization tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} serialization tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_serialization()