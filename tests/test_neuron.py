"""
Unit tests for NLM (Neural Learning Machine) - Neuron Class Tests

This file contains comprehensive unit tests for the improved Neuron class.
The tests verify that all the improvements we made are working correctly.
"""

import sys
import os

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

from nlm import Neuron, NeuronType, FiringState
from nlm import RandomGenerator, SimulationStep, Timestamp
from nlm import MembranePotential, TimestepDuration, SynapticWeight

def run_neuron_tests():
    """Run all neuron tests and return results."""
    print("Running Neuron Unit Tests...")
    
    total_tests = 0
    passed_tests = 0
    failed_tests = 0
    
    # Test 1: Neuron Creation
    print("\n1. Testing Neuron Creation...")
    try:
        neuron = Neuron(NeuronId(1))
        assert neuron.getId() == NeuronId(1)
        assert neuron.getType() == NeuronType.INTERNAL
        assert neuron.getMembranePotential() == -70.0f
        assert neuron.getThreshold() == -55.0f
        print("   ✓ Neuron Creation test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Neuron Creation test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 2: Neuron Type Setting
    print("\n2. Testing Neuron Type Setting...")
    try:
        neuron = Neuron(NeuronId(1))
        neuron.setType(NeuronType.EXCITATORY)
        assert neuron.getType() == NeuronType.EXCITATORY
        print("   ✓ Neuron Type Setting test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Neuron Type Setting test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 3: Membrane Potential Changes
    print("\n3. Testing Membrane Potential Changes...")
    try:
        neuron = Neuron(NeuronId(1))
        neuron.setMembranePotential(-65.0f)
        assert neuron.getMembranePotential() == -65.0f
        
        neuron.addToMembranePotential(5.0f)
        assert neuron.getMembranePotential() == -60.0f
        
        print("   ✓ Membrane Potential Changes test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Membrane Potential Changes test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 4: Current Injection
    print("\n4. Testing Current Injection...")
    try:
        neuron = Neuron(NeuronId(1))
        neuron.injectCurrent(10.0f)
        assert neuron.getTotalCurrent() == 10.0f
        
        neuron.clearTotalCurrent()
        assert neuron.getTotalCurrent() == 0.0f
        
        print("   ✓ Current Injection test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Current Injection test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 5: Plasticity Flags
    print("\n5. Testing Plasticity Flags...")
    try:
        neuron = Neuron(NeuronId(1))
        neuron.enablePlasticity(True, True, False)
        
        flags = neuron.getPlasticityFlags()
        assert flags.hebbian == True
        assert flags.stdp == True
        assert flags.reward_modulated == False
        
        print("   ✓ Plasticity Flags test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Plasticity Flags test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 6: Random Initialization
    print("\n6. Testing Random Initialization...")
    try:
        rng = RandomGenerator()
        rng.setSeed(42)
        
        neuron = Neuron(NeuronId(1))
        neuron.initializeRandom(rng)
        
        state = neuron.getState()
        
        # Check that all values are within reasonable ranges
        assert -100.0f <= state.membranePotential <= 50.0f
        assert -100.0f <= state.threshold <= -40.0f
        assert -100.0f <= state.restingPotential <= -50.0f
        assert 2 <= state.refractoryPeriod <= 10
        assert state.adaptationVariable == 0.0f
        assert state.lastSpikeTime == -1.0f
        
        print("   ✓ Random Initialization test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Random Initialization test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 7: Firing State
    print("\n7. Testing Firing State...")
    try:
        neuron = Neuron(NeuronId(1))
        
        assert not neuron.isFiring()
        assert not neuron.isRefractory()
        
        neuron.setFiringState(FiringState.ACTIVE)
        assert neuron.isFiring()
        
        neuron.setFiringState(FiringState.REFRACTORY)
        assert neuron.isRefractory()
        
        print("   ✓ Firing State test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Firing State test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 8: Spike History
    print("\n8. Testing Spike History...")
    try:
        neuron = Neuron(NeuronId(1))
        
        assert len(neuron.getSpikeHistory()) == 0
        
        neuron.recordSpike(Timestamp(100.0))
        assert len(neuron.getSpikeHistory()) == 1
        assert neuron.getSpikeHistory()[0] == Timestamp(100.0)
        
        neuron.clearSpikeHistory()
        assert len(neuron.getSpikeHistory()) == 0
        
        print("   ✓ Spike History test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Spike History test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 9: State Management
    print("\n9. Testing State Management...")
    try:
        neuron = Neuron(NeuronId(1))
        
        neuron.setMembranePotential(-65.0f)
        neuron.setThreshold(-50.0f)
        neuron.recordSpike(Timestamp(100.0))
        
        neuron.reset()
        
        assert neuron.getMembranePotential() == -70.0f
        assert neuron.getThreshold() == -55.0f
        assert len(neuron.getSpikeHistory()) == 0
        
        print("   ✓ State Management test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ State Management test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 10: Type Constants
    print("\n10. Testing Type Constants...")
    try:
        # Verify all type constants
        assert NeuronType.EXCITATORY.value == 0
        assert NeuronType.INHIBITORY.value == 1
        assert NeuronType.SENSORY.value == 2
        assert NeuronType.MOTOR.value == 3
        assert NeuronType.MODULATORY.value == 4
        assert NeuronType.INTERNAL.value == 5
        
        print("   ✓ Type Constants test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Type Constants test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 11: Firing State Constants
    print("\n11. Testing Firing State Constants...")
    try:
        # Verify firing state constants
        assert FiringState.RESTING.value == 0
        assert FiringState.ACTIVE.value == 1
        assert FiringState.REFRACTORY.value == 2
        
        print("   ✓ Firing State Constants test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Firing State Constants test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 12: Refractory Period
    print("\n12. Testing Refractory Period...")
    try:
        neuron = Neuron(NeuronId(1))
        
        neuron.setRefractoryPeriod(5)
        assert neuron.getRefractoryPeriod() == 5
        
        assert not neuron.isRefractory()
        
        print("   ✓ Refractory Period test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Refractory Period test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 13: Edge Cases
    print("\n13. Testing Edge Cases...")
    try:
        neuron = Neuron(NeuronId(1))
        
        # Test extreme current values
        neuron.injectCurrent(1000.0f)
        assert neuron.getTotalCurrent() == 1000.0f
        
        neuron.clearTotalCurrent()
        assert neuron.getTotalCurrent() == 0.0f
        
        # Test zero current
        neuron.injectCurrent(0.0f)
        assert neuron.getTotalCurrent() == 0.0f
        
        # Test empty spike history
        assert len(neuron.getSpikeHistory()) == 0
        neuron.clearSpikeHistory()
        assert len(neuron.getSpikeHistory()) == 0
        
        print("   ✓ Edge Cases test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Edge Cases test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 14: State Isolation
    print("\n14. Testing State Isolation...")
    try:
        neuron1 = Neuron(NeuronId(1))
        neuron2 = Neuron(NeuronId(2))
        
        neuron1.setMembranePotential(-65.0f)
        neuron1.setThreshold(-50.0f)
        
        assert neuron2.getMembranePotential() == -70.0f
        assert neuron2.getThreshold() == -55.0f
        
        print("   ✓ State Isolation test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ State Isolation test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Test 15: Type Comparison
    print("\n15. Testing Type Comparison...")
    try:
        neuron1 = Neuron(NeuronId(1))
        neuron2 = Neuron(NeuronId(2))
        
        neuron1.setType(NeuronType.EXCITATORY)
        neuron2.setType(NeuronType.INHIBITORY)
        
        assert neuron1.getType() != neuron2.getType()
        
        print("   ✓ Type Comparison test passed")
        total_tests += 1
        passed_tests += 1
    except Exception as e:
        print(f"   ✗ Type Comparison test failed: {e}")
        total_tests += 1
        failed_tests += 1
    
    # Print summary
    print("\n" + "="*60)
    print("Neuron Unit Tests Summary")
    print("="*60)
    print(f"Tests run: {total_tests}")
    print(f"Tests passed: {passed_tests}")
    print(f"Tests failed: {failed_tests}")
    print(f"Success rate: {passed_tests/max(1, total_tests)*100:.1f}%")
    
    if failed_tests > 0:
        print(f"\n❌ {failed_tests} test(s) failed!")
        return {
            'count': total_tests,
            'passed': passed_tests,
            'failed': failed_tests,
            'time': 0.0,
            'success': False
        }
    else:
        print(f"\n✅ All {total_tests} tests passed successfully!")
        return {
            'count': total_tests,
            'passed': passed_tests,
            'failed': failed_tests,
            'time': 0.0,
            'success': True
        }
if __name__ == "__main__":
    # Run the tests
    test_result = run_neuron_tests()
    
    # Exit with appropriate code
    sys.exit(0 if test_result['success'] else 1)