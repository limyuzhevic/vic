#!/usr/bin/env python3
"""
Comprehensive tests for Enhanced NLM Python API

This file contains unit tests for all enhanced features including:
- Enhanced configuration management
- Performance monitoring
- Advanced agent capabilities
- Learning experiment utilities
- Error handling and edge cases
"""

import sys
import os
import tempfile
import time
import unittest
from unittest.mock import patch, MagicMock
import pynlm_enhanced as nlm
class TestEnhancedConfig(unittest.TestCase):
    """Test EnhancedConfig class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = nlm.createEnhancedConfig()
    
    def test_default_creation(self):
        """Test that EnhancedConfig can be created."""
        self.assertIsNotNone(self.config)
        self.assertEqual(len(self.config.getAllKeys()), 0)  # Empty config
    
    def test_setup_default_brain(self):
        """Test default brain setup."""
        self.config.setupDefaultBrain(neuronCount=500, regionCount=2)
        
        self.assertTrue(self.config.getValueOr('neuron_count', 0) >= 500)
        self.assertTrue(self.config.getValueOr('region_count', 0) >= 2)
        self.assertEqual(self.config.getValueOr('random_seed', 0), 42)
    
    def test_setup_development_mode(self):
        """Test development mode setup."""
        self.config.setupDevelopmentMode()
        
        self.assertEqual(self.config.getValueOr('development_enabled', False), True)
        self.assertEqual(self.config.getValueOr('structural_plasticity_enabled', False), True)
    
    def test_setup_learning_mode(self):
        """Test learning mode setup."""
        self.config.setupLearningMode()
        
        self.assertEqual(self.config.getValueOr('reward_modulation_enabled', False), True)
        self.assertEqual(self.config.getValueOr('curiosity_enabled', False), True)
        self.assertEqual(self.config.getValueOr('novelty_detection_enabled', False), True)
        self.assertEqual(self.config.getValueOr('prediction_system_enabled', False), True)
    
    def test_setup_agent_mode(self):
        """Test agent mode setup."""
        self.config.setupAgentMode()
        
        self.assertEqual(self.config.getValueOr('agent_mode', False), True)
        self.assertEqual(self.config.getValueOr('sensory_processing_enabled', False), True)
        self.assertEqual(self.config.getValueOr('motor_decoding_enabled', False), True)
    
    def test_setup_high_performance(self):
        """Test high performance setup."""
        self.config.setupHighPerformance(threads=4)
        
        self.assertEqual(self.config.getValueOr('performance_parallel_neuron_updates', False), True)
        self.assertEqual(self.config.getValueOr('performance_use_memory_pools', False), True)
        self.assertEqual(self.config.getValueOr('performance_enable_simd', False), True)
    
    def test_get_value_or(self):
        """Test getValueOr method with defaults."""
        self.assertEqual(self.config.getValueOr('nonexistent_key', 'default_value'), 'default_value')
        self.assertEqual(self.config.getValueOr('neuron_count', 1000), 1000)
        
        # Set a value
        self.config.set('test_key', 42)
        self.assertEqual(self.config.getValueOr('test_key', 0), 42)
    
    def test_print_config(self):
        """Test config printing (should not raise exception)."""
        config_with_values = nlm.createEnhancedConfig()
        config_with_values.setupDefaultBrain()
        # Should not raise exception
        config_with_values.printConfig()
    
    def test_example_configs(self):
        """Test example configurations."""
        examples = nlm.getExampleConfigs()
        self.assertIn('default', examples)
        self.assertIn('learning', examples)
        self.assertIn('development', examples)
        
        self.assertIsInstance(examples['default'], str)
        self.assertIsInstance(examples['learning'], str)
        self.assertIsInstance(examples['development'], str)
    
    def test_version_constants(self):
        """Test version constants."""
        self.assertTrue(hasattr(nlm, 'VERSION'))
        self.assertTrue(hasattr(nlm, 'BUILD_TYPE'))
        self.assertEqual(nlm.BUILD_TYPE, 'Enhanced')
class TestEnhancedBrain(unittest.TestCase):
    """Test EnhancedBrain class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = nlm.createEnhancedConfig()
        self.config.setupDefaultBrain(neuronCount=100, regionCount=1)
        self.brain = nlm.createEnhancedBrain(self.config)
    
    def test_brain_creation(self):
        """Test EnhancedBrain creation."""
        self.assertIsNotNone(self.brain)
    
    def test_initialization(self):
        """Test brain initialization."""
        self.assertTrue(self.brain.initialize())
        
        self.assertGreaterEqual(self.brain.getNeuronCount(), 100)
        self.assertGreaterEqual(self.brain.getSynapseCount(), 0)
    
    def test_step_functionality(self):
        """Test brain stepping."""
        self.assertTrue(self.brain.initialize())
        
        # Test single step
        self.brain.step(0, 0.001, True)
        
        # Test batch processing
        self.brain.batchProcess(10, 0.001)
    
    def test_performance_metrics(self):
        """Test performance metrics collection."""
        self.assertTrue(self.brain.initialize())
        
        metrics = self.brain.getMetrics()
        self.assertIsNotNone(metrics)
        
        self.assertGreaterEqual(metrics.totalSpikeCount, 0)
        self.assertGreaterEqual(metrics.activeNeuronCount, 0)
        self.assertGreaterEqual(metrics.firingNeuronCount, 0)
        self.assertGreaterEqual(metrics.averageFiringRate, 0.0)
    
    def test_performance_report(self):
        """Test performance report generation."""
        self.assertTrue(self.brain.initialize())
        
        report = self.brain.getPerformanceReport()
        self.assertIsInstance(report, str)
        self.assertIn("Performance Report", report)
    
    def test_save_load(self):
        """Test brain save and load functionality."""
        self.assertTrue(self.brain.initialize())
        
        # Save brain state
        with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as tmp:
            tmp_path = tmp.name
        
        try:
            self.assertTrue(self.brain.save(tmp_path))
            self.assertTrue(os.path.exists(tmp_path))
            
            # Create new brain and load
            new_config = nlm.createEnhancedConfig()
            new_brain = nlm.createEnhancedBrain(new_config)
            self.assertTrue(new_brain.load(tmp_path))
            
        finally:
            if os.path.exists(tmp_path):
                os.unlink(tmp_path)
    
    def test_logging(self):
        """Test logging functionality."""
        self.assertTrue(self.brain.initialize())
        
        # Enable logging
        self.brain.enableLogging(True, "test_log.log")
        
        # Should not raise exception
        self.brain.step(0, 0.001, True)
class TestEnhancedAgent(unittest.TestCase):
    """Test EnhancedAgent class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = nlm.createEnhancedConfig()
        self.config.setupAgentMode()
        self.brain = nlm.createEnhancedBrain(self.config)
        self.agent = nlm.createEnhancedAgent(self.brain)
        
        # Setup world
        self.world = nlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    
    def test_agent_creation(self):
        """Test EnhancedAgent creation."""
        self.assertIsNotNone(self.agent)
    
    def test_agent_initialization(self):
        """Test agent initialization."""
        self.agent.initialize(self.world, enableExploration=True, enableLearning=True, enableDevelopment=True)
        
        # Should not raise exception
        self.agent.initialize(self.world, enableExploration=False, enableLearning=False, enableDevelopment=False)
    
    def test_execute_action(self):
        """Test action execution."""
        self.agent.initialize(self.world)
        
        # Execute action
        action = self.agent.executeAction(self.world, reward=0.5, predictedReward=0.3)
        
        self.assertIsNotNone(action)
        # Action should have been applied to world
        
        # Check agent status
        status = self.agent.getStatus()
        self.assertIsInstance(status, str)
        self.assertIn("Neuromodulation", status)
    
    def test_curiosity_exploration(self):
        """Test curiosity-driven exploration."""
        self.agent.initialize(self.world, enableExploration=True)
        
        # Set curiosity level high
        with patch.object(self.agent, 'getCuriosityLevel', return_value=0.8):
            exploration_action = self.agent.exploreWithCuriosity(self.world, threshold=0.5)
            self.assertIsNotNone(exploration_action)
    
    def test_feature_enabling(self):
        """Test feature enabling."""
        self.agent.initialize(self.world)
        
        # Test enabling features
        self.agent.enableFeature("reward_modulation", True)
        self.agent.enableFeature("curiosity", True)
        self.agent.enableFeature("development", True)
        self.agent.enableFeature("structural_plasticity", True)
        
        # Should not raise exception
class TestLearningExperiment(unittest.TestCase):
    """Test LearningExperiment class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = nlm.createEnhancedConfig()
        self.config.setupLearningMode()
        self.brain = nlm.createEnhancedBrain(self.config)
        self.experiment = nlm.createLearningExperiment(self.brain)
    
    def test_experiment_creation(self):
        """Test LearningExperiment creation."""
        self.assertIsNotNone(self.experiment)
    
    def test_run_learning_experiment(self):
        """Test running learning experiment."""
        # Should not raise exception
        self.experiment.runLearningExperiment(duration=1.0, verbose=False)
    
    def test_inject_stimuli(self):
        """Test learning stimuli injection."""
        # This is tested indirectly through runLearningExperiment
        pass
class TestIntegration(unittest.TestCase):
    """Test integration between components."""
    
    def test_full_workflow(self):
        """Test complete workflow with all components."""
        # Create configuration
        config = nlm.createEnhancedConfig()
        config.setupAgentMode()
        config.setupLearningMode()
        
        # Create brain and agent
        brain = nlm.createEnhancedBrain(config)
        agent = nlm.createEnhancedAgent(brain)
        
        # Setup world
        world = nlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        
        # Initialize agent
        agent.initialize(world)
        
        # Run simulation
        for step in range(10):
            action = agent.executeAction(world, reward=0.1, predictedReward=0.05)
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Check status
            status = agent.getStatus()
            self.assertIn("Neuromodulation", status)
        
        # Get final metrics
        metrics = brain.getMetrics()
        self.assertIsNotNone(metrics)
    
    def test_performance_monitoring(self):
        """Test performance monitoring integration."""
        config = nlm.createEnhancedConfig()
        config.setupHighPerformance()
        
        brain = nlm.createEnhancedBrain(config)
        
        if brain.initialize():
            brain.batchProcess(20, timestep=0.001)
            
            # Check performance metrics
            metrics = brain.getMetrics()
            report = brain.getPerformanceReport()
            
            self.assertIsInstance(metrics, nlm.PerformanceMonitor.Metrics)
            self.assertIsInstance(report, str)
            self.assertIn("Performance Score", report)
class TestErrorHandling(unittest.TestCase):
    """Test error handling and edge cases."""
    
    def test_invalid_config_values(self):
        """Test handling of invalid configuration values."""
        config = nlm.createEnhancedConfig()
        
        # Should not raise exception for invalid operations
        config.setupHighPerformance(threads=-1)  # Negative threads
        config.setupDefaultBrain(neuronCount=0)  # Zero neurons
        
        # Should still work
        brain = nlm.createEnhancedBrain(config)
        # Initialization might fail, but shouldn't crash
    
    def test_empty_world(self):
        """Test behavior with empty world."""
        config = nlm.createEnhancedConfig()
        brain = nlm.createEnhancedBrain(config)
        agent = nlm.createEnhancedAgent(brain)
        
        world = nlm.createSimpleWorld()
        world.configure(width=5, height=5)
        
        # Should not crash
        agent.initialize(world)
        action = agent.executeAction(world)
    
    def test_performance_monitor_reset(self):
        """Test performance monitor reset."""
        config = nlm.createEnhancedConfig()
        brain = nlm.createEnhancedBrain(config)
        
        if brain.initialize():
            brain.step(0, 0.001, True)
            
            # Reset monitor
            brain.getMonitor().reset()
            
            # Continue processing
            brain.step(1, 0.001, True)
    
    def test_multiple_instances(self):
        """Test creating multiple instances."""
        # Create multiple configs
        configs = [nlm.createEnhancedConfig() for _ in range(3)]
        
        for i, config in enumerate(configs):
            config.setupDefaultBrain(neuronCount=100 * (i + 1))
            brain = nlm.createEnhancedBrain(config)
            if brain.initialize():
                brain.batchProcess(5)
            # Each should work independently
class TestPerformance(unittest.TestCase):
    """Performance-related tests."""
    
    def test_batch_processing_performance(self):
        """Test batch processing performance."""
        config = nlm.createEnhancedConfig()
        config.setupHighPerformance()
        
        brain = nlm.createEnhancedBrain(config)
        
        if brain.initialize():
            start_time = time.time()
            brain.batchProcess(100, timestep=0.001)
            elapsed = time.time() - start_time
            
            # Should complete within reasonable time
            self.assertLess(elapsed, 5.0)  # 5 seconds max
            
            # Check performance score
            report = brain.getPerformanceReport()
            self.assertIn("Performance Score", report)
    
    def test_memory_efficiency(self):
        """Test memory efficiency with large configurations."""
        config = nlm.createEnhancedConfig()
        config.setupDefaultBrain(neuronCount=5000, regionCount=10)
        
        brain = nlm.createEnhancedBrain(config)
        
        if brain.initialize():
            # Process some steps
            brain.batchProcess(50)
            
            # Should not crash with large configuration
            metrics = brain.getMetrics()
            self.assertIsNotNone(metrics)
def run_all_tests():
    """Run all tests and report results."""
    print("=" * 80)
    print("Enhanced NLM Python API Test Suite")
    print("=" * 80)
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
    result = runner.run(suite)
    
    # Print summary
    print("\n" + "=" * 80)
    print("Test Summary")
    print("=" * 80)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Success rate: {((result.testsRun - len(result.failures) - len(result.errors)) / result.testsRun * 100):.1f}%")
    
    if result.failures:
        print("\nFailures:")
        for test, traceback in result.failures:
            print(f"- {test}: {traceback}")
    
    if result.errors:
        print("\nErrors:")
        for test, traceback in result.errors:
            print(f"- {test}: {traceback}")
    
    return result.wasSuccessful()
if __name__ == "__main__":
    success = run_all_tests()
    
    if success:
        print("\n✓ All tests passed!")
        sys.exit(0)
    else:
        print("\n✗ Some tests failed!")
        sys.exit(1)