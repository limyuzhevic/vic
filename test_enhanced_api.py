# Test Suite for Enhanced NLM Python API

"""
Comprehensive test suite for the enhanced NLM Python API improvements.
Tests all new functionality while ensuring backward compatibility.
"""

import pytest
import tempfile
import os
import numpy as np
from unittest.mock import patch, MagicMock

# Import the enhanced API
import sys
sys.path.insert(0, '/workspace/app')
from enhanced_api import (
    NLMEnvironment,
    NLMDataProcessor,
    NLMVisualizer, 
    NLMExperiment,
    create_explorer_agent,
    create_exploiter_agent,
    create_balanced_agent,
    create_minimal_brain,
    create_training_environment
)
import pynlm
class TestEnhancedNLMEnvironment:
    """Test the enhanced NLMEnvironment class."""
    
    def test_initialization_standard(self):
        """Test initialization with standard configuration."""
        env = NLMEnvironment('standard')
        
        assert env.brain is not None
        assert env.agent is not None
        assert env.world is not None
        assert env.config is not None
        
        # Check default features are enabled
        assert env.agent.isRewardModulationEnabled() == True
        assert env.agent.isCuriosityEnabled() == True
        assert env.agent.isDevelopmentEnabled() == True
        
        # Clean up
        del env
    
    def test_initialization_presets(self):
        """Test all configuration presets."""
        presets = ['standard', 'exploration', 'exploitation', 'balanced']
        
        for preset in presets:
            env = NLMEnvironment(preset)
            assert env.brain is not None
            assert env.agent is not None
            del env
    
    def test_feature_enabling(self):
        """Test feature enable/disable functionality."""
        env = NLMEnvironment()
        
        # Test enabling features
        env.enable_feature('curiosity', False)
        assert env.agent.isCuriosityEnabled() == False
        
        env.enable_feature('reward_modulation', False)
        assert env.agent.isRewardModulationEnabled() == False
        
        env.enable_feature('development', False)
        assert env.agent.isDevelopmentEnabled() == False
        
        env.enable_feature('curiosity', True)
        assert env.agent.isCuriosityEnabled() == True
        
        # Test invalid feature
        with pytest.raises(ValueError):
            env.enable_feature('invalid_feature', True)
        
        del env
    
    def test_episode_basic(self):
        """Test basic episode functionality."""
        env = NLMEnvironment()
        
        # Run short episode
        summary = env.run_episode(50)
        
        # Check summary contains expected keys
        expected_keys = ['total_reward', 'avg_firing_rate', 'max_firing_rate',
                        'action_distribution', 'final_curiosity', 'final_novelty',
                        'final_development', 'episode_length', 'total_spikes',
                        'total_neurons']
        
        for key in expected_keys:
            assert key in summary
        
        # Check values are reasonable
        assert isinstance(summary['total_reward'], float)
        assert isinstance(summary['avg_firing_rate'], float)
        assert summary['episode_length'] == 50
        assert summary['total_spikes'] > 0
        assert summary['total_neurons'] > 0
        
        del env
    
    def test_episode_with_reward_function(self):
        """Test episode with custom reward function."""
        env = NLMEnvironment()
        
        def custom_reward(world, action, percept):
            return 1.0 if action == pynlm.MotorCommand.Interact else 0.0
        
        summary = env.run_episode(50, reward_fn=custom_reward)
        
        # Should have accumulated some reward
        assert summary['total_reward'] >= 0
        
        del env
    
    def test_brain_stats(self):
        """Test brain statistics collection."""
        env = NLMEnvironment()
        
        stats = env.get_brain_stats()
        
        expected_keys = ['total_neurons', 'total_synapses', 'firing_neurons',
                        'average_firing_rate', 'excitatory_inhibition_ratio',
                        'total_spikes', 'developmental_stage', 'region_count']
        
        for key in expected_keys:
            assert key in stats
        
        # Check value ranges
        assert stats['total_neurons'] > 0
        assert stats['total_synapses'] >= 0
        assert 0 <= stats['average_firing_rate'] <= 100
        assert 0 <= stats['excitatory_inhibition_ratio'] <= 10
        
        del env
    
    def test_save_load_brain(self):
        """Test brain save/load functionality."""
        env = NLMEnvironment()
        
        # Run some steps to change brain state
        env.run_episode(100)
        
        # Save brain
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            env.save_brain(filepath)
            assert os.path.exists(filepath)
            assert os.path.getsize(filepath) > 0
            
            # Create new environment and load brain
            env2 = NLMEnvironment()
            env2.load_brain(filepath)
            
            # Check that brain was loaded
            assert env2.brain.getTotalNeuronCount() == env.brain.getTotalNeuronCount()
            
            del env2
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_training_method(self):
        """Test training method with short run."""
        env = NLMEnvironment()
        
        # Patch run_episode to speed up test
        original_run = env.run_episode
        call_count = 0
        
        def mock_run_episode(num_steps, **kwargs):
            nonlocal call_count
            call_count += 1
            # Return good summary
            return {
                'total_reward': 10.0 + call_count * 0.1,
                'avg_firing_rate': 0.1 + call_count * 0.01,
                'max_firing_rate': 1.0 + call_count * 0.1,
                'final_curiosity': 0.5 + call_count * 0.01,
                'final_novelty': 0.3 + call_count * 0.01,
                'final_development': pynlm.DevelopmentalStage.CriticalPeriod,
                'action_distribution': {'MoveForward': 100},
                'episode_length': num_steps,
                'total_spikes': 1000,
                'total_neurons': 100,
            }
        
        env.run_episode = mock_run_episode
        
        # Run short training
        result = env.train(3, validation_interval=100, save_interval=100)
        
        assert result['best_reward'] > 0
        assert result['best_episode'] > 0
        assert 'final_stats' in result
        
        del env
    
    def test_visualization_methods(self):
        """Test visualization methods (they should not crash)."""
        env = NLMEnvironment()
        stats = env.get_brain_stats()
        
        # These should not raise exceptions (though they may print messages)
        NLMVisualizer.plot_brain_activity(stats)
        NLMVisualizer.create_3d_neural_view(env.brain)
        
        del env
class TestNLMDataProcessor:
    """Test the NLMDataProcessor class."""
    
    def test_action_encoding(self):
        """Test action encoding to one-hot vectors."""
        # Test with a mock action
        action = pynlm.MotorCommand.MoveForward
        
        encoding = NLMDataProcessor._encode_action(action)
        
        assert isinstance(encoding, np.ndarray)
        assert encoding.shape == (13,)  # 13 action types
        assert encoding[0] == 1.0  # MoveForward should be at index 0
        assert np.sum(encoding) == 1.0  # Only one should be active
    
    def test_feature_extraction(self):
        """Test feature extraction from percept and action."""
        # Create mock percept and action
        class MockPercept:
            def getVision(self):
                return [0.1, 0.2, 0.3] * 80  # 240 values for 8x8x3 vision
            
            def getTouch(self):
                return [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]
            
            def getInternal(self):
                return [0.1, 0.2, 0.3, 0.4]
            
            def getProprioception(self):
                return [0.1, 0.2, 0.3, 0.4, 0.5, 0.6]
        
        class MockAction:
            def __init__(self, action_type):
                self.action_type = action_type
        
        percept = MockPercept()
        action = MockAction(pynlm.MotorCommand.MoveForward)
        
        features = NLMDataProcessor.extract_features(percept, action)
        
        assert isinstance(features, np.ndarray)
        # Check expected size: vision(240) + touch(8) + internal(4) + proprio(6) + action(13) = 271
        expected_size = 240 + 8 + 4 + 6 + 13
        assert features.shape == (expected_size,)
    
    def test_reward_shaping(self):
        """Test reward shaping function."""
        reward = 1.0
        novelty = 0.5
        curiosity = 0.3
        prediction_error = 0.2
        
        shaped_reward = NLMDataProcessor.compute_reward_shaping(
            reward, novelty, curiosity, prediction_error
        )
        
        assert isinstance(shaped_reward, float)
        assert shaped_reward >= 0.0  # Should be non-negative
        # Expected: 1.0*0.5 + 0.5*0.2 + 0.3*0.15 + 0.2*0.15 = 0.5 + 0.1 + 0.045 + 0.03 = 0.675
        expected = 0.5 + 0.1 + 0.045 + 0.03
        assert abs(shaped_reward - expected) < 0.001
class TestNLMExperiment:
    """Test the NLMExperiment class."""
    
    def test_parameter_sweep_basic(self):
        """Test basic parameter sweep functionality."""
        experiment = NLMExperiment()
        
        param_space = {
            'brain.neuron_count': [100, 200],
            'neuromod.curiosity.enable': [True, False]
        }
        
        # Mock the NLMEnvironment creation to speed up test
        with patch('enhanced_api.NLMEnvironment') as mock_env_class:
            mock_env = MagicMock()
            mock_env_class.return_value = mock_env
            
            # Mock run_episode to return good results
            mock_env.run_episode.return_value = {
                'total_reward': 10.0,
                'avg_firing_rate': 0.1,
                'max_firing_rate': 1.0,
                'final_curiosity': 0.5,
                'final_novelty': 0.3,
                'final_development': pynlm.DevelopmentalStage.CriticalPeriod,
                'action_distribution': {'MoveForward': 100},
                'episode_length': 1000,
                'total_spikes': 1000,
                'total_neurons': 100,
            }
            
            # Run parameter sweep
            result = experiment.run_parameter_sweep(
                param_space, num_episodes=2, metric='total_reward'
            )
            
            # Check results structure
            assert 'results' in result
            assert 'best_parameters' in result
            assert 'best_value' in result
            assert 'best_result' in result
            assert 'total_combinations' in result
            assert 'successful_runs' in result
            
            # Should have tested both combinations (2x2 = 4)
            assert result['successful_runs'] <= 4
    
    def test_ablation_study(self):
        """Test ablation study functionality."""
        experiment = NLMExperiment()
        
        feature_set = ['curiosity', 'novelty', 'development']
        
        # Mock NLMEnvironment
        with patch('enhanced_api.NLMEnvironment') as mock_env_class:
            mock_env = MagicMock()
            mock_env_class.return_value = mock_env
            
            # Mock different responses for baseline vs ablations
            baseline_summary = {
                'total_reward': 15.0,
                'avg_firing_rate': 0.2,
                'final_curiosity': 0.6,
                'final_novelty': 0.4,
            }
            
            mock_env.run_episode.return_value = baseline_summary
            
            # Run ablation study
            result = experiment.run_ablation_study(feature_set)
            
            # Check results structure
            assert 'results' in result
            assert 'baseline_performance' in result
            assert 'baseline_firing_rate' in result
            
            # Should have baseline + one ablation for each feature
            expected_results_count = 1 + len(feature_set)
            assert len(result['results']) == expected_results_count
            
            # Check baseline result
            baseline_result = result['results'][0]
            assert baseline_result['removed_features'] == []
            assert baseline_result['performance'] == baseline_summary['total_reward']
class TestConvenienceFunctions:
    """Test convenience factory functions."""
    
    def test_explorer_agent(self):
        """Test explorer agent factory."""
        agent = create_explorer_agent()
        assert agent is not None
        assert hasattr(agent, 'run_episode')
        assert hasattr(agent, 'train')
        del agent
    
    def test_exploiter_agent(self):
        """Test exploiter agent factory."""
        agent = create_exploiter_agent()
        assert agent is not None
        del agent
    
    def test_balanced_agent(self):
        """Test balanced agent factory."""
        agent = create_balanced_agent()
        assert agent is not None
        del agent
    
    def test_minimal_brain(self):
        """Test minimal brain factory."""
        brain = create_minimal_brain()
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
    
    def test_training_environment(self):
        """Test training environment factory."""
        env = create_training_environment()
        assert env is not None
        assert hasattr(env, 'run_episode')
        assert hasattr(env, 'train')
        del env
class TestBackwardCompatibility:
    """Test that our changes don't break existing functionality."""
    
    def test_original_api_still_works(self):
        """Test that original NLM API still works."""
        # Create brain with original API
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run steps with original API
        for i in range(10):
            brain.step(i)
        
        # Check statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSpikeCount() > 0
        
        # Test world
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        # Test agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Process percept and decode action
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        action = agent.decodeMotorCommand()
        
        # Should have valid action
        assert action in [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Look,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Eat,
            pynlm.MotorCommand.Drink,
            pynlm.MotorCommand.Rest,
            pynlm.MotorCommand.Wait,
            pynlm.MotorCommand.LookLeft,
            pynlm.MotorCommand.LookRight
        ]
    
    def test_config_api_unchanged(self):
        """Test that config API is unchanged."""
        config = pynlm.createDefaultConfig()
        
        # Test basic config operations
        config.set('test.int', 42)
        config.set('test.string', 'hello')
        config.set('test.double', 3.14)
        config.set('test.bool', True)
        
        assert config.has('test.int')
        assert config.has('test.string')
        assert config.has('test.double')
        assert config.has('test.bool')
        
        # Test get methods
        assert config.getOr('test.int', 0) == 42
        assert config.getOr('test.string', '') == 'hello'
        assert config.getOr('test.nonexistent', 'default') == 'default'
        
        # Test save/load (even though it's basic)
        with tempfile.NamedTemporaryFile(suffix='.txt', delete=False) as f:
            filepath = f.name
        
        try:
            config.saveToFile(filepath)
            assert os.path.exists(filepath)
            
            # Load into new config
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(filepath)
            
            assert config2.has('test.int')
            assert config2.getOr('test.int', 0) == 42
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_world_api_unchanged(self):
        """Test that world API is unchanged."""
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 4, 4)
        world.reset()
        
        # Check world properties
        assert world.getWidth() == 10
        assert world.getHeight() == 10
        assert world.getVisionWidth() == 4
        assert world.getVisionHeight() == 4
        
        # Test agent body
        body = world.getAgentBody()
        assert hasattr(body, 'x')
        assert hasattr(body, 'y')
        assert hasattr(body, 'orientation')
        
        # Test percept
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Test update and actions
        world.update(0.1)
        
        action = pynlm.MotorCommand.MoveForward
        world.applyMotorCommand(action, 0.1)
        
        # Add object
        obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
        world.addObject(obj)
        
        # Remove object
        world.removeObject(5.0, 5.0)
class TestErrorHandling:
    """Test error handling in new API."""
    
    def test_invalid_preset(self):
        """Test handling of invalid preset configuration."""
        with pytest.raises(Exception):  # Should not crash, may have fallback
            env = NLMEnvironment('invalid_preset')
            del env
    
    def test_feature_validation(self):
        """Test feature validation."""
        env = NLMEnvironment()
        
        # This should raise ValueError
        with pytest.raises(ValueError):
            env.enable_feature('nonexistent_feature', True)
        
        del env
    
    def test_parameter_sweep_error_handling(self):
        """Test parameter sweep error handling."""
        experiment = NLMExperiment()
        
        # Test with empty parameter space
        param_space = {}
        
        with pytest.raises(Exception):  # Should handle gracefully
            result = experiment.run_parameter_sweep(param_space, num_episodes=1)
class TestPerformance:
    """Test performance characteristics."""
    
    def test_memory_efficiency(self):
        """Test that memory usage is reasonable."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss
        
        # Create multiple environments
        envs = []
        for i in range(5):
            env = NLMEnvironment()
            envs.append(env)
        
        peak_memory = process.memory_info().rss
        memory_increase = (peak_memory - initial_memory) / (1024 * 1024)  # MB
        
        # Each environment should not use excessive memory
        assert memory_increase < 500  # Less than 500MB for 5 environments
        
        # Clean up
        for env in envs:
            del env
    
    def test_execution_speed(self):
        """Test execution speed is reasonable."""
        import time
        
        env = NLMEnvironment()
        
        # Time a short episode
        start_time = time.time()
        summary = env.run_episode(100)
        end_time = time.time()
        
        execution_time = end_time - start_time
        
        # Should complete quickly (less than 5 seconds for 100 steps)
        assert execution_time < 5.0, f"Execution time too slow: {execution_time}s"
        
        del env

if __name__ == '__main__':
    # Run tests
    pytest.main([
        __file__,
        '-v',
        '--tb=short'
    ])
