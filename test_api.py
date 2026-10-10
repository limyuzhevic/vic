#!/usr/bin/env python3
"""
Tests for NLM High-Level API Abstractions

This module contains tests for the high-level API abstractions that simplify
common NLM use cases while maintaining backward compatibility.
"""

import pytest
import pynlm
import pynlm_api
from pynlm_api import (
    NLMAPI, 
    SimulationResult, 
    TrainingResult,
    create_simple_neural_simulation,
    create_agent_training_simulation,
    create_experiment_simulation
)
import time
import random


class TestNLMAPI:
    """Test the NLMAPI class and its factory methods."""
    
    def test_nlmapi_creation_with_default_config(self):
        """Test creating NLMAPI with default configuration."""
        api = NLMAPI()
        assert api._config is not None
        assert api._brain is None
        assert api._world is None
        assert api._agent is None
    
    def test_nlmapi_creation_with_custom_config(self):
        """Test creating NLMAPI with custom configuration."""
        config = pynlm.createDefaultConfig()
        api = NLMAPI(config)
        assert api._config is config
    
    def test_create_default_config(self):
        """Test static method to create default config."""
        config = NLMAPI.create_default_config()
        assert isinstance(config, pynlm.Config)
    
    def test_create_brain_config(self):
        """Test creating brain configuration with custom parameters."""
        config = NLMAPI.create_brain_config(
            neuron_count=2000,
            region_count=10,
            connection_probability=0.15
        )
        assert config is not None
        # Note: We can't easily verify the config values without accessing
        # the C++ backend directly
    
    def test_create_simple_brain(self):
        """Test creating a simple brain for basic simulation."""
        api = NLMAPI.create_simple_brain(neuron_count=500)
        assert api._brain is not None
        assert api._world is None
        assert api._agent is None
        assert api._brain.getTotalNeuronCount() > 0
    
    def test_create_agent_training_brain(self):
        """Test creating brain optimized for agent training."""
        api = NLMAPI.create_agent_training_brain(neuron_count=1000)
        assert api._brain is not None
        assert api._world is not None
        assert api._agent is not None
        assert api._world.getWidth() == 16
        assert api._world.getHeight() == 16
        assert api._agent.getBrain() is not None
    
    def test_create_experiment_setup(self):
        """Test creating experiment setup with configuration."""
        experiment_config = {
            'neuron_count': 1200,
            'world_size': 10
        }
        api = NLMAPI.create_experiment_setup(experiment_config)
        assert api._brain is not None
        assert api._world is not None
        assert api._agent is not None
    
    def test_create_experiment_setup_without_world(self):
        """Test creating experiment setup without world."""
        experiment_config = {
            'neuron_count': 800
        }
        api = NLMAPI.create_experiment_setup(experiment_config)
        assert api._brain is not None
        assert api._world is None
        assert api._agent is None


class TestConvenienceFunctions:
    """Test the convenience factory functions."""
    
    def test_create_simple_neural_simulation(self):
        """Test create_simple_neural_simulation function."""
        result = create_simple_neural_simulation(neuron_count=300, steps=50)
        assert isinstance(result, SimulationResult)
        assert result.steps_completed == 50
        assert result.metadata['simulation_type'] == 'basic'
        assert result.avg_firing_rate >= 0
    
    def test_create_agent_training_simulation(self):
        """Test create_agent_training_simulation function."""
        result = create_agent_training_simulation(
            neuron_count=800,
            episodes=10,
            steps_per_episode=30
        )
        assert isinstance(result, TrainingResult)
        assert result.experience_gained == 10 * 30
        assert 'final_avg_reward' in result.performance_metrics
    
    def test_create_experiment_simulation(self):
        """Test create_experiment_simulation function."""
        experiment_config = {
            'neuron_count': 900,
            'enable_development': True
        }
        result = create_experiment_simulation(
            experiment_config,
            "test_experiment",
            max_steps=50
        )
        assert isinstance(result, dict)
        assert result['experiment_name'] == "test_experiment"
        assert result['steps_completed'] == 50
        assert result['metadata']['experiment_type'] == 'standard'


class TestSimulationResults:
    """Test SimulationResult dataclass."""
    
    def test_simulation_result_creation(self):
        """Test creating SimulationResult with all fields."""
        result = SimulationResult(
            total_reward=10.5,
            avg_firing_rate=5.2,
            total_spikes=150,
            active_neurons=45,
            episodic_episodes=2.0,
            novelty_level=0.7,
            curiosity_level=0.8,
            dopamine_level=0.6,
            execution_time=1.5,
            steps_completed=100,
            metadata={'test': 'value'}
        )
        assert result.total_reward == 10.5
        assert result.avg_firing_rate == 5.2
        assert result.total_spikes == 150
        assert result.active_neurons == 45
        assert result.episodic_episodes == 2.0
        assert result.novelty_level == 0.7
        assert result.curiosity_level == 0.8
        assert result.dopamine_level == 0.6
        assert result.execution_time == 1.5
        assert result.steps_completed == 100
        assert result.metadata == {'test': 'value'}
    
    def test_simulation_result_default_metadata(self):
        """Test SimulationResult with default metadata."""
        result = SimulationResult(
            total_reward=0.0,
            avg_firing_rate=0.0,
            total_spikes=0,
            active_neurons=0,
            episodic_episodes=0.0,
            novelty_level=0.0,
            curiosity_level=0.0,
            dopamine_level=0.0,
            execution_time=0.0,
            steps_completed=0
        )
        assert result.metadata is None


class TestTrainingResults:
    """Test TrainingResult dataclass."""
    
    def test_training_result_creation(self):
        """Test creating TrainingResult with all fields."""
        learning_curve = [
            {'episode': 0, 'total_reward': 10.0, 'avg_reward': 10.0, 'firing_rate': 5.0},
            {'episode': 1, 'total_reward': 15.0, 'avg_reward': 15.0, 'firing_rate': 6.0}
        ]
        
        result = TrainingResult(
            performance_metrics={
                'final_avg_reward': 15.0,
                'total_reward': 25.0,
                'convergence_score': 0.8,
                'final_firing_rate': 6.0
            },
            experience_gained=100,
            learning_curve=learning_curve,
            final_behavior={
                'avg_firing_rate': 6.0,
                'total_spikes': 200,
                'novelty': 0.5,
                'curiosity': 0.6
            },
            convergence_score=0.8
        )
        assert result.performance_metrics['final_avg_reward'] == 15.0
        assert result.experience_gained == 100
        assert len(result.learning_curve) == 2
        assert result.final_behavior['avg_firing_rate'] == 6.0
        assert result.convergence_score == 0.8
    
    def test_training_result_empty_data(self):
        """Test TrainingResult with empty data."""
        result = TrainingResult(
            performance_metrics={},
            experience_gained=0,
            learning_curve=[],
            final_behavior={},
            convergence_score=0.0
        )
        assert result.experience_gained == 0
        assert len(result.learning_curve) == 0


class TestSimulationMethods:
    """Test simulation methods in NLMAPI."""
    
    def test_run_basic_simulation(self):
        """Test running basic simulation."""
        api = NLMAPI.create_simple_brain(neuron_count=500)
        result = api.run_basic_simulation(steps=30)
        
        assert isinstance(result, SimulationResult)
        assert result.steps_completed == 30
        assert result.metadata['simulation_type'] == 'basic'
        assert result.avg_firing_rate >= 0
        assert result.execution_time >= 0
    
    def test_run_basic_simulation_error_handling(self):
        """Test error handling for basic simulation without brain."""
        api = NLMAPI()  # Empty API
        with pytest.raises(ValueError, match="Brain not initialized"):
            api.run_basic_simulation(steps=10)
    
    def test_run_agent_simulation(self):
        """Test running agent simulation."""
        api = NLMAPI.create_agent_training_brain(neuron_count=800)
        result = api.run_agent_simulation(steps=40, enable_world=True)
        
        assert isinstance(result, SimulationResult)
        assert result.steps_completed == 40
        assert result.metadata['simulation_type'] == 'agent'
        assert result.metadata['enable_world'] == True
        assert result.total_reward >= 0
        assert result.execution_time >= 0
    
    def test_run_agent_simulation_without_world(self):
        """Test running agent simulation without world."""
        api = NLMAPI.create_simple_brain(neuron_count=500)
        # This should work but create a brain-only simulation
        result = api.run_agent_simulation(steps=20, enable_world=False)
        
        assert isinstance(result, SimulationResult)
        assert result.steps_completed == 20
        assert result.metadata['enable_world'] == False


class TestTrainingMethods:
    """Test training methods in NLMAPI."""
    
    def test_train_agent(self):
        """Test training agent with callback."""
        api = NLMAPI.create_agent_training_brain(neuron_count=600)
        
        # Simple callback that just tracks progress
        def dummy_callback(episode, step, total_reward, current_reward):
            pass
        
        result = api.train_agent(
            episodes=5,
            steps_per_episode=20,
            exploration_rate=0.1,
            learning_callback=dummy_callback
        )
        
        assert isinstance(result, TrainingResult)
        assert result.experience_gained == 5 * 20
        assert len(result.learning_curve) == 5
        assert result.convergence_score >= 0
        
        # Verify learning curve data
        for i, ep_data in enumerate(result.learning_curve):
            assert ep_data['episode'] == i
            assert 'total_reward' in ep_data
            assert 'avg_reward' in ep_data
            assert 'firing_rate' in ep_data
            assert 'novelty' in ep_data
            assert 'curiosity' in ep_data
    
    def test_train_agent_error_handling(self):
        """Test error handling for training without proper setup."""
        api = NLMAPI.create_simple_brain(neuron_count=500)
        with pytest.raises(ValueError, match="Complete training setup required"):
            api.train_agent(episodes=5, steps_per_episode=10)


class TestExperimentMethods:
    """Test experiment methods in NLMAPI."""
    
    def test_run_experiment(self):
        """Test running experiment."""
        api = NLMAPI.create_experiment_setup({
            'neuron_count': 700,
            'enable_development': True
        })
        
        result = api.run_experiment("test_exp", max_steps=30)
        
        assert isinstance(result, dict)
        assert result['experiment_name'] == "test_exp"
        assert result['steps_completed'] == 30
        assert 'total_reward' in result
        assert 'avg_reward' in result
        assert 'avg_firing_rate' in result
        assert 'final_developmental_stage' in result
        assert 'metadata' in result
        assert result['metadata']['experiment_type'] == 'standard'


class TestAnalysisAndVisualization:
    """Test analysis and visualization methods."""
    
    def test_analyze_simulation_data(self):
        """Test analyzing simulation data."""
        api = NLMAPI()
        
        # Create sample simulation data
        data = [
            {'step': 0, 'reward': 10.0, 'firing_rate': 5.0},
            {'step': 1, 'reward': 15.0, 'firing_rate': 6.0},
            {'step': 2, 'reward': 12.0, 'firing_rate': 5.5},
            {'step': 3, 'reward': 18.0, 'firing_rate': 7.0},
            {'step': 4, 'reward': 14.0, 'firing_rate': 6.5}
        ]
        
        analysis = api.analyze_simulation_data(data)
        
        assert isinstance(analysis, dict)
        assert 'steps' in analysis
        assert 'total_rewards' in analysis
        assert 'firing_rates' in analysis
        assert analysis['steps'] == [0, 1, 2, 3, 4]
        assert analysis['total_rewards'] == [10.0, 15.0, 12.0, 18.0, 14.0]
        assert analysis['firing_rates'] == [5.0, 6.0, 5.5, 7.0, 6.5]
        assert 'avg_reward' in analysis
        assert 'avg_firing_rate' in analysis
        assert 'stability_score' in analysis
        assert 'trend' in analysis
        assert 'peaks' in analysis
        assert 'valleys' in analysis
    
    def test_analyze_simulation_data_empty(self):
        """Test analyzing empty simulation data."""
        api = NLMAPI()
        analysis = api.analyze_simulation_data([])
        assert isinstance(analysis, dict)
        assert analysis == {}
    
    def test_create_visualization_data(self):
        """Test creating visualization data."""
        api = NLMAPI.create_simple_brain(neuron_count=500)
        
        # Create visualization data
        viz_data = api.create_visualization_data(api._brain)
        
        assert isinstance(viz_data, dict)
        assert 'neural_activity' in viz_data
        assert 'memory' in viz_data
        assert 'neuromodulation' in viz_data
        assert 'development' in viz_data
        
        neural = viz_data['neural_activity']
        assert 'total_neurons' in neural
        assert 'total_synapses' in neural
        assert 'active_neurons' in neural
        assert 'firing_rate' in neural
        assert 'excitatory_inhibitory_ratio' in neural
        assert 'total_spikes' in neural
        
        memory = viz_data['memory']
        assert 'working_memory_active_traces' in memory
        assert 'episodic_memory_episodes' in memory
        assert 'associative_memory_patterns' in memory
        
        neuromod = viz_data['neuromodulation']
        assert 'dopamine_level' in neuromod
        
        dev = viz_data['development']
        assert 'developmental_stage' in dev


class TestBackwardCompatibility:
    """Test backward compatibility wrappers."""
    
    def test_run_legacy_simulation(self):
        """Test legacy simulation wrapper."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        result = pynlm_api.run_legacy_simulation(brain, steps=25)
        
        assert isinstance(result, SimulationResult)
        assert result.steps_completed == 25
        assert result.metadata['simulation_type'] == 'basic'
    
    def test_run_legacy_agent_simulation(self):
        """Test legacy agent simulation wrapper."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        result = pynlm_api.run_legacy_agent_simulation(brain, world, agent, steps=30)
        
        assert isinstance(result, SimulationResult)
        assert result.steps_completed == 30
        assert result.metadata['simulation_type'] == 'agent'
        assert result.metadata['enable_world'] == True


class TestIntegration:
    """Integration tests for the high-level API."""
    
    def test_complete_workflow(self):
        """Test a complete workflow using multiple API features."""
        # Step 1: Create API instance
        api = NLMAPI.create_agent_training_brain(neuron_count=800)
        
        # Step 2: Run agent simulation
        sim_result = api.run_agent_simulation(steps=50, enable_world=True)
        assert sim_result.steps_completed == 50
        
        # Step 3: Create analysis data
        sample_data = [
            {'reward': sim_result.total_reward / sim_result.steps_completed,
             'firing_rate': sim_result.avg_firing_rate}
            for _ in range(10)
        ]
        analysis = api.analyze_simulation_data(sample_data)
        assert 'avg_reward' in analysis
        
        # Step 4: Create visualization data
        viz_data = api.create_visualization_data(api._brain)
        assert 'neural_activity' in viz_data
        
        # Step 5: Verify integration
        assert sim_result.execution_time >= 0
        assert analysis['stability_score'] >= 0
        assert viz_data['neural_activity']['total_neurons'] > 0
    
    def test_error_handling_integration(self):
        """Test error handling integration."""
        api = NLMAPI()  # Empty API
        
        # Test various error conditions
        with pytest.raises(ValueError, match="Brain not initialized"):
            api.run_basic_simulation(steps=10)
        
        with pytest.raises(ValueError, match="Complete training setup required"):
            api.train_agent(episodes=5, steps_per_episode=10)
        
        with pytest.raises(ValueError, match="Brain not initialized"):
            api.run_experiment("test", max_steps=10)


if __name__ == "__main__":
    # Run tests manually if script is executed directly
    pytest.main([__file__, "-v"])
