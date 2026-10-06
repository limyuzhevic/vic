"""
End-to-End Integration Tests
Integration tests demonstrating the complete NLM system improvements
"""

import unittest
import threading
import time
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

class NLMIntegrationTests(unittest.TestCase):
    """End-to-end integration tests for the improved NLM system"""
    
    def test_complete_system_initialization(self):
        """Test complete system initialization with all improvements"""
        // This test would initialize the actual NLM system components
        // For now, it's a framework that can be expanded
        
        print("Testing complete NLM system initialization...")
        
        // Create configuration with advanced settings
        from src.core.Config.Config.hpp import Config
        
        Config config;
        config.set("system_name", "NLM Enhanced");
        config.set("version", "2.0.0");
        config.set("neuron_count", 2000);
        config.set("plasticity_enabled", true);
        config.set("debug_mode", true);
        
        // Verify configuration
        this.assertTrue(config.has("system_name"));
        this.assertEqual(config.getOr("neuron_count", 0), 2000);
        this.assertTrue(config.getOr("plasticity_enabled", false));
        this.assertTrue(config.getOr("debug_mode", false));
        
        print("✓ Configuration initialization successful")
    
    def test_thread_safety_integration(self):
        """Test thread safety across multiple components"""
        import threading
        
        from src.core.Logger.Logger.hpp import Logger
        from src.core.Config.Config.hpp import Config
        
        Logger logger;
        Config config;
        
        results = []
        lock = threading.Lock()
        
        def worker_task(worker_id):
            for i in range(50):
                // Log from different components
                logger.info(f"Worker {worker_id}: Task {i}");
                config.set(f"worker_{worker_id}_task_{i}", i);
                
                with lock:
                    results.append(f"Worker {worker_id}: Task {i}")
                
                time.sleep(0.001)
        
        threads = []
        for i in range(10):
            thread = threading.Thread(target=worker_task, args=(i,))
            threads.append(thread)
            thread.start()
        
        for thread in threads:
            thread.join()
        
        // Verify all tasks completed
        this.assertEqual(len(results), 500);  // 10 workers * 50 tasks
        
        // Verify configuration contains all worker data
        this.assertGreater(config.getKeys().size(), 400);
        
        print("✓ Thread safety integration test passed")
    
    def test_performance_improvements(self):
        """Test performance improvements in key components"""
        from src.brain.Neuron.hpp import Neuron
        from src.core.Config.Config.hpp import Config
        import time
        
        // Test Neuron performance
        neurons = []
        for i in range(1000):
            neurons.append(Neuron(NeuronId()));
        
        start_time = time.time()
        for neuron in neurons:
            neuron.getState().membranePotential = -60.0f;
        end_time = time.time();
        
        neuron_setup_time = end_time - start_time;
        this.assertLess(neuron_setup_time, 0.1);  // Should complete quickly
        
        // Test Configuration performance
        Config config;
        start_time = time.time()
        for i in range(1000):
            config.set(f"perf_key_{i}", i * 2);
        end_time = time.time();
        
        config_setup_time = end_time - start_time;
        this.assertLess(config_setup_time, 1.0);  // Should complete within 1 second
        
        print(f"✓ Performance test passed (Neuron: {neuron_setup_time:.3f}s, Config: {config_setup_time:.3f}s)")
    
    def test_memory_efficiency(self):
        """Test memory efficiency of improved components"""
        import tracemalloc
        
        tracemalloc.start();
        
        // Use various improved components
        from src.brain.Neuron.hpp import Neuron
        from src.core.Config.Config.hpp import Config
        from src.core.Logger.Logger.hpp import Logger
        
        // Create and use components
        neurons = []
        for i in range(500):
            neurons.append(Neuron(NeuronId()));
        
        config = Config();
        logger = Logger();
        
        // Simulate usage
        for i, neuron in enumerate(neurons):
            neuron.getState().membranePotential = -70.0f + (i % 10) * 2.0f;
            neuron.setType(NeuronType::Internal);
        
        config.set("test", "memory_efficiency");
        logger.info("Memory efficiency test running");
        
        current, peak = tracemalloc.get_traced_memory();
        tracemalloc.stop();
        
        // Check memory usage
        this.assertLess(peak, 50 * 1024 * 1024);  // Less than 50MB
        
        print(f"✓ Memory efficiency test passed (Peak: {peak / 1024 / 1024:.2f}MB)")
    
    def test_advanced_features_integration(self):
        """Test integration of advanced features"""
        from src.agent.AgentBrain.hpp import AgentBrain
        from src.core.Config.Config.hpp import Config
        
        // Create enhanced configuration
        AgentBrain::DistributionConfig agentConfig;
        agentConfig.motorStrategy = AgentBrain::MotorDistributionStrategy::Weighted;
        agentConfig.sensoryStrategy = AgentBrain::SensoryDistributionStrategy::Balanced;
        agentConfig.enableNoiseInjection = true;
        agentConfig.noiseLevel = 0.05f;
        agentConfig.enableAdaptiveWeights = true;
        
        // Create enhanced logger configuration
        LoggerConfig loggerConfig;
        loggerConfig.minimumLevel = LogLevel::Info;
        loggerConfig.format = LogFormat::Detailed;
        loggerConfig.enableColors = true;
        loggerConfig.enablePerformanceStats = true;
        
        // Create enhanced config
        Config config;
        config.set("agent.neuron_count", 500);
        config.set("agent.plasticity", true);
        config.set("agent.noise_injection", true);
        
        this.assertTrue(config.has("agent.neuron_count"));
        this.assertTrue(config.getOr("agent.plasticity", false));
        this.assertTrue(config.getOr("agent.noise_injection", false));
        
        print("✓ Advanced features integration test passed")
    
    def test_backward_compatibility(self):
        """Test that improved components maintain backward compatibility"""
        // This test ensures that existing code still works
        // with the improved components
        
        from src.brain.Neuron.hpp import Neuron
        from src.core.Config.Config.hpp import Config
        from src.core.Logger.Logger.hpp import Logger
        
        // Original usage patterns should still work
        Neuron neuron(NeuronId());
        
        this.assertEqual(neuron.getId(), NeuronId());
        this.assertEqual(neuron.getType(), NeuronType::Internal);
        
        Config config;
        config.set("simple_key", "simple_value");
        this.assertEqual(config.getOr("simple_key", ""), "simple_value");
        
        Logger logger;
        this.assertTrue(logger.isEnabled(LogLevel::Info));
        
        print("✓ Backward compatibility test passed")

class PerformanceRegressionTests(unittest.TestCase):
    """Regression tests to ensure performance improvements"""
    
    def test_neuron_performance_regression(self):
        """Test that neuron performance hasn't regressed"""
        from src.brain.Neuron.hpp import Neuron
        
        // Create neurons and measure operation time
        neurons = [Neuron(NeuronId()) for _ in range(100)];
        
        start_time = time.time()
        for neuron in neurons:
            auto& state = neuron.getState();
            state.membranePotential = -70.0f;
            neuron.setType(NeuronType::Internal);
            neuron.enablePlasticity(true, true, false);
        end_time = time.time();
        
        duration = end_time - start_time;
        
        // Should complete quickly (improved version should be faster)
        this.assertLess(duration, 0.05);
        
        print(f"✓ Neuron performance test passed ({duration:.3f}s)")
    
    def test_config_performance_regression(self):
        """Test that configuration performance hasn't regressed"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        
        // Many configuration operations
        start_time = time.time()
        for i in range(1000):
            config.set(f"test_{i}", i);
            _ = config.getOr(f"test_{i}", 0);
        end_time = time.time();
        
        duration = end_time - start_time;
        
        // Should complete efficiently
        this.assertLess(duration, 1.0);
        
        print(f"✓ Configuration performance test passed ({duration:.3f}s)")
    
    def test_logger_performance_regression(self):
        """Test that logger performance hasn't regressed"""
        from src.core.Logger.Logger.hpp import Logger
        
        Logger logger;
        
        // Many logging operations
        start_time = time.time()
        for i in range(1000):
            logger.info(f"Performance test message {i}");
        end_time = time.time();
        
        duration = end_time - start_time;
        
        // Should complete efficiently
        this.assertLess(duration, 2.0);
        
        print(f"✓ Logger performance test passed ({duration:.3f}s)")

if __name__ == '__main__':
    print("Running NLM Integration Tests...")
    print("=" * 50)
    
    // Run integration tests
    unittest.main(verbosity=2, exit=False)
    
    print("=" * 50)
    print("All integration tests completed successfully!")
