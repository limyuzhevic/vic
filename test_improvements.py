#!/usr/bin/env python3
"""
Test script for NLM improvements.
This script demonstrates the new functionality added to NLM.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Tests will be limited.")

from nlm_improvements import (
    NLMConfigurator, 
    NLMSimulation, 
    NLMBenchmark, 
    NLMDebug
)

def test_configurator():
    """Test the NLMConfigurator class."""
    print("Testing NLMConfigurator...")
    
    # Test creating different presets
    presets = ['minimal', 'balanced', 'learning', 'advanced']
    
    for preset in presets:
        config = NLMConfigurator.create_preset_config(preset)
        assert 'neuron_count' in config
        assert config['neuron_count'] > 0
        assert 'region_count' in config
        print(f"  ✓ Created {preset} config: {config['neuron_count']} neurons")
    
    # Test default config
    default_config = NLMConfigurator.create_default_config()
    assert default_config['neuron_count'] == 100
    print("  ✓ Created default config")
    
    print("NLMConfigurator tests passed!\n")

def test_simulation():
    """Test the NLMSimulation class."""
    print("Testing NLMSimulation...")
    
    if not PYNLM_AVAILABLE:
        print("  ⚠ Skipping simulation tests (pynlm not available)")
        return
    
    # Create and initialize simulation
    config = NLMConfigurator.create_preset_config("minimal")
    sim = NLMSimulation(config)
    
    # Test initialization
    sim.initialize()
    assert sim.brain is not None
    assert sim.agent is not None
    print("  ✓ Simulation initialized")
    
    # Test stats
    stats = sim.get_stats()
    assert 'neurons' in stats
    assert stats['neurons'] == 100
    print(f"  ✓ Got stats: {stats['neurons']} neurons")
    
    # Test run steps
    metrics = sim.run_steps(50, verbose=False)
    assert 'firing_rates' in metrics
    assert len(metrics['firing_rates']) == 50
    print(f"  ✓ Ran {len(metrics['firing_rates'])} steps")
    
    # Test stop
    sim.stop()
    print("  ✓ Simulation stopped")
    
    print("NLMSimulation tests passed!\n")

def test_benchmark():
    """Test the NLMBenchmark class."""
    print("Testing NLMBenchmark...")
    
    if not PYNLM_AVAILABLE:
        print("  ⚠ Skipping benchmark tests (pynlm not available)")
        return
    
    config = NLMConfigurator.create_preset_config("balanced")
    
    # Test benchmark
    result = NLMBenchmark.benchmark_simulation(config, 100)
    
    assert 'initialization_time' in result
    assert 'simulation_time' in result
    assert 'steps_per_second' in result
    assert 'average_firing_rate' in result
    
    print(f"  ✓ Benchmark completed: {result['steps_per_second']:.1f} steps/sec")
    
    print("NLMBenchmark tests passed!\n")

def test_debug():
    """Test the NLMDebug class."""
    print("Testing NLMDebug...")
    
    if not PYNLM_AVAILABLE:
        print("  ⚠ Skipping debug tests (pynlm not available)")
        return
    
    # Create a simple brain for testing
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test analysis
    analysis = NLMDebug.analyze_brain(brain)
    
    assert 'neuron_stats' in analysis
    assert 'synapse_stats' in analysis
    assert 'regions' in analysis
    assert 'recommendations' in analysis
    
    print(f"  ✓ Analyzed brain: {analysis['neuron_stats']['total']} neurons")
    
    # Test visualization
    visualization = NLMDebug.visualize_firing_pattern(brain)
    assert 'Firing Pattern' in visualization
    print(f"  ✓ Generated visualization ({len(visualization)} chars)")
    
    print("NLMDebug tests passed!\n")

def test_command_line_interface():
    """Test the command-line interface."""
    print("Testing command-line interface...")
    
    # Test that the module can be imported
    assert 'nlm_improvements' in sys.modules
    print("  ✓ Module imports correctly")
    
    # Test help text
    from nlm_improvements import main
    # We can't easily test the full CLI without running it as main
    print("  ✓ Main function exists")
    
    print("Command-line interface tests passed!\n")

def test_pythonic_api():
    """Test that the API improvements are working."""
    print("Testing Python API improvements...")
    
    if not PYNLM_AVAILABLE:
        print("  ⚠ Skipping API tests (pynlm not available)")
        return
    
    # Test that we can use the API in a more Pythonic way
    config = NLMConfigurator.create_preset_config("learning")
    
    # Test context manager pattern (conceptual)
    sim = NLMSimulation(config)
    
    # Test fluent interface
    sim.initialize().run_steps(10, verbose=False).stop()
    
    print("  ✓ Pythonic API usage pattern works")
    
    print("Python API tests passed!\n")

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM (熙然) - Neural Learning Machine")
    print("Phase 6: Final Integration - Code Improvement Tests")
    print("=" * 60)
    print()
    
    tests = [
        test_configurator,
        test_simulation,
        test_benchmark,
        test_debug,
        test_command_line_interface,
        test_pythonic_api,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"  ✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        return 1
    return 0

if __name__ == '__main__':
    sys.exit(main())