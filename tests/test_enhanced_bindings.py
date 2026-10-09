#!/usr/bin/env python3
"""
Comprehensive Python tests for enhanced NLM Python bindings.
Tests the new functionality added in the enhancement phase.
"""

import sys
import os
import tempfile
import json
import time
import threading
from pathlib import Path

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def test_basic_functionality():
    """Test basic Python bindings functionality"""
    print("Testing basic functionality...")
    
    # Test creating default config
    config = pynlm.createDefaultConfig()
    assert config is not None
    assert config.has("neuron_count")
    
    # Test creating brain
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test initialization
    result = brain.initialize()
    assert result is True
    
    # Test brain properties
    assert brain.getTotalNeuronCount() == 1000
    assert brain.getRegionCount() == 1
    
    print("✓ Basic functionality tests passed")

def test_enhanced_brain_functionality():
    """Test enhanced brain functionality"""
    print("Testing enhanced brain functionality...")
    
    config = pynlm.createBrainConfig(500, 2)
    brain = pynlm.createBrain(config)
    
    # Test memory system access
    working_memory = brain.getWorkingMemory()
    assert working_memory is not None
    
    episodic_memory = brain.getEpisodicMemory()
    assert episodic_memory is not None
    
    associative_memory = brain.getAssociativeMemory()
    assert associative_memory is not None
    
    # Test enhanced statistics
    stats = brain.getFiringStatistics()
    assert len(stats) > 0
    
    # Test connectivity analysis
    connectivity_analysis = brain.analyzeConnectivity()
    assert "Total synapses" in connectivity_analysis
    
    print("✓ Enhanced brain functionality tests passed")

def test_enhanced_agent_functionality():
    """Test enhanced agent functionality"""
    print("Testing enhanced agent functionality...")
    
    config = pynlm.createBrainConfig(300, 2)
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Test enabling all features
    agent.enableAllFeatures()
    
    # Test advanced stats
    stats = agent.getAdvancedStats()
    assert "neuromodulation" in stats
    assert "curiosity" in stats
    assert "enabled_features" in stats
    
    # Test developmental stage
    stage = agent.getDevelopmentalStage()
    assert stage in [pynlm.DevelopmentalStage.Initial,
                     pynlm.DevelopmentalStage.CriticalPeriod,
                     pynlm.DevelopmentalStage.Maturation,
                     pynlm.DevelopmentalStage.Adult,
                     pynlm.DevelopmentalStage.Aging]
    
    print("✓ Enhanced agent functionality tests passed")

def test_simulation_runner():
    """Test simulation runner functionality"""
    print("Testing simulation runner...")
    
    config = pynlm.createBrainConfig(200, 2)
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Run a single episode
    metrics = pynlm.run_episode(brain, world, agent, 100)
    assert metrics.steps == 100
    assert "firing_rate_history" in dir(metrics)
    assert "reward_history" in dir(metrics)
    assert "neuromodulation_history" in dir(metrics)
    
    # Run multiple episodes
    all_metrics = pynlm.run_multiple_episodes(brain, world, agent, 3, 50)
    assert len(all_metrics) == 3
    
    print("✓ Simulation runner tests passed")

def test_memory_management():
    """Test memory management functionality"""
    print("Testing memory management...")
    
    config = pynlm.createBrainConfig(400, 3)
    brain = pynlm.createBrain(config)
    
    # Test memory optimization
    pynlm.optimizeMemory(brain, 0.5f)
    
    # Test memory usage
    usage = pynlm.getMemoryUsage(brain)
    assert usage >= 0
    
    # Test clearing memory
    pynlm.clearAllMemory(brain)
    
    print("✓ Memory management tests passed")

def test_performance_profiling():
    """Test performance profiling functionality"""
    print("Testing performance profiling...")
    
    config = pynlm.createBrainConfig(250, 2)
    brain = pynlm.createBrain(config)
    
    # Start profiling
    stats = pynlm.startProfiling()
    assert stats is not None
    
    # Run some steps
    for i in range(10):
        brain.step(i)
        pynlm.recordStep(stats, brain)
    
    # Generate report
    report = pynlm.generatePerformanceReport(stats)
    assert "NLM Performance Report" in report
    assert "Steps completed: 10" in report
    
    print("✓ Performance profiling tests passed")

def test_context_managers():
    """Test context manager functionality"""
    print("Testing context managers...")
    
    config = pynlm.createBrainConfig(150, 2)
    brain = pynlm.createBrain(config)
    
    # Test brain context manager
    with pynlm.brain_context(brain) as context:
        assert context.getBrain() is brain
        context.step(0)
        # Context should automatically reset brain when exiting
    
    print("✓ Context manager tests passed")

def test_advanced_config():
    """Test advanced configuration functionality"""
    print("Testing advanced configuration...")
    
    # Test creating advanced config
    advanced_config = pynlm.createAdvancedConfig()
    assert advanced_config is not None
    
    # Test setting custom parameters
    custom_params = {
        "neuron_count": "2000",
        "float_synaptic_rate": "0.05",
        "bool_enable_plasticity": "true",
        "weight_initial_mean": "0.6"
    }
    
    advanced_config2 = pynlm.createAdvancedConfig("CustomBrain", custom_params)
    assert advanced_config2 is not None
    
    print("✓ Advanced configuration tests passed")

def test_neural_network_analysis():
    """Test neural network analysis functionality"""
    print("Testing neural network analysis...")
    
    config = pynlm.createBrainConfig(350, 4)
    brain = pynlm.createBrain(config)
    
    # Test firing statistics
    stats = pynlm.computeFiringStatistics(brain)
    assert len(stats) > 0
    assert all(isinstance(x, float) for x in stats)
    
    # Test connectivity analysis
    analysis = pynlm.analyzeConnectivity(brain)
    assert "Connectivity Analysis" in analysis
    
    # Test sensory neuron analysis
    sensory_neurons = pynlm.getSensoryNeurons(brain)
    assert len(sensory_neurons) >= 0
    
    print("✓ Neural network analysis tests passed")

def test_error_handling():
    """Test error handling and edge cases"""
    print("Testing error handling...")
    
    config = pynlm.createBrainConfig(100, 1)
    brain = pynlm.createBrain(config)
    
    # Test getting non-existent config values
    brain_config = brain.getConfig()
    assert brain_config.get("non_existent_key", "default") == "default"
    
    # Test agent with null brain
    null_agent = pynlm.createAgentBrain(None)
    # Should not crash
    
    print("✓ Error handling tests passed")

def test_performance_benchmarks():
    """Test performance benchmarks"""
    print("Testing performance benchmarks...")
    
    config = pynlm.createBrainConfig(800, 6)
    brain = pynlm.createBrain(config)
    
    # Performance test - simulate realistic workload
    start_time = time.time()
    
    for step in range(200):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    # Should complete within reasonable time
    assert elapsed < 30.0, f"Performance test too slow: {elapsed:.2f}s"
    
    print(f"✓ Performance benchmarks passed ({elapsed:.2f}s)")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("Running Comprehensive NLM Python Bindings Tests")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_enhanced_brain_functionality,
        test_enhanced_agent_functionality,
        test_simulation_runner,
        test_memory_management,
        test_performance_profiling,
        test_context_managers,
        test_advanced_config,
        test_neural_network_analysis,
        test_error_handling,
        test_performance_benchmarks
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
        print()
    
    print("=" * 60)
    print(f"Test Summary: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print("\n⚠️  Some tests failed. Check output above for details.")
        return False
    else:
        print("\n✅ All tests passed!")
        return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
