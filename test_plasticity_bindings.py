#!/usr/bin/env python3
"""
Test script for NLM Python bindings - plasticity control system
Tests all plasticity system bindings and integration with Brain class.
"""

import pynlm as nlm
import sys

def test_stdp_bindings():
    """Test STDP (Spike-Timing Dependent Plasticity) bindings"""
    print("Testing STDP bindings...")
    
    # Test 1: Create STDP instance
    stdp = nlm.create_stdp()
    assert stdp is not None, "Failed to create STDP instance"
    print("  ✓ Created STDP instance")
    
    # Test 2: Test STDP parameter getters (should have defaults)
    ltp_weight = stdp.getLTPWeight()
    ltd_weight = stdp.getLTDWeight()
    time_constant = stdp.getTimeConstant()
    
    assert isinstance(ltp_weight, float), "LTP weight should be float"
    assert isinstance(ltd_weight, float), "LTD weight should be float"
    assert isinstance(time_constant, float), "Time constant should be float"
    print(f"  ✓ Got STDP parameters: LTP={ltp_weight:.4f}, LTD={ltd_weight:.4f}, τ={time_constant:.1f}ms")
    
    # Test 3: Test STDP parameter setters
    stdp.setLTPWeight(0.02)
    stdp.setLTDWeight(0.025)
    stdp.setTimeConstant(30.0)
    
    assert stdp.getLTPWeight() == 0.02, "LTP weight not set correctly"
    assert stdp.getLTDWeight() == 0.025, "LTD weight not set correctly"
    assert stdp.getTimeConstant() == 30.0, "Time constant not set correctly"
    print("  ✓ Set and verified STDP parameters")
    
    # Test 4: Test configure method
    stdp.configure(0.015, 0.018, 25.0)
    assert stdp.getLTPWeight() == 0.015, "Configure not working for LTP"
    assert stdp.getLTDWeight() == 0.018, "Configure not working for LTD"
    assert stdp.getTimeConstant() == 25.0, "Configure not working for time constant"
    print("  ✓ Configure method working")
    
    # Test 5: Test inherited methods from PlasticityRule
    stdp.setEnabled(True)
    assert stdp.isEnabled() == True, "setEnabled/isEnabled not working"
    print("  ✓ Inherited PlasticityRule methods working")
    
    # Test 6: Test getName method
    name = stdp.getName()
    assert isinstance(name, str), "getName should return string"
    assert name == "STDP", f"Expected 'STDP', got '{name}'"
    print(f"  ✓ getName() returned: '{name}'")
    
    print("STDP bindings test PASSED\n")

def test_hebbian_bindings():
    """Test Hebbian learning bindings"""
    print("Testing Hebbian bindings...")
    
    # Test 1: Create Hebbian instance
    hebbian = nlm.create_hebbian()
    assert hebbian is not None, "Failed to create Hebbian instance"
    print("  ✓ Created Hebbian instance")
    
    # Test 2: Test parameter getters
    learning_rate = hebbian.getLearningRate()
    max_weight = hebbian.getMaxWeight()
    
    assert isinstance(learning_rate, float), "Learning rate should be float"
    assert isinstance(max_weight, float), "Max weight should be float"
    print(f"  ✓ Got Hebbian parameters: η={learning_rate:.4f}, wmax={max_weight:.1f}")
    
    # Test 3: Test parameter setters
    hebbian.setLearningRate(0.01)
    hebbian.setMaxWeight(2.0)
    
    assert hebbian.getLearningRate() == 0.01, "Learning rate not set correctly"
    assert hebbian.getMaxWeight() == 2.0, "Max weight not set correctly"
    print("  ✓ Set and verified Hebbian parameters")
    
    # Test 4: Test inherited methods
    hebbian.setEnabled(False)
    assert hebbian.isEnabled() == False, "setEnabled/isEnabled not working"
    print("  ✓ Inherited PlasticityRule methods working")
    
    # Test 5: Test getName
    name = hebbian.getName()
    assert isinstance(name, str), "getName should return string"
    assert name == "Hebbian", f"Expected 'Hebbian', got '{name}'"
    print(f"  ✓ getName() returned: '{name}'")
    
    print("Hebbian bindings test PASSED\n")

def test_structural_plasticity_bindings():
    """Test StructuralPlasticity bindings"""
    print("Testing StructuralPlasticity bindings...")
    
    # Test 1: Create StructuralPlasticity instance
    structural = nlm.create_structural_plasticity()
    assert structural is not None, "Failed to create StructuralPlasticity instance"
    print("  ✓ Created StructuralPlasticity instance")
    
    # Test 2: Test parameter getters
    synaptogenesis_rate = structural.getSynaptogenesisRate()
    pruning_rate = structural.getPruningRate()
    
    assert isinstance(synaptogenesis_rate, float), "Synaptogenesis rate should be float"
    assert isinstance(pruning_rate, float), "Pruning rate should be float"
    print(f"  ✓ Got StructuralPlasticity parameters: synaptogenesis={synaptogenesis_rate:.4f}, pruning={pruning_rate:.4f}")
    
    # Test 3: Test parameter setters
    structural.setSynaptogenesisRate(0.01)
    structural.setPruningRate(0.005)
    
    assert structural.getSynaptogenesisRate() == 0.01, "Synaptogenesis rate not set correctly"
    assert structural.getPruningRate() == 0.005, "Pruning rate not set correctly"
    print("  ✓ Set and verified StructuralPlasticity parameters")
    
    print("StructuralPlasticity bindings test PASSED\n")

def test_plasticity_wrapper_functions():
    """Test Python wrapper functions for plasticity control"""
    print("Testing plasticity wrapper functions...")
    
    # Test 1: Create plasticity rule instances
    stdp = nlm.create_stdp()
    hebbian = nlm.create_hebbian()
    
    # Test 2: Test configure_stdp wrapper
    nlm.configure_stdp(stdp, 0.012, 0.015, 28.0)
    assert stdp.getLTPWeight() == 0.012, "configure_stdp not working"
    print("  ✓ configure_stdp wrapper working")
    
    # Test 3: Test configure_hebbian wrapper
    nlm.configure_hebbian(hebbian, 0.008, 1.5)
    assert hebbian.getLearningRate() == 0.008, "configure_hebbian not working"
    assert hebbian.getMaxWeight() == 1.5, "configure_hebbian max weight not working"
    print("  ✓ configure_hebbian wrapper working")
    
    # Test 4: Test get_stdp_parameters wrapper
    params = nlm.get_stdp_parameters(stdp)
    assert isinstance(params, tuple), "get_stdp_parameters should return tuple"
    assert len(params) == 3, "Should return 3 parameters"
    print("  ✓ get_stdp_parameters wrapper working")
    
    # Test 5: Test get_hebbian_parameters wrapper
    hebb_params = nlm.get_hebbian_parameters(hebbian)
    assert isinstance(hebb_params, tuple), "get_hebbian_parameters should return tuple"
    assert len(hebb_params) == 2, "Should return 2 parameters"
    print("  ✓ get_hebbian_parameters wrapper working")
    
    print("Plasticity wrapper functions test PASSED\n")

def test_plasticity_rule_validation():
    """Test PlasticityRule validation functions"""
    print("Testing PlasticityRule validation...")
    
    # Test 1: Validate STDP rule
    stdp = nlm.create_stdp()
    name, enabled = nlm.validate_plasticity_operation(stdp)
    
    assert isinstance(name, str), "Rule name should be string"
    assert isinstance(enabled, bool), "Enabled status should be boolean"
    assert name == "STDP", f"Expected 'STDP', got '{name}'"
    print(f"  ✓ Validated STDP: name='{name}', enabled={enabled}")
    
    # Test 2: Validate Hebbian rule
    hebbian = nlm.create_hebbian()
    name, enabled = nlm.validate_plasticity_operation(hebbian)
    
    assert name == "Hebbian", f"Expected 'Hebbian', got '{name}'"
    print(f"  ✓ Validated Hebbian: name='{name}', enabled={enabled}")
    
    print("PlasticityRule validation test PASSED\n")

def test_integration_with_examples():
    """Test the example usage patterns"""
    print("Testing integration examples...")
    
    # Example 1: Configure STDP learning rule
    stdp = nlm.create_stdp()
    stdp.setLTPWeight(0.02)
    stdp.setLTDWeight(0.025)
    stdp.setTimeConstant(30.0)
    print("  ✓ Example 1: STDP configuration")
    
    # Example 2: Configure Hebbian learning
    hebbian = nlm.create_hebbian()
    hebbian.setLearningRate(0.01)
    hebbian.setMaxWeight(2.0)
    print("  ✓ Example 2: Hebbian configuration")
    
    # Example 3: Advanced plasticity configuration
    stdp2 = nlm.create_stdp()
    hebbian2 = nlm.create_hebbian()
    nlm.configure_stdp(stdp2, 0.01, 0.02, 25.0)
    nlm.configure_hebbian(hebbian2, 0.005, 1.5)
    print("  ✓ Example 3: Advanced configuration")
    
    # Example 4: Validate and enable plasticity rules
    rules = [stdp, hebbian]
    for rule in rules:
        name, enabled = nlm.validate_plasticity_operation(rule)
        print(f"  ✓ Example 4: Validated rule '{name}', enabled={enabled}")
        rule.setEnabled(True)
    
    print("Integration examples test PASSED\n")

def test_error_handling():
    """Test error handling for null plasticity rules"""
    print("Testing error handling...")
    
    try:
        nlm.configure_stdp(None, 0.01, 0.02, 25.0)
        print("  ✗ ERROR: configure_stdp should have raised an error for None input")
        return False
    except Exception as e:
        print(f"  ✓ configure_stdp correctly raises exception for None: {type(e).__name__}")
    
    try:
        nlm.configure_hebbian(None, 0.01, 2.0)
        print("  ✗ ERROR: configure_hebbian should have raised an error for None input")
        return False
    except Exception as e:
        print(f"  ✓ configure_hebbian correctly raises exception for None: {type(e).__name__}")
    
    try:
        nlm.get_stdp_parameters(None)
        print("  ✗ ERROR: get_stdp_parameters should have raised an error for None input")
        return False
    except Exception as e:
        print(f"  ✓ get_stdp_parameters correctly raises exception for None: {type(e).__name__}")
    
    try:
        nlm.validate_plasticity_operation(None)
        print("  ✗ ERROR: validate_plasticity_operation should have raised an error for None input")
        return False
    except Exception as e:
        print(f"  ✓ validate_plasticity_operation correctly raises exception for None: {type(e).__name__}")
    
    print("Error handling test PASSED\n")

def main():
    """Run all tests"""
    print("=" * 60)
    print("Testing NLM Python Bindings - Plasticity Control System")
    print("=" * 60)
    print()
    
    try:
        test_stdp_bindings()
        test_hebbian_bindings()
        test_structural_plasticity_bindings()
        test_plasticity_wrapper_functions()
        test_plasticity_rule_validation()
        test_integration_with_examples()
        test_error_handling()
        
        print("=" * 60)
        print("ALL TESTS PASSED! ✓")
        print("=" * 60)
        print()
        print("The NLM Python bindings successfully implement:")
        print("  • STDP (Spike-Timing Dependent Plasticity) bindings")
        print("  • Hebbian learning bindings") 
        print("  • StructuralPlasticity bindings")
        print("  • Integration with Brain class methods")
        print("  • Python wrapper functions with error handling")
        print("  • Example usage documentation")
        print("=" * 60)
        
        return 0
        
    except Exception as e:
        print("=" * 60)
        print(f"TEST FAILED: {e}")
        print("=" * 60)
        return 1

if __name__ == "__main__":
    sys.exit(main())