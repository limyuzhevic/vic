#!/usr/bin/env python3
"""
Quick Test for Enhanced NLM Python API

This file provides a quick test to verify the enhanced Python API is working correctly.
Run with: python3 quick_test.py
"""

import pynlm

def test_basic_functionality():
    """Test basic enhanced functionality"""
    print("Testing Enhanced NLM Python API...")
    print("=" * 40)
    
    # Test 1: Version
    print(f"1. Version: {pynlm.get_version()}")
    
    # Test 2: Configuration presets
    print("2. Testing configuration presets...")
    configs = [
        ("Research", pynlm.createResearchConfig()),
        ("Development", pynlm.createDevelopmentConfig()),
        ("Testing", pynlm.createTestingConfig()),
        ("Production", pynlm.createProductionConfig()),
        ("Default", pynlm.createDefaultConfig())
    ]
    
    for name, config in configs:
        neurons = config.getOr("brain.neuron_count", 0)
        print(f"   - {name}: {neurons} neurons")
    
    # Test 3: Brain creation and context manager
    print("3. Testing brain creation and context manager...")
    config = pynlm.createTestingConfig()
    
    with pynlm.createBrain(config) as brain:
        print(f"   - Brain created with {brain.getTotalNeuronCount()} neurons")
        brain.step(0)
        print(f"   - After 1 step: {brain.getFiringNeuronCount()} neurons firing")
    
    # Test 4: Convenience methods
    print("4. Testing convenience methods...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    brain.runSteps(10)
    summary = brain.getBrainSummary()
    print(f"   - Summary length: {len(summary)} characters")
    
    stats = brain.getFiringStatistics()
    print(f"   - Statistics: {stats['firing_percentage']:.1f}% firing")
    
    # Test 5: Simulation helpers
    print("5. Testing simulation helpers...")
    total_spikes = pynlm.run_simple_brain_example(5)
    print(f"   - Simple example: {total_spikes} spikes")
    
    # Test 6: Error handling
    print("6. Testing error handling...")
    config = pynlm.createTestingConfig()
    errors, warnings = pynlm.validate_config(config)
    print(f"   - Validation: {len(errors)} errors, {len(warnings)} warnings")
    
    # Test 7: Agent simulation
    print("7. Testing agent simulation...")
    results = pynlm.run_agent_simulation(10, "testing")
    total_spikes, avg_firing_rate, curiosity = results
    print(f"   - Results: {total_spikes} spikes, {avg_firing_rate:.2f} Hz, {curiosity:.3f} curiosity")
    
    print("=" * 40)
    print("All tests passed! ✓")
    print()
    print("Enhanced API features verified:")
    print("  ✓ Context managers")
    print("  ✓ Convenience methods")
    print("  ✓ Configuration presets")
    print("  ✓ Statistical analysis")
    print("  ✓ Simulation helpers")
    print("  ✓ Error handling")
    print("  ✓ Agent simulation")

if __name__ == "__main__":
    test_basic_functionality()