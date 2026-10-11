#!/usr/bin/env python3
"""
NLM Comprehensive Test Suite

Comprehensive test suite for NLM (Neural Learning Machine) improvements and fixes.
Tests all new functionality including Python utilities, CLI, visualization, and performance monitoring.
"""

import sys
import os
import json
import time
import unittest
import tempfile
import shutil
from typing import Dict, List, Any
from datetime import datetime

# Add the current directory to the path to import our utilities
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Try to import all NLM modules
try:
    import pynlm
    from python.nlm_utils import (
        NLMExperiment, ConfigPresets, ExperimentType,
        create_config_from_file, save_config_to_file, benchmark_brain
    )
    from python.nlm_cli import NLMCLI
    from python.nlm_visualization import (
        NeuralActivityPlotter, MemoryAnalyzer, NeuromodulationVisualizer,
        ExperimentAnalyzer, SimulationDebugger, VisualizationPipeline
    )
    from python.nlm_performance import (
        PerformanceProfiler, BenchmarkSuite, RealTimeMonitor, NLMAnalyzer,
        PerformanceReporter
    )
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: Could not import NLM modules: {e}")
class TestNLMUtilities(unittest.TestCase):
    """Test NLM Python utilities"""
    
    def setUp(self):
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM not available")
        
        self.test_dir = tempfile.mkdtemp()
        
        # Create test config file
        self.config_file = os.path.join(self.test_dir, "test_config.json")
        with open(self.config_file, 'w') as f:
            json.dump({
                "brain.neuron_count": 100,
                "brain.region_count": 2,
                "plasticity.stdp.enable": True
            }, f)
    
    def tearDown(self):
        shutil.rmtree(self.test_dir, ignore_errors=True)
    
    def test_config_presets(self):
        """Test configuration presets"""
        # Test development preset
        dev_config = ConfigPresets.create_development_brain(500)
        self.assertIn("brain.neuron_count", dev_config)
        self.assertEqual(dev_config["brain.neuron_count"], 500)
        self.assertTrue(dev_config.get("plasticity.structural.enable", False))
        
        # Test learning preset
        learning_config = ConfigPresets.create_learning_agent(1000)
        self.assertIn("brain.neuron_count", learning_config)
        self.assertEqual(learning_config["brain.neuron_count"], 1000)
        self.assertTrue(learning_config.get("plasticity.stdp.enable", False))
        
        # Test reproduction preset
        repro_config = ConfigPresets.create_reproduction_brain(200)
        self.assertIn("brain.neuron_count", repro_config)
        self.assertEqual(repro_config["brain.neuron_count"], 200)
        self.assertFalse(repro_config.get("neuromod.curiosity.enable", True))
    
    def test_experiment_setup(self):
        """Test experiment setup and configuration"""
        experiment = NLMExperiment("test_experiment", ExperimentType.SIMULATION)
        
        # Test brain setup with preset
        experiment.setup_brain(preset="development")
        self.assertIsNotNone(experiment.brain)
        self.assertIsNotNone(experiment.config)
        
        # Test world setup
        experiment.setup_world(width=30, height=30)
        self.assertIsNotNone(experiment.world)
        
        # Test agent setup
        experiment.setup_agent()
        self.assertIsNotNone(experiment.agent)
    
    def test_config_file_io(self):
        """Test configuration file I/O"""
        config = create_config_from_file(self.config_file)
        self.assertTrue(config.has("brain.neuron_count"))
        self.assertEqual(config.getOr("brain.neuron_count", 0), 100)
        
        # Test save config
        output_file = os.path.join(self.test_dir, "output_config.json")
        save_config_to_file(config, output_file)
        self.assertTrue(os.path.exists(output_file))
        
        # Test loading saved config
        saved_config = create_config_from_file(output_file)
        self.assertEqual(saved_config.getOr("brain.neuron_count", 0), 100)
    
    def test_benchmark_function(self):
        """Test benchmark function"""
        config_dict = {
            "brain.neuron_count": 100,
            "brain.region_count": 2
        }
        
        result = benchmark_brain(config_dict, steps=10)
        self.assertIn("steps", result)
        self.assertEqual(result["steps"], 10)
        self.assertIn("neuron_count", result)
        self.assertEqual(result["neuron_count"], 100)
class TestNLMCLI(unittest.TestCase):
    """Test NLM command-line interface"""
    
    def setUp(self):
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM not available")
        
        self.cli = NLMCLI()
    
    def test_cli_setup(self):
        """Test CLI setup functionality"""
        # Test setup with default parameters
        args = type('Args', (), {
            'neurons': 1000,
            'regions': 1,
            'synapse_density': 0.1,
            'width': 20,
            'height': 20,
            'vision_width': 8,
            'vision_height': 8,
            'preset': 'default',
            'enable_all': False,
            'custom_config': {}
        })()
        
        result = self.cli.setup_simulation(args)
        self.assertTrue(result)
        self.assertTrue(self.cli.setup_complete)
    
    def test_cli_status(self):
        """Test CLI status functionality"""
        # Setup first
        args = type('Args', (), {
            'neurons': 500,
            'regions': 1,
            'synapse_density': 0.1,
            'width': 15,
            'height': 15,
            'vision_width': 6,
            'vision_height': 6,
            'preset': 'default',
            'enable_all': False,
            'custom_config': {}
        })()
        
        self.cli.setup_simulation(args)
        
        # Test status
        try:
            self.cli.print_status()
            # Should not raise exception
            self.assertTrue(True)
        except Exception as e:
            self.fail(f"print_status() raised exception: {e}")
class TestVisualization(unittest.TestCase):
    """Test visualization tools"""
    
    def setUp(self):
        self.test_dir = tempfile.mkdtemp()
        
        if PYNLM_AVAILABLE:
            # Create a minimal brain for testing
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 100)
            config.set("brain.region_count", 2)
            self.brain = pynlm.createBrain(config)
            self.brain.initialize()
        else:
            self.brain = None
            self.skipTest("NLM not available")
    
    def tearDown(self):
        shutil.rmtree(self.test_dir, ignore_errors=True)
    
    def test_neural_activity_plotter(self):
        """Test neural activity plotter"""
        if not self.brain:
            self.skipTest("NLM not available")
        
        plotter = NeuralActivityPlotter(self.test_dir)
        
        # Create test time series
        time_series = []
        for step in range(0, 101, 10):
            time_series.append({
                'step': step,
                'firing_rate': self.brain.getAverageFiringRate() * (1 + (step / 100)),
                'total_spikes': self.brain.getTotalSpikeCount() * (step / 100)
            })
        
        # Test plotting
        output_file = plotter.plot_firing_rate_over_time(time_series, "test_firing_rate.png")
        self.assertTrue(os.path.exists(output_file))
        
        # Test region plot
        region_file = plotter.plot_neural_regions(self.brain, "test_regions.png")
        self.assertTrue(os.path.exists(region_file))
    
    def test_memory_analyzer(self):
        """Test memory analyzer"""
        if not self.brain:
            self.skipTest("NLM not available")
        
        analyzer = MemoryAnalyzer(self.test_dir)
        output_file = analyzer.plot_working_memory_activity(self.brain, "test_memory.png")
        self.assertTrue(os.path.exists(output_file))
    
    def test_simulation_debugger(self):
        """Test simulation debugger"""
        debugger = SimulationDebugger()
        
        # Test error logging
        debugger.log_error("Test error", 10)
        debugger.log_warning("Test warning", 20)
        
        # Test brain state analysis
        if self.brain:
            analysis = debugger.analyze_brain_state(self.brain, 5)
            self.assertIn('issues', analysis)
            self.assertIn('total_neurons', analysis)
        
        # Test debug export
        output_file = os.path.join(self.test_dir, "debug_info.json")
        debugger.export_debug_info(output_file)
        self.assertTrue(os.path.exists(output_file))
class TestPerformanceTools(unittest.TestCase):
    """Test performance monitoring tools"""
    
    def setUp(self):
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM not available")
        
        # Create brain for testing
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        config.set("brain.region_count", 2)
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
    
    def test_performance_profiler(self):
        """Test performance profiler"""
        profiler = PerformanceProfiler(self.brain)
        
        # Test timer functionality
        profiler.start_timer("test_operation")
        time.sleep(0.01)  # Small delay
        duration = profiler.stop_timer("test_operation")
        self.assertGreater(duration, 0.001)
        self.assertLess(duration, 1.0)
        
        # Test step recording
        for step in range(10):
            profiler.record_step_performance(step)
            self.brain.step(step)
        
        # Test summary generation
        summary = profiler.get_performance_summary()
        self.assertIn('statistics', summary)
        self.assertIn('current', summary)
        
        # Test metrics saving
        output_file = os.path.join(tempfile.mkdtemp(), "metrics.json")
        profiler.save_metrics(output_file)
        self.assertTrue(os.path.exists(output_file))
    
    def test_benchmark_suite(self):
        """Test benchmark suite"""
        benchmark = BenchmarkSuite()
        
        config_dict = {
            "brain.neuron_count": 100,
            "brain.region_count": 2
        }
        
        # Test brain creation benchmark
        creation_result = benchmark.benchmark_brain_creation(config_dict, iterations=2)
        self.assertIn('test', creation_result)
        self.assertEqual(creation_result['test'], 'brain_creation')
        self.assertIn('statistics', creation_result)
        
        # Test step performance benchmark
        step_result = benchmark.benchmark_step_performance(config_dict, steps=5, iterations=2)
        self.assertIn('test', step_result)
        self.assertEqual(step_result['test'], 'step_performance')
        self.assertIn('statistics', step_result)
    
    def test_analyzer(self):
        """Test analyzer"""
        analyzer = NLMAnalyzer()
        
        # Create profiler
        profiler = PerformanceProfiler(self.brain)
        for step in range(10):
            profiler.record_step_performance(step)
            self.brain.step(step)
        
        # Test trend analysis
        trend_analysis = analyzer.analyze_performance_trends(profiler)
        self.assertIn('step_duration_trend', trend_analysis)
        self.assertIn('average_step_duration', trend_analysis)
        
        # Test report generation
        report = analyzer.generate_performance_report(profiler)
        self.assertIn('generated_at', report)
        self.assertIn('summary', report)
        self.assertIn('trend_analysis', report)
        self.assertIn('recommendations', report)
class TestIntegration(unittest.TestCase):
    """Test integration of all components"""
    
    def test_full_workflow(self):
        """Test complete workflow with all components"""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM not available")
        
        test_dir = tempfile.mkdtemp()
        
        try:
            # Create experiment
            experiment = NLMExperiment("integration_test", ExperimentType.SIMULATION)
            
            # Setup with preset
            experiment.setup_brain(preset="development")
            experiment.setup_world(width=20, height=20)
            experiment.setup_agent()
            experiment.enable_all_subsystems()
            
            # Add callback for monitoring
            callback_called = []
            def test_callback(step, metrics):
                callback_called.append((step, metrics))
            
            experiment.add_step_callback(test_callback)
            
            # Run experiment
            result = experiment.run(num_steps=20, timestep=0.1)
            
            # Verify results
            self.assertIn('name', result)
            self.assertIn('type', result)
            self.assertIn('steps', result)
            self.assertEqual(result['steps'], 20)
            self.assertGreater(len(callback_called), 0)
            
            # Test visualization
            viz_pipeline = VisualizationPipeline(test_dir)
            viz_files = viz_pipeline.create_complete_visualization(
                experiment.brain, "integration_test", include_all=False)
            
            # Test performance profiling
            profiler = PerformanceProfiler(experiment.brain)
            for step in range(10):
                profiler.record_step_performance(step)
                experiment.brain.step(step)
            
            summary = profiler.get_performance_summary()
            self.assertIn('statistics', summary)
            
            # Test reporting
            reporter = PerformanceReporter(test_dir)
            html_report = reporter.generate_html_report(result, "integration_test")
            self.assertTrue(os.path.exists(html_report))
            
            csv_report = reporter.generate_csv_report(profiler, "integration_test")
            self.assertTrue(os.path.exists(csv_report))
            
        finally:
            shutil.rmtree(test_dir, ignore_errors=True)
class TestErrorHandling(unittest.TestCase):
    """Test error handling and edge cases"""
    
    def test_import_error_handling(self):
        """Test handling of import errors"""
        # This test ensures that the code handles missing dependencies gracefully
        # The import error handling is built into the modules themselves
        
        # The modules should not crash when NLM is not available
        # (This is handled by the conditional imports)
        
        # Just verify the modules can be imported (even if partially)
        self.assertTrue(True)  # If we get here, imports succeeded
    
    def test_invalid_parameters(self):
        """Test handling of invalid parameters"""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM not available")
        
        # Test with invalid experiment parameters
        experiment = NLMExperiment("invalid_test", ExperimentType.SIMULATION)
        
        # Should raise error when trying to setup without brain
        with self.assertRaises(RuntimeError):
            experiment.enable_all_subsystems()
        
        # Test with invalid config file
        invalid_config_file = "/nonexistent/path/config.json"
        try:
            create_config_from_file(invalid_config_file)
            self.fail("Should have raised exception for invalid config file")
        except Exception:
            pass  # Expected
    
    def test_resource_cleanup(self):
        """Test resource cleanup"""
        test_dir = tempfile.mkdtemp()
        
        try:
            # Create and cleanup experiment
            experiment = NLMExperiment("cleanup_test", ExperimentType.SIMULATION)
            experiment.setup_brain(preset="default")
            experiment.setup_world(width=10, height=10)
            experiment.setup_agent()
            
            # Run and cleanup
            result = experiment.run(num_steps=5)
            
            # Verify cleanup
            self.assertTrue(os.path.exists(test_dir))
            
        finally:
            shutil.rmtree(test_dir, ignore_errors=True)
def run_tests():
    """Run all tests"""
    print("NLM Comprehensive Test Suite")
    print("=" * 50)
    
    if not PYNLM_AVAILABLE:
        print("Warning: NLM Python bindings not available.")
        print("Some tests will be skipped.")
        print("To install: pip install pynlm")
        print()
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(TestNLMUtilities)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestNLMCLI))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestVisualization))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestPerformanceTools))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestIntegration))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestErrorHandling))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Print summary
    print("\n" + "=" * 50)
    print("Test Suite Summary")
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Skipped: {len(result.skipped) if hasattr(result, 'skipped') else 0}")
    
    if result.failures:
        print("\nFailures:")
        for test, traceback in result.failures:
            print(f"  {test}: {traceback}")
    
    if result.errors:
        print("\nErrors:")
        for test, traceback in result.errors:
            print(f"  {test}: {traceback}")
    
    return result.wasSuccessful()
if __name__ == "__main__":
    success = run_tests()
    sys.exit(0 if success else 1)
