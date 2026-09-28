#!/usr/bin/env python3
"""
Verification Script for NLM Improvements

This script verifies that the NLM improvements are working correctly.
"""

import sys
import traceback

def test_pythonic_interface():
    """Test the Pythonic interface functionality."""
    print("Testing Pythonic Interface...")
    
    try:
        import nlm
        print("✓ Successfully imported nlm module")
        
        # Test Config
        print("\nTesting Config...")
        config = nlm.Config()
        print(f"  ✓ Created Config: {config}")
        
        # Test basic configuration
        config.set("brain.neuron_count", 500)
        config.set("plasticity.stdp.enable", True)
        print(f"  ✓ Set configuration values")
        
        # Test Pythonic property access
        neuron_count = config.get_int("brain.neuron_count")
        print(f"  ✓ Got integer value: {neuron_count}")
        
        # Test Config properties
        print(f"  ✓ Has keys: {len(config.keys)}")
        print(f"  ✓ Summary: {config.summary}")
        
        # Test Brain
        print("\nTesting Brain...")
        brain = nlm.Brain(config)
        print(f"  ✓ Created Brain: {brain}")
        
        # Test Pythonic property access
        print(f"  ✓ Total neurons: {brain.total_neurons}")
        print(f"  ✓ Total synapses: {brain.total_synapses}")
        print(f"  ✓ Firing neurons: {brain.firing_neurons}")
        print(f"  ✓ Initialized: {brain.initialized}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing Pythonic interface: {e}")
        traceback.print_exc()
        return False

def test_documentation_consistency():
    """Test that documentation examples work."""
    print("\nTesting Documentation Examples...")
    
    try:
        import nlm
        
        # Test Example 1: Simplest Brain
        print("\n  Testing Example 1: Simplest Brain")
        brain = nlm.Brain(nlm.createDefaultConfig())
        brain.initialize()
        brain.step(0)
        print(f"    ✓ Created and ran simple brain: {brain.total_spikes} spikes")
        
        # Test Example 2: Brain in Simple World
        print("\n  Testing Example 2: Brain in Simple World")
        brain = nlm.Brain(nlm.createDefaultConfig())
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        for step in range(10):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        print(f"    ✓ Created and ran agent-world simulation")
        
        # Test Configuration
        print("\n  Testing Configuration")
        config = nlm.Config()
        config.set("brain.neuron_count", 1000)
        config.set("plasticity.stdp.enable", True)
        
        brain = nlm.Brain(config)
        brain.initialize()
        print(f"    ✓ Created brain with custom config: {brain.total_neurons} neurons")
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing documentation examples: {e}")
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced features from documentation."""
    print("\nTesting Advanced Features...")
    
    try:
        import nlm
        
        # Test Context Manager
        print("\n  Testing Context Manager")
        config = nlm.Config()
        config.set("brain.neuron_count", 100)
        
        brain = nlm.Brain(config)
        brain.initialize()
        
        # Test simulation session
        with nlm.simulation_session(brain, steps=10, timestep=0.1) as session:
            session.run()
        print(f"    ✓ Context manager works")
        
        # Test Experiment Helper
        print("\n  Testing Experiment Helper")
        brain = nlm.Brain()
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        results = nlm.run_experiment(brain, world, agent, num_steps=50)
        print(f"    ✓ Experiment completed: {len(results['steps'])} steps")
        
        # Test Profiling
        print("\n  Testing Performance Profiling")
        brain = nlm.Brain()
        brain.initialize()
        
        profile = nlm.profile_simulation(brain, steps=100, timestep=0.1)
        print(f"    ✓ Profile collected: {profile['total_time']:.2f}s total")
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing advanced features: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling and validation."""
    print("\nTesting Error Handling...")
    
    try:
        import nlm
        
        # Test uninitialized brain error
        print("\n  Testing uninitialized brain error")
        brain = nlm.Brain()
        
        try:
            brain.step(0)
            print(f"    ✗ Expected error but none occurred")
            return False
        except nlm.ValidationError as e:
            print(f"    ✓ Correctly caught ValidationError: {e}")
        
        # Test initialized brain works
        brain.initialize()
        brain.step(0)
        print(f"    ✓ Initialized brain works correctly")
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing error handling: {e}")
        traceback.print_exc()
        return False

def main():
    """Main verification function."""
    print("NLM Improvements Verification Script")
    print("=" * 50)
    
    results = []
    
    # Run all tests
    results.append(("Pythonic Interface", test_pythonic_interface()))
    results.append(("Documentation Examples", test_documentation_consistency()))
    results.append(("Advanced Features", test_advanced_features()))
    results.append(("Error Handling", test_error_handling()))
    
    # Summary
    print("\n" + "=" * 50)
    print("VERIFICATION SUMMARY")
    print("=" * 50)
    
    passed = 0
    total = len(results)
    
    for test_name, passed_test in results:
        status = "✓ PASS" if passed_test else "✗ FAIL"
        print(f"{test_name:30} {status}")
        if passed_test:
            passed += 1
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed! NLM improvements are working correctly.")
        return 0
    else:
        print(f"\n❌ {total - passed} tests failed. Please review the issues above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
