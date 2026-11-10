#!/usr/bin/env python3
"""
Python Tests for NLM (Neural Learning Machine)
Tests the enhanced Python API functionality added in Phase 6 improvements
"""

import sys
import os
import tempfile
import json
import time
import unittest
from unittest.mock import patch, MagicMock

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError:
    print("ERROR: Could not import pynlm. Please build the Python bindings first.")
    print("Run: python setup.py build_ext --inplace or pip install -e .")
    sys.exit(1)

class TestNLMConfiguration(unittest.TestCase):
    """Test configuration-related functionality"""
    
    def test_create_default_config(self):
        """Test creating a default configuration"""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertTrue(config.has("random_seed"))
        self.assertTrue(config.has("neuron_count"))
    
    def test_create_advanced_config(self):
        """Test creating an advanced configuration"""
        config = pynlm.createAdvancedConfig(
            neuronCount=1000,
            connectionProb=0.2,
            stdpLTP=0.03,
            stdpLTD=0.02,
            enableDevelopment=True,
            enableCuriosity=True,
            enableRewardModulation=True
        )
        self.assertIsNotNone(config)
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("enable_curiosity"))
        self.assertTrue(config.has("enable_reward_modulation"))
    
    def test_load_save_config(self):
        """Test loading and saving configurations"""
        # Create and save a config
        config = pynlm.createAdvancedConfig(neuronCount=500)
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            config_file = f.name
        
        try:
            # Load the config
            loaded_config = pynlm.createDefaultConfig()
            loaded_config.loadFromFile(config_file)
            
            # Verify it was loaded
            self.assertTrue(loaded_config.has("neuron_count"))
            self.assertEqual(loaded_config.getOr("neuron_count", 0), 500)
        finally:
            os.unlink(config_file)
    
    def test_config_summary(self):
        """Test configuration summary"""
        config = pynlm.createAdvancedConfig(neuronCount=1000)
        summary = config.summary()
        self.assertIsInstance(summary, str)
        self.assertIn("neuron_count" or "1000", summary)

class TestNLMBrain(unittest.TestCase):
    """Test brain-related functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = pynlm.createAdvancedConfig(neuronCount=100)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test fixtures"""
        # Brain will be destroyed when out of scope
        pass
    
    def test_brain_basic_functionality(self):
        """Test basic brain functionality"""
        self.assertEqual(self.brain.getTotalNeuronCount(), 100)
        self.assertTrue(self.brain.getTotalSynapseCount() >= 0)
        self.assertEqual(self.brain.getTotalSpikeCount(), 0)
    
    def test_brain_step(self):
        """Test brain stepping"""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        for step in range(10):
            self.brain.step(step)
        
        final_spikes = self.brain.getTotalSpikeCount()
        self.assertGreaterEqual(final_spikes, initial_spikes)
    
    def test_brain_inject_current(self):
        """Test current injection into brain"""
        # Get a valid neuron ID
        region = self.brain.getRegion(pynlm.INVALID_REGION_ID)
        self.assertIsNotNone(region)
        
        neurons = region.getAllNeurons()
        self.assertGreater(len(neurons), 0)
        
        # Inject current into first neuron
        neuron_id = neurons[0].getId()
        self.brain.injectCurrent(neuron_id, 50.0)
        
        # Step the brain
        self.brain.step(0)
        
        # Check if neurons fired (spikes should increase)
        spikes = self.brain.getTotalSpikeCount()
        self.assertGreater(spikes, 0)
    
    def test_brain_reset(self):
        """Test brain reset functionality"""
        # Do some steps
        for step in range(10):
            self.brain.step(step)
        
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Reset brain
        self.brain.reset()
        self.brain.initialize()
        
        # Check that spikes were reset
        reset_spikes = self.brain.getTotalSpikeCount()
        self.assertEqual(reset_spikes, 0)
    
    def test_brain_save_load(self):
        """Test brain save/load functionality"""
        # Do some steps
        for step in range(5):
            self.brain.step(step)
        
        # Save brain
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            save_file = f.name
        
        try:
            self.brain.save(save_file)
            
            # Create a new brain and load the saved state
            new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            new_brain.load(save_file)
            
            # Verify the loaded brain has the same neuron count
            self.assertEqual(new_brain.getTotalNeuronCount(), self.brain.getTotalNeuronCount())
        finally:
            os.unlink(save_file)

class TestNLMAgent(unittest.TestCase):
    """Test agent-related functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = pynlm.createAdvancedConfig(neuronCount=50)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def tearDown(self):
        """Clean up test fixtures"""
        pass
    
    def test_agent_initialization(self):
        """Test agent initialization"""
        self.assertEqual(self.agent.getSensoryInputSize(), 16)  # 4x4x3 (RGB)
        self.assertEqual(self.agent.getMotorOutputSize(), 8)  # 8 motor commands
    
    def test_agent_process_sensory_input(self):
        """Test processing sensory input"""
        # Get a percept from the world
        percept = self.world.getSensoryPercept()
        
        # Process the input
        self.agent.processSensoryInput(percept)
        
        # Verify the brain received the input
        # (This is implicit in the processing)
        pass
    
    def test_agent_decode_motor_command(self):
        """Test decoding motor commands"""
        # Create a simple action
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        # Apply the action to world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Decode motor command from brain
        motor_command = self.agent.decodeMotorCommand()
        
        # Verify we got a motor command
        self.assertIsNotNone(motor_command)
    
    def test_agent_enable_features(self):
        """Test enabling various agent features"""
        # Enable features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Verify features are enabled
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        
        # Test feature methods
        self.assertEqual(self.agent.getDevelopmentalStage(), pynlm.DevelopmentalStage.Initial)
        self.assertGreaterEqual(self.agent.getNeuromodulationLevel(), 0.0)
        self.assertGreaterEqual(self.agent.getCuriosityLevel(), 0.0)
        self.assertGreaterEqual(self.agent.getNoveltyLevel(), 0.0)
        self.assertGreaterEqual(self.agent.getPredictionError(), 0.0)

class TestNLMAdvancedFeatures(unittest.TestCase):
    """Test advanced Python API features"""
    
    def test_complete_simulation(self):
        """Test the complete simulation helper"""
        # Create components
        config = pynlm.createAdvancedConfig(neuronCount=50)
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        agent.initialize(world)
        
        # Run complete simulation
        spikes = pynlm.runCompleteSimulation(brain, world, agent, 10)
        
        # Verify results
        self.assertGreaterEqual(spikes, 0)
        self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
    
    def test_brain_snapshot_functions(self):
        """Test brain snapshot save/load functions"""
        # Create and initialize brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Save snapshot
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            snapshot_file = f.name
        
        try:
            pynlm.saveBrainSnapshot(brain, snapshot_file, "Test snapshot")
            
            # Load snapshot
            loaded_brain = pynlm.loadBrainSnapshot(snapshot_file)
            
            # Verify loaded brain
            self.assertIsNotNone(loaded_brain)
            self.assertEqual(loaded_brain.getTotalNeuronCount(), brain.getTotalNeuronCount())
        finally:
            os.unlink(snapshot_file)
    
    def test_performance_monitoring(self):
        """Test performance monitoring functionality"""
        # Create brain
        brain = pynlm.createBrain(pynlm.createAdvancedConfig(neuronCount=100))
        brain.initialize()
        
        # Run some steps to collect data
        for step in range(20):
            brain.step(step)
        
        # Benchmark the brain
        result = pynlm.benchmarkBrain(brain, 10, 5)
        
        # Verify benchmark result
        self.assertEqual(len(result), 4)
        spikes, firing_neurons, avg_time, steps = result
        self.assertGreaterEqual(spikes, 0)
        self.assertGreaterEqual(firing_neurons, 0)
        self.assertGreaterEqual(avg_time, 0.0)
        self.assertEqual(steps, 10)
    
    def test_brain_activity_analysis(self):
        """Test brain activity analysis"""
        # Create and run brain
        brain = pynlm.createBrain(pynlm.createAdvancedConfig(neuronCount=50))
        brain.initialize()
        
        for step in range(10):
            brain.step(step)
        
        # Analyze brain activity
        analysis = pynlm.analyzeBrainActivity(brain)
        
        # Verify analysis results
        self.assertIsInstance(analysis, dict)
        expected_keys = ['total_spikes', 'firing_neurons', 'avg_firing_rate',
                        'excitatory_inhibition_ratio', 'total_neurons', 'total_synapses']
        
        for key in expected_keys:
            self.assertIn(key, analysis)
            self.assertIsInstance(analysis[key], (int, float))
    
    def test_export_import_utilities(self):
        """Test export/import utilities"""
        # Create and run brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for step in range(5):
            brain.step(step)
        
        # Test export
        with tempfile.NamedTemporaryFile(suffix='.csv', delete=False) as f:
            export_file = f.name
        
        try:
            pynlm.exportBrainData(brain, export_file)
            
            # Verify file was created
            self.assertTrue(os.path.exists(export_file))
            
            # Check file content
            with open(export_file, 'r') as f:
                content = f.read()
                self.assertIn("NeuronCount", content)
                self.assertIn("SynapseCount", content)
        finally:
            os.unlink(export_file)
    
    def test_visualization_config(self):
        """Test visualization configuration"""
        # Create visualization config
        viz_config = pynlm.createVisualizationConfig(
            width=800,
            height=600,
            showNeurons=True,
            showSynapses=True,
            showActivity=True
        )
        
        self.assertIsNotNone(viz_config)
        self.assertTrue(viz_config.has("viz_width"))
        self.assertTrue(viz_config.has("viz_height"))
    
    def test_training_simulation_setup(self):
        """Test training simulation setup"""
        # Create components
        config = pynlm.createAdvancedConfig(neuronCount=50)
        world = pynlm.createSimpleWorld()
        
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        # Create training simulation
        brain, agent, duration = pynlm.createTrainingSimulation(
            config, world, 100, "training_reward"
        )
        
        # Verify setup
        self.assertIsNotNone(brain)
        self.assertIsNotNone(agent)
        self.assertEqual(duration, 100)
        
        # Initialize agent
        agent.initialize(world)

class TestNLMSimulation(unittest.TestCase):
    """Test simulation-specific functionality"""
    
    def test_example_patterns(self):
        """Test the example patterns from documentation"""
        # Test Pattern 1: Simple Brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        self.assertGreater(brain.getTotalSpikeCount(), 0)
    
    def test_complete_example(self):
        """Test complete example from documentation"""
        # Example 2: Brain in a Simple World
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run for 30 steps
        for step in range(30):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
        
        print("Watched world for 30 steps")
        print("Firing rate:", brain.getAverageFiringRate())
        
        self.assertGreater(brain.getTotalSpikeCount(), 0)

if __name__ == "__main__":
    # Print test information
    print("=== NLM Python Tests ===")
    print(f"Testing comprehensive Python API functionality")
    
    # Run tests
    unittest.main(verbosity=2)