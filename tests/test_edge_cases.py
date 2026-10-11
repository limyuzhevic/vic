"""Test edge cases of enhanced NLM Python bindings."""
import pytest
from pynlm import Brain, Config, NeuronId, SynapseId, RegionId, PopulationId
import pynlm.utils as utils

class TestEdgeCases:
    """Test edge cases and boundary conditions."""
    
    def test_invalid_inputs(self):
        """Test handling of invalid inputs."""
        brain = Brain()
        
        # Test invalid configuration
        config = Config()
        config.set("invalid_key", "should_work_in_stub")
        
        brain_with_config = Brain(config)
        assert brain_with_config is not None
        
        # Test invalid neuron IDs
        try:
            brain.inject_current(NeuronId(-1), 1.0)
            assert False, "Should have raised InvalidInputError"
        except pynlm.InvalidInputError:
            pass  # Expected
        
        try:
            brain.inject_current(NeuronId(999999), 1.0)
            assert False, "Should have raised InvalidInputError"
        except pynlm.InvalidInputError:
            pass  # Expected
        
        # Test invalid current values
        try:
            brain.inject_current(NeuronId(1), float('nan'))
            assert False, "Should have raised InvalidInputError"
        except pynlm.InvalidInputError:
            pass  # Expected
        
        try:
            brain.inject_current(NeuronId(1), float('inf'))
            assert False, "Should have raised InvalidInputError"
        except pynlm.InvalidInputError:
            pass  # Expected
    
    def test_brain_state_validation(self):
        """Test brain state validation."""
        brain = Brain()
        
        # Uninitialized brain should raise errors
        with pytest.raises(pynlm.NotInitializedError):
            brain.step()
        
        with pytest.raises(pynlm.NotInitializedError):
            _ = brain.spike_count
        
        with pytest.raises(pynlm.NotInitializedError):
            _ = brain.active_neurons
        
        with pytest.raises(pynlm.NotInitializedError):
            _ = brain.ei_ratio
        
        # Initialize brain
        assert brain.initialize() is True
        
        # Should work after initialization
        assert brain.step() is True
        assert brain.spike_count >= 0
    
    def test_edge_case_batch_operations(self, brain):
        """Test edge cases for batch operations."""
        brain.initialize()
        
        # Zero batch size
        result = brain.step_batch(0)
        assert result == 0
        assert brain.spike_count == 0
        
        # Single step batch
        result = brain.step_batch(1)
        assert result == 1
        assert brain.spike_count == 1
        
        # Large batch
        large_batch = 1000
        result = brain.step_batch(large_batch)
        assert result == large_batch
        assert brain.spike_count == large_batch
        
        # Very large batch (stress test)
        very_large_batch = 10000
        result = brain.step_batch(very_large_batch)
        assert result == very_large_batch
        assert brain.spike_count == very_large_batch
    
    def test_id_type_edge_cases(self):
        """Test ID type edge cases."""
        # Test NeuronId edge cases
        assert NeuronId(0).value == 0
        assert NeuronId(1).value == 1
        assert NeuronId(-1).value == -1
        assert NeuronId(999999).value == 999999
        
        # Test SynapseId edge cases
        assert SynapseId(0).value == 0
        assert SynapseId(-1).value == -1
        
        # Test RegionId edge cases
        assert RegionId(0).value == 0
        assert RegionId(999).value == 999
        
        # Test PopulationId edge cases
        assert PopulationId(0).value == 0
        assert PopulationId(-100).value == -100
        
        # Test ID equality
        id1 = NeuronId(5)
        id2 = NeuronId(5)
        id3 = NeuronId(10)
        
        assert id1 == id2
        assert id1 != id3
        assert id1 != "not an id"
        
        # Test ID hashing
        id_set = {id1, id2, id3}
        assert len(id_set) == 2  # id1 and id2 are the same
    
    def test_enum_edge_cases(self):
        """Test enumeration edge cases."""
        # Test all enum values are defined
        assert pynlm.NeuronType.Excitatory == 0
        assert pynlm.NeuronType.Inhibitory == 1
        assert pynlm.NeuronType.Modulatory == 2
        assert pynlm.NeuronType.Sensory == 3
        assert pynlm.NeuronType.Motor == 4
        assert pynlm.NeuronType.Internal == 5
        
        # Test that enum values are integers
        assert isinstance(pynlm.NeuronType.Excitatory, int)
        assert isinstance(pynlm.NeuronType.Inhibitory, int)
        
        # Test SynapseType
        assert pynlm.SynapseType.Electrical == 3
        assert pynlm.SynapseType.GapJunction == 4
        
        # Test DevelopmentalStage
        assert pynlm.DevelopmentalStage.Aging == 4
        
        # Test ActionType
        assert pynlm.ActionType.Wait == 12
        assert pynlm.ActionType.Custom == 14
        
        # Test WorldObjectType
        assert pynlm.WorldObjectType.Hazard == 2
        assert pynlm.WorldObjectType.Wall == 3
    
    def test_constant_edge_cases(self):
        """Test constant edge cases."""
        assert pynlm.PI > 3.0
        assert pynlm.PI < 4.0
        
        assert pynlm.EPSILON > 0
        assert pynlm.EPSILON < 0.1
        
        assert pynlm.MAX_SIMULATION_STEPS > 0
        assert pynlm.MAX_SIMULATION_STEPS == 1000000
        
        assert pynlm.DEFAULT_RANDOM_SEED >= 0
        assert pynlm.DEFAULT_RANDOM_SEED == 42
    
    def test_file_operation_edge_cases(self, brain, temp_file):
        """Test file operation edge cases."""
        brain.initialize()
        
        # Test save with empty filename
        with pytest.raises(pynlm.InvalidInputError):
            brain.save("")
        
        # Test save with None filename
        with pytest.raises(pynlm.InvalidInputError):
            brain.save(None)
        
        # Test load with empty filename
        with pytest.raises(pynlm.InvalidInputError):
            brain.load("")
        
        # Test load with None filename
        with pytest.raises(pynlm.InvalidInputError):
            brain.load(None)
        
        # Test convenience wrappers with edge cases
        assert brain.save_to_file("") is False
        assert brain.load_from_file("") is False
    
    def test_configuration_edge_cases(self):
        """Test configuration edge cases."""
        config = Config()
        
        # Test empty config
        assert config.get("non_existent_key") is None
        
        # Test setting various types
        config.set("int_value", 42)
        config.set("float_value", 3.14)
        config.set("string_value", "test")
        config.set("bool_value", True)
        config.set("list_value", [1, 2, 3])
        config.set("dict_value", {"key": "value"})
        
        # Test getting with defaults
        assert config.get("int_value", 0) == 42
        assert config.get("non_existent", "default") == "default"
        
        # Test config summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0

class TestErrorEdgeCases:
    """Test edge cases for error handling."""
    
    def test_error_message_quality(self):
        """Test that error messages are descriptive."""
        brain = Brain()
        
        # Test error messages for uninitialized operations
        try:
            brain.step()
        except pynlm.NotInitializedError as e:
            assert "initialized" in str(e).lower()
            assert len(str(e)) > 10
        
        # Test error messages for invalid input
        try:
            brain.save("")
        except pynlm.InvalidInputError as e:
            assert "cannot be empty" in str(e).lower()
            assert len(str(e)) > 10
        
        # Test error messages for invalid configuration
        try:
            # This should work in stub implementation
            brain.initialize()
        except pynlm.InvalidConfigurationError as e:
            assert "configuration" in str(e).lower()
    
    def test_error_chaining(self):
        """Test error chaining and context."""
        brain = Brain()
        
        # Create nested error scenarios
        try:
            brain.reset()
            brain.step()
        except pynlm.NotInitializedError:
            pass  # Expected
        
        # Error should have descriptive message
        try:
            brain.step()
        except pynlm.NotInitializedError as e:
            # Error should provide context about what went wrong
            assert "step" in str(e).lower() or "operation" in str(e).lower()
    
    def test_error_recovery(self):
        """Test error recovery capabilities."""
        brain = Brain()
        
        # Generate error
        try:
            brain.step()
        except pynlm.NotInitializedError:
            pass  # Expected
        
        # Recover and continue
        assert brain.initialize() is True
        assert brain.step() is True  # Should work after recovery

class TestPerformanceEdgeCases:
    """Test edge cases for performance."""
    
    def test_zero_operations(self, brain):
        """Test performance with zero operations."""
        brain.initialize()
        
        # Zero batch should complete quickly
        start_time = time.time()
        result = brain.step_batch(0)
        end_time = time.time()
        
        assert result == 0
        assert brain.spike_count == 0
        assert (end_time - start_time) < 0.001  # Should be very fast
    
    def test_single_operation(self, brain):
        """Test performance with single operation."""
        brain.initialize()
        
        # Single operation should complete quickly
        start_time = time.time()
        result = brain.step()
        end_time = time.time()
        
        assert result is True
        assert (end_time - start_time) < 0.001  # Should be very fast
    
    def test_large_batch_stress(self, brain):
        """Test stress with large batch sizes."""
        brain.initialize()
        
        # Large but reasonable batch
        large_batch = 5000
        
        start_time = time.time()
        result = brain.step_batch(large_batch)
        end_time = time.time()
        
        elapsed = end_time - start_time
        assert result == large_batch
        
        # Should complete in reasonable time
        assert elapsed < 5.0, f"Large batch too slow: {elapsed}s"
        
        # Should maintain decent throughput
        ops_per_second = large_batch / elapsed
        assert ops_per_second > 1000, f"Throughput too low: {ops_per_second} ops/s"

# Helper function (used by edge case tests)
import time
def run_steps(brain, steps):
    """Helper to run multiple steps."""
    for _ in range(steps):
        brain.step()