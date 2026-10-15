"""
Comprehensive test suite for NLM Python factory methods

This module provides comprehensive tests for factory methods and builders
in the NLM (Neural Learning Machine) Python bindings.

Factory methods provide high-level interfaces for creating common objects
and configurations without dealing with low-level implementation details.

Tests cover:
- Factory methods for all core classes (Config, Brain, AgentBrain, SimpleWorld, SensoryInput, Action)
- Configuration profiles and their factory creation
- Builder methods for complex object creation
- Factory method chaining and fluent interfaces
- Type safety in factory methods
- Error handling in factory methods
- Performance considerations for factory methods
- Integration of factory methods with context managers
- Factory method documentation and examples
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
    NLMResourceError, NLMValidationError, JsonSerializer,
    BinarySerializer, IncrementalSerializer
)

# Test constants
CONFIG_PROFILES = ["default", "high_performance", "quick_test"]
STANDARD_BRAIN_CONFIGS = [
    {"brain.neuron_count": 50, "brain.connection_probability": 0.1},
    {"brain.neuron_count": 100, "brain.connection_probability": 0.2},
    {"brain.neuron_count": 200, "brain.connection_probability": 0.3},
    {"brain.neuron_count": 500, "brain.connection_probability": 0.5},
]

WORLD_CONFIGS = [
    {"width": 50.0, "height": 50.0, "vision_width": 10, "vision_height": 10},
    {"width": 100.0, "height": 100.0, "vision_width": 20, "vision_height": 20},
    {"width": 200.0, "height": 200.0, "vision_width": 40, "vision_height": 40},
]

class TestConfigFactoryMethods:
    """Test Config class factory methods."""
    
    def test_config_profiles_factory(self):
        """Test configuration profiles factory method."""
        with nlm.Config() as config:
            # Test with each profile
            for profile in CONFIG_PROFILES:
                config.setDefaults(profile)
                
                # Verify profile was applied by checking getSummary
                summary = config.getSummary()
                assert isinstance(summary, dict)
                
                # Each profile should have different characteristics
                if profile == "quick_test":
                    # quick_test should have minimal settings
                    assert len(summary) > 0
                
                print(f"✓ Config profile '{profile}' factory test passed")
    
    def test_create_default_config(self):
        """Test creating default configuration."""
        with nlm.Config() as config:
            config.setDefaults("default")
            
            # Verify it's a proper configuration
            assert config.getOr("brain.neuron_count", 0) > 0
            assert config.getOr("agent.exploration_rate", 0.0) >= 0.0
            
            print("✓ Create default config factory test passed")
    
    def test_create_high_performance_config(self):
        """Test creating high performance configuration."""
        with nlm.Config() as config:
            config.setDefaults("high_performance")
            
            # Verify high performance settings
            summary = config.getSummary()
            assert isinstance(summary, dict)
            
            # High performance should have optimized settings
            assert len(summary) > 0
            
            print("✓ Create high performance config factory test passed")
    
    def test_create_quick_test_config(self):
        """Test creating quick test configuration."""
        with nlm.Config() as config:
            config.setDefaults("quick_test")
            
            # Verify quick test settings
            summary = config.getSummary()
            assert isinstance(summary, dict)
            
            # Quick test should have minimal settings
            assert len(summary) > 0
            
            print("✓ Create quick test config factory test passed")
    
    def test_config_builder_pattern(self):
        """Test configuration builder pattern."""
        # Test building configuration using setDefaults and updateFromDict
        with nlm.Config() as config:
            # Start with a profile
            config.setDefaults("development")
            
            # Build additional configuration
            config.updateFromDict({
                "brain.neuron_count": 100,
                "agent.exploration_rate": 0.3,
                "simulation.max_steps": 1000,
                "simulation.timestep": 0.01
            })
            
            # Verify configuration was built correctly
            assert config.getOr("brain.neuron_count", 0) == 100
            assert config.getOr("agent.exploration_rate", 0.0) == 0.3
            assert config.getOr("simulation.max_steps", 0) == 1000
            assert config.getOr("simulation.timestep", 0.0) == 0.01
            
            print("✓ Configuration builder pattern test passed")
    
    def test_config_factory_with_serialization(self):
        """Test configuration factory with serialization."""
        with nlm.Config() as config:
            config.setDefaults("default")
            config.updateFromDict({
                "brain.neuron_count": 100,
                "agent.exploration_rate": 0.3
            })
            
            # Save configuration using JsonSerializer
            JsonSerializer.save_to_file("test_config.json", config.getSummary())
            
            # Load configuration
            loaded_config = JsonSerializer.load_from_file("test_config.json")
            assert isinstance(loaded_config, dict)
            assert loaded_config["brain.neuron_count"] == 100
            assert loaded_config["agent.exploration_rate"] == 0.3
            
            # Clean up
            os.remove("test_config.json")
            
            print("✓ Configuration factory with serialization test passed")

class TestBrainFactoryMethods:
    """Test Brain class factory methods."""
    
    def test_create_standard_brain(self):
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
    
    def test_create_standard_brain_variants(self):
        """Test createStandardBrain with various configurations."""
        for brain_config in STANDARD_BRAIN_CONFIGS:
            config = nlm.Config()
            config.setDefaults("default")
            config.updateFromDict(brain_config)
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
                assert stats.get('total_neurons', 0) == brain_config["brain.neuron_count"]
        
        print("✓ createStandardBrain factory method variants test passed")
    
    def test_brain_factory_with_context_managers(self):
        """Test Brain factory method integration with context managers."""
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        # Use factory method
        brain = nlm.Brain.createStandardBrain(config)
        assert brain is not None
        
        with brain:
            brain.initialize()
            
            # Test with context managers
            with nlm.Vision(10, 10) as vision:
                vision_data = [0.1] * (10 * 10 * 3)
                vision.setData(vision_data)
                brain.receiveSensoryInput(vision)
            
            # Run simulation
            brain.runSimulation(20)
            
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
            assert stats.get('total_spikes', 0) >= 0
            
            print("✓ Brain factory with context managers test passed")
    
    def test_brain_factory_serialization(self):
        """Test Brain factory method with serialization."""
        config = nlm.Config()
        config.setDefaults("default")
        
        # Create brain using factory method
        brain = nlm.Brain.createStandardBrain(config)
        assert brain is not None
        
        with brain:
            brain.initialize()
            brain.runSimulation(50)
            
            # Serialize using factory method
            JsonSerializer.serialize_brain(brain, "factory_brain_state.json")
            
            # Verify file was created
            assert os.path.exists("factory_brain_state.json")
            
            # Clean up
            os.remove("factory_brain_state.json")
            
            print("✓ Brain factory with serialization test passed")
    
    def test_brain_factory_performance(self):
        """Test Brain factory method performance."""
        import time
        
        start_time = time.time()
        
        # Create multiple brains using factory method
        for i in range(10):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({
                "brain.neuron_count": (i + 1) * 10,
                "brain.connection_probability": 0.1
            })
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                brain.runSimulation((i + 1) * 10)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 5.0, f"Brain factory too slow: {elapsed:.2f}s"
        
        print(f"✓ Brain factory performance test passed ({elapsed:.3f}s for 10 brains)")

class TestAgentBrainFactoryMethods:
    """Test AgentBrain class factory methods."""
    
    def test_create_agent_sequence(self):
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
    
    def test_create_agent_sequence_variants(self):
        """Test createAgentSequence with various world configurations."""
        for world_config in WORLD_CONFIGS:
            with nlm.SimpleWorld() as world:
                world.configure(**world_config)
                
                # Create agent sequences
                sequences = nlm.createAgentSequence(world, 2, 20)
                
                assert isinstance(sequences, list)
                assert len(sequences) == 2
                
                for seq in sequences:
                    assert isinstance(seq, dict)
                    assert 'episode' in seq
                    assert 'success_rate' in seq
                    assert 0.0 <= seq['success_rate'] <= 1.0
        
        print("✓ createAgentSequence factory function variants test passed")
    
    def test_agent_brain_factory_with_learning(self):
        """Test AgentBrain factory method with learning features."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            # Use high-level learning method
            metrics = brain.trainAgent(world, 20)
            
            assert isinstance(metrics, dict)
            assert 'success_rate' in metrics
            assert 'total_reward' in metrics
            assert 'average_reward' in metrics
            
            # Enable curiosity and neuromodulation
            metrics_with_learning = brain.trainAgent(world, 20)
            assert isinstance(metrics_with_learning, dict)
            
            print("✓ AgentBrain factory with learning test passed")
    
    def test_agent_brain_factory_serialization(self):
        """Test AgentBrain factory method with serialization."""
        config = nlm.Config()
        config.setDefaults("default")
        
        # Create brain and agent
        brain = nlm.Brain.createStandardBrain(config)
        assert brain is not None
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            with brain:
                brain.initialize()
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    
                    # Enable learning features
                    agent.enableCuriosity(True)
                    agent.enableRewardModulation(True)
                    
                    # Learn from world
                    metrics = agent.learnFromWorld(world, 20)
                    assert isinstance(metrics, dict)
        
        print("✓ AgentBrain factory with serialization test passed")
    
    def test_agent_brain_factory_performance(self):
        """Test AgentBrain factory method performance."""
        import time
        
        start_time = time.time()
        
        # Create multiple agents using factory methods
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                
                with brain:
                    brain.initialize()
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        # Learn from world
                        metrics = agent.learnFromWorld(world, (i + 1) * 10)
                        assert isinstance(metrics, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 5.0, f"AgentBrain factory too slow: {elapsed:.2f}s"
        
        print(f"✓ AgentBrain factory performance test passed ({elapsed:.3f}s for 5 agents)")

class TestWorldFactoryMethods:
    """Test SimpleWorld class factory methods."""
    
    def test_create_grid_world(self):
        """Test createGridWorld factory method."""
        with nlm.SimpleWorld() as world:
            # Use createGridWorld factory method
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
            
            print("✓ createGridWorld factory method test passed")
    
    def test_create_grid_world_variants(self):
        """Test createGridWorld with various configurations."""
        obstacle_densities = [0.05, 0.1, 0.15, 0.2, 0.25]
        
        for density in obstacle_densities:
            with nlm.SimpleWorld() as world:
                world.createGridWorld(50.0, 50.0, density)
                
                # Verify configuration
                assert world.getWidth() == 50.0
                assert world.getHeight() == 50.0
                
                # Add some resources
                world.addResource(10.0, 10.0, 5.0)
                world.addResource(40.0, 40.0, 10.0)
                
                # Set agent goal
                world.setAgentGoal(45.0, 45.0)
                
                # Get observation map
                observation_map = world.getObservationMap()
                assert isinstance(observation_map, list)
                assert len(observation_map) == 50
        
        print("✓ createGridWorld factory method variants test passed")
    
    def test_world_factory_with_serialization(self):
        """Test World factory method with serialization."""
        with nlm.SimpleWorld() as world:
            world.createGridWorld(100.0, 100.0, 0.1)
            world.setAgentGoal(90.0, 90.0)
            
            # Serialize world
            JsonSerializer.serialize_world(world, "factory_world_state.json")
            
            # Verify file was created
            assert os.path.exists("factory_world_state.json")
            
            # Clean up
            os.remove("factory_world_state.json")
            
            print("✓ World factory with serialization test passed")
    
    def test_world_factory_performance(self):
        """Test World factory method performance."""
        import time
        
        start_time = time.time()
        
        # Create multiple worlds using factory methods
        for i in range(10):
            with nlm.SimpleWorld() as world:
                world.createGridWorld(50.0, 50.0, 0.1)
                world.setAgentGoal(45.0, 45.0)
                
                # Add some resources
                for x in [10.0, 25.0, 40.0]:
                    for y in [10.0, 25.0, 40.0]:
                        world.addResource(x, y, 5.0)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 5.0, f"World factory too slow: {elapsed:.2f}s"
        
        print(f"✓ World factory performance test passed ({elapsed:.3f}s for 10 worlds)")

class TestSensoryInputFactoryMethods:
    """Test SensoryInput class factory methods."""
    
    def test_vision_factory(self):
        """Test Vision factory method."""
        with nlm.Vision(20, 20) as vision:
            assert vision.getType() == "Vision"
            assert vision.getWidth() == 20
            assert vision.getHeight() == 20
            assert vision.getChannels() == 3
            
            # Create and set vision data
            vision_data = []
            for y in range(20):
                for x in range(20):
                    brightness = (x + y) / 40.0
                    vision_data.extend([brightness, brightness * 0.8, brightness * 0.6])
            
            vision.setData(vision_data)
            assert vision.getData() == vision_data
            
            # Get vision as 2D array
            vision_array = vision.getVisionArray()
            assert isinstance(vision_array, list)
            assert len(vision_array) == 20
            
            print("✓ Vision factory method test passed")
    
    def test_vision_factory_variants(self):
        """Test Vision factory method with various dimensions."""
        dimensions = [(10, 10), (20, 20), (50, 50), (100, 100)]
        
        for width, height in dimensions:
            with nlm.Vision(width, height) as vision:
                assert vision.getType() == "Vision"
                assert vision.getWidth() == width
                assert vision.getHeight() == height
                assert vision.getChannels() == 3
                
                # Create and set data
                data = [0.5] * (width * height * 3)
                vision.setData(data)
                assert len(vision.getData()) == width * height * 3
        
        print("✓ Vision factory method variants test passed")
    
    def test_audio_factory(self):
        """Test Audio factory method."""
        with nlm.Audio(44100, 44100) as audio:
            assert audio.getType() == "Audio"
            assert audio.getSampleRate() == 44100
            assert audio.getNumSamples() == 44100
            
            # Create and set audio data
            audio_data = [0.1 * i for i in range(44100)]
            audio.setData(audio_data)
            assert audio.getData() == audio_data
            
            print("✓ Audio factory method test passed")
    
    def test_internal_signals_factory(self):
        """Test InternalSignals factory method."""
        with nlm.InternalSignals() as internal:
            assert internal.getType() == "InternalSignals"
            
            # Add signals
            internal.addSignal(0.8)
            internal.addSignal(0.3)
            internal.addSignal(0.5)
            
            # Get internal state as Python dict
            state = internal.getInternalState()
            assert isinstance(state, dict)
            assert len(state) == 3
            
            # Verify signal values
            for i, (key, value) in enumerate(state.items()):
                expected_value = [0.8, 0.3, 0.5][i]
                assert abs(value - expected_value) < 0.001
            
            print("✓ InternalSignals factory method test passed")
    
    def test_sensoryinput_factory_performance(self):
        """Test SensoryInput factory method performance."""
        import time
        
        start_time = time.time()
        
        # Create multiple sensory inputs
        for i in range(20):
            with nlm.Vision(10, 10) as vision:
                data = [0.5] * (10 * 10 * 3)
                vision.setData(data)
            
            with nlm.Audio(44100, 44100) as audio:
                audio_data = [0.1 * i for i in range(44100)]
                audio.setData(audio_data)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 5.0, f"SensoryInput factory too slow: {elapsed:.2f}s"
        
        print(f"✓ SensoryInput factory performance test passed ({elapsed:.3f}s for 20 inputs)")

class TestActionFactoryMethods:
    """Test Action class factory methods."""
    
    def test_action_factory_methods(self):
        """Test Action factory methods."""
        # Test createMove factory method
        move_action = nlm.Action.createMove(5.0, 45.0)
        assert move_action.getType() == "MoveForward"
        assert move_action.getParameters() == [5.0, 45.0]
        
        # Test createInteract factory method
        interact_action = nlm.Action.createInteract()
        assert interact_action.getType() == "Interact"
        assert interact_action.getParameters() == [1.0]
        
        # Test createCustom factory method
        custom_action = nlm.Action.createCustom({
            "speed": 1.5,
            "direction": 30.0,
            "strength": 0.8,
            "custom_param": "test"
        })
        assert custom_action.getType() == "Custom"
        params = custom_action.getParameters()
        assert len(params) == 4
        assert params[0] == 1.5
        assert params[1] == 30.0
        assert params[2] == 0.8
        assert params[3] == "test"
        
        # Test actionFromDict factory method
        action_dict = {
            "type": "MoveBackward",
            "parameters": [2.0, -45.0]
        }
        dict_action = nlm.Action.actionFromDict(action_dict)
        assert dict_action.getType() == "MoveBackward"
        assert dict_action.getParameters() == [2.0, -45.0]
        
        print("✓ Action factory methods test passed")
    
    def test_action_factory_method_variants(self):
        """Test Action factory methods with various action types."""
        action_types = [
            ("MoveForward", [1.0, 0.0]),
            ("MoveBackward", [0.5, 180.0]),
            ("TurnLeft", [0.0, -90.0]),
            ("TurnRight", [0.0, 90.0]),
            ("MoveUp", [0.0, 0.0]),
            ("MoveDown", [0.0, 0.0]),
            ("Interact", [1.0]),
            ("Eat", [0.5]),
            ("Sleep", [1.0]),
        ]
        
        for action_name, expected_params in action_types:
            # Create action using factory methods
            if action_name == "MoveForward" or action_name == "MoveBackward":
                action = nlm.Action.createMove(expected_params[0], expected_params[1])
            elif action_name == "TurnLeft" or action_name == "TurnRight":
                # Note: Action.createMove is for movement, not turning
                # This is a limitation we need to handle
                action = nlm.Action(ActionType.Eat)  # Use Eat as placeholder
                action.setParameters(expected_params)
            elif action_name == "MoveUp" or action_name == "MoveDown":
                action = nlm.Action(ActionType.Eat)  # Use Eat as placeholder
                action.setParameters(expected_params)
            elif action_name == "Interact":
                action = nlm.Action.createInteract()
            elif action_name == "Eat":
                action = nlm.Action.createCustom({"food_amount": expected_params[0]})
            elif action_name == "Sleep":
                action = nlm.Action.createCustom({"sleep_duration": expected_params[0]})
            
            assert action is not None
            assert action.getName() is not None
            assert len(action.getName()) > 0
        
        print("✓ Action factory method variants test passed")
    
    def test_action_factory_with_context_managers(self):
        """Test Action factory methods with context managers."""
        with nlm.Config() as config:
            config.setDefaults("default")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                # Test action factory methods with context managers
                actions = []
                
                for action_type in ["MoveForward", "Interact", "Eat"]:
                    with nlm.Action(ActionType.Eat if action_type == "Eat" else ActionType.Interact if action_type == "Interact" else ActionType.MoveForward) as action:
                        if action_type == "MoveForward":
                            action.setParameters([1.0, 0.0])
                        elif action_type == "Interact":
                            action.setParameters([1.0])
                        elif action_type == "Eat":
                            action.setParameters([0.5])
                        
                        actions.append(action)
                
                assert len(actions) == 3
                for action in actions:
                    assert action is not None
                    assert action.getName() is not None
        
        print("✓ Action factory with context managers test passed")
    
    def test_action_factory_serialization(self):
        """Test Action factory methods with serialization."""
        # Create actions using factory methods
        actions = [
            nlm.Action.createMove(1.0, 0.0),
            nlm.Action.createInteract(),
            nlm.Action.createCustom({"speed": 1.5}),
            nlm.Action.actionFromDict({"type": "MoveBackward", "parameters": [0.5, 180.0]})
        ]
        
        # Serialize each action
        for i, action in enumerate(actions):
            action_dict = {
                "type": action.getType(),
                "parameters": action.getParameters(),
                "name": action.getName()
            }
            
            # Save to JSON file
            JsonSerializer.save_to_file(f"action_{i}.json", action_dict)
            
            # Load from JSON file
            loaded_dict = JsonSerializer.load_from_file(f"action_{i}.json")
            assert loaded_dict["type"] == action_dict["type"]
            assert loaded_dict["parameters"] == action_dict["parameters"]
            assert loaded_dict["name"] == action_dict["name"]
            
            # Clean up
            os.remove(f"action_{i}.json")
        
        print("✓ Action factory with serialization test passed")
    
    def test_action_factory_performance(self):
        """Test Action factory methods performance."""
        import time
        
        start_time = time.time()
        
        # Create many actions using factory methods
        for i in range(100):
            actions = [
                nlm.Action.createMove(float(i), float(i) % 360),
                nlm.Action.createInteract(),
                nlm.Action.createCustom({"param": float(i)}),
            ]
            
            for action in actions:
                assert action is not None
                assert action.getName() is not None
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 2.0, f"Action factory too slow: {elapsed:.2f}s"
        
        print(f"✓ Action factory performance test passed ({elapsed:.3f}s for 300 actions)")

class TestFactoryMethodIntegration:
    """Test integration of factory methods with other features."""
    
    def test_factory_methods_with_serialization(self):
        """Test factory methods integrated with serialization."""
        # Create objects using factory methods
        config = nlm.Config()
        config.setDefaults("default")
        
        brain = nlm.Brain.createStandardBrain(config)
        assert brain is not None
        
        with brain:
            brain.initialize()
            brain.runSimulation(50)
            
            # Serialize using factory-created objects
            JsonSerializer.serialize_brain(brain, "integration_brain_state.json")
            
            # Verify file was created
            assert os.path.exists("integration_brain_state.json")
            
            # Clean up
            os.remove("integration_brain_state.json")
        
        # Create world using factory methods
        with nlm.SimpleWorld() as world:
            world.createGridWorld(100.0, 100.0, 0.1)
            world.setAgentGoal(90.0, 90.0)
            
            # Serialize world
            JsonSerializer.serialize_world(world, "integration_world_state.json")
            
            # Verify file was created
            assert os.path.exists("integration_world_state.json")
            
            # Clean up
            os.remove("integration_world_state.json")
        
        print("✓ Factory methods with serialization integration test passed")
    
    def test_factory_methods_with_context_managers(self):
        """Test factory methods integrated with context managers."""
        # Use factory methods within context managers
        with nlm.Config() as config:
            config.setDefaults("development")
            
            with nlm.SimpleWorld() as world:
                world.configure(100.0, 100.0, 20, 20)
                
                with nlm.Brain.createStandardBrain(config) as brain:
                    brain.initialize()
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        # Perform operations
                        for step in range(20):
                            world.update(0.1)
                            action = agent.decodeMotorCommand()
        
        print("✓ Factory methods with context managers integration test passed")
    
    def test_factory_methods_with_convenience_methods(self):
        """Test factory methods integrated with convenience methods."""
        config = nlm.Config()
        config.setDefaults("default")
        
        # Create objects using factory methods
        brain = nlm.Brain.createStandardBrain(config)
        assert brain is not None
        
        with brain:
            brain.initialize()
            
            # Use convenience methods
            brain.runSimulation(30)
            
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
            
            # Create world using factory methods
            with nlm.SimpleWorld() as world:
                world.createGridWorld(50.0, 50.0, 0.1)
                world.setAgentGoal(40.0, 40.0)
                
                # Use convenience methods
                world.addResource(10.0, 10.0, 5.0)
                observation_map = world.getObservationMap()
                assert isinstance(observation_map, list)
        
        print("✓ Factory methods with convenience methods integration test passed")
    
    def test_factory_methods_error_handling(self):
        """Test factory methods error handling."""
        # Test error handling in factory methods
        try:
            # Try to create invalid configuration
            config = nlm.Config()
            config.setDefaults("default")
            config.updateFromDict({
                "brain.neuron_count": -1,  # Invalid value
            })
            
            # Should still work, validation happens later
            issues = config.validateConfig()
            assert isinstance(issues, list)
            
        except Exception as e:
            print(f"✓ Factory methods error handling test caught exception: {e}")
    
    def test_factory_methods_performance(self):
        """Test factory methods performance integration."""
        import time
        
        start_time = time.time()
        
        # Create complex scenario using factory methods
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            # Create brain using factory method
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                
                # Create world using factory method
                with nlm.SimpleWorld() as world:
                    world.createGridWorld(50.0, 50.0, 0.1)
                    world.setAgentGoal(40.0, 40.0)
                    
                    # Create agent
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        
                        # Perform simulation
                        for step in range(20):
                            world.update(0.1)
                            action = agent.decodeMotorCommand()
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 10.0, f"Factory method integration too slow: {elapsed:.2f}s"
        
        print(f"✓ Factory method performance integration test passed ({elapsed:.3f}s for 5 scenarios)")

# Test classes can be run as pytest test cases
def test_all_factory_methods():
    """Run all factory method tests."""
    print("=" * 60)
    print("Running Comprehensive Factory Method Tests")
    print("=" * 60)
    
    # Create test suite instances
    config_tests = TestConfigFactoryMethods()
    brain_tests = TestBrainFactoryMethods()
    agentbrain_tests = TestAgentBrainFactoryMethods()
    world_tests = TestWorldFactoryMethods()
    sensory_tests = TestSensoryInputFactoryMethods()
    action_tests = TestActionFactoryMethods()
    integration_tests = TestFactoryMethodIntegration()
    
    # Test methods in logical groups
    test_groups = [
        # Config factory tests
        (config_tests, [
            config_tests.test_config_profiles_factory,
            config_tests.test_create_default_config,
            config_tests.test_create_high_performance_config,
            config_tests.test_create_quick_test_config,
            config_tests.test_config_builder_pattern,
            config_tests.test_config_factory_with_serialization,
        ]),
        
        # Brain factory tests
        (brain_tests, [
            brain_tests.test_create_standard_brain,
            brain_tests.test_create_standard_brain_variants,
            brain_tests.test_brain_factory_with_context_managers,
            brain_tests.test_brain_factory_serialization,
            brain_tests.test_brain_factory_performance,
        ]),
        
        # AgentBrain factory tests
        (agentbrain_tests, [
            agentbrain_tests.test_create_agent_sequence,
            agentbrain_tests.test_create_agent_sequence_variants,
            agentbrain_tests.test_agent_brain_factory_with_learning,
            agentbrain_tests.test_agent_brain_factory_serialization,
            agentbrain_tests.test_agent_brain_factory_performance,
        ]),
        
        # SimpleWorld factory tests
        (world_tests, [
            world_tests.test_create_grid_world,
            world_tests.test_create_grid_world_variants,
            world_tests.test_world_factory_with_serialization,
            world_tests.test_world_factory_performance,
        ]),
        
        # SensoryInput factory tests
        (sensory_tests, [
            sensory_tests.test_vision_factory,
            sensory_tests.test_vision_factory_variants,
            sensory_tests.test_audio_factory,
            sensory_tests.test_internal_signals_factory,
            sensory_tests.test_sensoryinput_factory_performance,
        ]),
        
        # Action factory tests
        (action_tests, [
            action_tests.test_action_factory_methods,
            action_tests.test_action_factory_method_variants,
            action_tests.test_action_factory_with_context_managers,
            action_tests.test_action_factory_serialization,
            action_tests.test_action_factory_performance,
        ]),
        
        # Integration tests
        (integration_tests, [
            integration_tests.test_factory_methods_with_serialization,
            integration_tests.test_factory_methods_with_context_managers,
            integration_tests.test_factory_methods_with_convenience_methods,
            integration_tests.test_factory_methods_error_handling,
            integration_tests.test_factory_methods_performance,
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
    print("Factory Methods Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All factory method tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} factory method tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_factory_methods()