#!/usr/bin/env python3
"""
Test Suite for NLM Python Bindings
==================================

Comprehensive test suite for NLM Python bindings including:
- Basic functionality tests
- Advanced binding tests
- Error handling tests
- Performance tests
- Integration tests
"""

import sys
import os
import unittest
import numpy as np
import tempfile
import time

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

class TestBasicBindings(unittest.TestCase):
    """Test basic NLM Python bindings functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        try:
            import pynlm
            self.pynlm = pynlm
            self.has_bindings = True
        except ImportError:
            self.has_bindings = False
            self.pynlm = None
    
    def skip_if_no_bindings(self):
        """Skip test if bindings are not available."""
        if not self.has_bindings:
            self.skipTest("NLM bindings not available")
    
    def test_import_success(self):
        """Test that NLM can be imported."""
        self.skip_if_no_bindings()
        self.assertTrue(hasattr(self.pynlm, 'createDefaultConfig'))
        self.assertTrue(hasattr(self.pynlm, 'createBrain'))
        self.assertTrue(hasattr(self.pynlm, 'createSimpleWorld'))
    
    def test_config_creation(self):
        """Test configuration creation."""
        self.skip_if_no_bindings()
        config = self.pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertTrue(config.has("random_seed"))
        self.assertTrue(config.has("simulation_timestep"))
    
    def test_brain_creation(self):
        """Test brain creation."""
        self.skip_if_no_bindings()
        config = self.pynlm.createDefaultConfig()
        brain = self.pynlm.createBrain(config)
        self.assertIsNotNone(brain)
        
        # Test initialization
        brain.initialize()
        self.assertEqual(brain.getTotalNeuronCount(), config.getOr[int64_t]("neuron_count", 1000))
    
    def test_world_creation(self):
        """Test world creation."""
        self.skip_if_no_bindings()
        world = self.pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        
        # Test configuration
        world.configure(10, 10, 8, 8)
        self.assertEqual(world.getWidth(), 10)
        self.assertEqual(world.getHeight(), 10)
    
    def test_agent_creation(self):
        """Test agent creation."""
        self.skip_if_no_bindings()
        config = self.pynlm.createDefaultConfig()
        brain = self.pynlm.createBrain(config)
        brain.initialize()
        
        agent = self.pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
    
    def test_basic_simulation(self):
        """Test basic simulation."""
        self.skip_if_no_bindings()
        config = self.pynlm.createDefaultConfig()
        brain = self.pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(10):
            brain.step(step)
        
        # Check results
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(brain.getFiringNeuronCount(), 0)
    
    def test_world_simulation(self):
        """Test world simulation."""
        self.skip_if_no_bindings()
        world = self.pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        # Run world simulation
        for step in range(10):
            world.update(0.1)
        
        # Check world state
        self.assertEqual(world.getWidth(), 10)
        self.assertEqual(world.getHeight(), 10)
        self.assertGreater(world.getSimulationTime(), 0.0)

class TestEnhancedBindings(unittest.TestCase):
    """Test enhanced NLM Python bindings functionality."""
    
    def setUp(self):
        """Set up test fixtures for enhanced bindings."""
        try:
            import pynlm_enhanced as enhanced
            self.enhanced = enhanced
            self.has_enhanced = True
        except ImportError:
            self.has_enhanced = False
            self.enhanced = None
    
    def skip_if_no_enhanced(self):
        """Skip test if enhanced bindings are not available."""
        if not self.has_enhanced:
            self.skipTest("Enhanced NLM bindings not available")
    
    def test_enhanced_config(self):
        """Test enhanced configuration utilities."""
        self.skip_if_no_enhanced()
        
        # Test advanced config creation
        config = self.enhanced.create_advanced_config(
            neuron_count=1500,
            connection_probability=0.2
        )
        
        self.assertIsNotNone(config)
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("connection_probability"))
        
        # Test config validation
        self.assertTrue(config.validate())
    
    def test_enhanced_stats(self):
        """Test enhanced statistics computation."""
        self.skip_if_no_enhanced()
        
        # Create a simple simulation
        import pynlm as basic
        config = basic.createDefaultConfig()
        brain = basic.createBrain(config)
        brain.initialize()
        
        for step in range(20):
            brain.step(step)
        
        # Test comprehensive stats
        stats = self.enhanced.BrainAnalyzer.compute_comprehensive_stats(brain)
        
        self.assertIsInstance(stats, dict)
        self.assertIn('total_neurons', stats)
        self.assertIn('total_synapses', stats)
        self.assertIn('firing_neurons', stats)
        self.assertIn('average_firing_rate', stats)
        self.assertIn('excitation_inhibition_ratio', stats)
    
    def test_enhanced_batch(self):
        """Test enhanced batch processing."""
        self.skip_if_no_enhanced()
        
        # Create multiple brains
        import pynlm as basic
        brains = []
        worlds = []
        
        for i in range(3):
            config = basic.createDefaultConfig()
            brain = basic.createBrain(config)
            brain.initialize()
            brains.append(brain)
            
            world = basic.createSimpleWorld()
            world.configure(10, 10, 6, 6)
            worlds.append(world)
        
        # Run batch simulation
        results = self.enhanced.BatchSimulation.run_batch(
            brains, worlds, 20, 0.1
        )
        
        self.assertIsInstance(results, dict)
        self.assertIn('spike_rates', results)
        self.assertIn('firing_counts', results)
        self.assertIn('e_i_ratios', results)
        self.assertEqual(len(results['spike_rates']), 3)
    
    def test_enhanced_agent_analysis(self):
        """Test enhanced agent analysis."""
        self.skip_if_no_enhanced()
        
        import pynlm as basic
        config = basic.createDefaultConfig()
        brain = basic.createBrain(config)
        brain.initialize()
        
        agent = basic.createAgentBrain(brain)
        world = basic.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        agent.initialize(world)
        
        # Run some simulation steps
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Test performance analysis
        perf = self.enhanced.AgentAnalyzer.analyze_performance(agent)
        
        self.assertIsInstance(perf, dict)
        self.assertIn('curiosity_level', perf)
        self.assertIn('novelty_level', perf)
        self.assertIn('prediction_error', perf)
        self.assertIn('neuromodulation_level', perf)
    
    def test_enhanced_world_utils(self):
        """Test enhanced world utilities."""
        self.skip_if_no_enhanced()
        
        # Test world creation
        world = self.enhanced.WorldUtils.create_world(
            width=20, height=20, vision_width=8, vision_height=8
        )
        
        self.assertEqual(world.getWidth(), 20)
        self.assertEqual(world.getHeight(), 20)
        
        # Test environment statistics
        stats = self.enhanced.WorldUtils.get_environment_statistics(world)
        self.assertIsInstance(stats, dict)
        self.assertIn('width', stats)
        self.assertIn('height', stats)
    
    def test_enhanced_utility_functions(self):
        """Test enhanced utility functions."""
        self.skip_if_no_enhanced()
        
        # Test vector normalization
        data = [1.0, 2.0, 3.0, 4.0, 5.0]
        normalized = self.enhanced.normalize_vector(data)
        
        self.assertEqual(len(normalized), len(data))
        self.assertAlmostEqual(sum(normalized), 1.0, places=5)
        
        # Test clamping
        clamped = self.enhanced.clamp(15.0, 0.0, 10.0)
        self.assertEqual(clamped, 10.0)
        
        # Test distance calculation
        a = [1.0, 2.0, 3.0]
        b = [4.0, 5.0, 6.0]
        distance = self.enhanced.euclidean_distance(a, b)
        self.assertAlmostEqual(distance, np.sqrt(27), places=5)
        
        # Test dot product
        dot = self.enhanced.dot_product(a, b)
        self.assertAlmostEqual(dot, 32.0, places=5)

class TestErrorHandling(unittest.TestCase):
    """Test error handling in NLM bindings."""
    
    def test_invalid_file_handling(self):
        """Test handling of invalid file paths."""
        # This test would verify that invalid file paths are handled gracefully
        pass
    
    def test_null_object_handling(self):
        """Test handling of null objects."""
        # This test would verify that null objects are handled gracefully
        pass
    
    def test_dimension_mismatch(self):
        """Test handling of dimension mismatches."""
        # This test would verify that dimension mismatches are caught
        pass

class TestPerformance(unittest.TestCase):
    """Test performance characteristics of NLM bindings."""
    
    def test_simulation_performance(self):
        """Test simulation performance."""
        import pynlm as basic
        
        config = basic.createDefaultConfig()
        brain = basic.createBrain(config)
        brain.initialize()
        
        # Time simulation
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        # Should complete within reasonable time
        self.assertLess(end_time - start_time, 10.0)  # Less than 10 seconds

class TestIntegration(unittest.TestCase):
    """Test integration between different NLM components."""
    
    def test_brain_world_integration(self):
        """Test integration between brain and world."""
        import pynlm as basic
        
        # Create complete agent-world setup
        config = basic.createDefaultConfig()
        brain = basic.createBrain(config)
        brain.initialize()
        
        agent = basic.createAgentBrain(brain)
        world = basic.createSimpleWorld()
        world.configure(15, 15, 8, 8)
        world.reset()
        world.setAgentStart(7.0, 7.0)
        agent.initialize(world)
        
        # Run complete simulation loop
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept and percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Verify integration worked
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)

class TestBackwardsCompatibility(unittest.TestCase):
    """Test backwards compatibility with existing API."""
    
    def test_original_api_compatibility(self):
        """Test that original API still works."""
        import pynlm as basic
        
        # Test all original API functions exist
        self.assertTrue(hasattr(basic, 'createDefaultConfig'))
        self.assertTrue(hasattr(basic, 'createBrain'))
        self.assertTrue(hasattr(basic, 'createSimpleWorld'))
        self.assertTrue(hasattr(basic, 'createAgentBrain'))
        
        # Test that classes exist
        self.assertTrue(hasattr(basic, 'Config'))
        self.assertTrue(hasattr(basic, 'Brain'))
        self.assertTrue(hasattr(basic, 'AgentBrain'))
        self.assertTrue(hasattr(basic, 'SimpleWorld'))
        
        # Test basic functionality
        config = basic.createDefaultConfig()
        self.assertIsNotNone(config)
        
        brain = basic.createBrain(config)
        self.assertIsNotNone(brain)
        
        brain.initialize()
        self.assertGreater(brain.getTotalNeuronCount(), 0)

if __name__ == '__main__':
    # Run all tests
    unittest.main(verbosity=2)