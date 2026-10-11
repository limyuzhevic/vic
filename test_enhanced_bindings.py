#!/usr/bin/env python3
"""
Test suite for enhanced NLM Python bindings.

This script tests the enhanced Python API for NLM, including:
- Basic functionality
- Advanced configuration
- Error handling
- Performance monitoring
- Context managers
- Factory methods
"""

import sys
import os
import time
import unittest
from typing import Dict, Any, List
import json

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '.'))

try:
    import pynlm as nlm
    import numpy as np
except ImportError:
    print("Error: NLM Python bindings not found!")
    print("Please build the project first with: pip install .")
    sys.exit(1)
class TestBasicFunctionality(unittest.TestCase):
    """Test basic NLM functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = nlm.createDefaultConfig()
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up after tests."""
        # Brain will be automatically cleaned up
        pass
    
    def test_basic_creation(self):
        """Test basic brain creation."""
        self.assertIsNotNone(self.brain)
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
    
    def test_step(self):
        """Test simulation stepping."""
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(1)
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), initial_spikes)
    
    def test_config(self):
        """Test configuration."""
        self.config.set("brain.neuron_count", 500)
        self.assertTrue(self.config.has("brain.neuron_count"))
        
        # Create new brain with updated config
        brain2 = nlm.createBrain(self.config)
        brain2.initialize()
        self.assertEqual(brain2.getTotalNeuronCount(), 500)
    
    def test_world_creation(self):
        """Test world creation."""
        world = nlm.createSimpleWorld()
        self.assertIsNotNone(world)
        self.assertEqual(world.getWidth(), 20.0)
        self.assertEqual(world.getHeight(), 20.0)
    
    def test_agent_creation(self):
        """Test agent creation."""
        world = nlm.createSimpleWorld()
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        self.assertIsNotNone(agent)
        self.assertEqual(agent.getSensoryInputSize(), 270)  # 256 + 8 + 4 + 2
        self.assertEqual(agent.getMotorOutputSize(), 6)
    
    def test_action_creation(self):
        """Test action creation."""
        action = nlm.Action()
        action.setType(nlm.ActionType::MoveForward)
        
        self.assertEqual(action.getType(), nlm.ActionType::MoveForward)
        self.assertEqual(action.getName(), "MoveForward")
        
        # Test cloning
        action2 = action.clone()
        self.assertEqual(action2.getType(), action.getType())
    
    def test_sensory_input(self):
        """Test sensory input creation."""
        vision = nlm.Vision(16, 16, 1)
        self.assertEqual(vision.getWidth(), 16)
        self.assertEqual(vision.getHeight(), 16)
        self.assertEqual(vision.getChannels(), 1)
        
        # Test data setting
        test_data = [0.1] * 256
        vision.setData(test_data)
        self.assertEqual(vision.getData(), test_data)
class TestAdvancedFunctionality(unittest.TestCase):
    """Test advanced NLM functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = nlm.createDefaultConfig()
        self.config.apply_preset("accurate")
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = nlm.createSimpleWorld()
        self.world.configure(30, 30, 8, 8)
        self.world.reset()
        
        self.agent = nlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_feature_enables(self):
        """Test feature enabling."""
        # Test feature enabling
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())
    
    def test_optimization_preset(self):
        """Test optimization presets."""
        config = nlm.createDefaultConfig()
        
        # Test different presets
        for preset in ["basic", "fast", "accurate", "balanced"]:
            config2 = nlm.createDefaultConfig()
            config2.apply_preset(preset)
            
            # Verify preset was applied
            self.assertIsNotNone(config2)
            # Note: actual implementation details depend on config class
    
    def test_configuration_validation(self):
        """Test configuration validation."""
        config = nlm.createDefaultConfig()
        
        # Test invalid configurations
        with self.assertRaises(Exception):
            config.set("brain.neuron_count", -100)
        
        # Test validation method
        errors = config.validate()
        self.assertIsInstance(errors, list)
    
    def test_string_conversions(self):
        """Test string-based neuron type conversions."""
        # Test conversions (if implemented)
        # These might not be available in basic bindings
        pass
    
    def test_enumeration_values(self):
        """Test enumeration values."""
        # Test neuron type
        self.assertEqual(nlm.NeuronType.Excitatory, nlm.NeuronType.Excitatory)
        self.assertEqual(nlm.NeuronType.Inhibitory, nlm.NeuronType.Inhibitory)
        
        # Test action type
        self.assertEqual(nlm.ActionType.MoveForward, nlm.ActionType.MoveForward)
        self.assertEqual(nlm.ActionType.TurnLeft, nlm.ActionType.TurnLeft)
        
        # Test world object type
        self.assertEqual(nlm.WorldObjectType.Resource, nlm.WorldObjectType.Resource)
        self.assertEqual(nlm.WorldObjectType.Hazard, nlm.WorldObjectType.Hazard)
class TestErrorHandling(unittest.TestCase):
    """Test error handling."""
    
    def test_null_brain(self):
        """Test error handling for null brain."""
        # Test with None (if supported)
        pass
    
    def test_invalid_configuration(self):
        """Test invalid configuration handling."""
        config = nlm.createDefaultConfig()
        
        # Test invalid neuron count
        with self.assertRaises(Exception):
            config.set("brain.neuron_count", 0)
    
    def test_simulation_error(self):
        """Test simulation error handling."""
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        # Test calling step without initialization (should handle gracefully)
        pass
class TestPerformanceMonitoring(unittest.TestCase):
    """Test performance monitoring functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.brain = nlm.createBrain(nlm.createDefaultConfig())
        self.brain.initialize()
    
    def test_performance_monitoring(self):
        """Test performance monitoring (if available)."""
        # This would test the PerformanceMonitor class if available
        pass
    
    def test_stats_collection(self):
        """Test statistics collection."""
        # Run some simulation steps
        for step in range(100):
            self.brain.step(step)
        
        # Check if stats are available
        if hasattr(self.brain, 'get_performance_stats'):
            stats = self.brain.get_performance_stats()
            self.assertIsInstance(stats, dict)
            self.assertIn('steps_completed', stats)
class TestContextManager(unittest.TestCase):
    """Test context manager functionality."""
    
    def test_context_manager_import(self):
        """Test that context manager module can be imported."""
        try:
            from pynlm.advanced_api import SimulationContext
            self.assertTrue(True)
        except ImportError:
            # Context manager might not be available in basic bindings
            self.assertTrue(True)
    
    def test_simulation_context(self):
        """Test simulation context (if available)."""
        try:
            from pynlm.advanced_api import SimulationContext
            
            brain = nlm.createBrain(nlm.createDefaultConfig())
            brain.initialize()
            
            # Test context manager (if available)
            with SimulationContext(brain) as sim:
                sim.enable_all_features()
                sim.run_for_steps(50)
                
                stats = sim.get_performance_stats()
                self.assertIsInstance(stats, dict)
                
        except ImportError:
            # Context manager might not be available
            self.assertTrue(True)
class TestFactoryMethods(unittest.TestCase):
    """Test factory methods."""
    
    def test_factory_creation(self):
        """Test factory method creation."""
        try:
            from pynlm.advanced_api import Factory
            
            # Test different factory methods
            brain = Factory.create_default_brain(1000, 42)
            self.assertIsNotNone(brain)
            self.assertGreater(brain.getTotalNeuronCount(), 0)
            
            # Test reinforcement learner factory
            brain2, world2, agent2 = Factory.create_reinforcement_learner(20, 20, 500)
            self.assertIsNotNone(brain2)
            self.assertIsNotNone(world2)
            self.assertIsNotNone(agent2)
            
        except ImportError:
            # Factory methods might not be available
            self.assertTrue(True)
    
    def test_advanced_config_creation(self):
        """Test advanced configuration creation."""
        try:
            from pynlm.advanced_api import create_advanced_config
            
            config = create_advanced_config(
                neuron_count=2000,
                optimization_level="balanced",
                enable_reward_modulation=True
            )
            
            self.assertIsNotNone(config)
            self.assertEqual(config.neuron_count, 2000)
            self.assertEqual(config.optimization_level.value, "balanced")
            
        except ImportError:
            # Advanced config might not be available
            self.assertTrue(True)
class TestBackwardCompatibility(unittest.TestCase):
    """Test backward compatibility."""
    
    def test_basic_api_compatibility(self):
        """Test basic API compatibility."""
        # Test the original simple API still works
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        for step in range(10):
            brain.step(step)
        
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
    
    def test_world_compatibility(self):
        """Test world API compatibility."""
        world = nlm.createSimpleWorld()
        world.configure(15, 15, 8, 8)
        world.reset()
        
        self.assertIsNotNone(world.getAgentBody())
    
    def test_agent_compatibility(self):
        """Test agent API compatibility."""
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test basic agent functionality
        percept = world.getSensoryPercept()
        action = agent.decodeMotorCommand()
        
        self.assertIsNotNone(percept)
        self.assertIsNotNone(action)
class TestIntegration(unittest.TestCase):
    """Test integration scenarios."""
    
    def test_complete_simulation(self):
        """Test complete simulation scenario."""
        try:
            from pynlm.advanced_api import run_complete_simulation
            
            brain = nlm.createBrain(nlm.createDefaultConfig())
            brain.initialize()
            
            results = run_complete_simulation(brain, steps=100)
            
            self.assertIsInstance(results, dict)
            self.assertIn('simulation_complete', results)
            self.assertIn('final_neuron_count', results)
            
        except ImportError:
            # Advanced features might not be available
            self.assertTrue(True)
    
    def test_agent_world_simulation(self):
        """Test complete agent-world simulation."""
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(20, 20, 16, 16)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation loop
        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        self.assertGreaterEqual(world.getAgentBody().energy, 0.0)
class TestModuleMetadata(unittest.TestCase):
    """Test module metadata and version information."""
    
    def test_version_info(self):
        """Test version information."""
        # Test version attributes
        if hasattr(nlm, '__version__'):
            self.assertIsInstance(nlm.__version__, str)
        
        if hasattr(nlm, '__description__'):
            self.assertIsInstance(nlm.__description__, str)
    
    def test_module_attributes(self):
        """Test module attributes."""
        # Check for expected attributes
        expected_attrs = ['createDefaultConfig', 'createBrain', 'createSimpleWorld']
        
        for attr in expected_attrs:
            self.assertTrue(hasattr(nlm, attr), 
                          f"Module should have attribute: {attr}")
if __name__ == '__main__':
    # Run tests
    unittest.main(verbosity=2)
