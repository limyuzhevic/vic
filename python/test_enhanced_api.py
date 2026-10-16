#!/usr/bin/env python3
"""
Test suite for enhanced NLM Python API
Tests the new high-level convenience functions, context managers,
and advanced features implemented in bindings.cpp
"""

import sys
import os
import tempfile
import json
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_convenience_functions():
    """Test high-level convenience functions"""
    print("Testing basic convenience functions...")
    
    # Test simple brain preset
    brain = pynlm.create_simple_brain("simple")
    assert brain is not None
    print("✓ Created simple brain")
    
    # Test advanced brain preset
    brain2 = pynlm.create_simple_brain("advanced")
    assert brain2 is not None
    print("✓ Created advanced brain")
    
    # Test research brain preset
    brain3 = pynlm.create_simple_brain("research")
    assert brain3 is not None
    print("✓ Created research brain")
    
    # Test version function
    version = pynlm.get_version()
    assert version is not None
    print(f"✓ Version: {version}")
    
    # Test constants
    assert hasattr(pynlm, 'SIMPLE_PRESET')
    assert hasattr(pynlm, 'ADVANCED_PRESET')
    assert hasattr(pynlm, 'RESEARCH_PRESET')
    print("✓ Presets constants available")

def test_context_managers():
    """Test context managers for resource management"""
    print("\nTesting context managers...")
    
    # Test ResourceManager context manager
    with pynlm.ResourceManager() as rm:
        config = rm.get_config()
        brain = rm.get_brain()
        world = rm.get_world()
        agent = rm.get_agent()
        
        assert config is not None
        assert brain is not None
        assert world is not None
        assert agent is not None
        
        # Test that brain was initialized
        assert brain.getTotalNeuronCount() > 0
        print("✓ Context manager resource initialization works")
    
    print("✓ Context manager cleanup completed")

def test_simulation_control():
    """Test simulation control and monitoring"""
    print("\nTesting simulation control...")
    
    # Create simulation components
    brain = pynlm.create_simple_brain("simple")
    world = pynlm.create_simple_world()
    agent = pynlm.create_agent_brain(brain)
    
    # Configure world
    world.configure(20, 20, 16, 16)
    world.setAgentStart(10, 10)
    
    # Run simulation with progress callback
    step_count = 0
    def progress_callback(step, brain_obj):
        nonlocal step_count
        step_count = step + 1
    
    def stop_condition():
        return step_count > 50
    
    rewards = pynlm.run_simulation(
        brain, world, agent, 100,
        progress_callback=progress_callback,
        stop_condition=stop_condition
    )
    
    assert len(rewards) > 0
    assert step_count <= 50
    print(f"✓ Simulation completed with {len(rewards)} steps, {step_count} callbacks")

def test_model_analysis():
    """Test model analysis and visualization helpers"""
    print("\nTesting model analysis...")
    
    brain = pynlm.create_simple_brain("simple")
    
    # Test metrics
    metrics = pynlm.Metrics()
    metrics.set("test_key", 42.5)
    assert metrics.get("test_key") == 42.5
    
    # Test connectivity matrix (placeholder)
    connectivity = pynlm.ModelAnalyzer().generate_connectivity_matrix(brain)
    assert isinstance(connectivity, dict)
    print("✓ Connectivity matrix generated")
    
    # Test activity patterns analysis
    activity_patterns = pynlm.ModelAnalyzer().analyze_activity_patterns(brain)
    assert isinstance(activity_patterns, dict)
    print("✓ Activity patterns analyzed")
    
    # Test connectivity statistics
    stats = pynlm.ModelAnalyzer().compute_connectivity_stats(brain)
    assert isinstance(stats, dict)
    print("✓ Connectivity statistics computed")

def test_agent_development_tools():
    """Test agent development tools and training wrappers"""
    print("\nTesting agent development tools...")
    
    brain = pynlm.create_simple_brain("simple")
    world = pynlm.create_simple_world()
    agent = pynlm.create_agent_brain(brain)
    
    # Configure world
    world.configure(20, 20, 16, 16)
    
    # Test policy evaluation
    episode_rewards = pynlm.evaluate_policy(
        [brain], [world], [agent], 100
    )
    
    assert len(episode_rewards) == 1
    assert episode_rewards[0] > 0  # Should have accumulated rewards
    print(f"✓ Policy evaluated with reward: {episode_rewards[0]}")

def test_experiment_support():
    """Test research and experiment support utilities"""
    print("\nTesting experiment support...")
    
    # Create experiment setup
    experiment = pynlm.ExperimentResult("test_experiment")
    
    # Add components
    brain = pynlm.create_simple_brain("simple")
    world = pynlm.create_simple_world()
    agent = pynlm.create_agent_brain(brain)
    
    experiment.brains.append(brain)
    experiment.worlds.append(world)
    experiment.agents.append(agent)
    
    assert experiment.name == "test_experiment"
    assert len(experiment.brains) == 1
    assert len(experiment.worlds) == 1
    assert len(experiment.agents) == 1
    print("✓ Experiment setup created")
    
    # Test experiment manager
    manager = pynlm.ExperimentManager()
    manager.addResult(experiment)
    assert len(manager.getResults()) == 1
    print("✓ Experiment manager working")

def test_data_export():
    """Test data export functionality"""
    print("\nTesting data export...")
    
    # Create a temporary file for testing
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        filepath = f.name
    
    try:
        # Export experiment results
        experiment = pynlm.ExperimentResult("export_test")
        experiment.rewards = [1.0, 2.5, 3.7]
        
        manager = pynlm.ExperimentManager()
        manager.addResult(experiment)
        
        result = manager.exportResults(filepath, "json")
        assert "Results exported to:" in result
        
        # Verify file was created and has content
        with open(filepath, 'r') as f:
            content = f.read()
            assert len(content) > 0
            assert "test_experiment" in content
        
        print("✓ Data export successful")
    finally:
        # Clean up
        if os.path.exists(filepath):
            os.unlink(filepath)

def test_backward_compatibility():
    """Test backward compatibility with original API"""
    print("\nTesting backward compatibility...")
    
    # Original API should still work
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("✓ createDefaultConfig() works")
    
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("✓ createBrain() works")
    
    world = pynlm.createSimpleWorld()
    assert world is not None
    print("✓ createSimpleWorld() works")
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    print("✓ createAgentBrain() works")

def test_benchmarking():
    """Test benchmarking tools"""
    print("\nTesting benchmarking...")
    
    # Test simulation benchmark
    brain = pynlm.create_simple_brain("simple")
    world = pynlm.create_simple_world()
    agent = pynlm.create_agent_brain(brain)
    
    world.configure(20, 20, 16, 16)
    
    result = pynlm.benchmark_simulation(
        brain, world, agent, 100, 1
    )
    
    assert len(result) == 3
    print("✓ Simulation benchmark completed")
    
    # Test general benchmark
    def setup():
        pass
    
    def operation():
        pass
    
    def cleanup():
        pass
    
    bench_result = pynlm.benchmark_brain(
        "test_bench", setup, operation, cleanup, 10
    )
    
    assert len(bench_result) == 3
    print("✓ General benchmark completed")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("NLM Enhanced Python API Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_convenience_functions,
        test_context_managers,
        test_simulation_control,
        test_model_analysis,
        test_agent_development_tools,
        test_experiment_support,
        test_data_export,
        test_backward_compatibility,
        test_benchmarking,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed: {e}")
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)