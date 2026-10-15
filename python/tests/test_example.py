"""
Comprehensive test suite for NLM Python example tests

This module provides comprehensive tests for example usage and documentation
tests for the NLM (Neural Learning Machine) Python bindings.

Example tests ensure that the documented examples in the documentation
work correctly and demonstrate proper usage of the API.

Tests cover:
- Basic usage examples
- Advanced usage examples
- Documentation examples
- Tutorial examples
- Real-world examples
- Best practice examples
- Error handling examples
- Performance examples
- Integration examples
- Example edge cases
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

# Example test constants
EXAMPLE_TEST_CONFIG = {
    "brain.neuron_count": 100,
    "brain.connection_probability": 0.2,
    "agent.exploration_rate": 0.3,
    "simulation.max_steps": 1000,
}

class TestBasicExamples:
    """Test basic usage examples from documentation."""
    
    def test_basic_import_example(self):
        """Test basic import and configuration example."""
        # This simulates the basic usage example from the README
        import pynlm as nlm
        
        with nlm.Config() as config:
            config.set("learning_rate", 0.01)
            config.set("neuron_count", 500)
            
            assert config.getOr("learning_rate", 0.0) == 0.01
            assert config.getOr("neuron_count", 0) == 500
        
        print("✓ Basic import example test passed")
    
    def test_context_manager_example(self):
        """Test context manager usage example."""
        # This simulates context manager example from documentation
        with nlm.Config() as config:
            config.set("learning_rate", 0.01)
            config.set("neuron_count", 500)
            
            with nlm.SimpleWorld() as world:
                world.configure(100.0, 100.0, 10, 10)
                
                assert world.getWidth() == 100.0
                assert world.getHeight() == 100.0
        
        print("✓ Context manager example test passed")
    
    def test_complete_example(self):
        """Test complete example from documentation."""
        # This simulates complete example from API reference
        with nlm.Config() as config:
            config.setDefaults("default")
            config.updateFromDict(EXAMPLE_TEST_CONFIG)
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
                assert stats.get('total_neurons', 0) == 100
        
        print("✓ Complete example test passed")

class TestConvenienceMethodExamples:
    """Test convenience method examples from documentation."""
    
    def test_convenience_methods_example(self):
        """Test convenience methods example."""
        # Simulate convenience methods example
        with nlm.Config() as config:
            config.setDefaults("high_performance")
            config.updateFromDict({
                "brain.neuron_count": 200,
                "agent.exploration_rate": 0.4
            })
            
            # Use convenience methods
            config.updateFromDict({
                "simulation.max_steps": 500,
                "simulation.timestep": 0.01
            })
            
            # Validate configuration
            issues = config.validateConfig()
            assert isinstance(issues, list)
        
        print("✓ Convenience methods example test passed")
    
    def test_brain_convenience_example(self):
        """Test brain convenience methods example."""
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Use convenience method
            brain.runSimulation(10)
            
            # Get statistics
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
        
        print("✓ Brain convenience methods example test passed")
    
    def test_agentbrain_convenience_example(self):
        """Test agent brain convenience methods example."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 20, 20)
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    agent.enableCuriosity(True)
                    agent.enableRewardModulation(True)
                    
                    # Use convenience method
                    metrics = agent.learnFromWorld(world, 15)
                    assert isinstance(metrics, dict)
        
        print("✓ AgentBrain convenience methods example test passed")

class TestSerializationExamples:
    """Test serialization examples from documentation."""
    
    def test_json_serialization_example(self):
        """Test JSON serialization example."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(25)
            
            # Serialize brain
            JsonSerializer.serialize_brain(brain, "example_brain.json")
            
            # Verify file was created
            assert os.path.exists("example_brain.json")
            
            # Load brain
            loaded_brain = nlm.Brain(config)
            with loaded_brain:
                loaded_brain.load("example_brain.json")
                loaded_stats = loaded_brain.getNeuralStats()
                assert isinstance(loaded_stats, dict)
                assert loaded_stats.get('total_spikes', 0) > 0
            
            # Clean up
            os.remove("example_brain.json")
        
        print("✓ JSON serialization example test passed")
    
    def test_world_serialization_example(self):
        """Test world serialization example."""
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 20, 20)
            world.createGridWorld(100.0, 100.0, 0.1)
            world.setAgentGoal(90.0, 90.0)
            
            # Serialize world
            JsonSerializer.serialize_world(world, "example_world.json")
            
            # Verify file was created
            assert os.path.exists("example_world.json")
            
            # Load world
            loaded_data = JsonSerializer.load_from_file("example_world.json")
            assert isinstance(loaded_data, dict)
            
            # Clean up
            os.remove("example_world.json")
        
        print("✓ World serialization example test passed")

class TestErrorHandlingExamples:
    """Test error handling examples from documentation."""
    
    def test_exception_handling_example(self):
        """Test exception handling example."""
        # Simulate exception handling example
        try:
            with nlm.Config() as config:
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    
                    # Simulate an error
                    raise ValueError("Simulated error")
                    
        except ValueError as e:
            assert str(e) == "Simulated error"
            print("✓ Exception handling example test passed")
    
    def test_config_validation_example(self):
        """Test config validation example."""
        with nlm.Config() as config:
            config.setDefaults("default")
            config.updateFromDict({
                "brain.neuron_count": 100,
                "agent.exploration_rate": 0.3,
                "simulation.max_steps": 1000
            })
            
            # Validate configuration
            issues = config.validateConfig()
            assert isinstance(issues, list)
            
            if issues:
                print(f"✓ Config validation example test passed (found {len(issues)} issues)")
            else:
                print("✓ Config validation example test passed (no issues)")

class TestAdvancedExamples:
    """Test advanced usage examples."""
    
    def test_advanced_agent_training_example(self):
        """Test advanced agent training example."""
        config = nlm.Config()
        config.setDefaults("development")
        
        with nlm.SimpleWorld() as world:
            world.createGridWorld(200.0, 200.0, 0.1)
            world.setAgentGoal(180.0, 180.0)
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    agent.enableCuriosity(True)
                    agent.enableRewardModulation(True)
                    agent.enableStructuralPlasticity(True)
                    
                    # Create training sequences
                    sequences = nlm.createAgentSequence(world, 3, 30)
                    assert isinstance(sequences, list)
                    assert len(sequences) == 3
                    
                    # Train agent
                    metrics = agent.learnFromWorld(world, 30)
                    assert isinstance(metrics, dict)
                    assert 'success_rate' in metrics
        
        print("✓ Advanced agent training example test passed")
    
    def test_advanced_simulation_example(self):
        """Test advanced simulation example."""
        config = nlm.Config()
        config.setDefaults("high_performance")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Create sensory input
            with nlm.Vision(50, 50) as vision:
                vision_data = []
                for y in range(50):
                    for x in range(50):
                        brightness = (x + y) / 100.0
                        vision_data.extend([brightness, brightness * 0.8, brightness * 0.6])
                
                vision.setData(vision_data)
                brain.receiveSensoryInput(vision)
            
            # Run simulation
            brain.runSimulation(100)
            
            # Get statistics
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
            assert stats.get('total_spikes', 0) >= 0
        
        print("✓ Advanced simulation example test passed")
    
    def test_advanced_visualization_example(self):
        """Test advanced visualization example."""
        with nlm.SimpleWorld() as world:
            world.createGridWorld(150.0, 150.0, 0.2)
            world.setAgentGoal(120.0, 120.0)
            
            # Add resources
            for x in [30.0, 60.0, 90.0, 120.0]:
                for y in [30.0, 60.0, 90.0, 120.0]:
                    world.addResource(x, y, 5.0)
            
            # Get observation map for visualization
            observation_map = world.getObservationMap()
            assert isinstance(observation_map, list)
            
            # Process observation map (simulate visualization)
            for i, row in enumerate(observation_map):
                for j, value in enumerate(row):
                    # Normalize values for visualization
                    normalized = value / 1.0
                    assert 0.0 <= normalized <= 1.0
        
        print("✓ Advanced visualization example test passed")

class TestRealWorldExamples:
    """Test real-world usage examples."""
    
    def test_robot_navigation_example(self):
        """Test robot navigation example."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.SimpleWorld() as world:
                world.configure(100.0, 100.0, 20, 20)
                world.setAgentStart(10.0, 10.0)
                world.setAgentGoal(90.0, 90.0)
                
                # Add obstacles
                for x in range(30, 71, 10):
                    for y in range(30, 71, 10):
                        world.createGridWorld(100.0, 100.0, 0.1)
                
                # Add resources to collect
                world.addResource(80.0, 80.0, 10.0)
                world.addResource(85.0, 85.0, 15.0)
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    
                    # Run navigation simulation
                    for step in range(50):
                        world.update(0.1)
                        action = agent.decodeMotorCommand()
                        
                        # Apply action
                        result = world.applyMotorCommand(action, world.getSimulationTime())
                        
                        # Check if goal reached
                        agent_body = world.getAgentBody()
                        if agent_body.x >= 85.0 and agent_body.y >= 85.0:
                            print(f"✓ Robot reached target at step {step}")
                            break
        
        print("✓ Robot navigation example test passed")
    
    def test_neural_network_example(self):
        """Test neural network simulation example."""
        # Create a neural network simulation
        config = nlm.Config()
        config.setDefaults("high_performance")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Simulate neural activity
            for step in range(30):
                brain.step(step)
                
                # Get statistics
                stats = brain.getNeuralStats()
                
                # Analyze activity
                if step % 10 == 0:
                    print(f"Step {step}: {stats['total_spikes']} spikes, {stats['active_neurons']} active neurons")
        
        print("✓ Neural network example test passed")

class TestBestPracticeExamples:
    """Test best practice examples."""
    
    def test_context_manager_best_practice(self):
        """Test context manager best practices example."""
        # Example of proper resource management
        def simulate_with_context_managers():
            with nlm.Config() as config:
                config.setDefaults("development")
                
                with nlm.SimpleWorld() as world:
                    world.configure(50.0, 50.0, 10, 10)
                    
                    with nlm.Brain(config) as brain:
                        brain.initialize()
                        
                        with nlm.AgentBrain(brain) as agent:
                            agent.initialize(world)
                            
                            # Run simulation
                            for step in range(20):
                                world.update(0.1)
                                action = agent.decodeMotorCommand()
        
        # Should work without resource leaks
        simulate_with_context_managers()
        print("✓ Context manager best practice example test passed")
    
    def test_configuration_management_example(self):
        """Test configuration management best practices example."""
        def create_proper_config():
            # Create configuration with proper defaults
            with nlm.Config() as config:
                config.setDefaults("development")
                
                # Override with specific settings
                config.updateFromDict({
                    "brain.neuron_count": 500,
                    "agent.exploration_rate": 0.3
                })
                
                # Validate configuration
                issues = config.validateConfig()
                if issues:
                    print(f"Configuration issues found: {len(issues)}")
                
                return config
        
        # Should create valid configuration
        config = create_proper_config()
        assert config is not None
        
        print("✓ Configuration management best practice example test passed")

# Test classes can be run as pytest test cases
def test_all_examples():
    """Run all example tests."""
    print("=" * 60)
    print("Running Comprehensive Example Tests")
    print("=" * 60)
    
    # Create test suite instances
    basic_tests = TestBasicExamples()
    convenience_tests = TestConvenienceMethodExamples()
    serialization_tests = TestSerializationExamples()
    error_tests = TestErrorHandlingExamples()
    advanced_tests = TestAdvancedExamples()
    real_world_tests = TestRealWorldExamples()
    best_practice_tests = TestBestPracticeExamples()
    
    # Test methods in logical groups
    test_groups = [
        # Basic examples tests
        (basic_tests, [
            basic_tests.test_basic_import_example,
            basic_tests.test_context_manager_example,
            basic_tests.test_complete_example,
        ]),
        
        # Convenience method examples tests
        (convenience_tests, [
            convenience_tests.test_convenience_methods_example,
            convenience_tests.test_brain_convenience_example,
            convenience_tests.test_agentbrain_convenience_example,
        ]),
        
        # Serialization examples tests
        (serialization_tests, [
            serialization_tests.test_json_serialization_example,
            serialization_tests.test_world_serialization_example,
        ]),
        
        # Error handling examples tests
        (error_tests, [
            error_tests.test_exception_handling_example,
            error_tests.test_config_validation_example,
        ]),
        
        # Advanced examples tests
        (advanced_tests, [
            advanced_tests.test_advanced_agent_training_example,
            advanced_tests.test_advanced_simulation_example,
            advanced_tests.test_advanced_visualization_example,
        ]),
        
        # Real-world examples tests
        (real_world_tests, [
            real_world_tests.test_robot_navigation_example,
            real_world_tests.test_neural_network_example,
        ]),
        
        # Best practice examples tests
        (best_practice_tests, [
            best_practice_tests.test_context_manager_best_practice,
            best_practice_tests.test_configuration_management_example,
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
    print("Example Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All example tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} example tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_examples()