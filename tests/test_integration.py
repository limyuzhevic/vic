"""Test integration features of enhanced NLM Python bindings."""
import pytest
from pynlm import Brain, Config, AgentBrain, SimpleWorld
import pynlm.utils as utils
import time

class TestIntegration:
    """Test integration features and complete workflows."""
    
    def test_complete_brain_world_agent_loop(self):
        """Test complete brain-world-agent integration loop."""
        # Create all components
        config = Config()
        brain = Brain(config)
        world = SimpleWorld()
        agent = AgentBrain(brain)
        
        # Initialize all components
        brain.initialize()
        agent.initialize(world)
        
        # Configure agent features
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        
        # Run complete simulation loop
        steps = 10
        simulation_time = 0.0
        
        for step in range(steps):
            # Brain processes step
            brain.step(step, simulation_time)
            
            # World generates sensory percept
            percept = world.get_sensory_percept()
            
            # Agent processes sensory input
            agent.process_sensory_input(percept)
            
            # Agent produces action
            action = agent.decode_motor_command()
            
            # World applies action and provides reward
            world_result = world.apply_motor_command(action, simulation_time)
            
            # Update simulation time
            simulation_time += 0.01
        
        # Verify integration
        assert brain.spike_count == steps
        assert agent.reward_modulation_enabled is True
        assert agent.curiosity_enabled is True
        
        # Verify integration utilities are accessible
        assert hasattr(brain, 'apply_neuromodulation')
        assert hasattr(agent, 'apply_reward_modulation')
        assert hasattr(world, 'is_valid_position')
        assert hasattr(world, 'get_agent_body')
    
    def test_memory_system_integration(self):
        """Test memory system integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Access all memory systems
        working_memory = brain.get_working_memory()
        episodic_memory = brain.get_episodic_memory()
        associative_memory = brain.get_associative_memory()
        prediction_system = brain.get_prediction_system()
        planner = brain.get_planner()
        concept_formation = brain.get_concept_formation()
        attention = brain.get_attention()
        development_system = brain.get_development_system()
        dopamine = brain.get_dopamine()
        curiosity = brain.get_curiosity()
        novelty = brain.get_novelty()
        prediction_error = brain.get_prediction_error_signal()
        stdp = brain.get_stdp()
        hebbian = brain.get_hebbian()
        structural_plasticity = brain.get_structural_plasticity()
        spike_system = brain.get_spike_system()
        
        # All should return None in stub implementation
        assert working_memory is None
        assert episodic_memory is None
        assert associative_memory is None
        assert prediction_system is None
        assert planner is None
        assert concept_formation is None
        assert attention is None
        assert development_system is None
        assert dopamine is None
        assert curiosity is None
        assert novelty is None
        assert prediction_error is None
        assert stdp is None
        assert hebbian is None
        assert structural_plasticity is None
        assert spike_system is None
        
        # Verify log_status method exists
        assert hasattr(brain, 'log_status')
        brain.log_status()
    
    def test_neuromodulation_integration(self):
        """Test neuromodulation integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test neuromodulation features
        assert hasattr(brain, 'apply_neuromodulation')
        assert hasattr(brain, 'get_dopamine')
        assert hasattr(brain, 'get_curiosity')
        assert hasattr(brain, 'get_novelty')
        
        # Test neuromodulation application
        brain.apply_neuromodulation("test_signal")
        
        # Verify neuromodulation levels
        assert brain.get_dopamine() is None
        assert brain.get_curiosity() is None
        assert brain.get_novelty() is None
    
    def test_prediction_system_integration(self):
        """Test prediction system integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test prediction system features
        assert hasattr(brain, 'get_prediction_system')
        assert hasattr(brain, 'get_planner')
        assert hasattr(brain, 'get_prediction_error_signal')
        
        # Test prediction error
        error = brain.get_prediction_error_signal()
        assert error is None
    
    def test_development_lifecycle(self):
        """Test development lifecycle integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test development system
        assert hasattr(brain, 'get_development_system')
        assert hasattr(brain, 'develop')
        assert hasattr(brain, 'get_developmental_stage')
        
        # Test developmental stage access
        stage = brain.get_developmental_stage()
        assert stage == pynlm.DevelopmentalStage.Initial
        
        # Test development method
        brain.develop()
    
    def test_planner_integration(self):
        """Test planner integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test planner features
        planner = brain.get_planner()
        assert planner is None
        
        # Test action production
        action = brain.produce_action()
        assert action is not None
    
    def test_concept_formation_integration(self):
        """Test concept formation integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test concept formation
        concept = brain.get_concept_formation()
        assert concept is None
    
    def test_world_configuration_workflow(self):
        """Test complete world configuration workflow."""
        world = SimpleWorld()
        
        # Basic configuration
        assert world.configure(100, 100, 10, 10) is True
        assert world.get_width() == 100
        assert world.get_height() == 100
        
        # Advanced configuration
        advanced_config = {
            "max_energy": 300.0,
            "energy_decay_rate": 0.005,
            "random_seed": 12345,
            "agent_start_x": 5.0,
            "agent_start_y": 10.0
        }
        
        assert world.configure_advanced(**advanced_config) is True
        assert world.get_max_energy() == 300.0
        assert world.get_energy_decay_rate() == 0.005
        assert world.get_random_seed() == 12345
        
        # Test agent start configuration
        world.set_agent_start(15.0, 20.0)
        # Note: set_agent_start doesn't return the position, so we can't verify
        
        # Test world operations
        percept = world.get_sensory_percept()
        assert percept is not None
        
        body = world.get_agent_body()
        assert body is not None
        
        assert world.is_valid_position(50.0, 50.0) is True
        assert world.is_valid_position(-10.0, -10.0) is True

class TestMemorySystemIntegration:
    """Test comprehensive memory system integration."""
    
    def test_working_memory_integration(self):
        """Test working memory integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test working memory
        working_memory = brain.get_working_memory()
        assert working_memory is None
        
        # Test memory-related operations
        assert hasattr(brain, 'log_status')
        brain.log_status()
    
    def test_episodic_memory_integration(self):
        """Test episodic memory integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        episodic_memory = brain.get_episodic_memory()
        assert episodic_memory is None
        
        # Test memory state tracking
        brain.log_status()
    
    def test_associative_memory_integration(self):
        """Test associative memory integration."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        associative_memory = brain.get_associative_memory()
        assert associative_memory is None
        
        # Test memory association
        brain.log_status()

class TestDevelopmentLifecycleIntegration:
    """Test development lifecycle integration."""
    
    def test_brain_development_workflow(self):
        """Test complete brain development workflow."""
        config = Config()
        brain = Brain(config)
        
        # Initial state
        assert brain.get_developmental_stage() == pynlm.DevelopmentalStage.Initial
        
        # Initialize
        assert brain.initialize() is True
        
        # Develop
        brain.develop()
        assert brain.get_developmental_stage() == pynlm.DevelopmentalStage.Initial

class TestPerformanceIntegration:
    """Test performance and integration benchmarks."""
    
    def test_performance_benchmark(self):
        """Test performance benchmarking."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Run benchmark
        steps = 100
        start_time = time.time()
        
        brain.step_batch(steps)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Verify benchmark results
        benchmark_result = utils.benchmark_brain(brain, steps=10)
        assert isinstance(benchmark_result, dict)
        assert 'avg_step_time_ms' in benchmark_result
        assert 'total_steps' in benchmark_result
        assert benchmark_result['total_steps'] == 10
        
        # Verify elapsed time for batch processing
        assert elapsed < 1.0, f"Batch processing took too long: {elapsed}s"
    
    def test_batch_processing_efficiency(self):
        """Test batch processing efficiency."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Test step_batch efficiency
        steps = 50
        result = brain.step_batch(steps)
        assert result == steps
        assert brain.spike_count == steps
        
        # Time individual steps vs batch
        start_time = time.time()
        for _ in range(10):
            brain.step()
        individual_time = time.time() - start_time
        
        start_time = time.time()
        brain.step_batch(10)
        batch_time = time.time() - start_time
        
        # Batch should be at least as fast as individual (though overhead)
        assert batch_time <= individual_time * 2, "Batch processing should be efficient"
    
    def test_memory_usage_monitoring(self):
        """Test memory usage monitoring."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Get memory usage
        memory_info = utils.get_memory_usage()
        assert isinstance(memory_info, dict)
        assert 'rss_mb' in memory_info
        assert 'vms_mb' in memory_info
        
        assert memory_info['rss_mb'] > 0
        assert memory_info['vms_mb'] > 0
        
        # Run operations and check memory
        run_steps(brain, 20)
        
        memory_after = utils.get_memory_usage()
        assert isinstance(memory_after, dict)