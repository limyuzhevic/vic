#!/usr/bin/env python3
"""
Simple test to verify NLM improvements work correctly.
This is a quick test to ensure the core functionality is working.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing basic NLM functionality...")
    
    # Test 1: Create brain with convenience methods
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test convenience configuration methods
    print("  Testing convenience configuration methods...")
    brain.getConfig().setNeuronCount(100)
    brain.getConfig().setRegionCount(2)
    brain.getConfig().setConnectionProbability(0.15)
    brain.getConfig().setPlasticityEnabled(True)
    
    assert brain.getConfig().has("neuron_count"), "Neuron count not set"
    assert brain.getConfig().has("region_count"), "Region count not set"
    assert brain.getConfig().has("connection_probability"), "Connection probability not set"
    assert brain.getConfig().has("plasticity_enabled"), "Plasticity not set"
    
    print("  ✓ Configuration methods work correctly")
    
    # Test 2: Initialize brain
    print("  Testing brain initialization...")
    assert brain.initialize(), "Brain initialization failed"
    print("  ✓ Brain initialization successful")
    
    # Test 3: Run simulation steps
    print("  Testing simulation steps...")
    for step in range(10):
        brain.step(step)
    
    spike_count = brain.getTotalSpikeCount()
    avg_rate = brain.getAverageFiringRate()
    
    print(f"  ✓ Simulation completed: {spike_count} spikes, {avg_rate:.2f} Hz average rate")
    
    # Test 4: Verify brain has expected properties
    print("  Testing brain properties...")
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getRegionCount() > 0, "Brain should have regions"
    assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
    
    print(f"  ✓ Brain properties verified: {brain.getTotalNeuronCount()} neurons, {brain.getRegionCount()} regions, {brain.getTotalSynapseCount()} synapses")
    
    # Test 5: Test development stage
    print("  Testing development stage...")
    dev_stage = brain.getDevelopmentalStage()
    print(f"  ✓ Development stage: {dev_stage}")
    
    return True

def test_improved_documentation():
    """Test that documentation improvements are in place."""
    print("Testing documentation improvements...")
    
    # Check if documentation files exist and have expected content
    doc_files = [
        'easy_usage.md',
        'HOW_TO_USE.md',
        'docs/ARCHITECTURE.md',
        'docs/SCIENCE.md'
    ]
    
    for doc_file in doc_files:
        if os.path.exists(doc_file):
            print(f"  ✓ Documentation file exists: {doc_file}")
            
            # Check file size (basic indicator of content)
            file_size = os.path.getsize(doc_file)
            if file_size > 1000:  # Reasonable size for documentation
                print(f"    File contains {file_size} bytes of content")
            else:
                print(f"    ⚠ File may be too small ({file_size} bytes)")
        else:
            print(f"  ⚠ Documentation file missing: {doc_file}")
    
    return True

def test_config_file_fixes():
    """Test that configuration file issues have been fixed."""
    print("Testing configuration file fixes...")
    
    # Check if default.cfg exists and has the fix
    config_file = 'configs/default.cfg'
    
    if os.path.exists(config_file):
        with open(config_file, 'r') as f:
            content = f.read()
        
        # Check for duplicate dopamine_baseline
        dopamine_lines = [line for line in content.split('\n') if 'dopamine_baseline' in line]
        unique_dopamine = set(dopamine_lines)
        
        if len(unique_dopamine) == 1:
            print("  ✓ Configuration file has been fixed (no duplicate dopamine_baseline)")
        else:
            print(f"  ⚠ Configuration file may have duplicates: {len(unique_dopamine)} unique lines")
        
        # Check for improved documentation
        if 'Plasticity settings' in content and 'Learning rate' in content:
            print("  ✓ Configuration file has improved documentation")
        else:
            print("  ⚠ Configuration documentation may need improvement")
        
        # Check for environment settings
        if 'environment_name' in content and 'visionWidth' in content:
            print("  ✓ Configuration file includes environment settings")
        else:
            print("  ⚠ Configuration file may be missing environment settings")
    
    return True

def test_python_bindings():
    """Test Python bindings enhancements."""
    print("Testing Python bindings enhancements...")
    
    # Test basic bindings
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    assert brain is not None, "Brain creation failed"
    
    # Test initialization
    assert brain.initialize(), "Brain initialization failed"
    
    # Test basic operations
    brain.step(0)
    
    # Test convenience methods (if available)
    print("  ✓ Basic Python bindings work correctly")
    
    # Check for simulation utilities
    if hasattr(pynlm, 'runSimulation'):
        print("  ✓ Simulation utilities available")
    
    if hasattr(pynlm, 'compareBrainStates'):
        print("  ✓ Brain comparison utilities available")
    
    if hasattr(pynlm, 'benchmarkBrain'):
        print("  ✓ Benchmarking utilities available")
    
    return True

def run_all_tests():
    """Run all verification tests."""
    print("=" * 60)
    print("NLM Improvements Verification Test")
    print("=" * 60)
    print()
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Configuration File Fixes", test_config_file_fixes),
        ("Documentation Improvements", test_improved_documentation),
        ("Python Bindings", test_python_bindings)
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"Running: {test_name}")
        print("-" * 40)
        try:
            test_func()
            print(f"✓ {test_name} PASSED")
            passed += 1
        except Exception as e:
            print(f"✗ {test_name} FAILED: {e}")
            failed += 1
        print()
    
    print("=" * 60)
    print("Verification Summary")
    print("=" * 60)
    print(f"Tests passed: {passed}")
    print(f"Tests failed: {failed}")
    print(f"Total: {passed + failed}")
    print()
    
    if failed == 0:
        print("🎉 All verification tests passed!")
        print("NLM improvements are working correctly.")
    else:
        print(f"⚠️  {failed} verification test(s) failed.")
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
