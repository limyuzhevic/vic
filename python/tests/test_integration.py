#!/usr/bin/env python3
"""
Integration Tests for NLM Python Bindings
=========================================

Integration tests for NLM Python bindings including:
- Complete agent simulation tests
- Batch processing integration tests
- Performance benchmarking
- Cross-version compatibility
- Real-world scenario simulations
"""

import sys
import os
import unittest
import time
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

class TestCompleteAgentSimulation(unittest.TestCase):
    """Test complete agent simulation scenarios."""
    
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
    
    def test_complete_agent_episode(self):
        """Test a complete agent episode with learning."""
        self.skip_if_no_bindings()
        
        def run_episode(num_steps=100):
            """Run a complete agent episode."""
            
            # Setup
            config = self.pynlm.createDefaultConfig()
            brain = self.pynlm.createBrain(config)
            brain.initialize()
            
            agent = self.pynlm.createAgentBrain(brain)
            world = self.pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            world.setAgentStart(10.0, 10.0)
            agent.initialize(world)
            
            # Enable learning subsystems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Track episode data
            episode_data = {
                'steps': [],
                'rewards': [],
                'curiosity_levels': [],
                'novelty_levels': [],
                'firing_rates': []
            }
            
            # Run episode
            for step in range(num_steps):
                # Update world
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Run brain
                brain.step(step)
                
                # Get action
                motor_cmd = agent.decodeMotorCommand()
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                # Calculate reward
                reward = 0.0
                if percept and percept.getInternal():
                    reward = percept.getInternal()[0]
                
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
                
                # Record data
                episode_data['steps'].append(step)
                episode_data['rewards'].append(reward)
                episode_data['curiosity_levels'].append(agent.getCuriosityLevel())
                episode_data['novelty_levels'].append(agent.getNoveltyLevel())
                episode_data['firing_rates'].append(brain.getFiringNeuronCount())
            
            return episode_data
        
        # Run episode
        episode = run_episode(50)
        
        # Verify episode data
        self.assertEqual(len(episode['steps']), 50)
        self.assertEqual(len(episode['rewards']), 50)
        self.assertEqual(len(episode['curiosity_levels']), 50)
        self.assertEqual(len(episode['novelty_levels']), 50)
        self.assertEqual(len(episode['firing_rates']), 50)
        
        # Verify learning occurred
        self.assertGreater(np.mean(episode['curiosity_levels']), 0.0)
        self.assertGreater(np.mean(episode['novelty_levels']), 0.0)
        
        # Verify simulation was active
        self.assertGreater(np.mean(episode['firing_rates']), 0)
        
        # Verify reward modulation was applied
        reward_sum = sum(episode['rewards'])
        self.assertGreaterEqual(reward_sum, 0.0)  # Should not be negative

class TestBatchProcessingIntegration(unittest.TestCase):
    """Test batch processing integration scenarios."""
    
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
    
    def test_multibrain_batch(self):
        """Test batch processing with multiple brains."""
        self.skip_if_no_bindings()
        
        # Create multiple brain configurations
        configs = []
        for i in range(5):
            config = self.pynlm.createDefaultConfig()
            config.set("neuron_count", 500 + i * 300)
            configs.append(config)
        
        # Create brains and worlds
        brains = [self.pynlm.createBrain(config) for config in configs]
        worlds = []
        agents = []
        
        for brain in brains:
            brain.initialize()
            world = self.pynlm.createSimpleWorld()
            world.configure(10, 10, 6, 6)
            world.reset()
            worlds.append(world)
            
            agent = self.pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agents.append(agent)
        
        # Run batch simulation
        start_time = time.time()
        
        for step in range(100):
            for i, (brain, world, agent) in enumerate(zip(brains, worlds, agents)):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                motor_cmd = agent.decodeMotorCommand()
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                reward = percept.getInternal()[0] if percept and percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
        
        end_time = time.time()
        
        # Verify batch processing worked
        for i, brain in enumerate(brains):
            self.assertGreater(brain.getTotalSpikeCount(), 0)
            self.assertGreater(brain.getFiringNeuronCount(), 0)
        
        # Verify performance
        self.assertLess(end_time - start_time, 5.0)  # Should complete quickly
        
        print(f"Batch processing completed {len(brains)} brains in {end_time - start_time:.2f}s")
    
    def test_batch_statistics(self):
        """Test batch processing statistics collection."""
        self.skip_if_no_bindings()
        
        # Create two identical brains for comparison
        config = self.pynlm.createDefaultConfig()
        brain1 = self.pynlm.createBrain(config)
        brain1.initialize()
        
        brain2 = self.pynlm.createBrain(config)
        brain2.initialize()
        
        # Run simulation on both
        for step in range(50):
            brain1.step(step)
            brain2.step(step)
        
        # Collect statistics
        stats1 = {
            'total_spikes': brain1.getTotalSpikeCount(),
            'firing_neurons': brain1.getFiringNeuronCount(),
            'average_firing_rate': brain1.getAverageFiringRate(),
            'e_i_ratio': brain1.getExcitationInhibitionRatio()
        }
        
        stats2 = {
            'total_spikes': brain2.getTotalSpikeCount(),
            'firing_neurons': brain2.getFiringNeuronCount(),
            'average_firing_rate': brain2.getAverageFiringRate(),
            'e_i_ratio': brain2.getExcitationInhibitionRatio()
        }
        
        # Verify statistics are reasonable
        for stats in [stats1, stats2]:
            self.assertGreaterEqual(stats['total_spikes'], 0)
            self.assertGreaterEqual(stats['firing_neurons'], 0)
            self.assertGreaterEqual(stats['average_firing_rate'], 0.0)
            self.assertGreaterEqual(stats['e_i_ratio'], 0.0)

class TestPerformanceBenchmarking(unittest.TestCase):
    """Test performance benchmarking scenarios."""
    
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
    
    def test_simulation_scalability(self):
        """Test simulation scalability with different brain sizes."""
        self.skip_if_no_bindings()
        
        brain_sizes = [500, 1000, 1500, 2000]
        simulation_steps = 20
        
        times = []
        
        for neuron_count in brain_sizes:
            # Create configuration
            config = self.pynlm.createDefaultConfig()
            config.set("neuron_count", neuron_count)
            
            # Create and initialize brain
            brain = self.pynlm.createBrain(config)
            brain.initialize()
            
            # Time simulation
            start_time = time.time()
            for step in range(simulation_steps):
                brain.step(step)
            end_time = time.time()
            
            elapsed_time = end_time - start_time
            times.append(elapsed_time)
            
            # Verify brain state
            self.assertGreater(brain.getTotalNeuronCount(), 0)
            self.assertGreater(brain.getTotalSpikeCount(), 0)
            
            print(f"Brain size {neuron_count}: {elapsed_time:.3f}s for {simulation_steps} steps")
        
        # Verify scalability (time should increase reasonably with brain size)
        self.assertEqual(len(times), len(brain_sizes))
        self.assertLess(times[-1] / times[0], 5.0)  # Last should not be more than 5x slower
    
    def test_memory_usage(self):
        """Test memory usage patterns."""
        self.skip_if_no_bindings()
        
        # Create brain and run simulation
        config = self.pynlm.createDefaultConfig()
        brain = self.pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        # Get brain statistics
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        
        # Verify memory usage is reasonable
        # (Note: This is a rough estimate since we don't have direct memory access)
        estimated_memory_mb = (total_neurons * 8 + total_synapses * 4) / (1024 * 1024)  # Rough estimate
        
        print(f"Estimated memory usage: {estimated_memory_mb:.2f} MB")
        self.assertLess(estimated_memory_mb, 100.0)  # Should be less than 100MB

class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced NLM features."""
    
    def setUp(self):
        """Set up test fixtures."""
        try:
            import pynlm as basic
            import pynlm_enhanced as enhanced
            self.basic = basic
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
    
    def test_enhanced_configuration_features(self):
        """Test enhanced configuration features."""
        self.skip_if_no_both()
        
        # Test various configuration options
        config = self.enhanced.create_advanced_config(
            neuron_count=1500,
            region_count=3,
            connection_probability=0.2,
            plasticity_rate=0.02,
            development_rate=0.003
        )
        
        # Verify configuration was created successfully
        self.assertIsNotNone(config)
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("region_count"))
        
        # Verify values
        self.assertEqual(config.getOr[int64_t]("neuron_count", 0), 1500)
        self.assertEqual(config.getOr[int64_t]("region_count", 0), 3)
        self.assertEqual(config.getOr[float]("connection_probability", 0), 0.2)
    
    def test_utility_functions_integration(self):
        """Test integration of utility functions."""
        self.skip_if_no_both()
        
        # Test mathematical utility functions
        data = [1.0, 2.0, 3.0, 4.0, 5.0]
        
        # Test normalization
        normalized = self.enhanced.normalize_vector(data)
        self.assertEqual(len(normalized), len(data))
        self.assertAlmostEqual(sum(normalized), 1.0, places=5)
        
        # Test clamping
        clamped = self.enhanced.clamp(15.0, 0.0, 10.0)
        self.assertEqual(clamped, 10.0)
        
        # Test distance calculation
        a = [1.0, 2.0]
        b = [4.0, 6.0]
        distance = self.enhanced.euclidean_distance(a, b)
        expected = np.sqrt((4-1)**2 + (6-2)**2)  # sqrt(9 + 16) = 5
        self.assertAlmostEqual(distance, expected, places=10)
        
        # Test dot product
        dot = self.enhanced.dot_product(a, b)
        expected = 1*4 + 2*6  # 16
        self.assertAlmostEqual(dot, expected, places=10)
    
    def test_enhanced_agent_features(self):
        """Test enhanced agent features."""
        self.skip_if_no_both()
        
        # Create basic setup
        config = self.basic.createDefaultConfig()
        brain = self.basic.createBrain(config)
        brain.initialize()
        
        agent = self.basic.createAgentBrain(brain)
        world = self.basic.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        agent.initialize(world)
        
        # Run simulation
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Test enhanced analysis
        perf = self.enhanced.AgentAnalyzer.analyze_performance(agent)
        
        self.assertIsInstance(perf, dict)
        self.assertGreaterEqual(perf['curiosity_level'], 0.0)
        self.assertLessEqual(perf['curiosity_level'], 1.0)
        self.assertGreaterEqual(perf['novelty_level'], 0.0)
        self.assertLessEqual(perf['novelty_level'], 1.0)
    
    def test_world_analyzer_integration(self):
        """Test world analyzer integration."""
        self.skip_if_no_both()
        
        # Create world using enhanced utilities
        world = self.enhanced.WorldUtils.create_world(
            width=15, height=15, vision_width=6, vision_height=6
        )
        
        # Verify world properties
        self.assertEqual(world.getWidth(), 15)
        self.assertEqual(world.getHeight(), 15)
        
        # Get environment statistics
        stats = self.enhanced.WorldUtils.get_environment_statistics(world)
        
        self.assertIsInstance(stats, dict)
        self.assertIn('width', stats)
        self.assertIn('height', stats)
        self.assertIn('simulation_time', stats)

class TestErrorHandlingIntegration(unittest.TestCase):
    """Test error handling integration."""
    
    def setUp(self):
        """Set up test fixtures."""
        try:
            import pynlm as basic
            self.basic = basic
            self.has_bindings = True
        except ImportError:
            self.has_bindings = False
            self.basic = None
    
    def skip_if_no_bindings(self):
        """Skip test if bindings are not available."""
        if not self.has_bindings:
            self.skipTest("NLM bindings not available")
    
    def test_simulation_error_handling(self):
        """Test error handling during simulation."""
        self.skip_if_no_bindings()
        
        config = self.basic.createDefaultConfig()
        brain = self.basic.createBrain(config)
        brain.initialize()
        
        # Test that simulation can handle various edge cases
        try:
            for step in range(10):
                brain.step(step)
            
            # Simulation should complete without errors
            self.assertGreater(brain.getTotalSpikeCount(), 0)
            
        except Exception as e:
            self.fail(f"Simulation raised an unexpected exception: {e}")
    
    def test_world_error_handling(self):
        """Test error handling with world interactions."""
        self.skip_if_no_bindings()
        
        world = self.basic.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        # Test world operations
        try:
            world.update(0.1)
            percept = world.getSensoryPercept()
            self.assertIsNotNone(percept)
            
            # Test agent body access
            body = world.getAgentBody()
            self.assertIsNotNone(body)
            
        except Exception as e:
            self.fail(f"World operations raised an unexpected exception: {e}")

if __name__ == '__main__':
    # Run all integration tests
    unittest.main(verbosity=2)