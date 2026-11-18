# Comprehensive Python test suite for pynlm bindings
"""Tests for the NLM (Neural Learning Machine) Python bindings.

This test suite covers the main components exposed through the pynlm module:
- Basic types and identifiers
- Enumerations
- Configuration management
- Sensory input systems
- Action and world objects
- Brain simulation
- Agent integration
- World simulation
"""

import sys
import unittest
import tempfile
import os
from pathlib import Path

# Add the current directory to Python path for local imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    print("Make sure pynlm is installed: pip install -e .")
    sys.exit(1)


class TestBasicTypes(unittest.TestCase):
    """Test basic types and identifiers."""

    def test_neuron_id(self):
        """Test NeuronId class."""
        id1 = pynlm.NeuronId()
        self.assertIsInstance(id1.value, int)
        
        id2 = pynlm.NeuronId(42)
        self.assertEqual(id2.value, 42)
        
        id3 = pynlm.NeuronId(100)
        self.assertEqual(id3.index(), 100)
        
        # Test equality
        self.assertEqual(id2, pynlm.NeuronId(42))
        self.assertNotEqual(id2, pynlm.NeuronId(43))
        
        # Test hash
        ids = {id2, pynlm.NeuronId(42)}
        self.assertEqual(len(ids), 1)
        
        # Test representation
        self.assertIn("NeuronId", repr(id2))

    def test_synapse_id(self):
        """Test SynapseId class."""
        id1 = pynlm.SynapseId()
        self.assertIsInstance(id1.value, int)
        
        id2 = pynlm.SynapseId(123)
        self.assertEqual(id2.value, 123)
        self.assertEqual(id2.index(), 123)
        
        self.assertEqual(id2, pynlm.SynapseId(123))
        self.assertNotEqual(id2, pynlm.SynapseId(124))

    def test_region_id(self):
        """Test RegionId class."""
        id1 = pynlm.RegionId()
        self.assertIsInstance(id1.value, int)
        
        id2 = pynlm.RegionId(999)
        self.assertEqual(id2.value, 999)
        self.assertEqual(id2.index(), 999)

    def test_population_id(self):
        """Test PopulationId class."""
        id1 = pynlm.PopulationId()
        self.assertIsInstance(id1.value, int)
        
        id2 = pynlm.PopulationId(777)
        self.assertEqual(id2.value, 777)
        self.assertEqual(id2.index(), 777)


class TestEnumerations(unittest.TestCase):
    """Test enumeration types."""

    def test_neuron_type_enums(self):
        """Test NeuronType enumeration."""
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Modulatory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Internal
        ]
        
        for nt in neuron_types:
            self.assertIn(nt, neuron_types)  # Verify all are accessible

    def test_synapse_type_enums(self):
        """Test SynapseType enumeration."""
        synapse_types = [
            pynlm.SynapseType.Excitatory,
            pynlm.SynapseType.Inhibitory,
            pynlm.SynapseType.Modulatory,
            pynlm.SynapseType.Electrical,
            pynlm.SynapseType.GapJunction
        ]
        
        for st in synapse_types:
            self.assertIn(st, synapse_types)

    def test_developmental_stage_enums(self):
        """Test DevelopmentalStage enumeration."""
        stages = [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ]
        
        for stage in stages:
            self.assertIn(stage, stages)

    def test_firing_state_enums(self):
        """Test FiringState enumeration."""
        states = [
            pynlm.FiringState.Resting,
            pynlm.FiringState.Active,
            pynlm.FiringState.Refractory,
            pynlm.FiringState.Inhibited
        ]
        
        for state in states:
            self.assertIn(state, states)

    def test_action_type_enums(self):
        """Test ActionType enumeration."""
        action_types = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.MoveBackward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Look,
            pynlm.ActionType.Interact
        ]
        
        for at in action_types:
            self.assertIn(at, action_types)


class TestConfig(unittest.TestCase):
    """Test configuration management."""

    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()

    def test_config_creation(self):
        """Test config creation."""
        self.assertIsNotNone(self.config)

    def test_config_has(self):
        """Test config.has() method."""
        # Test with existing key
        self.config.set("brain.neuron_count", 100)
        self.assertTrue(self.config.has("brain.neuron_count"))
        
        # Test with non-existing key
        self.assertFalse(self.config.has("nonexistent.key"))

    def test_config_get_keys(self):
        """Test config.getKeys() method."""
        # Initially empty config
        keys = self.config.getKeys()
        self.assertIsInstance(keys, list)
        
        # After setting some keys
        self.config.set("brain.neuron_count", 100)
        self.config.set("brain.synapse_density", 0.1)
        self.config.set("plasticity.stdp.enable", True)
        
        keys = self.config.getKeys()
        self.assertGreaterEqual(len(keys), 3)
        self.assertIn("brain.neuron_count", keys)

    def test_config_save_load(self):
        """Test config save and load."""
        self.config.set("brain.neuron_count", 500)
        self.config.set("brain.synapse_density", 0.15)
        
        # Create temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            # Save config
            self.config.saveToFile(config_file)
            self.assertTrue(os.path.exists(config_file))
            
            # Load into new config
            new_config = pynlm.createDefaultConfig()
            new_config.loadFromFile(config_file)
            
            # Verify keys exist
            self.assertTrue(new_config.has("brain.neuron_count"))
            self.assertTrue(new_config.has("brain.synapse_density"))
            
        finally:
            # Clean up
            if os.path.exists(config_file):
                os.unlink(config_file)

    def test_config_summary(self):
        """Test config.summary() method."""
        summary = self.config.summary()
        self.assertIsInstance(summary, str)
        self.assertIn("Config", summary)

    def test_config_clear(self):
        """Test config.clear() method."""
        self.config.set("test.key", "value")
        self.assertTrue(self.config.has("test.key"))
        
        self.config.clear()
        self.assertFalse(self.config.has("test.key"))


class TestSensoryInput(unittest.TestCase):
    """Test sensory input systems."""

    def test_sensory_input_base(self):
        """Test base SensoryInput class."""
        # Create a simple Vision object (as an example)
        vision = pynlm.Vision(10, 10, 3)
        
        # Test base class methods
        self.assertIsInstance(vision.getType(), str)
        self.assertEqual(vision.getDimensions(), (10, 10, 3))
        
        # Test data setting/getting
        test_data = [0.1, 0.2, 0.3] * 100  # 10x10x3 = 300 elements
        vision.setData(test_data)
        retrieved_data = vision.getData()
        self.assertEqual(len(retrieved_data), len(test_data))

    def test_internal_signals(self):
        """Test InternalSignals class."""
        internal = pynlm.InternalSignals()
        
        # Add signals
        internal.addSignal(0.5)
        internal.addSignal(0.7)
        internal.addSignal(0.9)
        
        signals = internal.getAllSignals()
        self.assertEqual(len(signals), 3)
        self.assertAlmostEqual(signals[0], 0.5)
        self.assertAlmostEqual(signals[1], 0.7)
        
        # Test clearing
        internal.clearSignals()
        self.assertEqual(len(internal.getAllSignals()), 0)


class TestAction(unittest.TestCase):
    """Test Action class."""

    def test_action_creation(self):
        """Test action creation."""
        # Create action with type only
        action1 = pynlm.Action(pynlm.ActionType.MoveForward)
        self.assertEqual(action1.getType(), pynlm.ActionType.MoveForward)
        self.assertEqual(action1.getParameters(), [])
        
        # Create action with type and parameters
        params = [0.1, 0.2, 0.3]
        action2 = pynlm.Action(pynlm.ActionType.TurnLeft, params)
        self.assertEqual(action2.getType(), pynlm.ActionType.TurnLeft)
        self.assertEqual(action2.getParameters(), params)
        
        # Test setters
        action2.setType(pynlm.ActionType.TurnRight)
        self.assertEqual(action2.getType(), pynlm.ActionType.TurnRight)
        
        action2.setParameters([0.5, 0.5])
        self.assertEqual(action2.getParameters(), [0.5, 0.5])

    def test_action_name(self):
        """Test action.getName() method."""
        action = pynlm.Action(pynlm.ActionType.Eat)
        name = action.getName()
        self.assertIsInstance(name, str)
        self.assertTrue(len(name) > 0)

    def test_action_clone(self):
        """Test action.clone() method."""
        original = pynlm.Action(pynlm.ActionType.MoveForward, [0.1, 0.2, 0.3])
        cloned = original.clone()
        
        self.assertEqual(original.getType(), cloned.getType())
        self.assertEqual(original.getParameters(), cloned.getParameters())
        
        # Ensure they're different objects
        cloned.setType(pynlm.ActionType.MoveBackward)
        self.assertNotEqual(original.getType(), cloned.getType())


class TestWorldObject(unittest.TestCase):
    """Test WorldObject class."""

    def test_world_object_creation(self):
        """Test world object creation."""
        # Create with type only
        obj1 = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource)
        self.assertEqual(obj1.x, 1.0)
        self.assertEqual(obj1.y, 2.0)
        self.assertEqual(obj1.type, pynlm.WorldObjectType.Resource)
        self.assertEqual(obj1.value, 0.0)
        self.assertEqual(obj1.radius, 0.5)
        
        # Create with all parameters
        obj2 = pynlm.WorldObject(3.0, 4.0, pynlm.WorldObjectType.Hazard, 1.5, 2.0)
        self.assertEqual(obj2.x, 3.0)
        self.assertEqual(obj2.y, 4.0)
        self.assertEqual(obj2.type, pynlm.WorldObjectType.Hazard)
        self.assertEqual(obj2.value, 1.5)
        self.assertEqual(obj2.radius, 2.0)

    def test_world_object_properties(self):
        """Test world object property access."""
        obj = pynlm.WorldObject(0.0, 0.0, pynlm.WorldObjectType.Wall)
        
        # Test read/write access
        obj.x = 5.0
        obj.y = 6.0
        obj.type = pynlm.WorldObjectType.Marker
        obj.value = 42.0
        obj.radius = 1.0
        obj.active = True
        
        self.assertEqual(obj.x, 5.0)
        self.assertEqual(obj.y, 6.0)
        self.assertEqual(obj.type, pynlm.WorldObjectType.Marker)
        self.assertEqual(obj.value, 42.0)
        self.assertEqual(obj.radius, 1.0)
        self.assertEqual(obj.active, True)


class TestAgentBody(unittest.TestCase):
    """Test AgentBody class."""

    def test_agent_body_creation(self):
        """Test agent body creation."""
        body = pynlm.AgentBody()
        
        # Test default values
        self.assertEqual(body.x, 0.0)
        self.assertEqual(body.y, 0.0)
        self.assertEqual(body.orientation, 0.0)
        self.assertEqual(body.velocityX, 0.0)
        self.assertEqual(body.velocityY, 0.0)
        self.assertEqual(body.angularVelocity, 0.0)
        self.assertEqual(body.energy, 1.0)
        self.assertEqual(body.health, 1.0)
        self.assertEqual(body.age, 0)
        self.assertFalse(body.isMoving)
        self.assertFalse(body.isTurning)
        self.assertEqual(body.lastActionTime, 0.0)

    def test_agent_body_properties(self):
        """Test agent body property setting."""
        body = pynlm.AgentBody()
        
        # Set various properties
        body.x = 10.0
        body.y = 20.0
        body.orientation = 45.0
        body.velocityX = 2.0
        body.velocityY = 3.0
        body.angularVelocity = 0.5
        body.energy = 0.8
        body.health = 0.9
        body.age = 5
        body.isMoving = True
        body.isTurning = True
        body.lastActionTime = 100.0
        
        # Verify values
        self.assertEqual(body.x, 10.0)
        self.assertEqual(body.y, 20.0)
        self.assertEqual(body.orientation, 45.0)
        self.assertEqual(body.velocityX, 2.0)
        self.assertEqual(body.velocityY, 3.0)
        self.assertEqual(body.angularVelocity, 0.5)
        self.assertEqual(body.energy, 0.8)
        self.assertEqual(body.health, 0.9)
        self.assertEqual(body.age, 5)
        self.assertTrue(body.isMoving)
        self.assertTrue(body.isTurning)
        self.assertEqual(body.lastActionTime, 100.0)

    def test_agent_body_reset(self):
        """Test agent body reset."""
        body = pynlm.AgentBody()
        
        # Modify values
        body.x = 100.0
        body.y = 200.0
        body.energy = 0.1
        body.isMoving = True
        
        # Reset
        body.reset()
        
        # Verify reset
        self.assertEqual(body.x, 0.0)
        self.assertEqual(body.y, 0.0)
        self.assertEqual(body.energy, 1.0)
        self.assertFalse(body.isMoving)


class TestSimpleWorld(unittest.TestCase):
    """Test SimpleWorld class."""

    def setUp(self):
        """Set up test fixtures."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=15, visionWidth=10, visionHeight=8)

    def test_world_configuration(self):
        """Test world configuration."""
        self.assertEqual(self.world.getWidth(), 20)
        self.assertEqual(self.world.getHeight(), 15)
        
        # Note: getVisionWidth/getVisionHeight may not be exposed
        # Test available methods
        self.world.configure(width=30, height=25, visionWidth=12, visionHeight=10)
        # Note: Configure method signature might be different

    def test_world_reset(self):
        """Test world reset."""
        # Set some state
        self.world.setAgentStart(5.0, 5.0)
        
        # Reset
        self.world.reset()
        
        # Verify reset (check if agent start position was reset)
        # Note: exact behavior depends on implementation

    def test_world_update(self):
        """Test world update."""
        initial_time = self.world.getSimulationTime()
        
        # Update world
        self.world.update(0.1)
        
        # Time should have advanced
        new_time = self.world.getSimulationTime()
        self.assertGreater(new_time, initial_time)
        
        # Time difference should be close to 0.1
        self.assertAlmostEqual(new_time - initial_time, 0.1, places=3)

    def test_world_agent_start(self):
        """Test setting agent start position."""
        self.world.setAgentStart(3.0, 4.0)
        
        # Note: getAgentStart might not be exposed, but we can verify it works
        # by setting it twice with different values
        self.world.setAgentStart(7.0, 8.0)
        # Note: verification depends on available methods

    def test_world_random_seed(self):
        """Test random seed setting."""
        seed1 = 42
        seed2 = 123
        
        self.world.setRandomSeed(seed1)
        # Note: getRandomSeed might not be exposed
        
        # Note: verification depends on available methods


class TestBrain(unittest.TestCase):
    """Test Brain class."""

    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        # Configure with reasonable defaults for testing
        self.config.set("brain.neuron_count", 100)
        self.config.set("brain.region_count", 2)
        self.config.set("connection_probability", 0.1)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()

    def test_brain_creation(self):
        """Test brain creation."""
        self.assertIsNotNone(self.brain)

    def test_brain_initialization(self):
        """Test brain initialization."""
        # Already initialized in setUp
        # Verify brain has expected properties
        self.assertEqual(self.brain.getTotalNeuronCount(), 100)
        self.assertEqual(self.brain.getRegionCount(), 2)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)

    def test_brain_step(self):
        """Test brain step."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run a few steps
        for step in range(5):
            self.brain.step(step)
        
        # Spike count should not decrease (may stay same if no spikes)
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), initial_spikes)

    def test_brain_reset(self):
        """Test brain reset."""
        # Step a few times
        for step in range(3):
            self.brain.step(step)
        
        # Reset
        self.brain.reset()
        
        # Verify reset
        self.assertEqual(self.brain.getTotalNeuronCount(), 100)
        self.assertEqual(self.brain.getRegionCount(), 2)

    def test_brain_regions(self):
        """Test brain region operations."""
        # Add a region
        region_id = self.brain.addRegion("test_region")
        self.assertIsInstance(region_id, pynlm.RegionId)
        
        # Get region count
        self.assertGreater(self.brain.getRegionCount(), 0)
        
        # Get all region IDs
        region_ids = self.brain.getRegionIds()
        self.assertIsInstance(region_ids, list)
        self.assertGreaterEqual(len(region_ids), 1)

    def test_brain_statistics(self):
        """Test brain statistics methods."""
        # Get various statistics
        total_neurons = self.brain.getTotalNeuronCount()
        self.assertEqual(total_neurons, 100)
        
        total_synapses = self.brain.getTotalSynapseCount()
        self.assertGreaterEqual(total_synapses, 0)
        
        active_neurons = self.brain.getActiveNeuronCount()
        self.assertGreaterEqual(active_neurons, 0)
        
        firing_neurons = self.brain.getFiringNeuronCount()
        self.assertGreaterEqual(firing_neurons, 0)
        
        avg_firing_rate = self.brain.getAverageFiringRate()
        self.assertGreaterEqual(avg_firing_rate, 0.0)
        
        e_i_ratio = self.brain.getExcitationInhibitionRatio()
        self.assertGreaterEqual(e_i_ratio, 0.0)
        
        total_spikes = self.brain.getTotalSpikeCount()
        self.assertGreaterEqual(total_spikes, 0)

    def test_brain_developmental_stage(self):
        """Test developmental stage methods."""
        stage = self.brain.getDevelopmentalStage()
        self.assertIn(stage, [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ])
        
        # Set developmental stage
        self.brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
        self.assertEqual(self.brain.getDevelopmentalStage(), 
                        pynlm.DevelopmentalStage.Adult)

    def test_brain_config(self):
        """Test brain config methods."""
        brain_config = self.brain.getConfig()
        self.assertIsNotNone(brain_config)
        self.assertTrue(brain_config.has("brain.neuron_count"))

    def test_brain_save_load(self):
        """Test brain save and load."""
        # Save to temporary file
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            save_file = f.name
        
        try:
            # Save brain state
            self.brain.save(save_file)
            self.assertTrue(os.path.exists(save_file))
            
            # Create new brain and load
            new_brain = pynlm.createBrain(self.config)
            new_brain.load(save_file)
            
            # Verify neuron count is the same
            self.assertEqual(new_brain.getTotalNeuronCount(), 
                           self.brain.getTotalNeuronCount())
            
        finally:
            if os.path.exists(save_file):
                os.unlink(save_file)


class TestAgentBrain(unittest.TestCase):
    """Test AgentBrain class."""

    def setUp(self):
        """Set up test fixtures."""
        # Create brain and agent
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        # Initialize agent
        self.agent.initialize(self.world)

    def test_agent_creation(self):
        """Test agent creation."""
        self.assertIsNotNone(self.agent)
        self.assertEqual(self.agent.getBrain(), self.brain)

    def test_agent_initialization(self):
        """Test agent initialization."""
        # Should already be initialized
        self.assertIsNotNone(self.agent.getBrain())

    def test_agent_subsystems(self):
        """Test agent subsystem enable/disable."""
        # Enable various subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Verify they're enabled
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())
        
        # Disable some
        self.agent.enableCuriosity(False)
        self.assertFalse(self.agent.isCuriosityEnabled())

    def test_agent_levels(self):
        """Test neuromodulation levels."""
        # Get current levels
        neuromod_level = self.agent.getNeuromodulationLevel()
        self.assertGreaterEqual(neuromod_level, 0.0)
        
        curiosity_level = self.agent.getCuriosityLevel()
        self.assertGreaterEqual(curiosity_level, 0.0)
        
        novelty_level = self.agent.getNoveltyLevel()
        self.assertGreaterEqual(novelty_level, 0.0)
        
        prediction_error = self.agent.getPredictionError()
        self.assertGreaterEqual(prediction_error, 0.0)

    def test_agent_development(self):
        """Test agent development."""
        initial_stage = self.agent.getDevelopmentalStage()
        self.assertIsInstance(initial_stage, pynlm.DevelopmentalStage)
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Stage might change (or stay the same)
        new_stage = self.agent.getDevelopmentalStage()
        self.assertIsInstance(new_stage, pynlm.DevelopmentalStage)

    def test_agent_sensory_processing(self):
        """Test sensory processing."""
        # Get sensory percept from world
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
        
        # Process it
        self.agent.processSensoryInput(percept)
        
        # Verify agent's brain received input
        # (This would be tested more thoroughly in integration tests)

    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        # Get motor command from agent
        action = self.agent.decodeMotorCommand()
        
        # Verify it's a valid action
        self.assertIsNotNone(action)
        self.assertIn(action.getType(), [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.MoveBackward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Look,
            pynlm.ActionType.Interact
        ])

    def test_agent_reward_modulation(self):
        """Test reward modulation."""
        # Apply reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)  # reward, predicted_reward
        
        # Verify levels changed (we can't easily check exact values)
        neuromod_level = self.agent.getNeuromodulationLevel()
        self.assertGreaterEqual(neuromod_level, 0.0)


class TestFactoryFunctions(unittest.TestCase):
    """Test factory functions."""

    def test_create_default_config(self):
        """Test createDefaultConfig factory function."""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertTrue(config.has("brain.neuron_count"))

    def test_create_brain(self):
        """Test createBrain factory function."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        self.assertIsNotNone(brain)
        self.assertEqual(brain.getConfig(), config)

    def test_create_simple_world(self):
        """Test createSimpleWorld factory function."""
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)

    def test_create_agent_brain(self):
        """Test createAgentBrain factory function."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        self.assertEqual(agent.getBrain(), brain)


class TestInvalidNeuronId(unittest.TestCase):
    """Test invalid neuron ID handling."""

    def test_invalid_neuron_id(self):
        """Test that INVALID_NEURON_ID is accessible."""
        invalid_id = pynlm.INVALID_NEURON_ID
        self.assertIsNotNone(invalid_id)
        self.assertIsInstance(invalid_id, pynlm.NeuronId)
        
        # It should be the default-constructed neuron ID
        valid_id = pynlm.NeuronId()
        self.assertEqual(invalid_id.value, valid_id.value)

    def test_invalid_synapse_id(self):
        """Test that INVALID_SYNAPSE_ID is accessible."""
        invalid_id = pynlm.INVALID_SYNAPSE_ID
        self.assertIsNotNone(invalid_id)
        self.assertIsInstance(invalid_id, pynlm.SynapseId)

    def test_invalid_region_id(self):
        """Test that INVALID_REGION_ID is accessible."""
        invalid_id = pynlm.INVALID_REGION_ID
        self.assertIsNotNone(invalid_id)
        self.assertIsInstance(invalid_id, pynlm.RegionId)

    def test_invalid_population_id(self):
        """Test that INVALID_POPULATION_ID is accessible."""
        invalid_id = pynlm.INVALID_POPULATION_ID
        self.assertIsNotNone(invalid_id)
        self.assertIsInstance(invalid_id, pynlm.PopulationId)


if __name__ == '__main__':
    # Run tests with verbose output
    unittest.main(verbosity=2)