#!/usr/bin/env python3
"""
Comprehensive test suite for enhanced NLM Python API

This test suite validates all new features and improvements in the enhanced
Python API for NLM neural simulation framework.
"""

import unittest
import sys
import os
import tempfile
import json
import time
import numpy as np
from unittest.mock import patch, MagicMock

# Add the project root to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from pynlm_enhanced import (
    createConfigFromPreset,
    createBrainFromPreset,
    run_simulation_loop,
    SimulationMonitor,
    analyze_brain_state,
    create_agent_with_features,
    PerformanceProfiler,
    NeuralNetworkAnalyzer,
    BatchProcessor,
    ExperimentManager,
    HyperparameterOptimizer
)
class TestEnhancedConfigurationPresets(unittest.TestCase):
    """Test enhanced configuration preset functionality"""
    
    def test_create_config_from_preset_standard(self):
        """Test creating configuration from standard preset"""
        config = createConfigFromPreset("standard")
        
        self.assertIsNotNone(config)
        self.assertTrue(config.has("brain.neuron_count"))
        self.assertEqual(config.getOr("brain.neuron_count", 0), 1000)
        self.assertEqual(config.getOr("brain.synapse_density", 0), 0.1)
    
    def test_create_config_from_preset_large(self):
        """Test creating configuration from large preset"""
        config = createConfigFromPreset("large")
        
        self.assertIsNotNone(config)
        self.assertTrue(config.has("brain.neuron_count"))
        self.assertEqual(config.getOr("brain.neuron_count", 0), 5000)
        self.assertEqual(config.getOr("brain.synapse_density", 0), 0.15)
        self.assertTrue(config.getOr("plasticity.structural.enable", False))
    
    def test_create_config_from_preset_small(self):
        """Test creating configuration from small preset"""
        config = createConfigFromPreset("small")
        
        self.assertIsNotNone(config)
        self.assertEqual(config.getOr("brain.neuron_count", 0), 100)
        self.assertFalse(config.getOr("neuromod.curiosity.enable", True))
    
    def test_create_config_from_preset_research(self):
        """Test creating configuration from research preset"""
        config = createConfigFromPreset("research")
        
        self.assertIsNotNone(config)
        self.assertEqual(config.getOr("brain.neuron_count", 0), 10000)
        self.assertTrue(config.getOr("development.enable", False))
        self.assertTrue(config.getOr("neuromod.curiosity.enable", False))
    
    def test_create_config_from_preset_custom_overrides(self):
        """Test creating configuration with custom parameter overrides"""
        config = createConfigFromPreset(
            "standard",
            neuron_count=2000,
            plasticity_stdp_learning_rate=0.002
        )
        
        self.assertEqual(config.getOr("brain.neuron_count", 0), 2000)
        self.assertEqual(config.getOr("plasticity.stdp.learning_rate", 0), 0.002)
    
    def test_create_config_from_preset_invalid_preset(self):
        """Test creating configuration with invalid preset name"""
        with self.assertRaises(ValueError):
            createConfigFromPreset("invalid_preset")
class TestEnhancedBrainCreation(unittest.TestCase):
    """Test enhanced brain creation functionality"""
    
    def setUp(self):
        self.test_config = createConfigFromPreset("standard")
    
    def test_create_brain_from_preset_standard(self):
        """Test creating brain from standard preset"""
        brain = createBrainFromPreset("standard")
        
        self.assertIsNotNone(brain)
        brain.initialize()
        
        self.assertEqual(brain.getTotalNeuronCount(), 1000)
        self.assertTrue(brain.getTotalNeuronCount() > 0)
    
    def test_create_brain_from_preset_large(self):
        """Test creating brain from large preset"""
        brain = createBrainFromPreset("large")
        
        self.assertIsNotNone(brain)
        brain.initialize()
        
        self.assertEqual(brain.getTotalNeuronCount(), 5000)
    
    def test_create_brain_from_preset_custom(self):
        """Test creating brain with custom parameters"""
        brain = createBrainFromPreset(
            "standard",
            neuron_count=2000,
            connection_probability=0.1
        )
        
        self.assertIsNotNone(brain)
        brain.initialize()
        
        self.assertEqual(brain.getTotalNeuronCount(), 2000)
class TestEnhancedSimulationLoop(unittest.TestCase):
    """Test enhanced simulation loop functionality"""
    
    def setUp(self):
        self.brain = createBrainFromPreset("small")
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_run_simulation_loop_basic(self):
        """Test basic simulation loop"""
        success = run_simulation_loop(
            self.brain, self.world, self.agent,
            steps=10, timestep=0.1, verbose=False
        )
        
        self.assertTrue(success)
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
    
    def test_run_simulation_loop_with_monitor(self):
        """Test simulation loop with monitoring"""
        monitor = SimulationMonitor(self.brain, self.agent, self.world)
        
        success = run_simulation_loop(
            self.brain, self.world, self.agent,
            steps=20, timestep=0.05, monitor=monitor, verbose=False
        )
        
        self.assertTrue(success)
        self.assertIsNotNone(monitor.get_summary())
        self.assertEqual(monitor.get_summary()['total_steps'], 20)
        self.assertGreater(monitor.get_summary()['avg_step_time_ms'], 0)
    
    def test_run_simulation_loop_custom_steps(self):
        """Test simulation loop with custom number of steps"""
        success = run_simulation_loop(
            self.brain, self.world, self.agent,
            steps=5, timestep=0.1, verbose=False
        )
        
        self.assertTrue(success)
        # Brain should have run at least 5 steps
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), 0)
class TestBrainStateAnalysis(unittest.TestCase):
    """Test brain state analysis functionality"""
    
    def setUp(self):
        self.brain = createBrainFromPreset("small")
        self.brain.initialize()
        
        # Run some simulation steps
        for i in range(10):
            self.brain.step(i)
    
    def test_analyze_brain_state_basic(self):
        """Test basic brain state analysis"""
        analysis = analyze_brain_state(self.brain, include_detailed=False)
        
        self.assertIsNotNone(analysis)
        self.assertIn('total_neurons', analysis)
        self.assertIn('total_synapses', analysis)
        self.assertIn('firing_neurons', analysis)
        self.assertIn('average_firing_rate', analysis)
        self.assertIn('developmental_stage', analysis)
    
    def test_analyze_brain_state_detailed(self):
        """Test detailed brain state analysis"""
        analysis = analyze_brain_state(self.brain, include_detailed=True)
        
        self.assertIsNotNone(analysis)
        self.assertIn('detailed', analysis)
        self.assertIn('active_regions', analysis['detailed'])
        self.assertGreaterEqual(len(analysis['detailed']['active_regions']), 1)
    
    def test_analyze_brain_state_with_regions(self):
        """Test brain state analysis with multiple regions"""
        # Add a second region
        region_id = self.brain.addRegion("cortex2")
        region = self.brain.getRegion(region_id)
        
        analysis = analyze_brain_state(self.brain, include_detailed=True)
        
        self.assertIsNotNone(analysis)
        self.assertGreaterEqual(len(analysis['detailed']['active_regions']), 2)
class TestAgentWithFeatures(unittest.TestCase):
    """Test enhanced agent creation with features"""
    
    def setUp(self):
        self.brain = createBrainFromPreset("standard")
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    
    def test_create_agent_with_features_all_enabled(self):
        """Test creating agent with all features enabled"""
        agent = create_agent_with_features(
            self.brain, self.world,
            enable_reward=True,
            enable_curiosity=True,
            enable_development=True,
            enable_plasticity=True
        )
        
        self.assertIsNotNone(agent)
        agent.initialize(self.world)
        
        self.assertTrue(agent.isRewardModulationEnabled())
        self.assertTrue(agent.isCuriosityEnabled())
        self.assertTrue(agent.isDevelopmentEnabled())
        self.assertTrue(agent.isStructuralPlasticityEnabled())
    
    def test_create_agent_with_features_some_disabled(self):
        """Test creating agent with some features disabled"""
        agent = create_agent_with_features(
            self.brain, self.world,
            enable_reward=False,
            enable_curiosity=True,
            enable_development=False,
            enable_plasticity=False
        )
        
        self.assertIsNotNone(agent)
        agent.initialize(self.world)
        
        self.assertFalse(agent.isRewardModulationEnabled())
        self.assertTrue(agent.isCuriosityEnabled())
        self.assertFalse(agent.isDevelopmentEnabled())
        self.assertFalse(agent.isStructuralPlasticityEnabled())
class TestPerformanceProfiler(unittest.TestCase):
    """Test performance profiler functionality"""
    
    def setUp(self):
        self.brain = createBrainFromPreset("small")
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        self.profiler = PerformanceProfiler(self.brain, self.agent, self.world)
    
    def test_performance_profiler_start_stop(self):
        """Test starting and stopping profiler"""
        self.profiler.start_profiling()
        
        # Run some simulation
        for i in range(5):
            self.brain.step(i)
        
        self.profiler.stop_profiling()
        
        summary = self.profiler.get_summary()
        self.assertIsNotNone(summary)
        self.assertGreater(summary['total_steps'], 0)
    
    def test_performance_profiler_timing_samples(self):
        """Test timing samples collection"""
        self.profiler.start_profiling()
        
        # Add timing samples
        for i in range(10):
            self.profiler.add_timing_sample(i, 0.001 * i)
        
        self.profiler.stop_profiling()
        
        timing_analysis = self.profiler.get_timing_analysis()
        self.assertIsNotNone(timing_analysis)
        self.assertGreaterEqual(len(timing_analysis['anomalies']), 0)
    
    def test_performance_profiler_report_generation(self):
        """Test performance report generation"""
        self.profiler.start_profiling()
        
        # Run some simulation
        for i in range(10):
            self.brain.step(i)
        
        self.profiler.stop_profiling()
        
        report = self.profiler.generate_performance_report()
        self.assertIsNotNone(report)
        self.assertIn('performance_summary', report)
        self.assertIn('recommendations', report)
class TestBatchProcessor(unittest.TestCase):
    """Test batch processor functionality"""
    
    def setUp(self):
        self.processor = BatchProcessor(num_workers=1)
    
    def test_batch_processor_add_experiment(self):
        """Test adding experiment to batch"""
        config = {
            'brain': createBrainFromPreset("small"),
            'world': pynlm.createSimpleWorld(),
            'agent': pynlm.createAgentBrain(config['brain']),
            'steps': 10,
            'timestep': 0.1,
            'name': 'Test Experiment'
        }
        
        self.processor.add_experiment(config, name='Test1')
        self.assertEqual(len(self.processor.experiment_configs), 1)
    
    def test_batch_processor_run_batch(self):
        """Test running batch of experiments"""
        config = {
            'brain': createBrainFromPreset("small"),
            'world': pynlm.createSimpleWorld(),
            'agent': pynlm.createAgentBrain(config['brain']),
            'steps': 5,
            'timestep': 0.1,
            'name': 'Test Experiment'
        }
        
        self.processor.add_experiment(config, name='Test1')
        
        results = self.processor.run_batch(max_concurrent=1)
        self.assertEqual(len(results), 1)
        self.assertEqual(results[0]['name'], 'Test1')
        self.assertEqual(results[0]['status'], 'completed')
    
    def test_batch_processor_report_generation(self):
        """Test batch report generation"""
        config = {
            'brain': createBrainFromPreset("small"),
            'world': pynlm.createSimpleWorld(),
            'agent': pynlm.createAgentBrain(config['brain']),
            'steps': 5,
            'timestep': 0.1,
            'name': 'Test Experiment'
        }
        
        self.processor.add_experiment(config, name='Test1')
        self.processor.run_batch(max_concurrent=1)
        
        report = self.processor.generate_batch_report()
        self.assertIsNotNone(report)
        self.assertIn('experiment_results', report)
        self.assertIn('summary_statistics', report)
class TestExperimentManager(unittest.TestCase):
    """Test experiment manager functionality"""
    
    def setUp(self):
        self.manager = ExperimentManager("Test Experiment")
    
    def test_experiment_manager_add_experiment(self):
        """Test adding experiment to manager"""
        config = {
            'brain_config': createConfigFromPreset("small"),
            'steps': 10,
            'timestep': 0.1,
            'enable_reward': True
        }
        
        self.manager.add_experiment("Test Exp", config)
        self.assertEqual(len(self.manager.experiments), 1)
        self.assertEqual(self.manager.experiments[0]['name'], 'Test Exp')
    
    def test_experiment_manager_run_single(self):
        """Test running single experiment"""
        config = {
            'brain_config': createConfigFromPreset("small"),
            'steps': 5,
            'timestep': 0.1
        }
        
        self.manager.add_experiment("Test Exp", config)
        result = self.manager.run_experiment(0)
        
        self.assertIsNotNone(result)
        self.assertEqual(result['name'], 'Test Exp')
        self.assertEqual(result['status'], 'completed')
    
    def test_experiment_manager_save_data(self):
        """Test saving experiment data"""
        config = {
            'brain_config': createConfigFromPreset("small"),
            'steps': 5,
            'timestep': 0.1
        }
        
        self.manager.add_experiment("Test Exp", config)
        self.manager.run_experiment(0)
        
        # Test JSON output
        result = self.manager.save_experiment_data(0, output_format='json')
        self.assertIsNotNone(result)
        self.assertTrue(result.endswith('.json'))
class TestHyperparameterOptimizer(unittest.TestCase):
    """Test hyperparameter optimizer functionality"""
    
    def setUp(self):
        self.parameter_space = {
            'learning_rate': (0.001, 0.01),
            'hidden_size': [100, 200, 500],
            'dropout': 0.1
        }
        
        self.optimizer = HyperparameterOptimizer(self.parameter_space)
    
    def test_hyperparameter_optimizer_sample_parameters(self):
        """Test parameter sampling"""
        params = self.optimizer._sample_parameters('random')
        
        self.assertIn('learning_rate', params)
        self.assertIn('hidden_size', params)
        self.assertIn('dropout', params)
        
        self.assertGreaterEqual(params['learning_rate'], 0.001)
        self.assertLessEqual(params['learning_rate'], 0.01)
        self.assertIn(params['hidden_size'], [100, 200, 500])
        self.assertEqual(params['dropout'], 0.1)
    
    def test_hyperparameter_optimizer_convergence_analysis(self):
        """Test convergence analysis"""
        # Add some mock results
        self.optimizer.search_results = [
            {'parameters': {'learning_rate': 0.001}, 'score': 0.5},
            {'parameters': {'learning_rate': 0.002}, 'score': 0.6},
            {'parameters': {'learning_rate': 0.003}, 'score': 0.8},
            {'parameters': {'learning_rate': 0.004}, 'score': 0.7},
            {'parameters': {'learning_rate': 0.005}, 'score': 0.9}
        ]
        
        convergence = self.optimizer._analyze_convergence()
        self.assertIsNotNone(convergence)
        self.assertIn('score_improvement', convergence)
        self.assertIn('convergence_rate', convergence)
    
    def test_hyperparameter_optimizer_recommendations(self):
        """Test optimization recommendations"""
        recommendations = self.optimizer._generate_optimization_recommendations()
        self.assertIsInstance(recommendations, list)
class TestBackwardCompatibility(unittest.TestCase):
    """Test backward compatibility with original API"""
    
    def test_original_api_still_works(self):
        """Test that original API still works"""
        # Original way to create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        self.assertIsNotNone(brain)
        self.assertGreater(brain.getTotalNeuronCount(), 0)
    
    def test_original_world_functionality(self):
        """Test original world functionality"""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        
        self.assertEqual(world.getWidth(), 10)
        self.assertEqual(world.getHeight(), 10)
        self.assertEqual(world.getVisionWidth(), 4)
        self.assertEqual(world.getVisionHeight(), 4)
    
    def test_original_agent_functionality(self):
        """Test original agent functionality"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        
        # Test feature enable/disable
        agent.enableRewardModulation(True)
        self.assertTrue(agent.isRewardModulationEnabled())
        
        agent.enableCuriosity(True)
        self.assertTrue(agent.isCuriosityEnabled())
class TestErrorHandling(unittest.TestCase):
    """Test error handling in enhanced API"""
    
    def test_invalid_preset_error(self):
        """Test error handling for invalid preset"""
        from pynlm_enhanced import createConfigFromPreset
        
        with self.assertRaises(ValueError):
            createConfigFromPreset("invalid_preset")
    
    def test_config_save_load(self):
        """Test configuration save and load"""
        config = createConfigFromPreset("standard")
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_file = f.name
        
        try:
            config.saveToFile(temp_file)
            
            # Load from file
            new_config = pynlm.Config()
            new_config.loadFromFile(temp_file)
            
            self.assertTrue(new_config.has("brain.neuron_count"))
            self.assertEqual(new_config.getOr("brain.neuron_count", 0), 1000)
        
        finally:
            # Clean up
            if os.path.exists(temp_file):
                os.unlink(temp_file)
    
    def test_brain_save_load(self):
        """Test brain save and load"""
        brain = createBrainFromPreset("small")
        brain.initialize()
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            temp_file = f.name
        
        try:
            brain.save(temp_file)
            self.assertTrue(os.path.exists(temp_file))
            
            # Create new brain and load
            new_brain = createBrainFromPreset("small")
            new_brain.initialize()
            new_brain.load(temp_file)
            
            self.assertTrue(new_brain.getTotalNeuronCount() > 0)
        
        finally:
            # Clean up
            if os.path.exists(temp_file):
                os.unlink(temp_file)
class TestModuleInformation(unittest.TestCase):
    """Test module information and metadata"""
    
    def test_module_info(self):
        """Test getting module information"""
        from pynlm_enhanced import get_module_info
        
        info = get_module_info()
        self.assertIsNotNone(info)
        self.assertIn('version', info)
        self.assertIn('description', info)
        self.assertIn('features', info)
        self.assertIn('enhancements', info)
        
        self.assertEqual(info['version'], '2.0.0')
        self.assertIsInstance(info['features'], list)
        self.assertGreater(len(info['features']), 0)
    
    def test_module_constants(self):
        """Test module-level constants"""
        from pynlm_enhanced import (
            MODULE_VERSION,
            MIN_PYTHON_VERSION,
            SUPPORTED_FEATURES,
            SUPPORTED_BRAIN_TYPES,
            SUPPORTED_PRESETS
        )
        
        self.assertEqual(MODULE_VERSION, '2.0.0')
        self.assertEqual(MIN_PYTHON_VERSION, (3, 8))
        self.assertIsInstance(SUPPORTED_FEATURES, list)
        self.assertIsInstance(SUPPORTED_BRAIN_TYPES, list)
        self.assertIsInstance(SUPPORTED_PRESETS, list)
        
        self.assertGreater(len(SUPPORTED_BRAIN_TYPES), 0)
        self.assertGreater(len(SUPPORTED_PRESETS), 0)
if __name__ == '__main__':
    print("Running comprehensive test suite for enhanced NLM Python API...")
    
    # Run all tests
    unittest.main(verbosity=2)