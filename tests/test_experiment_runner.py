#!/usr/bin/env python3
"""
Experiment Runner Unit Tests for NLM
Comprehensive unit tests for NLM ExperimentRunner functionality
"""

import unittest
import sys
import os
from unittest.mock import Mock, patch, MagicMock
import tempfile
import json
import time
import threading
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
from python.experiments.ExperimentRunner import (
    ExperimentRunner, 
    ExperimentConfig, 
    ExperimentResult,
    ExperimentStatus,
    ExperimentType
)

class TestExperimentRunner(unittest.TestCase):
    """Test cases for ExperimentRunner class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.runner = ExperimentRunner()
        
        # Mock pynlm functions to avoid dependency issues
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
    
    def testExperimentConfigCreation(self):
        """Test ExperimentConfig creation"""
        config = ExperimentConfig(
            name="test_experiment",
            exp_type=ExperimentType.SILENT_BRAIN,
            max_steps=100,
            description="Test experiment"
        )
        
        self.assertEqual(config.name, "test_experiment")
        self.assertEqual(config.exp_type, ExperimentType.SILENT_BRAIN)
        self.assertEqual(config.max_steps, 100)
        self.assertEqual(config.description, "Test experiment")
        self.assertEqual(len(config.tags), 0)
    
    def testExperimentResultCreation(self):
        """Test ExperimentResult creation"""
        result = ExperimentResult(
            success=True,
            steps_completed=50,
            total_steps=100,
            execution_time=5.0,
            final_stats={'neurons': 100, 'synapses': 1000}
        )
        
        self.assertTrue(result.success)
        self.assertEqual(result.steps_completed, 50)
        self.assertEqual(result.total_steps, 100)
        self.assertEqual(result.execution_time, 5.0)
        self.assertEqual(result.final_stats['neurons'], 100)
    
    def testAddExperiment(self):
        """Test adding an experiment"""
        config = ExperimentConfig(
            name="test_exp",
            exp_type=ExperimentType.WORLD_EXPLORATION
        )
        
        self.runner.add_experiment(config)
        
        self.assertIn("test_exp", self.runner.experiments)
        self.assertEqual(self.runner.status["test_exp"], ExperimentStatus.PENDING)
    
    def testRemoveExperiment(self):
        """Test removing an experiment"""
        config = ExperimentConfig(
            name="test_exp",
            exp_type=ExperimentType.WORLD_EXPLORATION
        )
        self.runner.add_experiment(config)
        
        self.runner.remove_experiment("test_exp")
        
        self.assertNotIn("test_exp", self.runner.experiments)
        self.assertNotIn("test_exp", self.runner.status)
        self.assertNotIn("test_exp", self.runner.results)
    
    def testSetProgressCallback(self):
        """Test setting progress callback"""
        def progress_callback(step, total):
            pass
        
        self.runner.set_progress_callback("test_exp", progress_callback)
        
        self.assertIn("test_exp", self.runner._progress_callbacks)
    
    def testLoadConfigFile(self):
        """Test loading experiment configuration from file"""
        # Create a temporary config file
        config_data = {
            "exp1": {
                'type': 'silent_brain',
                'brain_config': {'neurons': 50},
                'world_config': {'width': 20},
                'max_steps': 200,
                'timestep': 0.1,
                'description': 'Test experiment',
                'tags': ['test', 'basic']
            }
        }
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(config_data, f)
            config_file = f.name
        
        try:
            # Mock the pynlm module for loading
            with patch('pynlm') as mock_pynlm:
                mock_pynlm.createDefaultConfig = Mock(return_value=Mock())
                mock_pynlm.createBrain = Mock(return_value=Mock())
                
                self.runner.load_config(config_file)
                
                self.assertIn("exp1", self.runner.experiments)
                self.assertEqual(self.runner.experiments["exp1"].name, "exp1")
                self.assertEqual(self.runner.experiments["exp1"].exp_type, ExperimentType.SILENT_BRAIN)
                self.assertEqual(self.runner.experiments["exp1"].max_steps, 200)
                
        finally:
            os.unlink(config_file)
    
    def testRunExperimentSuccess(self):
        """Test successful experiment execution"""
        # Mock brain and world
        mock_brain = Mock()
        mock_brain.initialize = Mock(return_value=True)
        mock_brain.total_neuron_count = Mock(return_value=100)
        mock_brain.total_synapse_count = Mock(return_value=1000)
        mock_brain.firing_neuron_count = Mock(return_value=10)
        mock_brain.active_neuron_count = Mock(return_value=5)
        mock_brain.total_spike_count = Mock(return_value=1000)
        mock_brain.average_firing_rate = Mock(return_value=10.0)
        mock_brain.developmental_stage = Mock(return_value=Mock(value='adult'))
        mock_brain.excitation_inhibition_ratio = Mock(return_value=2.0)
        
        mock_world = Mock()
        mock_world.update = Mock()
        mock_world.getSensoryPercept = Mock(return_value=Mock())
        mock_world.getSimulationTime = Mock(return_value=0.0)
        
        mock_agent = Mock()
        mock_agent.processSensoryInput = Mock()
        mock_agent.decodeMotorCommand = Mock(return_value=Mock())
        mock_agent.applyRewardModulation = Mock()
        mock_agent.enableRewardModulation = Mock()
        mock_agent.enableCuriosity = Mock()
        mock_agent.enableStructuralPlasticity = Mock()
        mock_agent.enableDevelopment = Mock()
        mock_agent.getPredictionError = Mock(return_value=0.5)
        
        # Configure mocks
        with patch('pynlm.createDefaultConfig') as mock_create_config:
            with patch('pynlm.createBrain') as mock_create_brain:
                with patch('pynlm.createSimpleWorld') as mock_create_world:
                    with patch('pynlm.createAgentBrain') as mock_create_agent:
                        mock_create_config.return_value = Mock()
                        mock_create_brain.return_value = mock_brain
                        mock_create_world.return_value = mock_world
                        mock_create_agent.return_value = mock_agent
                        
                        config = ExperimentConfig(
                            name="success_exp",
                            exp_type=ExperimentType.SILENT_BRAIN,
                            max_steps=50,
                            timestep=0.1
                        )
                        
                        self.runner.add_experiment(config)
                        
                        # Mock progress callback
                        progress_called = []
                        def progress_callback(step, total):
                            progress_called.append((step, total))
                        
                        result = self.runner.run_experiment("success_exp", progress_callback)
                        
                        # Verify result
                        self.assertTrue(result.success)
                        self.assertEqual(result.steps_completed, 50)
                        self.assertEqual(result.total_steps, 50)
                        self.assertGreater(result.execution_time, 0)
                        self.assertIn('total_neurons', result.final_stats)
                        
                        # Verify progress callback was called
                        self.assertGreater(len(progress_called), 0)
    
    def testRunExperimentFailure(self):
        """Test experiment execution failure"""
        # Configure mocks to raise an exception
        with patch('pynlm.createBrain') as mock_create_brain:
            mock_create_brain.side_effect = RuntimeError("Brain creation failed")
            
            config = ExperimentConfig(
                name="failure_exp",
                exp_type=ExperimentType.WORLD_EXPLORATION,
                max_steps=50
            )
            
            self.runner.add_experiment(config)
            
            with self.assertRaises(RuntimeError):
                self.runner.run_experiment("failure_exp")
            
            # Verify experiment is marked as failed
            self.assertEqual(self.runner.status["failure_exp"], ExperimentStatus.FAILED)
    
    def testRunExperimentsBatch(self):
        """Test running multiple experiments in batch"""
        # Create mock brains for both experiments
        mock_brain = Mock()
        mock_brain.initialize = Mock(return_value=True)
        mock_brain.step = Mock()
        mock_brain.total_neuron_count = Mock(return_value=100)
        mock_brain.total_synapse_count = Mock(return_value=1000)
        mock_brain.firing_neuron_count = Mock(return_value=10)
        mock_brain.active_neuron_count = Mock(return_value=5)
        mock_brain.total_spike_count = Mock(return_value=1000)
        mock_brain.average_firing_rate = Mock(return_value=10.0)
        mock_brain.developmental_stage = Mock(return_value=Mock(value='adult'))
        mock_brain.excitation_inhibition_ratio = Mock(return_value=2.0)
        
        with patch('pynlm.createBrain') as mock_create_brain:
            mock_create_brain.return_value = mock_brain
            
            # Add two experiments
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
            
            self.runner.add_experiment(config1)
            self.runner.add_experiment(config2)
            
            # Run batch
            results = self.runner.run_experiments_batch(["batch_exp1", "batch_exp2"])
            
            # Verify results
            self.assertIn("batch_exp1", results)
            self.assertIn("batch_exp2", results)
            self.assertTrue(results["batch_exp1"].success)
            self.assertTrue(results["batch_exp2"].success)
            self.assertEqual(results["batch_exp1"].steps_completed, 30)
            self.assertEqual(results["batch_exp2"].steps_completed, 40)
    
    def testGetExperimentStatus(self):
        """Test getting experiment status"""
        config = ExperimentConfig(
            name="status_exp",
            exp_type=ExperimentType.DEVELOPMENT
        )
        
        self.runner.add_experiment(config)
        
        # Initially should be pending
        self.assertEqual(self.runner.get_experiment_status("status_exp"), ExperimentStatus.PENDING)
        
        # Manually set status to running
        self.runner.status["status_exp"] = ExperimentStatus.RUNNING
        
        self.assertEqual(self.runner.get_experiment_status("status_exp"), ExperimentStatus.RUNNING)
    
    def testGetExperimentResult(self):
        """Test getting experiment result"""
        # Add a result
        result = ExperimentResult(
            success=True,
            steps_completed=25,
            total_steps=50,
            execution_time=2.5,
            final_stats={'neurons': 100}
        )
        self.runner.results["test_exp"] = result
        
        retrieved = self.runner.get_experiment_result("test_exp")
        
        self.assertEqual(retrieved.success, True)
        self.assertEqual(retrieved.steps_completed, 25)
        self.assertEqual(retrieved.total_steps, 50)
        self.assertEqual(retrieved.execution_time, 2.5)
    
    def testListExperiments(self):
        """Test listing experiments"""
        config1 = ExperimentConfig(
            name="list_exp1",
            exp_type=ExperimentType.SILENT_BRAIN,
            description="First experiment",
            max_steps=100,
            tags=['test', 'basic']
        )
        
        config2 = ExperimentConfig(
            name="list_exp2",
            exp_type=ExperimentType.WORLD_EXPLORATION,
            description="Second experiment",
            max_steps=200,
            tags=['integration']
        )
        
        self.runner.add_experiment(config1)
        self.runner.add_experiment(config2)
        
        # Set some statuses
        self.runner.status["list_exp1"] = ExperimentStatus.COMPLETED
        self.runner.status["list_exp2"] = ExperimentStatus.PENDING
        
        experiment_info = self.runner.list_experiments()
        
        self.assertIn("list_exp1", experiment_info)
        self.assertIn("list_exp2", experiment_info)
        self.assertEqual(experiment_info["list_exp1"]["description"], "First experiment")
        self.assertEqual(experiment_info["list_exp1"]["max_steps"], 100)
        self.assertEqual(experiment_info["list_exp1"]["type"], "silent_brain")
        self.assertEqual(experiment_info["list_exp1"]["tags"], ['test', 'basic'])
        self.assertEqual(experiment_info["list_exp1"]["status"], "completed")
    
    def testExportResults(self):
        """Test exporting experiment results to file"""
        # Add an experiment with results
        config = ExperimentConfig(
            name="export_exp",
            exp_type=ExperimentType.SILENT_BRAIN
        )
        self.runner.add_experiment(config)
        
        result = ExperimentResult(
            success=True,
            steps_completed=20,
            total_steps=50,
            execution_time=1.5,
            final_stats={'neurons': 100}
        )
        self.runner.results["export_exp"] = result
        self.runner.status["export_exp"] = ExperimentStatus.COMPLETED
        
        # Export to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            export_file = f.name
        
        try:
            self.runner.export_results(export_file)
            
            # Verify file was created and contains data
            self.assertTrue(os.path.exists(export_file))
            
            with open(export_file, 'r') as f:
                export_data = json.load(f)
            
            self.assertIn("export_exp", export_data)
            self.assertIn("config", export_data["export_exp"])
            self.assertIn("result", export_data["export_exp"])
            self.assertIn("status", export_data["export_exp"])
            
        finally:
            if os.path.exists(export_file):
                os.unlink(export_file)

if __name__ == '__main__':
    unittest.main()
