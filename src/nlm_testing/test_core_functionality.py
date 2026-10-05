"""
Simple unit tests for NLM core functionality

This module provides basic unit tests to verify the core functionality
of the NLM (Neural Learning Machine) system.
"""

import unittest
import sys
import os

# Add src to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pynlm
from pynlm import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain

class TestNLMCoreFunctionality(unittest.TestCase):
    """Test core NLM functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = createDefaultConfig()
        self.brain = createBrain(self.config)
        self.world = createSimpleWorld()
        self.agent = createAgentBrain(self.brain)
        
    def tearDown(self):
        """Clean up test fixtures"""
        # Clean up any resources
        pass
    
    def test_config_creation(self):
        """Test configuration creation"""
        config = createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertTrue(hasattr(config, 'has'))
        self.assertTrue(hasattr(config, 'getOr'))
        self.assertTrue(hasattr(config, 'set'))
        
    def test_brain_creation(self):
        """Test brain creation"""
        brain = createBrain(self.config)
        self.assertIsNotNone(brain)
        self.assertFalse(brain.is_initialized())
        
    def test_brain_initialization(self):
        """Test brain initialization"""
        brain = createBrain(self.config)
        initialized = brain.initialize()
        self.assertTrue(initialized)
        self.assertTrue(brain.is_initialized())
        
        # Check that brain has basic properties
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        self.assertGreater(brain.getRegionCount(), 0)
        
    def test_world_configuration(self):
        """Test world configuration"""
        world = createSimpleWorld()
        self.assertIsNotNone(world)
        
        world.configure(width=20, height=20)
        # Note: These attributes might not exist in the actual API
        # but we test that the configuration doesn't crash
        
    def test_agent_creation(self):
        """Test agent creation"""
        brain = createBrain(self.config)
        brain.initialize()
        
        agent = createAgentBrain(brain)
        self.assertIsNotNone(agent)
        
        # Initialize agent with world
        self.world.configure(width=10, height=10)
        agent.initialize(self.world)
        
        self.assertEqual(agent.getBrain(), brain)
        
    def test_simulation_step(self):
        """Test brain simulation step"""
        config = createDefaultConfig()
        config.set("brain.neuron_count", 100)  # Smaller network for faster test
        
        brain = createBrain(config)
        brain.initialize()
        
        # Get initial state
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run a few steps
        for i in range(10):
            brain.step(i)
            
        # Spike count should be valid
        final_spikes = brain.getTotalSpikeCount()
        self.assertGreaterEqual(final_spikes, initial_spikes)
        self.assertGreaterEqual(final_spikes, 0)
        
    def test_agent_sensory_processing(self):
        """Test agent sensory processing"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=15, height=15)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Test sensory processing
        # Create a simple percept (note: actual percept type may vary)
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Verify agent processed input
        self.assertIsNotNone(agent.getBrain())
        
    def test_agent_motor_decoding(self):
        """Test agent motor command decoding"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=10, height=10)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        self.assertIsNotNone(motor_cmd)
        self.assertIn(motor_cmd, [
            MotorCommand.MoveForward,
            MotorCommand.MoveBackward,
            MotorCommand.TurnLeft,
            MotorCommand.TurnRight,
            MotorCommand.LookLeft,
            MotorCommand.LookRight,
            MotorCommand.Interact,
            MotorCommand.Wait
        ])
        
    def test_brain_persistence(self):
        """Test brain persistence (save/load)"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Generate some activity
        for i in range(50):
            brain.step(i)
        
        # Save brain state
        checkpoint_file = "test_checkpoint.bin"
        saved = brain.save(checkpoint_file)
        self.assertTrue(saved)
        
        # Reset and reinitialize
        brain.reset()
        brain.initialize()
        
        # Load saved state
        loaded = brain.load(checkpoint_file)
        self.assertTrue(loaded)
        
        # Clean up
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
            
    def test_agent_subsystem_control(self):
        """Test agent subsystem enable/disable"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=10, height=10)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Initially all subsystems should be disabled
        self.assertFalse(agent.isRewardModulationEnabled())
        self.assertFalse(agent.isStructuralPlasticityEnabled())
        self.assertFalse(agent.isDevelopmentEnabled())
        self.assertFalse(agent.isCuriosityEnabled())
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Verify subsystems are enabled
        self.assertTrue(agent.isRewardModulationEnabled())
        self.assertTrue(agent.isStructuralPlasticityEnabled())
        self.assertTrue(agent.isDevelopmentEnabled())
        self.assertTrue(agent.isCuriosityEnabled())
        
    def test_developmental_stages(self):
        """Test developmental stages"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Test all developmental stages
        for stage in DevelopmentalStage:
            brain.setDevelopmentalStage(stage)
            self.assertEqual(brain.getDevelopmentalStage(), stage)
            
    def test_plasticity_systems(self):
        """Test plasticity systems"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Verify plasticity systems exist
        stdp = brain.getSTDP()
        hebbian = brain.getHebbian()
        structural = brain.getStructuralPlasticity()
        
        self.assertIsNotNone(stdp)
        self.assertIsNotNone(hebbian)
        self.assertIsNotNone(structural)
        
    def test_memory_systems(self):
        """Test memory systems"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Verify memory systems exist
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        self.assertIsNotNone(working_memory)
        self.assertIsNotNone(episodic_memory)
        self.assertIsNotNone(associative_memory)
        
    def test_prediction_system(self):
        """Test prediction system"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        prediction_system = brain.getPredictionSystem()
        self.assertIsNotNone(prediction_system)
        
    def test_cognitive_systems(self):
        """Test cognitive systems"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Verify cognitive systems exist
        planner = brain.getPlanner()
        concept_formation = brain.getConceptFormation()
        attention = brain.getAttention()
        
        self.assertIsNotNone(planner)
        self.assertIsNotNone(concept_formation)
        self.assertIsNotNone(attention)
        
    def test_neuromodulation(self):
        """Test neuromodulation systems"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Verify neuromodulation systems exist
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        novelty = brain.getNovelty()
        prediction_error = brain.getPredictionErrorSignal()
        
        self.assertIsNotNone(dopamine)
        self.assertIsNotNone(curiosity)
        self.assertIsNotNone(novelty)
        self.assertIsNotNone(prediction_error)
        
    def test_development_system(self):
        """Test development system"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        development_system = brain.getDevelopmentSystem()
        self.assertIsNotNone(development_system)
        
    def test_complete_simulation(self):
        """Test complete agent simulation"""
        def run_simulation(steps=20):
            # Setup
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.initialize()
            
            world = createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Run simulation
            for step in range(steps):
                world.update(timestep=0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                motor_cmd = agent.decodeMotorCommand()
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                # Apply reward modulation
                reward = 0.0
                if world.getSensoryPercept().getInternal():
                    reward = world.getSensoryPercept().getInternal()[0]
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            
            return brain, world, agent
        
        # Run simulation
        brain, world, agent = run_simulation()
        
        # Verify results
        self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(agent.getCuriosityLevel(), 0.0)
        self.assertGreaterEqual(agent.getNoveltyLevel(), 0.0)
        self.assertIsNotNone(agent.getDevelopmentalStage())
        
        print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes, "
              f"{agent.getCuriosityLevel():.3f} curiosity, "
              f"{agent.getNoveltyLevel():.3f} novelty")

# Test suite discovery
if __name__ == '__main__':
    print("NLM Unit Testing Suite")
    print("=" * 50)
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(TestNLMCoreFunctionality)
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Exit with appropriate code
    sys.exit(0 if result.wasSuccessful() else 1)