"""Test advanced features of enhanced NLM Python bindings."""
import pytest
from pynlm import Brain, Config, AgentBrain, SimpleWorld, NeuronType, SynapseType
import pynlm.utils as utils

class TestAdvancedFeatures:
    """Test advanced NLM features."""
    
    def test_type_definitions(self):
        """Test type definitions and enumerations."""
        # Test basic enumerations
        assert hasattr(pynlm, 'NeuronType')
        assert hasattr(pynlm.NeuronType, 'Excitatory')
        assert hasattr(pynlm.NeuronType, 'Inhibitory')
        assert hasattr(pynlm.NeuronType, 'Modulatory')
        assert hasattr(pynlm.NeuronType, 'Sensory')
        assert hasattr(pynlm.NeuronType, 'Motor')
        assert hasattr(pynlm.NeuronType, 'Internal')
        
        assert pynlm.NeuronType.Excitatory == 0
        assert pynlm.NeuronType.Inhibitory == 1
        assert pynlm.NeuronType.Modulatory == 2
        assert pynlm.NeuronType.Sensory == 3
        assert pynlm.NeuronType.Motor == 4
        assert pynlm.NeuronType.Internal == 5
        
        # Test SynapseType
        assert hasattr(pynlm, 'SynapseType')
        assert pynlm.SynapseType.Excitatory == 0
        assert pynlm.SynapseType.Inhibitory == 1
        assert pynlm.SynapseType.Modulatory == 2
        assert pynlm.SynapseType.Electrical == 3
        assert pynlm.SynapseType.GapJunction == 4
        
        # Test DevelopmentalStage
        assert hasattr(pynlm, 'DevelopmentalStage')
        assert pynlm.DevelopmentalStage.Initial == 0
        assert pynlm.DevelopmentalStage.CriticalPeriod == 1
        assert pynlm.DevelopmentalStage.Maturation == 2
        assert pynlm.DevelopmentalStage.Adult == 3
        assert pynlm.DevelopmentalStage.Aging == 4
        
        # Test ActionType
        assert hasattr(pynlm, 'ActionType')
        assert pynlm.ActionType.MoveForward == 0
        assert pynlm.ActionType.MoveBackward == 1
        assert pynlm.ActionType.MoveLeft == 2
        assert pynlm.ActionType.MoveRight == 3
        assert pynlm.ActionType.TurnLeft == 4
        assert pynlm.ActionType.TurnRight == 5
        
        # Test WorldObjectType
        assert hasattr(pynlm, 'WorldObjectType')
        assert pynlm.WorldObjectType.Empty == 0
        assert pynlm.WorldObjectType.Resource == 1
        assert pynlm.WorldObjectType.Hazard == 2
        assert pynlm.WorldObjectType.Wall == 3
        assert pynlm.WorldObjectType.Marker == 4
    
    def test_id_types(self):
        """Test ID type definitions."""
        # Test NeuronId
        neuron_id = pynlm.NeuronId(1)
        assert hasattr(neuron_id, 'value')
        assert neuron_id.value == 1
        assert str(neuron_id) == "NeuronId(1)"
        
        # Test SynapseId
        synapse_id = pynlm.SynapseId(5)
        assert synapse_id.value == 5
        assert str(synapse_id) == "SynapseId(5)"
        
        # Test RegionId
        region_id = pynlm.RegionId(10)
        assert region_id.value == 10
        
        # Test PopulationId
        pop_id = pynlm.PopulationId(20)
        assert pop_id.value == 20
        
        # Test equality
        assert neuron_id == pynlm.NeuronId(1)
        assert neuron_id != pynlm.NeuronId(2)
        assert neuron_id != "not an id"
    
    def test_constants(self):
        """Test constant definitions."""
        assert hasattr(pynlm, 'PI')
        assert abs(pynlm.PI - 3.141592653589793) < 1e-10
        
        assert hasattr(pynlm, 'EPSILON')
        assert pynlm.EPSILON == 1e-10
        
        assert hasattr(pynlm, 'MAX_SIMULATION_STEPS')
        assert pynlm.MAX_SIMULATION_STEPS == 1000000
        
        assert hasattr(pynlm, 'DEFAULT_RANDOM_SEED')
        assert pynlm.DEFAULT_RANDOM_SEED == 42
    
    def test_brain_comparison(self):
        """Test brain comparison utility."""
        config = Config()
        brain1 = Brain(config)
        brain2 = Brain(config)
        
        brain1.initialize()
        brain2.initialize()
        
        # Run different numbers of steps on each brain
        brain1.step_batch(10)
        brain2.step_batch(5)
        
        # Compare brains
        comparison = utils.compare_brains(brain1, brain2)
        assert isinstance(comparison, dict)
        assert 'has_differences' in comparison
        assert 'brain1_spike_count' in comparison
        assert 'brain2_spike_count' in comparison
        
        assert comparison['has_differences'] is True
        assert comparison['brain1_spike_count'] == 10
        assert comparison['brain2_spike_count'] == 5
    
    def test_brain_validation(self):
        """Test brain validation utility."""
        # Test uninitialized brain
        uninitialized_brain = Brain()
        result = utils.validate_brain(uninitialized_brain)
        assert isinstance(result, bool)
        
        # Test initialized brain
        initialized_brain = Brain()
        initialized_brain.initialize()
        result = utils.validate_brain(initialized_brain)
        assert isinstance(result, bool)
    
    def test_utility_functions(self):
        """Test all utility functions."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test get_version_info
        version_info = utils.get_version_info()
        assert isinstance(version_info, dict)
        assert 'version' in version_info
        assert isinstance(version_info['version'], str)
        
        # Test validate_brain
        assert utils.validate_brain(brain) is True
        
        # Test compare_brains
        brain2 = Brain(config)
        brain2.initialize()
        comparison = utils.compare_brains(brain, brain2)
        assert isinstance(comparison, dict)
        assert 'has_differences' in comparison
        
        # Test benchmark_brain
        benchmark_result = utils.benchmark_brain(brain, steps=10)
        assert isinstance(benchmark_result, dict)
        assert 'avg_step_time_ms' in benchmark_result
        assert 'total_steps' in benchmark_result
        assert 'min_step_time_ms' in benchmark_result
        assert 'max_step_time_ms' in benchmark_result
        assert 'std_step_time_ms' in benchmark_result
        
        # Test get_memory_usage
        memory_info = utils.get_memory_usage()
        assert isinstance(memory_info, dict)
        assert 'rss_mb' in memory_info
        assert 'vms_mb' in memory_info
    
    def test_utility_error_handling(self):
        """Test error handling in utility functions."""
        # Test with invalid brain (None)
        with pytest.raises((pynlm.PynlmError, TypeError)):
            utils.validate_brain(None)
        
        # Test with invalid parameters for utility functions
        config = Config()
        brain = Brain(config)
        
        with pytest.raises((pynlm.PynlmError, TypeError)):
            utils.benchmark_brain(None, steps=10)
        
        with pytest.raises((pynlm.PynlmError, TypeError)):
            utils.benchmark_brain(brain, steps=-1)
    
    def test_brain_memory_access(self):
        """Test memory system access methods."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test memory system access
        assert hasattr(brain, 'get_working_memory')
        assert hasattr(brain, 'get_episodic_memory')
        assert hasattr(brain, 'get_associative_memory')
        
        # Test that these methods return expected types (None for stubs)
        assert brain.get_working_memory() is None
        assert brain.get_episodic_memory() is None
        assert brain.get_associative_memory() is None
    
    def test_prediction_system_access(self):
        """Test prediction system access."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test prediction system access
        assert hasattr(brain, 'get_prediction_system')
        assert hasattr(brain, 'get_planner')
        assert hasattr(brain, 'get_concept_formation')
        
        # These should return None for stubs
        assert brain.get_prediction_system() is None
        assert brain.get_planner() is None
        assert brain.get_concept_formation() is None
    
    def test_agent_features(self):
        """Test agent-specific advanced features."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        agent = AgentBrain(brain)
        agent.initialize(SimpleWorld())
        
        # Test agent brain features
        assert hasattr(agent, 'get_summary')
        assert hasattr(agent, 'process_sensory_batch')
        assert hasattr(agent, 'decode_motor_command')
        
        # Test enable methods
        agent.enable_reward_modulation(True)
        assert agent.reward_modulation_enabled is True
        
        agent.enable_curiosity(True)
        assert agent.curiosity_enabled is True
        
        # Test developmental features
        assert hasattr(agent, 'get_developmental_stage')
        assert hasattr(agent, 'get_neuromodulation_level')
        assert hasattr(agent, 'get_curiosity_level')
        
        assert agent.get_developmental_stage() == pynlm.DevelopmentalStage.Initial
    
    def test_world_features(self):
        """Test world advanced features."""
        world = SimpleWorld()
        
        # Test advanced configuration
        config = {
            "max_energy": 500.0,
            "energy_decay_rate": 0.002,
            "random_seed": 42,
            "agent_start_x": 5.0,
            "agent_start_y": 10.0
        }
        
        result = world.configure_advanced(**config)
        assert result is True
        
        # Test world properties
        assert hasattr(world, 'save_to_file')
        assert hasattr(world, 'load_from_file')
        
        # Test additional world methods
        assert hasattr(world, 'get_sensory_percept')
        assert hasattr(world, 'get_agent_body')
        assert hasattr(world, 'is_valid_position')
        
        # Test position validation
        assert world.is_valid_position(0, 0) is True
        assert world.is_valid_position(-10, -10) is True

class TestIntegrationFeatures:
    """Test integration features and complete workflows."""
    
    def test_complete_brain_world_agent_loop(self):
        """Test complete brain-world-agent integration loop."""
        # Create all components
        config = Config()
        brain = Brain(config)
        world = SimpleWorld()
        agent = AgentBrain(brain)
        
        # Initialize brain
        brain.initialize()
        
        # Configure agent with world
        agent.initialize(world)
        
        # Configure agent features
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        
        # Run simulation steps
        steps = 20
        for step in range(steps):
            # Brain processes
            brain.step(step, step * 0.1)
            
            # Get sensory percept
            percept = world.get_sensory_percept()
            
            # Agent processes sensory input
            agent.process_sensory_input(percept)
            
            # Update world
            world.update(step * 0.1)
        
        # Verify all components are functional
        assert brain.spike_count == steps
        assert agent.reward_modulation_enabled is True
        assert agent.curiosity_enabled is True
        
        # Test integration utilities
        assert hasattr(brain, 'apply_neuromodulation')
        assert hasattr(agent, 'apply_reward_modulation')
        assert hasattr(world, 'apply_motor_command')
    
    def test_memory_system_integration(self):
        """Test memory system integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test memory system integration
        working_memory = brain.get_working_memory()
        episodic_memory = brain.get_episodic_memory()
        associative_memory = brain.get_associative_memory()
        
        assert working_memory is None  # Stub implementation
        assert episodic_memory is None
        assert associative_memory is None
        
        # Verify memory-related methods exist
        assert hasattr(brain, 'log_status')
        assert hasattr(brain, 'reset')
    
    def test_neuromodulation_integration(self):
        """Test neuromodulation integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test neuromodulation methods
        assert hasattr(brain, 'apply_neuromodulation')
        assert hasattr(brain, 'get_dopamine')
        assert hasattr(brain, 'get_curiosity')
        
        # Verify methods are callable
        brain.apply_neuromodulation(None)
        assert brain.get_dopamine() is None
        assert brain.get_curiosity() is None
    
    def test_prediction_system_integration(self):
        """Test prediction system integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test prediction system integration
        prediction_system = brain.get_prediction_system()
        planner = brain.get_planner()
        
        assert prediction_system is None
        assert planner is None
        
        # Verify prediction-related methods exist
        assert hasattr(brain, 'get_prediction_error_signal')
        assert hasattr(brain, 'get_stdp')
        assert hasattr(brain, 'get_hebbian')
        
        assert brain.get_prediction_error_signal() is None
        assert brain.get_stdp() is None
        assert brain.get_hebbian() is None