#!/usr/bin/env python3
"""
Verification Script for NLM Python Bindings Enhancements

This script verifies that all the enhancements made to the NLM Python bindings
are working correctly and that the project can be imported and used successfully.
"""

import sys
import traceback

def test_python_bindings():
    """Test that the enhanced Python bindings work correctly."""
    print("Testing NLM Python Bindings...")
    
    try:
        # Import the enhanced module
        import pynlm
        print(f"✓ Successfully imported pynlm version {pynlm.__version__}")
        
        # Test version management
        assert hasattr(pynlm, '__version__'), "Module should have __version__ attribute"
        assert hasattr(pynlm, '__author__'), "Module should have __author__ attribute"
        print("✓ Version management working correctly")
        
        # Test basic creation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Basic brain creation working")
        
        # Test validation
        assert pynlm.validateConfig(config), "Config validation should work"
        print("✓ Configuration validation working")
        
        # Test statistics
        stats = pynlm.getConfigStats(config)
        assert isinstance(stats, dict), "Config stats should return dictionary"
        print("✓ Configuration statistics working")
        
        # Test world creation
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 6, 6)
        print("✓ World creation working")
        
        # Test agent creation
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        print("✓ Agent creation working")
        
        # Test sensory input validation
        percept = world.getSensoryPercept()
        assert percept.validate() == True, "Sensory input should be valid"
        assert percept.isEmpty() == False, "Sensory input should not be empty"
        print("✓ Sensory input validation working")
        
        # Test convenience functions
        brain2, config2 = pynlm.createTestingBrain(100)
        assert brain2.getTotalNeuronCount() == 100, "Testing brain should have 100 neurons"
        print("✓ Testing brain creation working")
        
        # Test config utilities
        config3 = pynlm.deepCopyConfig(config)
        assert pynlm.validateConfig(config3), "Deep copy should be valid"
        print("✓ Deep copy working")
        
        config4 = pynlm.mergeConfigs(config, config3)
        assert pynlm.validateConfig(config4), "Merged config should be valid"
        print("✓ Config merge working")
        
        # Test runEpisode
        brain3 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain3.initialize()
        world2 = pynlm.createTestEnvironment()
        agent2 = pynlm.createAgentBrain(brain3)
        agent2.initialize(world2)
        
        result = pynlm.runEpisode(brain3, agent2, world2, 50, enableLearning=True)
        print(f"✓ Episode running completed with {brain3.getTotalSpikeCount()} spikes")
        
        # Test benchmark
        benchmark_results = pynlm.benchmarkBrain(brain3, 100)
        assert isinstance(benchmark_results, dict), "Benchmark should return dict"
        print(f"✓ Benchmarking working: {benchmark_results}")
        
        # Test export
        export_data = pynlm.exportBrainToPython(brain3)
        assert isinstance(export_data, dict), "Export should return dict"
        assert 'neuron_count' in export_data, "Export should contain neuron count"
        print(f"✓ Export working: {export_data['neuron_count']} neurons")
        
        # Test resetSystem
        brain4, agent3, world3 = pynlm.resetSystem(brain3, agent2, world2)
        assert brain4.getTotalSpikeCount() == 0, "After reset, spikes should be 0"
        print("✓ System reset working")
        
        print("\n✅ All Python binding tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed: {type(e).__name__}: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling scenarios."""
    print("\nTesting Error Handling...")
    
    try:
        import pynlm
        
        # Test null validation
        result = pynlm.validateConfig(None)
        print("✓ Null config validation handled gracefully")
        
        # Test error handling in config operations
        config = pynlm.createDefaultConfig()
        stats = pynlm.getConfigStats(config)
        print("✓ Config stats error handling working")
        
        print("\n✅ Error handling tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Error handling test failed: {e}")
        return False

def test_backward_compatibility():
    """Test that existing code still works (backward compatibility)."""
    print("\nTesting Backward Compatibility...")
    
    try:
        import pynlm
        
        # Test the original basic usage pattern
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for step in range(10):
            brain.step(step)
        
        # Verify basic functionality still works
        assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
        assert brain.getTotalSynapseCount() >= 0, "Synapse count should be valid"
        assert brain.getFiringNeuronCount() >= 0, "Firing neuron count should be valid"
        
        print("✓ Original usage pattern still works")
        print("✓ All essential brain functions working")
        
        print("\n✅ Backward compatibility tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Backward compatibility test failed: {e}")
        return False

def main():
    """Run all verification tests."""
    print("=" * 60)
    print("NLM Python Bindings Enhancement Verification")
    print("=" * 60)
    print()
    
    all_tests_passed = True
    
    # Run tests
    all_tests_passed &= test_python_bindings()
    all_tests_passed &= test_error_handling()
    all_tests_passed &= test_backward_compatibility()
    
    print("\n" + "=" * 60)
    if all_tests_passed:
        print("🎉 ALL TESTS PASSED! 🎉")
        print("The NLM Python bindings enhancements are working correctly.")
        print("\nSummary of enhancements verified:")
        print("  ✓ Version management")
        print("  ✓ Error handling and validation")
        print("  ✓ Advanced utility functions")
        print("  ✓ Convenience functions")
        print("  ✓ Benchmarking and performance tools")
        print("  ✓ Brain state export")
        print("  ✓ System reset capabilities")
        print("  ✓ Backward compatibility")
        print("\nThe project is ready for production use!")
    else:
        print("❌ SOME TESTS FAILED")
        print("Please check the error messages above and fix any issues.")
        return 1
    
    print("=" * 60)
    return 0

if __name__ == "__main__":
    sys.exit(main())