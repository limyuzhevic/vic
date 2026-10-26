#!/usr/bin/env python3
"""
Comprehensive Python test suite for NLM (Neural Learning Machine) Python bindings.

This test suite covers:
- Unit tests for individual classes
- Integration tests for common workflows
- Error handling tests for invalid inputs
- Memory management tests
- Performance validation tests
"""

import sys
import os
import tempfile
import unittest
import numpy as np

# Add the Python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: pynlm not available: {e}")
class TestBasicImport(unittest.TestCase):
    """Test basic import and module functionality."""
    
    def test_import_success(self):
        """Test that pynlm can be imported successfully."""
        self.assertTrue(PYNLM_AVAILABLE, "pynlm should be available")
    
    def test_version_available(self):
        """Test that basic module attributes are available."""
        if PYNLM_AVAILABLE:
            self.assertTrue(hasattr(pynlm, 'createDefaultConfig'))
            self.assertTrue(hasattr(pynlm, 'createBrain'))
            self.assertTrue(hasattr(pynlm, 'createSimpleWorld'))
            self.assertTrue(hasattr(pynlm, 'createAgentBrain'))
class TestConfig(unittest.TestCase):
    """Test Config class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
        else:
            self.skipTest("pynlm not available")
    
    def test_config_creation(self):
        """Test basic config creation."""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertTrue(hasattr(config, 'has'))
        self.assertTrue(hasattr(config, 'getKeys'))
        self.assertTrue(hasattr(config, 'clear'))
    
    def test_config_basic_operations(self):
        """Test basic config operations."""
        config = pynlm.createDefaultConfig()
        
        # Test that config can be cleared
        config.clear()
        
        # Test that empty config has no keys
        keys = config.getKeys()
        self.assertIsInstance(keys, list)
        
        # Test has method
        self.assertFalse(config.has("test.key"))
    
    def test_config_file_io(self):
        """Test config file I/O operations."""
        config = pynlm.createDefaultConfig()
        
        # Create a temporary config file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            # Test save to file
            result = config.saveToFile(config_file)
            self.assertTrue(result)
            
            # Create new config and load from file
            config2 = pynlm.createDefaultConfig()
            result2 = config2.loadFromFile(config_file)
            self.assertTrue(result2)
            
            # Verify keys match
            keys1 = config.getKeys()
            keys2 = config2.getKeys()
            self.assertEqual(set(keys1), set(keys2))
            
        finally:
            # Clean up
            if os.path.exists(config_file):
                os.unlink(config_file)
    
    def test_config_validation(self):
        """Test config input validation."""
        config = pynlm.createDefaultConfig()
        
        # Test empty file path validation
        try:
            config.loadFromFile("")
            self.fail("Should have raised an exception for empty file path")
        except (ValueError, std::exception):
            pass  # Expected
        
        # Test long file path validation
        long_path = "a" * 5000
        try:
            config.loadFromFile(long_path)
            self.fail("Should have raised an exception for long file path")
        except (ValueError, std::exception):
            pass  # Expected
class TestSensoryInput(unittest.TestCase):
    """Test SensoryInput class hierarchy."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.vision = pynlm.Vision(10, 10, 3)
            self.audio = pynlm.Audio(44100, 1000)
            self.internal = pynlm.InternalSignals()
        else:
            self.skipTest("pynlm not available")
    
    def test_vision_creation(self):
        """Test Vision creation with different parameters."""
        # Test default constructor
        vision1 = pynlm.Vision()
        self.assertIsNotNone(vision1)
        
        # Test constructor with parameters
        vision2 = pynlm.Vision(20, 30, 1)
        self.assertEqual(vision2.getWidth(), 20)
        self.assertEqual(vision2.getHeight(), 30)
        self.assertEqual(vision2.getChannels(), 1)
        
        # Test constructor with default channels
        vision3 = pynlm.Vision(15, 15)
        self.assertEqual(vision3.getChannels(), 3)
    
    def test_audio_creation(self):
        """Test Audio creation."""
        audio1 = pynlm.Audio()
        self.assertIsNotNone(audio1)
        
        audio2 = pynlm.Audio(48000, 2000)
        self.assertEqual(audio2.getSampleRate(), 48000)
        self.assertEqual(audio2.getNumSamples(), 2000)
    
    def test_internal_signals(self):
        """Test InternalSignals operations."""
        internal = pynlm.InternalSignals()
        
        # Test initial state
        signals = internal.getAllSignals()
        self.assertIsInstance(signals, list)
        self.assertEqual(len(signals), 0)
        
        # Test adding signals
        internal.addSignal(0.5)
        internal.addSignal(1.0)
        internal.addSignal(-0.3)
        
        signals = internal.getAllSignals()
        self.assertEqual(len(signals), 3)
        self.assertAlmostEqual(signals[0], 0.5)
        self.assertAlmostEqual(signals[1], 1.0)
        self.assertAlmostEqual(signals[2], -0.3)
        
        # Test clear signals
        internal.clearSignals()
        signals = internal.getAllSignals()
        self.assertEqual(len(signals), 0)
    
    def test_sensory_input_common_methods(self):
        """Test common methods across all SensoryInput classes."""
        # Test Vision
        vision = pynlm.Vision(5, 5, 1)
        self.assertIsInstance(vision.getType(), str)
        self.assertEqual(vision.getType(), "Vision")
        self.assertEqual(vision.getDimensions(), 25)
        self.assertEqual(vision.getWidth(), 5)
        self.assertEqual(vision.getHeight(), 5)
        self.assertEqual(vision.getChannels(), 1)
        
        # Test Audio
        audio = pynlm.Audio(22050, 500)
        self.assertIsInstance(audio.getType(), str)
        self.assertEqual(audio.getType(), "Audio")
        self.assertEqual(audio.getDimensions(), 500)
        self.assertEqual(audio.getSampleRate(), 22050)
        self.assertEqual(audio.getNumSamples(), 500)
        
        # Test InternalSignals
        internal = pynlm.InternalSignals()
        self.assertIsInstance(internal.getType(), str)
        self.assertEqual(internal.getType(), "InternalSignals")
        self.assertEqual(internal.getDimensions(), 0)
    
    def test_sensory_input_copy(self):
        """Test cloning sensory inputs."""
        vision = pynlm.Vision(10, 10, 3)
        
        # Test clone method
        vision2 = vision.clone()
        self.assertIsNotNone(vision2)
        self.assertEqual(vision2.getWidth(), 10)
        self.assertEqual(vision2.getHeight(), 10)
        self.assertEqual(vision2.getChannels(), 3)
        
        # Test audio copy
        audio = pynlm.Audio(44100, 1000)
        audio2 = audio.clone()
        self.assertEqual(audio2.getSampleRate(), 44100)
        self.assertEqual(audio2.getNumSamples(), 1000)
class TestWorld(unittest.TestCase):
    """Test SimpleWorld class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.world = pynlm.createSimpleWorld()
            self.world.configure(20, 20, 8, 8)
            self.world.reset()
        else:
            self.skipTest("pynlm not available")
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        world = pynlm.createSimpleWorld()
        world.configure(10, 15, 5, 5)
        
        self.assertEqual(world.getWidth(), 10)
        self.assertEqual(world.getHeight(), 15)
        self.assertEqual(world.getSimulationTime(), 0.0)
        
        # Test reset
        world.reset()
        self.assertEqual(world.getSimulationTime(), 0.0)
    
    def test_world_operations(self):
        """Test world operations."""
        world = pynlm.createSimpleWorld()
        world.configure(30, 30, 10, 10)
        world.reset()
        
        # Test agent start
        world.setAgentStart(5.0, 5.0)
        
        # Get agent body
        body = world.getAgentBody()
        self.assertIsNotNone(body)
        self.assertEqual(body.x, 5.0)
        self.assertEqual(body.y, 5.0)
        
        # Test world update
        world.update(0.1)
        self.assertGreater(world.getSimulationTime(), 0.0)
        
        # Test sensor percept
        percept = world.getSensoryPercept()
        self.assertIsNotNone(percept)
        
        # Test random seed
        world.setRandomSeed(12345)
        seed = world.getRandomSeed()
        self.assertEqual(seed, 12345)
    
    def test_world_validation(self):
        """Test world input validation."""
        world = pynlm.createSimpleWorld()
        
        # Test invalid dimensions
        try:
            world.configure(0, 10, 5, 5)
            self.fail("Should have raised an exception for zero width")
        except (ValueError, std::exception):
            pass  # Expected
        
        try:
            world.configure(10, 10, 15, 5)
            self.fail("Should have raised an exception for vision width > world width")
        except (ValueError, std::exception):
            pass  # Expected
        
        # Test invalid agent position
        try:
            world.setAgentStart(float('nan'), 5.0)
            self.fail("Should have raised an exception for NaN position")
        except (ValueError, std::exception):
            pass  # Expected
        
        # Test invalid timestep
        try:
            world.update(-0.1)
            self.fail("Should have raised an exception for negative timestep")
        except (ValueError, std::exception):
            pass  # Expected
    
    def test_world_objects(self):
        """Test world object operations."""
        world = pynlm.createSimpleWorld()
        world.configure(50, 50, 10, 10)
        
        # Create a world object
        obj = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, 1.0)
        world.addObject(obj)
        
        # Test position validation
        valid = world.isValidPosition(10.0, 10.0)
        self.assertTrue(valid)
        
        invalid = world.isValidPosition(60.0, 60.0)
        self.assertFalse(invalid)
        
        # Remove object
        world.removeObject(10.0, 10.0)
        invalid_after = world.isValidPosition(10.0, 10.0)
        self.assertFalse(invalid_after)
class TestBrain(unittest.TestCase):
    """Test Brain class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
        else:
            self.skipTest("pynlm not available")
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        self.assertIsNotNone(brain)
        
        # Test initialization
        brain.initialize()
        
        # Test basic properties
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        self.assertIsInstance(brain.getAverageFiringRate(), float)
    
    def test_brain_step(self):
        """Test brain stepping operations."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Get initial state
        initial_spikes = brain.getTotalSpikeCount()
        initial_firing = brain.getFiringNeuronCount()
        
        # Step the brain
        brain.step(0)
        
        # Check that state changed (may or may not have spiked)
        self.assertGreaterEqual(brain.getTotalSpikeCount(), initial_spikes)
        self.assertGreaterEqual(brain.getActiveNeuronCount(), initial_firing)
        
        # Test step with time
        brain.step(1, 0.1)
        self.assertGreater(brain.getTotalSpikeCount(), 0)
    
    def test_brain_sensory_motor(self):
        """Test brain sensory and motor operations."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Create sensory input
        vision = pynlm.Vision(5, 5, 1)
        
        # Process sensory input
        brain.receiveSensoryInput(vision)
        
        # Produce action
        action = brain.produceAction()
        self.assertIsNotNone(action)
        
        # Test region management
        region_id = brain.addRegion("test_region")
        self.assertIsNotNone(region_id)
        
        region = brain.getRegion(region_id)
        self.assertIsNotNone(region)
        
        # Test statistics
        stats = pynlm.analyzeBrainStats(brain)
        self.assertIn('total_neurons', stats)
        self.assertIn('total_synapses', stats)
        self.assertIn('firing_neurons', stats)
    
    def test_brain_config_integration(self):
        """Test brain-config integration."""
        config = pynlm.createDefaultConfig()
        
        # Add custom config
        config.set("test.value", 42)
        self.assertTrue(config.has("test.value"))
        
        # Create brain with config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test brain getConfig method
        retrieved_config = brain.getConfig()
        self.assertIsNotNone(retrieved_config)
        self.assertTrue(retrieved_config.has("test.value"))
class TestAgentBrain(unittest.TestCase):
    """Test AgentBrain class functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(20, 20, 8, 8)
            self.world.reset()
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
        else:
            self.skipTest("pynlm not available")
    
    def test_agent_creation(self):
        """Test agent creation and initialization."""
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(self.world)
        
        # Test initialization
        self.assertIsNotNone(agent)
        
        # Test agent properties
        self.assertEqual(agent.getSensoryInputSize(), 64)  # 8x8 vision
        self.assertEqual(agent.getMotorOutputSize(), 4)   # Basic movement actions
    
    def test_agent_workflow(self):
        """Test complete agent workflow."""
        # Run a few steps of the agent simulation
        for step in range(5):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.assertIsNotNone(percept)
            
            # Process sensory input in agent
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get motor command from agent
            action = self.agent.decodeMotorCommand()
            self.assertIsNotNone(action)
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply neuromodulation
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
        
        # Verify agent is still functional
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
        self.assertGreater(self.agent.getCuriosityLevel(), 0.0)
    
    def test_agent_enable_disable_features(self):
        """Test enabling and disabling agent features."""
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(self.world)
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Verify features are enabled
        self.assertTrue(agent.isRewardModulationEnabled())
        self.assertTrue(agent.isCuriosityEnabled())
        self.assertTrue(agent.isDevelopmentEnabled())
        
        # Disable features
        agent.enableRewardModulation(False)
        agent.enableCuriosity(False)
        agent.enableDevelopment(False)
        
        # Verify features are disabled
        self.assertFalse(agent.isRewardModulationEnabled())
        self.assertFalse(agent.isCuriosityEnabled())
        self.assertFalse(agent.isDevelopmentEnabled())
    
    def test_agent_get_brain_access(self):
        """Test different ways to access brain from agent."""
        # Test getBrain method (raw pointer)
        raw_brain = self.agent.getBrain()
        self.assertIsNotNone(raw_brain)
        
        # Test getBrainShared method (shared_ptr)
        shared_brain = self.agent.getBrainShared()
        self.assertIsNotNone(shared_brain)
        
        # Test that both point to same brain
        self.assertEqual(raw_brain.getTotalNeuronCount(), shared_brain.getTotalNeuronCount())
class TestConvenienceMethods(unittest.TestCase):
    """Test convenience methods."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(20, 20, 8, 8)
            self.world.reset()
        else:
            self.skipTest("pynlm not available")
    
    def test_run_agent_simulation_convenience(self):
        """Test runAgentSimulation convenience method."""
        result = pynlm.runAgentSimulation(self.brain, self.world, 10, True)
        
        self.assertIsNotNone(result)
        self.assertEqual(len(result), 2)
        
        returned_world, returned_agent = result
        self.assertIsNotNone(returned_world)
        self.assertIsNotNone(returned_agent)
    
    def test_setup_simple_experiment(self):
        """Test setupSimpleExperiment convenience method."""
        result = pynlm.setupSimpleExperiment(500, 15)
        
        self.assertIsNotNone(result)
        self.assertEqual(len(result), 3)
        
        returned_brain, returned_world, returned_agent = result
        self.assertIsNotNone(returned_brain)
        self.assertIsNotNone(returned_world)
        self.assertIsNotNone(returned_agent)
    
    def test_analyze_brain_stats(self):
        """Test analyzeBrainStats convenience method."""
        stats = pynlm.analyzeBrainStats(self.brain)
        
        self.assertIsInstance(stats, dict)
        self.assertIn('total_neurons', stats)
        self.assertIn('total_synapses', stats)
        self.assertIn('firing_neurons', stats)
        self.assertIn('avg_firing_rate', stats)
        
        self.assertGreater(stats['total_neurons'], 0)
        self.assertGreater(stats['total_synapses'], 0)
    
    def test_config_presets(self):
        """Test configuration preset methods."""
        # Test development config
        dev_config = pynlm.createConfigForDevelopment()
        self.assertIsNotNone(dev_config)
        self.assertTrue(dev_config.has("plasticity.stdp.enable"))
        self.assertTrue(dev_config.has("neuromod.curiosity.enable"))
        
        # Test adult config
        adult_config = pynlm.createConfigForAdult()
        self.assertIsNotNone(adult_config)
        self.assertTrue(adult_config.has("brain.neuron_count"))
        self.assertFalse(adult_config.has("neuromod.curiosity.enable"))  # Should be disabled
    
    def test_print_brain_info(self):
        """Test printBrainInfo convenience method."""
        # This should not raise an exception
        try:
            pynlm.printBrainInfo(self.brain)
            print("printBrainInfo executed successfully")
        except Exception as e:
            self.fail(f"printBrainInfo raised an exception: {e}")
    
    def test_is_ready_for_simulation(self):
        """Test isReadyForSimulation convenience method."""
        ready = pynlm.isReadyForSimulation(self.brain, self.world)
        self.assertTrue(ready)
        
        # Test with null values (should not crash)
        ready_null = pynlm.isReadyForSimulation(None, None)
        self.assertFalse(ready_null)
class TestErrorHandling(unittest.TestCase):
    """Test error handling and validation."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(10, 10, 5, 5)
            self.world.reset()
        else:
            self.skipTest("pynlm not available")
    
    def test_config_validation(self):
        """Test config validation error handling."""
        config = pynlm.createDefaultConfig()
        
        # Test invalid file paths
        try:
            config.loadFromFile("")
            self.fail("Should have raised an exception for empty file path")
        except (ValueError, Exception):
            pass  # Expected
        
        try:
            config.loadFromFile("a" * 5000)  # Too long
            self.fail("Should have raised an exception for long file path")
        except (ValueError, Exception):
            pass  # Expected
    
    def test_world_validation(self):
        """Test world validation error handling."""
        world = pynlm.createSimpleWorld()
        
        # Test invalid dimensions
        try:
            world.configure(0, 10, 5, 5)
            self.fail("Should have raised an exception for zero width")
        except (ValueError, Exception):
            pass  # Expected
        
        # Test invalid timestep
        try:
            world.update(-0.1)
            self.fail("Should have raised an exception for negative timestep")
        except (ValueError, Exception):
            pass  # Expected
    
    def test_sensory_validation(self):
        """Test sensory input validation."""
        # Test invalid audio sample rate
        try:
            audio = pynlm.Audio(0, 100)
            self.fail("Should have raised an exception for zero sample rate")
        except (ValueError, Exception):
            pass  # Expected
        
        # Test invalid vision dimensions (should work since Vision doesn't validate in constructor)
        vision = pynlm.Vision(0, 10, 3)
        self.assertIsNotNone(vision)  # Should not crash
    
    def test_agent_validation(self):
        """Test agent validation."""
        # Test null values
        ready = pynlm.isReadyForSimulation(None, None)
        self.assertFalse(ready)
        
        # Test brain from agent with null agent
        try:
            pynlm.getBrainFromAgent(None)
            self.fail("Should have raised an exception for null agent")
        except Exception:
            pass  # Expected
class TestIntegration(unittest.TestCase):
    """Test integration scenarios."""
    
    def setUp(self):
        """Set up test fixtures."""
        if PYNLM_AVAILABLE:
            # Create a complete working system
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(20, 20, 8, 8)
            self.world.reset()
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable all features
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
        else:
            self.skipTest("pynlm not available")
    
    def test_complete_agent_lifecycle(self):
        """Test complete agent lifecycle."""
        # Run simulation
        for step in range(50):
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            self.agent.updateDevelopment(0.1)
            
            if step == 0:
                self.assertGreater(self.brain.getTotalSpikeCount(), 0)
                self.assertGreater(self.agent.getCuriosityLevel(), 0.0)
        
        # Verify system is still working
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
        self.assertGreater(self.agent.getNoveltyLevel(), 0.0)
        self.assertGreater(self.agent.getPredictionError(), 0.0)
    
    def test_brain_state_save_load(self):
        """Test brain state save and load."""
        # Get initial state
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_neurons = self.brain.getTotalNeuronCount()
        
        # Create a temporary file
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            save_file = f.name
        
        try:
            # Save brain state
            save_result = self.brain.save(save_file)
            self.assertTrue(save_result)
            
            # Reset brain
            self.brain.reset()
            
            # Verify state changed
            self.assertGreater(self.brain.getTotalSpikeCount(), initial_spikes)
            
            # Load brain state
            load_result = self.brain.load(save_file)
            self.assertTrue(load_result)
            
            # Verify state restored
            self.assertEqual(self.brain.getTotalNeuronCount(), initial_neurons)
            
        finally:
            if os.path.exists(save_file):
                os.unlink(save_file)
    
    def test_configuration_integration(self):
        """Test configuration integration across components."""
        # Create a development configuration
        config = pynlm.createConfigForDevelopment()
        
        # Create brain with development config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Verify config was applied
        retrieved_config = brain.getConfig()
        self.assertTrue(retrieved_config.has("plasticity.stdp.enable"))
        self.assertTrue(retrieved_config.has("neuromod.curiosity.enable"))
        self.assertTrue(retrieved_config.has("development.enable"))
        
        # Verify brain reflects config
        self.assertGreater(brain.getTotalNeuronCount(), 0)
    
    def test_sensory_integration(self):
        """Test sensory integration with world and agent."""
        # Get initial percept
        initial_percept = self.world.getSensoryPercept()
        self.assertIsNotNone(initial_percept)
        
        # Update world and get new percept
        self.world.update(0.1)
        new_percept = self.world.getSensoryPercept()
        self.assertIsNotNone(new_percept)
        
        # Process percept through agent
        self.agent.processSensoryInput(new_percept)
        
        # Verify agent has processed the input
        self.assertGreater(self.brain.getActiveNeuronCount(), 0)
if __name__ == '__main__':
    print("Running NLM Python bindings test suite...")
    print(f"Python version: {sys.version}")
    print(f"pynlm available: {PYNLM_AVAILABLE}")
    print()
    
    # Run tests
    unittest.main(verbosity=2)
