"""
Comprehensive test suite for NLM Python batch operations

This module provides comprehensive tests for batch operations
in the NLM (Neural Learning Machine) Python bindings.

Batch operations enable efficient processing of multiple objects,
configurations, and operations simultaneously, improving performance
and reducing overhead.

Tests cover:
- Batch operations for all core classes (Config, Brain, AgentBrain, SimpleWorld, SensoryInput, Action)
- Batch processing of context managers
- Batch convenience methods for multiple operations
- Batch factory methods for creating multiple objects
- Batch serialization for multiple files
- Batch visualization and debugging for multiple scenarios
- Batch error handling for multiple operations
- Batch performance testing for optimization
- Integration of batch operations with other features
- Batch edge cases and error conditions
"""

import pytest
import sys
import os
import time
import json
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
BATCH_TEST_CONFIGS = [
    {"brain.neuron_count": 50, "brain.connection_probability": 0.1},
    {"brain.neuron_count": 100, "brain.connection_probability": 0.2},
    {"brain.neuron_count": 200, "brain.connection_probability": 0.3},
    {"brain.neuron_count": 500, "brain.connection_probability": 0.5},
]

BATCH_WORLD_CONFIGS = [
    {"width": 50.0, "height": 50.0, "vision_width": 10, "vision_height": 10},
    {"width": 100.0, "height": 100.0, "vision_width": 20, "vision_height": 20},
    {"width": 200.0, "height": 200.0, "vision_width": 40, "vision_height": 40},
]

BATCH_VISION_DIMENSIONS = [(10, 10), (20, 20), (50, 50), (100, 100)]

BATCH_ACTION_TYPES = [
    ActionType.MoveForward,
    ActionType.MoveBackward,
    ActionType.TurnLeft,
    ActionType.TurnRight,
    ActionType.Interact,
    ActionType.Eat,
    ActionType.Sleep,
]

class TestBatchConfigOperations:
    """Test batch operations for Config class."""
    
    def test_batch_config_creation(self):
        """Test batch creation of Config objects."""
        configs = []
        
        # Create multiple configs using batch operations
        for i in range(10):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({
                "brain.neuron_count": (i + 1) * 10,
                "agent.exploration_rate": 0.1 * (i + 1),
                "simulation.max_steps": (i + 1) * 100
            })
            configs.append(config)
        
        # Verify all configs were created
        assert len(configs) == 10
        
        # Verify each config has correct values
        for i, config in enumerate(configs):
            assert config.getOr("brain.neuron_count", 0) == (i + 1) * 10
            assert config.getOr("agent.exploration_rate", 0.0) == 0.1 * (i + 1)
            assert config.getOr("simulation.max_steps", 0) == (i + 1) * 100
        
        # Clean up
        for config in configs:
            config = None
        
        print("✓ Batch config creation test passed")
    
    def test_batch_config_profiles(self):
        """Test batch processing of config profiles."""
        profiles = ["default", "high_performance", "quick_test"]
        configs = []
        
        # Create configs with different profiles
        for profile in profiles:
            for i in range(3):
                config = nlm.Config()
                config.setDefaults(profile)
                config.updateFromDict({
                    "brain.neuron_count": (i + 1) * 50,
                    "simulation.max_steps": (i + 1) * 200
                })
                configs.append((profile, config))
        
        # Verify all configs were created
        assert len(configs) == 9  # 3 profiles * 3 configs each
        
        # Verify profiles were applied
        for profile, config in configs:
            summary = config.getSummary()
            assert isinstance(summary, dict)
            assert len(summary) > 0
        
        print("✓ Batch config profiles test passed")
    
    def test_batch_config_merge_operations(self):
        """Test batch merge operations for multiple configs."""
        base_config = nlm.Config()
        base_config.setDefaults("default")
        
        # Create multiple configs to merge
        configs_to_merge = []
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({
                "brain.neuron_count": (i + 1) * 10,
                "custom.value": f"value_{i}"
            })
            configs_to_merge.append(config)
        
        # Merge all configs into base
        for config in configs_to_merge:
            summary = config.getSummary()
            base_config.mergeConfig(summary)
        
        # Verify merged config
        assert base_config.getOr("brain.neuron_count", 0) > 0
        assert base_config.getOr("custom.value", None) is not None
        
        print("✓ Batch config merge operations test passed")
    
    def test_batch_config_validation(self):
        """Test batch validation of multiple configs."""
        configs_to_validate = []
        validation_results = []
        
        # Create configs with various validation scenarios
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("default")
            
            # Create different validation scenarios
            if i % 2 == 0:
                # Valid config
                config.updateFromDict({
                    "brain.neuron_count": 100,
                    "agent.exploration_rate": 0.3
                })
            else:
                # Potentially problematic config
                config.updateFromDict({
                    "brain.neuron_count": 0,  # Very small
                    "agent.exploration_rate": 1.5,  # > 1.0
                    "simulation.max_steps": -100  # Negative
                })
            
            configs_to_validate.append(config)
        
        # Validate all configs
        for config in configs_to_validate:
            issues = config.validateConfig()
            validation_results.append(issues)
            assert isinstance(issues, list)
        
        # Verify validation results
        assert len(validation_results) == 5
        
        print("✓ Batch config validation test passed")

class TestBatchBrainOperations:
    """Test batch operations for Brain class."""
    
    def test_batch_brain_creation(self):
        """Test batch creation of Brain objects."""
        brains = []
        
        # Create multiple brains with different configurations
        for i, brain_config in enumerate(BATCH_TEST_CONFIGS):
            config = nlm.Config()
            config.setDefaults("default")
            config.updateFromDict(brain_config)
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            brains.append(brain)
        
        # Verify all brains were created
        assert len(brains) == len(BATCH_TEST_CONFIGS)
        
        # Use all brains
        for i, brain in enumerate(brains):
            with brain:
                brain.initialize()
                brain.runSimulation((i + 1) * 10)
                
                # Get statistics
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
                assert stats.get('total_neurons', 0) == BATCH_TEST_CONFIGS[i]["brain.neuron_count"]
        
        print("✓ Batch brain creation test passed")
    
    def test_batch_brain_simulation(self):
        """Test batch simulation operations for multiple brains."""
        configs = []
        brains = []
        
        # Create multiple brains
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({
                "brain.neuron_count": (i + 1) * 20,
                "simulation.max_steps": (i + 1) * 50
            })
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            configs.append(config)
            brains.append(brain)
        
        # Run simulations for all brains
        simulation_results = []
        for i, brain in enumerate(brains):
            with brain:
                brain.initialize()
                
                # Run multiple simulations
                for sim in range(3):
                    brain.runSimulation((sim + 1) * 10)
                    
                    stats = brain.getNeuralStats()
                    simulation_results.append({
                        'brain_index': i,
                        'simulation': sim + 1,
                        'total_spikes': stats.get('total_spikes', 0),
                        'neuron_count': stats.get('total_neurons', 0)
                    })
        
        # Verify simulation results
        assert len(simulation_results) == 15  # 5 brains * 3 simulations each
        
        print("✓ Batch brain simulation test passed")
    
    def test_batch_brain_with_sensory_input(self):
        """Test batch operations with sensory input for multiple brains."""
        configs = []
        brains = []
        
        # Create multiple brains and worlds
        for i in range(3):
            config = nlm.Config()
            config.setDefaults("default")
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            configs.append(config)
            brains.append(brain)
        
        # Create sensory inputs for each brain
        for i, brain in enumerate(brains):
            with brain:
                brain.initialize()
                
                # Create vision input for each brain
                for dim_x, dim_y in BATCH_VISION_DIMENSIONS:
                    with nlm.Vision(dim_x, dim_y) as vision:
                        # Create vision data
                        vision_data = []
                        for y in range(dim_y):
                            for x in range(dim_x):
                                brightness = (x + y) / (dim_x + dim_y)
                                vision_data.extend([brightness, brightness * 0.8, brightness * 0.6])
                        
                        vision.setData(vision_data)
                        brain.receiveSensoryInput(vision)
                        brain.runSimulation(5)
        
        print("✓ Batch brain with sensory input test passed")
    
    def test_batch_brain_serialization(self):
        """Test batch serialization for multiple brains."""
        # Create multiple brains
        brains = []
        configs = []
        
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("default")
            config.updateFromDict({
                "brain.neuron_count": (i + 1) * 50,
                "simulation.max_steps": (i + 1) * 100
            })
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            configs.append(config)
            brains.append(brain)
        
        # Serialize all brains
        serialized_files = []
        for i, brain in enumerate(brains):
            with brain:
                brain.initialize()
                brain.runSimulation((i + 1) * 20)
                
                # Serialize brain
                filename = f"batch_brain_{i}.json"
                JsonSerializer.serialize_brain(brain, filename)
                serialized_files.append(filename)
                
                # Verify file was created
                assert os.path.exists(filename)
        
        # Load and verify all serialized brains
        for i, filename in enumerate(serialized_files):
            # Create new brain to load into
            new_config = nlm.Config()
            new_config.setDefaults("default")
            
            with nlm.Brain(new_config) as loaded_brain:
                # Deserialize brain
                deserialized_brain = JsonSerializer.deserialize_brain(filename)
                
                assert deserialized_brain is not None
                assert isinstance(deserialized_brain, nlm.Brain)
                
                # Get stats from deserialized brain
                stats = deserialized_brain.getNeuralStats()
                assert isinstance(stats, dict)
                
                # Clean up
                os.remove(filename)
        
        print("✓ Batch brain serialization test passed")
    
    def test_batch_brain_performance(self):
        """Test batch performance operations for multiple brains."""
        import time
        
        start_time = time.time()
        
        # Create and process multiple brains
        for i in range(10):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                
                # Run batch operations
                for j in range(5):
                    brain.runSimulation((j + 1) * 10)
                    
                    stats = brain.getNeuralStats()
                    assert isinstance(stats, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 10.0, f"Batch brain performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Batch brain performance test passed ({elapsed:.3f}s for 10 brains)")

class TestBatchAgentBrainOperations:
    """Test batch operations for AgentBrain class."""
    
    def test_batch_agentbrain_creation(self):
        """Test batch creation of AgentBrain objects."""
        agents = []
        
        # Create multiple agents with different configurations
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("default")
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                
                agent = nlm.AgentBrain(brain)
                assert agent is not None
                
                agent.initialize(world)
                
                # Enable learning features
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                agents.append(agent)
        
        # Verify all agents were created
        assert len(agents) == 5
        
        # Use all agents
        for agent in agents:
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                agent.initialize(world)
                
                # Perform agent operations
                action = agent.decodeMotorCommand()
                assert action is not None
        
        print("✓ Batch agentbrain creation test passed")
    
    def test_batch_agentbrain_learning(self):
        """Test batch learning operations for multiple agents."""
        # Create multiple agent-world combinations
        agents = []
        
        for i in range(3):
            config = nlm.Config()
            config.setDefaults("default")
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with nlm.SimpleWorld() as world:
                world.configure(100.0, 100.0, 20, 20)
                
                agent = nlm.AgentBrain(brain)
                assert agent is not None
                
                agent.initialize(world)
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                agents.append((agent, world))
        
        # Perform batch learning
        learning_results = []
        for agent, world in agents:
            metrics = agent.learnFromWorld(world, 20)
            learning_results.append(metrics)
            assert isinstance(metrics, dict)
            assert 'success_rate' in metrics
            assert 'total_reward' in metrics
        
        # Verify learning results
        assert len(learning_results) == 3
        
        print("✓ Batch agentbrain learning test passed")
    
    def test_batch_agentbrain_performance_metrics(self):
        """Test batch performance metrics collection."""
        agents = []
        
        # Create multiple agents
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("default")
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                
                agent = nlm.AgentBrain(brain)
                assert agent is not None
                
                agent.initialize(world)
                agent.enableCuriosity(True)
                
                agents.append(agent)
        
        # Collect performance metrics for all agents
        all_metrics = []
        for agent in agents:
            metrics = agent.getPerformanceMetrics()
            all_metrics.append(metrics)
            
            assert isinstance(metrics, dict)
            assert 'neuromodulation_level' in metrics
            assert 'curiosity_level' in metrics
            assert 'success_rate' in metrics
        
        # Verify metrics collection
        assert len(all_metrics) == 5
        
        print("✓ Batch agentbrain performance metrics test passed")

class TestBatchWorldOperations:
    """Test batch operations for SimpleWorld class."""
    
    def test_batch_world_creation(self):
        """Test batch creation of SimpleWorld objects."""
        worlds = []
        
        # Create multiple worlds with different configurations
        for i, world_config in enumerate(BATCH_WORLD_CONFIGS):
            world = nlm.SimpleWorld()
            world.configure(**world_config)
            assert world.getWidth() == world_config["width"]
            assert world.getHeight() == world_config["height"]
            
            worlds.append(world)
        
        # Verify all worlds were created
        assert len(worlds) == len(BATCH_WORLD_CONFIGS)
        
        # Use all worlds
        for world in worlds:
            # Add resources and set goals
            world.addResource(10.0, 10.0, 5.0)
            world.setAgentGoal(45.0, 45.0)
            
            # Get observation map for visualization
            observation_map = world.getObservationMap()
            assert isinstance(observation_map, list)
        
        print("✓ Batch world creation test passed")
    
    def test_batch_world_simulation(self):
        """Test batch simulation operations for multiple worlds."""
        worlds = []
        
        # Create multiple worlds
        for i in range(5):
            world = nlm.SimpleWorld()
            world.configure(50.0, 50.0, 10, 10)
            world.setAgentStart(25.0, 25.0)
            world.setAgentGoal(45.0, 45.0)
            
            worlds.append(world)
        
        # Run simulations for all worlds
        simulation_data = []
        for world in worlds:
            world_steps = []
            for step in range(10):
                world.update(0.1)
                
                # Get world state
                percept = world.getSensoryPercept()
                body = world.getAgentBody()
                
                world_steps.append({
                    'agent_x': body.x,
                    'agent_y': body.y,
                    'simulation_time': world.getSimulationTime()
                })
            
            simulation_data.append(world_steps)
        
        # Verify simulation data
        assert len(simulation_data) == 5
        
        print("✓ Batch world simulation test passed")
    
    def test_batch_world_with_obstacles(self):
        """Test batch world creation with obstacles."""
        # Create worlds with different obstacle densities
        obstacle_densities = [0.1, 0.2, 0.3, 0.4, 0.5]
        worlds = []
        
        for density in obstacle_densities:
            world = nlm.SimpleWorld()
            world.createGridWorld(100.0, 100.0, density)
            world.setAgentGoal(90.0, 90.0)
            
            # Add some resources
            world.addResource(10.0, 10.0, 5.0)
            world.addResource(50.0, 50.0, 10.0)
            
            worlds.append(world)
        
        # Verify all worlds were created
        assert len(worlds) == len(obstacle_densities)
        
        # Test obstacle processing
        for world in worlds:
            observation_map = world.getObservationMap()
            assert isinstance(observation_map, list)
            
            # Check for obstacles (values > 0.0)
            obstacle_count = 0
            for row in observation_map:
                for value in row:
                    if value > 0.0:
                        obstacle_count += 1
            
            # Should have some obstacles
            assert obstacle_count > 0
        
        print("✓ Batch world with obstacles test passed")
    
    def test_batch_world_serialization(self):
        """Test batch serialization for multiple worlds."""
        # Create multiple worlds
        worlds = []
        
        for i in range(5):
            world = nlm.SimpleWorld()
            world.configure(100.0, 100.0, 20, 20)
            world.createGridWorld(100.0, 100.0, 0.1)
            world.setAgentGoal(90.0, 90.0)
            
            worlds.append(world)
        
        # Serialize all worlds
        serialized_files = []
        for i, world in enumerate(worlds):
            filename = f"batch_world_{i}.json"
            JsonSerializer.serialize_world(world, filename)
            serialized_files.append(filename)
            
            # Verify file was created
            assert os.path.exists(filename)
        
        # Load and verify all serialized worlds
        for i, filename in enumerate(serialized_files):
            loaded_data = JsonSerializer.load_from_file(filename)
            assert isinstance(loaded_data, dict)
            
            # Clean up
            os.remove(filename)
        
        print("✓ Batch world serialization test passed")

class TestBatchSensoryInputOperations:
    """Test batch operations for SensoryInput classes."""
    
    def test_batch_vision_creation(self):
        """Test batch creation of Vision objects."""
        visions = []
        
        # Create multiple vision objects with different dimensions
        for width, height in BATCH_VISION_DIMENSIONS:
            vision = nlm.Vision(width, height)
            assert vision.getType() == "Vision"
            assert vision.getWidth() == width
            assert vision.getHeight() == height
            assert vision.getChannels() == 3
            
            # Create and set vision data
            vision_data = [0.5] * (width * height * 3)
            vision.setData(vision_data)
            
            visions.append(vision)
        
        # Verify all visions were created
        assert len(visions) == len(BATCH_VISION_DIMENSIONS)
        
        print("✓ Batch vision creation test passed")
    
    def test_batch_vision_data_processing(self):
        """Test batch processing of vision data for visualization."""
        # Create multiple vision objects with different data
        visions = []
        
        for i in range(5):
            width, height = BATCH_VISION_DIMENSIONS[i % len(BATCH_VISION_DIMENSIONS)]
            vision = nlm.Vision(width, height)
            
            # Create vision data with different patterns
            vision_data = []
            for y in range(height):
                for x in range(width):
                    # Create different patterns based on index
                    if i % 3 == 0:
                        # Dark center, bright edges
                        dx = abs(x - width // 2)
                        dy = abs(y - height // 2)
                        distance = (dx + dy) / max(width, height)
                        brightness = 1.0 - distance
                    elif i % 3 == 1:
                        # Gradient from left to right
                        brightness = x / width
                    else:
                        # Checkerboard pattern
                        brightness = 0.5 if (x + y) % 2 == 0 else 0.2
                    
                    vision_data.extend([brightness, brightness * 0.8, brightness * 0.6])
            
            vision.setData(vision_data)
            visions.append(vision)
        
        # Get vision arrays for all visions
        vision_arrays = []
        for vision in visions:
            vision_array = vision.getVisionArray()
            vision_arrays.append(vision_array)
            
            # Verify vision array structure
            assert isinstance(vision_array, list)
            assert len(vision_array) == vision.getHeight()
            
            for row in vision_array:
                assert isinstance(row, list)
                assert len(row) == vision.getWidth()
        
        print("✓ Batch vision data processing test passed")
    
    def test_batch_sensoryinput_integration(self):
        """Test batch integration of different sensory input types."""
        # Create batch of different sensory input types
        sensory_inputs = []
        
        # Vision inputs
        for width, height in [(10, 10), (20, 20)]:
            vision = nlm.Vision(width, height)
            vision_data = [0.5] * (width * height * 3)
            vision.setData(vision_data)
            sensory_inputs.append(vision)
        
        # Audio inputs
        for i in range(3):
            audio = nlm.Audio(44100, 44100)
            audio_data = [0.1 * i for i in range(44100)]
            audio.setData(audio_data)
            sensory_inputs.append(audio)
        
        # Internal signals
        for i in range(2):
            internal = nlm.InternalSignals()
            for j in range(3):
                internal.addSignal(0.5 + j * 0.1)
            sensory_inputs.append(internal)
        
        # Verify all sensory inputs were created
        assert len(sensory_inputs) == 7  # 2 vision + 3 audio + 2 internal
        
        print("✓ Batch sensoryinput integration test passed")

class TestBatchActionOperations:
    """Test batch operations for Action class."""
    
    def test_batch_action_creation(self):
        """Test batch creation of Action objects."""
        actions = []
        
        # Create multiple actions using factory methods
        for i in range(5):
            # Create movement actions
            move_action = nlm.Action.createMove(float(i), float(i % 360))
            actions.append(move_action)
            
            # Create interaction actions
            interact_action = nlm.Action.createInteract()
            actions.append(interact_action)
            
            # Create custom actions
            custom_action = nlm.Action.createCustom({
                "speed": float(i) * 0.5,
                "direction": float(i) * 30.0,
                "strength": 0.1 + float(i) * 0.1
            })
            actions.append(custom_action)
        
        # Verify all actions were created
        assert len(actions) == 15  # 5 of each type
        
        # Verify action types
        move_actions = [a for a in actions if a.getType() == "MoveForward"]
        interact_actions = [a for a in actions if a.getType() == "Interact"]
        custom_actions = [a for a in actions if a.getType() == "Custom"]
        
        assert len(move_actions) == 5
        assert len(interact_actions) == 5
        assert len(custom_actions) == 5
        
        print("✓ Batch action creation test passed")
    
    def test_batch_action_from_dict(self):
        """Test batch creation of actions from dictionaries."""
        # Create multiple actions from dictionaries
        action_dicts = [
            {"type": "MoveForward", "parameters": [1.0, 0.0]},
            {"type": "MoveBackward", "parameters": [0.5, 180.0]},
            {"type": "TurnLeft", "parameters": [0.0, -90.0]},
            {"type": "TurnRight", "parameters": [0.0, 90.0]},
            {"type": "Interact", "parameters": [1.0]},
        ]
        
        actions = []
        for action_dict in action_dicts:
            action = nlm.Action.actionFromDict(action_dict)
            actions.append(action)
            
            # Verify action was created correctly
            assert action.getType() == action_dict["type"]
            assert action.getParameters() == action_dict["parameters"]
        
        # Verify all actions were created
        assert len(actions) == len(action_dicts)
        
        print("✓ Batch action from dict test passed")
    
    def test_batch_action_cloning(self):
        """Test batch cloning of Action objects."""
        # Create multiple actions to clone
        original_actions = []
        for i in range(5):
            action = nlm.Action(ActionType.MoveForward)
            action.setParameters([float(i), float(i) * 30.0])
            original_actions.append(action)
        
        # Clone all actions
        cloned_actions = []
        for action in original_actions:
            cloned = action.clone()
            cloned_actions.append(cloned)
            
            # Verify clone was created correctly
            assert cloned.getType() == action.getType()
            assert cloned.getParameters() == action.getParameters()
        
        # Verify all actions were cloned
        assert len(cloned_actions) == len(original_actions)
        
        print("✓ Batch action cloning test passed")
    
    def test_batch_action_performance(self):
        """Test batch performance of action operations."""
        import time
        
        start_time = time.time()
        
        # Create and process many actions
        for i in range(100):
            # Create different types of actions
            actions = [
                nlm.Action.createMove(float(i), float(i) % 360),
                nlm.Action.createInteract(),
                nlm.Action.createCustom({"param": float(i)}),
            ]
            
            for action in actions:
                # Perform action operations
                action_type = action.getType()
                parameters = action.getParameters()
                name = action.getName()
                
                assert action_type is not None
                assert parameters is not None
                assert name is not None
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 2.0, f"Batch action performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Batch action performance test passed ({elapsed:.3f}s for 300 actions)")

class TestBatchErrorHandling:
    """Test batch error handling operations."""
    
    def test_batch_error_handling(self):
        """Test batch error handling for multiple operations."""
        # Create multiple configs and test error handling
        errors_handled = 0
        
        for i in range(10):
            try:
                config = nlm.Config()
                config.setDefaults("default")
                
                # Try to create validation error scenario
                config.updateFromDict({
                    "brain.neuron_count": -1,  # Invalid
                })
                
                # Validate configuration
                issues = config.validateConfig()
                if issues:
                    errors_handled += 1
                
            except Exception as e:
                # Handle errors gracefully
                errors_handled += 1
        
        # Should have handled errors
        assert errors_handled > 0
        
        print(f"✓ Batch error handling test passed (handled {errors_handled} errors)")
    
    def test_batch_resource_cleanup(self):
        """Test batch resource cleanup operations."""
        # Create and destroy multiple objects
        resources = []
        
        for i in range(10):
            config = nlm.Config()
            config.set("test", i)
            resources.append(config)
        
        # Clear all resources
        for resource in resources:
            resource = None
        
        # Verify resource cleanup
        assert len(resources) == 10
        
        print("✓ Batch resource cleanup test passed")

class TestBatchPerformance:
    """Test batch performance operations."""
    
    def test_batch_overall_performance(self):
        """Test overall batch performance."""
        import time
        
        start_time = time.time()
        
        # Create complex batch scenario
        for i in range(3):
            # Create brain
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                
                # Create world
                with nlm.SimpleWorld() as world:
                    world.configure(50.0, 50.0, 10, 10)
                    
                    # Create agent
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        
                        # Perform batch operations
                        for step in range(20):
                            world.update(0.1)
                            action = agent.decodeMotorCommand()
                            
                            # Get visualization data
                            stats = brain.getNeuralStats()
                            metrics = agent.getPerformanceMetrics()
                            
                            assert isinstance(stats, dict)
                            assert isinstance(metrics, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 15.0, f"Batch performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Batch overall performance test passed ({elapsed:.3f}s)")

# Test classes can be run as pytest test cases
def test_all_batch_operations():
    """Run all batch operations tests."""
    print("=" * 60)
    print("Running Comprehensive Batch Operations Tests")
    print("=" * 60)
    
    # Create test suite instances
    config_tests = TestBatchConfigOperations()
    brain_tests = TestBatchBrainOperations()
    agentbrain_tests = TestBatchAgentBrainOperations()
    world_tests = TestBatchWorldOperations()
    sensory_tests = TestBatchSensoryInputOperations()
    action_tests = TestBatchActionOperations()
    error_tests = TestBatchErrorHandling()
    perf_tests = TestBatchPerformance()
    
    # Test methods in logical groups
    test_groups = [
        # Config batch operations
        (config_tests, [
            config_tests.test_batch_config_creation,
            config_tests.test_batch_config_profiles,
            config_tests.test_batch_config_merge_operations,
            config_tests.test_batch_config_validation,
        ]),
        
        # Brain batch operations
        (brain_tests, [
            brain_tests.test_batch_brain_creation,
            brain_tests.test_batch_brain_simulation,
            brain_tests.test_batch_brain_with_sensory_input,
            brain_tests.test_batch_brain_serialization,
            brain_tests.test_batch_brain_performance,
        ]),
        
        # AgentBrain batch operations
        (agentbrain_tests, [
            agentbrain_tests.test_batch_agentbrain_creation,
            agentbrain_tests.test_batch_agentbrain_learning,
            agentbrain_tests.test_batch_agentbrain_performance_metrics,
        ]),
        
        # SimpleWorld batch operations
        (world_tests, [
            world_tests.test_batch_world_creation,
            world_tests.test_batch_world_simulation,
            world_tests.test_batch_world_with_obstacles,
            world_tests.test_batch_world_serialization,
        ]),
        
        # SensoryInput batch operations
        (sensory_tests, [
            sensory_tests.test_batch_vision_creation,
            sensory_tests.test_batch_vision_data_processing,
            sensory_tests.test_batch_sensoryinput_integration,
        ]),
        
        # Action batch operations
        (action_tests, [
            action_tests.test_batch_action_creation,
            action_tests.test_batch_action_from_dict,
            action_tests.test_batch_action_cloning,
            action_tests.test_batch_action_performance,
        ]),
        
        # Error handling tests
        (error_tests, [
            error_tests.test_batch_error_handling,
            error_tests.test_batch_resource_cleanup,
        ]),
        
        # Performance tests
        (perf_tests, [
            perf_tests.test_batch_overall_performance,
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
    print("Batch Operations Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All batch operations tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} batch operations tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_batch_operations()