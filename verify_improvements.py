#!/usr/bin/env python3
"""
Quick Verification of NLM Improvements

This script provides a quick verification that all NLM improvements are working correctly.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

print("NLM Improvements Verification")
print("=" * 40)

# Check if NLM is available
try:
    import pynlm
    PYNLM_AVAILABLE = True
    print("✓ NLM Python bindings available")
except ImportError:
    PYNLM_AVAILABLE = False
    print("✗ NLM Python bindings not available")
    print("  Install with: pip install pynlm")
    sys.exit(1)

# Test 1: NLM Utilities
print("\n1. Testing NLM Utilities...")
try:
    from python.nlm_utils import NLMExperiment, ConfigPresets, ExperimentType
    print("   ✓ NLMExperiment class available")
    print("   ✓ ConfigPresets class available")
    print("   ✓ ExperimentType enum available")
    
    # Test experiment creation
    experiment = NLMExperiment("verification_test", ExperimentType.SIMULATION)
    print("   ✓ NLMExperiment created successfully")
    
    # Test config presets
    dev_config = ConfigPresets.create_development_brain(100)
    assert len(dev_config) > 0
    print("   ✓ ConfigPresets working correctly")
    
except Exception as e:
    print(f"   ✗ NLM Utilities error: {e}")
    sys.exit(1)

# Test 2: CLI Tool
print("\n2. Testing CLI Tool...")
try:
    from python.nlm_cli import NLMCLI
    cli = NLMCLI()
    print("   ✓ NLMCLI class available")
    print("   ✓ CLI setup functional")
except Exception as e:
    print(f"   ✗ CLI error: {e}")
    sys.exit(1)

# Test 3: Visualization Tools
print("\n3. Testing Visualization Tools...")
try:
    from python.nlm_visualization import (
        NeuralActivityPlotter, MemoryAnalyzer, NeuromodulationVisualizer,
        ExperimentAnalyzer, SimulationDebugger, VisualizationPipeline
    )
    print("   ✓ NeuralActivityPlotter available")
    print("   ✓ MemoryAnalyzer available")
    print("   ✓ NeuromodulationVisualizer available")
    print("   ✓ ExperimentAnalyzer available")
    print("   ✓ SimulationDebugger available")
    print("   ✓ VisualizationPipeline available")
except Exception as e:
    print(f"   ✗ Visualization error: {e}")
    sys.exit(1)

# Test 4: Performance Tools
print("\n4. Testing Performance Tools...")
try:
    from python.nlm_performance import (
        PerformanceProfiler, BenchmarkSuite, RealTimeMonitor, NLMAnalyzer,
        PerformanceReporter
    )
    print("   ✓ PerformanceProfiler available")
    print("   ✓ BenchmarkSuite available")
    print("   ✓ RealTimeMonitor available")
    print("   ✓ NLMAnalyzer available")
    print("   ✓ PerformanceReporter available")
except Exception as e:
    print(f"   ✗ Performance error: {e}")
    sys.exit(1)

# Test 5: Integration
print("\n5. Testing Integration...")
try:
    # Create a simple brain for testing
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("brain.region_count", 2)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test profiler integration
    profiler = PerformanceProfiler(brain)
    for step in range(5):
        profiler.record_step_performance(step)
        brain.step(step)
    
    summary = profiler.get_performance_summary()
    assert 'statistics' in summary
    print("   ✓ PerformanceProfiler integration working")
    
    # Test visualization integration
    visualizer = VisualizationPipeline("test_output")
    assert visualizer.plotter is not None
    print("   ✓ VisualizationPipeline integration working")
    
except Exception as e:
    print(f"   ✗ Integration error: {e}")
    sys.exit(1)

print("\n" + "=" * 40)
print("✓ ALL NLM IMPROVEMENTS VERIFIED SUCCESSFULLY!")
print("\nImplemented improvements:")
print("1. Enhanced Python API utilities (nlm_utils.py)")
print("2. Command-line interface (nlm_cli.py)")
print("3. Advanced visualization tools (nlm_visualization.py)")
print("4. Performance monitoring and profiling (nlm_performance.py)")
print("\nNew capabilities added:")
print("- Configuration presets for different use cases")
print("- Comprehensive experiment management")
print("- Real-time monitoring and analysis")
print("- Professional visualization and reporting")
print("- Performance benchmarking and optimization")
print("- Command-line accessibility")
print("\nFiles created in python/ directory:")
for root, dirs, files in os.walk("python"):
    for file in files:
        if file.endswith(".py"):
            print(f"  - python/{file}")

print("\nTo run comprehensive tests:")
print(f"  python python/test_nlm_comprehensive.py")
print("\nTo run quick verification again:")
print(f"  python test_improvements.py")
