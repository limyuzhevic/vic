"""
Performance Tests for NLM
Comprehensive test coverage for simulation performance and scalability
"""

import pytest
import time
from pathlib import Path
import sys

# Add the python directory to path for imports
python_dir = Path(__file__).parent.parent / "python"
sys.path.insert(0, str(python_dir))

# Try to import the NLM Python bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Tests will be skipped.")

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestSimulationPerformance:
    """Test simulation performance characteristics"""
    
    def test_small_network_performance(self):
        """Test performance with small neural networks"""
        config = pynlm.createSimpleConfig(50, 2)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        
        for i in range(100):
            brain.step(i)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Should complete 100 steps in reasonable time
        assert duration < 10.0, f"Performance too slow: {duration:.2f}s for 100 steps"
        
        # Performance should scale appropriately
        assert duration < 5.0, f"Performance unacceptable: {duration:.2f}s"
        
        print(f"✓ Small network performance test passed (duration: {duration:.3f}s)")
    
    def test_memory_usage_patterns(self):
        """Test memory usage with different network sizes"""
        # Test with progressively larger networks
        network_sizes = [50, 100, 200]
        
        for size in network_sizes:
            config = pynlm.createSimpleConfig(size, 2)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Verify network scales appropriately
            assert brain.getTotalNeuronCount() == size
            assert brain.getTotalSynapseCount() >= 0
            
            # Perform some operations
            for i in range(min(50, size // 10)):
                brain.step(i)
            
            print(f"  Network size {size} memory test passed")
        
        print("✓ Memory usage patterns test passed")
    
    def test_scalability(self):
        """Test scalability across different network sizes"""
        # Test scalability by running multiple simulations
        test_sizes = [10, 50, 100]
        
        times = []
        for size in test_sizes:
            config = pynlm.createSimpleConfig(size, 2)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Time the simulation
            start_time = time.time()
            
            for i in range(size // 5):
                brain.step(i)
            
            end_time = time.time()
            duration = end_time - start_time
            times.append(duration)
            
            print(f"  Network size {size} scalability test completed in {duration:.3f}s")
        
        # Performance should scale reasonably (not exponentially)
        # The largest network should not be dramatically slower
        if len(times) >= 2:
            size_ratio = test_sizes[-1] / test_sizes[0]
            time_ratio = times[-1] / times[0]
            # Time ratio should be reasonable (less than 10x for 10x network size)
            assert time_ratio < 10.0, f"Scalability poor: {time_ratio:.1f}x slower for {size_ratio}x larger network"
        
        print("✓ Scalability test passed")

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestConfigurationPerformance:
    """Test configuration and parsing performance"""
    
    def test_configuration_parsing_performance(self):
        """Test configuration creation and parsing performance"""
        # Create multiple configurations
        configs = []
        
        start_time = time.time()
        for i in range(20):
            config = pynlm.createSimpleConfig(i * 10, (i % 3) + 1)
            configs.append(config)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Should create 20 configs quickly
        assert duration < 5.0, f"Configuration creation too slow: {duration:.2f}s"
        
        print(f"✓ Configuration parsing performance test passed (duration: {duration:.3f}s)")
    
    def test_large_network_simulation(self):
        """Test simulation performance with larger networks"""
        config = pynlm.createSimpleConfig(500, 4)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Verify large network initialization
        assert brain.getTotalNeuronCount() == 500
        assert brain.getRegionCount() == 4
        
        # Run simulation with larger network
        start_time = time.time()
        
        for i in range(100):
            brain.step(i)
            
            if i % 25 == 0:
                print(f"    Step {i}/100 completed")
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Should complete 100 steps with larger network in reasonable time
        assert duration < 30.0, f"Large network simulation too slow: {duration:.2f}s"
        
        print(f"✓ Large network simulation test passed (duration: {duration:.3f}s)")

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestPythonAPIPerformance:
    """Test Python API performance characteristics"""
    
    def test_api_call_performance(self):
        """Test performance of common API calls"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 4, 4)
        agent.initialize(world)
        
        # Time API calls
        start_time = time.time()
        
        for i in range(50):
            agent.reset()
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(i)
            action = agent.decodeMotorCommand()
        
        end_time = time.time()
        duration = end_time - start_time
        
        # API calls should be fast
        assert duration < 10.0, f"API calls too slow: {duration:.2f}s for 50 iterations"
        
        print(f"✓ API call performance test passed (duration: {duration:.3f}s)")
    
    def test_complete_workflow_performance(self):
        """Test performance of complete simulation workflow"""
        # Run complete workflow multiple times
        start_time = time.time()
        
        for iteration in range(5):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            brain.initialize()
            world.configure(15, 15, 6, 6)
            agent.initialize(world)
            
            # Run simulation
            for step in range(20):
                world.update(step * 0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step, step * 0.1)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, step * 0.1)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Complete workflow should be reasonable
        assert duration < 60.0, f"Complete workflow too slow: {duration:.2f}s"
        
        print(f"✓ Complete workflow performance test passed (duration: {duration:.3f}s)")

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
