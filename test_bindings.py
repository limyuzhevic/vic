#!/usr/bin/env python3
"""
Test script for NLM Python bindings improvements.
Tests enhanced APIs, backward compatibility, and new functionality.
"""

import sys
import os
import traceback
import time
from typing import Dict, Any

# Add the python directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_backward_compatibility():
    """Test that existing API still works."""
    print("\n=== Testing Backward Compatibility ===")
    
    try:
        # Test basic functionality from original documentation
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
        
        brain = pynlm.createBrain(config)
        print("✓ createBrain() works")
        
        brain.initialize()
        print("✓ brain.initialize() works")
        
        for i in range(10):
            brain.step(i)
        print("✓ brain.step() works")
        
        spike_count = brain.getTotalSpikeCount()
        print(f"✓ getTotalSpikeCount() returns: {spike_count}")
        
        # Test world functionality
        world = pynlm.createSimpleWorld()
        print("✓ createSimpleWorld() works")
        
        world.configure(10, 10, 8, 8)
        print("✓ world.configure() works")
        
        world.reset()
        print("✓ world.reset() works")
        
        # Test agent functionality
        agent = pynlm.createAgentBrain(brain)
        print("✓ createAgentBrain() works")
        
        agent.initialize(world)
        print("✓ agent.initialize() works")
        
        percept = world.getSensoryPercept()
        print("✓ world.getSensoryPercept() works")
        
        agent.processSensoryInput(percept)
        print("✓ agent.processSensoryInput() works")
        
        action = agent.decodeMotorCommand()
        print(f"✓ agent.decodeMotorCommand() returns: {action}")
        
        return True
        
    except Exception as e:
        print(f"✗ Backward compatibility test failed: {e}")
        traceback.print_exc()
        return False

def test_enhanced_apis():
    """Test new enhanced APIs."""
    print("\n=== Testing Enhanced APIs ===")
    
    try:
        # Test enhanced convenience methods
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test Action name conversion
        action_name = brain.produceAction().getName()
        print(f"✓ Action name: {action_name}")
        
        # Test enhanced brain statistics
        firing_rate = brain.getAverageFiringRate()
        print(f"✓ Average firing rate: {firing_rate}")
        
        # Test new convenience functions
        spike_count = pynlm.run_quick_test(100)
        print(f"✓ run_quick_test() returned: {spike_count} spikes")
        
        # Test benchmark function
        stats = pynlm.benchmark_performance(500)
        print(f"✓ benchmark_performance() returned stats with {len(stats)} entries")
        print(f"  - Total time: {stats['total_time_ms']} ms")
        print(f"  - FPS: {stats['fps']}")
        
        # Test training environment creation
        config_settings = {"brain.neuron_count": 200}
        world_settings = {"width": 15, "height": 15, "visionWidth": 8, "visionHeight": 8}
        
        env = pynlm.create_training_environment(config_settings, world_settings)
        print("✓ create_training_environment() works")
        
        config, brain, agent, world = env
        print("✓ Environment unpacking works")
        
        # Test experiment system
        experiment = pynlm.Experiment("test_experiment")
        experiment.setSeed(42)
        experiment.setConfigValue("brain.neuron_count", "200")
        print("✓ Experiment creation works")
        
        experiment.recordMetric("spike_rate", 0.5)
        experiment.recordMetric("memory_usage", 0.8)
        print("✓ Metric recording works")
        
        # Test Phase 6 integration
        phase6_result = pynlm.verify_phase6_integration()
        print(f"✓ Phase 6 integration verification: {phase6_result}")
        
        system_tests = pynlm.test_phase6_systems()
        print(f"✓ Phase 6 system tests: {system_tests}")
        
        return True
        
    except Exception as e:
        print(f"✗ Enhanced APIs test failed: {e}")
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced features."""
    print("\n=== Testing Advanced Features ===")
    
    try:
        # Test visualization tools
        viz = pynlm.VisualizationTools()
        print("✓ VisualizationTools created")
        
        # Test configuration presets
        presets = pynlm.ConfigPresets()
        
        default_config = presets.get_default_config()
        print("✓ ConfigPresets.get_default_config() works")
        
        # Test performance monitor
        perf_monitor = pynlm.PerformanceMonitor()
        print("✓ PerformanceMonitor created")
        
        perf_monitor.start_monitoring()
        print("✓ Performance monitoring started")
        
        # Simulate some work
        for i in range(50):
            perf_monitor.record_timestep(i, 500, 250)
        
        perf_monitor.stop_monitoring()
        print("✓ Performance monitoring stopped")
        
        stats = perf_monitor.get_performance_stats()
        print(f"✓ Performance stats retrieved: {len(stats)} entries")
        
        # Test experiment builder
        builder = pynlm.ExperimentBuilder()
        builder.set_brain_parameters(300, 2)
        builder.set_plasticity_parameters(True, True)
        builder.enable_checkpointing("checkpoint.bin")
        
        built_config = builder.build_config()
        print("✓ ExperimentBuilder works")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced features test failed: {e}")
        traceback.print_exc()
        return False

def test_performance():
    """Test performance of new features."""
    print("\n=== Testing Performance ===")
    
    try:
        # Time the enhanced convenience methods
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for i in range(100):
            brain.step(i)
        end_time = time.time()
        
        print(f"✓ 100 brain steps took {end_time - start_time:.3f} seconds")
        print(f"  - Rate: {(100 / (end_time - start_time)):.1f} steps/second")
        
        # Time the new convenience functions
        start_time = time.time()
        result = pynlm.run_quick_test(200)
        end_time = time.time()
        
        print(f"✓ run_quick_test(200) took {end_time - start_time:.3f} seconds")
        
        # Time benchmark function
        start_time = time.time()
        stats = pynlm.benchmark_performance(300)
        end_time = time.time()
        
        print(f"✓ benchmark_performance(300) took {end_time - start_time:.3f} seconds")
        
        return True
        
    except Exception as e:
        print(f"✗ Performance test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings - Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        ("Backward Compatibility", test_backward_compatibility),
        ("Enhanced APIs", test_enhanced_apis),
        ("Advanced Features", test_advanced_features),
        ("Performance", test_performance),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'=' * 60}")
        print(f"Running: {test_name}")
        print('=' * 60)
        
        success = test_func()
        results.append((test_name, success))
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{status}: {test_name}")
        
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed > 0:
        print(f"\n❌ {failed} test(s) failed. Please check the output above.")
        return 1
    else:
        print(f"\n✅ All {passed} tests passed!")
        return 0

if __name__ == "__main__":
    sys.exit(main())