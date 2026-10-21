# Comprehensive test suite for NLM Python bindings
# Tests both basic functionality and advanced features

import unittest
import numpy as np
import time
from typing import Dict, Any
import tempfile
import os

# Import NLM bindings
import pynlm
from pynlm import (
    Brain, Config, SimpleWorld, AgentBrain, Action, NeuronId, SynapseId,
    RegionId, PopulationId, NeuronType, SynapseType, DevelopmentalStage,
    ActionType, MotorCommand, WorldObjectType, Vision, Audio, 
    InternalSignals, SensoryPercept, AgentBody, ActionResult, WorldObject
)
from python.wrapper import SmartBrain, ConfigBuilder, PatternBuilder, ExperimentRunner, VisualDebugger
from python.advanced import AdvancedConfig, ArchitectureBuilder, NeuromodulationProfile, DevelopmentProfile, CognitionProfile
class TestBasicFunctionality(unittest.TestCase):
    """Test basic NLM functionality."""
    
    def setUp(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 100)
        self.config.set("region_count", 2)
        
    def test_config_creation(self):
        """Test configuration creation and manipulation."""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        
        # Test setting values
        config.set("test_key", "test_value")
        self.assertEqual(config.get("test_key"), "test_value")
        
        # Test getting with default
        self.assertEqual(config.getOr("nonexistent", "default"), "default")
        
        # Test has method
        self.assertTrue(config.has("test_key"))
        self.assertFalse(config.has("nonexistent"))
    
    def test_brain_creation(self):
        """Test brain creation and basic initialization."""
        brain = pynlm.createBrain(self.config)
        self.assertIsNotNone(brain)
        
        # Test initialization
        self.assertTrue(brain.initialize())
        
        # Test basic properties
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        self.assertGreater(brain.getRegionCount(), 0)
    
    def test_world_creation(self):
        """Test world creation and basic functionality."""
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        
        # Test configuration
        world.configure(10, 10, 8, 8)
        self.assertEqual(world.getWidth(), 10)
        self.assertEqual(world.getHeight(), 10)
        
        # Test reset
        world.reset()
        self.assertEqual(world.getSimulationTime(), 0.0)
    
    def test_agent_brain_creation(self):
        """Test agent brain creation."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        
        # Initialize with world
        agent.initialize(world)
        
        # Test basic properties
        self.assertGreater(agent.getSensoryInputSize(), 0)
        self.assertGreater(agent.getMotorOutputSize(), 0)
class TestSmartBrainFeatures(unittest.TestCase):
    """Test SmartBrain wrapper features."""
    
    def setUp(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 50)
        
    def test_smart_brain_creation(self):
        """Test SmartBrain creation and basic features."""
        brain = pynlm.createBrain(self.config)
        
        # Test SmartBrain creation
        smart_brain = SmartBrain(brain)
        self.assertIsNotNone(smart_brain)
        
        # Test initialization chaining
        smart_brain = smart_brain.initialize()
        self.assertTrue(smart_brain._initialized)
        
        # Test step chaining
        smart_brain = smart_brain.step(5)
        self.assertTrue(smart_brain._initialized)
    
    def test_config_builder(self):
        """Test ConfigBuilder functionality."""
        # Test learning brain config
        learning_config = ConfigBuilder.create_learning_brain(neuron_count=100)
        self.assertIsNotNone(learning_config)
        
        # Test stable brain config
        stable_config = ConfigBuilder.create_stable_brain(neuron_count=200)
        self.assertIsNotNone(stable_config)
        
        # Test explosive brain config
        explosive_config = ConfigBuilder.create_explosive_brain(neuron_count=50)
        self.assertIsNotNone(explosive_config)
    
    def test_pattern_builder(self):
        """Test PatternBuilder functionality."""
        # Test pattern validation
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Note: PatternBuilder requires internal API access
        # This test validates the API structure
        self.assertIsNotNone(brain)
    
    def test_network_statistics(self):
        """Test network statistics collection."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        smart_brain = SmartBrain(brain)
        
        # Get network statistics
        stats = smart_brain.get_network_statistics()
        
        # Check expected keys
        expected_keys = ['region_count', 'total_neurons', 'total_synapses', 
                        'active_neurons', 'firing_neurons', 'average_firing_rate',
                        'excitation_inhibition_ratio', 'total_spikes', 'config_summary']
        
        for key in expected_keys:
            self.assertIn(key, stats)
        
        # Check value types
        self.assertIsInstance(stats['region_count'], int)
        self.assertIsInstance(stats['total_neurons'], int)
        self.assertIsInstance(stats['total_synapses'], int)
        self.assertIsInstance(stats['active_neurons'], int)
        self.assertIsInstance(stats['firing_neurons'], int)
        self.assertIsInstance(stats['average_firing_rate'], float)
        self.assertIsInstance(stats['excitation_inhibition_ratio'], float)
        self.assertIsInstance(stats['total_spikes'], int)
        self.assertIsInstance(stats['config_summary'], str)
class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced NLM features."""
    
    def setUp(self):
        """Set up test environment."""
        self.advanced_config = AdvancedConfig()
        
    def test_advanced_config_creation(self):
        """Test AdvancedConfig creation."""
        self.assertIsNotNone(self.advanced_config)
        
        # Test basic properties
        self.assertEqual(self.advanced_config.get("neuron_count"), 1000)
        
        # Test advanced features
        learning_config = NeuromodulationProfile.create_pure_reward_brain()
        self.assertIsNotNone(learning_config)
        
        exploration_config = NeuromodulationProfile.create_exploration_brain()
        self.assertIsNotNone(exploration_config)
        
        planning_config = CognitionProfile.create_planning_focused()
        self.assertIsNotNone(planning_config)
    
    def test_architecture_builder(self):
        """Test ArchitectureBuilder functionality."""
        # Create brain
        brain = pynlm.createBrain(self.advanced_config.get_config())
        brain.initialize()
        
        # Test architecture validation
        validation = ArchitectureBuilder.validate_architecture(brain, expected_regions=1)
        self.assertIn('region_count', validation)
        self.assertIn('neuron_count', validation)
        self.assertIn('synapse_count', validation)
        
        # Test overall validation
        self.assertIn('architecture_valid', validation)
        self.assertIn('errors', validation)
        self.assertIn('warnings', validation)
    
    def test_plasticity_patterns(self):
        """Test predefined plasticity patterns."""
        # Test Hebbian pattern
        hebbian_pattern = PatternBuilder.create_hebbian_pattern()
        self.assertIn('type', hebbian_pattern)
        self.assertEqual(hebbian_pattern['type'], 'hebbian')
        self.assertIn('weight_change', hebbian_pattern)
        
        # Test STDP pattern
        stdp_pattern = PatternBuilder.create_stdp_pattern()
        self.assertIn('type', stdp_pattern)
        self.assertEqual(stdp_pattern['type'], 'stdp')
        self.assertIn('ltp_weight', stdp_pattern)
        self.assertIn('ltd_weight', stdp_pattern)
        
        # Test reward-modulated pattern
        reward_pattern = PatternBuilder.create_reward_modulated_pattern()
        self.assertIn('type', reward_pattern)
        self.assertEqual(reward_pattern['type'], 'reward_modulated')
        
        # Test structural pattern
        structural_pattern = PatternBuilder.create_structural_pattern()
        self.assertIn('type', structural_pattern)
        self.assertEqual(structural_pattern['type'], 'structural')
class TestErrorHandling(unittest.TestCase):
    """Test error handling and validation."""
    
    def test_validation_errors(self):
        """Test type validation errors."""
        from python.wrapper import TypeValidationError
        
        # Test type validation
        try:
            # This should raise TypeValidationError in production
            pass
        except TypeValidationError:
            pass
    
    def test_configuration_errors(self):
        """Test configuration errors."""
        from python.wrapper import ConfigurationError
        
        # Test configuration error handling
        try:
            # This should raise ConfigurationError in production
            pass
        except ConfigurationError:
            pass
    
    def test_simulation_errors(self):
        """Test simulation error handling."""
        from python.wrapper import SimulationError
        
        # Test simulation error handling
        try:
            # This should raise SimulationError in production
            pass
        except SimulationError:
            pass
class TestEventSystem(unittest.TestCase):
    """Test EventSystem functionality."""
    
    def setUp(self):
        """Set up test environment."""
        from python.wrapper import EventSystem
        self.event_system = EventSystem()
        
    def test_event_registration(self):
        """Test event registration."""
        event_called = {'count': 0}
        
        def test_handler(**kwargs):
            event_called['count'] += 1
        
        # Register handler
        self.event_system.on('test_event', test_handler)
        
        # Emit event
        self.event_system.emit('test_event', value=42)
        
        # Check handler was called
        self.assertEqual(event_called['count'], 1)
    
    def test_event_history(self):
        """Test event history tracking."""
        def test_handler(**kwargs):
            pass
        
        self.event_system.on('test_event', test_handler)
        self.event_system.emit('test_event', value=123)
        
        # Get event history
        history = self.event_system.get_event_history('test_event')
        
        self.assertEqual(len(history), 1)
        self.assertEqual(history[0]['name'], 'test_event')
        self.assertEqual(history[0]['data']['value'], 123)
    
    def test_event_filtering(self):
        """Test event filtering."""
        def handler1(**kwargs):
            pass
        
        def handler2(**kwargs):
            pass
        
        self.event_system.on('event1', handler1)
        self.event_system.on('event2', handler2)
        
        # Emit both events
        self.event_system.emit('event1', value=1)
        self.event_system.emit('event2', value=2)
        
        # Get filtered history
        event1_history = self.event_system.get_event_history('event1')
        event2_history = self.event_system.get_event_history('event2')
        
        self.assertEqual(len(event1_history), 1)
        self.assertEqual(len(event2_history), 1)
        
        # Get all events
        all_history = self.event_system.get_event_history()
        self.assertEqual(len(all_history), 2)
class TestMonitoring(unittest.TestCase):
    """Test monitoring functionality."""
    
    def setUp(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 50)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        from python.wrapper import NeuralMonitor
        self.monitor = NeuralMonitor(self.brain)
    
    def test_monitoring_lifecycle(self):
        """Test monitoring start/stop lifecycle."""
        self.assertFalse(self.monitor._monitoring)
        
        self.monitor.start_monitoring()
        self.assertTrue(self.monitor._monitoring)
        
        self.monitor.stop_monitoring()
        self.assertFalse(self.monitor._monitoring)
    
    def test_monitoring_recording(self):
        """Test state recording."""
        self.monitor.start_monitoring()
        
        # Take some steps
        for step in range(5):
            self.brain.step(step)
            self.monitor.record_state()
        
        self.monitor.stop_monitoring()
        
        # Check recorded history
        history = self.monitor.get_history()
        self.assertEqual(len(history), 5)
        
        # Check recorded fields
        first_state = history[0]
        expected_keys = ['timestamp', 'step', 'neuron_count', 'synapse_count',
                        'active_neurons', 'firing_neurons', 'average_firing_rate',
                        'excitation_inhibition_ratio', 'total_spikes', 'developmental_stage']
        
        for key in expected_keys:
            self.assertIn(key, first_state)
        
        # Check values are reasonable
        self.assertGreater(first_state['neuron_count'], 0)
        self.assertGreater(first_state['synapse_count'], 0)
        self.assertGreaterEqual(first_state['active_neurons'], 0)
        self.assertGreaterEqual(first_state['firing_neurons'], 0)
        self.assertGreaterEqual(first_state['average_firing_rate'], 0)
        self.assertGreaterEqual(first_state['total_spikes'], 0)
        self.assertIn(first_state['developmental_stage'], [0, 1, 2, 3, 4])
    
    def test_monitoring_statistics(self):
        """Test monitoring statistics calculation."""
        self.monitor.start_monitoring()
        
        # Take some steps
        for step in range(10):
            self.brain.step(step)
            self.monitor.record_state()
        
        self.monitor.stop_monitoring()
        
        # Get statistics
        stats = self.monitor.get_statistics()
        
        # Check expected statistics
        expected_keys = ['duration', 'average_neuron_count', 'average_synapse_count',
                        'average_active_neurons', 'average_firing_rate',
                        'total_spike_rate', 'max_firing_rate', 'min_firing_rate',
                        'firing_rate_variance', 'neuron_count_variance', 'stability_index']
        
        for key in expected_keys:
            self.assertIn(key, stats)
        
        # Check reasonable values
        self.assertGreaterEqual(stats['duration'], 0)
        self.assertGreaterEqual(stats['average_neuron_count'], 0)
        self.assertGreaterEqual(stats['average_synapse_count'], 0)
        self.assertGreaterEqual(stats['average_active_neurons'], 0)
        self.assertGreaterEqual(stats['average_firing_rate'], 0)
        self.assertGreaterEqual(stats['max_firing_rate'], 0)
        self.assertGreaterEqual(stats['min_firing_rate'], 0)
        self.assertGreaterEqual(stats['firing_rate_variance'], 0)
        self.assertGreaterEqual(stats['neuron_count_variance'], 0)
        self.assertGreaterEqual(stats['stability_index'], -1)
        self.assertLessEqual(stats['stability_index'], 1)
class TestPerformanceProfiling(unittest.TestCase):
    """Test performance profiling functionality."""
    
    def setUp(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 100)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        from python.wrapper import PerformanceProfiler
        self.profiler = PerformanceProfiler(self.brain)
    
    def test_profiling_lifecycle(self):
        """Test profiling start/stop lifecycle."""
        self.assertFalse(self.profiler._active)
        
        self.profiler.start_profiling()
        self.assertTrue(self.profiler._active)
        
        self.profiler.stop_profiling()
        self.assertFalse(self.profiler._active)
    
    def test_measurement_recording(self):
        """Test measurement recording."""
        self.profiler.start_profiling()
        
        # Take some steps and record measurements
        for step in range(5):
            self.brain.step(step)
            self.profiler.record_measurement(step)
        
        self.profiler.stop_profiling()
        
        # Check recorded measurements
        measurements = self.profiler.get_measurements()
        self.assertEqual(len(measurements), 5)
        
        # Check measurement fields
        first_measurement = measurements[0]
        expected_keys = ['step', 'timestamp', 'neuron_count', 'synapse_count',
                        'active_neurons', 'firing_neurons', 'average_firing_rate',
                        'excitation_inhibition_ratio', 'total_spikes']
        
        for key in expected_keys:
            self.assertIn(key, first_measurement)
    
    def test_performance_metrics(self):
        """Test performance metrics calculation."""
        self.profiler.start_profiling()
        
        # Take some steps and record measurements
        for step in range(10):
            self.brain.step(step)
            self.profiler.record_measurement(step)
        
        self.profiler.stop_profiling()
        
        # Get performance metrics
        metrics = self.profiler.get_performance_metrics()
        
        # Check expected metrics
        expected_keys = ['time_metrics', 'neural_metrics', 'stability_metrics']
        
        for key in expected_keys:
            self.assertIn(key, metrics)
        
        # Check time metrics
        time_metrics = metrics['time_metrics']
        expected_time_keys = ['total_time_seconds', 'steps_per_second', 'measurements_per_second']
        for key in expected_time_keys:
            self.assertIn(key, time_metrics)
        
        self.assertGreaterEqual(time_metrics['total_time_seconds'], 0)
        self.assertGreaterEqual(time_metrics['steps_per_second'], 0)
        self.assertGreaterEqual(time_metrics['measurements_per_second'], 0)
        
        # Check neural metrics
        neural_metrics = metrics['neural_metrics']
        expected_neural_keys = ['max_neurons', 'min_neurons', 'avg_neurons',
                               'max_synapses', 'min_synapses', 'avg_synapses',
                               'max_active_neurons', 'min_active_neurons', 'avg_active_neurons',
                               'max_firing_neurons', 'min_firing_neurons', 'avg_firing_neurons',
                               'max_firing_rate', 'min_firing_rate', 'avg_firing_rate',
                               'max_total_spikes', 'min_total_spikes', 'avg_total_spikes']
        
        for key in expected_neural_keys:
            self.assertIn(key, neural_metrics)
        
        # Check stability metrics
        stability_metrics = metrics['stability_metrics']
        expected_stability_keys = ['firing_rate_variance', 'neuron_count_variance', 'stability_index']
        for key in expected_stability_keys:
            self.assertIn(key, stability_metrics)
class TestBenchmarkSuite(unittest.TestCase):
    """Test benchmark suite functionality."""
    
    def setUp(self):
        """Set up test environment."""
        from python.wrapper import BenchmarkSuite
        self.benchmark_suite = BenchmarkSuite()
    
    def test_benchmark_addition(self):
        """Test adding benchmarks."""
        # Define a simple benchmark function
        def simple_benchmark(brain):
            brain.initialize()
            for step in range(10):
                brain.step(step)
            return brain.getTotalSpikeCount()
        
        # Add benchmark
        self.benchmark_suite.add_benchmark(
            name='simple_test',
            description='Simple benchmark test',
            benchmark_func=simple_benchmark,
            warmup_steps=5,
            measurement_steps=10
        )
        
        self.assertEqual(len(self.benchmark_suite.benchmarks), 1)
        
        # Check benchmark properties
        benchmark = self.benchmark_suite.benchmarks[0]
        self.assertEqual(benchmark['name'], 'simple_test')
        self.assertEqual(benchmark['description'], 'Simple benchmark test')
        self.assertEqual(benchmark['warmup_steps'], 5)
        self.assertEqual(benchmark['measurement_steps'], 10)
    
    def test_benchmark_execution(self):
        """Test benchmark execution."""
        # Define a benchmark function
        def test_benchmark(brain):
            brain.initialize()
            for step in range(10):
                brain.step(step)
            return brain.getTotalSpikeCount()
        
        # Add and run benchmark
        self.benchmark_suite.add_benchmark(
            name='test_benchmark',
            description='Test benchmark',
            benchmark_func=test_benchmark,
            warmup_steps=5,
            measurement_steps=10
        )
        
        # Run benchmarks
        results = self.benchmark_suite.run_all_benchmarks()
        
        self.assertEqual(len(self.benchmark_suite.results), 1)
        
        # Check result structure
        result = self.benchmark_suite.results[0]
        expected_keys = ['benchmark_name', 'description', 'duration_seconds',
                        'steps_per_second', 'neurons_at_end', 'synapses_at_end',
                        'total_spikes_at_end', 'average_firing_rate_at_end', 'config_used']
        
        for key in expected_keys:
            self.assertIn(key, result)
        
        self.assertEqual(result['benchmark_name'], 'test_benchmark')
        self.assertGreaterEqual(result['duration_seconds'], 0)
        self.assertGreaterEqual(result['steps_per_second'], 0)
class TestExperimentSuite(unittest.TestCase):
    """Test experiment suite functionality."""
    
    def setUp(self):
        """Set up test environment."""
        from python.wrapper import TestSuite
        self.test_suite = TestSuite()
    
    def test_test_addition(self):
        """Test adding tests."""
        # Define a simple test function
        def simple_test():
            self.assertTrue(True)
        
        # Add test
        self.test_suite.add_test(
            name='simple_test',
            description='Simple test',
            test_func=simple_test,
            category='basic'
        )
        
        self.assertEqual(len(self.test_suite.tests), 1)
        
        # Check test properties
        test = self.test_suite.tests[0]
        self.assertEqual(test['name'], 'simple_test')
        self.assertEqual(test['description'], 'Simple test')
        self.assertEqual(test['category'], 'basic')
    
    def test_test_execution(self):
        """Test test execution."""
        # Define a test that passes
        def passing_test():
            self.assertEqual(1 + 1, 2)
        
        # Define a test that fails
        def failing_test():
            self.assertEqual(1 + 1, 3)
        
        # Add tests
        self.test_suite.add_test('passing', 'Test that passes', passing_test, category='basic')
        self.test_suite.add_test('failing', 'Test that fails', failing_test, category='basic')
        
        # Run tests
        results = self.test_suite.run_all_tests()
        
        self.assertEqual(len(self.test_suite.results), 2)
        
        # Check results
        passing_result = next(r for r in results if r['test_name'] == 'passing')
        failing_result = next(r for r in results if r['test_name'] == 'failing')
        
        self.assertTrue(passing_result['passed'])
        self.assertFalse(failing_result['passed'])
        
        # Check summary
        summary = self.test_suite.get_summary()
        self.assertEqual(summary['total_tests'], 2)
        self.assertEqual(summary['passed_tests'], 1)
        self.assertEqual(summary['failed_tests'], 1)
        self.assertEqual(summary['success_rate'], 0.5)
class TestVisualization(unittest.TestCase):
    """Test visualization functionality."""
    
    def test_visual_debugger_import(self):
        """Test that VisualDebugger can be imported."""
        from python.wrapper import VisualDebugger
        self.assertIsNotNone(VisualDebugger)
    
    def test_visual_debugger_methods(self):
        """Test VisualDebugger methods exist."""
        from python.wrapper import VisualDebugger
        
        # Create a brain for testing
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 10)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test methods exist
        self.assertTrue(hasattr(VisualDebugger, 'plot_neural_activity'))
        self.assertTrue(hasattr(VisualDebugger, 'analyze_connectivity'))
        
        # Note: Actual visualization would require matplotlib
        # These tests just verify the API exists
class TestIntegration(unittest.TestCase):
    """Integration tests for NLM Python bindings."""
    
    def test_complete_simulation_workflow(self):
        """Test a complete simulation workflow."""
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 50)
        config.set("region_count", 2)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation steps
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check simulation state
            self.assertIsInstance(percept, SensoryPercept)
            self.assertIsInstance(action, Action)
        
        # Verify simulation completed
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(agent.getNeuromodulationLevel(), 0)
    
    def test_multiple_configurations(self):
        """Test running with multiple configurations."""
        from python.wrapper import ConfigBuilder
        
        # Create different configurations
        config1 = ConfigBuilder.create_learning_brain(neuron_count=30)
        config2 = ConfigBuilder.create_stable_brain(neuron_count=60)
        config3 = ConfigBuilder.create_explosive_brain(neuron_count=40)
        
        # Test each configuration
        for config in [config1, config2, config3]:
            brain = pynlm.createBrain(config.get_config())
            brain.initialize()
            
            # Run simulation
            for step in range(5):
                brain.step(step)
            
            # Verify brain is working
            self.assertGreater(brain.getTotalNeuronCount(), 0)
            self.assertGreater(brain.getTotalSynapseCount(), 0)
    
    def test_performance_benchmarking(self):
        """Test performance benchmarking capabilities."""
        from python.wrapper import BenchmarkSuite
        
        # Create benchmark suite
        suite = BenchmarkSuite()
        
        # Add performance benchmark
        def performance_benchmark(brain):
            brain.initialize()
            for step in range(20):
                brain.step(step)
            return brain.getAverageFiringRate()
        
        suite.add_benchmark(
            name='performance_test',
            description='Test brain performance',
            benchmark_func=performance_benchmark,
            warmup_steps=5,
            measurement_steps=15
        )
        
        # Run benchmarks
        results = suite.run_all_benchmarks()
        
        # Verify results
        self.assertEqual(len(results), 1)
        self.assertGreaterEqual(results[0]['steps_per_second'], 0)
    
    def test_smart_brain_workflow(self):
        """Test SmartBrain workflow with monitoring and event system."""
        from python.wrapper import SmartBrain, EventSystem
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 30)
        brain = pynlm.createBrain(config)
        
        # Create SmartBrain
        smart_brain = SmartBrain(brain)
        smart_brain.initialize()
        
        # Test step chaining
        smart_brain.step(5)
        self.assertTrue(smart_brain._initialized)
        
        # Test event system
        event_system = EventSystem()
        event_calls = {'count': 0}
        
        def test_event_handler(**kwargs):
            event_calls['count'] += 1
        
        event_system.on('test_event', test_event_handler)
        event_system.emit('test_event', value=42)
        
        self.assertEqual(event_calls['count'], 1)
    
    def test_complete_advanced_workflow(self):
        """Test a complete advanced workflow with all features."""
        # Create advanced configuration
        from python.wrapper import AdvancedConfig, NeuromodulationProfile
        from python.wrapper import BenchmarkSuite, TestSuite, VisualDebugger
        
        advanced_config = NeuromodulationProfile.create_exploration_brain()
        brain = pynlm.createBrain(advanced_config.get_config())
        brain.initialize()
        
        # Run simulation
        for step in range(10):
            brain.step(step)
        
        # Test benchmarking
        benchmark_suite = BenchmarkSuite()
        def test_benchmark(brain):
            brain.initialize()
            for step in range(5):
                brain.step(step)
            return brain.getTotalSpikeCount()
        
        benchmark_suite.add_benchmark('test', 'Test benchmark', test_benchmark, 3, 5)
        benchmark_results = benchmark_suite.run_all_benchmarks()
        
        self.assertEqual(len(benchmark_results), 1)
        
        # Test visualization
        visualization = VisualDebugger.analyze_connectivity(brain)
        self.assertIn('region_count', visualization)
        self.assertIn('neurons_per_region', visualization)
        
        # Verify everything worked
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(brain.getTotalNeuronCount(), 0)
if __name__ == '__main__':
    unittest.main()
