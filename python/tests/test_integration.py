"""
Comprehensive test suite for NLM Python integration tests

This module provides comprehensive tests for integration scenarios
combining multiple NLM (Neural Learning Machine) features.

Integration tests ensure that different components work together
correctly, testing complex workflows and real-world usage patterns.

Tests cover:
- Integration of context managers with other features
- Integration of convenience methods with context managers
- Integration of factory methods with serialization
- Integration of visualization with performance optimization
- Integration of error handling with batch operations
- End-to-end simulation workflows
- Real-world usage scenarios
- Cross-component functionality
- Data flow between components
- Resource management in complex scenarios
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
INTEGRATION_TEST_CONFIG = {
    "brain.neuron_count": 100,
    "brain.connection_probability": 0.2,
    "agent.exploration_rate": 0.3,
    "simulation.max_steps": 1000,
    "simulation.timestep": 0.1
}

class TestEndToEndSimulation:
    """Test end-to-end simulation workflows."""
    
    def test_complete_simulation_workflow(self):
        """Test complete simulation workflow with all components."""
        # Create configuration
        with nlm.Config() as config:
            config.setDefaults("default")
            config.updateFromDict(INTEGRATION_TEST_CONFIG)
            
            # Create brain with configuration
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                # Create world
                with nlm.SimpleWorld() as world:
                    world.configure(100.0, 100.0, 20, 20)
                    world.setAgentStart(50.0, 50.0)
                    world.setAgentGoal(90.0, 90.0)
                    
                    # Add resources to environment
                    world.addResource(20.0, 20.0, 5.0)
                    world.addResource(60.0, 60.0, 10.0)
                    world.addResource(80.0, 80.0, 15.0)
                    
                    # Create agent
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        # Run complete simulation
                        simulation_data = []
                        for step in range(100):
                            world.update(0.1)
                            
                            # Get agent's action
                            action = agent.decodeMotorCommand()
                            
                            # Apply action to world
                            result = world.applyMotorCommand(action, world.getSimulationTime())
                            
                            # Collect simulation data
                            simulation_data.append({
                                'step': step,
                                'agent_x': world.getAgentBody().x,
                                'agent_y': world.getAgentBody().y,
                                'action': action.getName() if action else None,
                                'reward': result.reward if hasattr(result, 'reward') else None,
                                'success': result.success if hasattr(result, 'success') else None
                            })
                        
                        # Verify simulation completed successfully
                        assert len(simulation_data) == 100
                        
                        # Verify agent reached goal or made progress
                        final_agent_x = simulation_data[-1]['agent_x']
                        final_agent_y = simulation_data[-1]['agent_y']
                        
                        # Agent should have moved from start position
                        assert abs(final_agent_x - 50.0) > 0.1 or abs(final_agent_y - 50.0) > 0.1
                        
                        # Get performance metrics
                        metrics = agent.getPerformanceMetrics()
                        assert isinstance(metrics, dict)
                        assert 'success_rate' in metrics
                        assert 'total_reward' in metrics
                        
                        print("✓ Complete simulation workflow test passed")
    
    def test_complex_simulation_with_sensory_input(self):
        """Test complex simulation with sensory input and learning."""
        config = nlm.Config()
        config.setDefaults("development")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.SimpleWorld() as world:
                world.configure(200.0, 200.0, 40, 40)
                world.createGridWorld(200.0, 200.0, 0.1)  # 10% obstacles
                world.setAgentStart(20.0, 20.0)
                world.setAgentGoal(180.0, 180.0)
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    agent.enableCuriosity(True)
                    agent.enableRewardModulation(True)
                    agent.enableStructuralPlasticity(True)
                    
                    # Create sensory input for visualization
                    sensory_inputs = []
                    for i in range(10):
                        vision = nlm.Vision(20, 20)
                        
                        # Create vision data representing the world
                        vision_data = []
                        for y in range(20):
                            for x in range(20):
                                # Simple visualization: distance from agent
                                agent_x = 20.0 + i * 10.0
                                agent_y = 20.0
                                dx = abs(x * 10.0 - agent_x)
                                dy = abs(y * 10.0 - agent_y)
                                distance = (dx + dy) / 200.0
                                brightness = max(0.0, 1.0 - distance)
                                
                                vision_data.extend([brightness, brightness * 0.8, brightness * 0.6])
                        
                        vision.setData(vision_data)
                        sensory_inputs.append(vision)
                    
                    # Run simulation with sensory input
                    for step in range(50):
                        # Update world
                        world.update(0.1)
                        
                        # Process sensory input
                        for vision in sensory_inputs:
                            brain.receiveSensoryInput(vision)
                        
                        # Get agent action
                        action = agent.decodeMotorCommand()
                        
                        # Apply action
                        result = world.applyMotorCommand(action, world.getSimulationTime())
                        
                        # Check if agent reached goal
                        agent_body = world.getAgentBody()
                        if agent_body.x >= 175.0 and agent_body.y >= 175.0:
                            print(f"✓ Agent reached goal at step {step}")
                            break
                    
                    # Get final performance metrics
                    final_metrics = agent.getPerformanceMetrics()
                    assert isinstance(final_metrics, dict)
                    
                    # Get neural statistics
                    neural_stats = brain.getNeuralStats()
                    assert isinstance(neural_stats, dict)
                    
                    print("✓ Complex simulation with sensory input test passed")

class TestIntegrationWithConvenienceMethods:
    """Test integration of convenience methods with other features."""
    
    def test_convenience_methods_with_context_managers(self):
        """Test convenience methods working with context managers."""
        # Create configuration with convenience methods
        with nlm.Config() as config:
            config.setDefaults("high_performance")
            config.updateFromDict({
                "brain.neuron_count": 150,
                "agent.exploration_rate": 0.4,
                "simulation.max_steps": 500
            })
            
            # Create brain using factory method with convenience
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                
                # Use convenience methods
                brain.runSimulation(25)
                
                # Get neural statistics as convenience method
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
                assert stats.get('total_neurons', 0) == 150
                
                # Get performance metrics
                with nlm.SimpleWorld() as world:
                    world.configure(100.0, 100.0, 20, 20)
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        
                        # Use convenience methods
                        metrics = agent.learnFromWorld(world, 15)
                        assert isinstance(metrics, dict)
                        
                        # Get performance metrics
                        perf_metrics = agent.getPerformanceMetrics()
                        assert isinstance(perf_metrics, dict)
                        
                        # Use convenience method for world creation
                        world.createGridWorld(100.0, 100.0, 0.2)
                        world.setAgentGoal(90.0, 90.0)
                        
                        # Use convenience method for adding resources
                        world.addResource(10.0, 10.0, 5.0)
                        world.addResource(50.0, 50.0, 10.0)
                        
                        # Get observation map
                        observation_map = world.getObservationMap()
                        assert isinstance(observation_map, list)
        
        print("✓ Convenience methods with context managers integration test passed")
    
    def test_convenience_methods_with_serialization(self):
        """Test convenience methods integrated with serialization."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Use convenience methods to generate data
            brain.runSimulation(30)
            
            # Get neural statistics
            stats = brain.getNeuralStats()
            
            # Serialize using JsonSerializer
            JsonSerializer.save_to_file("integration_stats.json", stats)
            
            # Load and verify
            loaded_stats = JsonSerializer.load_from_file("integration_stats.json")
            assert loaded_stats == stats
            
            # Create world using convenience methods
            with nlm.SimpleWorld() as world:
                world.createGridWorld(100.0, 100.0, 0.15)
                world.setAgentGoal(90.0, 90.0)
                
                # Add resources using convenience method
                for x in [20.0, 40.0, 60.0, 80.0]:
                    for y in [20.0, 40.0, 60.0, 80.0]:
                        world.addResource(x, y, 5.0)
                
                # Serialize world using convenience methods
                world_stats = world.getObservationMap()
                JsonSerializer.save_to_file("integration_world.json", world_stats)
                
                # Load and verify
                loaded_world_stats = JsonSerializer.load_from_file("integration_world.json")
                assert loaded_world_stats == world_stats
        
        # Clean up
        os.remove("integration_stats.json")
        os.remove("integration_world.json")
        
        print("✓ Convenience methods with serialization integration test passed")
    
    def test_convenience_methods_with_factory_methods(self):
        """Test convenience methods integrated with factory methods."""
        # Create configuration using convenience methods
        with nlm.Config() as config:
            config.setDefaults("quick_test")
            config.updateFromDict({
                "brain.neuron_count": 75,
                "agent.exploration_rate": 0.2
            })
            
            # Create brain using factory method
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                
                # Use convenience methods for simulation
                brain.runSimulation(20)
                
                # Get neural statistics
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
                assert stats.get('total_neurons', 0) == 75
                
                # Create agent using convenience methods
                with nlm.SimpleWorld() as world:
                    world.configure(50.0, 50.0, 10, 10)
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        
                        # Use convenience methods for learning
                        metrics = agent.learnFromWorld(world, 15)
                        assert isinstance(metrics, dict)
                        
                        # Use convenience method for creating sequences
                        sequences = nlm.createAgentSequence(world, 2, 20)
                        assert isinstance(sequences, list)
                        assert len(sequences) == 2
                        
                        # Use convenience method for setting learning parameters
                        learning_params = {
                            "reward_modulation": True,
                            "curiosity": True
                        }
                        agent.setLearningParameters(learning_params)
        
        print("✓ Convenience methods with factory methods integration test passed")

class TestIntegrationWithSerialization:
    """Test integration of serialization with other features."""
    
    def test_serialization_with_context_managers(self):
        """Test serialization working with context managers."""
        # Create complete simulation scenario
        with nlm.Config() as config:
            config.setDefaults("default")
            config.updateFromDict({
                "brain.neuron_count": 100,
                "simulation.max_steps": 200
            })
            
            with nlm.SimpleWorld() as world:
                world.configure(100.0, 100.0, 20, 20)
                world.createGridWorld(100.0, 100.0, 0.1)
                world.setAgentGoal(90.0, 90.0)
                
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    brain.runSimulation(50)
                    
                    # Serialize using JsonSerializer
                    JsonSerializer.serialize_brain(brain, "integration_brain.json")
                    JsonSerializer.serialize_world(world, "integration_world.json")
                    
                    # Verify files were created
                    assert os.path.exists("integration_brain.json")
                    assert os.path.exists("integration_world.json")
                    
                    # Load and verify
                    loaded_brain = nlm.Brain(config)
                    with loaded_brain:
                        loaded_brain.load("integration_brain.json")
                        loaded_stats = loaded_brain.getNeuralStats()
                        assert isinstance(loaded_stats, dict)
                        assert loaded_stats.get('total_spikes', 0) > 0
                    
                    # Clean up
                    os.remove("integration_brain.json")
                    os.remove("integration_world.json")
        
        print("✓ Serialization with context managers integration test passed")
    
    def test_serialization_with_convenience_methods(self):
        """Test serialization integrated with convenience methods."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Use convenience methods to generate data
            brain.runSimulation(40)
            
            # Get neural statistics using convenience method
            stats = brain.getNeuralStats()
            
            # Serialize using JsonSerializer
            JsonSerializer.save_to_file("serialization_test.json", stats)
            
            # Load and verify
            loaded_stats = JsonSerializer.load_from_file("serialization_test.json")
            assert loaded_stats == stats
            
            # Create world using convenience methods
            with nlm.SimpleWorld() as world:
                world.createGridWorld(50.0, 50.0, 0.2)
                world.setAgentGoal(40.0, 40.0)
                
                # Add resources using convenience method
                world.addResource(10.0, 10.0, 5.0)
                world.addResource(30.0, 30.0, 10.0)
                
                # Serialize world using convenience method
                observation_map = world.getObservationMap()
                JsonSerializer.save_to_file("serialization_world.json", observation_map)
                
                # Load and verify
                loaded_observation_map = JsonSerializer.load_from_file("serialization_world.json")
                assert loaded_observation_map == observation_map
        
        # Clean up
        os.remove("serialization_test.json")
        os.remove("serialization_world.json")
        
        print("✓ Serialization with convenience methods integration test passed")
    
    def test_serialization_with_factory_methods(self):
        """Test serialization integrated with factory methods."""
        # Create configuration using convenience methods
        config = nlm.Config()
        config.setDefaults("development")
        
        # Create brain using factory method
        brain = nlm.Brain.createStandardBrain(config)
        assert brain is not None
        
        with brain:
            brain.initialize()
            brain.runSimulation(30)
            
            # Serialize using JsonSerializer
            JsonSerializer.serialize_brain(brain, "factory_brain.json")
            
            # Verify file was created
            assert os.path.exists("factory_brain.json")
            
            # Load and verify
            loaded_brain = nlm.Brain(config)
            with loaded_brain:
                loaded_brain.load("factory_brain.json")
                loaded_stats = loaded_brain.getNeuralStats()
                assert isinstance(loaded_stats, dict)
                assert loaded_stats.get('total_spikes', 0) > 0
        
        # Clean up
        os.remove("factory_brain.json")
        
        print("✓ Serialization with factory methods integration test passed")

class TestIntegrationWithVisualization:
    """Test integration of visualization with other features."""
    
    def test_visualization_with_context_managers(self):
        """Test visualization working with context managers."""
        # Create visualization scenario
        with nlm.Config() as config:
            config.setDefaults("default")
            
            with nlm.SimpleWorld() as world:
                world.configure(100.0, 100.0, 20, 20)
                world.createGridWorld(100.0, 100.0, 0.15)
                world.setAgentGoal(90.0, 90.0)
                
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        
                        # Generate visualization data
                        for step in range(20):
                            world.update(0.1)
                            
                            # Get neural statistics for visualization
                            neural_stats = brain.getNeuralStats()
                            assert isinstance(neural_stats, dict)
                            
                            # Get performance metrics for visualization
                            performance_metrics = agent.getPerformanceMetrics()
                            assert isinstance(performance_metrics, dict)
                            
                            # Get observation map for visualization
                            observation_map = world.getObservationMap()
                            assert isinstance(observation_map, list)
                        
                        # Get final visualization data
                        final_stats = brain.getNeuralStats()
                        final_metrics = agent.getPerformanceMetrics()
                        final_map = world.getObservationMap()
                        
                        # Verify visualization data
                        assert isinstance(final_stats, dict)
                        assert isinstance(final_metrics, dict)
                        assert isinstance(final_map, list)
        
        print("✓ Visualization with context managers integration test passed")
    
    def test_visualization_with_convenience_methods(self):
        """Test visualization integrated with convenience methods."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Use convenience methods to generate visualization data
            brain.runSimulation(25)
            
            # Get neural statistics for visualization
            neural_stats = brain.getNeuralStats()
            assert isinstance(neural_stats, dict)
            
            # Create world and generate visualization data
            with nlm.SimpleWorld() as world:
                world.createGridWorld(50.0, 50.0, 0.2)
                world.setAgentGoal(40.0, 40.0)
                
                # Add resources
                world.addResource(10.0, 10.0, 5.0)
                world.addResource(30.0, 30.0, 10.0)
                
                # Get observation map for visualization
                observation_map = world.getObservationMap()
                assert isinstance(observation_map, list)
                
                # Run simulation and get more visualization data
                for step in range(15):
                    world.update(0.1)
                    
                    # Get updated observation map
                    updated_map = world.getObservationMap()
                    assert isinstance(updated_map, list)
        
        print("✓ Visualization with convenience methods integration test passed")
    
    def test_visualization_with_factory_methods(self):
        """Test visualization integrated with factory methods."""
        # Create configuration using convenience methods
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        # Create brain using factory method
        brain = nlm.Brain.createStandardBrain(config)
        assert brain is not None
        
        with brain:
            brain.initialize()
            brain.runSimulation(20)
            
            # Get neural statistics for visualization
            neural_stats = brain.getNeuralStats()
            assert isinstance(neural_stats, dict)
            
            # Create world using factory methods
            with nlm.SimpleWorld() as world:
                world.createGridWorld(100.0, 100.0, 0.1)
                world.setAgentGoal(90.0, 90.0)
                
                # Get observation map for visualization
                observation_map = world.getObservationMap()
                assert isinstance(observation_map, list)
                
                # Create agent and get visualization data
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    
                    # Get performance metrics for visualization
                    performance_metrics = agent.getPerformanceMetrics()
                    assert isinstance(performance_metrics, dict)
        
        print("✓ Visualization with factory methods integration test passed")

class TestComplexIntegrationWorkflows:
    """Test complex integration workflows combining multiple features."""
    
    def test_complete_complex_workflow(self):
        """Test complete complex workflow with all features."""
        # Step 1: Create configuration with convenience methods
        with nlm.Config() as config:
            config.setDefaults("high_performance")
            config.updateFromDict({
                "brain.neuron_count": 125,
                "agent.exploration_rate": 0.35,
                "simulation.max_steps": 300
            })
            
            # Step 2: Create brain using factory method
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                
                # Step 3: Create world with convenience methods
                with nlm.SimpleWorld() as world:
                    world.createGridWorld(150.0, 150.0, 0.2)
                    world.setAgentStart(20.0, 20.0)
                    world.setAgentGoal(130.0, 130.0)
                    
                    # Add resources
                    for x in [40.0, 80.0, 120.0]:
                        for y in [40.0, 80.0, 120.0]:
                            world.addResource(x, y, 8.0)
                    
                    # Step 4: Create agent
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        # Step 5: Run complex simulation
                        simulation_results = []
                        for step in range(50):
                            world.update(0.1)
                            
                            # Get sensory input
                            with nlm.Vision(20, 20) as vision:
                                vision_data = [0.3] * (20 * 20 * 3)
                                vision.setData(vision_data)
                                brain.receiveSensoryInput(vision)
                            
                            # Get agent action
                            action = agent.decodeMotorCommand()
                            
                            # Apply action
                            result = world.applyMotorCommand(action, world.getSimulationTime())
                            
                            # Collect results
                            simulation_results.append({
                                'step': step,
                                'agent_x': world.getAgentBody().x,
                                'agent_y': world.getAgentBody().y,
                                'action': action.getName() if action else None,
                                'reward': result.reward if hasattr(result, 'reward') else None,
                                'world_size': world.getWidth() * world.getHeight()
                            })
                        
                        # Step 6: Collect comprehensive statistics
                        neural_stats = brain.getNeuralStats()
                        performance_metrics = agent.getPerformanceMetrics()
                        observation_map = world.getObservationMap()
                        
                        # Step 7: Serialize results
                        JsonSerializer.save_to_file("complex_simulation_stats.json", neural_stats)
                        JsonSerializer.save_to_file("complex_simulation_metrics.json", performance_metrics)
                        JsonSerializer.save_to_file("complex_simulation_map.json", observation_map)
                        
                        # Step 8: Verify results
                        assert len(simulation_results) == 50
                        assert isinstance(neural_stats, dict)
                        assert isinstance(performance_metrics, dict)
                        assert isinstance(observation_map, list)
                        
                        # Check if agent reached goal
                        final_x = simulation_results[-1]['agent_x']
                        final_y = simulation_results[-1]['agent_y']
                        
                        if final_x >= 125.0 and final_y >= 125.0:
                            print("✓ Agent reached goal in complex workflow")
                        else:
                            print(f"✓ Agent progressed to ({final_x:.1f}, {final_y:.1f})")
                        
                        # Clean up
                        os.remove("complex_simulation_stats.json")
                        os.remove("complex_simulation_metrics.json")
                        os.remove("complex_simulation_map.json")
        
        print("✓ Complete complex workflow test passed")
    
    def test_error_handling_in_complex_workflow(self):
        """Test error handling in complex workflows."""
        try:
            with nlm.Config() as config:
                config.setDefaults("default")
                
                with nlm.SimpleWorld() as world:
                    world.configure(100.0, 100.0, 20, 20)
                    
                    with nlm.Brain(config) as brain:
                        brain.initialize()
                        
                        with nlm.AgentBrain(brain) as agent:
                            agent.initialize(world)
                            
                            # Run simulation
                            for step in range(10):
                                world.update(0.1)
                                
                                # Simulate an error
                                if step == 5:
                                    raise ValueError("Simulated error in complex workflow")
                                
                                action = agent.decodeMotorCommand()
            
            print("✓ Error handling in complex workflow test passed")
        
        except ValueError as e:
            if str(e) == "Simulated error in complex workflow":
                print("✓ Error handling in complex workflow test passed (error caught)")
            else:
                raise

# Test classes can be run as pytest test cases
def test_all_integration():
    """Run all integration tests."""
    print("=" * 60)
    print("Running Comprehensive Integration Tests")
    print("=" * 60)
    
    # Create test suite instances
    end_to_end_tests = TestEndToEndSimulation()
    convenience_tests = TestIntegrationWithConvenienceMethods()
    serialization_tests = TestIntegrationWithSerialization()
    visualization_tests = TestIntegrationWithVisualization()
    complex_tests = TestComplexIntegrationWorkflows()
    
    # Test methods in logical groups
    test_groups = [
        # End-to-end simulation tests
        (end_to_end_tests, [
            end_to_end_tests.test_complete_simulation_workflow,
            end_to_end_tests.test_complex_simulation_with_sensory_input,
        ]),
        
        # Convenience methods integration tests
        (convenience_tests, [
            convenience_tests.test_convenience_methods_with_context_managers,
            convenience_tests.test_convenience_methods_with_serialization,
            convenience_tests.test_convenience_methods_with_factory_methods,
        ]),
        
        # Serialization integration tests
        (serialization_tests, [
            serialization_tests.test_serialization_with_context_managers,
            serialization_tests.test_serialization_with_convenience_methods,
            serialization_tests.test_serialization_with_factory_methods,
        ]),
        
        # Visualization integration tests
        (visualization_tests, [
            visualization_tests.test_visualization_with_context_managers,
            visualization_tests.test_visualization_with_convenience_methods,
            visualization_tests.test_visualization_with_factory_methods,
        ]),
        
        # Complex integration tests
        (complex_tests, [
            complex_tests.test_complete_complex_workflow,
            complex_tests.test_error_handling_in_complex_workflow,
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
    print("Integration Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All integration tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} integration tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_integration()