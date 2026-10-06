"""
Integration tests for NLM Python bindings (pynlm)

These integration tests validate the complete end-to-end functionality
of the NLM Python bindings, including all new features such as:
- Advanced visualization and monitoring
- Data analysis and export
- Error handling and validation
- Convenience functions and factory methods
- Complete agent workflows

The tests ensure that all components work together seamlessly
and that the Python bindings provide a robust, user-friendly interface
to the NLM neural simulation framework.
"""

import pynlm
import tempfile
import os
import unittest
import json


class TestCompleteAgentWorkflow(unittest.TestCase):
    """Integration test for complete agent workflow."""
    
    def setUp(self):
        """Set up test environment."""
        # Create configuration
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.num_neurons", 200)
        self.config.set("brain.synapse_density", 0.05)
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
    
    def test_complete_simulation_episode(self):
        """Test a complete simulation episode with all subsystems active."""
        # Run simulation episode
        for step in range(100):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply motor command
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = self.get_reward_from_world()
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Verify state at key points
            if step == 0:
                self.assertGreater(self.brain.getTotalNeuronCount(), 0)
                self.assertGreater(self.world.getObjectCount(), 0)
            
            if step == 99:
                # Final state checks
                self.assertGreater(self.brain.getTotalSpikeCount(), 0)
                self.assertGreater(self.brain.getFiringNeuronCount(), 0)
                self.assertGreater(self.agent.getCuriosityLevel(), 0.0)
                self.assertGreater(self.agent.getNoveltyLevel(), 0.0)
    
    def get_reward_from_world(self):
        """Extract reward from world state (simplified)."""
        percept = self.world.getSensoryPercept()
        internal = percept.getInternal()
        return internal[0] if internal else 0.0
    
    def test_configuration_validation(self):
        """Test configuration validation with smart_brain."""
        # Test valid configuration
        valid_config = pynlm.createDefaultConfig()
        valid_config.set("brain.neuron_count", 500)
        valid_config.set("brain.synapse_density", 0.1)
        
        # Should not raise exception
        self.assertEqual(pynlm.validate_parameters(valid_config), "Configuration is valid")
        
        # Test invalid configuration
        invalid_config = pynlm.createDefaultConfig()
        invalid_config.set("brain.neuron_count", -100)  # Invalid
        
        # Should raise ValueError
        with self.assertRaises(ValueError):
            pynlm.validate_parameters(invalid_config)
        
        # Check validation errors
        errors = pynlm.get_validation_errors(invalid_config)
        self.assertGreater(len(errors), 0)
        self.assertIn("brain.neuron_count", errors[0])
    
    def test_smart_brain_functionality(self):
        """Test smart brain functionality."""
        # Create smart brain
        smart_brain = pynlm.create_smart_brain(self.config)
        
        # Test context manager
        with smart_brain as sb:
            self.assertIsNotNone(sb)
            self.assertGreater(sb.get_neuron_count(), 0)
            self.assertGreater(sb.get_synapse_count(), 0)
        
        # Test simulation
        result = smart_brain.run_simulation(10)
        self.assertIn("10 simulation steps completed", result)
        
        # Test state dictionary
        state = smart_brain.get_state_dict()
        self.assertIsInstance(state, dict)
        self.assertGreater(len(state), 0)
    
    def test_agent_lifecycle_manager(self):
        """Test agent lifecycle manager."""
        # Create lifecycle
        lifecycle = pynlm.create_agent_lifecycle(self.brain, self.world, 2, 50)
        
        # Check initial state
        self.assertTrue(lifecycle.has_next_episode())
        self.assertEqual(lifecycle.getEpisodeProgress(), 0.0)
        self.assertEqual(lifecycle.getStepProgress(), 0.0)
        
        # Run all episodes
        episodes_run = 0
        while lifecycle.has_next_episode():
            lifecycle.run_next_episode()
            episodes_run += 1
            self.assertGreater(lifecycle.getEpisodeProgress(), (episodes_run - 1) / 2.0)
        
        self.assertEqual(episodes_run, 2)
        self.assertEqual(lifecycle.getEpisodeProgress(), 1.0)
        
        # Test reset
        lifecycle.reset()
        self.assertEqual(lifecycle.getEpisodeProgress(), 0.0)
    
    def test_training_utility(self):
        """Test training utility."""
        # Create training utility
        training = pynlm.create_training_utility(self.agent, self.world, 2, 0.1, 0.01)
        
        # Run training
        rewards, lengths = training.train()
        
        # Verify results
        self.assertEqual(len(rewards), 2)
        self.assertEqual(len(lengths), 2)
        self.assertGreaterEqual(lengths[0], 90)  # Close to 100
        
        # Check average reward calculation
        avg_reward = training.get_average_reward(rewards)
        self.assertIsInstance(avg_reward, float)
        self.assertGreaterOrEqual(avg_reward, -10.0)
    
    def test_visualization_workflow(self):
        """Test visualization workflow."""
        # Create advanced visualizer
        visualizer = pynlm.create_advanced_visualizer(
            self.brain, self.world, 0.1, True, "test_visualization.csv"
        )
        
        # Run some updates
        for i in range(5):
            visualizer.update()
            self.world.update(0.1)
            self.brain.step(i)
        
        # Check history
        history = visualizer.get_history()
        self.assertGreater(len(history), 0)
        
        # Check statistics
        stats = visualizer.get_history_stats()
        self.assertGreater(stats["total_samples"], 0)
        self.assertGreater(stats["neuron_count_avg"], 0)
        
        # Check export
        self.assertTrue(visualizer.save_to_file())
        
        # Check current state
        current_state = visualizer.get_current_state()
        self.assertIsInstance(current_state, dict)
        self.assertIn("neuron_count", current_state)
        
        # Check realtime plot data
        plot_data = visualizer.get_realtime_plot_data(100)
        self.assertIn("time", plot_data)
        self.assertIn("firing_rate", plot_data)
        self.assertIn("neuron_count", plot_data)
        
        # Check analysis results
        analysis = visualizer.export_analysis_results()
        self.assertIn("history_statistics", analysis)
        self.assertIn("performance_metrics", analysis)
        self.assertIn("summary", analysis)
        self.assertIn("recommendations", analysis)
        
        # Clean up
        visualizer.clear_history()
    
    def test_realtime_monitoring(self):
        """Test real-time monitoring."""
        # Create monitor
        monitor = pynlm.create_realtime_monitor(self.brain, self.world, 0.2, 100)
        
        # Start monitoring
        monitor.start_monitoring()
        self.assertTrue(monitor.is_monitoring())
        
        # Run simulation while monitoring
        for i in range(10):
            self.world.update(0.1)
            self.brain.step(i)
            monitor.update_monitor()
            self.assertGreater(monitor.get_current_monitor_data()["neuron_count"], 0)
        
        # Check history
        history = monitor.get_monitor_history()
        self.assertGreater(len(history), 0)
        
        # Check current data
        current_data = monitor.get_current_monitor_data()
        self.assertIn("neuron_count", current_data)
        self.assertIn("brain_health", current_data)
        self.assertIn("stability_score", current_data)
        self.assertIn("activity_level", current_data)
        self.assertIn("alerts", current_data)
        
        # Check health calculations
        self.assertGreater(current_data["brain_health"], 0)
        self.assertGreaterOrEqual(current_data["stability_score"], 0)
        self.assertGreaterOrEqual(current_data["activity_level"], 0)
        
        # Test alerts
        alerts = current_data["alerts"]
        self.assertIsInstance(alerts, list)
        
        # Export monitoring data
        export_file = "test_monitor_data.csv"
        self.assertTrue(monitor.export_monitoring_data(export_file))
        
        # Verify file was created
        self.assertTrue(os.path.exists(export_file))
        
        # Clean up
        os.remove(export_file)
    
    def test_data_analysis(self):
        """Test data analysis utility."""
        # Create sample data file
        sample_data = """timestamp,neuron_count,synapse_count,active_neuron_count,firing_neuron_count,average_firing_rate,total_spikes
g_test,100,500,50,30,2.5,500
g_test,105,520,55,35,2.8,600
g_test,110,530,60,40,3.2,700
"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.csv', delete=False) as f:
            f.write(sample_data)
            data_file = f.name
        
        try:
            # Analyze data
            results = pynlm.analyze_simulation_data(data_file)
            
            # Check results
            self.assertIn("time_span", results)
            self.assertIn("neuron_count_avg", results)
            self.assertIn("firing_rate_avg", results)
            self.assertIn("total_spikes_avg", results)
            
            # Verify values
            self.assertGreater(results["time_span"], 0)
            self.assertGreater(results["neuron_count_avg"], 0)
            self.assertGreater(results["firing_rate_avg"], 0)
            
            # Check activity trend
            self.assertIn("activity_trend", results)
            
            # Check stability
            self.assertIn("neuron_stability", results)
            self.assertIn("firing_stability", results)
            
            # Check quality
            self.assertIn("data_quality_score", results)
            
        finally:
            os.unlink(data_file)
    
    def test_example_scenarios(self):
        """Test example scenarios from documentation."""
        # Test example_usage
        result = pynlm.example_usage()
        self.assertIn("Example usage printed to console", result)
        
        # Test advanced training example
        result = pynlm.example_advanced_training()
        self.assertIn("Advanced training example completed", result)
        
        # Test visualization example
        result = pynlm.example_visualization()
        self.assertIn("Visualization example completed", result)
        
        # Test memory management example
        result = pynlm.example_memory_management()
        self.assertIn("Memory management example completed", result)
    
    def test_error_handling_integration(self):
        """Test error handling integration."""
        # Test safe operations
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        # Test safe config operations
        result = pynlm.safe_config_set(config, "brain.synapse_density", "0.1")
        self.assertIn("Successfully set", result)
        
        value = pynlm.safe_config_get(config, "brain.synapse_density")
        self.assertIn("0.1", value)
        
        # Test safe simulation
        result = pynlm.safe_simulation_step(self.brain, 5)
        self.assertIn("Simulation step completed successfully", result)
        
        # Test smart brain error handling
        smart_brain = pynlm.create_smart_brain(config)
        
        # Should raise error for negative steps
        with self.assertRaises(ValueError):
            smart_brain.run_simulation(-1)
    
    def test_comprehensive_workflow(self):
        """Test a comprehensive workflow using all features."""
        # Step 1: Create and initialize brain
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 150)
        config.set("brain.synapse_density", 0.08)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Step 2: Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Step 3: Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Step 4: Create visualization
        visualizer = pynlm.create_advanced_visualizer(
            brain, world, 0.5, True, "comprehensive_test.csv"
        )
        
        # Step 5: Create monitoring
        monitor = pynlm.create_realtime_monitor(brain, world, 0.3, 500)
        monitor.start_monitoring()
        
        # Step 6: Run simulation loop
        simulation_stats = {
            'total_steps': 200,
            'final_neuron_count': 0,
            'final_spike_count': 0,
            'max_curiosity': 0.0,
            'max_novelty': 0.0,
            'avg_firing_rate': 0.0
        }
        
        for step in range(simulation_stats['total_steps']):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = self.get_reward_from_world()
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Update visualization and monitoring
            visualizer.update()
            monitor.update_monitor()
            
            # Collect statistics
            simulation_stats['final_neuron_count'] = brain.getTotalNeuronCount()
            simulation_stats['final_spike_count'] = brain.getTotalSpikeCount()
            simulation_stats['max_curiosity'] = max(simulation_stats['max_curiosity'], agent.getCuriosityLevel())
            simulation_stats['max_novelty'] = max(simulation_stats['max_novelty'], agent.getNoveltyLevel())
            simulation_stats['avg_firing_rate'] += brain.getAverageFiringRate()
        
        simulation_stats['avg_firing_rate'] /= simulation_stats['total_steps']
        
        # Step 7: Verify results
        self.assertGreater(simulation_stats['final_neuron_count'], 0)
        self.assertGreater(simulation_stats['final_spike_count'], 0)
        self.assertGreater(simulation_stats['max_curiosity'], 0.0)
        self.assertGreater(simulation_stats['max_novelty'], 0.0)
        self.assertGreater(simulation_stats['avg_firing_rate'], 0.0)
        
        # Step 8: Export and analyze data
        monitor.export_monitoring_data("comprehensive_monitor.csv")
        visualizer.save_to_file()
        
        # Verify files were created
        self.assertTrue(os.path.exists("comprehensive_monitor.csv"))
        self.assertTrue(os.path.exists("visualization_output.csv"))
        
        # Step 9: Clean up
        os.remove("comprehensive_monitor.csv")
        
        # Clean up visualization file (it keeps growing)
        if os.path.exists("visualization_output.csv"):
            os.remove("visualization_output.csv")
        
        # Step 10: Generate final reports
        analysis = visualizer.export_analysis_results()
        monitor_stats = monitor.get_current_monitor_data()
        
        # Verify analysis contains expected data
        self.assertIn("history_statistics", analysis)
        self.assertIn("performance_metrics", analysis)
        self.assertIn("summary", analysis)
        
        # Verify monitor contains expected data
        self.assertIn("neuron_count", monitor_stats)
        self.assertIn("brain_health", monitor_stats)
        
        # Step 11: Run agent lifecycle
        lifecycle = pynlm.create_agent_lifecycle(brain, world, 1, 100)
        while lifecycle.has_next_episode():
            lifecycle.run_next_episode()
        
        self.assertEqual(lifecycle.getEpisodeProgress(), 1.0)
        
        # Step 12: Clean up everything
        brain.reset()
        world.reset()
        
        # Final verification
        self.assertEqual(brain.getTotalNeuronCount(), 150)


class TestErrorScenarios(unittest.TestCase):
    """Test error scenarios and edge cases."""
    
    def test_invalid_configurations(self):
        """Test handling of invalid configurations."""
        # Test negative neuron count
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", -1)
        
        with self.assertRaises(ValueError):
            pynlm.validate_parameters(config)
        
        # Test invalid synapse density
        config.set("brain.neuron_count", 100)
        config.set("brain.synapse_density", 1.5)  # > 1.0
        
        with self.assertRaises(ValueError):
            pynlm.validate_parameters(config)
        
        # Test invalid connection probability
        config.set("brain.synapse_density", 0.1)
        config.set("brain.connection_probability", -0.1)  # < 0.0
        
        with self.assertRaises(ValueError):
            pynlm.validate_parameters(config)
    
    def test_invalid_simulation_steps(self):
        """Test handling of invalid simulation steps."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 100)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test negative steps
        with self.assertRaises(ValueError):
            pynlm.safe_simulation_step(brain, -1)
        
        # Test zero timestep
        with self.assertRaises(ValueError):
            pynlm.safe_simulation_step(brain, 5, 0.0)
        
        # Test negative timestep
        with self.assertRaises(ValueError):
            pynlm.safe_simulation_step(brain, 5, -0.1)
    
    def test_null_pointer_handling(self):
        """Test handling of null pointers."""
        # Test null brain
        with self.assertRaises(ValueError):
            pynlm.safe_brain_operation(None, lambda b: None)
        
        # Test null brain in other operations
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        with self.assertRaises(ValueError):
            pynlm.safe_brain_operation(brain, None)
    
    def test_file_not_found_handling(self):
        """Test handling of file not found errors."""
        # Test loading non-existent file
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This should not crash even if file doesn't exist
        with self.assertRaises(Exception):
            brain.load("non_existent_file.bin")
        
        # Test analyzing non-existent file
        with self.assertRaises(Exception):
            pynlm.analyze_simulation_data("non_existent_file.csv")
    
    def test_monitoring_edge_cases(self):
        """Test monitoring edge cases."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 50)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        monitor = pynlm.create_realtime_monitor(brain, world, 1.0, 10)
        
        # Test monitoring before starting
        self.assertFalse(monitor.is_monitoring())
        
        # Start monitoring
        monitor.start_monitoring()
        self.assertTrue(monitor.is_monitoring())
        
        # Stop monitoring
        monitor.stop_monitoring()
        self.assertFalse(monitor.is_monitoring())
        
        # Test monitor with invalid parameters
        invalid_monitor = pynlm.create_realtime_monitor(brain, world, -1.0, -10)
        invalid_monitor.start_monitoring()
    
    def test_visualization_edge_cases(self):
        """Test visualization edge cases."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 30)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=5, height=5, visionWidth=4, visionHeight=4)
        
        # Create visualizer with invalid parameters
        visualizer = pynlm.create_advanced_visualizer(
            brain, world, 0.0, False, ""  # Invalid parameters
        )
        
        # Should still create visualizer
        self.assertIsNotNone(visualizer)
        
        # Update should not crash
        visualizer.update()
        
        # Clear history
        visualizer.clear_history()
        
        # Get history (should be empty)
        history = visualizer.get_history()
        self.assertEqual(len(history), 0)
        
        # Get current state
        state = visualizer.get_current_state()
        self.assertIsInstance(state, dict)
    
    def test_error_recovery(self):
        """Test error recovery capabilities."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 75)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create smart brain
        smart_brain = pynlm.create_smart_brain(config)
        
        # Run successful simulation
        result1 = smart_brain.run_simulation(5)
        self.assertIn("5 simulation steps completed", result1)
        
        # Try invalid simulation (should fail)
        with self.assertRaises(ValueError):
            smart_brain.run_simulation(-1)
        
        # Smart brain should still be usable
        result2 = smart_brain.run_simulation(3)
        self.assertIn("3 simulation steps completed", result2)
        
        # Context manager should still work
        with smart_brain as sb:
            self.assertIsNotNone(sb)
    
    def test_resource_cleanup(self):
        """Test resource cleanup after errors."""
        import gc
        
        # Create and destroy multiple objects
        configs = []
        brains = []
        worlds = []
        agents = []
        
        for i in range(5):
            config = pynlm.createDefaultConfig()
            config.set("brain.num_neurons", 20 + i * 10)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            configs.append(config)
            brains.append(brain)
            worlds.append(world)
            agents.append(agent)
        
        # Force garbage collection
        del configs
        del brains
        del worlds
        del agents
        gc.collect()
        
        # Verify remaining objects still work
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 100)
        brain = pynlm.createBrain(config)
        brain.initialize()
        self.assertGreater(brain.getTotalNeuronCount(), 0)


class TestPerformanceBenchmarks(unittest.TestCase):
    """Performance benchmark tests."""
    
    def setUp(self):
        """Set up benchmarks."""
        self.benchmark_config = pynlm.createDefaultConfig()
        self.benchmark_config.set("brain.num_neurons", 500)
    
    def test_brain_creation_performance(self):
        """Test brain creation performance."""
        import time
        
        start_time = time.time()
        brain = pynlm.createBrain(self.benchmark_config)
        brain.initialize()
        end_time = time.time()
        
        creation_time = end_time - start_time
        self.assertLess(creation_time, 2.0)  # Should be fast
        
        # Verify brain was created correctly
        self.assertGreater(brain.getTotalNeuronCount(), 0)
    
    def test_simulation_performance(self):
        """Test simulation performance."""
        import time
        
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        # Run simulation
        for i in range(100):
            brain.step(i)
        
        end_time = time.time()
        
        simulation_time = end_time - start_time
        self.assertLess(simulation_time, 5.0)  # Should be fast
        
        # Verify simulation completed
        self.assertGreater(brain.getTotalSpikeCount(), 0)
    
    def test_visualization_performance(self):
        """Test visualization performance."""
        import time
        
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        # Create visualizer
        start_time = time.time()
        visualizer = pynlm.create_advanced_visualizer(brain, world, 0.1, False)
        end_time = time.time()
        
        creation_time = end_time - start_time
        self.assertLess(creation_time, 1.0)
        
        # Update visualization
        start_time = time.time()
        for i in range(50):
            visualizer.update()
            world.update(0.1)
            brain.step(i)
        end_time = time.time()
        
        update_time = end_time - start_time
        self.assertLess(update_time, 2.0)
        
        # Check performance characteristics
        self.assertGreaterEqual(visualizer.get_history().size(), 0)
    
    def test_memory_usage(self):
        """Test memory usage patterns."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create multiple brains
        brains = []
        for i in range(5):
            config = pynlm.createDefaultConfig()
            config.set("brain.num_neurons", 100 + i * 50)
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        after_creation = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run simulation
        for brain in brains:
            for i in range(20):
                brain.step(i)
        
        after_simulation = process.memory_info().rss / 1024 / 1024  # MB
        
        # Memory usage should be reasonable
        memory_increase = after_creation - initial_memory
        self.assertLess(memory_increase, 100.0)  # Less than 100MB increase
        
        # Clean up
        del brains
        import gc
        gc.collect()
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_after_cleanup = final_memory - initial_memory
        self.assertLess(memory_after_cleanup, 50.0)  # Less than 50MB after cleanup


if __name__ == '__main__':
    unittest.main()
