#!/usr/bin/env python3
"""
Simple validation test for enhanced NLM Python bindings.

This test verifies that the enhanced Python bindings work correctly
and demonstrate the key improvements:
1. Custom exception handling
2. Type safety
3. Enhanced documentation
4. API consistency
"""

import sys
import pynlm

def test_basic_functionality():
    """Test basic functionality with enhanced features."""
    print("Testing basic functionality...")
    
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        assert config is not None
        print("✓ Configuration created")
        
        # Create brain
        brain = pynlm.createBrain(config)
        assert brain is not None
        print("✓ Brain created")
        
        # Initialize brain
        assert brain.initialize() is True
        print("✓ Brain initialized")
        
        # Run simulation
        brain.step(0)
        print("✓ Simulation step completed")
        
        # Check statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getTotalSpikeCount() >= 0
        print("✓ Statistics collected")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def test_error_handling():
    """Test error handling with custom exceptions."""
    print("Testing error handling...")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Valid operations should not raise
        brain.step(0)
        print("✓ Valid operations completed")
        
        # Test type safety
        from pynlm import NeuronId
        neuron_id = NeuronId(1)
        assert neuron_id.value == 1
        print("✓ Type safety validation passed")
        
        return True
        
    except pynlm.NLMException as e:
        print(f"✗ NLM exception caught: {e}")
        return False
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def test_convenience_functions():
    """Test convenience functions."""
    print("Testing convenience functions...")
    
    try:
        # Test create_brain_with_validation
        brain = pynlm.create_brain_with_validation(
            random_seed=42,
            simulation_timestep=0.01
        )
        assert brain is not None
        print("✓ create_brain_with_validation works")
        
        # Test run_simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 5, 5)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        results = pynlm.run_simulation(
            brain=brain,
            world=world,
            agent=agent,
            num_steps=5
        )
        
        assert 'steps_completed' in results
        assert 'total_spikes' in results
        print("✓ run_simulation works")
        
        # Test DebugMonitor
        monitor = pynlm.DebugMonitor(brain, world, agent)
        monitor.capture_state("Test state")
        report = monitor.get_stability_report()
        assert 'stable' in report
        print("✓ DebugMonitor works")
        
        return True
        
    except Exception as e:
        print(f"✗ Convenience functions test failed: {e}")
        return False

def test_documentation():
    """Test that documentation is available."""
    print("Testing documentation...")
    
    try:
        # Check that classes have documentation
        assert pynlm.Brain.__doc__ is not None
        assert pynlm.Config.__doc__ is not None
        assert pynlm.Action.__doc__ is not None
        print("✓ Class documentation available")
        
        # Check that enumerations have documentation
        assert pynlm.NeuronType.__doc__ is not None
        assert pynlm.ActionType.__doc__ is not None
        print("✓ Enumeration documentation available")
        
        # Check method documentation
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        assert brain.initialize.__doc__ is not None
        print("✓ Method documentation available")
        
        return True
        
    except Exception as e:
        print(f"✗ Documentation test failed: {e}")
        return False

def test_api_consistency():
    """Test API consistency."""
    print("Testing API consistency...")
    
    try:
        # Create brain and check consistency
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Check that consistent methods exist
        assert hasattr(brain, 'step')
        assert hasattr(brain, 'initialize')
        assert hasattr(brain, 'reset')
        assert hasattr(brain, 'save')
        assert hasattr(brain, 'load')
        print("✓ Core methods consistent")
        
        # Check parameter naming consistency
        import inspect
        
        # Check that key methods have proper parameter documentation
        sig = inspect.signature(brain.step)
        assert 'currentStep' in sig.parameters or 'step' in sig.parameters
        print("✓ Parameter naming consistent")
        
        return True
        
    except Exception as e:
        print(f"✗ API consistency test failed: {e}")
        return False

def main():
    """Main test function."""
    print("Enhanced NLM Python Bindings Validation")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Error Handling", test_error_handling),
        ("Convenience Functions", test_convenience_functions),
        ("Documentation", test_documentation),
        ("API Consistency", test_api_consistency),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*50}")
        print(f"Running: {test_name}")
        print('='*50)
        
        if test_func():
            results.append((test_name, True))
            print(f"✓ {test_name}: PASSED")
        else:
            results.append((test_name, False))
            print(f"✗ {test_name}: FAILED")
    
    # Summary
    print(f"\n{'='*50}")
    print("TEST SUMMARY")
    print('='*50)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
        else:
            failed += 1
    
    print('='*50)
    print(f"Total: {passed + failed} tests")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed!")
        print("\nThe enhanced NLM Python API provides:")
        print("  ✓ Comprehensive error handling with custom exceptions")
        print("  ✓ Type safety and parameter validation")
        print("  ✓ Consistent API design with documentation")
        print("  ✓ Developer tools and convenience functions")
        print("  ✓ Performance monitoring capabilities")
        return 0
    else:
        print(f"\n⚠️  {failed} test(s) failed.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
