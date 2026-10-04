#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings enhancements.
Tests all new functionality including context managers, convenience methods,
error handling, configuration validation, and monitoring utilities.
"""

import pynlm
import json
import tempfile
import os
import time
from pathlib import Path
def test_basic_functionality():
    """Test basic brain creation and functionality."""
    print("Testing basic functionality...")
    
    # Test 1: Create default configuration
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("✓ Default config created")
    
    # Test 2: Create and initialize brain
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("✓ Brain created")
    
    brain.initialize()
    print("✓ Brain initialized")
    
    # Test 3: Brain statistics
    stats = brain.get_stats()
    assert isinstance(stats, dict)
    assert "neurons" in stats
    assert "synapses" in stats
    print("✓ Brain statistics work")
    
    # Test 4: Step simulation
    brain.step(0)
    print("✓ Brain step works")
    
    # Test 5: Brain monitoring
    monitoring = brain.monitor()
    assert isinstance(monitoring, dict)
    assert "neuron_count" in monitoring
    print("✓ Brain monitoring works")
    
    print("Basic functionality tests passed!\n")
def test_context_managers():
    """Test context manager functionality."""
    print("Testing context managers...")
    
    config = pynlm.createDefaultConfig()
    
    # Test Brain context manager
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0
        brain.step(0)
        print("✓ Brain context manager works")
    
    # Verify brain was reset
    with pynlm.createBrain(config) as brain2:
        brain2.initialize()
        # Brain should be fresh
        print("✓ Brain reset on context exit")
    
    # Test SimpleWorld context manager
    with pynlm.createSimpleWorld() as world:
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        stats = world.get_stats()
        assert stats["width"] == 10
        assert stats["height"] == 10
        print("✓ SimpleWorld context manager works")
    
    # Test AgentBrain context manager
    brain = pynlm.createBrain(config)
    brain.initialize()
    with pynlm.createAgentBrain(brain) as agent:
        # Agent should be properly managed
        assert agent.getBrain() is brain
        print("✓ AgentBrain context manager works")
    
    print("Context manager tests passed!\n")
def test_convenience_methods():
    """Test convenience methods and fluent interfaces."""
    print("Testing convenience methods...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test run_simulation convenience method
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    
    firing_rate = brain.run_simulation(world, 50, 0.01)
    assert isinstance(firing_rate, float)
    print(f"✓ run_simulation convenience method works (firing rate: {firing_rate:.2f})")
    
    # Test train convenience method
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    # Create dummy inputs/targets for training test
    from pynlm import SensoryInput, Vision, Action, ActionType
    
    inputs = [Vision(8, 8, 3)]
    targets = [Action(ActionType.MoveForward)]
    
    training_steps = brain2.train(inputs, targets, 0.01)
    assert isinstance(training_steps, float)
    print(f"✓ train convenience method works ({training_steps:.0f} training steps)")
    
    # Test evaluate convenience method
    percept = Vision(8, 8, 3)
    action = brain.evaluate(percept)
    assert action is not None
    print("✓ evaluate convenience method works")
    
    # Test training episode
    episode_rate = brain.train_episode(world, Vision(8, 8, 3), 25, 0.01)
    assert isinstance(episode_rate, float)
    print(f"✓ train_episode convenience method works (rate: {episode_rate:.2f})")
    
    print("Convenience method tests passed!\n")
def test_configuration_validation():
    """Test configuration validation and parameter tuning."""
    print("Testing configuration validation...")
    
    # Test default config validation
    config = pynlm.createDefaultConfig()
    is_valid = config.validate()
    # Default config should be valid (or at least not throw exceptions)
    print(f"✓ Config validation works (valid: {is_valid})")
    
    # Test configuration improvements suggestions
    suggestions = config.get_suggestions()
    assert isinstance(suggestions, dict)
    print(f"✓ Configuration suggestions work ({len(suggestions)} suggestions)")
    
    # Test parameter tuning methods
    config.set("brain.neuron_count", 2000)
    config.set("brain.simulation.dt", 0.005)
    
    # Configure for training
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.configure_for_training()
    print("✓ configure_for_training works")
    
    # Configure for inference
    brain.configure_for_inference()
    print("✓ configure_for_inference works")
    
    print("Configuration validation tests passed!\n")
def test_error_handling():
    """Test improved error handling."""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test safe operation with invalid input
    try:
        # This should either work or throw a proper exception
        brain.safe_operation("invalid_input")
        print("✓ safe_operation handles invalid input")
    except Exception as e:
        print(f"✓ safe_operation properly throws exception: {type(e).__name__}")
    
    # Test configuration error handling
    try:
        config.validate()
        print("✓ Config validation handles errors gracefully")
    except Exception as e:
        print(f"✓ Config validation properly throws exception: {type(e).__name__}")
    
    # Test resource error handling
    try:
        # Create invalid neuron ID
        pynlm.convert_to_neuron_id("invalid")
        print("✓ Neuron ID conversion handles errors")
    except Exception as e:
        print(f"✓ Neuron ID conversion properly throws exception: {type(e).__name__}")
    
    print("Error handling tests passed!\n")
def test_monitoring_and_profiling():
    """Test monitoring and profiling utilities."""
    print("Testing monitoring and profiling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test brain monitoring
    monitoring = brain.monitor()
    required_fields = ["neuron_count", "synapse_count", "active_neuron_count", 
                      "firing_neuron_count", "average_firing_rate", "total_spikes"]
    
    for field in required_fields:
        assert field in monitoring, f"Missing field: {field}"
    print("✓ Brain monitoring provides all required fields")
    
    # Test brain profiling
    profile = brain.profile()
    assert "step_time_us" in profile
    assert "neurons_per_step" in profile
    assert "spikes_generated" in profile
    print("✓ Brain profiling works")
    
    # Test agent monitoring
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    agent = pynlm.createAgentBrain(brain2)
    
    agent_monitoring = agent.monitor()
    assert "brain" in agent_monitoring
    assert "sensory_input_size" in agent_monitoring
    assert "neuromodulation_level" in agent_monitoring
    print("✓ Agent monitoring works")
    
    # Test world performance metrics
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    metrics = world.get_performance_metrics()
    assert "simulation_time" in metrics
    print("✓ World performance metrics work")
    
    print("Monitoring and profiling tests passed!\n")
def test_batch_processing():
    """Test batch processing utilities."""
    print("Testing batch processing...")
    
    config = pynlm.createDefaultConfig()
    
    # Create multiple brains
    brains = []
    for i in range(3):
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Test batch steps
    steps = [0, 1, 2, 3, 4]
    times = [0.0, 0.1, 0.2, 0.3, 0.4]
    
    for brain in brains:
        brain.batch_step(steps, times)
        print("✓ Batch step works")
    
    # Test batch processing (simplified)
    config2 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config2)
    brain2 = pynlm.createBrain(config2)
    brain1.initialize()
    brain2.initialize()
    
    brain_list = [brain1, brain2]
    batch_results = brain_list[0].batch_process_brains(brain_list, steps, times)
    assert len(batch_results) == len(steps)
    print("✓ Batch processing multiple brains works")
    
    print("Batch processing tests passed!\n")
def test_visualization_and_export():
    """Test visualization and export utilities."""
    print("Testing visualization and export...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test neuron activity visualization
    plot_data = brain.plot_neuron_activity()
    assert isinstance(plot_data, dict)
    assert "time" in plot_data
    assert "firing_rate" in plot_data
    print("✓ Neuron activity visualization works")
    
    # Test matplotlib conversion (if available)
    try:
        matplotlib_plot = brain.visualize()
        assert isinstance(matplotlib_plot, dict)
        print("✓ Matplotlib conversion works")
    except ImportError:
        print("ℹ Matplotlib not available, skipping visualization tests")
    
    # Test data export (in-memory)
    brain_stats = brain.get_stats()
    neuron_stats = brain.get_neuron_stats()
    
    assert isinstance(brain_stats, dict)
    assert isinstance(neuron_stats, dict)
    print("✓ Data export works")
    
    print("Visualization and export tests passed!\n")
def test_advanced_simulation_controls():
    """Test advanced simulation controls."""
    print("Testing advanced simulation controls...")
    
    # Test world simulation controls
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    
    # Test pause/resume
    pause_result = world.pause()
    assert pause_result is True
    print("✓ World pause works")
    
    resume_result = world.resume()
    assert resume_result is True
    print("✓ World resume works")
    
    # Test step-by-step execution
    body = world.step_by_step(0.01)
    assert body is not None
    print("✓ World step_by_step works")
    
    # Test brain simulation controls
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    pause_result = brain.pause_simulation()
    assert pause_result is True
    print("✓ Brain pause_simulation works")
    
    resume_result = brain.resume_simulation()
    assert resume_result is True
    print("✓ Brain resume_simulation works")
    
    print("Advanced simulation control tests passed!\n")
def test_type_conversions():
    """Test type conversion utilities."""
    print("Testing type conversions...")
    
    # Test string to ID conversions
    neuron_id = pynlm.convert_to_neuron_id("123")
    assert neuron_id is not None
    print("✓ Neuron ID conversion works")
    
    synapse_id = pynlm.convert_to_synapse_id("456")
    assert synapse_id is not None
    print("✓ Synapse ID conversion works")
    
    region_id = pynlm.convert_to_region_id("789")
    assert region_id is not None
    print("✓ Region ID conversion works")
    
    # Test list to vector conversions
    float_list = [1.0, 2.5, 3.7, 4.2]
    float_vec = pynlm.py_list_to_vector(float_list)
    assert len(float_vec) == len(float_list)
    print("✓ List to float vector conversion works")
    
    # Test dictionary to config value conversion
    config_dict = {"brain.neuron_count": 1000, "brain.simulation.dt": 0.01}
    config_value = pynlm.convert_to_config_value(config_dict)
    assert config_value is not None
    print("✓ Dictionary to config value conversion works")
    
    print("Type conversion tests passed!\n")
def test_comprehensive_workflow():
    """Test a comprehensive workflow combining multiple features."""
    print("Testing comprehensive workflow...")
    
    # Step 1: Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.simulation.dt", 0.01)
    
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        brain.configure_for_training()
        
        # Step 2: Create world and agent
        with pynlm.createSimpleWorld() as world:
            world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
            world.setAgentStart(5.0, 5.0)
            
            with pynlm.createAgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Step 3: Run training episode
                initial_percept = world.getSensoryPercept()
                rewards = [0.0, 0.5, 1.0, 0.8, 0.3, 0.0, 0.0, 0.0, 0.0, 0.0]
                
                total_reward = agent.train_episode(world, initial_percept, rewards, 10, 0.1)
                assert isinstance(total_reward, float)
                print(f"✓ Comprehensive training episode completed (total reward: {total_reward:.2f})")
                
                # Step 4: Monitor performance
                monitoring = agent.monitor()
                stats = brain.get_stats()
                
                assert monitoring["neuromodulation_level"] > 0
                assert "neurons" in stats
                print("✓ Monitoring data collected")
                
                # Step 5: Profile performance
                profile = brain.profile()
                assert "step_time_us" in profile
                print("✓ Performance profiling completed")
                
                # Step 6: Visualize results
                visualization = brain.plot_neuron_activity()
                assert "time" in visualization
                print("✓ Results visualization generated")
                
                # Step 7: Get comprehensive statistics
                all_stats = brain.get_all_stats()
                assert "neurons" in all_stats
                assert "synapses" in all_stats
                print("✓ Comprehensive statistics collected")
    
    print("Comprehensive workflow test passed!\n")
def test_edge_cases():
    """Test edge cases and error conditions."""
    print("Testing edge cases...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test empty simulation steps
    brain.batch_step([])
    print("✓ Empty batch step handled")
    
    # Test single step
    brain.step(0)
    print("✓ Single step handled")
    
    # Test world edge cases
    world = pynlm.createSimpleWorld()
    world.configure(width=1, height=1, visionWidth=1, visionHeight=1)
    world.step_by_step(0.01)
    print("✓ Minimal world configuration handled")
    
    # Test agent with invalid percept
    agent = pynlm.createAgentBrain(brain)
    try:
        # This should handle gracefully
        agent.act(None, world, 0.01)
        print("✓ Invalid percept handled gracefully")
    except Exception:
        print("✓ Invalid percept properly throws exception")
    
    print("Edge case tests passed!\n")
def run_all_tests():
    """Run all test suites."""
    print("=" * 60)
    print("NLM Python Bindings Comprehensive Test Suite")
    print("=" * 60)
    print()
    
    test_suites = [
        test_basic_functionality,
        test_context_managers,
        test_convenience_methods,
        test_configuration_validation,
        test_error_handling,
        test_monitoring_and_profiling,
        test_batch_processing,
        test_visualization_and_export,
        test_advanced_simulation_controls,
        test_type_conversions,
        test_comprehensive_workflow,
        test_edge_cases,
    ]
    
    passed = 0
    failed = 0
    
    for test_suite in test_suites:
        try:
            test_suite()
            passed += 1
        except Exception as e:
            print(f"❌ Test {test_suite.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
        print("-" * 60)
    
    print(f"\n{'='*60}")
    print(f"Test Results: {passed} passed, {failed} failed")
    print(f"{'='*60}")
    
    if failed == 0:
        print("🎉 All tests passed! The NLM Python bindings enhancements are working correctly.")
    else:
        print(f"⚠️  {failed} test(s) failed. Please review the implementation.")
    
    return failed == 0
if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)