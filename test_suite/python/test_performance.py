"""
Performance and Stress Tests

Tests for performance benchmarking, stress testing, and scalability
"""
import pytest
import sys
import os
import time
import psutil
import gc
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

class TestLargeNetworkPerformance:
    """Tests for large network performance"""
    
    def test_1000_neurons_performance(self):
        """Test performance with ~1000 neurons"""
        import time
        
        config = pynlm.createDefaultConfig()
        start_time = time.time()
        
        # Create many neurons (simulated)
        neurons = []
        for i in range(1000):
            # In a real implementation, this would create neurons in C++
            neurons.append(pynlm.Config())  # Using Config as placeholder
        
        creation_time = time.time() - start_time
        
        # Should be able to create 1000 objects quickly
        assert len(neurons) == 1000
        assert creation_time < 5.0  # Should complete in reasonable time
    
    def test_10000_synapses_performance(self):
        """Test performance with ~10000 synapses"""
        import time
        
        start_time = time.time()
        
        # Create many synapse-like objects
        synapses = []
        for i in range(10000):
            synapses.append(pynlm.Config())  # Using Config as placeholder
        
        creation_time = time.time() - start_time
        
        assert len(synapses) == 10000
        assert creation_time < 10.0  # Should complete in reasonable time
    
    def test_memory_efficiency(self):
        """Test memory efficiency with large objects"""
        import psutil
        import gc
        
        process = psutil.Process()
        gc.collect()
        
        initial_memory = process.memory_info().rss
        
        # Create many objects
        objects = []
        for i in range(1000):
            objects.append(pynlm.Config())
            objects.append(pynlm.Vision(10, 10, 3))
            objects.append(pynlm.Action(pynlm.ActionType.MoveForward))
        
        peak_memory = process.memory_info().rss
        
        # Clean up
        del objects
        gc.collect()
        
        final_memory = process.memory_info().rss
        
        memory_growth = peak_memory - initial_memory
        assert memory_growth < 100 * 1024 * 1024  # Less than 100MB growth

class TestLongRunPerformance:
    """Tests for long-running simulation performance"""
    
    def test_checkpoint_restart_cycles(self):
        """Test checkpoint save/load performance"""
        import tempfile
        import os
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for i in range(100):
            brain.step(i)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(delete=False, suffix='.nlm') as f:
            checkpoint_file = f.name
        
        try:
            save_start = time.time()
            brain.save(checkpoint_file)
            save_time = time.time() - save_start
            
            # Load checkpoint
            load_start = time.time()
            brain.load(checkpoint_file)
            load_time = time.time() - load_start
            
            # Should complete in reasonable time
            assert save_time < 5.0
            assert load_time < 5.0
            
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
    
    def test_long_simulation_runs(self):
        """Test performance over extended simulation runs"""
        import time
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run extended simulation
        start_time = time.time()
        steps = 1000
        
        for i in range(steps):
            brain.step(i)
        
        total_time = time.time() - start_time
        
        # Should complete in reasonable time
        assert total_time < 60.0  # Should complete within 1 minute
        
        # Calculate performance
        steps_per_second = steps / total_time
        assert steps_per_second > 10.0  # Should be able to do at least 10 steps/second

class TestStressTesting:
    """Stress testing for edge cases and error conditions"""
    
    def test_extreme_memory_usage(self):
        """Test behavior under extreme memory pressure"""
        import gc
        import psutil
        
        process = psutil.Process()
        gc.collect()
        
        # Create maximum reasonable number of objects
        objects = []
        try:
            for i in range(5000):  # Large but reasonable number
                objects.append(pynlm.Config())
                objects.append(pynlm.Vision(50, 50, 3))
                objects.append(pynlm.Action(pynlm.ActionType.Custom))
            
            # Should not crash
            assert len(objects) == 15000
            
        except MemoryError:
            # Memory error is acceptable in stress test
            pass
        
        # Clean up
        del objects
        gc.collect()
    
    def test_concurrent_access(self):
        """Test concurrent access to shared resources"""
        import threading
        import time
        
        results = []
        errors = []
        lock = threading.Lock()
        
        def worker(worker_id):
            try:
                config = pynlm.Config()
                config.set(f"worker_{worker_id}", f"value_{worker_id}")
                
                with lock:
                    results.append(worker_id)
                    
                time.sleep(0.001)  # Simulate work
                
            except Exception as e:
                with lock:
                    errors.append(str(e))
        
        threads = []
        for i in range(50):  # Many concurrent threads
            thread = threading.Thread(target=worker, args=(i,))
            threads.append(thread)
            thread.start()
        
        for thread in threads:
            thread.join()
        
        # All threads should complete without errors
        assert len(errors) == 0
        assert len(results) == 50
    
    def test_configuration_extremes(self):
        """Test configuration with extreme values"""
        config = pynlm.createDefaultConfig()
        
        # Test with extreme values
        config.set("extreme_int", 2**31 - 1)  # Max int32
        config.set("extreme_float", 1e20)      # Large float
        config.set("extreme_string", "x" * 1000)  # Long string
        
        # Should accept extreme values without crashing
        assert config.has("extreme_int")
        assert config.has("extreme_float")
        assert config.has("extreme_string")

if __name__ == "__main__":
    pytest.main([__file__, "-v"])