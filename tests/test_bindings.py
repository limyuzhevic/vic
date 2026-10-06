"""
Python tests for NLM Python bindings (pynlm)

This test suite covers the main functionality of the NLM Python bindings,
including brain simulation, agent behavior, world interaction, and
the new higher-level abstractions.
"""

import pynlm
import tempfile
import os
import unittest
import pytest


class TestConfig(unittest.TestCase):
    """Test Config class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
    
    def test_config_initialization(self):
        """Test that Config initializes correctly."""
        self.assertIsNotNone(self.config)
        # Config should be empty initially
        self.assertEqual(len(self.config.getKeys()), 0)
    
    def test_config_has(self):
        """Test config.has() method."""
        self.assertFalse(self.config.has("brain.neuron_count"))
        
        # Set a value
        self.config.set("brain.neuron_count", 1000)
        self.assertTrue(self.config.has("brain.neuron_count"))
    
    def test_config_set_get(self):
        """Test config.set() and config.getOr()."""
        self.config.set("brain.neuron_count", 500)
        # Use getOr for type-safe retrieval
        self.assertEqual(self.config.getOr("brain.neuron_count", 0), 500)
        
        # Test default value
        self.assertEqual(self.config.getOr("nonexistent.key", 42), 42)
    
    def test_config_clear(self):
        """Test config.clear() method."""
        self.config.set("key1", "value1")
        self.config.set("key2", "value2")
        self.assertEqual(len(self.config.getKeys()), 2)
        
        self.config.clear()
        self.assertEqual(len(self.config.getKeys()), 0)
    
    def test_config_summary(self):
        """Test config.summary() method."""
        self.config.set("brain.neuron_count", 1000)
        summary = self.config.summary()
        self.assertIn("brain.neuron_count", summary)
        self.assertIn("1000", summary)


class TestBrain(unittest.TestCase):
    """Test Brain class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 100)
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        self.assertIsNotNone(self.brain)
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
    
    def test_brain_step(self):
        """Test brain.step() method."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run a few steps
        for i in range(10):
            self.brain.step(i)
        
        # Spikes may have increased but not necessarily
        # Just verify the method doesn't crash
        self.assertIsNotNone(self.brain.getTotalSpikeCount())
    
    def test_brain_statistics(self):
        """Test brain statistics methods."""
        self.assertEqual(self.brain.getTotalNeuronCount(), 100)
        self.assertGreater(self.brain.getFiringNeuronCount(), 0)
        self.assertGreater(self.brain.getAverageFiringRate(), 0.0)
        self.assertGreaterOrEqual(self.brain.getTotalSpikeCount(), 0)
    
    def test_brain_regions(self):
        """Test brain region functionality."""
        initial_regions = self.brain.getRegionCount()
        self.assertGreaterOrEqual(initial_regions, 1)
        
        # Add a region
        region_id = self.brain.addRegion("test_region")
        self.assertIsNotNone(region_id)
        
        # Verify region exists
        region = self.brain.getRegion(region_id)
        self.assertIsNotNone(region)
    
    def test_brain_reset(self):
        """Test brain.reset() method."""
        # Get current stats
        neurons_before = self.brain.getTotalNeuronCount()
        
        # Run some steps
        for i in range(20):
            self.brain.step(i)
        
        # Reset
        self.brain.reset()
        self.brain.initialize()
        
        # Stats should be same after reset
        self.assertEqual(self.brain.getTotalNeuronCount(), neurons_before)


class TestAgentBrain(unittest.TestCase):
    """Test AgentBrain class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 200)
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_initialization(self):
        """Test AgentBrain initialization."""
        self.assertIsNotNone(self.agent)
        self.assertGreater(self.agent.getSensoryInputSize(), 0)
        self.assertGreater(self.agent.getMotorOutputSize(), 0)
    
    def test_agent_sensory_processing(self):
        """Test agent sensory processing."""
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Should not crash
        self.assertIsNotNone(self.agent.getCuriosityLevel())
        self.assertIsNotNone(self.agent.getNoveltyLevel())
    
    def test_agent_motor_decoding(self):
        """Test agent motor decoding."""
        # First process some input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        self.assertIsNotNone(action)
        
        # Test known actions
        valid_actions = [pynlm.MotorCommand.MoveForward, 
                        pynlm.MotorCommand.MoveBackward,
                        pynlm.MotorCommand.TurnLeft,
                        pynlm.MotorCommand.TurnRight,
                        pynlm.MotorCommand.Interact,
                        pynlm.MotorCommand.Wait]
        self.assertIn(action, valid_actions)
    
    def test_agent_neuromodulation(self):
        """Test agent neuromodulation."""
        # Apply reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)
        
        # Check that neuromodulation level changed
        self.assertIsNotNone(self.agent.getNeuromodulationLevel())
        self.assertIsNotNone(self.agent.getPredictionError())
    
    def test_agent_development(self):
        """Test agent development."""
        stage = self.agent.getDevelopmentalStage()
        self.assertIsNotNone(stage)
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Check that development stage might have changed
        new_stage = self.agent.getDevelopmentalStage()
        self.assertIsNotNone(new_stage)
    
    def test_agent_subsystem_enable(self):
        """Test enabling/disabling agent subsystems."""
        # Test enable/disable methods
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Check that methods don't crash
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())


class TestSimpleWorld(unittest.TestCase):
    """Test SimpleWorld class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    
    def test_world_initialization(self):
        """Test world initialization."""
        self.assertIsNotNone(self.world)
        self.assertEqual(self.world.getWidth(), 10)
        self.assertEqual(self.world.getHeight(), 10)
        self.assertEqual(self.world.getVisionWidth(), 8)
        self.assertEqual(self.world.getVisionHeight(), 8)
    
    def test_world_control(self):
        """Test world control methods."""
        initial_time = self.world.getSimulationTime()
        
        # Update world
        self.world.update(0.1)
        
        # Time should have increased
        self.assertGreater(self.world.getSimulationTime(), initial_time)
    
    def test_world_agent_interaction(self):
        """Test agent interaction methods."""
        # Set agent start
        self.world.setAgentStart(5.0, 5.0)
        
        # Apply motor command
        action = pynlm.MotorCommand.MoveForward
        result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Should return an ActionResult
        self.assertIsNotNone(result)
        self.assertIsInstance(result, pynlm.ActionResult)
    
    def test_world_state_access(self):
        """Test world state access methods."""
        # Get agent body
        body = self.world.getAgentBody()
        self.assertIsNotNone(body)
        self.assertEqual(body.x, 0.0)  # Default position
        self.assertEqual(body.y, 0.0)
        
        # Get sensory percept
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
    
    def test_world_objects(self):
        """Test world object management."""
        initial_count = self.world.getObjectCount()
        
        # Add an object
        obj = pynlm.WorldObject(3.0, 3.0, pynlm.WorldObjectType.Resource, 1.0)
        self.world.addObject(obj)
        
        # Count should have increased
        self.assertEqual(self.world.getObjectCount(), initial_count + 1)
        
        # Remove object
        self.world.removeObject(3.0, 3.0)
        self.assertEqual(self.world.getObjectCount(), initial_count)
    
    def test_world_configuration(self):
        """Test world configuration methods."""
        # Change configuration
        self.world.setMaxEnergy(100.0)
        self.assertEqual(self.world.getMaxEnergy(), 100.0)
        
        self.world.setEnergyDecayRate(0.5)
        self.assertEqual(self.world.getEnergyDecayRate(), 0.5)
    
    def test_world_validation(self):
        """Test world validation methods."""
        # Valid position
        self.assertTrue(self.world.isValidPosition(5.0, 5.0))
        
        # Invalid position (outside bounds)
        self.assertFalse(self.world.isValidPosition(-1.0, 5.0))
        self.assertFalse(self.world.isValidPosition(5.0, -1.0))
    
    def test_world_random_seed(self):
        """Test world random seed functionality."""
        seed = 42
        self.world.setRandomSeed(seed)
        self.assertEqual(self.world.getRandomSeed(), seed)


class TestConvenienceFunctions(unittest.TestCase):
    """Test convenience functions and examples."""
    
    def test_simulate_step(self):
        """Test simulate_step convenience function."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 50)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Use convenience function
        pynlm.simulate_step(brain, 5)
        
        # Should not crash
        self.assertIsNotNone(brain.getTotalSpikeCount())
    
    def test_example_usage(self):
        """Test example_usage function."""
        result = pynlm.example_usage()
        self.assertIsInstance(result, str)
        self.assertIn("Example usage printed to console", result)


class TestFactoryFunctions(unittest.TestCase):
    """Test factory functions."""
    
    def test_create_default_config(self):
        """Test createDefaultConfig factory function."""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertIsInstance(config, pynlm.Config)
    
    def test_create_brain(self):
        """Test createBrain factory function."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        self.assertIsNotNone(brain)
        self.assertIsInstance(brain, pynlm.Brain)
    
    def test_create_simple_world(self):
        """Test createSimpleWorld factory function."""
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        self.assertIsInstance(world, pynlm.SimpleWorld)
    
    def test_create_agent_brain(self):
        """Test createAgentBrain factory function."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        self.assertIsInstance(agent, pynlm.AgentBrain)
    
    def test_create_world_object(self):
        """Test createWorldObject factory function."""
        obj = pynlm.createWorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
        self.assertIsNotNone(obj)
        self.assertEqual(obj.x, 5.0)
        self.assertEqual(obj.y, 5.0)
        self.assertEqual(obj.type, pynlm.WorldObjectType.Resource)
    
    def test_create_memory_manager(self):
        """Test createMemoryManager factory function."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 100)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        memory_manager = pynlm.createMemoryManager(brain)
        self.assertIsNotNone(memory_manager)
        
        # Test memory stats
        stats = memory_manager.get_memory_stats()
        self.assertIsInstance(stats, dict)
        self.assertIn("neuron_count", stats)
        self.assertIn("synapse_count", stats)
    
    def test_create_visualizer(self):
        """Test createVisualizer factory function."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 50)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        visualizer = pynlm.createVisualizer(brain, world)
        self.assertIsNotNone(visualizer)
        
        # Test visualizations
        brain_view = visualizer.get_brain_text_view()
        world_view = visualizer.get_world_text_view()
        combined_view = visualizer.get_combined_view()
        
        self.assertIsInstance(brain_view, str)
        self.assertIsInstance(world_view, str)
        self.assertIsInstance(combined_view, str)
        self.assertIn(brain_view, combined_view)
        self.assertIn(world_view, combined_view)
    
    def test_create_agent_lifecycle(self):
        """Test createAgentLifecycle factory function."""
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 100)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
        
        lifecycle = pynlm.createAgentLifecycle(brain, world, 5, 50)
        self.assertIsNotNone(lifecycle)
        
        # Test lifecycle methods
        self.assertTrue(lifecycle.has_next_episode())
        self.assertEqual(lifecycle.getEpisodeProgress(), 0.0)
        self.assertEqual(lifecycle.getStepProgress(), 0.0)
        
        # Clean up
        lifecycle.reset()
        self.assertEqual(lifecycle.getEpisodeProgress(), 0.0)


class TestEnums(unittest.TestCase):
    """Test enumeration types."""
    
    def test_neuron_type_enum(self):
        """Test NeuronType enum."""
        self.assertIsNotNone(pynlm.NeuronType.Excitatory)
        self.assertIsNotNone(pynlm.NeuronType.Inhibitory)
        self.assertIsNotNone(pynlm.NeuronType.Sensory)
        self.assertIsNotNone(pynlm.NeuronType.Motor)
        self.assertIsNotNone(pynlm.NeuronType.Modulatory)
        self.assertIsNotNone(pynlm.NeuronType.Internal)
    
    def test_synapse_type_enum(self):
        """Test SynapseType enum."""
        self.assertIsNotNone(pynlm.SynapseType.Excitatory)
        self.assertIsNotNone(pynlm.SynapseType.Inhibitory)
        self.assertIsNotNone(pynlm.SynapseType.Modulatory)
        self.assertIsNotNone(pynlm.SynapseType.Electrical)
        self.assertIsNotNone(pynlm.SynapseType.GapJunction)
    
    def test_developmental_stage_enum(self):
        """Test DevelopmentalStage enum."""
        self.assertIsNotNone(pynlm.DevelopmentalStage.Initial)
        self.assertIsNotNone(pynlm.DevelopmentalStage.CriticalPeriod)
        self.assertIsNotNone(pynlm.DevelopmentalStage.Maturation)
        self.assertIsNotNone(pynlm.DevelopmentalStage.Adult)
        self.assertIsNotNone(pynlm.DevelopmentalStage.Aging)
    
    def test_motor_command_enum(self):
        """Test MotorCommand enum."""
        self.assertIsNotNone(pynlm.MotorCommand.MoveForward)
        self.assertIsNotNone(pynlm.MotorCommand.MoveBackward)
        self.assertIsNotNone(pynlm.MotorCommand.TurnLeft)
        self.assertIsNotNone(pynlm.MotorCommand.TurnRight)
        self.assertIsNotNone(pynlm.MotorCommand.LookLeft)
        self.assertIsNotNone(pynlm.MotorCommand.LookRight)
        self.assertIsNotNone(pynlm.MotorCommand.Interact)
        self.assertIsNotNone(pynlm.MotorCommand.Wait)
    
    def test_world_object_type_enum(self):
        """Test WorldObjectType enum."""
        self.assertIsNotNone(pynlm.WorldObjectType.Empty)
        self.assertIsNotNone(pynlm.WorldObjectType.Resource)
        self.assertIsNotNone(pynlm.WorldObjectType.Hazard)
        self.assertIsNotNone(pynlm.WorldObjectType.Wall)
        self.assertIsNotNone(pynlm.WorldObjectType.Marker)


class TestIntegration(unittest.TestCase):
    """Integration tests for complete workflows."""
    
    def test_complete_agent_workflow(self):
        """Test a complete agent workflow from start to finish."""
        # 1. Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 300)
        
        # 2. Create brain and initialize
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 3. Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=12, visionHeight=12)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # 4. Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # 5. Run simulation loop
        for step in range(100):
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
            
            # Apply neuromodulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # 6. Verify results
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreaterOrEqual(brain.getFiringNeuronCount(), 0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
        self.assertGreater(agent.getNoveltyLevel(), 0.0)
        
        # 7. Save brain state
        with tempfile.NamedTemporaryFile(delete=False) as f:
            save_file = f.name
        
        try:
            brain.save(save_file)
            
            # Reset and load
            brain.reset()
            brain.initialize()
            brain.load(save_file)
            
            # Verify loaded state
            self.assertEqual(brain.getTotalNeuronCount(), 300)
            
        finally:
            # Clean up
            if os.path.exists(save_file):
                os.unlink(save_file)
    
    def test_multiple_episodes(self):
        """Test running multiple episodes."""
        # Create lifecycle
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 150)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        lifecycle = pynlm.createAgentLifecycle(brain, world, 3, 50)
        
        # Run all episodes
        episodes_run = 0
        while lifecycle.has_next_episode():
            lifecycle.run_next_episode()
            episodes_run += 1
        
        # Should have run all episodes
        self.assertEqual(episodes_run, 3)
        self.assertEqual(lifecycle.getEpisodeProgress(), 1.0)
    
    def test_training_utility(self):
        """Test training utility functionality."""
        # Create brain and agent
        config = pynlm.createDefaultConfig()
        config.set("brain.num_neurons", 100)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=12, height=12, visionWidth=10, visionHeight=10)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create training utility
        training = pynlm.createTrainingUtility(agent, world, 2, 0.2, 0.01)
        
        # Run training
        rewards, lengths = training.train()
        
        # Verify results
        self.assertEqual(len(rewards), 2)
        self.assertEqual(len(lengths), 2)
        self.assertGreaterEqual(lengths[0], 90)  # Close to 100
        
        # Get average reward
        avg_reward = training.get_average_reward(rewards)
        self.assertIsInstance(avg_reward, float)
        self.assertGreaterOrEqual(avg_reward, -10.0)  # Can be negative
        self.assertLessOrEqual(avg_reward, 10.0)


class TestPythonExamples(unittest.TestCase):
    """Test Python example functions."""
    
    def test_example_advanced_training(self):
        """Test advanced training example."""
        result = pynlm.example_advanced_training()
        self.assertIsInstance(result, str)
        self.assertIn("Advanced training example completed", result)
    
    def test_example_visualization(self):
        """Test visualization example."""
        result = pynlm.example_visualization()
        self.assertIsInstance(result, str)
        self.assertIn("Visualization example completed", result)
    
    def test_example_memory_management(self):
        """Test memory management example."""
        result = pynlm.example_memory_management()
        self.assertIsInstance(result, str)
        self.assertIn("Memory management example completed", result)


if __name__ == '__main__':
    unittest.main()
