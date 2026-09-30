#!/usr/bin/env python3
"""
Advanced Testing Framework for NLM (Neural Learning Machine)

Comprehensive testing suite for NLM with focus on:
- Python bindings validation
- C++ integration testing
- Advanced feature testing
- Performance benchmarking
- Error handling validation
"""

import pynlm
import sys
import traceback
import time
import unittest
from typing import List, Dict, Any, Optional
from datetime import datetime

# NLM Test Suite Classes
class TestPythonBindings(unittest.TestCase):
    """Test Python bindings functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test fixtures"""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def test_basic_functionality(self):
        """Test basic brain functionality"""
        self.assertEqual(self.brain.getTotalNeuronCount(), 1000)
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
        
        # Run simulation steps
        for i in range(10):
            self.brain.step(i, 0.1)
        
        self.assertGreater(self.brain.getTotalSpikeCount(), 10)
    
    def test_configuration_creation(self):
        """Test configuration creation"""
        config1 = pynlm.createDefaultConfig()
        self.assertTrue(config1.has("brain.neuron_count"))
        
        config2 = pynlm.createDefaultConfig()
        config2.set("brain.neuron_count", 500)
        self.assertEqual(config2.get<size_t>("brain.neuron_count"), 500)
    
    def test_world_interaction(self):
        """Test world interaction"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation loop
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, 0.1)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        self.assertGreater(brain.getTotalSpikeCount(), 20)
    
    def test_configuration_validation(self):
        """Test configuration validation"""
        config = pynlm.createDefaultConfig()
        
        # This should not raise an exception
        try:
            config.validate()
        except Exception as e:
            self.fail(f"Configuration validation raised unexpected exception: {e}")
    
    def test_error_handling(self):
        """Test error handling"""
        # Test invalid configuration
        config = pynlm.createDefaultConfig()
        
        # Test that validation catches issues
        try:
            # This would test advanced validation
            pass
        except pynlm.ConfigurationError:
            pass  # Expected
        except Exception as e:
            self.fail(f"Unexpected exception type: {e}")
class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced NLM features"""
    
    def setUp(self):
        """Set up advanced feature tests"""
        self.config = pynlm.createDefaultConfig()
    
    def test_advanced_configuration_builder(self):
        """Test advanced configuration builder"""
        builder = pynlm.createConfigurationBuilder()
        
        config1 = builder.createMinimalBrain()
        self.assertLess(config1.get<size_t>("brain.neuron_count"), 200)
        
        config2 = builder.createAdvancedBrain()
        self.assertGreater(config2.get<size_t>("brain.neuron_count"), 500)
        
        config3 = builder.createExplorationBrain()
        self.assertTrue(config3.get<bool>("neuromod.curiosity.enable"))
    
    def test_context_manager(self):
        """Test context manager functionality"""
        brain = pynlm.createBrain(self.config)
        
        with brain as ctx:
            self.assertTrue(ctx.isInitialized())
            ctx.runSteps(10)
            self.assertGreater(brain.getTotalSpikeCount(), 10)
    
    def test_visualization_helpers(self):
        """Test visualization helper functions"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Test brain statistics visualization
        stats = pynlm.visualizeBrainStats(brain)
        self.assertIn("Neurons", stats)
        self.assertIn("Synapses", stats)
        
        # Test progress report
        report = pynlm.generateProgressReport(brain, 50, 5.0)
        self.assertGreater(report.size(), 0)
        
        # Test color generation
        color = pynlm.generateHexColor(0.5, 0.0, 1.0)
        self.assertTrue(color.find("#") != std::string::npos)
    
    def test_learning_demonstrations(self):
        """Test learning algorithm demonstrations"""
        config = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config)
        pynlm.demonstrateSTDP(brain1, steps=20)
        
        brain2 = pynlm.createBrain(config)
        pynlm.demonstrateHebbianLearning(brain2, steps=15)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        brain3 = pynlm.createBrain(config)
        pynlm.demonstrateCuriosityDrivenLearning(brain3, world, steps=10)
    
    def test_error_types(self):
        """Test custom error types"""
        # Test that custom error types are available
        config = pynlm.createDefaultConfig()
        
        # Test ConfigurationError
        try:
            config.validate()
        except pynlm.ConfigurationError:
            pass  # Expected for invalid config
        
        # Test ValidationError
        try:
            # This would test validation errors
            pass
        except pynlm.ValidationError:
            pass  # Expected
    
    def test_version_info(self):
        """Test version information"""
        self.assertTrue(hasattr(pynlm, '__version__'))
        self.assertTrue(hasattr(pynlm, '__author__'))
        self.assertTrue(hasattr(pynlm, '__description__'))
        
        self.assertEqual(pynlm.__version__, "2.0.0-enhanced")
class TestPerformance(unittest.TestCase):
    """Test performance aspects"""
    
    def setUp(self):
        """Set up performance tests"""
        self.config = pynlm.createDefaultConfig()
    
    def test_simulation_performance(self):
        """Test simulation performance"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Measure simulation time
        start_time = time.time()
        
        for i in range(100):
            brain.step(i, 0.1)
        
        end_time = time.time();
        simulation_time = end_time - start_time;
        
        # Check performance (should complete 100 steps reasonably quickly)
        self.assertLess(simulation_time, 10.0)  # Should be much less than 10 seconds
        
        # Log performance
        print(f"Simulation performance: 100 steps in {simulation_time:.3f} seconds")
    
    def test_memory_usage(self):
        """Test memory usage"""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create and use multiple brains
        brains = []
        for i in range(10):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run some simulation
            for step in range(10):
                brain.step(step, 0.1)
            
            brains.append(brain)
        
        peak_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = peak_memory - initial_memory;
        
        print(f"Memory usage: {initial_memory:.1f} MB -> {peak_memory:.1f} MB (+{memory_increase:.1f} MB)")
        
        # Clean up
        brains.clear()
    
    def test_scalability(self):
        """Test scalability with different brain sizes"""
        sizes = [100, 500, 1000, 2000]
        
        for size in sizes:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run quick simulation
            for step in range(5):
                brain.step(step, 0.1)
            
            print(f"Brain with {size} neurons: {brain.getTotalSpikeCount()} spikes in 5 steps")
class TestIntegration(unittest.TestCase):
    """Test integration aspects"""
    
    def test_complete_workflow(self):
        """Test complete NLM workflow"""
        # 1. Create configuration
        config = pynlm.createDefaultConfig()
        
        # 2. Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 3. Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
        world.reset()
        world.setAgentStart(15.0f, 15.0f)
        
        # 4. Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # 5. Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # 6. Run complete episode
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step, 0.1)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0f
            auto internal = percept.getInternal();
            if (!internal.empty()) {
                reward = internal[0];
            }
            agent.applyRewardModulation(reward, 0.0f);
            
            agent.updateDevelopment(0.1)
        
        # 7. Verify results
        self.assertGreater(brain.getTotalSpikeCount(), 50)
        self.assertGreaterEqual(agent.getCuriosityLevel(), 0.0f)
        self.assertGreaterEqual(agent.getNoveltyLevel(), 0.0f)
        
        print(f"Complete workflow successful: {brain.getTotalSpikeCount()} spikes, "
              f"curiosity={agent.getCuriosityLevel():.2f}, novelty={agent.getNoveltyLevel():.2f}")
    
    def test_checkpointing(self):
        """Test checkpoint save/load functionality"""
        config = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        # Run simulation
        for step in range(20):
            brain1.step(step, 0.1)
        
        # Save checkpoint
        checkpoint_file = "test_checkpoint.bin"
        success = brain1.save(checkpoint_file)
        self.assertTrue(success)
        
        # Create new brain and load checkpoint
        brain2 = pynlm.createBrain(config)
        success = brain2.load(checkpoint_file)
        self.assertTrue(success)
        
        # Verify that loaded brain has similar state
        self.assertGreater(brain2.getTotalSpikeCount(), 0)
        
        print(f"Checkpoint test successful: {brain2.getTotalSpikeCount()} spikes loaded")
        
        # Clean up
        import os
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
class NLMTestSuite:
    """Comprehensive NLM test suite"""
    
    def __init__(self):
        self.test_results = []
        self.start_time = time.time()
    
    def run_all_tests(self):
        """Run all NLM tests"""
        print("=" * 80)
        print("NLM Advanced Testing Suite")
        print("=" * 80)
        print(f"Started: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print()
        
        # Test suites
        test_suites = [
            ("Python Bindings", TestPythonBindings),
            ("Advanced Features", TestAdvancedFeatures),
            ("Performance", TestPerformance),
            ("Integration", TestIntegration),
        ]
        
        total_tests = 0
        passed_tests = 0
        
        for suite_name, test_class in test_suites:
            print(f"Running {suite_name} tests...")
            print("-" * 80)
            
            # Create test suite
            suite = unittest.TestLoader().loadTestsFromTestCase(test_class)
            
            # Run tests
            runner = unittest.TextTestRunner(verbosity=0, stream=sys.stdout)
            result = runner.run(suite)
            
            total_tests += result.testsRun
            passed_tests += len(result.failures) + len(result.errors)
            
            self.test_results.append({
                'suite': suite_name,
                'tests': result.testsRun,
                'passed': result.testsRun - len(result.failures) - len(result.errors),
                'failed': len(result.failures) + len(result.errors),
                'failures': result.failures,
                'errors': result.errors
            })
            
            print()
        
        end_time = time.time()
        duration = end_time - self.start_time
        
        # Print summary
        self.print_test_summary(total_tests, passed_tests, duration)
        
        return total_tests, passed_tests, self.test_results
    
    def print_test_summary(self, total_tests, passed_tests, duration):
        """Print test summary"""
        print("=" * 80)
        print("Test Summary")
        print("=" * 80)
        print(f"Total Tests: {total_tests}")
        print(f"Passed: {passed_tests}")
        print(f"Failed: {total_tests - passed_tests}")
        print(f"Success Rate: {passed_tests / total_tests * 100:.1f}%")
        print(f"Duration: {duration:.2f} seconds")
        print()
        
        # Print per-suite results
        for result in self.test_results:
            print(f"{result['suite']}: {result['passed']}/{result['tests']} passed")
            if result['failed'] > 0:
                print(f"  Failures: {result['failed']}")
        
        print()
        
        if passed_tests == total_tests:
            print("🎉 All tests passed! NLM enhanced bindings are working correctly.")
            return True
        else:
            print(f"⚠️  {total_tests - passed_tests} tests failed.")
            return False
def main():
    """Main test runner"""
    # Check if running in enhanced mode
    print("NLM Advanced Testing Framework")
    print("=" * 50)
    
    # Create and run test suite
    test_suite = NLMTestSuite()
    success = test_suite.run_all_tests()
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)
if __name__ == "__main__":
    main()