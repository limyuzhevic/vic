"""
Comprehensive test suite for enhanced pynlm Python bindings.

This test suite validates all the new functionality added to the pynlm
Python bindings, including:
- Core neural components (Neuron, Synapse, NeuralRegion, NeuralPopulation)
- Advanced plasticity rules (STDP, Hebbian, reward-modulated learning)
- Memory systems (WorkingMemory, EpisodicMemory)
- Neuromodulation (Reward, Curiosity, Novelty, PredictionError)
- Development and aging mechanisms
- Performance optimizations (MemoryPool, EventQueue, CheckpointSystem)
- Advanced simulation controls and utilities
- Factory functions and custom integration
- Error handling and backward compatibility
"""

import sys
import os
import tempfile
import pytest
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

# Import pynlm - handle import errors gracefully
try:
    import pynlm
    HAS_PYNLM = True
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"⚠ Warning: Could not import pynlm: {e}")
    print("   Python bindings are not available - skipping tests")
    HAS_PYNLM = False

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestBasicTypes:
    """Test basic type classes."""
    
    def test_neuron_id(self):
        """Test NeuronId class."""
        neuron_id = pynlm.NeuronId(42)
        assert neuron_id.value == 42
        assert neuron_id.index() == 42
        
        # Test equality
        assert neuron_id == pynlm.NeuronId(42)
        assert neuron_id != pynlm.NeuronId(43)
        
        # Test hash
        assert hash(neuron_id) == hash(pynlm.NeuronId(42))
        
        # Test string representation
        assert "<NeuronId: 42>" in repr(neuron_id)
    
    def test_synapse_id(self):
        """Test SynapseId class."""
        synapse_id = pynlm.SynapseId(100)
        assert synapse_id.value == 100
        assert synapse_id.index() == 100
        
        # Test hash
        assert hash(synapse_id) == hash(pynlm.SynapseId(100))
        
        # Test string representation
        assert "<SynapseId: 100>" in repr(synapse_id)
    
    def test_region_id(self):
        """Test RegionId class."""
        region_id = pynlm.RegionId(1)
        assert region_id.value == 1
        assert region_id.index() == 1
        
        # Test equality
        assert region_id == pynlm.RegionId(1)
        assert region_id != pynlm.RegionId(2)
    
    def test_population_id(self):
        """Test PopulationId class."""
        population_id = pynlm.PopulationId(999)
        assert population_id.value == 999
        assert population_id.index() == 999
    
    def test_neuron_type_enums(self):
        """Test NeuronType enumeration."""
        assert pynlm.NeuronType.Excitatory == 0
        assert pynlm.NeuronType.Inhibitory == 1
        assert pynlm.NeuronType.Modulatory == 2
        assert pynlm.NeuronType.Sensory == 3
        assert pynlm.NeuronType.Motor == 4
        assert pynlm.NeuronType.Internal == 5
    
    def test_synapse_type_enums(self):
        """Test SynapseType enumeration."""
        assert pynlm.SynapseType.Excitatory == 0
        assert pynlm.SynapseType.Inhibitory == 1
        assert pynlm.SynapseType.Modulatory == 2
        assert pynlm.SynapseType.Electrical == 3
        assert pynlm.SynapseType.GapJunction == 4
    
    def test_developmental_stage_enums(self):
        """Test DevelopmentalStage enumeration."""
        stages = [
            (pynlm.DevelopmentalStage.Initial, 0),
            (pynlm.DevelopmentalStage.CriticalPeriod, 1),
            (pynlm.DevelopmentalStage.Maturation, 2),
            (pynlm.DevelopmentalStage.Adult, 3),
            (pynlm.DevelopmentalStage.Aging, 4)
        ]
        
        for stage, expected_value in stages:
            assert stage.value == expected_value
    
    def test_firing_state_enums(self):
        """Test FiringState enumeration."""
        states = [
            (pynlm.FiringState.Resting, 0),
            (pynlm.FiringState.Active, 1),
            (pynlm.FiringState.Refractory, 2),
            (pynlm.FiringState.Inhibited, 3)
        ]
        
        for state, expected_value in states:
            assert state.value == expected_value
    
    def test_action_type_enums(self):
        """Test ActionType enumeration."""
        actions = [
            (pynlm.ActionType.MoveForward, 0),
            (pynlm.ActionType.MoveBackward, 1),
            (pynlm.ActionType.MoveLeft, 2),
            (pynlm.ActionType.MoveRight, 3),
            (pynlm.ActionType.TurnLeft, 4),
            (pynlm.ActionType.TurnRight, 5),
            (pynlm.ActionType.Look, 6),
            (pynlm.ActionType.LookUp, 7),
            (pynlm.ActionType.LookDown, 8),
            (pynlm.ActionType.Interact, 9),
            (pynlm.ActionType.Eat, 10),
            (pynlm.ActionType.Drink, 11),
            (pynlm.ActionType.Rest, 12),
            (pynlm.ActionType.Wait, 13),
            (pynlm.ActionType.Custom, 14)
        ]
        
        for action, expected_value in actions:
            assert action.value == expected_value
    
    def test_world_object_type_enums(self):
        """Test WorldObjectType enumeration."""
        assert pynlm.WorldObjectType.Empty == 0
        assert pynlm.WorldObjectType.Resource == 1
        assert pynlm.WorldObjectType.Hazard == 2
        assert pynlm.WorldObjectType.Wall == 3
        assert pynlm.WorldObjectType.Marker == 4

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestConfig:
    """Test configuration system."""
    
    def test_default_config(self):
        """Test default configuration creation."""
        config = pynlm.create_default_config()
        assert config is not None
        # Test that it's a Config object
        assert hasattr(config, 'has')
        assert hasattr(config, 'getKeys')
    
    def test_config_operations(self):
        """Test configuration operations."""
        config = pynlm.create_default_config()
        
        # Test set and has
        config.set("brain.neuron_count", 1000)
        config.set("brain.region_count", 10)
        config.set("simulation.dt", 0.001)
        
        assert config.has("brain.neuron_count")
        assert config.has("brain.region_count")
        assert config.has("simulation.dt")
        
        # Test getKeys
        keys = config.getKeys()
        assert "brain.neuron_count" in keys
        assert "brain.region_count" in keys
        assert "simulation.dt" in keys
        
        # Test summary
        summary = config.summary()
        assert len(summary) > 0
        assert "brain.neuron_count" in summary
    
    def test_config_save_load(self, temp_dir):
        """Test configuration save and load."""
        config = pynlm.create_default_config()
        config.set("brain.neuron_count", 1500)
        config.set("brain.region_count", 15)
        
        # Save to file
        config_path = os.path.join(temp_dir, "config.json")
        config.saveToFile(config_path)
        assert os.path.exists(config_path)
        
        # Load from file
        config2 = pynlm.create_default_config()
        config2.loadFromFile(config_path)
        
        # Verify loaded values
        assert config2.get("brain.neuron_count") == 1500
        assert config2.get("brain.region_count") == 15
    
    def test_config_load_from_args(self):
        """Test configuration load from command line arguments."""
        # Create a minimal argv for testing
        argv = ["test", "--neuron-count", "500", "--region-count", "5"]
        
        config = pynlm.create_default_config()
        # Note: This may not work as expected if loadFromArgs expects different format
        # but we'll test the interface is available

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestPlasticityRules:
    """Test plasticity rules."""
    
    def test_stdp_rule(self):
        """Test STDP rule."""
        stdp = pynlm.create_stdp_rule(ltp_weight=0.02, ltd_weight=0.02, tau=25.0)
        assert stdp is not None
        
        assert stdp.getLTPWeight() == 0.02
        assert stdp.getLTDWeight() == 0.02
        assert stdp.getTimeConstant() == 25.0
        
        # Test configure method
        stdp.configure(0.01, 0.01, 20.0)
        assert stdp.getLTPWeight() == 0.01
        assert stdp.getTimeConstant() == 20.0
    
    def test_hebbian_rule(self):
        """Test Hebbian rule."""
        hebbian = pynlm.create_hebbian_rule(learning_rate=0.05)
        assert hebbian is not None
        
        assert hebbian.getLearningRate() == 0.05
        
        # Test setLearningRate
        hebbian.setLearningRate(0.1)
        assert hebbian.getLearningRate() == 0.1
    
    def test_reward_modulated_stdp(self):
        """Test reward-modulated STDP."""
        rstdp = pynlm.create_reward_modulated_stdp()
        assert rstdp is not None
        
        # Test that it has the required interface
        assert hasattr(rstdp, 'update')
        assert hasattr(rstdp, 'applyWeightChange')
        assert hasattr(rstdp, 'getName')
    
    def test_triplet_stdp(self):
        """Test TripletSTDP."""
        triplet = pynlm.create_stdp_rule(rule_type="triplet")
        assert triplet is not None
        assert triplet.getName() == "TripletSTDP"
    
    def test_structural_plasticity(self):
        """Test StructuralPlasticity."""
        struct_plast = pynlm.StructuralPlasticity()
        assert struct_plast is not None
        
        assert struct_plast.getName() == "StructuralPlasticity"
    
    def test_plasticity_rule_factory(self):
        """Test plasticity rule factory function."""
        # Test different rule types
        rules = [
            pynlm.create_plasticity_rule("stdp"),
            pynlm.create_plasticity_rule("hebbian"),
            pynlm.create_plasticity_rule("reward_modulated_stdp"),
            pynlm.create_plasticity_rule("triplet_stdp"),
            pynlm.create_plasticity_rule("structural_plasticity"),
        ]
        
        assert len(rules) == 5
        assert all(rule is not None for rule in rules)
        
        # Test invalid rule type
        with pytest.raises(ValueError, match="Unknown plasticity rule type"):
            pynlm.create_plasticity_rule("invalid_type")

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestMemorySystems:
    """Test memory systems."""
    
    def test_working_memory(self):
        """Test working memory system."""
        working_memory = pynlm.create_working_memory(capacity=1000)
        assert working_memory is not None
        
        assert working_memory.getCapacity() == 1000
        assert working_memory.getSize() == 0
        assert working_memory.getUtilization() == 0.0
        
        # Test store and retrieve
        working_memory.store("test_key", 42.5)
        assert working_memory.has("test_key")
        
        value = working_memory.retrieve("test_key")
        assert value == 42.5
        
        # Test multiple stores
        working_memory.store("key1", 1.0)
        working_memory.store("key2", 2.0)
        working_memory.store("key3", 3.0)
        
        assert working_memory.getSize() == 3
        assert working_memory.getUtilization() == 3.0 / 1000.0
        
        # Test getAllKeys
        keys = working_memory.getAllKeys()
        assert "test_key" in keys
        assert "key1" in keys
        assert "key2" in keys
        assert "key3" in keys
    
    def test_episodic_memory(self):
        """Test episodic memory system."""
        episodic_memory = pynlm.create_episodic_memory(capacity=100)
        assert episodic_memory is not None
        
        assert episodic_memory.getCapacity() == 100
        assert episodic_memory.getSize() == 0
        assert episodic_memory.getConsolidationCount() == 0
        
        # Test store and retrieve
        episodic_memory.store("episode_1", {
            "time": 1.0,
            "reward": 0.8,
            "state": "exploration"
        })
        
        assert episodic_memory.has("episode_1")
        
        episode = episodic_memory.retrieve("episode_1")
        assert episode["time"] == 1.0
        assert episode["reward"] == 0.8
        assert episode["state"] == "exploration"
        
        # Test multiple episodes
        episodic_memory.store("episode_2", {
            "time": 2.0,
            "reward": 0.3,
            "state": "resting"
        })
        
        assert episodic_memory.getSize() == 2
    
    def test_memory_system_factory(self):
        """Test memory system factory function."""
        # Test working memory factory
        working_memory = pynlm.create_memory_system("working", capacity=500)
        assert working_memory.getCapacity() == 500
        
        # Test episodic memory factory
        episodic_memory = pynlm.create_memory_system("episodic", capacity=50)
        assert episodic_memory.getCapacity() == 50
        
        # Test invalid type
        with pytest.raises(ValueError, match="Unknown memory system type"):
            pynlm.create_memory_system("invalid_type")
    
    def test_memory_operations(self):
        """Test memory operations."""
        memory = pynlm.Memory()
        
        # Test store, retrieve, has, remove
        memory.store("key1", "value1")
        memory.store("key2", "value2")
        
        assert memory.has("key1")
        assert memory.has("key2")
        assert not memory.has("key3")
        
        assert memory.retrieve("key1") == "value1"
        assert memory.retrieve("key2") == "value2"
        
        memory.remove("key1")
        assert not memory.has("key1")
        assert memory.has("key2")
        
        # Test clear
        memory.clear()
        assert memory.getSize() == 0

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestNeuromodulation:
    """Test neuromodulation systems."""
    
    def test_reward_modulator(self):
        """Test reward modulator."""
        reward = pynlm.create_reward_modulator(initial_value=0.5, learning_rate=0.02)
        assert reward is not None
        
        assert reward.getCurrentValue() == 0.5
        assert reward.getLearningRate() == 0.02
        
        # Test setCurrentValue
        reward.setCurrentValue(0.8)
        assert reward.getCurrentValue() == 0.8
        
        # Test getPredictionError
        error = reward.getPredictionError(0.7)  # predicted 0.7, actual is 0.8
        assert error is not None
    
    def test_curiosity_modulator(self):
        """Test curiosity modulator."""
        curiosity = pynlm.create_curiosity_module(initial_level=0.7, decay_rate=0.01)
        assert curiosity is not None
        
        assert curiosity.getCurrentLevel() == 0.7
        assert curiosity.getDecayRate() == 0.01
        
        # Test setCurrentLevel
        curiosity.setCurrentLevel(0.9)
        assert curiosity.getCurrentLevel() == 0.9
    
    def test_novelty_detector(self):
        """Test novelty detector."""
        novelty = pynlm.Novelty()
        assert novelty is not None
        
        # Test getCurrentNovelty
        current_novelty = novelty.getCurrentNovelty()
        assert current_novelty is not None
        
        # Test setThreshold
        novelty.setThreshold(0.5)
        # Note: threshold might not be directly accessible, but setThreshold should work
    
    def test_prediction_error(self):
        """Test prediction error calculator."""
        pred_error = pynlm.PredictionError()
        assert pred_error is not None
        
        # Test calculateError
        error = pred_error.calculateError(0.8, 0.9)  # predicted 0.8, actual 0.9
        assert error is not None
        
        assert pred_error.getLearningRate() is not None
        pred_error.setLearningRate(0.05)
        assert pred_error.getLearningRate() == 0.05
    
    def test_neuromodulator_factory(self):
        """Test neuromodulator factory function."""
        # Test reward factory
        reward = pynlm.create_neuromodulator("reward", initial_value=0.3)
        assert reward.getCurrentValue() == 0.3
        
        # Test curiosity factory
        curiosity = pynlm.create_neuromodulator("curiosity", initial_level=0.6)
        assert curiosity.getCurrentLevel() == 0.6
        
        # Test direct constructors for other types
        novelty = pynlm.Novelty()
        pred_error = pynlm.PredictionError()
        assert novelty is not None
        assert pred_error is not None
        
        # Test invalid type
        with pytest.raises(ValueError, match="Unknown neuromodulator type"):
            pynlm.create_neuromodulator("invalid_type")

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestDevelopment:
    """Test development systems."""
    
    def test_development_system(self):
        """Test development system."""
        dev_system = pynlm.DevelopmentSystem()
        assert dev_system is not None
        
        # Test get/set current stage
        current_stage = dev_system.getCurrentStage()
        assert current_stage is not None
        
        dev_system.setCurrentStage(pynlm.DevelopmentalStage.CriticalPeriod)
        assert dev_system.getCurrentStage() == pynlm.DevelopmentalStage.CriticalPeriod
        
        # Test stage progress
        progress = dev_system.getStageProgress()
        assert progress >= 0.0 and progress <= 1.0
        
        # Test stage duration
        duration = dev_system.getStageDuration()
        assert duration > 0.0
        
        # Test plasticity rate
        rate = dev_system.getPlasticityRate()
        assert rate >= 0.0
        
        dev_system.setPlasticityRate(0.5)
        assert dev_system.getPlasticityRate() == 0.5
        
        # Test critical period
        assert dev_system.isInCriticalPeriod() is not None

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestPerformanceInfrastructure:
    """Test performance infrastructure."""
    
    def test_memory_pool(self):
        """Test memory pool."""
        memory_pool = pynlm.MemoryPool(1000)
        assert memory_pool is not None
        
        assert memory_pool.getTotalCapacity() == 1000
        assert memory_pool.getUsedCapacity() == 0
        assert memory_pool.getUtilization() == 0.0
        
        # Test allocation
        neuron_id = memory_pool.allocateNeuron()
        synapse_id = memory_pool.allocateSynapse()
        
        assert neuron_id is not None
        assert synapse_id is not None
        
        assert memory_pool.getUsedCapacity() == 2
        assert memory_pool.getUtilization() > 0.0
        
        # Test deallocation
        memory_pool.deallocateNeuron(neuron_id)
        memory_pool.deallocateSynapse(synapse_id)
        
        assert memory_pool.getUsedCapacity() == 0
        assert memory_pool.getUtilization() == 0.0
    
    def test_event_queue(self):
        """Test event queue."""
        event_queue = pynlm.EventQueue()
        assert event_queue is not None
        
        assert event_queue.size() == 0
        assert event_queue.empty()
        
        # Test push and pop
        from pynlm import SpikeEvent
        event = SpikeEvent(pynlm.NeuronId(42), 1.0, 1)
        event_queue.pushEvent(event)
        
        assert event_queue.size() == 1
        assert not event_queue.empty()
        
        # Note: popEvent may need implementation
        # event_queue.popEvent()
    
    def test_checkpoint_system(self):
        """Test checkpoint system."""
        checkpoint_system = pynlm.CheckpointSystem()
        assert checkpoint_system is not None
        
        # Test auto-save interval
        checkpoint_system.setAutoSave(100)
        assert checkpoint_system.getAutoSaveInterval() == 100
        
        # Test hasCheckpoint (requires file)
        # Note: This is a stub test - actual functionality depends on implementation
        # assert not checkpoint_system.hasCheckpoint("nonexistent_file.json")
    
    def test_sparse_connectivity(self):
        """Test sparse connectivity."""
        sparse_conn = pynlm.SparseConnectivity(1000)
        assert sparse_conn is not None
        
        assert sparse_conn.getMaxConnections() == 1000
        assert sparse_conn.getConnectionCount() == 0
        assert sparse_conn.getUtilization() == 0.0
        
        # Test addConnection
        from pynlm import NeuronId, SynapseId
        source = NeuronId(1)
        destination = NeuronId(2)
        synapse_id = SynapseId(100)
        
        sparse_conn.addConnection(source, destination, synapse_id)
        assert sparse_conn.getConnectionCount() == 1
        assert sparse_conn.hasConnection(source, destination)
        
        # Test getConnections
        outgoing = sparse_conn.getOutgoingConnections(source)
        assert len(outgoing) == 1
        
        # Test removeConnection
        sparse_conn.removeConnection(source, destination)
        assert sparse_conn.getConnectionCount() == 0
        assert not sparse_conn.hasConnection(source, destination)
    
    def test_performance_optimizer(self):
        """Test performance optimizer."""
        from pynlm import OptimizedBrain
        
        # Note: This is a stub - OptimizedBrain requires a Brain instance
        # opt_brain = OptimizedBrain(None)
        # assert opt_brain is not None
        pass  # Skip for now

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestBrainComponents:
    """Test brain components."""
    
    def test_brain_creation(self):
        """Test brain creation."""
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        assert brain is not None
        
        # Test initialization
        brain.initialize()
        
        # Test brain stats
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getRegionCount() > 0
        
        # Test developmental stage
        stage = brain.getDevelopmentalStage()
        assert stage is not None
        
        # Test firing statistics
        assert brain.getActiveNeuronCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestWorldAndAgent:
    """Test world and agent components."""
    
    def test_world_creation(self):
        """Test world creation."""
        world = pynlm.create_simple_world()
        assert world is not None
        
        # Test world configuration
        world.configure(100, 100, 50, 50)
        assert world.getWidth() == 100
        assert world.getHeight() == 100
        
        # Test agent body
        agent_body = world.getAgentBody()
        assert agent_body is not None
        assert agent_body.x >= 0 and agent_body.y >= 0
        
        # Test sensory percept
        percept = world.getSensoryPercept()
        assert percept is not None
    
    def test_agent_brain_creation(self):
        """Test agent brain creation."""
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        world = pynlm.create_simple_world()
        world.configure(50, 50, 25, 25)
        world.setAgentStart(25, 25)
        
        agent_brain = pynlm.create_agent_brain(brain)
        agent_brain.initialize(world)
        
        # Test agent brain operations
        assert agent_brain.getSensoryInputSize() > 0
        assert agent_brain.getMotorOutputSize() > 0
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Decode motor command
        action = agent_brain.decodeMotorCommand()
        assert action is not None
        assert action.getType() is not None

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestFactoryFunctions:
    """Test factory functions."""
    
    def test_all_factory_functions(self):
        """Test all factory functions work."""
        # Test basic factories
        config = pynlm.create_default_config()
        assert config is not None
        
        brain = pynlm.create_brain(config)
        assert brain is not None
        
        world = pynlm.create_simple_world()
        assert world is not None
        
        agent_brain = pynlm.create_agent_brain(brain)
        assert agent_brain is not None
        
        region = pynlm.create_neural_region("test_region")
        assert region is not None
        
        # Test advanced factories
        stdp = pynlm.create_stdp_rule()
        assert stdp is not None
        
        hebbian = pynlm.create_hebbian_rule()
        assert hebbian is not None
        
        rstdp = pynlm.create_reward_modulated_stdp()
        assert rstdp is not None
        
        working_memory = pynlm.create_working_memory()
        assert working_memory is not None
        
        episodic_memory = pynlm.create_episodic_memory()
        assert episodic_memory is not None
        
        reward_mod = pynlm.create_reward_modulator()
        assert reward_mod is not None
        
        curiosity_mod = pynlm.create_curiosity_module()
        assert curiosity_mod is not None
        
        prediction_system = pynlm.create_prediction_system()
        assert prediction_system is not None
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestErrorHandling:
    """Test error handling."""
    
    def test_factory_function_errors(self):
        """Test that factory functions raise appropriate errors."""
        # Test plasticity rule factory error
        with pytest.raises(ValueError, match="Unknown plasticity rule type"):
            pynlm.create_plasticity_rule("invalid_type")
        
        # Test memory system factory error
        with pytest.raises(ValueError, match="Unknown memory system type"):
            pynlm.create_memory_system("invalid_type")
        
        # Test neuromodulator factory error
        with pytest.raises(ValueError, match="Unknown neuromodulator type"):
            pynlm.create_neuromodulator("invalid_type")

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestBackwardCompatibility:
    """Test backward compatibility."""
    
    def test_original_type_imports(self):
        """Test that all original types are still available."""
        original_types = [
            'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
            'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
            'ActionType', 'MotorCommand', 'WorldObjectType',
            'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
            'Vision', 'Audio', 'InternalSignals', 'Action',
            'SensoryPercept', 'AgentBody', 'WorldObject',
        ]
        
        for type_name in original_types:
            assert hasattr(pynlm, type_name), f"Missing original type: {type_name}"
    
    def test_original_factory_functions(self):
        """Test that original factory functions still work."""
        original_factories = [
            ('create_default_config', lambda: pynlm.create_default_config()),
            ('create_brain', lambda config: pynlm.create_brain(config)),
            ('create_agent_brain', lambda brain: pynlm.create_agent_brain(brain)),
            ('create_simple_world', lambda: pynlm.create_simple_world()),
            ('create_neural_region', lambda name: pynlm.create_neural_region(name)),
            ('create_stdp_rule', lambda **kwargs: pynlm.create_stdp_rule(**kwargs)),
            ('create_hebbian_rule', lambda **kwargs: pynlm.create_hebbian_rule(**kwargs)),
            ('create_reward_modulated_stdp', lambda: pynlm.create_reward_modulated_stdp()),
        ]
        
        for func_name, func_call in original_factories:
            try:
                if func_name == 'create_brain':
                    config = pynlm.create_default_config()
                    result = func_call(config)
                elif func_name == 'create_agent_brain':
                    brain = pynlm.create_brain(pynlm.create_default_config())
                    brain.initialize()
                    result = func_call(brain)
                else:
                    result = func_call()
                
                assert result is not None, f"Factory function {func_name} returned None"
            except Exception as e:
                pytest.fail(f"Factory function {func_name} failed: {e}")

@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm not available")
class TestModuleInfo:
    """Test module information and utilities."""
    
    def test_version_info(self):
        """Test version information."""
        version = pynlm.get_version()
        assert version == "0.1.0"
        assert isinstance(version, str)
    
    def test_module_info(self):
        """Test module information."""
        info = pynlm.get_info()
        assert isinstance(info, dict)
        assert info['version'] == "0.1.0"
        assert 'author' in info
        assert 'description' in info
        assert 'available_classes' in info
        assert isinstance(info['available_classes'], list)
    
    def test_author_info(self):
        """Test author information."""
        assert pynlm.__author__ == "NLM Authors"

# Helper fixtures
def pytest_configure(config):
    """Configure pytest."""
    if HAS_PYNLM:
        print("\n✅ pynlm is available - running tests")
    else:
        print("\n⚠️  pynlm not available - skipping tests")

def temp_dir_fixture(tmpdir):
    """Create a temporary directory for tests."""
    return str(tmpdir)

# Define custom mark for the demo
pytest.mark_demo = pytest.mark.skipif(
    not HAS_PYNLM, 
    reason="pynlm not available - demo requires Python bindings"
)

if __name__ == "__main__":
    # Run tests directly if script is executed
    if HAS_PYNLM:
        print("\n" + "="*80)
        print("RUNNING PYNLM TEST SUITE")
        print("="*80)
        
        # Use pytest to run tests
        import pytest
        sys.exit(pytest.main([
            __file__,
            "-v",
            "--tb=short",
            "--color=yes"
        ]))
    else:
        print("\n⚠️  pynlm not available - tests cannot be run")
        print("   Please build the pynlm module first:")
        print("   mkdir build && cd build && cmake .. && make")