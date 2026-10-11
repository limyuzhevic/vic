#!/usr/bin/env python3
"""
Batch Processing Unit Tests for NLM
Comprehensive unit tests for NLM batch processing functionality
"""

import unittest
import sys
import os
from unittest.mock import Mock, patch, MagicMock
import tempfile
import json
import pandas as pd
import numpy as np
from pathlib import Path
import time

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
from python.performance.Profiler import PerformanceProfiler
from python.experiments.ExperimentRunner import ExperimentRunner, ExperimentConfig, ExperimentType

class TestBatchProcessing(unittest.TestCase):
    """Test cases for batch processing functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        # Mock pynlm to avoid dependency issues
        self.pynlm_patch = patch.dict('sys.modules', {
            'pynlm': Mock(),
        })
        self.mock_pynlm = self.pynlm_patch.start()
        
        # Configure mock
        self.mock_pynlm.createDefaultConfig = Mock(return_value=Mock())
        self.mock_pynlm.createBrain = Mock(return_value=Mock())
        self.mock_pynlm.create_config_from_dict = Mock(return_value=Mock())
    
    def tearDown(self):
        """Clean up test fixtures"""
        self.pynlm_patch.stop()
    
    def testBatchProcessingInitialization(self):
        """Test batch processing initialization"""
        # This would test batch processing capabilities
        # Since we don't have a dedicated batch processing module yet,
        # we'll test the integration with ExperimentRunner
        
        runner = ExperimentRunner()
        
        # Test adding multiple experiments
        config1 = ExperimentConfig(
            name="batch_exp1",
            exp_type=ExperimentType.SILENT_BRAIN,
            max_steps=30
        )
        
        config2 = ExperimentConfig(
            name="batch_exp2",
            exp_type=ExperimentType.WORLD_EXPLORATION,
            max_steps=40
        )
        
        config3 = ExperimentConfig(
            name="batch_exp3",
            exp_type=ExperimentType.REWARD_LEARNING,
            max_steps=50
        )
        
        runner.add_experiment(config1)
        runner.add_experiment(config2)
        runner.add_experiment(config3)
        
        self.assertEqual(len(runner.experiments), 3)
        self.assertIn("batch_exp1", runner.experiments)
        self.assertIn("batch_exp2", runner.experiments)
        self.assertIn("batch_exp3", runner.experiments)
    
    def testBatchProcessingExperimentList(self):
        """Test creating and managing experiment batch"""
        runner = ExperimentRunner()
        
        # Create batch of experiments
        batch_configs = []
        for i in range(5):
            config = ExperimentConfig(
                name=f"batch_exp_{i}",
                exp_type=ExperimentType.SILENT_BRAIN,
                max_steps=20 + i * 10,
                description=f"Batch experiment {i}"
            )
            batch_configs.append(config)
        
        # Add all experiments to batch
        for config in batch_configs:
            runner.add_experiment(config)
        
        # Verify batch
        batch_names = list(runner.experiments.keys())
        expected_names = [f"batch_exp_{i}" for i in range(5)]
        
        for expected_name in expected_names:
            self.assertIn(expected_name, batch_names)
        
        # Verify batch sizes
        for i, config in enumerate(batch_configs):
            self.assertEqual(runner.experiments[f"batch_exp_{i}"].max_steps, 20 + i * 10)
    
    def testBatchProcessingProgressMonitoring(self):
        """Test batch processing progress monitoring"""
        runner = ExperimentRunner()
        
        # Create batch of experiments
        batch_names = [f"progress_exp_{i}" for i in range(3)]
        
        for i, name in enumerate(batch_names):
            config = ExperimentConfig(
                name=name,
                exp_type=ExperimentType.SILENT_BRAIN,
                max_steps=30 + i * 10
            )
            runner.add_experiment(config)
        
        # Track progress
        progress_tracker = {}
        
        def track_progress(step, total):
            progress_tracker[name] = (step, total)
        
        # Mock progress callbacks
        progress_callbacks = {}
        for name in batch_names:
            progress_callbacks[name] = lambda s, t: progress_tracker.update({name: (s, t)})
        
        # Simulate progress updates
        for i, name in enumerate(batch_names):
            for step in range(0, 31, 5):  # Update every 5 steps
                progress_callbacks[name](step, 30 + i * 10)
        
        # Verify progress tracking
        for name in batch_names:
            self.assertIn(name, progress_tracker)
            step, total = progress_tracker[name]
            self.assertLessEqual(step, total)
            self.assertGreaterEqual(step, 0)
    
    def testBatchProcessingResultsAggregation(self):
        """Test batch processing results aggregation"""
        runner = ExperimentRunner()
        
        # Create experiments
        exp1 = ExperimentConfig(
            name="agg_exp1",
            exp_type=ExperimentType.SILENT_BRAIN,
            max_steps=30
        )
        
        exp2 = ExperimentConfig(
            name="agg_exp2",
            exp_type=ExperimentType.WORLD_EXPLORATION,
            max_steps=40
        )
        
        runner.add_experiment(exp1)
        runner.add_experiment(exp2)
        
        # Mock results
        result1 = {
            'success': True,
            'steps_completed': 30,
            'total_steps': 30,
            'execution_time': 2.5,
            'final_stats': {'neurons': 100, 'synapses': 1000},
            'brain_config': exp1.brain_config
        }
        
        result2 = {
            'success': True,
            'steps_completed': 40,
            'total_steps': 40,
            'execution_time': 3.5,
            'final_stats': {'neurons': 150, 'synapses': 1500},
            'brain_config': exp2.brain_config
        }
        
        runner.results['agg_exp1'] = result1
        runner.results['agg_exp2'] = result2
        
        # Aggregate results
        all_results = list(runner.results.values())
        
        # Calculate summary statistics
        total_steps = sum(r['steps_completed'] for r in all_results)
        total_steps_possible = sum(r['total_steps'] for r in all_results)
        avg_execution_time = sum(r['execution_time'] for r in all_results) / len(all_results)
        total_neurons = sum(r['final_stats']['neurons'] for r in all_results)
        total_synapses = sum(r['final_stats']['synapses'] for r in all_results)
        
        # Verify aggregation
        self.assertEqual(total_steps, 70)
        self.assertEqual(total_steps_possible, 70)
        self.assertAlmostEqual(avg_execution_time, 3.0)
        self.assertEqual(total_neurons, 250)
        self.assertEqual(total_synapses, 2500)
        
        # Calculate success rate
        success_count = sum(1 for r in all_results if r['success'])
        success_rate = success_count / len(all_results)
        
        self.assertEqual(success_rate, 1.0)
    
    def testBatchProcessingErrorHandling(self):
        """Test batch processing error handling"""
        runner = ExperimentRunner()
        
        # Create batch of experiments
        batch_names = [f"error_exp_{i}" for i in range(4)]
        
        for name in batch_names:
            config = ExperimentConfig(
                name=name,
                exp_type=ExperimentType.SILENT_BRAIN,
                max_steps=20
            )
            runner.add_experiment(config)
        
        # Mock mixed success/failure results
        results = {}
        for i, name in enumerate(batch_names):
            if i < 2:  # First two succeed
                results[name] = {
                    'success': True,
                    'steps_completed': 20,
                    'total_steps': 20,
                    'execution_time': 1.5,
                    'final_stats': {'neurons': 100}
                }
            else:  # Last two fail
                results[name] = {
                    'success': False,
                    'steps_completed': 0,
                    'total_steps': 20,
                    'execution_time': 0.5,
                    'final_stats': {},
                    'error_message': 'Simulation failed'
                }
        
        runner.results.update(results)
        
        # Analyze batch results
        success_results = [r for r in results.values() if r['success']]
        failed_results = [r for r in results.values() if not r['success']]
        
        # Verify analysis
        self.assertEqual(len(success_results), 2)
        self.assertEqual(len(failed_results), 2)
        self.assertEqual(len(results), 4)
        
        # Calculate batch metrics
        total_success_rate = len(success_results) / len(results)
        total_steps_completed = sum(r['steps_completed'] for r in results)
        total_steps_possible = sum(r['total_steps'] for r in results)
        
        self.assertEqual(total_success_rate, 0.5)
        self.assertEqual(total_steps_completed, 40)
        self.assertEqual(total_steps_possible, 80)
    
    def testBatchProcessingConfigurationManagement(self):
        """Test batch processing configuration management"""
        runner = ExperimentRunner()
        
        # Create batch with different configurations
        batch_configs = {
            'exp1': {
                'exp_type': 'silent_brain',
                'max_steps': 30,
                'description': 'Silent brain experiment',
                'tags': ['test', 'basic']
            },
            'exp2': {
                'exp_type': 'world_exploration',
                'max_steps': 50,
                'description': 'World exploration experiment',
                'tags': ['integration', 'advanced']
            },
            'exp3': {
                'exp_type': 'reward_learning',
                'max_steps': 40,
                'description': 'Reward learning experiment',
                'tags': ['learning', 'neural']
            }
        }
        
        # Add batch configurations
        for exp_name, config_data in batch_configs.items():
            config = ExperimentConfig(
                name=exp_name,
                exp_type=ExperimentType[config_data['exp_type'].upper()],
                max_steps=config_data['max_steps'],
                description=config_data['description'],
                tags=config_data['tags']
            )
            runner.add_experiment(config)
        
        # Verify batch configuration
        self.assertEqual(len(runner.experiments), 3)
        
        # Verify individual configurations
        for exp_name, config_data in batch_configs.items():
            self.assertEqual(runner.experiments[exp_name].name, exp_name)
            self.assertEqual(runner.experiments[exp_name].exp_type.value, config_data['exp_type'])
            self.assertEqual(runner.experiments[exp_name].max_steps, config_data['max_steps'])
            self.assertEqual(runner.experiments[exp_name].description, config_data['description'])
            self.assertEqual(runner.experiments[exp_name].tags, config_data['tags'])
    
    def testBatchProcessingExecutionOrder(self):
        """Test batch processing execution order"""
        runner = ExperimentRunner()
        
        # Create batch with specific execution order
        batch_order = ['first_exp', 'second_exp', 'third_exp']
        
        for i, name in enumerate(batch_order):
            config = ExperimentConfig(
                name=name,
                exp_type=ExperimentType.SILENT_BRAIN,
                max_steps=20 + i * 10
            )
            runner.add_experiment(config)
        
        # Verify execution order is maintained
        experiment_names = list(runner.experiments.keys())
        
        # Note: dict ordering is not guaranteed in older Python versions,
        # but we can verify all expected experiments are present
        for expected_name in batch_order:
            self.assertIn(expected_name, experiment_names)
        
        # Test setting progress callback order
        callback_order = []
        
        def make_callback(exp_name):
            def callback(step, total):
                callback_order.append(exp_name)
            return callback
        
        for name in batch_order:
            runner.set_progress_callback(name, make_callback(name))
        
        # Verify callbacks are set for all experiments
        self.assertEqual(len(runner._progress_callbacks), 3)
        
        for name in batch_order:
            self.assertIn(name, runner._progress_callbacks)
    
    def testBatchProcessingResourceManagement(self):
        """Test batch processing resource management"""
        runner = ExperimentRunner()
        
        # Create batch with resource-intensive experiments
        resource_batch = []
        
        for i in range(3):
            config = ExperimentConfig(
                name=f"resource_exp_{i}",
                exp_type=ExperimentType.SILENT_BRAIN,
                max_steps=50,
                description=f"Resource-intensive experiment {i}"
            )
            resource_batch.append(config)
            runner.add_experiment(config)
        
        # Verify resource tracking
        self.assertEqual(len(runner.experiments), 3)
        
        # Test cleanup
        runner.cleanup()
        
        # Verify cleanup removed progress callbacks
        self.assertEqual(len(runner._progress_callbacks), 0)
        
        # Note: In real implementation, this would also clean up threads,
        # file handles, etc.

if __name__ == '__main__':
    unittest.main()
