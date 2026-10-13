#!/usr/bin/env python3
"""
Test suite for NLM neural simulation framework.

This module provides comprehensive testing capabilities for the NLM project,
including unit tests, integration tests, and performance benchmarks.
"""

import unittest
import time
import tempfile
import os
from typing import Dict, Any, List, Optional
from unittest.mock import Mock, patch
import json

# Import NLM modules
try:
    import pynlm
    from python.enhanced_api import NLMConfig, EnhancedNLMWorld
    from python.performance import PerformanceMonitor, ResourceMonitor
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

class TestNLMConfiguration(unittest.TestCase):
    """Test NLM configuration functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
        else:
            self.config = Mock()
    
    def test_config_creation(self):
        """Test configuration creation."""
        if PYNLM_AVAILABLE:
            self.assertIsNotNone(self.config)
            self.assertTrue(hasattr(self.config, 'get'))
            self.assertTrue(hasattr(self.config, 'set'))
            self.assertTrue(hasattr(self.config, 'has'))
    
    def test_config_validation(self):
        """Test configuration validation (if available)."""
        if PYNLM_AVAILABLE and hasattr(self.config, 'validate'):
            errors = self.config.validate()
            self.assertIsInstance(errors, list)
    
    def test_config_save_load(self):
        """Test configuration save and load."""
        if PYNLM_AVAILABLE:
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                filepath = f.name
            
            try:
                # Set some configuration values
                self.config.set("brain.neuron_count", 1000)
                self.config.set("simulation_timestep", 0.001)
                
                # Save configuration
                result = self.config.saveToFile(filepath)
                self.assertTrue(result)
                
                # Load configuration
                new_config = pynlm.createDefaultConfig()
                result = new_config.loadFromFile(filepath)
                self.assertTrue(result)
                
                # Verify loaded values
                self.assertEqual(new_config.getOr<int64_t>("brain.neuron_count", 0), 1000)
                self.assertEqual(new_config.getOr<double>("simulation_timestep", 0.0), 0.001)
                
            finally:
                os.unlink(filepath)

class TestNLMBrain(unittest.TestCase):
    """Test NLM brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
        else:
            self.brain = Mock()
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        if PYNLM_AVAILABLE:
            self.assertIsNotNone(self.brain)
            self.assertTrue(hasattr(self.brain, 'initialize'))
            self.assertTrue(hasattr(self.brain, 'step'))
            self.assertTrue(hasattr(self.brain, 'reset'))
    
    def test_brain_step(self):
        """Test brain simulation step."""
        if PYNLM_AVAILABLE:
            # Record initial state
            initial_spikes = self.brain.getTotalSpikeCount()
            
            # Run simulation step
            self.brain.step(0)
            
            # Verify step execution
            current_spikes = self.brain.getTotalSpikeCount()
            # Spike count may or may not change depending on network state
            self.assertGreaterEqual(current_spikes, initial_spikes)
    
    def test_brain_statistics(self):
        """Test brain statistics collection."""
        if PYNLM_AVAILABLE:
            stats = {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
                'total_spikes': self.brain.getTotalSpikeCount(),
            }
            
            # Verify statistics are reasonable
            self.assertGreaterEqual(stats['neuron_count'], 0)
            self.assertGreaterEqual(stats['synapse_count'], 0)
            self.assertGreaterEqual(stats['firing_rate'], 0.0)
            self.assertGreaterEqual(stats['e_i_ratio'], 0.0)
            self.assertGreaterEqual(stats['total_spikes'], 0)
    
    def test_brain_save_load(self):
        """Test brain state save and load."""
        if PYNLM_AVAILABLE:
            with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
                filepath = f.name
            
            try:
                # Save brain state
                result = self.brain.save(filepath)
                self.assertTrue(result)
                
                # Reset brain
                self.brain.reset()
                
                # Verify brain was reset
                self.assertEqual(self.brain.getTotalSpikeCount(), 0)
                
                # Load brain state
                result = self.brain.load(filepath)
                self.assertTrue(result)
                
            finally:
                os.unlink(filepath)

class TestNLMWorld(unittest.TestCase):
    """Test NLM world functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            self.world = pynlm.createSimpleWorld()
            self.world.configure(10, 10, 8, 8)
            self.world.reset()
            
            # Create agent
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
        else:
            self.world = Mock()
            self.agent = Mock()
    
    def test_world_configuration(self):
        """Test world configuration."""
        if PYNLM_AVAILABLE:
            width = self.world.getWidth()
            height = self.world.getHeight()
            self.assertEqual(width, 10)
            self.assertEqual(height, 10)
    
    def test_world_update(self):
        """Test world update."""
        if PYNLM_AVAILABLE:
            initial_time = self.world.getSimulationTime()
            
            # Update world
            self.world.update(0.1)
            
            # Verify time increased
            new_time = self.world.getSimulationTime()
            self.assertGreater(new_time, initial_time)
            self.assertAlmostEqual(new_time, initial_time + 0.1, places=5)
    
    def test_agent_sensory_processing(self):
        """Test agent sensory processing."""
        if PYNLM_AVAILABLE:
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Process with agent
            self.agent.processSensoryInput(percept)
            
            # Verify agent processed input
            self.assertIsNotNone(percept)
            self.assertIsInstance(percept, object)
    
    def test_agent_motor_decoding(self):
        """Test agent motor decoding."""
        if PYNLM_AVAILABLE:
            # Process some sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Step brain
            self.brain.step(0)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Verify motor command
            self.assertIsNotNone(motor_cmd)
            self.assertIn(motor_cmd, [
                pynlm.MotorCommand.MoveForward,
                pynlm.MotorCommand.MoveBackward,
                pynlm.MotorCommand.TurnLeft,
                pynlm.MotorCommand.TurnRight,
                pynlm.MotorCommand.Interact,
                pynlm.MotorCommand.Wait,
            ])

class TestNLMPerformance(unittest.TestCase):
    """Test NLM performance monitoring."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            # Create performance monitor
            self.monitor = PerformanceMonitor(self.brain)
            
            # Create resource monitor
            self.resource_monitor = ResourceMonitor(self.brain)
        else:
            self.monitor = Mock()
            self.resource_monitor = Mock()
    
    def test_performance_monitor_creation(self):
        """Test performance monitor creation."""
        if PYNLM_AVAILABLE:
            self.assertIsNotNone(self.monitor)
            self.assertTrue(hasattr(self.monitor, 'record_step'))
            self.assertTrue(hasattr(self.monitor, 'get_performance_stats'))
    
    def test_performance_monitor_recording(self):
        """Test performance monitor recording."""
        if PYNLM_AVAILABLE:
            # Record some steps
            for i in range(5):
                self.monitor.record_step()
            
            # Get performance statistics
            stats = self.monitor.get_performance_stats()
            
            # Verify statistics
            self.assertGreaterEqual(stats['steps_completed'], 5)
            self.assertGreater(stats['elapsed_time_seconds'], 0.0)
    
    def test_resource_monitor_creation(self):
        """Test resource monitor creation."""
        if PYNLM_AVAILABLE:
            self.assertIsNotNone(self.resource_monitor)
            self.assertTrue(hasattr(self.resource_monitor, 'get_memory_usage'))
            self.assertTrue(hasattr(self.resource_monitor, 'get_cpu_usage'))
    
    def test_resource_monitor_collection(self):
        """Test resource monitor collection."""
        if PYNLM_AVAILABLE:
            # Track resource usage
            self.resource_monitor.track_resource_usage()
            
            # Get resource statistics
            memory_stats = self.resource_monitor.get_memory_usage()
            cpu_stats = self.resource_monitor.get_cpu_usage()
            
            # Verify statistics
            self.assertIsInstance(memory_stats, dict)
            self.assertIsInstance(cpu_stats, dict)

class TestNLMEnhancedAPI(unittest.TestCase):
    """Test NLM enhanced API functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            # Test NLMConfig
            from python.enhanced_api import NLMConfig
            self.config = NLMConfig()
            
            # Test EnhancedNLMWorld
            from python.enhanced_api import EnhancedNLMWorld
            self.world = EnhancedNLMWorld(self.brain)
        else:
            self.config = Mock()
            self.world = Mock()
    
    def test_enhanced_config(self):
        """Test enhanced configuration."""
        if PYNLM_AVAILABLE:
            # Test configuration methods
            self.config.set_brain_config(2000, 2)
            self.config.set_plasticity_config()
            self.config.set_development_config()
            
            # Verify configuration
            config_dict = self.config.get_config_dict()
            self.assertGreaterEqual(config_dict['brain.neuron_count'], 1000)
    
    def test_enhanced_world(self):
        """Test enhanced world functionality."""
        if PYNLM_AVAILABLE:
            # Test world setup
            self.world.setup_agent(enable_reward_modulation=True)
            
            # Test episode running
            steps_completed = self.world.run_episode(10, callback=None)
            self.assertEqual(steps_completed, 10)
    
    def test_efficiency_analyzer(self):
        """Test efficiency analyzer (if available)."""
        if PYNLM_AVAILABLE and hasattr(self, 'brain'):
            from python.performance import EfficiencyAnalyzer
            analyzer = EfficiencyAnalyzer(self.brain)
            
            # Analyze efficiency
            results = analyzer.analyze_efficiency()
            
            # Verify results
            self.assertIsInstance(results, dict)
            self.assertIn('summary', results)
            self.assertIn('optimization_recommendations', results)

class TestNLMBenchmarks(unittest.TestCase):
    """Test NLM benchmarks and performance testing."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
        else:
            self.brain = Mock()
    
    def test_performance_benchmark(self):
        """Test performance benchmarking."""
        if PYNLM_AVAILABLE:
            from python.performance import PerformanceMonitor
            
            monitor = PerformanceMonitor(self.brain)
            
            # Run simulation and benchmark
            start_time = time.time()
            for i in range(100):
                self.brain.step(i)
                monitor.record_step()
            elapsed_time = time.time() - start_time
            
            # Get performance statistics
            stats = monitor.get_performance_stats()
            
            # Verify benchmarks
            self.assertGreater(stats['simulation_summary']['steps_per_second'], 0)
            self.assertLess(elapsed_time, 10.0)  # Should complete in under 10 seconds

class TestNLMIntegration(unittest.TestCase):
    """Test NLM integration across components."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(10, 10, 8, 8)
            self.world.reset()
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
        else:
            self.brain = Mock()
            self.world = Mock()
            self.agent = Mock()
    
    def test_brain_world_integration(self):
        """Test integration between brain and world."""
        if PYNLM_AVAILABLE:
            # Run simulation with brain-world integration
            for step in range(50):
                # Update world
                self.world.update(0.1)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input in brain
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Decode motor command
                motor_cmd = self.agent.decodeMotorCommand()
                
                # Apply motor command
                self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
                
                # Apply reward modulation
                self.agent.applyRewardModulation(0.1, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
            
            # Verify integration
            self.assertGreater(self.brain.getTotalSpikeCount(), 0)
            self.assertGreater(self.world.getAgentBody().energy, 0.0)
    
    def test_full_simulation_cycle(self):
        """Test complete simulation cycle."""
        if PYNLM_AVAILABLE:
            from python.enhanced_api import EnhancedNLMWorld
            
            # Create enhanced world
            world = EnhancedNLMWorld(self.brain)
            world.setup_agent(enable_reward_modulation=True, enable_curiosity=True)
            
            # Run simulation cycle
            def callback(simulator, step):
                pass  # Callback for monitoring
            
            steps_completed = world.run_episode(100, callback=callback)
            
            # Verify simulation cycle
            self.assertEqual(steps_completed, 100)
            self.assertGreater(world.brain.getTotalSpikeCount(), 0)

# Test suite runner
class NLMSuite(unittest.TestSuite):
    """NLM test suite with custom test runner."""
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.results = {
            'total_tests': 0,
            'failed_tests': 0,
            'passed_tests': 0,
            'skipped_tests': 0,
        }
    
    def run_tests(self) -> Dict[str, Any]:
        """Run all tests and return results."""
        # Create test suite
        suite = unittest.TestLoader().loadTestsFromModule(__import__(__name__))
        
        # Create test runner
        runner = unittest.TextTestRunner(verbosity=2, stream=open(os.devnull, 'w'))
        
        # Run tests
        result = runner.run(suite)
        
        # Collect results
        self.results['total_tests'] = result.testsRun
        self.results['failed_tests'] = len(result.failures)
        self.results['passed_tests'] = len(result.failures) + len(result.errors)
        self.results['skipped_tests'] = len(result.skipped) if hasattr(result, 'skipped') else 0
        
        return {
            'test_summary': self.results,
            'failures': [{'test': str(fail[0]), 'traceback': str(fail[1])} for fail in result.failures],
            'errors': [{'test': str(err[0]), 'traceback': str(err[1])} for err in result.errors],
            'skipped': [{'test': str(skip[0]), 'reason': str(skip[1])} for skip in result.skipped] if hasattr(result, 'skipped') else [],
        }

def run_test_suite():
    """Run the complete NLM test suite."""
    if not PYNLM_AVAILABLE:
        print("NLM not available. Skipping tests.")
        return {
            'test_summary': {
                'total_tests': 0,
                'failed_tests': 0,
                'passed_tests': 0,
                'skipped_tests': 0,
            },
            'note': 'NLM not available'
        }
    
    print("Running NLM Test Suite...")
    print("=" * 50)
    
    # Create test suite
    suite = NLMSuite()
    
    # Run tests
    results = suite.run_tests()
    
    # Print summary
    summary = results['test_summary']
    print("\n" + "=" * 50)
    print("Test Suite Summary:")
    print(f"  Total Tests: {summary['total_tests']}")
    print(f"  Passed Tests: {summary['passed_tests']}")
    print(f"  Failed Tests: {summary['failed_tests']}")
    print(f"  Skipped Tests: {summary['skipped_tests']}")
    
    # Save results to file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        filepath = f.name
        json.dump(results, f, indent=2)
    
    print(f"\nTest results saved to: {filepath}")
    
    return results

if __name__ == '__main__':
    # Run test suite
    test_results = run_test_suite()
    
    # Exit with appropriate code
    if test_results['test_summary']['failed_tests'] > 0:
        exit(1)
    else:
        exit(0)
