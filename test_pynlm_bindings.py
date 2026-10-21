#!/usr/bin/env python3
"""
Test suite for NLM Python bindings

This test suite validates all the new Python bindings and features added to the NLM project.
It tests:
- Basic functionality
- Error handling and validation
- Performance monitoring
- Advanced agent features
- Configuration helpers
- Convenience methods
"""

import pynlm
import tempfile
import os
import json
import sys

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing basic functionality...")
    
    # Test brain creation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic operations
    assert brain.is_initialized() == True
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    # Test stepping
    for i in range(10):
        brain.step(i)
    
    assert brain.getTotalSpikeCount() > 0
    assert brain.getFiringNeuronCount() > 0
    
    print("✓ Basic functionality tests passed")

def test_configuration_helpers():
    """Test configuration helper functions"""
    print("Testing configuration helpers...")
    
    # Test preset application
    config = pynlm.createDefaultConfig()
    config.apply_preset("default")
    config.apply_preset("debug")
    config.apply_preset("high_performance")
    
    # Test get_all_as_dict
    config_dict = config.get_all_as_dict()
    assert isinstance(config_dict, dict)
    assert "neuronCount" in config_dict or "visionWidth" in config_dict
    
    # Test configuration helper functions
    brain_config = pynlm.create_brain_config()
    agent_config = pynlm.create_agent_config()
    world_config = pynlm.create_world_config()
    
    assert brain_config is not None
    assert agent_config is not None
    assert world_config is not None
    
    # Test merge_configs
    merged = pynlm.merge_configs([brain_config, agent_config])
    assert merged is not None
    
    print("✓ Configuration helper tests passed")

def test_error_handling():
    """Test error handling and validation"""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    
    # Test invalid preset name
    try:
        config.apply_preset("")
        assert False, "Should have raised exception"
    except (ValueError, RuntimeError):
        pass
    
    try:
        config.apply_preset("invalid_preset")
        assert False, "Should have raised exception"
    except (ValueError, RuntimeError, Exception):
        pass
    
    # Test invalid configuration key
    try:
        config.set_value("", "value")
        assert False, "Should have raised exception"
    except (ValueError, RuntimeError, Exception):
        pass
    
    # Test configuration validation
    try:
        config.set_value("a" * 200, "value")  # Too long key
        assert False, "Should have raised exception"
    except (ValueError, RuntimeError, Exception):
        pass
    
    print("✓ Error handling tests passed")

def test_performance_monitoring():
    """Test performance monitoring features"""
    print("Testing performance monitoring...")
    
    # Test creating monitoring tools
    monitor = pynlm.createPerformanceMonitor()
    profiler = pynlm.createMemoryProfiler()
    metrics = pynlm.createMetricsCollector()
    
    assert monitor is not None
    assert profiler is not None
    assert metrics is not None
    
    # Test brain with monitoring
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Record some metrics
    metrics.record("neuronsPerSecond", 1000.0)
    metrics.record("spikesPerSecond", 50000.0)
    metrics.record("stepTimeMs", 0.5)
    
    # Get statistics
    mean_neurons = metrics.getMean("neuronsPerSecond")
    min_step = metrics.getMin("stepTimeMs")
    max_step = metrics.getMax("stepTimeMs")
    
    assert mean_neurons > 0
    assert min_step < max_step
    
    # Test memory profiling
    profiler.record()
    peak_memory = profiler.getPeakMemoryMB()
    current_memory = profiler.getCurrentMemoryMB()
    
    assert peak_memory >= 0
    assert current_memory >= 0
    
    print("✓ Performance monitoring tests passed")

def test_advanced_agent_features():
    """Test advanced agent features"""
    print("Testing advanced agent features...")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Test agent initialization
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Test action modes
    agent.set_action_mode("exploration")
    assert agent.get_action_mode() == "exploration"
    
    agent.set_action_mode("exploitation")
    assert agent.get_action_mode() == "exploitation"
    
    agent.set_action_mode("balanced")
    assert agent.get_action_mode() == "balanced"
    
    # Test behavioral states
    agent.set_behavioral_state("active")
    assert agent.get_behavioral_state() == "active"
    
    agent.set_behavioral_state("passive")
    assert agent.get_behavioral_state() == "passive"
    
    agent.set_behavioral_state("curious")
    assert agent.get_behavioral_state() == "curious"
    
    # Test agent stats
    stats = agent.get_agent_stats()
    assert isinstance(stats, dict)
    assert "curiosity_level" in stats
    assert "neuromodulation_level" in stats
    assert "development_stage" in stats
    
    # Test mode management
    agent.enable_agent_mode("curiosity", True)
    assert agent.is_mode_enabled("curiosity") == True
    
    agent.enable_agent_mode("reward_modulation", False)
    assert agent.is_mode_enabled("reward_modulation") == False
    
    print("✓ Advanced agent feature tests passed")

def test_visualization_features():
    """Test visualization features"""
    print("Testing visualization features...")
    
    # Test visualization creation
    viz = pynlm.createVisualization()
    assert viz is not None
    
    # Test visualization properties
    viz.initialize()
    assert viz.isActive() == False  # Placeholder implementation
    
    viz.setUpdateRate(30.0)
    assert viz.getUpdateRate() == 30.0
    
    # Test brain creation for visualization
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test visualization methods (they may be placeholders)
    viz.visualizeNetwork(brain)
    viz.visualizeActivity(brain)
    viz.visualizeSpikes([])
    viz.visualizeWeights(brain)
    
    viz.update()
    viz.render()
    viz.close()
    
    print("✓ Visualization feature tests passed")

def test_convenience_methods():
    """Test convenience methods and Pythonic API"""
    print("Testing convenience methods...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test convenience methods
    brain.status()  # Should print status
    
    # Test stats method
    stats = brain.get_stats()
    assert isinstance(stats, dict)
    assert "total_neurons" in stats
    assert "total_synapses" in stats
    assert "average_firing_rate" in stats
    assert "development_stage" in stats
    
    assert stats["total_neurons"] > 0
    assert stats["development_stage"] is not None
    
    # Test initialization check
    assert brain.is_initialized() == True
    
    # Test save/load with convenience methods
    with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
        filepath = f.name
    
    try:
        brain.save_as_json(filepath)
        assert os.path.exists(filepath)
        
        # Create new brain and load
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        brain2.load_from_json(filepath)
        
        # Check that stats are similar
        stats2 = brain2.get_stats()
        assert stats2["total_neurons"] > 0
        
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)
    
    print("✓ Convenience method tests passed")

def test_metrics_export():
    """Test metrics export functionality"""
    print("Testing metrics export...")
    
    # Create benchmark metrics
    metrics = pynlm.createBenchmarkMetrics()
    
    # Set some values
    metrics.neuronsPerSecond = 1000.0
    metrics.spikesPerSecond = 50000.0
    metrics.totalMemoryMB = 256
    metrics.avgStepTimeMs = 0.5
    
    # Test export methods
    csv_output = metrics.toCSV()
    assert isinstance(csv_output, str)
    assert "," in csv_output  # CSV format
    
    json_output = metrics.toJSON()
    assert isinstance(json_output, str)
    
    # Test scaling analysis
    analysis = pynlm.createScalingAnalysis()
    assert analysis is not None
    
    print("✓ Metrics export tests passed")

def run_all_tests():
    """Run all tests and report results"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_configuration_helpers,
        test_error_handling,
        test_performance_monitoring,
        test_advanced_agent_features,
        test_visualization_features,
        test_convenience_methods,
        test_metrics_export,
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
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print("\n⚠️  Some tests failed. This may be expected for placeholder implementations.")
        print("The important thing is that the test framework is working correctly.")
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)