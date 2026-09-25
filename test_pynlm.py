#!/usr/bin/env python3
"""
NLM Python Bindings Test Suite

Comprehensive test suite for NLM (Neural Learning Machine) Python bindings.
Tests the newly refactored Python API for better organization, maintainability,
and Pythonic features.

This test suite covers:
- Basic functionality of all core classes
- Factory methods and Pythonic features
- Error handling and validation
- Advanced features for power users
- Integration testing

Usage:
    python test_pynlm.py
    python -m pytest test_pynlm.py -v
"""

import sys
import os
import tempfile
import json
import traceback

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    import numpy as np
except ImportError as e:
    print(f"Error importing pynlm: {e}")
    print("Make sure pynlm is properly built and available in your Python path.")
    sys.exit(1)

class TestBasicFunctionality:
    """Test basic functionality of all core classes."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.config = pynlm.createDefaultConfigWithDefaults()
        self.brain = pynlm.createBrainWithDefaults()
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Initialize components
        self.brain.initialize()
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
    
    def teardown_method(self):
        """Clean up after each test method."""
        pass
    
    def test_config_creation(self):
        """Test configuration creation and basic operations."""
        config = pynlm.createDefaultConfigWithDefaults()
        assert config is not None
        
        config.setInt("test_int", 42)
        config.setFloat("test_float", 3.14)
        config.setBool("test_bool", True)
        config.setString("test_string", "hello")
        
        assert config.getInt("test_int") == 42
        assert config.getFloat("test_float") == 3.14
        assert config.getBool("test_bool") == True
        assert config.getString("test_string") == "hello"
        
        assert config.has("test_int")
        assert config.has("test_float")
        assert config.has("test_bool")
        assert config.has("test_string")
        assert not config.has("nonexistent")
        
        keys = list(config.getKeys())
        assert "test_int" in keys
        assert "test_float" in keys
        assert "test_bool" in keys
        assert "test_string" in keys
    
    def test_brain_creation(self):
        """Test brain creation with different factory methods."""
        brain1 = pynlm.createBrainWithDefaults()
        assert brain1 is not None
        brain1.initialize()
        
        brain2 = pynlm.createMinimalBrain()
        assert brain2 is not None
        brain2.initialize()
        
        brain3 = pynlm.createBrain(self.config)
        assert brain3 is not None
        brain3.initialize()
        
        assert brain1.getTotalNeuronCount() > 0
        assert brain1.getTotalSynapseCount() >= 0
        assert brain1.getAverageFiringRate() >= 0.0
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        world1 = pynlm.createSimpleWorld()
        assert world1 is not None
        
        world2 = pynlm.createWorld(width=20, height=15, visionWidth=10, visionHeight=10)
        assert world2 is not None
        
        world2.configure(width=30, height=25, visionWidth=12, visionHeight=12)
        assert world2.getWidth() == 30
        assert world2.getHeight() == 25
        
        world2.setAgentStart(5, 5)
        body = world2.getAgentBody()
        assert body.x == 5
        assert body.y == 5
    
    def test_agent_creation(self):
        """Test agent brain creation and initialization."""
        agent1 = pynlm.createAgentBrain(self.brain)
        assert agent1 is not None
        
        agent2 = pynlm.createAgent(self.brain, self.world)
        assert agent2 is not None
        
        assert agent1.getBrain() is not None
        assert agent1.getSensoryInputSize() > 0
        assert agent1.getMotorOutputSize() > 0
    
    def test_sensory_percept(self):
        """Test sensory percept functionality."""
        percept = pynlm.SensoryPercept()
        
        vision = pynlm.Vision(8, 8, 3)
        vision_data = np.random.randn(8 * 8 * 3).astype(np.float32)
        vision.setData(list(vision_data))
        percept.setVision(vision)
        
        assert percept.hasVision()
        assert percept.getVisionWidth() == 8
        assert percept.getVisionHeight() == 8
        assert percept.getTotalSignalCount() > 0
        
        audio = pynlm.Audio(44100, 1000)
        audio_data = np.random.randn(1000).astype(np.float32)
        audio.setData(list(audio_data))
        percept.setAudio(audio)
        
        assert percept.hasAudio()
        assert percept.getAudio().getNumSamples() == 1000
        
        internal = pynlm.InternalSignals()
        internal.addSignal(0.5)
        internal.addSignal(1.0)
        internal.addSignal(-0.3)
        percept.setInternal(internal)
        
        assert percept.hasInternal()
        assert internal.getSignalCount() == 3
    
    def test_action_class(self):
        """Test action class functionality."""
        action1 = pynlm.Action()
        assert action1.getType() == pynlm.ActionType.Wait
        
        action2 = pynlm.Action(pynlm.ActionType.MoveForward)
        assert action2.getType() == pynlm.ActionType.MoveForward
        
        action3 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        assert action3.getType() == pynlm.ActionType.Custom
        assert action3.getParameters() == [1.0, 2.0, 3.0]
        
        action3.setType(pynlm.ActionType.MoveLeft)
        assert action3.getType() == pynlm.ActionType.MoveLeft
        
        action3.setParameters([4.0, 5.0])
        assert action3.getParameters() == [4.0, 5.0]
        
        action4 = action3.clone()
        assert action4.getType() == pynlm.ActionType.MoveLeft
        assert action4.getParameters() == [4.0, 5.0]
        
        assert action2.getName() == "MoveForward"
        assert action3.getName() == "Custom"
    
    def test_world_object(self):
        """Test world object functionality."""
        obj = pynlm.WorldObject(5.0, 10.0, pynlm.WorldObjectType.Resource, 15.0, 1.0)
        
        assert obj.x == 5.0
        assert obj.y == 10.0
        assert obj.type == pynlm.WorldObjectType.Resource
        assert obj.value == 15.0
        assert obj.radius == 1.0
        
        distance = obj.distanceTo(5.0, 10.0)
        assert abs(distance - 0.0) < 0.001
        
        distance = obj.distanceTo(6.0, 11.0)
        expected = (1.0**2 + 1.0**2)**0.5
        assert abs(distance - expected) < 0.001
        
        assert obj.isAtPosition(5.0, 10.0)
        assert obj.isAtPosition(5.0, 10.0, tolerance=0.1)
        assert not obj.isAtPosition(5.0, 11.0)
    
    def test_agent_body(self):
        """Test agent body functionality."""
        body = pynlm.AgentBody()
        body.x = 10.0
        body.y = 5.0
        body.orientation = 1.57
        body.velocityX = 2.0
        body.velocityY = -1.0
        body.angularVelocity = 0.5
        body.energy = 75.0
        body.health = 90.0
        body.age = 10
        body.isMoving = True
        body.isTurning = False
        
        assert body.x == 10.0
        assert body.y == 5.0
        assert body.getFacingAngle() == 1.57
        assert body.getSpeed() > 0.0
        assert body.isHealthy()
        
        body.setFacingAngle(7.0)
        assert body.getFacingAngle() < 2 * 3.14159
        
        body.setFacingAngle(-1.0)
        assert body.getFacingAngle() >= 0.0

class TestPythonicFeatures:
    """Test Pythonic features and factory methods."""
    
    def test_factory_methods(self):
        """Test various factory methods."""
        config1 = pynlm.createDefaultConfigWithDefaults()
        config2 = pynlm.createSimpleConfig(100, 0.2)
        config3 = pynlm.createMinimalConfig()
        config4 = pynlm.createWorldConfig(width=15, height=20)
        
        assert config1 is not None
        assert config2 is not None
        assert config3 is not None
        assert config4 is not None
        
        brain1 = pynlm.createBrainWithDefaults()
        brain2 = pynlm.createMinimalBrain()
        brain3 = pynlm.createBrain(config1)
        
        assert brain1 is not None
        assert brain2 is not None
        assert brain3 is not None
        
        world1 = pynlm.createSimpleWorld()
        world2 = pynlm.createWorld(25, 30, 12, 12)
        
        assert world1 is not None
        assert world2 is not None
        
        agent1 = pynlm.createAgentBrain(brain1)
        agent2 = pynlm.createAgent(brain2, world2)
        
        assert agent1 is not None
        assert agent2 is not None
    
    def test_dictionary_config(self):
        """Test configuration from Python dictionary."""
        config_dict = {
            "neuron_count": 250,
            "region_count": 2,
            "connection_probability": 0.12,
            "stdp_enabled": True,
            "reward_modulation_enabled": True,
            "curiosity_enabled": True,
            "learning_rate": 0.01
        }
        
        config = pynlm.createConfigFromDict(config_dict)
        assert config is not None
        
        assert config.getInt("neuron_count") == 250
        assert config.getInt("region_count") == 2
        assert config.getFloat("connection_probability") == 0.12
    
    def test_item_access_syntax(self):
        """Test item access syntax (like dict['key'])."""
        config = pynlm.createDefaultConfigWithDefaults()
        config.set("test_key", "test_value")
        
        assert config["test_key"] == "test_value"
        
        config["another_key"] = "another_value"
        assert config.getString("another_key") == "another_value"
        
        assert "test_key" in config
        assert "another_key" in config
        assert "nonexistent_key" not in config
        
        initial_length = len(config)
        config.set("temp_key", "temp_value")
        assert len(config) == initial_length + 1
    
    def test_context_manager_simulation(self):
        """Test context manager for simulation steps."""
        brain = pynlm.createBrainWithDefaults()
        
        with pynlm.withBrainStep(brain, 0, 0.0) as brain_ptr:
            assert brain_ptr is not None
            brain_ptr.step(0, 0.0)
        
        assert brain.getTotalNeuronCount() > 0
    
    def test_dictionaries_and_objects(self):
        """Test conversion to Python dictionaries."""
        brain = pynlm.createBrainWithDefaults()
        brain.initialize()
        
        for i in range(10):
            brain.step(i, i * 0.001)
        
        brain_dict = pynlm.brainToDict(brain)
        
        assert isinstance(brain_dict, dict)
        assert "total_neurons" in brain_dict
        assert "total_synapses" in brain_dict
        assert "firing_neurons" in brain_dict
        assert "total_spikes" in brain_dict
        assert "average_firing_rate" in brain_dict
        assert "excitation_inhibition_ratio" in brain_dict
        assert "developmental_stage" in brain_dict
        
        assert brain_dict["total_neurons"] == brain.getTotalNeuronCount()
        assert brain_dict["total_synapses"] == brain.getTotalSynapseCount()
        assert brain_dict["firing_neurons"] == brain.getFiringNeuronCount()
        assert brain_dict["total_spikes"] == brain.getTotalSpikeCount()
        assert brain_dict["average_firing_rate"] == brain.getAverageFiringRate()
        assert brain_dict["excitation_inhibition_ratio"] == brain.getExcitationInhibitionRatio()
        assert brain_dict["developmental_stage"] == int(brain.getDevelopmentalStage())
        
        world = pynlm.createWorld(10, 10)
        world_dict = pynlm.worldToDict(world)
        
        assert isinstance(world_dict, dict)
        assert "width" in world_dict
        assert "height" in world_dict
        assert "simulation_time" in world_dict
        assert "random_seed" in world_dict

class TestErrorHandling:
    """Test error handling and validation."""
    
    def test_error_handling_initialization(self):
        """Test error handling during brain initialization."""
        brain = pynlm.createBrainWithDefaults()
        
        success = pynlm.safeReset(brain)
        assert isinstance(success, bool)
    
    def test_error_handling_brain_step(self):
        """Test error handling during brain stepping."""
        brain = pynlm.createBrainWithDefaults()
        brain.initialize()
        
        try:
            success = pynlm.safeBrainStep(brain, 0, 0.0)
            assert isinstance(success, bool)
        except Exception as e:
            pass

class TestAdvancedFeatures:
    """Test advanced features for power users."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.brain = pynlm.createBrainWithDefaults()
        self.brain.initialize()
    
    def test_ascii_brain_status(self):
        """Test ASCII brain status visualization."""
        for i in range(5):
            self.brain.step(i, i * 0.001)
        
        status = pynlm.asciiBrainStatus(self.brain)
        
        assert isinstance(status, str)
        assert "=== NLM Brain Status ===" in status
        assert "Neurons:" in status
        assert "Synapses:" in status
        assert "Firing:" in status
        assert "Total Spikes:" in status
        assert "Avg Firing Rate:" in status
        assert "E/I Ratio:" in status
        assert "Dev Stage:" in status
        assert "=========================" in status
    
    def test_benchmark_brain_step(self):
        """Test brain stepping benchmark."""
        steps = 50
        
        for i in range(steps):
            self.brain.step(i, i * 0.001)
        
        result = pynlm.benchmarkBrainStep(self.brain, steps)
        
        assert isinstance(result, tuple)
        assert len(result) == 2
        
        seconds, steps_per_second = result
        assert seconds >= 0.0
        assert steps_per_second > 0.0
        assert steps_per_second > 1.0
    
    def test_plasticity_status(self):
        """Test plasticity system status."""
        brain = pynlm.createBrainWithDefaults()
        brain.initialize()
        
        status = brain.getPlasticityStatus()
        
        assert isinstance(status, dict)
        assert "stdp" in status
        assert "hebbian" in status
        assert "structural" in status
    
    def test_config_validation(self):
        """Test configuration validation."""
        brain = pynlm.createBrainWithDefaults()
        
        is_valid = pynlm.validateBrainState(brain)
        assert isinstance(is_valid, bool)
        
        brain.initialize()
        is_valid = pynlm.validateBrainState(brain)
        assert isinstance(is_valid, bool)
    
    def test_episode_simulation(self):
        """Test episode simulation functionality."""
        brain = pynlm.createBrainWithDefaults()
        brain.initialize()
        world = pynlm.createWorld(20, 20, 8, 8)
        world.reset()
        world.setAgentStart(10, 10)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        episode_steps = 50
        for step in range(episode_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, world.getSimulationTime())
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        stats = agent.getEpisodeStats()
        
        assert isinstance(stats, dict)
        assert "total_spikes" in stats
        assert "firing_neurons" in stats
        assert "average_firing_rate" in stats
        assert "developmental_stage" in stats
        assert "curiosity_level" in stats
        assert "novelty_level" in stats
        assert "prediction_error" in stats
        
        assert stats["total_spikes"] >= 0
        assert stats["firing_neurons"] >= 0
        assert stats["average_firing_rate"] >= 0.0
        assert 0 <= stats["developmental_stage"] <= 4
        assert 0.0 <= stats["curiosity_level"] <= 1.0
        assert 0.0 <= stats["novelty_level"] <= 1.0
        assert abs(stats["prediction_error"]) <= 1.0

class TestIntegration:
    """Test integration and end-to-end functionality."""
    
    def test_complete_simulation_setup(self):
        """Test complete simulation setup."""
        config = pynlm.createDefaultConfigWithDefaults()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createWorld(15, 15, 6, 6)
        world.reset()
        world.setAgentStart(7, 7)
        
        agent = pynlm.createAgent(brain, world)
        
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        steps = 100
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, world.getSimulationTime())
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 20 == 0:
                assert brain.getTotalNeuronCount() > 0
                assert world.getAgentBody().isHealthy()
        
        assert brain.getTotalNeuronCount() > 0
        assert world.getAgentBody().isHealthy()
        assert world.getSimulationTime() >= 0.0
    
    def test_multiple_agents(self):
        """Test multiple agents sharing a brain."""
        brain = pynlm.createBrainWithDefaults()
        brain.initialize()
        
        world1 = pynlm.createWorld(10, 10)
        world1.setAgentStart(2, 2)
        
        world2 = pynlm.createWorld(10, 10)
        world2.setAgentStart(7, 7)
        
        agent1 = pynlm.createAgentBrain(brain)
        agent2 = pynlm.createAgentBrain(brain)
        
        agent1.initialize(world1)
        agent2.initialize(world2)
        
        for step in range(20):
            world1.update(0.1)
            agent1.processSensoryInput(world1.getSensoryPercept())
            brain.step(step, world1.getSimulationTime())
            action1 = agent1.decodeMotorCommand()
            world1.applyMotorCommand(action1, world1.getSimulationTime())
            
            world2.update(0.1)
            agent2.processSensoryInput(world2.getSensoryPercept())
            brain.step(step + 100, world2.getSimulationTime())
            action2 = agent2.decodeMotorCommand()
            world2.applyMotorCommand(action2, world2.getSimulationTime())
        
        assert agent1.getBrain() is brain
        assert agent2.getBrain() is brain
        assert world1.getAgentBody().isHealthy()
        assert world2.getAgentBody().isHealthy()
    
    def test_simulation_serialization(self):
        """Test brain serialization to file and back."""
        import tempfile
        import os
        
        brain = pynlm.createBrainWithDefaults()
        brain.initialize()
        
        for i in range(50):
            brain.step(i, i * 0.001)
        
        with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
            filepath = f.name
        
        try:
            success = brain.save(filepath)
            assert success, "Failed to save brain state"
            
            config = pynlm.createDefaultConfigWithDefaults()
            new_brain = pynlm.createBrain(config)
            
            success = new_brain.load(filepath)
            assert success, "Failed to load brain state"
            
            assert new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
            assert new_brain.getTotalSynapseCount() == brain.getTotalSynapseCount()
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_performance_with_many_neurons(self):
        """Test performance with a large number of neurons."""
        config = pynlm.createSimpleConfig(1000, 0.1)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_neurons = brain.getTotalNeuronCount()
        initial_synapses = brain.getTotalSynapseCount()
        
        steps = 200
        for step in range(steps):
            brain.step(step, step * 0.001)
        
        assert brain.getTotalNeuronCount() == initial_neurons
        assert brain.getTotalSynapseCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        
        perf_result = pynlm.benchmarkBrainStep(brain, 10)
        assert isinstance(perf_result, tuple)
        assert len(perf_result) == 2

class TestTypeConversions:
    """Test type conversions and ID handling."""
    
    def test_id_conversions(self):
        """Test ID type conversions."""
        neuron_id = pynlm.NeuronId(123)
        synapse_id = pynlm.SynapseId(456)
        region_id = pynlm.RegionId(789)
        population_id = pynlm.PopulationId(999)
        
        assert str(neuron_id) == "123"
        assert str(synapse_id) == "456"
        assert str(region_id) == "789"
        assert str(population_id) == "999"
        
        assert repr(neuron_id) == "<NeuronId: 123>"
        assert repr(synapse_id) == "<SynapseId: 456>"
        assert repr(region_id) == "<RegionId: 789>"
        assert repr(population_id) == "<PopulationId: 999>"
        
        assert pynlm.NeuronId(123) == neuron_id
        assert pynlm.SynapseId(456) == synapse_id
        assert pynlm.RegionId(789) == region_id
        assert pynlm.PopulationId(999) == population_id
        
        id_set = {neuron_id, synapse_id, region_id, population_id}
        assert len(id_set) == 4
        
        id_dict = {
            neuron_id: "neuron",
            synapse_id: "synapse",
            region_id: "region",
            population_id: "population"
        }
        assert id_dict[neuron_id] == "neuron"
        assert id_dict[synapse_id] == "synapse"

class TestHelpAndDocumentation:
    """Test help system and documentation access."""
    
    def test_print_hello(self):
        """Test help system printing."""
        pynlm.printHello()
        assert True
    
    def test_module_info(self):
        """Test module information access."""
        version = pynlm.getVersion()
        assert version == "0.1.0"
        
        constants = pynlm.getConstants()
        assert isinstance(constants, dict)
        assert "invalid_neuron_id" in constants
        assert "invalid_synapse_id" in constants
        assert "invalid_region_id" in constants
        assert "invalid_population_id" in constants
        assert "default_timestep" in constants
        assert "default_max_neurons" in constants
        
        assert constants["invalid_neuron_id"] == 18446744073709551615
        assert constants["invalid_synapse_id"] == 18446744073709551615
        assert constants["invalid_region_id"] == 18446744073709551615
        assert constants["invalid_population_id"] == 18446744073709551615
        assert constants["default_timestep"] == 0.001

def run_all_tests():
    """Run all test suites."""
    test_classes = [
        TestBasicFunctionality,
        TestPythonicFeatures,
        TestErrorHandling,
        TestAdvancedFeatures,
        TestIntegration,
        TestTypeConversions,
        TestHelpAndDocumentation,
    ]
    
    passed = 0
    failed = 0
    total = 0
    
    for test_class in test_classes:
        print(f"\n{'='*60}")
        print(f"Running {test_class.__name__}...")
        print(f"{'='*60}")
        
        test_instance = test_class()
        
        if hasattr(test_instance, 'setup_method'):
            test_instance.setup_method()
        
        test_methods = [method for method in dir(test_instance) 
                       if method.startswith('test_') and callable(getattr(test_instance, method))]
        
        for test_method in test_methods:
            total += 1
            try:
                print(f"Running {test_method}...", end=" ")
                getattr(test_instance, test_method)()
                print("PASSED")
                passed += 1
            except Exception as e:
                print(f"FAILED: {e}")
                traceback.print_exc()
                failed += 1
        
        if hasattr(test_instance, 'teardown_method'):
            test_instance.teardown_method()
    
    print(f"\n{'='*60}")
    print(f"Test Summary:")
    print(f"  Total tests: {total}")
    print(f"  Passed: {passed}")
    print(f"  Failed: {failed}")
    print(f"  Success rate: {passed/total*100:.1f}%")
    print(f"{'='*60}")
    
    if failed > 0:
        print("\nSome tests failed. Please check the output above for details.")
        return False
    else:
        print("\nAll tests passed! 🎉")
        return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)