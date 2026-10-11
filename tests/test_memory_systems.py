"""Test memory systems integration of enhanced NLM Python bindings."""
import pytest
from pynlm import Brain, Config
import pynlm.utils as utils

class TestMemorySystems:
    """Test memory systems integration."""
    
    def test_working_memory_integration(self):
        """Test working memory system integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test working memory access
        working_memory = brain.get_working_memory()
        
        # In stub implementation, should return None
        assert working_memory is None
        
        # Test memory-related operations
        assert hasattr(brain, 'log_status')
        brain.log_status()
        
        # Test memory state tracking
        brain.reset()
        brain.initialize()
        
        working_memory2 = brain.get_working_memory()
        assert working_memory2 is None
    
    def test_episodic_memory_integration(self):
        """Test episodic memory system integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test episodic memory access
        episodic_memory = brain.get_episodic_memory()
        assert episodic_memory is None
        
        # Test episodic memory operations
        assert hasattr(brain, 'log_status')
        brain.log_status()
        
        # Test episodic memory state
        run_steps(brain, 10)
        
        episodic_memory2 = brain.get_episodic_memory()
        assert episodic_memory2 is None
    
    def test_associative_memory_integration(self):
        """Test associative memory system integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test associative memory access
        associative_memory = brain.get_associative_memory()
        assert associative_memory is None
        
        # Test associative memory operations
        assert hasattr(brain, 'log_status')
        brain.log_status()
        
        # Test associative memory state
        run_steps(brain, 20)
        
        associative_memory2 = brain.get_associative_memory()
        assert associative_memory2 is None

class TestMemorySystemIntegration:
    """Test comprehensive memory system integration."""
    
    def test_complete_memory_system_access(self):
        """Test complete memory system access."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Access all memory systems
        memory_systems = {
            'working_memory': brain.get_working_memory(),
            'episodic_memory': brain.get_episodic_memory(),
            'associative_memory': brain.get_associative_memory(),
            'prediction_system': brain.get_prediction_system(),
            'planner': brain.get_planner(),
            'concept_formation': brain.get_concept_formation(),
            'attention': brain.get_attention(),
            'development_system': brain.get_development_system(),
            'dopamine': brain.get_dopamine(),
            'curiosity': brain.get_curiosity(),
            'novelty': brain.get_novelty(),
            'prediction_error': brain.get_prediction_error_signal(),
            'stdp': brain.get_stdp(),
            'hebbian': brain.get_hebbian(),
            'structural_plasticity': brain.get_structural_plasticity(),
            'spike_system': brain.get_spike_system()
        }
        
        # All should return None in stub implementation
        for system_name, system in memory_systems.items():
            assert system is None, f"{system_name} should return None in stub"
        
        # Verify memory operations
        assert hasattr(brain, 'log_status')
        brain.log_status()
        
        # Test memory state after operations
        run_steps(brain, 10)
        brain.log_status()
    
    def test_memory_system_workflow(self):
        """Test complete memory system workflow."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Initialize brain
        assert brain.initialize() is True
        
        # Log initial state
        brain.log_status()
        
        # Perform operations
        run_steps(brain, 10)
        
        # Log state after operations
        brain.log_status()
        
        # Verify memory systems
        working_memory = brain.get_working_memory()
        episodic_memory = brain.get_episodic_memory()
        associative_memory = brain.get_associative_memory()
        
        assert working_memory is None
        assert episodic_memory is None
        assert associative_memory is None
        
        # Reset and verify recovery
        brain.reset()
        brain.initialize()
        
        working_memory_reset = brain.get_working_memory()
        assert working_memory_reset is None
    
    def test_memory_system_consistency(self):
        """Test memory system consistency."""
        config = Config()
        
        # Create multiple brains
        brains = [Brain(config) for _ in range(5)]
        
        for i, brain in enumerate(brains):
            brain.initialize()
            run_steps(brain, i + 1)
            
            # All memory systems should behave consistently
            working_memory = brain.get_working_memory()
            episodic_memory = brain.get_episodic_memory()
            associative_memory = brain.get_associative_memory()
            
            assert working_memory is None
            assert episodic_memory is None
            assert associative_memory is None
            
            # All should log status
            brain.log_status()
    
    def test_memory_system_error_handling(self):
        """Test memory system error handling."""
        config = Config()
        brain = Brain(config)
        
        # Test memory operations on uninitialized brain
        with pytest.raises(pynlm.NotInitializedError):
            brain.get_working_memory()
        
        with pytest.raises(pynlm.NotInitializedError):
            brain.get_episodic_memory()
        
        with pytest.raises(pynlm.NotInitializedError):
            brain.get_associative_memory()
        
        # Initialize and test again
        assert brain.initialize() is True
        
        working_memory = brain.get_working_memory()
        episodic_memory = brain.get_episodic_memory()
        associative_memory = brain.get_associative_memory()
        
        assert working_memory is None
        assert episodic_memory is None
        assert associative_memory is None

class TestMemorySystemPerformance:
    """Test memory system performance."""
    
    def test_memory_system_performance(self):
        """Test memory system performance characteristics."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test performance with memory operations
        start_time = time.time()
        
        # Perform multiple operations with memory access
        for _ in range(50):
            brain.step()
            brain.get_working_memory()
            brain.get_episodic_memory()
            brain.get_associative_memory()
            brain.log_status()
        
        end_time = time.time()
        
        elapsed = end_time - start_time
        ops_per_second = 200 / elapsed  # 200 operations (50 steps * 4 memory ops)
        
        print(f"Memory system performance: {ops_per_second:.0f} ops/s")
        
        # Should have reasonable performance
        assert ops_per_second > 100, f"Memory system performance too low: {ops_per_second} ops/s"
    
    def test_memory_system_scaling(self):
        """Test memory system scaling with increasing operations."""
        config = Config()
        
        operation_counts = [10, 50, 100, 200]
        
        for count in operation_counts:
            config = Config()
            brain = Brain(config)
            brain.initialize()
            
            start_time = time.time()
            
            for _ in range(count):
                brain.step()
                brain.get_working_memory()
                brain.get_episodic_memory()
                brain.log_status()
            
            end_time = time.time()
            
            elapsed = end_time - start_time
            ops_per_second = (count * 4) / elapsed  # 4 operations per iteration
            
            print(f"Count {count}: {ops_per_second:.0f} ops/s")
            
            # Should maintain decent performance
            assert elapsed < 10.0, f"Scaling test for {count} operations too slow: {elapsed}s"
            assert ops_per_second > 100, f"Scaling test performance too low: {ops_per_second} ops/s"

class TestMemorySystemValidation:
    """Test memory system validation."""
    
    def test_memory_system_validation(self):
        """Test memory system validation utilities."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test validation utilities
        assert utils.validate_brain(brain) is True
        
        # Test with uninitialized brain
        uninitialized_brain = Brain()
        # validate_brain should handle this gracefully
        result = utils.validate_brain(uninitialized_brain)
        assert isinstance(result, bool)
    
    def test_memory_system_brain_comparison(self):
        """Test brain comparison with memory systems."""
        config = Config()
        brain1 = Brain(config)
        brain2 = Brain(config)
        
        brain1.initialize()
        brain2.initialize()
        
        # Run different operations
        run_steps(brain1, 10)
        run_steps(brain2, 5)
        
        # Compare brains (includes memory system state)
        comparison = utils.compare_brains(brain1, brain2)
        assert isinstance(comparison, dict)
        assert 'has_differences' in comparison
        
        # They should have differences
        assert comparison['has_differences'] is True
        
        # Check memory system comparison
        assert 'brain1_working_memory' in comparison
        assert 'brain2_working_memory' in comparison
        assert 'brain1_episodic_memory' in comparison
        assert 'brain2_episodic_memory' in comparison
        
        # All should be None in stub
        assert comparison['brain1_working_memory'] is None
        assert comparison['brain2_working_memory'] is None
        assert comparison['brain1_episodic_memory'] is None
        assert comparison['brain2_episodic_memory'] is None

# Helper function (used by memory system tests)
def run_steps(brain, steps):
    """Helper to run multiple steps."""
    for _ in range(steps):
        brain.step()

# Import for error handling
import time
import pytest
import pynlm