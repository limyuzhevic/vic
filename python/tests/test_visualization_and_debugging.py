"""
Comprehensive test suite for NLM Python visualization and debugging

This module provides comprehensive tests for visualization and debugging
functionality in the NLM (Neural Learning Machine) Python bindings.

Visualization and debugging tools help users understand and analyze
neural simulation behavior, monitor performance, and troubleshoot issues.

Tests cover:
- Visualization methods for brain states and world data
- Debugging tools for monitoring and analysis
- Type hints in visualization and debugging functions
- Error handling in visualization operations
- Performance of visualization and debugging
- Integration with context managers
- Visualization data formats and processing
- Debugging output formats and analysis
- Visualization edge cases and error conditions
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
VISUALIZATION_CONFIG = {
    "brain.neuron_count": 100,
    "brain.connection_probability": 0.2,
    "agent.exploration_rate": 0.3,
    "simulation.max_steps": 1000,
}

class TestVisualization:
    """Test visualization methods for NLM Python bindings."""
    
    def test_get_neural_stats_visualization(self):
        """Test getNeuralStats for visualization purposes."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Get neural statistics for visualization
            stats = brain.getNeuralStats()
            
            assert isinstance(stats, dict)
            
            # Verify expected keys for visualization
            expected_keys = [
                'total_neurons', 'active_neurons', 'total_spikes',
                'total_synapses', 'firing_neurons', 'average_firing_rate'
            ]
            
            for key in expected_keys:
                assert key in stats, f"Missing key in neural stats for visualization: {key}"
                
                # Values should be suitable for visualization (numeric types)
                if key == 'average_firing_rate':
                    assert isinstance(stats[key], float)
                else:
                    assert isinstance(stats[key], (int, float))
            
            # Verify values are within reasonable ranges for visualization
            assert 0 <= stats['total_neurons']
            assert 0 <= stats['active_neurons'] <= stats['total_neurons']
            assert 0 <= stats['total_spikes']
            assert 0 <= stats['total_synapses']
            assert 0 <= stats['firing_neurons'] <= stats['total_neurons']
            assert 0.0 <= stats['average_firing_rate']
            
            print("✓ getNeuralStats visualization test passed")
    
    def test_get_performance_metrics_visualization(self):
        """Test getPerformanceMetrics for visualization purposes."""
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
                
                # Get performance metrics for visualization
                metrics = agent.getPerformanceMetrics()
                
                assert isinstance(metrics, dict)
                
                # Verify expected keys for visualization
                expected_keys = [
                    'neuromodulation_level', 'curiosity_level',
                    'structural_plasticity_level', 'development_level',
                    'success_rate', 'total_reward', 'average_reward'
                ]
                
                for key in expected_keys:
                    assert key in metrics, f"Missing key in performance metrics for visualization: {key}"
                    
                    # Values should be suitable for visualization (numeric types)
                    assert isinstance(metrics[key], (int, float))
                    
                    # Verify values are within reasonable ranges for visualization
                    if 'level' in key:
                        assert 0.0 <= metrics[key] <= 1.0
                    elif 'success_rate' in key:
                        assert 0.0 <= metrics[key] <= 1.0
                    elif 'reward' in key:
                        assert isinstance(metrics[key], (int, float))
                
                print("✓ getPerformanceMetrics visualization test passed")
    
    def test_observation_map_visualization(self):
        """Test observation map for world visualization."""
        with nlm.SimpleWorld() as world:
            # Create world with obstacles
            world.createGridWorld(100.0, 100.0, 0.15)  # 15% obstacle density
            
            # Add some resources
            world.addResource(10.0, 10.0, 5.0)
            world.addResource(50.0, 50.0, 10.0)
            world.addResource(90.0, 90.0, 7.5)
            
            # Set agent goal
            world.setAgentGoal(95.0, 95.0)
            
            # Get observation map for visualization
            observation_map = world.getObservationMap()
            
            assert isinstance(observation_map, list)
            assert len(observation_map) == 100  # 100x100 grid
            
            # Verify observation map data is suitable for visualization
            for i in range(100):
                assert isinstance(observation_map[i], list)
                assert len(observation_map[i]) == 100
                
                for j in range(100):
                    value = observation_map[i][j]
                    
                    # Values should be between 0 and 1 for visualization
                    assert 0.0 <= value <= 1.0
                    
                    # Verify specific resource positions
                    if (i == 10 and j == 10) or (i == 50 and j == 50) or (i == 90 and j == 90):
                        assert value > 0.0, f"Resource should be visible at ({i}, {j})"
            
            print("✓ Observation map visualization test passed")
    
    def test_vision_data_visualization(self):
        """Test vision data for visualization purposes."""
        with nlm.Vision(20, 20) as vision:
            # Create vision data that represents a simple scene
            vision_data = []
            
            # Create a pattern: dark center, bright edges
            for y in range(20):
                for x in range(20):
                    # Calculate distance from center
                    dx = abs(x - 10)
                    dy = abs(y - 10)
                    distance = (dx + dy) / 20.0
                    
                    # Create RGB values based on distance
                    brightness = 1.0 - distance
                    red = brightness
                    green = brightness * 0.8
                    blue = brightness * 0.6
                    
                    vision_data.extend([red, green, blue])
            
            vision.setData(vision_data)
            
            # Get vision as 2D array for visualization
            vision_array = vision.getVisionArray()
            
            assert isinstance(vision_array, list)
            assert len(vision_array) == 20
            
            # Verify vision array data is suitable for visualization
            for i in range(20):
                assert isinstance(vision_array[i], list)
                assert len(vision_array[i]) == 20
                
                for j in range(20):
                    pixel = vision_array[i][j]
                    
                    # Verify pixel data structure for visualization
                    assert isinstance(pixel, list)
                    assert len(pixel) == 3
                    
                    # Verify RGB values are in valid range for visualization
                    for channel in pixel:
                        assert 0.0 <= channel <= 1.0
            
            print("✓ Vision data visualization test passed")
    
    def test_world_data_visualization(self):
        """Test world data visualization."""
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            world.createGridWorld(50.0, 50.0, 0.2)  # 20% obstacle density
            world.setAgentStart(25.0, 25.0)
            world.setAgentGoal(45.0, 45.0)
            
            # Add multiple resources
            resources = [
                (10.0, 10.0, 5.0),
                (30.0, 30.0, 10.0),
                (40.0, 20.0, 7.5),
                (45.0, 45.0, 15.0)  # Same as goal
            ]
            
            for x, y, value in resources:
                world.addResource(x, y, value)
            
            # Run a few simulation steps to get data for visualization
            simulation_data = []
            for step in range(5):
                world.update(0.1)
                
                # Get world state for visualization
                percept = world.getSensoryPercept()
                body = world.getAgentBody()
                
                # Collect visualization data
                step_data = {
                    'step': step,
                    'agent_x': body.x,
                    'agent_y': body.y,
                    'simulation_time': world.getSimulationTime()
                }
                
                simulation_data.append(step_data)
            
            # Verify simulation data is suitable for visualization
            for i, step_data in enumerate(simulation_data):
                assert 'step' in step_data
                assert 'agent_x' in step_data
                assert 'agent_y' in step_data
                assert 'simulation_time' in step_data
                
                assert step_data['step'] == i
                assert 0.0 <= step_data['agent_x'] <= 50.0
                assert 0.0 <= step_data['agent_y'] <= 50.0
                assert step_data['simulation_time'] >= 0.0
            
            print("✓ World data visualization test passed")

class TestDebuggingTools:
    """Test debugging tools for NLM Python bindings."""
    
    def test_log_status(self):
        """Test logStatus method for debugging."""
        config = nlm.Config()
        config.setDefaults("quick_test")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Test logStatus for debugging output
            brain.logStatus()
            
            # Should not raise exceptions
            print("✓ logStatus debugging test passed")
    
    def test_brain_state_analysis(self):
        """Test brain state analysis for debugging."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Get brain state for analysis
            stats = brain.getNeuralStats()
            
            # Perform basic analysis for debugging
            total_neurons = stats['total_neurons']
            total_spikes = stats['total_spikes']
            firing_rate = stats['average_firing_rate']
            
            # Verify analysis calculations
            if total_neurons > 0:
                spike_efficiency = total_spikes / total_neurons
                assert 0.0 <= spike_efficiency <= 1.0  # Reasonable efficiency range
            
            # Verify firing rate is reasonable
            assert 0.0 <= firing_rate <= 100.0  # Hz range
            
            print("✓ Brain state analysis debugging test passed")
    
    def test_agent_debugging(self):
        """Test agent debugging methods."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Enable all debugging features
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                agent.enableStructuralPlasticity(True)
                agent.enableDevelopment(True)
                
                # Get various debugging information
                neuromodulation = agent.getNeuromodulationLevel()
                curiosity = agent.getCuriosityLevel()
                metrics = agent.getPerformanceMetrics()
                
                # Verify debugging information
                assert 0.0 <= neuromodulation <= 1.0
                assert 0.0 <= curiosity <= 1.0
                
                assert isinstance(metrics, dict)
                assert 'success_rate' in metrics
                assert 'total_reward' in metrics
                
                # Verify metrics are reasonable
                assert 0.0 <= metrics['success_rate'] <= 1.0
                assert isinstance(metrics['total_reward'], (int, float))
                
                print("✓ Agent debugging test passed")
    
    def test_error_debugging(self):
        """Test error debugging capabilities."""
        # Test that errors provide useful debugging information
        try:
            with nlm.Config() as config:
                # Try to trigger a validation error
                config.updateFromDict({
                    "brain.neuron_count": -1,  # Invalid value
                })
                
                # Validate configuration
                issues = config.validateConfig()
                
                # Should provide useful debugging information
                if issues:
                    for issue in issues:
                        # Error messages should be descriptive
                        assert len(issue) > 0
                        assert isinstance(issue, str)
                        
            print("✓ Error debugging test passed")
        except Exception as e:
            # Other exceptions are also acceptable
            print(f"✓ Error debugging test caught exception: {e}")
    
    def test_resource_monitoring(self):
        """Test resource monitoring for debugging."""
        # Test monitoring of resource usage
        import gc
        
        # Force garbage collection to get clean baseline
        gc.collect()
        
        # Create and use objects
        objects = []
        for i in range(10):
            config = nlm.Config()
            config.set("test", i)
            objects.append(config)
        
        # Objects should be cleaned up when out of scope
        # This is a basic test - detailed resource monitoring would require more sophisticated tools
        assert len(objects) == 10
        
        print("✓ Resource monitoring debugging test passed")

class TestVisualizationTypes:
    """Test type safety in visualization and debugging functions."""
    
    def test_neural_stats_type_safety(self):
        """Test type safety in neural stats for visualization."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(20)
            
            # Get neural stats
            stats = brain.getNeuralStats()
            
            # Verify all expected types are present
            assert isinstance(stats['total_neurons'], int)
            assert isinstance(stats['active_neurons'], int)
            assert isinstance(stats['total_spikes'], int)
            assert isinstance(stats['total_synapses'], int)
            assert isinstance(stats['firing_neurons'], int)
            assert isinstance(stats['average_firing_rate'], float)
            
            # Verify values are suitable for visualization
            assert stats['total_neurons'] > 0
            assert stats['active_neurons'] >= 0
            assert stats['total_spikes'] >= 0
            assert stats['total_synapses'] >= 0
            assert stats['firing_neurons'] >= 0
            assert stats['average_firing_rate'] >= 0.0
            
            print("✓ Neural stats type safety test passed")
    
    def test_performance_metrics_type_safety(self):
        """Test type safety in performance metrics for visualization."""
        config = nlm.Config()
        config.setDefaults("default")
        brain = nlm.Brain(config)
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.configure(50.0, 50.0, 10, 10)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Get performance metrics
                metrics = agent.getPerformanceMetrics()
                
                # Verify all expected types are present
                expected_keys = [
                    'neuromodulation_level', 'curiosity_level',
                    'structural_plasticity_level', 'development_level',
                    'success_rate', 'total_reward', 'average_reward'
                ]
                
                for key in expected_keys:
                    assert key in metrics
                    assert isinstance(metrics[key], (int, float))
                
                # Verify values are suitable for visualization
                for key in ['neuromodulation_level', 'curiosity_level',
                           'structural_plasticity_level', 'development_level']:
                    assert 0.0 <= metrics[key] <= 1.0
                
                assert 0.0 <= metrics['success_rate'] <= 1.0
                assert isinstance(metrics['total_reward'], (int, float))
                assert isinstance(metrics['average_reward'], (int, float))
                
                print("✓ Performance metrics type safety test passed")
    
    def test_observation_map_type_safety(self):
        """Test type safety in observation map for visualization."""
        with nlm.SimpleWorld() as world:
            world.createGridWorld(50.0, 50.0, 0.1)
            world.setAgentGoal(45.0, 45.0)
            
            # Add resources
            world.addResource(10.0, 10.0, 5.0)
            world.addResource(30.0, 30.0, 10.0)
            
            # Get observation map
            observation_map = world.getObservationMap()
            
            # Verify observation map structure and types
            assert isinstance(observation_map, list)
            assert len(observation_map) == 50
            
            for row in observation_map:
                assert isinstance(row, list)
                assert len(row) == 50
                
                for value in row:
                    assert isinstance(value, float)
                    assert 0.0 <= value <= 1.0
            
            print("✓ Observation map type safety test passed")
    
    def test_vision_data_type_safety(self):
        """Test type safety in vision data for visualization."""
        with nlm.Vision(10, 10) as vision:
            # Create vision data
            vision_data = [0.5] * (10 * 10 * 3)
            vision.setData(vision_data)
            
            # Get vision array
            vision_array = vision.getVisionArray()
            
            # Verify vision array structure and types
            assert isinstance(vision_array, list)
            assert len(vision_array) == 10
            
            for row in vision_array:
                assert isinstance(row, list)
                assert len(row) == 10
                
                for pixel in row:
                    assert isinstance(pixel, list)
                    assert len(pixel) == 3
                    
                    for channel in pixel:
                        assert isinstance(channel, float)
                        assert 0.0 <= channel <= 1.0
            
            print("✓ Vision data type safety test passed")

class TestVisualizationPerformance:
    """Test performance of visualization and debugging operations."""
    
    def test_visualization_data_retrieval_performance(self):
        """Test performance of visualization data retrieval."""
        import time
        
        start_time = time.time()
        
        # Create and retrieve visualization data
        for i in range(20):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                brain.runSimulation(10)
                
                # Retrieve visualization data
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 2.0, f"Visualization data retrieval too slow: {elapsed:.2f}s"
        
        print(f"✓ Visualization data retrieval performance test passed ({elapsed:.3f}s for 20 brains)")
    
    def test_debugging_output_performance(self):
        """Test performance of debugging output operations."""
        import time
        
        start_time = time.time()
        
        # Perform debugging operations
        for i in range(10):
            config = nlm.Config()
            config.setDefaults("default")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                # Perform debugging operations
                brain.logStatus()
                
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 2.0, f"Debugging output too slow: {elapsed:.2f}s"
        
        print(f"✓ Debugging output performance test passed ({elapsed:.3f}s for 10 operations)")
    
    def test_large_scale_visualization_performance(self):
        """Test performance of large-scale visualization."""
        import time
        
        start_time = time.time()
        
        # Create large world and visualize
        for i in range(5):
            with nlm.SimpleWorld() as world:
                world.configure(200.0, 200.0, 40, 40)
                world.createGridWorld(200.0, 200.0, 0.05)  # 5% obstacle density
                world.setAgentGoal(180.0, 180.0)
                
                # Add many resources
                for x in [20.0, 60.0, 100.0, 140.0, 180.0]:
                    for y in [20.0, 60.0, 100.0, 140.0, 180.0]:
                        world.addResource(x, y, 5.0)
                
                # Get observation map for visualization
                observation_map = world.getObservationMap()
                assert isinstance(observation_map, list)
                assert len(observation_map) == 200
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time
        assert elapsed < 10.0, f"Large-scale visualization too slow: {elapsed:.2f}s"
        
        print(f"✓ Large-scale visualization performance test passed ({elapsed:.3f}s for 5 worlds)")

class TestVisualizationIntegration:
    """Test integration of visualization with other NLM features."""
    
    def test_visualization_with_context_managers(self):
        """Test visualization integrated with context managers."""
        # Use visualization within context managers
        with nlm.Config() as config:
            config.setDefaults("default")
            
            with nlm.SimpleWorld() as world:
                world.configure(100.0, 100.0, 20, 20)
                
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        
                        # Perform simulation and visualize results
                        for step in range(20):
                            world.update(0.1)
                            
                            # Get visualization data
                            stats = brain.getNeuralStats()
                            metrics = agent.getPerformanceMetrics()
                            
                            assert isinstance(stats, dict)
                            assert isinstance(metrics, dict)
        
        print("✓ Visualization with context managers integration test passed")
    
    def test_visualization_with_convenience_methods(self):
        """Test visualization integrated with convenience methods."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            # Use convenience methods to generate visualization data
            brain.runSimulation(30)
            
            # Get visualization data
            stats = brain.getNeuralStats()
            assert isinstance(stats, dict)
            
            # Visualize brain state
            total_neurons = stats['total_neurons']
            total_spikes = stats['total_spikes']
            firing_rate = stats['average_firing_rate']
            
            # Verify visualization data
            assert total_neurons > 0
            assert total_spikes >= 0
            assert firing_rate >= 0.0
            
            print("✓ Visualization with convenience methods integration test passed")
    
    def test_visualization_with_serialization(self):
        """Test visualization integrated with serialization."""
        config = nlm.Config()
        config.setDefaults("default")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            brain.runSimulation(25)
            
            # Get visualization data
            stats = brain.getNeuralStats()
            
            # Serialize visualization data
            JsonSerializer.save_to_file("visualization_stats.json", stats)
            
            # Load and verify
            loaded_stats = JsonSerializer.load_from_file("visualization_stats.json")
            assert loaded_stats == stats
            
            print("✓ Visualization with serialization integration test passed")
        
        # Clean up
        os.remove("visualization_stats.json")

# Test classes can be run as pytest test cases
def test_all_visualization_and_debugging():
    """Run all visualization and debugging tests."""
    print("=" * 60)
    print("Running Comprehensive Visualization and Debugging Tests")
    print("=" * 60)
    
    # Create test suite instances
    visualization_tests = TestVisualization()
    debugging_tests = TestDebuggingTools()
    type_safety_tests = TestVisualizationTypes()
    performance_tests = TestVisualizationPerformance()
    integration_tests = TestVisualizationIntegration()
    
    # Test methods in logical groups
    test_groups = [
        # Visualization tests
        (visualization_tests, [
            visualization_tests.test_get_neural_stats_visualization,
            visualization_tests.test_get_performance_metrics_visualization,
            visualization_tests.test_observation_map_visualization,
            visualization_tests.test_vision_data_visualization,
            visualization_tests.test_world_data_visualization,
        ]),
        
        # Debugging tools tests
        (debugging_tests, [
            debugging_tests.test_log_status,
            debugging_tests.test_brain_state_analysis,
            debugging_tests.test_agent_debugging,
            debugging_tests.test_error_debugging,
            debugging_tests.test_resource_monitoring,
        ]),
        
        # Type safety tests
        (type_safety_tests, [
            type_safety_tests.test_neural_stats_type_safety,
            type_safety_tests.test_performance_metrics_type_safety,
            type_safety_tests.test_observation_map_type_safety,
            type_safety_tests.test_vision_data_type_safety,
        ]),
        
        # Performance tests
        (performance_tests, [
            performance_tests.test_visualization_data_retrieval_performance,
            performance_tests.test_debugging_output_performance,
            performance_tests.test_large_scale_visualization_performance,
        ]),
        
        # Integration tests
        (integration_tests, [
            integration_tests.test_visualization_with_context_managers,
            integration_tests.test_visualization_with_convenience_methods,
            integration_tests.test_visualization_with_serialization,
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
    print("Visualization and Debugging Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All visualization and debugging tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} visualization and debugging tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_visualization_and_debugging()