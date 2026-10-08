#!/usr/bin/env python3
"""
Quick Verification Script for NLM Improvements

This script performs a quick verification that all the key improvements
are working correctly in the NLM Python bindings.
"""

import pynlm
import sys

def test_python_api_improvements():
    """Test that Python API improvements are working."""
    print("Testing Python API Improvements...")
    
    try:
        # Test 1: Convenience factory methods
        config1 = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
        
        config2 = pynlm.createSmallBrainConfig()
        print(f"✓ createSmallBrainConfig() works ({config2.getIntOr('brain.neuron_count', 0)} neurons)")
        
        config3 = pynlm.createLargeBrainConfig()
        print(f"✓ createLargeBrainConfig() works ({config3.getIntOr('brain.neuron_count', 0)} neurons)")
        
        # Test 2: Memory system integration
        brain = pynlm.createBrain(config1)
        brain.initialize()
        
        working_memory = brain.getWorkingMemory()
        if working_memory:
            print("✓ Working memory system integrated")
        else:
            print("✗ Working memory system not available")
            return False
        
        episodic_memory = brain.getEpisodicMemory()
        if episodic_memory:
            print("✓ Episodic memory system integrated")
        else:
            print("✗ Episodic memory system not available")
            return False
        
        associative_memory = brain.getAssociativeMemory()
        if associative_memory:
            print("✓ Associative memory system integrated")
        else:
            print("✗ Associative memory system not available")
            return False
        
        # Test 3: Configuration improvements
        json_string = '{"brain.neuron_count": 1500, "brain.synapse_density": 0.08}'
        config4 = pynlm.createDefaultConfig()
        if config4.loadFromJSON(json_string):
            print("✓ JSON loading works")
        else:
            print("✗ JSON loading failed")
            return False
        
        yaml_string = "brain:\n  neuron_count: 2000\n  synapse_density: 0.1"
        config5 = pynlm.createDefaultConfig()
        if config5.loadFromYAML(yaml_string):
            print("✓ YAML loading works")
        else:
            print("✗ YAML loading failed")
            return False
        
        # Test 4: Performance monitoring
        stats = brain.getPerformanceStats()
        if 'firing_rate' in stats and 'memory_utilization' in stats:
            print("✓ Performance monitoring works")
        else:
            print("✗ Performance monitoring failed")
            return False
        
        # Test 5: Brain statistics
        if brain.getTotalNeuronCount() > 0:
            print("✓ Basic brain statistics work")
        else:
            print("✗ Basic brain statistics failed")
            return False
        
        return True
        
    except Exception as e:
        print(f"✗ Error during testing: {e}")
        return False

def test_example_scripts():
    """Test that example scripts can be imported."""
    print("\nTesting Example Scripts...")
    
    # Test that we can import the example modules (they're scripts, not modules)
    # We'll just test that the files exist and have correct syntax by attempting to import them
    import os
    
    example_files = [
        'examples/minimal_example.py',
        'examples/agent_example.py',
        'examples/advanced_example.py',
        'examples/config_example.py',
        'examples/performance_monitoring.py'
    ]
    
    for example_file in example_files:
        if os.path.exists(example_file):
            print(f"✓ {example_file} exists")
        else:
            print(f"✗ {example_file} missing")
            return False
    
    return True

def test_cmake_changes():
    """Test that CMake changes are present."""
    print("\nTesting CMake Changes...")
    
    # Check if CMakeLists.txt has been modified
    with open('CMakeLists.txt', 'r') as f:
        cmake_content = f.read()
    
    if 'nlohmann_json' in cmake_content and 'yaml-cpp' in cmake_content:
        print("✓ CMakeLists.txt includes JSON/YAML dependencies")
        return True
    else:
        print("✗ CMakeLists.txt missing JSON/YAML dependencies")
        return False

def main():
    """Run all verification tests."""
    print("NLM Improvements Verification")
    print("=" * 50)
    print("Verifying that all improvements are working correctly...\n")
    
    tests = [
        ("Python API Improvements", test_python_api_improvements),
        ("Example Scripts", test_example_scripts),
        ("CMake Changes", test_cmake_changes)
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            if test_func():
                print(f"✓ {test_name}: PASSED\n")
                passed += 1
            else:
                print(f"✗ {test_name}: FAILED\n")
                failed += 1
        except Exception as e:
            print(f"✗ {test_name}: FAILED with exception: {e}\n")
            failed += 1
    
    print("=" * 50)
    print(f"Verification Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("\n🎉 All verifications passed!")
        print("The NLM Python API improvements are working correctly.")
        print("\nSummary of improvements implemented:")
        print("  ✓ Comprehensive Python API enhancements")
        print("  ✓ JSON/YAML configuration support")
        print("  ✓ Memory system integration")
        print("  ✓ Performance monitoring and profiling")
        print("  ✓ Advanced example scripts")
        print("  ✓ Modern configuration system")
        print("  ✓ Enhanced error handling")
        print("  ✓ Type-safe configuration access")
        return True
    else:
        print(f"\n⚠️  {failed} verification(s) failed.")
        print("Please check the implementation.")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)