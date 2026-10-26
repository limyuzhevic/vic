#!/usr/bin/env python3
"""
Test Suite for Enhanced NLM Python Bindings
===========================================

Comprehensive test suite specifically for enhanced NLM bindings including:
- Advanced API tests
- Enhanced utility function tests
- Batch processing tests
- Advanced configuration tests
- Error handling tests for enhanced bindings
"""

import sys
import os
import unittest
import numpy as np
import tempfile
import json

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

class TestEnhancedAPIFunctions(unittest.TestCase):
    """Test enhanced API functions."""
    
    def setUp(self):
        """Set up test fixtures."""
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
    
    def test_create_advanced_config(self):
        """Test advanced configuration creation."""
        self.skip_if_no_enhanced()
        
        config = self.enhanced.create_advanced_config(
            neuron_count=2000,
            region_count=3,
            connection_probability=0.25,
            plasticity_rate=0.015,
            development_rate=0.002
        )
        
        self.assertIsNotNone(config)
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("region_count"))
        self.assertTrue(config.has("connection_probability"))
        
        # Check values
        self.assertEqual(config.getOr[int64_t]("neuron_count", 0), 2000)
        self.assertEqual(config.getOr[int64_t]("region_count", 0), 3)
        self.assertEqual(config.getOr[float]("connection_probability", 0), 0.25)
    
    def test_config_validation(self):
        """Test configuration validation."""
        self.skip_if_no_enhanced()
        
        config = self.enhanced.create_advanced_config()
        self.assertTrue(config.validate())
        
        # Test with empty config
        empty_config = self.enhanced.ConfigWrapper()
        self.assertFalse(empty_config.validate())
    
    def test_get_config_summary(self):
        """Test configuration summary."""
        self.skip_if_no_enhanced()
        
        config = self.enhanced.create_advanced_config()
        summary = self.enhanced.getConfigSummary(config)
        
        self.assertIsInstance(summary, dict)
        self.assertTrue(len(summary) > 0)
        
        # Check that keys are strings and values are strings
        for key, value in summary.items():
            self.assertIsInstance(key, str)
            self.assertIsInstance(value, str)
    
    def test_vector_normalization(self):
        """Test vector normalization utility."""
        self.skip_if_no_enhanced()
        
        # Test positive values
        data = [1.0, 2.0, 3.0, 4.0, 5.0]
        normalized = self.enhanced.normalize_vector(data)
        
        self.assertEqual(len(normalized), len(data))
        self.assertAlmostEqual(sum(normalized), 1.0, places=5)
        self.assertTrue(all(0 <= x <= 1 for x in normalized))
        
        # Test negative values
        data2 = [-2.0, -1.0, 0.0, 1.0, 2.0]
        normalized2 = self.enhanced.normalize_vector(data2)
        self.assertEqual(len(normalized2), len(data2))
        self.assertAlmostEqual(sum(normalized2), 1.0, places=5)
    
    def test_clamp_function(self):
        """Test clamp utility function."""
        self.skip_if_no_enhanced()
        
        # Test normal clamping
        result = self.enhanced.clamp(5.0, 0.0, 10.0)
        self.assertEqual(result, 5.0)
        
        result = self.enhanced.clamp(-5.0, 0.0, 10.0)
        self.assertEqual(result, 0.0)
        
        result = self.enhanced.clamp(15.0, 0.0, 10.0)
        self.assertEqual(result, 10.0)
        
        # Test edge cases
        result = self.enhanced.clamp(0.0, 0.0, 10.0)
        self.assertEqual(result, 0.0)
        
        result = self.enhanced.clamp(10.0, 0.0, 10.0)
        self.assertEqual(result, 10.0)
    
    def test_euclidean_distance(self):
        """Test Euclidean distance calculation."""
        self.skip_if_no_enhanced()
        
        a = [1.0, 2.0, 3.0]
        b = [4.0, 5.0, 6.0]
        
        distance = self.enhanced.euclidean_distance(a, b)
        expected = np.sqrt((4-1)**2 + (5-2)**2 + (6-3)**2)  # sqrt(27)
        self.assertAlmostEqual(distance, expected, places=10)
        
        # Test dimension mismatch error
        with self.assertRaises(Exception):
            self.enhanced.euclidean_distance([1.0, 2.0], [1.0, 2.0, 3.0])
    
    def test_dot_product(self):
        """Test dot product calculation."""
        self.skip_if_no_enhanced()
        
        a = [1.0, 2.0, 3.0]
        b = [4.0, 5.0, 6.0]
        
        dot = self.enhanced.dot_product(a, b)
        expected = 1*4 + 2*5 + 3*6  # 32
        self.assertAlmostEqual(dot, expected, places=10)
        
        # Test dimension mismatch error
        with self.assertRaises(Exception):
            self.enhanced.dot_product([1.0, 2.0], [1.0, 2.0, 3.0])
    
    def test_brain_stats_computation(self):
        """Test comprehensive brain statistics."""
        self.skip_if_no_enhanced()
        
        # Create a basic brain for testing
        import pynlm as basic
        config = basic.createDefaultConfig()
        brain = basic.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(20):
            brain.step(step)
        
        # Compute stats
        stats = self.enhanced.BrainAnalyzer.compute_comprehensive_stats(brain)
        
        self.assertIsInstance(stats, dict)
        self.assertIn('total_neurons', stats)
        self.assertIn('total_synapses', stats)
        self.assertIn('firing_neurons', stats)
        self.assertIn('average_firing_rate', stats)
        self.assertIn('excitation_inhibition_ratio', stats)
        self.assertIn('total_spikes', stats)
        
        # Check that values are reasonable
        self.assertGreaterEqual(stats['total_neurons'], 0)
        self.assertGreaterEqual(stats['total_synapses'], 0)
        self.assertGreaterEqual(stats['firing_neurons'], 0)
        self.assertGreaterEqual(stats['average_firing_rate'], 0.0)
        self.assertGreaterEqual(stats['total_spikes'], 0)
    
    def test_batch_simulation(self):
        """Test batch simulation functionality."""
        self.skip_if_no_enhanced()
        
        # Create multiple brains and worlds
        import pynlm as basic
        brains = []
        worlds = []
        
        for i in range(3):
            config = basic.createDefaultConfig()
            config.set("neuron_count", 500 + i * 200)
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
        self.assertIn('total_time', results)
        self.assertIn('total_steps', results)
        
        # Check results length
        self.assertEqual(len(results['spike_rates']), 3)
        self.assertEqual(len(results['firing_counts']), 3)
        self.assertEqual(len(results['e_i_ratios']), 3)
    
    def test_agent_performance_analysis(self):
        """Test agent performance analysis."""
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
        
        # Analyze performance
        perf = self.enhanced.AgentAnalyzer.analyze_performance(agent)
        
        self.assertIsInstance(perf, dict)
        self.assertIn('curiosity_level', perf)
        self.assertIn('novelty_level', perf)
        self.assertIn('prediction_error', perf)
        self.assertIn('neuromodulation_level', perf)
        self.assertIn('exploration_rate', perf)
        self.assertIn('learning_efficiency', perf)
        self.assertIn('action_diversity', perf)
        self.assertIn('adaptation_rate', perf)
        
        # Check reasonable ranges
        self.assertGreaterEqual(perf['curiosity_level'], 0.0)
        self.assertLessEqual(perf['curiosity_level'], 1.0)
        self.assertGreaterEqual(perf['novelty_level'], 0.0)
        self.assertLessEqual(perf['novelty_level'], 1.0)
    
    def test_recommended_actions(self):
        """Test recommended actions generation."""
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
        
        # Test with different agent states
        recommendations = self.enhanced.AgentAnalyzer.get_recommended_actions(
            agent, world, max_suggestions=5
        )
        
        self.assertIsInstance(recommendations, list)
        self.assertLessEqual(len(recommendations), 5)
        
        for action in recommendations:
            self.assertIsInstance(action, str)
            self.assertTrue(len(action) > 0)
    
    def test_world_utilities(self):
        """Test world utility functions."""
        self.skip_if_no_enhanced()
        
        # Test world creation
        world = self.enhanced.WorldUtils.create_world(
            width=20, height=20, vision_width=8, vision_height=8
        )
        
        self.assertEqual(world.getWidth(), 20)
        self.assertEqual(world.getHeight(), 20)
        self.assertEqual(world.getVisionWidth(), 8)
        self.assertEqual(world.getVisionHeight(), 8)
        
        # Test environment statistics
        stats = self.enhanced.WorldUtils.get_environment_statistics(world)
        self.assertIsInstance(stats, dict)
        self.assertIn('width', stats)
        self.assertIn('height', stats)
        self.assertIn('simulation_time', stats)
        self.assertIn('max_energy', stats)
    
    def test_brain_with_pattern(self):
        """Test brain creation with initial pattern."""
        self.skip_if_no_enhanced()
        
        import pynlm as basic
        config = basic.createDefaultConfig()
        
        # Create pattern
        pattern = [0.1, 0.5, 0.8, 0.3, 0.9, 0.2, 0.7, 0.4, 0.6, 0.1]
        
        # Create brain with pattern
        brain = self.enhanced.BrainUtils.create_brain_with_pattern(
            config, pattern, scale=1.0
        )
        
        self.assertIsNotNone(brain)
        brain.initialize()
        
        # Verify pattern was applied (rough check)
        self.assertGreater(brain.getTotalNeuronCount(), 0)
    
    def test_neuron_activity_pattern(self):
        """Test neuron activity pattern extraction."""
        self.skip_if_no_enhanced()
        
        import pynlm as basic
        config = basic.createDefaultConfig()
        brain = basic.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(20):
            brain.step(step)
        
        # Get activity pattern
        pattern = self.enhanced.BrainUtils.get_neuron_activity_pattern(
            brain, basic.NeuronType.Excitatory, max_neurons=50
        )
        
        self.assertIsInstance(pattern, list)
        self.assertLessEqual(len(pattern), 50)
        self.assertTrue(all(-1.0 <= x <= 1.0 for x in pattern))

class TestEnhancedErrorHandling(unittest.TestCase):
    """Test error handling in enhanced bindings."""
    
    def setUp(self):
        """Set up test fixtures."""
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
    
    def test_vector_normalization_error_handling(self):
        """Test error handling in vector normalization."""
        self.skip_if_no_enhanced()
        
        # Test with empty vector
        result = self.enhanced.normalize_vector([])
        self.assertEqual(result, [])
        
        # Test with zero-sum vector
        result = self.enhanced.normalize_vector([0.0, 0.0, 0.0])
        self.assertEqual(len(result), 3)  # Should return original
    
    def test_clamp_edge_cases(self):
        """Test edge cases in clamp function."""
        self.skip_if_no_enhanced()
        
        # Test with equal min and max
        result = self.enhanced.clamp(5.0, 10.0, 10.0)
        self.assertEqual(result, 10.0)
        
        # Test with value equal to min
        result = self.enhanced.clamp(0.0, 0.0, 10.0)
        self.assertEqual(result, 0.0)
        
        # Test with value equal to max
        result = self.enhanced.clamp(10.0, 0.0, 10.0)
        self.assertEqual(result, 10.0)
    
    def test_distance_dimension_error(self):
        """Test dimension mismatch error in distance calculation."""
        self.skip_if_no_enhanced()
        
        # This should raise an error
        with self.assertRaises(Exception):
            self.enhanced.euclidean_distance([1.0, 2.0], [1.0, 2.0, 3.0])
    
    def test_dot_product_dimension_error(self):
        """Test dimension mismatch error in dot product."""
        self.skip_if_no_enhanced()
        
        # This should raise an error
        with self.assertRaises(Exception):
            self.enhanced.dot_product([1.0, 2.0], [1.0, 2.0, 3.0])

class TestEnhancedIntegration(unittest.TestCase):
    """Test integration of enhanced bindings with original bindings."""
    
    def setUp(self):
        """Set up test fixtures."""
        try:
            import pynlm as basic
            self.basic = basic
            import pynlm_enhanced as enhanced
            self.enhanced = enhanced
            self.has_both = True
        except ImportError:
            self.has_both = False
            self.basic = None
            self.enhanced = None
    
    def skip_if_no_both(self):
        """Skip test if both bindings are not available."""
        if not self.has_both:
            self.skipTest("Both NLM bindings not available")
    
    def test_backward_compatibility(self):
        """Test that enhanced bindings maintain backward compatibility."""
        self.skip_if_no_both()
        
        # Create basic config using original API
        config1 = self.basic.createDefaultConfig()
        config1.set("neuron_count", 1000)
        
        # Create config using enhanced API
        config2 = self.enhanced.create_advanced_config(neuron_count=1000)
        
        # Both should work
        brain1 = self.basic.createBrain(config1)
        brain1.initialize()
        
        brain2 = self.basic.createBrain(config2)
        brain2.initialize()
        
        self.assertGreaterEqual(brain1.getTotalNeuronCount(), 0)
        self.assertGreaterEqual(brain2.getTotalNeuronCount(), 0)
    
    def test_enhanced_features_accessible(self):
        """Test that enhanced features are accessible through enhanced bindings."""
        self.skip_if_no_both()
        
        # Test that enhanced API has additional features
        enhanced_features = [
            'create_advanced_config',
            'BrainAnalyzer',
            'BatchSimulation',
            'AgentAnalyzer',
            'WorldUtils',
            'normalize_vector',
            'clamp',
            'euclidean_distance',
            'dot_product'
        ]
        
        for feature in enhanced_features:
            self.assertTrue(hasattr(self.enhanced, feature),
                           f"Enhanced binding missing feature: {feature}")

if __name__ == '__main__':
    # Run all tests
    unittest.main(verbosity=2)