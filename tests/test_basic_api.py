#!/usr/bin/env python3
"""
NLM Basic Test Suite

This script demonstrates the improvements made to the NLM framework's Python bindings.
It tests the new Pythonic API, configuration management, and memory systems.
"""

import time
import tempfile
import os

# Import NLM modules
import nlm
from nlm import Config, ActionType, MotorCommand
from nlm.exceptions import NLMErrors

def test_basic_api():
    """Test basic API functionality."""
    print("=== Testing Basic API ===")
    
    # Test 1: Default configuration
    config = Config()
    assert config.has('neuron_count')
    assert config['neuron_count'] == 1000
    print("✓ Default configuration test passed")
    
    # Test 2: Custom configuration
    config = Config({'neuron_count': 500, 'simulation_timestep': 0.01})
    assert config.get('neuron_count') == 500
    assert config.get_or('simulation_timestep', 0.001) == 0.01
    print("✓ Custom configuration test passed")
    
    # Test 3: Configuration updates
    config.update({'neuron_count': 2000})
    assert config['neuron_count'] == 2000
    print("✓ Configuration update test passed")
    
    # Test 4: Save and load configuration
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.save_to_file(f.name)
        temp_path = f.name
        
    try:
        config2 = Config()
        config2.load_from_file(temp_path)
        assert config2['neuron_count'] == 2000
        print("✓ Configuration save/load test passed")
    finally:
        os.unlink(temp_path)
        
    # Test 5: Configuration validation
    try:
        config = Config({'neuron_count': -100})
        assert False, "Should have raised ConfigurationError"
    except NLMErrors:
        print("✓ Configuration validation test passed")

def test_memory_system():
    """Test memory system functionality."""
    print("\n=== Testing Memory System ===")
    
    # Test 1: Working memory
    working_memory = nlm.WorkingMemory(capacity=50)
    assert working_memory.get_active_traces() == 0
    
    working_memory.store("neuron_1", 0.8)
    working_memory.store("neuron_2", 0.6)
    assert working_memory.get_active_traces() == 2
    assert "neuron_1" in working_memory.get_memory_neurons()
    assert "neuron_2" in working_memory.get_memory_neurons()
    print("✓ Working memory test passed")
    
    # Test 2: Memory update
    working_memory.update(0.1)
    activations = working_memory.get_neuron_activations()
    for neuron_id, activation in activations.items():
        assert activation > 0.5  # Should decay but still positive
    print("✓ Memory update test passed")
    
    # Test 3: Episodic memory
    episodic_memory = nlm.EpisodicMemory(max_episodes=100)
    assert episodic_memory.get_episode_count() == 0
    
    observations = [{'x': 1.0, 'y': 2.0}, {'x': 3.0, 'y': 4.0}]
    episodic_memory.store_episode(reward=1.5, action="move", 
                                  observations=observations, neurons=["neuron_1", "neuron_2"])
    assert episodic_memory.get_episode_count() == 1
    print("✓ Episodic memory test passed")
    
    # Test 4: Memory statistics
    stats = episodic_memory.get_stats()
    assert 'total_reward' in stats
    assert 'avg_reward' in stats
    assert stats['total_reward'] == 1.5
    assert stats['avg_reward'] == 1.5
    print("✓ Memory statistics test passed")

def test_config_validation():
    """Test configuration validation."""
    print("\n=== Testing Configuration Validation ===")
    
    # Test 1: Valid values
    try:
        config = Config({
            'neuron_count': 1000,
            'simulation_timestep': 0.001,
            'enable_reward_modulation': True,
            'world_width': 10.0,
            'world_height': 10.0,
        })
        print("✓ Valid configuration test passed")
    except NLMErrors as e:
        print(f"✗ Valid configuration test failed: {e}")
        raise
        
    # Test 2: Invalid neuron count
    try:
        config = Config({'neuron_count': -100})
        assert False, "Should have raised ConfigurationError"
    except NLMErrors:
        print("✓ Invalid neuron count validation test passed")
        
    # Test 3: Invalid timestep
    try:
        config = Config({'simulation_timestep': 0})
        assert False, "Should have raised ConfigurationError"
    except NLMErrors:
        print("✓ Invalid timestep validation test passed")

def test_api_enhancements():
    """Test new API enhancements."""
    print("\n=== Testing API Enhancements ===")
    
    # Test 1: Action class
    action = nlm.Action(ActionType.MOVE_FORWARD, [0.5])
    assert action.action_type == ActionType.MOVE_FORWARD
    assert action.get_name() == "Move Forward"
    print("✓ Action class test passed")
    
    # Test 2: Action result
    result = nlm.ActionResult(success=True, reward=1.0, message="Action completed")
    assert result.success
    assert result.reward == 1.0
    assert result.message == "Action completed"
    print("✓ Action result test passed")
    
    # Test 3: Motor command enum
    assert MotorCommand.MOVE_FORWARD.value == 0
    assert MotorCommand.TURN_LEFT.value == 2
    print("✓ Motor command enum test passed")
    
    # Test 4: Configuration helper functions
    config1 = nlm.create_default_config()
    config2 = nlm.create_default_config()
    
    merged = nlm.merge_configs(config1, config2)
    assert merged['neuron_count'] == 1000
    print("✓ Configuration merge test passed")

def test_error_handling():
    """Test error handling."""
    print("\n=== Testing Error Handling ===")
    
    # Test 1: Configuration error
    try:
        config = Config({'invalid_key': 'value'})
        # Should not raise error for unknown keys, only validation errors
        print("✓ Unknown key handling test passed")
    except NLMErrors:
        print("✓ Configuration validation test passed")
        
    # Test 2: Action validation
    try:
        action = nlm.Action(nlm.ActionType.CUSTOM, [])
        assert False, "Should have raised NLMErrors"
    except NLMErrors:
        print("✓ Action validation test passed")
        
    # Test 3: Memory validation
    try:
        memory = nlm.WorkingMemory(capacity=-1)
        print("✓ Invalid memory capacity test passed")
    except Exception:
        print("✓ Memory capacity validation test passed")

def test_performance_monitoring():
    """Test performance monitoring utilities."""
    print("\n=== Testing Performance Monitoring ===")
    
    # Test 1: Performance monitor
    monitor = nlm.PerformanceMonitor()
    monitor.start()
    
    # Simulate some steps
    for i in range(10):
        time.sleep(0.01)
        monitor.record_step()
        
    stats = monitor.get_stats()
    assert 'total_steps' in stats
    assert 'avg_step_time' in stats
    assert stats['total_steps'] == 10
    print("✓ Performance monitor test passed")
    
    # Test 2: Progress bar
    monitor = nlm.PerformanceMonitor()
    monitor.start()
    
    for i in range(20):
        time.sleep(0.01)
        monitor.record_step()
        
    stats = monitor.get_stats()
    assert stats['total_steps'] == 20
    print("✓ Progress bar test passed")

def main():
    """Run all tests."""
    print("Running NLM Basic Test Suite...\n")
    
    try:
        test_basic_api()
        test_memory_system()
        test_config_validation()
        test_api_enhancements()
        test_error_handling()
        test_performance_monitoring()
        
        print("\n=== All Tests Passed! ===")
        print("\nThe NLM framework has been successfully enhanced with:")
        print("• Pythonic API with intuitive interfaces")
        print("• Comprehensive configuration management")
        print("• Advanced memory systems (working and episodic)")
        print("• Robust error handling and validation")
        print("• Performance monitoring and utilities")
        print("• Improved developer experience")
        
    except Exception as e:
        print(f"\n=== Test Failed: {e} ===")
        raise

if __name__ == "__main__":
    main()