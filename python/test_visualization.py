#!/usr/bin/env python3
"""
Python test script for NLM Phase 6 visualization tools.

This script tests the Python bindings for visualization and demonstrates
various visualization features.
"""

import sys
import os
import time
import json
import pynlm_viz

def test_basic_visualization():
    """Test basic visualization functionality"""
    print("=== Testing Basic Visualization ===")
    
    # Create visualization interface
    viz = pynlm_viz.VisualizationInterface(pynlm_viz.VisualizationMode.TextAdvanced)
    
    # Initialize with custom settings
    viz.initialize(
        title="NLM Phase 6 - Python Test",
        width=120,
        height=80
    )
    
    print("✅ Visualization interface created successfully")
    print(f"   Mode: {viz.getMode()}")
    print(f"   Active: {viz.isActive()}")
    print(f"   Update rate: {viz.getUpdateRate()} Hz")
    
    # Test metrics
    metrics = viz.getCurrentMetrics()
    print(f"   Initial metrics loaded")
    
    # Test configuration
    config = viz.getConfig()
    print(f"   Configuration loaded: {config.window_title}")
    
    viz.close()
    print("✅ Basic visualization test completed successfully")
    return True

def test_visualization_config():
    """Test visualization configuration"""
    print("\n=== Testing Visualization Configuration ===")
    
    # Create different visualization modes
    modes = [
        pynlm_viz.VisualizationMode.Text,
        pynlm_viz.VisualizationMode.TextAdvanced,
        pynlm_viz.VisualizationMode.Compact,
        pynlm_viz.VisualizationMode.PythonAPI
    ]
    
    for mode in modes:
        print(f"\nTesting mode: {mode}")
        viz = pynlm_viz.VisualizationInterface(mode)
        viz.initialize("Test", 80, 40)
        
        config = viz.getConfig()
        print(f"   Config: {config.window_title}")
        print(f"   Size: {config.width}x{config.height}")
        
        viz.close()
        print(f"   ✅ Mode {mode} test passed")
    
    print("\n✅ All configuration tests passed")
    return True

def test_command_line_help():
    """Test command line help functionality"""
    print("\n=== Testing Command Line Help ===")
    
    help_text = pynlm_viz.VisualizationInterface.getCommandLineHelp()
    
    if help_text and "Visualization Command Line Options" in help_text:
        print("✅ Command line help retrieved successfully")
        print(f"   Length: {len(help_text)} characters")
        print("   Contains expected sections")
        return True
    else:
        print("❌ Command line help test failed")
        return False

def test_metrics_export():
    """Test metrics export functionality"""
    print("\n=== Testing Metrics Export ===")
    
    viz = pynlm_viz.VisualizationInterface(pynlm_viz.VisualizationMode.TextAdvanced)
    viz.initialize("Export Test", 100, 60)
    
    # Get current metrics
    metrics = viz.getCurrentMetrics()
    print(f"   Metrics retrieved: reward={metrics.total_reward}, firing_rate={metrics.avg_firing_rate}")
    
    # Test export (in memory)
    try:
        viz.exportData("/tmp/test_visualization.json")
        print("   ✅ Data export test passed")
    except Exception as e:
        print(f"   ❌ Data export test failed: {e}")
        return False
    
    try:
        viz.exportMetrics("/tmp/test_visualization.csv")
        print("   ✅ Metrics export test passed")
    except Exception as e:
        print(f"   ❌ Metrics export test failed: {e}")
        return False
    
    viz.close()
    print("✅ All export tests passed")
    return True

def test_integration_status():
    """Test integration status tracking"""
    print("\n=== Testing Integration Status ===")
    
    viz = pynlm_viz.VisualizationInterface(pynlm_viz.VisualizationMode.TextAdvanced)
    viz.initialize("Integration Test", 100, 60)
    
    # Get metrics to check integration status
    metrics = viz.getCurrentMetrics()
    
    print(f"   Memory integrated: {metrics.memory_integrated}")
    print(f"   Episodic memory integrated: {metrics.episodic_memory_integrated}")
    print(f"   Neuromodulation integrated: {metrics.neuromodulation_integrated}")
    print(f"   Prediction integrated: {metrics.prediction_integrated}")
    print(f"   Development integrated: {metrics.development_integrated}")
    print(f"   Checkpointing works: {metrics.checkpointing_works}")
    print(f"   Replay works: {metrics.replay_works}")
    
    # Check if we have all required metrics
    required_metrics = [
        'total_reward', 'avg_firing_rate', 'avg_synaptic_weight',
        'novelty_level', 'curiosity_level', 'dopamine_level',
        'working_memory_active_traces', 'episodic_memory_episodes',
        'prediction_confidence', 'development_stage', 'simulation_time',
        'wall_clock_time', 'memory_integrated', 'episodic_memory_integrated',
        'neuromodulation_integrated', 'prediction_integrated', 'development_integrated',
        'checkpointing_works', 'replay_works'
    ]
    
    missing_metrics = [metric for metric in required_metrics if not hasattr(metrics, metric)]
    if missing_metrics:
        print(f"   ❌ Missing metrics: {missing_metrics}")
        return False
    else:
        print("   ✅ All required metrics present")
    
    viz.close()
    print("✅ Integration status test passed")
    return True

def run_all_tests():
    """Run all visualization tests"""
    print("NLM Phase 6 Visualization Tools - Python Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_visualization,
        test_visualization_config,
        test_command_line_help,
        test_metrics_export,
        test_integration_status
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"❌ Test {test.__name__} failed with exception: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print("Test Results Summary:")
    print(f"  ✅ Passed: {passed}")
    print(f"  ❌ Failed: {failed}")
    print(f"  📊 Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed successfully!")
        return True
    else:
        print(f"\n⚠️  {failed} test(s) failed. Please check the output above.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)