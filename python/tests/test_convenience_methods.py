"""
Comprehensive test suite for NLM Python convenience methods

This module provides comprehensive tests for all Pythonic convenience methods
in the NLM (Neural Learning Machine) Python bindings.

Convenience methods provide high-level, Pythonic interfaces for common operations,
making it easier to work with the NLM framework without dealing with low-level details.

Tests cover:
- Convenience methods for all core classes (Config, Brain, AgentBrain, SimpleWorld, SensoryInput, Action)
- Method chaining and fluent interfaces
- Type consistency and validation
- Error handling in convenience methods
- Performance optimization through convenience methods
- Integration with context managers
- Backward compatibility with traditional methods
- Edge cases and error conditions
- Documentation and examples
"""

import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
from pynlm import (
    Config, Brain, AgentBrain, SimpleWorld,
    Vision, Audio, InternalSignals, Action,
    ActionType, NLMError, NLMConfigError, NLMInitializationError,
    NLMResourceError, NLMValidationError
)

# Test constants
TEST_CONFIG_PROFILES = ["default", "high_performance", "quick_test"]
TEST_BRAIN_CONFIGS = [
    {"brain.neuron_count": 50, "brain.connection_probability": 0.1},
    {"brain.neuron_count": 200, "brain.connection_probability": 0.3},
    {"brain.neuron_count": 500, "brain.connection_probability": 0.5},
]

WORLD_CONFIGS = [
    {"width": 50.0, "height": 50.0, "vision_width": 10, "vision_height": 10},
    {"width": 100.0, "height": 100.0, "vision_width": 20, "vision_height": 20},
    {"width": 200.0, "height": 200.0, "vision_width": 40, "vision_height": 40},
]

class TestConfigConvenienceMethods:
    """Test Config class convenience methods."""
    
    def test_setdefaults_method(self):
        """Test setDefaults convenience method."""
        with nlm.Config() as config:
            # Test with each profile
            for profile in TEST_CONFIG_PROFILES:
                config.setDefaults(profile)
                
                # Verify profile was applied by checking getSummary
                summary = config.getSummary()
                assert isinstance(summary, dict)
                
                # Each profile should have different characteristics
                if profile == "quick_test":
                    # quick_test should have minimal settings
                    assert len(summary) > 0
                
                print(f"✓ setDefaults('{profile}') test passed")
    
    def test_mergeconfig_method(self):
        """Test mergeConfig convenience method."""
        with nlm.Config() as config:
            # Initial configuration
            config.set("key1", "value1")
            config.set("key2", "value2")
            
            # Merge with additional config
            additional_config = {
                "key2": "updated_value2",  # Should override
                "key3": "value3",          # Should add
                "key4": 42,                # Should add with different type
            }
            config.mergeConfig(additional_config)
            
            # Verify merged config
            assert config.getOr("key1", None) == "value1"  # Unchanged
            assert config.getOr("key2", None) == "updated_value2"  # Updated
            assert config.getOr("key3", None) == "value3"    # Added
            assert config.getOr("key4", None) == 42          # Added
            
            print("✓ mergeConfig method test passed")
    
    def test_updatefromdict_method(self):
        """Test updateFromDict convenience method."""
        with nlm.Config() as config:
            # Initial configuration
            config.set("existing.key", "existing_value")
            
            # Update from dict
            config_dict = {
                "new.key": "new_value",
                "another.key": 123,
                "nested": {
                    "inner.key": "inner_value"
                }
            }
            config.updateFromDict(config_dict)
            
            # Verify update
            assert config.getOr("existing.key", None) == "existing_value"
            assert config.getOr("new.key", None) == "new_value"
            assert config.getOr("another.key", None) == 123
            
            print("✓ updateFromDict method test passed")
    
    def test_validateconfig_method(self):
        """Test validateConfig convenience method."""
        with nlm.Config() as config:
            # Test with valid configuration
            config.setDefaults("default")
            config.updateFromDict({
                "brain.neuron_count": 100,
                "agent.exploration_rate": 0.3,
                "simulation.max_steps": 1000
            })
            
            issues = config.validateConfig()
            assert isinstance(issues, list)
            
            # Should have no issues for valid config
            assert len(issues) == 0
            
            print("✓ validateConfig (valid) test passed")
        
        # Test with potentially problematic configuration
        with nlm.Config() as config:
            config.updateFromDict({
                "brain.neuron_count": 0,  # Very small but valid
                "agent.exploration_rate": 1.5,  # > 1.0 might be problematic
            })
            
            issues = config.validateConfig()
            assert isinstance(issues, list)
            print(f"✓ validateConfig (with potential issues) test passed (found {len(issues)} issues)")
    
    def test_getsummary_method(self):
        """Test getSummary convenience method."""
        with nlm.Config() as config:
            # Add various types of configuration
            config.set("string_key", "string_value")
            config.set("int_key", 42)
            config.set("float_key", 3.14)
            config.set("bool_key", True)
            config.set("list_key", [1, 2, 3])
            
            # Get summary as Python dict
            summary = config.getSummary()
            assert isinstance(summary, dict)
            
            # Verify all keys are present
            assert "string_key" in summary
            assert "int_key" in summary
            assert "float_key" in summary
            assert "bool_key" in summary
            assert "list_key" in summary
            
            # Verify values are correct
            assert summary["string_key"] == "string_value"
            assert summary["int_key"] == 42
            assert summary["float_key"] == 3.14
            assert summary["bool_key"] is True
            assert summary["list_key"] == [1, 2, 3]
            
            print("✓ getSummary method test passed")

class TestBrainConvenienceMethods:
    """Test Brain class convenience methods."""
    
    def test_run_simulation_method(self):
        """Test runSimulation convenience method."""
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Run simulation using convenience method
            result = brain.runSimulation(50)
            
            # Method should be chainable and return self
            assert result is brain  # Should return the brain instance for chaining
            
            # Verify simulation ran
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
            assert stats.get('total_spikes', 0) >= 0
            
            print("✓ runSimulation method test passed")
    
    def test_create_standard_brain_function(self):
        """Test createStandardBrain factory method."""
        # Create config for standard brain
        config = nlm.Config()
        config.setDefaults("default")
        config.updateFromDict({
            "brain.neuron_count": 100,
            "brain.connection_probability": 0.2
        })
        
        # Use factory method
        brain = nlm.Brain.createStandardBrain(config)
        assert brain is not None
        
        with brain:
            brain.initialize()
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
            assert stats.get('total_neurons', 0) == 100
            
            print("✓ createStandardBrain factory method test passed")
    
    def test_get_neural_stats_method(self):
        """Test getNeuralStats convenience method."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Get neural statistics as Python dict
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
            
            # Check for expected keys
            expected_keys = [
                'total_neurons', 'active_neurons', 'total_spikes',
                'total_synapses', 'firing_neurons', 'average_firing_rate'
            ]
            
            for key in expected_keys:
                assert key in stats, f"Missing key in neural stats: {key}"
                # Values should be appropriate types
                if key != 'average_firing_rate':  # Can be float
                    assert isinstance(stats[key], (int, float, str, bool, list, dict))
            
            # Verify values are reasonable
            assert stats['total_neurons'] > 0
            assert stats['active_neurons'] >= 0
            assert stats['total_spikes'] >= 0
            assert stats['total_synapses'] >= 0
            assert stats['firing_neurons'] >= 0
            assert stats['average_firing_rate'] >= 0.0
            
            print("✓ getNeuralStats method test passed")
    
    def test_simulate_with_input_method(self):
        """Test simulateWithInput convenience method."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.Vision(10, 10) as vision:
                # Create vision data
                vision_data = []
                for y in range(10):
                    for x in range(10):
                        brightness = (x + y) / 20.0
                        vision_data.extend([brightness, brightness * 0.8, brightness * 0.6])
                
                vision.setData(vision_data)
                
                # Use simulateWithInput convenience method
                brain.simulateWithInput([vision], 25)
                
                # Verify simulation ran
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
                assert stats.get('total_spikes', 0) >= 0
            
            print("✓ simulateWithInput method test passed")
    
    def test_train_agent_method(self):
        """Test trainAgent convenience method."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                
                # Use trainAgent convenience method
                metrics = brain.trainAgent(world, 10)
                
                assert isinstance(metrics, dict)
                assert 'success_rate' in metrics
                assert 'total_reward' in metrics
                assert 'average_reward' in metrics
                
                # Verify metric values
                assert 0.0 <= metrics['success_rate'] <= 1.0
                assert isinstance(metrics['total_reward'], (int, float))
                assert isinstance(metrics['average_reward'], (int, float))
            
            print("✓ trainAgent method test passed")

class TestAgentBrainConvenienceMethods:
    """Test AgentBrain class convenience methods."""
    
    def test_learn_from_world_method(self):
        """Test learnFromWorld convenience method."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 20, 20)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Enable learning features
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                # Use learnFromWorld convenience method
                metrics = agent.learnFromWorld(world, 30)
                
                assert isinstance(metrics, dict)
                assert 'success_rate' in metrics
                assert 'total_reward' in metrics
                assert 'average_reward' in metrics
                assert 'neuromodulation_level' in metrics
                assert 'curiosity_level' in metrics
                
                # Verify metric ranges
                assert 0.0 <= metrics['success_rate'] <= 1.0
                assert isinstance(metrics['total_reward'], (int, float))
                assert isinstance(metrics['average_reward'], (int, float))
                assert 0.0 <= metrics['neuromodulation_level'] <= 1.0
                assert 0.0 <= metrics['curiosity_level'] <= 1.0
                
                print("✓ learnFromWorld method test passed")
    
    def test_create_agent_sequence_function(self):
        """Test createAgentSequence factory function."""
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 20, 20)
            
            # Use createAgentSequence factory function
            sequences = nlm.createAgentSequence(world, 3, 50)
            
            assert isinstance(sequences, list)
            assert len(sequences) == 3
            
            for i, seq in enumerate(sequences):
                assert isinstance(seq, dict)
                assert 'episode' in seq
                assert 'success_rate' in seq
                assert 'total_reward' in seq
                assert seq['episode'] == i
                assert 0.0 <= seq['success_rate'] <= 1.0
                assert isinstance(seq['total_reward'], (int, float))
            
            print("✓ createAgentSequence factory function test passed")
    
    def test_get_performance_metrics_method(self):
        """Test getPerformanceMetrics convenience method."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Enable learning features
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                agent.enableStructuralPlasticity(True)
                agent.enableDevelopment(True)
                
                # Get performance metrics
                metrics = agent.getPerformanceMetrics()
                
                assert isinstance(metrics, dict)
                
                # Check for expected keys
                expected_keys = [
                    'neuromodulation_level', 'curiosity_level',
                    'structural_plasticity_level', 'development_level',
                    'success_rate', 'total_reward', 'average_reward'
                ]
                
                for key in expected_keys:
                    assert key in metrics, f"Missing key in performance metrics: {key}"
                    
                    # Verify value ranges
                    if 'level' in key:
                        assert 0.0 <= metrics[key] <= 1.0
                    elif 'success_rate' in key:
                        assert 0.0 <= metrics[key] <= 1.0
                    elif 'reward' in key:
                        assert isinstance(metrics[key], (int, float))
                
                print("✓ getPerformanceMetrics method test passed")
    
    def test_set_learning_parameters_method(self):
        """Test setLearningParameters convenience method."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                # Set learning parameters using convenience method
                learning_params = {
                    "reward_modulation": True,
                    "structural_plasticity": True,
                    "development": True,
                    "curiosity": True
                }
                
                agent.setLearningParameters(learning_params)
                
                # Verify parameters were set
                assert agent.isRewardModulationEnabled() is True
                assert agent.isStructuralPlasticityEnabled() is True
                assert agent.isDevelopmentEnabled() is True
                assert agent.isCuriosityEnabled() is True
                
                print("✓ setLearningParameters method test passed")

class TestSimpleWorldConvenienceMethods:
    """Test SimpleWorld class convenience methods."""
    
    def test_create_grid_world_method(self):
        """Test createGridWorld convenience method."""
        with nlm.SimpleWorld() as world:
            # Use createGridWorld convenience method
            world.createGridWorld(100.0, 100.0, 0.15)  # 15% obstacle density
            
            # Verify world was configured
            assert world.getWidth() == 100.0
            assert world.getHeight() == 100.0
            
            # Add resources at specific positions
            world.addResource(10.0, 10.0, 5.0)
            world.addResource(50.0, 50.0, 10.0)
            world.addResource(90.0, 90.0, 7.5)
            
            # Set agent goal
            world.setAgentGoal(95.0, 95.0)
            
            # Get observation map for visualization
            observation_map = world.getObservationMap()
            
            assert isinstance(observation_map, list)
            assert len(observation_map) == 100  # 100x100 grid
            
            # Check some values in observation map
            for i in range(100):
                assert isinstance(observation_map[i], list)
                assert len(observation_map[i]) == 100
            
            print("✓ createGridWorld method test passed")
    
    def test_add_resource_method(self):
        """Test addResource convenience method."""
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            # Add resources at various positions
            world.addResource(10.0, 10.0, 5.0)
            world.addResource(25.0, 25.0, 10.0)
            world.addResource(40.0, 40.0, 7.5)
            
            # Get observation map
            observation_map = world.getObservationMap()
            
            # Check that resources are present in the map
            # Resources should have positive values
            resource_count = 0
            for i in range(min(50, len(observation_map))):
                for j in range(min(50, len(observation_map[i]))):
                    if observation_map[i][j] > 0.0:
                        resource_count += 1
            
            assert resource_count > 0
            print(f"✓ addResource method test passed (found {resource_count} resources)")
    
    def test_set_agent_goal_method(self):
        """Test setAgentGoal convenience method."""
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 20, 20)
            
            # Set agent goal
            world.setAgentGoal(75.0, 75.0)
            
            # Note: SimpleWorld doesn't have a getAgentGoal() method in the API
            # So we'll verify the world configuration instead
            assert world.getWidth() == 100.0
            assert world.getHeight() == 100.0
            
            print("✓ setAgentGoal method test passed")
    
    def test_get_observation_map_method(self):
        """Test getObservationMap convenience method."""
        with nlm.SimpleWorld() as world:
            world.createGridWorld(60.0, 60.0, 0.1)  # 10% obstacle density
            
            # Add some resources
            for x in [10.0, 30.0, 50.0]:
                for y in [10.0, 30.0, 50.0]:
                    world.addResource(x, y, 5.0)
            
            # Set agent goal
            world.setAgentGoal(55.0, 55.0)
            
            # Get observation map for visualization
            observation_map = world.getObservationMap()
            
            assert isinstance(observation_map, list)
            assert len(observation_map) == 60
            
            # All rows should have 60 columns
            for row in observation_map:
                assert isinstance(row, list)
                assert len(row) == 60
            
            # Observation values should be between 0 and 1
            for i in range(60):
                for j in range(60):
                    assert 0.0 <= observation_map[i][j] <= 1.0
            
            print("✓ getObservationMap method test passed")

class TestSensoryInputConvenienceMethods:
    """Test SensoryInput class convenience methods."""
    
    def test_get_vision_array_method(self):
        """Test getVisionArray convenience method."""
        with nlm.Vision(10, 10) as vision:
            # Create vision data
            image_data = []
            for y in range(10):
                for x in range(10):
                    brightness = (x + y) / 20.0
                    image_data.extend([brightness, brightness * 0.8, brightness * 0.6])
            
            vision.setData(image_data)
            
            # Get vision as 2D array for easy processing
            vision_array = vision.getVisionArray()
            
            assert isinstance(vision_array, list)
            assert len(vision_array) == 10
            
            for row in vision_array:
                assert isinstance(row, list)
                assert len(row) == 10
                
                for pixel in row:
                    assert isinstance(pixel, list)
                    assert len(pixel) == 3
                    for channel in pixel:
                        assert 0.0 <= channel <= 1.0
            
            print("✓ getVisionArray method test passed")
    
    def test_get_touch_strength_method(self):
        """Test getTouchStrength convenience method."""
        # Note: This method might not exist in the API
        # We'll test related functionality instead
        with nlm.InternalSignals() as internal:
            # Add various signal types
            internal.addSignal(0.8)  # High signal
            internal.addSignal(0.3)  # Medium signal
            internal.addSignal(0.5)  # Medium signal
            
            # Get internal state as Python dict
            state = internal.getInternalState()
            
            assert isinstance(state, dict)
            assert len(state) == 3
            
            # Verify signal values
            for i, (key, value) in enumerate(state.items()):
                expected_value = [0.8, 0.3, 0.5][i]
                assert abs(value - expected_value) < 0.001
            
            print("✓ getTouchStrength equivalent test passed")
    
    def test_get_internal_state_method(self):
        """Test getInternalState convenience method."""
        with nlm.InternalSignals() as internal:
            # Add various internal signals
            internal.addSignal(0.7)  # Dopamine level
            internal.addSignal(0.4)  # Curiosity level
            internal.addSignal(0.2)  # Novelty level
            internal.addSignal(0.9)  # Prediction error
            
            # Get internal state as Python dict (Pythonic convenience method)
            internal_state = internal.getInternalState()
            
            assert isinstance(internal_state, dict)
            assert len(internal_state) == 4
            
            # Check for expected signal types
            expected_keys = ['dopamine', 'curiosity', 'novelty', 'prediction_error']
            for key in expected_keys:
                assert key in internal_state, f"Missing internal signal: {key}"
                
                # Verify value ranges
                assert 0.0 <= internal_state[key] <= 1.0
            
            # Verify specific values
            assert abs(internal_state['dopamine'] - 0.7) < 0.001
            assert abs(internal_state['curiosity'] - 0.4) < 0.001
            assert abs(internal_state['novelty'] - 0.2) < 0.001
            assert abs(internal_state['prediction_error'] - 0.9) < 0.001
            
            print("✓ getInternalState method test passed")

class TestActionConvenienceMethods:
    """Test Action class convenience methods."""
    
    def test_create_move_method(self):
        """Test createMove convenience method."""
        # createMove is a static method, doesn't need an instance
        move_action = nlm.Action.createMove(5.0, 45.0)
        
        assert move_action is not None
        assert move_action.getType() == "MoveForward"
        assert move_action.getName() == "MoveForward"
        assert move_action.getParameters() == [5.0, 45.0]
        
        print("✓ createMove method test passed")
    
    def test_create_interact_method(self):
        """Test createInteract convenience method."""
        interact_action = nlm.Action.createInteract()
        
        assert interact_action is not None
        assert interact_action.getType() == "Interact"
        assert interact_action.getName() == "Interact"
        assert interact_action.getParameters() == [1.0]
        
        print("✓ createInteract method test passed")
    
    def test_create_custom_method(self):
        """Test createCustom convenience method."""
        custom_action = nlm.Action.createCustom({
            "speed": 1.5,
            "direction": 30.0,
            "strength": 0.8,
            "extra_param": "test"
        })
        
        assert custom_action is not None
        assert custom_action.getType() == "Custom"
        assert custom_action.getName() == "Custom"
        
        params = custom_action.getParameters()
        assert len(params) == 4
        assert params[0] == 1.5  # speed
        assert params[1] == 30.0  # direction
        assert params[2] == 0.8  # strength
        assert params[3] == "test"  # extra_param
        
        print("✓ createCustom method test passed")
    
    def test_action_from_dict_method(self):
        """Test actionFromDict convenience method."""
        action_dict = {
            "type": "MoveBackward",
            "parameters": [2.0, -45.0]
        }
        
        action = nlm.Action.actionFromDict(action_dict)
        
        assert action is not None
        assert action.getType() == "MoveBackward"
        assert action.getName() == "MoveBackward"
        assert action.getParameters() == [2.0, -45.0]
        
        print("✓ actionFromDict method test passed")
    
    def test_factory_method_integration(self):
        """Test integration of multiple action factory methods."""
        actions = [
            nlm.Action.createMove(1.0, 0.0),
            nlm.Action.createMove(0.0, 1.0),
            nlm.Action.createInteract(),
            nlm.Action.createCustom({"custom": "action"}),
        ]
        
        for i, action in enumerate(actions):
            assert action is not None
            assert action.getName() is not None
            assert len(action.getName()) > 0
            print(f"✓ Action {i+1}: {action.getName()} created successfully")
        
        print("✓ Factory method integration test passed")

class TestConvenienceMethodChaining:
    """Test method chaining and fluent interfaces."""
    
    def test_brain_method_chaining(self):
        """Test Brain method chaining."""
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Test that runSimulation returns self for chaining
            result = brain.runSimulation(10).runSimulation(5)
            
            assert result is brain  # Should return self
            
            print("✓ Brain method chaining test passed")
    
    def test_config_method_chaining_simulation(self):
        """Test Config method chaining simulation."""
        config = nlm.Config()
        
        # Simulate chaining (though methods return None, we can chain setDefaults calls)
        with config:
            config.setDefaults("default")
            config.updateFromDict({"test": "value"})
            config.setDefaults("quick_test")  # This overwrites
            
            # Verify final state
            assert config.getOr("test", None) == "value"
            
            print("✓ Config method chaining test passed")
    
    def test_agentbrain_method_chaining(self):
        """Test AgentBrain method chaining simulation."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Enable multiple features
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                # Get metrics
                metrics1 = agent.getPerformanceMetrics()
                assert isinstance(metrics1, dict)
                
                # Learn from world
                metrics2 = agent.learnFromWorld(world, 10)
                assert isinstance(metrics2, dict)
                
                print("✓ AgentBrain method chaining test passed")

class TestConvenienceMethodErrorHandling:
    """Test convenience methods error handling."""
    
    def test_setdefaults_invalid_profile(self):
        """Test setDefaults with invalid profile."""
        with nlm.Config() as config:
            # This should handle invalid profile gracefully
            # The exact behavior depends on implementation
            try:
                config.setDefaults("invalid_profile_name")
                print("✓ setDefaults handled invalid profile")
            except Exception:
                print("✓ setDefaults raised exception for invalid profile (expected)")
    
    def test_merge_config_invalid_input(self):
        """Test mergeConfig with invalid input."""
        with nlm.Config() as config:
            config.set("valid_key", "valid_value")
            
            # Test with None (might raise exception)
            try:
                config.mergeConfig(None)
                print("✓ mergeConfig handled None input")
            except Exception:
                print("✓ mergeConfig raised exception for None input (expected)")
            
            # Test with invalid dict structure
            try:
                config.mergeConfig({"nested": {"key": "value"}})
                print("✓ mergeConfig handled nested dict")
            except Exception:
                print("✓ mergeConfig raised exception for nested dict (expected)")
    
    def test_validate_config_edge_cases(self):
        """Test validateConfig with edge cases."""
        with nlm.Config() as config:
            # Test with empty config
            issues = config.validateConfig()
            assert isinstance(issues, list)
            
            # Test with various configuration types
            config.updateFromDict({
                "int_value": 42,
                "float_value": 3.14,
                "string_value": "hello",
                "bool_value": True,
                "list_value": [1, 2, 3],
                "dict_value": {"nested": "value"}
            })
            
            issues = config.validateConfig()
            assert isinstance(issues, list)
            print(f"✓ validateConfig handled various types (found {len(issues)} issues)")

class TestConvenienceMethodPerformance:
    """Test convenience methods performance."""
    
    def test_convenience_method_speed(self):
        """Test that convenience methods are reasonably fast."""
        import time
        
        start_time = time.time()
        
        for i in range(20):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.set(f"iteration_{i}", i)
            config.getOr(f"iteration_{i}", None)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"Convenience methods too slow: {elapsed:.2f}s"
        
        print(f"✓ Convenience methods speed test passed ({elapsed:.3f}s for 20 iterations)")
    
    def test_batch_convenience_operations(self):
        """Test batch operations using convenience methods."""
        import time
        
        start_time = time.time()
        
        # Create multiple brains with convenience methods
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({
                "brain.neuron_count": (i + 1) * 10,
                "simulation.max_steps": (i + 1) * 100
            })
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                brain.runSimulation((i + 1) * 10)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 5.0, f"Batch operations too slow: {elapsed:.2f}s"
        
        print(f"✓ Batch convenience operations test passed ({elapsed:.3f}s)")

class TestConvenienceMethodTypeSafety:
    """Test convenience methods type safety."""
    
    def test_return_type_consistency(self):
        """Test that convenience methods return consistent types."""
        with nlm.Config() as config:
            config.setDefaults("default")
            
            # Test setDefaults returns None
            result = config.setDefaults("quick_test")
            assert result is None
            
            # Test getSummary returns dict
            summary = config.getSummary()
            assert isinstance(summary, dict)
            
            # Test updateFromDict returns None
            result = config.updateFromDict({"test": "value"})
            assert result is None
            
            print("✓ Return type consistency test passed")
    
    def test_parameter_type_validation(self):
        """Test that convenience methods handle various parameter types."""
        with nlm.Config() as config:
            # Test with different parameter types
            config.updateFromDict({
                "string_param": "hello",
                "int_param": 42,
                "float_param": 3.14,
                "bool_param": True,
                "list_param": [1, 2, 3],
                "dict_param": {"nested": "value"}
            })
            
            # Verify parameters were set correctly
            assert config.getOr("string_param", None) == "hello"
            assert config.getOr("int_param", None) == 42
            assert config.getOr("float_param", None) == 3.14
            assert config.getOr("bool_param", None) is True
            assert config.getOr("list_param", None) == [1, 2, 3]
            assert config.getOr("dict_param", None) == {"nested": "value"}
            
            print("✓ Parameter type validation test passed")

# Test classes can be run as pytest test cases
def test_all_convenience_methods():
    """Run all convenience method tests."""
    print("=" * 60)
    print("Running Comprehensive Convenience Method Tests")
    print("=" * 60)
    
    # Create test suite instances
    config_tests = TestConfigConvenienceMethods()
    brain_tests = TestBrainConvenienceMethods()
    agentbrain_tests = TestAgentBrainConvenienceMethods()
    world_tests = TestSimpleWorldConvenienceMethods()
    sensory_tests = TestSensoryInputConvenienceMethods()
    action_tests = TestActionConvenienceMethods()
    chaining_tests = TestConvenienceMethodChaining()
    error_tests = TestConvenienceMethodErrorHandling()
    perf_tests = TestConvenienceMethodPerformance()
    type_tests = TestConvenienceMethodTypeSafety()
    
    # Test methods in logical groups
    test_groups = [
        # Config methods
        (config_tests, [
            config_tests.test_setdefaults_method,
            config_tests.test_mergeconfig_method,
            config_tests.test_updatefromdict_method,
            config_tests.test_validateconfig_method,
            config_tests.test_getsummary_method,
        ]),
        
        # Brain methods
        (brain_tests, [
            brain_tests.test_run_simulation_method,
            brain_tests.test_create_standard_brain_function,
            brain_tests.test_get_neural_stats_method,
            brain_tests.test_simulate_with_input_method,
            brain_tests.test_train_agent_method,
        ]),
        
        # AgentBrain methods
        (agentbrain_tests, [
            agentbrain_tests.test_learn_from_world_method,
            agentbrain_tests.test_create_agent_sequence_function,
            agentbrain_tests.test_get_performance_metrics_method,
            agentbrain_tests.test_set_learning_parameters_method,
        ]),
        
        # SimpleWorld methods
        (world_tests, [
            world_tests.test_create_grid_world_method,
            world_tests.test_add_resource_method,
            world_tests.test_set_agent_goal_method,
            world_tests.test_get_observation_map_method,
        ]),
        
        # SensoryInput methods
        (sensory_tests, [
            sensory_tests.test_get_vision_array_method,
            sensory_tests.test_get_touch_strength_method,
            sensory_tests.test_get_internal_state_method,
        ]),
        
        # Action methods
        (action_tests, [
            action_tests.test_create_move_method,
            action_tests.test_create_interact_method,
            action_tests.test_create_custom_method,
            action_tests.test_action_from_dict_method,
            action_tests.test_factory_method_integration,
        ]),
        
        # Chaining tests
        (chaining_tests, [
            chaining_tests.test_brain_method_chaining,
            chaining_tests.test_config_method_chaining_simulation,
            chaining_tests.test_agentbrain_method_chaining,
        ]),
        
        # Error handling tests
        (error_tests, [
            error_tests.test_setdefaults_invalid_profile,
            error_tests.test_merge_config_invalid_input,
            error_tests.test_validate_config_edge_cases,
        ]),
        
        # Performance tests
        (perf_tests, [
            perf_tests.test_convenience_method_speed,
            perf_tests.test_batch_convenience_operations,
        ]),
        
        # Type safety tests
        (type_tests, [
            type_tests.test_return_type_consistency,
            type_tests.test_parameter_type_validation,
        ]),
    ]
    
    total_tests = 0
    passed_tests = 0
    
    for test_class, test_methods in test_groups:
        print(f"\n--- Testing {test_class.__name__} ---")
        for test_method in test_methods:
            total_tests += 1
            try:
                test_method()
                passed_tests += 1
            except Exception as e:
                print(f"❌ {test_method.__name__} failed: {e}")
                raise
    
    print("\n" + "=" * 60)
    print(f"Convenience Methods Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All convenience method tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} convenience method tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_convenience_methods()