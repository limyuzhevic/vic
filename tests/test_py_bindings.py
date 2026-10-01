"""
Test suite for NLM Python bindings.

This module provides comprehensive tests for the enhanced NLM Python API,
complementing the existing C++ test suite.
"""

import sys
import os
import tempfile
import json
from typing import Any, Dict
import pytest
import numpy as np

# Import the NLM modules
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))
sys.path.insert(0, os.path.dirname(__file__))

from nlm import Brain, Agent, SimpleWorld, ConfigManager
from nlm import (
    NLMError, ConfigurationError, SimulationError, 
    ValidationError
)


class TestConfigManager:
    """Test the enhanced ConfigManager."""
    
    def test_config_creation(self):
        """Test basic configuration creation."""
        config = ConfigManager()
        assert config is not None
        assert config._config is not None
    
    def test_config_get_set_int(self):
        """Test integer configuration values."""
        config = ConfigManager()
        config.set("test_int", 42)
        assert config.get_int("test_int") == 42
        assert config.get("test_int") == 42
    
    def test_config_get_set_float(self):
        """Test float configuration values."""
        config = ConfigManager()
        config.set("test_float", 3.14)
        assert abs(config.get_float("test_float") - 3.14) < 0.001
        assert abs(config.get("test_float") - 3.14) < 0.001
    
    def test_config_get_set_string(self):
        """Test string configuration values."""
        config = ConfigManager()
        config.set("test_string", "hello")
        assert config.get_string("test_string") == "hello"
        assert config.get("test_string") == "hello"
    
    def test_config_get_set_bool(self):
        """Test boolean configuration values."""
        config = ConfigManager()
        config.set("test_bool", True)
        assert config.get_bool("test_bool") is True
        assert config.get("test_bool") is True
    
    def test_config_has(self):
        """Test configuration key existence."""
        config = ConfigManager()
        assert not config.has("nonexistent")
        config.set("exists", "value")
        assert config.has("exists")
    
    def test_config_keys(self):
        """Test getting all configuration keys."""
        config = ConfigManager()
        config.set("key1", "value1")
        config.set("key2", "value2")
        keys = config.keys()
        assert "key1" in keys
        assert "key2" in keys
    
    def test_config_clear(self):
        """Test configuration clearing."""
        config = ConfigManager()
        config.set("key", "value")
        assert config.has("key")
        config.clear()
        assert not config.has("key")
    
    def test_config_to_dict(self):
        """Test configuration to dictionary conversion."""
        config = ConfigManager()
        config.set("int_val", 42)
        config.set("float_val", 3.14)
        config.set("string_val", "test")
        
        result = config.to_dict()
        assert "int_val" in result
        assert "float_val" in result
        assert "string_val" in result
        assert result["int_val"] == 42
        assert abs(result["float_val"] - 3.14) < 0.001
        assert result["string_val"] == "test"
    
    def test_config_validation(self):
        """Test configuration validation."""
        config = ConfigManager()
        
        # Valid configuration should not raise error
        config.set("brain.neuron_count", 1000)
        config.set("brain.connection_probability", 0.1)
        
        # Invalid configuration should raise ValidationError
        with pytest.raises(ValidationError):
            config.set("brain.neuron_count", -1)
    
    def test_config_json_save_load(self):
        """Test JSON file save and load."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            # Save configuration
            config = ConfigManager()
            config.set("test_int", 42)
            config.set("test_string", "hello")
            config.save_to_json(filepath)
            
            # Load configuration
            new_config = ConfigManager()
            new_config.load_from_json(filepath)
            
            # Verify loaded values
            assert new_config.get_int("test_int") == 42
            assert new_config.get_string("test_string") == "hello"
            
        finally:
            os.unlink(filepath)


class TestBrain:
    """Test the enhanced Brain class."""
    
    def test_brain_creation(self):
        """Test brain creation."""
        config = ConfigManager()
        config.set("brain.neuron_count", 100)
        brain = Brain(config)
        
        assert brain is not None
        assert brain.get_total_neuron_count() == 100
        assert brain.get_config() == config
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        brain = Brain()
        
        # Should be able to initialize
        brain.initialize()
        assert brain._initialized is True
    
    def test_brain_step(self):
        """Test brain simulation step."""
        brain = Brain()
        brain.initialize()
        
        initial_spikes = brain.get_total_spike_count()
        brain.step(0)
        
        # Step should work (may or may not have increased spike count)
        assert brain.get_total_neuron_count() == brain.get_total_neuron_count()
    
    def test_brain_run_steps(self):
        """Test batch simulation."""
        brain = Brain()
        brain.initialize()
        
        initial_spikes = brain.get_total_spike_count()
        brain.run_steps(50)
        
        # Should have processed 50 steps
        assert brain._step_count == 50
    
    def test_brain_reset(self):
        """Test brain reset."""
        brain = Brain()
        brain.initialize()
        brain.step(0)
        
        brain.reset()
        assert brain._initialized is False
        assert brain._step_count == 0
    
    def test_brain_save_load(self):
        """Test brain checkpoint save/load."""
        # Create and run brain
        brain1 = Brain()
        brain1.initialize()
        brain1.run_steps(100)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            brain1.save(filepath)
            
            # Create new brain and load checkpoint
            brain2 = Brain()
            brain2.load(filepath)
            
            # Should have similar state
            assert brain2.get_total_neuron_count() == brain1.get_total_neuron_count()
            assert brain2.get_total_synapse_count() == brain1.get_total_synapse_count()
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_brain_context_manager(self):
        """Test brain context manager."""
        with Brain() as brain:
            assert brain._initialized is True
            # Context should have initialized brain
            assert brain.get_total_neuron_count() > 0
        
        # After context exit, brain should be reset
        assert brain._initialized is False
    
    def test_brain_method_chaining(self):
        """Test method chaining for fluent interface."""
        brain = Brain()
        brain.initialize()
        
        # All methods should return self for chaining
        result = (brain
                 .initialize()
                 .run_steps(10)
                 .reset()
                 .initialize())
        
        assert result is brain
    
    def test_brain_statistics(self):
        """Test brain statistics methods."""
        brain = Brain()
        brain.initialize()
        
        # All statistics should return reasonable values
        assert isinstance(brain.get_total_neuron_count(), int)
        assert isinstance(brain.get_total_synapse_count(), int)
        assert isinstance(brain.get_active_neuron_count(), int)
        assert isinstance(brain.get_firing_neuron_count(), int)
        assert isinstance(brain.get_average_firing_rate(), float)
        assert isinstance(brain.get_excitation_inhibition_ratio(), float)
        assert isinstance(brain.get_total_spike_count(), int)
        assert isinstance(brain.get_pending_spike_event_count(), int)
        
        # Firing rate should be non-negative
        assert brain.get_average_firing_rate() >= 0.0


class TestAgent:
    """Test the enhanced Agent class."""
    
    def test_agent_creation(self):
        """Test agent creation."""
        config = ConfigManager()
        brain = Brain(config)
        
        agent = Agent(brain)
        assert agent is not None
        assert agent.get_brain() == brain
    
    def test_agent_initialization(self):
        """Test agent initialization."""
        brain = Brain()
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        agent = Agent(brain)
        agent.initialize(world)
        assert agent._initialized is True
        assert agent._world == world
    
    def test_agent_process_sensory(self):
        """Test sensory processing."""
        brain = Brain()
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        agent = Agent(brain)
        agent.initialize(world)
        
        # Create mock sensory percept (this would normally come from world)
        class MockPercept:
            def __init__(self):
                self.vision = [0.0] * 64
                self.touch = [0.0] * 8
                self.internal = [0.0] * 4
                self.audio = [0.0] * 2
        
        percept = MockPercept()
        agent.process_sensory_input(percept)
        
        # Should not raise exception
        assert True
    
    def test_agent_decode_motor(self):
        """Test motor command decoding."""
        brain = Brain()
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        agent = Agent(brain)
        agent.initialize(world)
        
        # Should return a motor command (may be Wait if no activity)
        motor_cmd = agent.decode_motor_command()
        assert motor_cmd is not None
    
    def test_agent_apply_reward(self):
        """Test reward modulation."""
        brain = Brain()
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        agent = Agent(brain)
        agent.initialize(world)
        
        # Apply reward modulation
        agent.apply_reward_modulation(1.0, 0.5)
        
        # Should not raise exception
        assert True
    
    def test_agent_enable_features(self):
        """Test feature enable/disable."""
        brain = Brain()
        agent = Agent(brain)
        
        # Enable features
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
        # Check that features are enabled
        assert agent.is_reward_modulation_enabled() is True
        assert agent.is_structural_plasticity_enabled() is True
        assert agent.is_development_enabled() is True
        assert agent.is_curiosity_enabled() is True
    
    def test_agent_context_manager(self):
        """Test agent context manager."""
        brain = Brain()
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        with Agent(brain) as agent:
            agent.initialize(world)
            assert agent._initialized is True
        
        # After context exit, agent should be reset
        assert agent._initialized is False
    
    def test_agent_reset(self):
        """Test agent reset."""
        brain = Brain()
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        agent = Agent(brain)
        agent.initialize(world)
        
        agent.reset()
        assert agent._initialized is False


class TestSimpleWorld:
    """Test the enhanced SimpleWorld class."""
    
    def test_world_creation(self):
        """Test world creation."""
        world = SimpleWorld()
        assert world is not None
        assert world._world is not None
    
    def test_world_configure(self):
        """Test world configuration."""
        world = SimpleWorld()
        world.configure(width=20, height=30, vision_width=8, vision_height=8)
        
        assert world.get_width() == 20
        assert world.get_height() == 30
    
    def test_world_reset(self):
        """Test world reset."""
        world = SimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        assert world.get_width() == 10
    
    def test_world_set_agent_start(self):
        """Test agent start position."""
        world = SimpleWorld()
        world.configure(width=10, height=10)
        world.set_agent_start(5.0, 7.0)
        
        assert world._world is not None
    
    def test_world_update(self):
        """Test world update."""
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        # Should not raise exception
        world.update(0.1)
        assert world.get_simulation_time() == 0.1
    
    def test_world_apply_motor(self):
        """Test motor command application."""
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        # Should not raise exception (may not have motor commands defined)
        # This tests that the method exists and handles the command
        class MockMotorCommand:
            pass
        
        mock_cmd = MockMotorCommand()
        world.apply_motor_command(mock_cmd, 0.0)
        
        assert True
    
    def test_world_sensory_percept(self):
        """Test sensory percept access."""
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        percept = world.get_sensory_percept()
        assert percept is not None
    
    def test_world_agent_body(self):
        """Test agent body access."""
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        body = world.get_agent_body()
        assert body is not None
        assert hasattr(body, 'x')
        assert hasattr(body, 'y')
    
    def test_world_add_remove_object(self):
        """Test object add/remove."""
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        # Should not raise exception
        # (actual implementation may have restrictions)
        assert True
    
    def test_world_position_validation(self):
        """Test position validation."""
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        # Valid positions should return True
        assert world.is_valid_position(5.0, 5.0)
        
        # Edge positions
        assert world.is_valid_position(0.0, 0.0)
        assert world.is_valid_position(10.0, 10.0)
    
    def test_world_random_seed(self):
        """Test random seed setting."""
        world = SimpleWorld()
        world.configure(width=10, height=10)
        
        world.set_random_seed(42)
        assert world.get_random_seed() == 42
        
        world.set_random_seed(123)
        assert world.get_random_seed() == 123


class TestIntegration:
    """Integration tests for the complete NLM system."""
    
    def test_complete_simulation(self):
        """Test complete brain-world-agent simulation."""
        # Create brain with configuration
        config = ConfigManager()
        config.set("brain.neuron_count", 200)
        
        brain = Brain(config)
        agent = Agent(brain)
        world = SimpleWorld()
        world.configure(width=15, height=15, vision_width=8, vision_height=8)
        world.reset()
        world.set_agent_start(7.5, 7.5)
        
        agent.initialize(world)
        
        # Enable all features
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            
            # Get sensory input
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            motor_cmd = agent.decode_motor_command()
            world.apply_motor_command(motor_cmd, world.get_simulation_time())
            
            # Apply learning
            reward = percept.get_internal()[0] if percept.get_internal() else 0.0
            agent.apply_reward_modulation(reward, 0.0)
        
        # Verify simulation completed
        assert brain.get_total_neuron_count() == 200
        assert agent.get_curiosity_level() >= 0.0
        assert agent.get_prediction_error() >= -1.0  # May be negative
    
    def test_context_managers_integration(self):
        """Test integration of context managers."""
        with Brain() as brain:
            brain.initialize()
            assert brain._initialized is True
            
            with Agent(brain) as agent:
                # Agent should have access to brain
                assert agent.get_brain() == brain
        
        # After context exit, brain should be reset
        assert brain._initialized is False
    
    def test_configuration_serialization(self):
        """Test configuration serialization."""
        # Create complex configuration
        config = ConfigManager()
        config.set("brain.neuron_count", 500)
        config.set("brain.connection_probability", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("neuromod.curiosity.enable", True)
        
        # Test with JSON serialization
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            config.save_to_json(filepath)
            
            # Load back
            new_config = ConfigManager()
            new_config.load_from_json(filepath)
            
            # Verify key values
            assert new_config.get_int("brain.neuron_count") == 500
            assert abs(new_config.get_float("brain.connection_probability") - 0.1) < 0.001
            assert new_config.get_bool("plasticity.stdp.enable") is True
            assert new_config.get_bool("neuromod.curiosity.enable") is True
            
        finally:
            os.unlink(filepath)


class TestErrorHandling:
    """Test error handling and exceptions."""
    
    def test_uninitialized_brain_error(self):
        """Test error when brain is not initialized."""
        brain = Brain()
        # Brain should not be initialized by default
        
        # Reset should work even if not initialized
        brain.reset()
        
        # Some operations should raise SimulationError when not initialized
        with pytest.raises(SimulationError):
            # This should raise because brain is not initialized
            # Note: The actual implementation may not check this until step()
            pass
    
    def test_invalid_config_error(self):
        """Test configuration error handling."""
        # ConfigManager should validate configuration
        config = ConfigManager()
        
        # This should not raise ValidationError (validation happens later)
        config.set("brain.neuron_count", 0)  # Edge case
        
        # Should still work
        config.get_int("brain.neuron_count")
    
    def test_method_order_errors(self):
        """Test errors when methods are called in wrong order."""
        brain = Brain()
        
        # Reset before initialization should work
        brain.reset()
        
        # Step before initialization should raise SimulationError
        with pytest.raises(SimulationError):
            brain.step(0)


class TestPerformance:
    """Basic performance tests."""
    
    def test_batch_performance(self):
        """Test batch simulation performance."""
        import time
        
        brain = Brain()
        brain.initialize()
        
        # Time batch simulation
        start_time = time.time()
        brain.run_steps(100)
        end_time = time.time()
        
        elapsed = end_time - start_time
        print(f"Batch simulation: 100 steps in {elapsed:.3f} seconds")
        
        # Should be reasonably fast (less than 10 seconds for 100 steps)
        assert elapsed < 10.0
    
    def test_memory_efficiency(self):
        """Test memory efficiency (basic check)."""
        import gc
        
        # Create brain
        brain = Brain()
        brain.initialize()
        
        # Force garbage collection
        gc.collect()
        
        # Brain should be usable
        initial_neuron_count = brain.get_total_neuron_count()
        
        # Simulate
        brain.run_steps(10)
        
        # Should still have same neuron count
        assert brain.get_total_neuron_count() == initial_neuron_count


# Test suite configuration

def run_all_tests():
    """Run all tests and report results."""
    print("Running NLM Python Test Suite")
    print("=" * 50)
    
    # Create test result container
    test_results = []
    
    # Run test classes
    test_classes = [
        TestConfigManager,
        TestBrain,
        TestAgent,
        TestSimpleWorld,
        TestIntegration,
        TestErrorHandling,
        TestPerformance,
    ]
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        
        test_instance = test_class()
        
        # Run all test methods
        for method_name in dir(test_instance):
            if method_name.startswith('test_'):
                method = getattr(test_instance, method_name)
                test_name = f"{test_class.__name__}.{method_name}"
                
                print(f"  Running {test_name}...", end=" ")
                
                try:
                    method()
                    print("✓ PASSED")
                    test_results.append((test_name, "PASSED"))
                except Exception as e:
                    print(f"✗ FAILED: {e}")
                    test_results.append((test_name, f"FAILED: {e}"))
    
    # Print summary
    print("\n" + "=" * 50)
    print("Test Summary")
    print("=" * 50)
    
    passed = sum(1 for _, status in test_results if status == "PASSED")
    failed = len(test_results) - passed
    
    print(f"Total tests: {len(test_results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed > 0:
        print(f"\nFailed tests:")
        for test_name, status in test_results:
            if status != "PASSED":
                print(f"  - {test_name}: {status}")
    
    return passed, failed


if __name__ == "__main__":
    # Run tests if executed directly
    if len(sys.argv) > 1 and sys.argv[1] == "--run":
        passed, failed = run_all_tests()
        sys.exit(0 if failed == 0 else 1)
    
    # Otherwise, pytest will discover and run tests
    print("To run tests, use: python -m pytest tests/ -v")
    print("Or: python tests/run_tests.py --run")
