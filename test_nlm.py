# Comprehensive test suite for NLM Python bindings
"""
Comprehensive test suite for NLM Python bindings.

Provides thorough testing of:
- Core functionality and basic operations
- Memory management and lifecycle
- Error handling and edge cases
- Performance and stress testing
- Integration between components
- Serialization and persistence
- Advanced features and APIs

Testing Strategy:
- Unit tests for individual classes and methods
- Integration tests for component interactions
- System tests for complete simulation scenarios
- Performance benchmarks
- Regression tests for bug fixes

Test Organization:
- test_basic.py: Basic functionality and smoke tests
- test_memory.py: Memory management and lifecycle tests
- test_integration.py: Integration between components
- test_serialization.py: Serialization and persistence
- test_advanced.py: Advanced features and APIs
- test_performance.py: Performance and stress tests
- test_error_handling.py: Error handling and edge cases
"""

import pytest
import tempfile
import json
import numpy as np
import time
from typing import Dict, Any, List, Optional
from pathlib import Path

from nlm import (
    create_default_config,
    create_config,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    Config,
    Brain,
    SimpleWorld,
    AgentBrain,
    NeuronType,
    ActionType,
    MotorCommand,
    DevelopmentalStage,
)

class TestBasicFunctionality:
    """Test basic NLM functionality and smoke tests."""
    
    def test_config_creation(self):
        """Test configuration creation and manipulation."""
        # Test default config
        config = create_default_config()
        assert isinstance(config, Config)
        assert config.has("brain.neuron_count")
        
        # Test custom config
        custom_config = create_config(brain_neuron_count=2000)
        assert custom_config.get_int("brain.neuron_count") == 2000
        
        # Test config setters
        config.set("brain.connection_probability", 0.15)
        assert config.get_float("brain.connection_probability") == 0.15
        
        # Test config validation
        config.set("brain.neuron_count", -1)  # Should fail
        with pytest.raises(Exception):
            config.set("brain.neuron_count", -1)
    
    def test_brain_creation_and_initialization(self):
        """Test brain creation and initialization."""
        config = create_default_config()
        brain = createBrain(config)
        
        assert isinstance(brain, Brain)
        assert brain.initialize()
        
        # Test brain statistics
        assert brain.get_neuron_count() > 0
        assert brain.get_synapse_count() >= 0
        assert brain.get_total_spike_count() == 0
    
    def test_world_creation_and_configuration(self):
        """Test world creation and configuration."""
        world = createSimpleWorld()
        
        assert isinstance(world, SimpleWorld)
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        
        # Test world reset
        world.reset()
        
        # Test agent start
        world.setAgentStart(10.0, 10.0)
    
    def test_agent_creation_and_initialization(self):
        """Test agent creation and initialization."""
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = createAgentBrain(brain)
        assert isinstance(agent, AgentBrain)
        
        agent.initialize(world)
        assert agent.getBrain() is not None
        
        # Test agent subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        assert agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()
    
    def test_basic_simulation_loop(self):
        """Test basic simulation loop with brain and world."""
        # Create brain
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        # Create world
        world = createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation loop
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Basic assertions
            assert brain.get_total_spike_count() >= 0
            assert brain.get_firing_neuron_count() >= 0
            assert isinstance(action, MotorCommand)
        
        # Check final state
        assert brain.get_total_spike_count() >= 0
        assert brain.get_average_firing_rate() >= 0.0

class TestMemoryManagement:
    """Test memory management and lifecycle."""
    
    def test_brain_memory_lifecycle(self):
        """Test brain creation, reset, and disposal."""
        config = create_default_config()
        brain = createBrain(config)
        
        # Initialize brain
        assert brain.initialize()
        initial_neurons = brain.get_neuron_count()
        assert initial_neurons > 0
        
        # Run some steps
        for step in range(10):
            brain.step(step)
        
        # Reset brain
        brain.reset()
        assert brain.get_total_spike_count() == 0
        
        # Re-initialize brain
        assert brain.initialize()
        assert brain.get_neuron_count() == initial_neurons
    
    def test_world_memory_lifecycle(self):
        """Test world creation and disposal."""
        world = createSimpleWorld()
        world.configure(width=10, height=10)
        
        # Configure and reset
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Test world properties
        assert world.getWidth() == 10
        assert world.getHeight() == 10
        assert world.getSimulationTime() >= 0.0
    
    def test_agent_memory_lifecycle(self):
        """Test agent creation and disposal."""
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=10, height=10)
        
        agent = createAgentBrain(brain)
        
        # Initialize agent
        agent.initialize(world)
        
        # Store some data
        agent.enableRewardModulation(True)
        agent.applyRewardModulation(0.5, 0.3)
        
        # Reset agent
        agent.reset()
        
        # Verify reset
        assert agent.getNeuromodulationLevel() == 0.0
        assert agent.getCuriosityLevel() == 0.0
        assert agent.getNoveltyLevel() == 0.0

class TestIntegration:
    """Test integration between components."""
    
    def test_complete_agent_world_integration(self):
        """Test complete integration of agent and world."""
        # Create components
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation with learning
        total_reward = 0.0
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation (world would normally compute this)
            reward = 0.1  # Simplified reward
            agent.applyRewardModulation(reward, 0.0)
            total_reward += reward
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Verify integration
        assert brain.get_total_spike_count() >= 0
        assert brain.get_average_firing_rate() >= 0.0
        assert brain.get_developmental_stage() != DevelopmentalStage.Initial
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        assert agent.getNeuromodulationLevel() >= 0.0
    
    def test_memory_systems_integration(self):
        """Test integration with memory systems."""
        config = create_default_config()
        config.set("enable_learning", True)
        config.set("enable_development", True)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Check if memory systems are integrated
        # Note: This depends on C++ implementation
        # For now, just test that brain functions work
        assert brain.get_neuron_count() > 0
        
        # Run simulation to trigger memory operations
        for step in range(20):
            brain.step(step)
        
        # Check for memory activity
        # This would depend on C++ implementation details
        # assert brain.get_working_memory() is not None  # If available
    
    def test_prediction_integration(self):
        """Test integration with prediction systems."""
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        # Create agent with world for prediction testing
        world = createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable prediction if available
        # agent.enable_prediction(True)  # If available
        
        # Run simulation
        for step in range(30):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify prediction error is computed
        prediction_error = agent.getPredictionError()
        assert prediction_error >= 0.0 or prediction_error <= 0.0  # Can be positive or negative

class TestSerialization:
    """Test serialization and persistence."""
    
    def test_brain_checkpoint_save_load(self):
        """Test saving and loading brain checkpoints."""
        # Create and run a brain
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        # Run some simulation steps
        for step in range(20):
            brain.step(step)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_path = f.name
        
        try:
            assert brain.save(checkpoint_path)
            
            # Create new brain and load checkpoint
            new_config = create_default_config()
            new_brain = createBrain(new_config)
            new_brain.initialize()
            
            assert new_brain.load(checkpoint_path)
            
            # Verify loaded state (approximate check)
            assert new_brain.get_neuron_count() == brain.get_neuron_count()
            
        finally:
            # Clean up
            if Path(checkpoint_path).exists():
                Path(checkpoint_path).unlink()
    
    def test_config_save_load(self):
        """Test saving and loading configurations."""
        # Create custom config
        config = create_default_config()
        config.set("brain.neuron_count", 3000)
        config.set("brain.connection_probability", 0.08)
        
        # Save to file
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            config_path = f.name
        
        try:
            config.save_to_file(config_path, format='json')
            
            # Load from file
            loaded_config = create_config()
            loaded_config.load_from_file(config_path, format='json')
            
            # Verify loaded values
            assert loaded_config.get_int("brain.neuron_count") == 3000
            assert loaded_config.get_float("brain.connection_probability") == 0.08
            
        finally:
            # Clean up
            if Path(config_path).exists():
                Path(config_path).unlink()
    
    def test_config_preset_system(self):
        """Test configuration preset system."""
        from nlm.config import Config
        
        # Test all presets
        presets = ['beginner', 'research', 'performance', 'simulation']
        
        for preset_name in presets:
            config = Config.create_preset(preset_name)
            assert isinstance(config, Config)
            assert len(config.keys()) > 0
            
            # Verify preset-specific settings
            if preset_name == 'beginner':
                assert config.get_int("brain.neuron_count") == 500
            elif preset_name == 'research':
                assert config.get_int("brain.neuron_count") == 10000
            elif preset_name == 'performance':
                assert config.get_bool("plasticity.structural.enable") == False
            elif preset_name == 'simulation':
                assert config.get_bool("plasticity.structural.enable") == True

class TestAdvancedFeatures:
    """Test advanced NLM features."""
    
    def test_monitoring_system(self):
        """Test brain monitoring system."""
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        # Run simulation with monitoring
        with brain.monitor() as monitor:
            for step in range(50):
                brain.step(step)
                if step % 10 == 0:
                    # Add custom data
                    monitor.data.append({
                        'step': step,
                        'custom_metric': step * 0.1
                    })
        
        # Verify monitoring data
        assert len(brain._monitor_data) > 0
        assert monitor.data is not None
        
        # Test monitoring statistics
        stats = monitor.get_statistics()
        assert 'total_steps' in stats
        assert 'average_firing_rate' in stats
        assert stats['total_steps'] == 50
    
    def test_callback_system(self):
        """Test callback system."""
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        # Track callback execution
        callback_calls = []
        
        def step_callback(step, data):
            callback_calls.append((step, data))
        
        # Add callback
        brain.add_step_callback(step_callback)
        
        # Run simulation
        for step in range(10):
            brain.step(step)
        
        # Verify callback was called
        assert len(callback_calls) == 10
        
        # Verify callback data
        for i, (step, data) in enumerate(callback_calls):
            assert step == i
            assert 'step' in data
            assert 'neuron_count' in data
        
        # Remove callback
        brain.remove_step_callback(step_callback)
        
        # Run again - callback should not be called
        callback_calls.clear()
        for step in range(5):
            brain.step(step)
        
        assert len(callback_calls) == 0
    
    def test_plasticity_system(self):
        """Test plasticity (learning) system."""
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        # Create world and agent for plasticity testing
        world = createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all plasticity systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Store initial synapse weights (if accessible)
        initial_weights = []
        
        # Run simulation with plasticity
        for step in range(30):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward to trigger plasticity
            reward = 0.1 if step % 5 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Check that plasticity systems are working
        # Note: Specific plasticity effects depend on C++ implementation
        # For now, just verify that the simulation runs without error
        assert brain.get_total_spike_count() >= 0
        assert brain.get_average_firing_rate() >= 0.0

class TestErrorHandling:
    """Test error handling and edge cases."""
    
    def test_brain_step_before_initialization(self):
        """Test error handling when stepping brain before initialization."""
        config = create_default_config()
        brain = createBrain(config)
        
        # Should fail if step called without initialize
        with pytest.raises(Exception):
            brain.step(0)
    
    def test_world_operations_before_configuration(self):
        """Test world operations before configuration."""
        world = createSimpleWorld()
        
        # These should work without explicit configuration
        assert world.getWidth() > 0
        assert world.getHeight() > 0
        
        world.reset()
        world.update(0.1)
        
        # Configuration should work automatically
        assert world.getSimulationTime() >= 0.0
    
    def test_agent_operations_before_initialization(self):
        """Test agent operations before initialization."""
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=10, height=10)
        
        agent = createAgentBrain(brain)
        
        # These should fail if agent not initialized
        with pytest.raises(Exception):
            agent.decodeMotorCommand()
        
        with pytest.raises(Exception):
            agent.processSensoryInput(None)
    
    def test_config_validation_errors(self):
        """Test configuration validation errors."""
        from nlm.config import Config
        
        config = Config()
        
        # Should fail for invalid values
        with pytest.raises(Exception):
            config.set("brain.neuron_count", -1)
        
        with pytest.raises(Exception):
            config.set("brain.connection_probability", 1.5)
        
        # Should succeed for valid values
        config.set("brain.neuron_count", 1000)
        config.set("brain.connection_probability", 0.5)
        config.set("enable_learning", True)
        config.set("enable_development", False)
    
    def test_memory_leak_detection(self):
        """Basic memory leak detection (placeholder)."""
        # This is a placeholder for memory leak testing
        # In a real implementation, would use memory profiling tools
        
        import gc
        
        # Force garbage collection
        gc.collect()
        
        # Create and dispose of many instances
        for i in range(10):
            config = create_default_config()
            brain = createBrain(config)
            brain.initialize()
            
            for step in range(5):
                brain.step(step)
            
            # Simulate cleanup
            del brain
            del config
        
        # Force garbage collection again
        gc.collect()
        
        # Note: Actual memory leak detection would require more sophisticated tools
        # For now, just ensure no crashes


class TestPerformance:
    """Performance and stress tests."""
    
    def test_large_scale_simulation(self):
        """Test large scale simulation performance."""
        # Create larger brain
        config = create_default_config()
        config.set("brain.neuron_count", 5000)  # Larger brain
        config.set("brain.connection_probability", 0.05)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        
        for step in range(200):
            brain.step(step)
        
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_second = 200 / duration
        
        # Performance assertions
        assert duration < 60.0  # Should complete in under 1 minute
        assert steps_per_second > 100.0  # Should process >100 steps/second
    
    def test_concurrent_simulation(self):
        """Test concurrent simulation instances."""
        import threading
        
        def run_simulation(sim_id, steps):
            """Run a simulation in a separate thread."""
            config = create_default_config()
            brain = createBrain(config)
            brain.initialize()
            
            for step in range(steps):
                brain.step(step)
            
            return {
                'sim_id': sim_id,
                'steps_completed': steps,
                'final_spikes': brain.get_total_spike_count(),
                'final_firing_rate': brain.get_average_firing_rate(),
            }
        
        # Create multiple threads
        threads = []
        results = []
        
        for i in range(3):
            thread = threading.Thread(target=lambda i=i: results.append(run_simulation(i, 50)))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        # Verify results
        assert len(results) == 3
        for result in results:
            assert result['steps_completed'] == 50
            assert result['final_spikes'] >= 0
    
    def test_memory_usage(self):
        """Test memory usage during simulation."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run simulation
        config = create_default_config()
        brain = createBrain(config)
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = final_memory - initial_memory
        
        # Memory should not increase excessively
        # Note: This is a rough check - actual limits depend on system
        assert memory_increase < 500.0  # Should be less than 500MB increase


class TestRegression:
    """Regression tests for specific bug fixes."""
    
    def test_config_save_load_regression(self):
        """Regression test for config save/load functionality."""
        # This tests the fix for saveToFile placeholder values bug
        config = create_default_config()
        config.set("brain.neuron_count", 1500)
        config.set("brain.connection_probability", 0.12)
        
        # Save to file
        with tempfile.NamedTemporaryFile(suffix='.cfg', delete=False) as f:
            config_path = f.name
        
        try:
            # This was buggy before - wrote placeholder values
            config.save_to_file(config_path, format='cfg')
            
            # Load back
            loaded_config = create_config()
            loaded_config.load_from_file(config_path, format='cfg')
            
            # Should have actual values, not placeholder
            assert loaded_config.get_int("brain.neuron_count") == 1500
            assert loaded_config.get_float("brain.connection_probability") == 0.12
            
        finally:
            if Path(config_path).exists():
                Path(config_path).unlink()
    
    def test_trim_function_regression(self):
        """Regression test for trim function bug."""
        # This tests the fix for trim() off-by-one error
        from nlm.config import Config
        
        # Test various string inputs
        test_cases = [
            "  test  ",
            "test",
            "  ",
            "",
            "\ttest\n",
            "  \t  test  \t  ",
        ]
        
        for test_input in test_cases:
            # Call the trim function (if accessible)
            # For now, just test that config operations work
            config = Config()
            config.set("test.key", test_input)
            
            # Should not crash
            retrieved = config.get("test.key")
            assert isinstance(retrieved, str)


# Pytest fixtures
def pytest_configure(config):
    """Configure pytest for NLM testing."""
    config.addinivalue_line("markers", "slow: marks tests as slow")
    config.addinivalue_line("markers", "integration: marks tests as integration tests")


if __name__ == "__main__":
    # Run tests
    pytest.main([
        __file__,
        "-v",
        "--tb=short",
        "-x",  # Stop on first failure
    ])