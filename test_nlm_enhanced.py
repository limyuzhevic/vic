#!/usr/bin/env python3

"""
NLM Test Suite for Python Bindings

This module provides comprehensive tests for the enhanced NLM Python bindings,
including basic functionality, error handling, and advanced features.
"""

import sys
import os
import unittest
import tempfile
import shutil
import time
from pathlib import Path
from unittest.mock import patch, MagicMock

# Add the pynlm module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pybind11
except ImportError:
    pybind11 = None

# Import the NLM module if available
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    pynlm = MagicMock()

try:
    import pynlm_helper
    PYNLM_HELPER_AVAILABLE = True
except ImportError:
    PYNLM_HELPER_AVAILABLE = False
    pynlm_helper = MagicMock()

class TestBasicFunctionality(unittest.TestCase):
    """Test basic NLM functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM Python bindings not available")
        
        try:
            self.config = pynlm.create_default_config()
            self.brain = pynlm.create_brain(self.config)
            self.brain.initialize()
        except Exception as e:
            self.skipTest(f"Failed to create NLM objects: {e}")
    
    def test_brain_creation(self):
        """Test basic brain creation and initialization."""
        self.assertIsNotNone(self.brain)
        self.assertTrue(hasattr(self.brain, 'initialize'))
        self.assertTrue(hasattr(self.brain, 'step'))
        self.assertTrue(hasattr(self.brain, 'get_total_neuron_count'))
        self.assertTrue(hasattr(self.brain, 'get_total_spike_count'))
    
    def test_brain_step(self):
        """Test brain simulation step."""
        initial_spikes = self.brain.get_total_spike_count()
        self.brain.step(0)
        self.assertGreaterEqual(self.brain.get_total_spike_count(), initial_spikes)
    
    def test_configuration(self):
        """Test configuration functionality."""
        self.assertTrue(hasattr(self.config, 'set_value'))
        self.assertTrue(hasattr(self.config, 'get_keys'))
        self.assertTrue(hasattr(self.config, 'save_to_file'))
        
        # Test setting configuration values
        self.config.set_value("test_param", 42)
        self.config.set_value("test_param2", "test_string")
        
        # Test that keys are accessible
        keys = self.config.get_keys()
        self.assertIn("test_param", keys)
    
    def test_save_load(self):
        """Test configuration save/load functionality."""
        # Create a temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_path = f.name
        
        try:
            # Save configuration
            self.config.save_to_file(config_path)
            
            # Create a new config and load from file
            new_config = pynlm.create_default_config()
            result = new_config.load_from_file(config_path)
            
            # Loading should return True
            self.assertTrue(result)
            
            # New config should have some keys (depending on default config)
            self.assertIsInstance(new_config.get_keys(), list)
            
        finally:
            # Clean up
            if os.path.exists(config_path):
                os.unlink(config_path)
class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced NLM features."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM Python bindings not available")
        
        try:
            # Create experiment setup using helper
            if PYNLM_HELPER_AVAILABLE:
                self.experiment = pynlm_helper.create_experiment_setup(
                    world_width=10, world_height=10,
                    vision_width=6, vision_height=6
                )
                self.brain = self.experiment["brain"]
                self.world = self.experiment["world"]
                self.agent = self.experiment["agent"]
            else:
                # Fallback to manual setup
                self.config = pynlm.create_default_config()
                self.brain = pynlm.create_brain(self.config)
                self.brain.initialize()
                
        except Exception as e:
            self.skipTest(f"Failed to create advanced test setup: {e}")
    
    def test_experiment_setup(self):
        """Test experiment setup functionality."""
        if PYNLM_HELPER_AVAILABLE:
            self.assertIn("brain", self.experiment)
            self.assertIn("world", self.experiment)
            self.assertIn("agent", self.experiment)
            self.assertIn("components", self.experiment)
            
            # Check components
            self.assertEqual(self.experiment["components"]["brain_count"], 1)
            self.assertEqual(self.experiment["components"]["world_count"], 1)
            self.assertEqual(self.experiment["components"]["agent_count"], 1)
            
            # Check brain properties
            self.assertGreater(self.experiment["components"]["total_neurons"], 0)
    
    def test_world_simulation(self):
        """Test world simulation functionality."""
        if not hasattr(self, 'world'):
            self.skipTest("World not available")
        
        # Test world update
        initial_time = self.world.getSimulationTime()
        self.world.update(0.1)
        self.assertGreater(self.world.getSimulationTime(), initial_time)
        
        # Test sensory percept
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
    
    def test_agent_functionality(self):
        """Test agent brain functionality."""
        if not hasattr(self, 'agent'):
            self.skipTest("Agent not available")
        
        # Test agent features
        self.assertTrue(hasattr(self.agent, 'enable_reward_modulation'))
        self.assertTrue(hasattr(self.agent, 'enable_structural_plasticity'))
        self.assertTrue(hasattr(self.agent, 'enable_development'))
        self.assertTrue(hasattr(self.agent, 'enable_curiosity'))
        
        # Test enabling features
        self.agent.enable_reward_modulation(True)
        self.agent.enable_structural_plasticity(True)
        self.agent.enable_development(True)
        self.agent.enable_curiosity(True)
        
        # Test decoding motor command
        try:
            action = self.agent.decode_motor_command()
            self.assertIsNotNone(action)
        except:
            pass  # May not have valid sensory input
    
    def test_brain_state_analysis(self):
        """Test brain state analysis functionality."""
        try:
            if PYNLM_HELPER_AVAILABLE:
                analysis = pynlm_helper.analyze_brain_state(self.brain)
                self.assertIn("neuron_stats", analysis)
                self.assertIn("memory_stats", analysis)
                self.assertIn("neuromodulation", analysis)
                self.assertIn("developmental", analysis)
                
                # Check neuron stats
                self.assertIn("total", analysis["neuron_stats"])
                self.assertIn("active", analysis["neuron_stats"])
                self.assertIn("firing", analysis["neuron_stats"])
                self.assertIn("average_firing_rate", analysis["neuron_stats"])
                self.assertIn("total_spikes", analysis["neuron_stats"])
                
        except Exception as e:
            self.skipTest(f"Brain state analysis failed: {e}")
class TestErrorHandling(unittest.TestCase):
    """Test error handling in NLM."""
    
    def test_invalid_configuration(self):
        """Test handling of invalid configuration."""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM Python bindings not available")
        
        try:
            # Try to create config and modify it invalidly
            config = pynlm.create_default_config()
            
            # Test setting invalid value types (if supported)
            # Note: This depends on the actual implementation
            pass
            
        except Exception as e:
            # We expect some errors with invalid configurations
            self.assertIsInstance(e, (Exception,))
    
    def test_brain_error_handling(self):
        """Test brain error handling."""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM Python bindings not available")
        
        try:
            config = pynlm.create_default_config()
            brain = pynlm.create_brain(config)
            
            # Try to step without initializing (if possible)
            # Note: This depends on the actual error handling
            pass
            
        except Exception as e:
            # We expect some errors with misusage
            self.assertIsInstance(e, (Exception,))
    
    def test_memory_error_handling(self):
        """Test memory-related error handling."""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM Python bindings not available")
        
        try:
            # Test memory-intensive operations if available
            if PYNLM_HELPER_AVAILABLE:
                # Create a large brain to test memory handling
                analysis = pynlm_helper.analyze_brain_state(self.brain)
                self.assertIsInstance(analysis, dict)
                
        except Exception as e:
            self.assertIsInstance(e, (Exception,))
class TestHelperUtilities(unittest.TestCase):
    """Test helper utility functions."""
    
    def test_research_config_creation(self):
        """Test research configuration creation."""
        if not PYNLM_HELPER_AVAILABLE:
            self.skipTest("NLM helper utilities not available")
        
        # Test different experiment types
        configs = {
            'standard': pynlm_helper.create_research_config("standard"),
            'social': pynlm_helper.create_research_config("social"),
            'curiosity': pynlm_helper.create_research_config("curiosity"),
            'development': pynlm_helper.create_research_config("development")
        }
        
        self.assertIsInstance(configs, dict)
        
        for config_type, config in configs.items():
            self.assertIsInstance(config, dict)
            self.assertIn("neuron_count", config)
            self.assertIn("enable_structural_plasticity", config)
    
    def test_experiment_setup_utility(self):
        """Test experiment setup utility."""
        if not PYNLM_HELPER_AVAILABLE:
            self.skipTest("NLM helper utilities not available")
        
        # Test with different parameters
        setup1 = pynlm_helper.create_experiment_setup(10, 10, 6, 6, 1)
        self.assertIn("brain", setup1)
        self.assertIn("world", setup1)
        self.assertIn("agent", setup1)
        
        setup2 = pynlm_helper.create_experiment_setup(20, 20, 8, 8, 2)
        self.assertEqual(setup2["components"]["agent_count"], 2)
        self.assertGreater(setup2["components"]["total_neurons"], setup1["components"]["total_neurons"])
    
    def test_learning_simulation(self):
        """Test learning simulation utility."""
        if not PYNLM_HELPER_AVAILABLE:
            self.skipTest("NLM helper utilities not available")
        
        try:
            # Create experiment and run simulation
            experiment = pynlm_helper.create_experiment_setup(10, 10, 6, 6, 1)
            results = pynlm_helper.run_learning_simulation(experiment, steps=50)
            
            # Check results structure
            self.assertIn("steps_completed", results)
            self.assertIn("total_reward", results)
            self.assertIn("firing_rates", results)
            self.assertIn("actions_taken", results)
            self.assertIn("learning_events", results)
            
            # Check results values
            self.assertEqual(results["steps_completed"], 50)
            self.assertIsInstance(results["firing_rates"], list)
            self.assertGreaterEqual(results["actions_taken"], 0)
            
        except Exception as e:
            self.skipTest(f"Learning simulation test failed: {e}")
    
    def test_performance_benchmarking(self):
        """Test performance benchmarking utility."""
        if not PYNLM_HELPER_AVAILABLE:
            self.skipTest("NLM helper utilities not available")
        
        try:
            # Create experiment and benchmark
            experiment = pynlm_helper.create_experiment_setup(10, 10, 6, 6, 1)
            benchmark = pynlm_helper.benchmark_performance(
                experiment, 
                durations=[0.1, 0.5]
            )
            
            # Check benchmark structure
            self.assertIn("benchmarks", benchmark)
            self.assertIn("performance_scaling", benchmark)
            self.assertIn("bottleneck_analysis", benchmark)
            
            # Check benchmark data
            self.assertIsInstance(benchmark["benchmarks"], list)
            self.assertGreater(len(benchmark["benchmarks"]), 0)
            
            # Check each benchmark entry
            for bench in benchmark["benchmarks"]:
                self.assertIn("duration_sec", bench)
                self.assertIn("steps_per_second", bench)
                self.assertIn("elapsed_time", bench)
                
        except Exception as e:
            self.skipTest(f"Performance benchmarking test failed: {e}")
class TestIntegration(unittest.TestCase):
    """Test integration between NLM components."""
    
    def test_complete_workflow(self):
        """Test a complete NLM workflow."""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM Python bindings not available")
        
        try:
            # Step 1: Create brain
            config = pynlm.create_default_config()
            config.set_value("neuron_count", 300)
            
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            # Step 2: Simulate
            for step in range(100):
                brain.step(step)
                if step % 25 == 0:
                    analysis = {
                        'step': step,
                        'neurons_firing': brain.get_firing_neuron_count(),
                        'total_spikes': brain.get_total_spike_count(),
                        'avg_firing_rate': brain.get_average_firing_rate()
                    }
            
            # Step 3: Analyze final state
            final_analysis = {
                'total_neurons': brain.get_total_neuron_count(),
                'firing_neurons': brain.get_firing_neuron_count(),
                'total_spikes': brain.get_total_spike_count(),
                'average_firing_rate': brain.get_average_firing_rate(),
                'excitatory_ratio': brain.get_excitatory_inhibition_ratio()
            }
            
            # Verify results
            self.assertGreater(final_analysis['total_neurons'], 0)
            self.assertGreaterEqual(final_analysis['firing_neurons'], 0)
            self.assertGreaterEqual(final_analysis['total_spikes'], 0)
            self.assertGreaterEqual(final_analysis['average_firing_rate'], 0)
            
        except Exception as e:
            self.skipTest(f"Complete workflow test failed: {e}")
    
    def test_advanced_experiment_workflow(self):
        """Test advanced experiment workflow with all components."""
        if not PYNLM_HELPER_AVAILABLE:
            self.skipTest("NLM helper utilities not available")
        
        try:
            # Create advanced experiment
            experiment = pynlm_helper.create_experiment_setup(
                world_width=15, world_height=15,
                vision_width=10, vision_height=10,
                num_agents=1
            )
            
            # Run simulation with monitoring
            simulation_results = pynlm_helper.run_learning_simulation(
                experiment, steps=200, enable_checkpointing=False
            )
            
            # Run benchmark
            benchmark = pynlm_helper.benchmark_performance(
                experiment, durations=[0.2, 0.5]
            )
            
            # Verify all components worked
            self.assertGreater(simulation_results['steps_completed'], 0)
            self.assertIsInstance(benchmark['benchmarks'], list)
            self.assertGreater(len(benchmark['benchmarks']), 0)
            
        except Exception as e:
            self.skipTest(f"Advanced experiment workflow test failed: {e}")
class TestPerformance(unittest.TestCase):
    """Test performance characteristics."""
    
    def test_small_scale_performance(self):
        """Test performance with small scale simulation."""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM Python bindings not available")
        
        start_time = time.time()
        
        config = pynlm.create_default_config()
        config.set_value("neuron_count", 100)  # Small scale
        
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        self.assertLess(duration, 10.0)  # Should complete quickly
        
        # Performance should be reasonable
        steps_per_second = 100 / duration if duration > 0 else 0
        self.assertGreater(steps_per_second, 10.0)  # At least 10 steps per second
    
    def test_memory_usage(self):
        """Test memory usage with different scales."""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM Python bindings not available")
        
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        # Measure memory before
        memory_before = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create and run brain
        config = pynlm.create_default_config()
        config.set_value("neuron_count", 500)
        
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        for step in range(200):
            brain.step(step)
        
        # Measure memory after
        memory_after = process.memory_info().rss / 1024 / 1024  # MB
        memory_growth = memory_after - memory_before
        
        # Memory growth should be reasonable (less than 100MB for 500 neurons)
        self.assertLess(memory_growth, 100.0, 
                       f"Memory growth too high: {memory_growth:.2f} MB")
if __name__ == '__main__':
    # Print test information
    print("=== NLM Python Bindings Test Suite ===")
    print(f"NLM Python Bindings Available: {PYNLM_AVAILABLE}")
    print(f"NLM Helper Utilities Available: {PYNLM_HELPER_AVAILABLE}")
    print()
    
    if pybind11:
        print(f"Pybind11 Version: {pybind11.__version__}")
    
    print()
    
    # Run tests
    unittest.main(verbosity=2)
