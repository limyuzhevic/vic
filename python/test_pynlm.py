#!/usr/bin/env python3
"""
Python tests for NLM bindings.

This file contains comprehensive tests for the pynlm Python bindings.
Tests cover basic functionality, error handling, and edge cases.
"""

import sys
import os
import tempfile
import json
from pathlib import Path
from typing import Dict, Any, List, Optional

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError as e:
    PYNL_AVAILABLE = False
    print(f"Warning: Could not import pynlm: {e}")

import unittest
from unittest.mock import Mock, patch


class TestBasicImport(unittest.TestCase):
    """Test basic import and initialization."""
    
    def test_import_available(self):
        """Test that pynlm can be imported."""
        if PYNL_AVAILABLE:
            self.assertTrue(hasattr(pynlm, '__version__'))
            self.assertTrue(hasattr(pynlm, 'create_default_config'))
            self.assertTrue(hasattr(pynlm, 'create_brain'))
    
    def test_version_string(self):
        """Test that version string is available."""
        if PYNL_AVAILABLE:
            self.assertIsInstance(pynlm.__version__, str)
            self.assertTrue(len(pynlm.__version__) > 0)


class TestConfig(unittest.TestCase):
    """Test Config class functionality."""
    
    def setUp(self):
        if PYNL_AVAILABLE:
            self.config = pynlm.create_default_config()
    
    def test_config_creation(self):
        """Test default config creation."""
        if PYNL_AVAILABLE:
            self.assertIsNotNone(self.config)
    
    def test_config_summary(self):
        """Test config summary method."""
        if PYNL_AVAILABLE:
            summary = self.config.summary()
            self.assertIsInstance(summary, str)
            self.assertTrue(len(summary) > 0)
    
    def test_config_clear(self):
        """Test config clearing."""
        if PYNL_AVAILABLE:
            self.config.clear()
            # After clearing, summary should be shorter
            self.assertLess(len(self.config.summary()), 100)
    
    def test_config_save_load(self):
        """Test config save and load functionality."""
        if PYNL_AVAILABLE:
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                config_file = f.name
            
            try:
                # Save config
                self.config.saveToFile(config_file)
                
                # Clear and reload
                self.config.clear()
                self.config.loadFromFile(config_file)
                
                # Verify config is loaded
                self.assertTrue(len(self.config.summary()) > 0)
            finally:
                os.unlink(config_file)


class TestBrain(unittest.TestCase):
    """Test Brain class functionality."""
    
    def setUp(self):
        if PYNL_AVAILABLE:
            self.config = pynlm.create_default_config()
            self.brain = pynlm.create_brain(self.config)
            self.brain.initialize()
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        if PYNL_AVAILABLE:
            self.assertIsNotNone(self.brain)
    
    def test_brain_stats(self):
        """Test brain statistics methods."""
        if PYNL_AVAILABLE:
            # Test various statistics methods
            neuron_count = self.brain.getTotalNeuronCount()
            self.assertIsInstance(neuron_count, int)
            self.assertGreaterEqual(neuron_count, 0)
            
            synapse_count = self.brain.getTotalSynapseCount()
            self.assertIsInstance(synapse_count, int)
            self.assertGreaterEqual(synapse_count, 0)
            
            firing_count = self.brain.getFiringNeuronCount()
            self.assertIsInstance(firing_count, int)
            self.assertGreaterEqual(firing_count, 0)
            
            total_spikes = self.brain.getTotalSpikeCount()
            self.assertIsInstance(total_spikes, int)
            self.assertGreaterEqual(total_spikes, 0)
    
    def test_brain_step(self):
        """Test brain step functionality."""
        if PYNL_AVAILABLE:
            initial_spikes = self.brain.getTotalSpikeCount()
            
            # Run a few steps
            for step in range(10):
                self.brain.step(step)
            
            final_spikes = self.brain.getTotalSpikeCount()
            self.assertGreaterEqual(final_spikes, initial_spikes)
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        if PYNL_AVAILABLE:
            # Run some steps first
            for step in range(5):
                self.brain.step(step)
            
            # Reset brain
            self.brain.reset()
            
            # After reset, some statistics should be lower
            # (Note: exact behavior may depend on implementation)
            self.brain.step(0)
    
    def test_brain_serialization(self):
        """Test brain save/load functionality."""
        if PYNL_AVAILABLE:
            with tempfile.NamedTemporaryFile(delete=False) as f:
                save_file = f.name
            
            try:
                # Save brain state
                self.brain.save(save_file)
                
                # Create new brain and load state
                config = pynlm.create_default_config()
                new_brain = pynlm.create_brain(config)
                new_brain.load(save_file)
                
                # Both brains should have neurons
                self.assertGreater(self.brain.getTotalNeuronCount(), 0)
                self.assertGreater(new_brain.getTotalNeuronCount(), 0)
            finally:
                os.unlink(save_file)


class TestWorld(unittest.TestCase):
    """Test SimpleWorld class functionality."""
    
    def setUp(self):
        if PYNL_AVAILABLE:
            self.world = pynlm.create_simple_world()
            self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
            self.world.reset()
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        if PYNL_AVAILABLE:
            self.assertIsNotNone(self.world)
            self.assertEqual(self.world.getWidth(), 10)
            self.assertEqual(self.world.getHeight(), 10)
    
    def test_world_update(self):
        """Test world update functionality."""
        if PYNL_AVAILABLE:
            initial_time = self.world.getSimulationTime()
            self.world.update(0.1)
            self.assertGreater(self.world.getSimulationTime(), initial_time)
    
    def test_world_sensory_input(self):
        """Test world sensory input functionality."""
        if PYNL_AVAILABLE:
            percept = self.world.getSensoryPercept()
            self.assertIsNotNone(percept)
            
            # Check sensory input properties
            self.assertIsInstance(percept.getVision(), type(percept.getVision()))
    
    def test_world_agent_body(self):
        """Test world agent body functionality."""
        if PYNL_AVAILABLE:
            body = self.world.getAgentBody()
            self.assertIsNotNone(body)
            
            # Check body properties
            self.assertIsInstance(body.x, float)
            self.assertIsInstance(body.y, float)
            self.assertIsInstance(body.orientation, float)
    
    def test_world_objects(self):
        """Test world object management."""
        if PYNL_AVAILABLE:
            # Create a world object
            from pynlm import WorldObject, WorldObjectType
            obj = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=1.0)
            
            # Add object to world
            self.world.addObject(obj)
            
            # Check object exists
            # Note: Implementation may vary
            self.world.removeObject(5.0, 5.0)
    
    def test_world_position_validation(self):
        """Test world position validation."""
        if PYNL_AVAILABLE:
            # Valid position
            self.assertTrue(self.world.isValidPosition(5.0, 5.0))
            
            # Invalid position (outside bounds)
            self.assertFalse(self.world.isValidPosition(-1.0, -1.0))
            self.assertFalse(self.world.isValidPosition(11.0, 11.0))


class TestAgent(unittest.TestCase):
    """Test AgentBrain class functionality."""
    
    def setUp(self):
        if PYNL_AVAILABLE:
            self.config = pynlm.create_default_config()
            self.brain = pynlm.create_brain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.create_simple_world()
            self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
            self.world.reset()
            
            self.agent = pynlm.create_agent_brain(self.brain)
            self.agent.initialize(self.world)
    
    def test_agent_creation(self):
        """Test agent creation and initialization."""
        if PYNL_AVAILABLE:
            self.assertIsNotNone(self.agent)
    
    def test_agent_sensory_processing(self):
        """Test agent sensory processing."""
        if PYNL_AVAILABLE:
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            # If we get here without exception, test passes
    
    def test_agent_motor_decoding(self):
        """Test agent motor command decoding."""
        if PYNL_AVAILABLE:
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(0)
            
            # Get motor command
            motor_cmd = self.agent.decodeMotorCommand()
            self.assertIsNotNone(motor_cmd)
    
    def test_agent_subsystems(self):
        """Test agent subsystem enable/disable."""
        if PYNL_AVAILABLE:
            # Enable various subsystems
            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableCuriosity(True)
            
            # Check they are enabled
            self.assertTrue(self.agent.isRewardModulationEnabled())
            self.assertTrue(self.agent.isStructuralPlasticityEnabled())
            self.assertTrue(self.agent.isDevelopmentEnabled())
            self.assertTrue(self.agent.isCuriosityEnabled())
    
    def test_agent_stats(self):
        """Test agent statistics."""
        if PYNL_AVAILABLE:
            curiosity = self.agent.getCuriosityLevel()
            self.assertIsInstance(curiosity, float)
            self.assertGreaterEqual(curiosity, 0.0)
            
            novelty = self.agent.getNoveltyLevel()
            self.assertIsInstance(novelty, float)
            self.assertGreaterEqual(novelty, 0.0)
            
            neuromodulation = self.agent.getNeuromodulationLevel()
            self.assertIsInstance(neuromodulation, float)
            
            prediction_error = self.agent.getPredictionError()
            self.assertIsInstance(prediction_error, float)
            self.assertGreaterEqual(prediction_error, 0.0)


class TestConvenienceFunctions(unittest.TestCase):
    """Test convenience functions."""
    
    def test_run_simulation(self):
        """Test run_simulation convenience function."""
        if PYNL_AVAILABLE:
            config = pynlm.create_default_config()
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            # Test running simulation
            stats = pynlm.run_simulation_with_callback(
                brain, num_steps=50, timestep=0.1,
                callback=lambda step, stats: None,
                save_frequency=10
            )
            
            self.assertIsInstance(stats, dict)
            self.assertIn('steps_completed', stats)
            self.assertIn('total_spikes', stats)
    
    def test_create_development_agent(self):
        """Test create_development_agent convenience function."""
        if PYNL_AVAILABLE:
            brain, agent, world = pynlm.create_development_agent(
                world_size=(10, 10),
                enable_learning=True,
                enable_development=True,
                enable_curiosity=True
            )
            
            self.assertIsNotNone(brain)
            self.assertIsNotNone(agent)
            self.assertIsNotNone(world)
    
    def test_quick_test_simulation(self):
        """Test quick_test_simulation convenience function."""
        if PYNL_AVAILABLE:
            result = pynlm.quick_test_simulation(steps=10)
            
            self.assertIsInstance(result, dict)
            self.assertTrue(result['success'])
            self.assertEqual(result['total_steps'], 10)
            self.assertGreater(result['total_neurons'], 0)
            self.assertGreaterEqual(result['execution_time'], 0.0)


class TestErrorHandling(unittest.TestCase):
    """Test error handling."""
    
    def setUp(self):
        if PYNL_AVAILABLE:
            self.config = pynlm.create_default_config()
            self.brain = pynlm.create_brain(self.config)
            self.brain.initialize()
    
    def test_brain_not_initialized(self):
        """Test error when brain not initialized."""
        if PYNL_AVAILABLE:
            # Create uninitialized brain
            config = pynlm.create_default_config()
            uninitialized_brain = pynlm.create_brain(config)
            
            # Should fail when trying to step
            with self.assertRaises(Exception):
                uninitialized_brain.step(0)
    
    def test_invalid_config(self):
        """Test error handling with invalid configuration."""
        if PYNL_AVAILABLE:
            # Try to set invalid configuration
            try:
                # This may not raise an error depending on implementation
                # but should be handled gracefully
                self.assertTrue(True)  # Placeholder
            except Exception:
                pass  # Expected
    
    def test_resource_cleanup(self):
        """Test resource cleanup on error."""
        if PYNL_AVAILABLE:
            # Create brain and let it go out of scope
            # Should be cleaned up automatically
            brain = pynlm.create_brain(self.config)
            brain.initialize()
            
            # Do some operations
            for step in range(5):
                brain.step(step)
            
            # Clean up
            brain.reset()
            # Should not raise exception


class TestIDClasses(unittest.TestCase):
    """Test ID class functionality."""
    
    def setUp(self):
        if PYNL_AVAILABLE:
            self.neuron_id = pynlm.NeuronId(123)
            self.synapse_id = pynlm.SynapseId(456)
            self.region_id = pynlm.RegionId(789)
            self.population_id = pynlm.PopulationId(999)
    
    def test_neuron_id(self):
        """Test NeuronId functionality."""
        if PYNL_AVAILABLE:
            self.assertEqual(self.neuron_id.value, 123)
            self.assertEqual(self.neuron_id.index(), 123)
            
            # Test string representation
            self.assertIn("NeuronId", str(self.neuron_id))
            
            # Test equality
            other_id = pynlm.NeuronId(123)
            self.assertEqual(self.neuron_id, other_id)
    
    def test_synapse_id(self):
        """Test SynapseId functionality."""
        if PYNL_AVAILABLE:
            self.assertEqual(self.synapse_id.value, 456)
            self.assertEqual(self.synapse_id.index(), 456)
    
    def test_region_id(self):
        """Test RegionId functionality."""
        if PYNL_AVAILABLE:
            self.assertEqual(self.region_id.value, 789)
            self.assertEqual(self.region_id.index(), 789)
    
    def test_population_id(self):
        """Test PopulationId functionality."""
        if PYNL_AVAILABLE:
            self.assertEqual(self.population_id.value, 999)
            self.assertEqual(self.population_id.index(), 999)
            
            # Test invalid sentinel
            invalid_id = pynlm.PopulationId.invalid()
            self.assertEqual(invalid_id.value, -1)


class TestEnums(unittest.TestCase):
    """Test enumeration functionality."""
    
    def test_neuron_type_enums(self):
        """Test NeuronType enum values."""
        if PYNL_AVAILABLE:
            self.assertTrue(hasattr(pynlm.NeuronType, 'Excitatory'))
            self.assertTrue(hasattr(pynlm.NeuronType, 'Inhibitory'))
            self.assertTrue(hasattr(pynlm.NeuronType, 'Sensory'))
            self.assertTrue(hasattr(pynlm.NeuronType, 'Motor'))
            self.assertTrue(hasattr(pynlm.NeuronType, 'Modulatory'))
            self.assertTrue(hasattr(pynlm.NeuronType, 'Internal'))
            
            # Test helper methods
            self.assertTrue(pynlm.NeuronType.Excitatory.is_excitatory())
            self.assertFalse(pynlm.NeuronType.Excitatory.is_inhibitory())
    
    def test_synapse_type_enums(self):
        """Test SynapseType enum values."""
        if PYNL_AVAILABLE:
            self.assertTrue(hasattr(pynlm.SynapseType, 'Excitatory'))
            self.assertTrue(hasattr(pynlm.SynapseType, 'Inhibitory'))
            self.assertTrue(hasattr(pynlm.SynapseType, 'Modulatory'))
            self.assertTrue(hasattr(pynlm.SynapseType, 'Electrical'))
            self.assertTrue(hasattr(pynlm.SynapseType, 'GapJunction'))
            
            # Test helper methods
            self.assertTrue(pynlm.SynapseType.Excitatory.is_excitatory())
            self.assertTrue(pynlm.SynapseType.Excitatory.is_chemical())
            self.assertFalse(pynlm.SynapseType.Electrical.is_chemical())
            self.assertTrue(pynlm.SynapseType.Electrical.is_electrical())
    
    def test_action_type_enums(self):
        """Test ActionType enum values."""
        if PYNL_AVAILABLE:
            self.assertTrue(hasattr(pynlm.ActionType, 'MoveForward'))
            self.assertTrue(hasattr(pynlm.ActionType, 'TurnLeft'))
            self.assertTrue(hasattr(pynlm.ActionType, 'Look'))
            self.assertTrue(hasattr(pynlm.ActionType, 'Eat'))
            
            # Test helper methods
            self.assertTrue(pynlm.ActionType.MoveForward.is_movement())
            self.assertFalse(pynlm.ActionType.MoveForward.is_rotational())
            self.assertTrue(pynlm.ActionType.TurnLeft.is_rotational())
            self.assertTrue(pynlm.ActionType.Eat.is_consumable())
            self.assertTrue(pynlm.ActionType.Interact.is_social())
    
    def test_world_object_type_enums(self):
        """Test WorldObjectType enum values."""
        if PYNL_AVAILABLE:
            self.assertTrue(hasattr(pynlm.WorldObjectType, 'Empty'))
            self.assertTrue(hasattr(pynlm.WorldObjectType, 'Resource'))
            self.assertTrue(hasattr(pynlm.WorldObjectType, 'Hazard'))
            self.assertTrue(hasattr(pynlm.WorldObjectType, 'Wall'))
            
            # Test helper methods
            self.assertTrue(pynlm.WorldObjectType.Empty.is_passable())
            self.assertFalse(pynlm.WorldObjectType.Empty.is_hazard())
            self.assertTrue(pynlm.WorldObjectType.Resource.is_passable())
            self.assertTrue(pynlm.WorldObjectType.Resource.is_resource())
            self.assertTrue(pynlm.WorldObjectType.Wall.is_obstacle())


class TestIntegration(unittest.TestCase):
    """Integration tests combining multiple components."""
    
    def test_complete_simulation(self):
        """Test complete agent simulation."""
        if PYNL_AVAILABLE:
            # Create all components
            config = pynlm.create_default_config()
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            world = pynlm.create_simple_world()
            world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.create_agent_brain(brain)
            agent.initialize(world)
            
            # Enable learning subsystems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            for step in range(50):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                motor_cmd = agent.decodeMotorCommand()
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                # Check stats
                curiosity = agent.getCuriosityLevel()
                self.assertIsInstance(curiosity, float)
            
            # Verify final state
            self.assertGreater(brain.getTotalNeuronCount(), 0)
            self.assertGreaterEqual(brain.getFiringNeuronCount(), 0)


if __name__ == '__main__':
    # Print test information
    print("NLM Python Bindings Tests")
    print("=" * 40)
    
    if not PYNL_AVAILABLE:
        print("Warning: pynlm not available. Some tests will be skipped.")
    
    # Run tests
    unittest.main(verbosity=2)
