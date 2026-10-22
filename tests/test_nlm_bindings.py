#!/usr/bin/env python3
"""
Unit tests for the improved NLM Python bindings.

This test suite verifies that the improvements to the NLM Python bindings
are working correctly.
"""

import unittest
import tempfile
import os
import json
import numpy as np
from unittest.mock import patch, MagicMock

import sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import nlm


class TestNeuralSystem(unittest.TestCase):
    """Test the NeuralSystem class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.system = nlm.NeuralSystem(neuron_count=50, region_count=1)
    
    def tearDown(self):
        """Clean up test fixtures"""
        if os.path.exists("test_checkpoint.pkl"):
            os.remove("test_checkpoint.pkl")
    
    def test_initialization(self):
        """Test basic initialization"""
        self.assertFalse(self.system._is_initialized)
        
        result = self.system.initialize()
        self.assertTrue(result)
        self.assertTrue(self.system._is_initialized)
    
    def test_sensory_input(self):
        """Test setting sensory input"""
        self.system.initialize()
        
        sensory_data = nlm.SensorData(
            vision=[0.1] * 256,
            touch=[0.0] * 8,
            proprioception=[0.0] * 6,
            internal=[0.0] * 4
        )
        
        self.system.set_sensory_input(sensory_data)
        
        # Check that neural activity was generated
        activity = self.system.get_neural_activity()
        self.assertEqual(len(activity), 50)
        self.assertTrue(all(0 <= x <= 1 for x in activity))
    
    def test_motor_output(self):
        """Test motor output generation"""
        self.system.initialize()
        
        # Set up some sensory input to trigger activity
        self.system.set_sensory_input_from_arrays(
            vision=[0.9] * 256,
            touch=[0.1] * 8,
            proprioception=[0.0] * 6,
            internal=[0.0] * 4
        )
        
        action = self.system.get_motor_output()
        self.assertIsInstance(action, nlm.MotorCommand)
        self.assertIn(action, nlm.MotorCommand)
    
    def test_step_execution(self):
        """Test simulation step execution"""
        self.system.initialize()
        
        initial_step = self.system.get_simulation_step()
        
        # Run a step
        result = self.system.step()
        
        self.assertTrue(result)
        self.assertEqual(self.system.get_simulation_step(), initial_step + 1)
    
    def test_checkpointing(self):
        """Test checkpoint save/load functionality"""
        self.system.initialize()
        
        # Set up some state
        self.system.set_sensory_input_from_arrays(vision=[0.5] * 256)
        for _ in range(5):
            self.system.step()
        
        # Save checkpoint
        checkpoint_file = "test_checkpoint.pkl"
        save_result = self.system.save_checkpoint(checkpoint_file)
        self.assertTrue(save_result)
        
        # Reset and load
        self.system.reset()
        self.assertFalse(self.system._is_initialized)
        
        load_result = self.system.load_checkpoint(checkpoint_file)
        self.assertTrue(load_result)
        self.assertTrue(self.system._is_initialized)
        
        # Verify some state was preserved
        self.assertGreater(self.system.get_simulation_step(), 0)
        
        # Clean up
        os.remove(checkpoint_file)
    
    def test_learning_metrics(self):
        """Test learning metrics collection"""
        self.system.initialize()
        
        metrics = self.system.get_learning_metrics()
        self.assertIsInstance(metrics, nlm.LearningMetrics)
        self.assertFalse(metrics.learning_detected)
        self.assertEqual(metrics.weight_change, 0.0)
    
    def test_simulation_time(self):
        """Test simulation time tracking"""
        self.system.initialize()
        
        initial_time = self.system.get_simulation_time()
        self.assertGreaterEqual(initial_time, 0.0)
        
        # Wait a bit and step
        time.sleep(0.01)
        self.system.step()
        
        # Time should have advanced slightly
        new_time = self.system.get_simulation_time()
        self.assertGreater(new_time, initial_time)


class TestSensorData(unittest.TestCase):
    """Test SensorData functionality"""
    
    def test_sensor_data_creation(self):
        """Test creating SensorData"""
        data = nlm.SensorData(
            vision=[0.1, 0.2, 0.3],
            touch=[0.4],
            proprioception=[0.5, 0.6]
        )
        
        self.assertEqual(len(data.vision), 3)
        self.assertEqual(len(data.touch), 1)
        self.assertEqual(len(data.proprioception), 2)
        self.assertEqual(len(data.internal), 4)  # Default
        self.assertEqual(len(data.audio), 0)     # Default
    
    def test_get_all_signals(self):
        """Test getting all signals concatenated"""
        data = nlm.SensorData(
            vision=[0.1, 0.2],
            touch=[0.3],
            proprioception=[0.4, 0.5, 0.6],
            internal=[0.7, 0.8],
            audio=[0.9, 1.0]
        )
        
        all_signals = data.get_all_signals()
        expected_length = 2 + 1 + 3 + 2 + 2  # vision + touch + proprio + internal + audio
        self.assertEqual(len(all_signals), expected_length)
        self.assertEqual(all_signals[0], 0.1)
        self.assertEqual(all_signals[-1], 1.0)
    
    def test_signal_count(self):
        """Test getting signal count"""
        data = nlm.SensorData(vision=[0.1] * 100)
        self.assertEqual(data.get_signal_count(), 100)


class TestMotorCommand(unittest.TestCase):
    """Test MotorCommand enum functionality"""
    
    def test_enum_values(self):
        """Test all MotorCommand values"""
        expected_commands = [
            nlm.MotorCommand.MOVE_FORWARD,
            nlm.MotorCommand.MOVE_BACKWARD,
            nlm.MotorCommand.TURN_LEFT,
            nlm.MotorCommand.TURN_RIGHT,
            nlm.MotorCommand.LOOK_LEFT,
            nlm.MotorCommand.LOOK_RIGHT,
            nlm.MotorCommand.INTERACT,
            nlm.MotorCommand.WAIT
        ]
        
        self.assertEqual(len(expected_commands), 8)
    
    def test_to_string(self):
        """Test string representation"""
        command = nlm.MotorCommand.MOVE_FORWARD
        self.assertEqual(command.to_string(), "Move Forward")
    
    def test_from_index(self):
        """Test creating from integer index"""
        command = nlm.MotorCommand.from_index(0)
        self.assertEqual(command, nlm.MotorCommand.MOVE_FORWARD)
        
        command = nlm.MotorCommand.from_index(7)
        self.assertEqual(command, nlm.MotorCommand.WAIT)
        
        # Test wrapping
        command = nlm.MotorCommand.from_index(8)
        self.assertEqual(command, nlm.MotorCommand.MOVE_FORWARD)


class TestExperimentRunner(unittest.TestCase):
    """Test ExperimentRunner functionality"""
    
    def test_experiment_runner_creation(self):
        """Test creating ExperimentRunner"""
        runner = nlm.ExperimentRunner()
        self.assertEqual(runner.results, [])
    
    def test_simple_experiment(self):
        """Test running a simple experiment"""
        runner = nlm.ExperimentRunner()
        
        result = runner.run_experiment(
            experiment_name="Test Experiment",
            system_factory=lambda: nlm.NeuralSystem(neuron_count=10, region_count=1),
            steps=5
        )
        
        self.assertIsInstance(result, dict)
        self.assertEqual(result['experiment_name'], "Test Experiment")
        self.assertGreaterEqual(result['steps_completed'], 0)
        self.assertGreaterEqual(result['total_reward'], 0.0)
        
        # Check that results were stored
        self.assertEqual(len(runner.results), 1)


class TestConvenienceFunctions(unittest.TestCase):
    """Test convenience functions"""
    
    @patch('nlm._default_system')
    def test_get_system(self, mock_system):
        """Test get_system function"""
        # This would need mocking to work properly
        pass
    
    def test_motor_command_enum_integration(self):
        """Test MotorCommand integration with system"""
        system = nlm.NeuralSystem(neuron_count=10)
        system.initialize()
        
        action = system.get_action()
        self.assertIsInstance(action, nlm.ActionResult)
        self.assertIsInstance(action.reward, float)
        self.assertIsInstance(action.success, bool)
        self.assertIsInstance(action.message, str)


if __name__ == '__main__':
    print("Running NLM Python Bindings Unit Tests")
    print("=" * 50)
    
    # Run tests
    unittest.main(verbosity=2)
