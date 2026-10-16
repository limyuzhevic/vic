#!/usr/bin/env python3
"""Test script for STDP Python bindings"""

import sys
import os

# Add the current directory to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Try to import the module
try:
    from pynlm import STDP, createSTDP, createBrain, createDefaultConfig
    print("Successfully imported STDP bindings")
except ImportError as e:
    print(f"Failed to import STDP bindings: {e}")
    sys.exit(1)

# Test 1: Create an STDP instance
print("\nTest 1: Creating STDP instance")
try:
    stdp = STDP()
    print("✓ STDP instance created successfully")
except Exception as e:
    print(f"✗ Failed to create STDP instance: {e}")
    sys.exit(1)

# Test 2: Test default values
print("\nTest 2: Testing default values")
try:
    ltp_weight = stdp.getLTPWeight()
    ltd_weight = stdp.getLTDWeight()
    tau = stdp.getTimeConstant()
    print(f"✓ Default LTP weight: {ltp_weight}")
    print(f"✓ Default LTD weight: {ltd_weight}")
    print(f"✓ Default time constant (tau): {tau}")
except Exception as e:
    print(f"✗ Failed to get default values: {e}")
    sys.exit(1)

# Test 3: Test setLTPWeight
print("\nTest 3: Testing setLTPWeight")
try:
    stdp.setLTPWeight(0.05)
    new_ltp_weight = stdp.getLTPWeight()
    print(f"✓ LTP weight set to 0.05, retrieved: {new_ltp_weight}")
except Exception as e:
    print(f"✗ Failed to set/get LTP weight: {e}")
    sys.exit(1)

# Test 4: Test setLTDWeight
print("\nTest 4: Testing setLTDWeight")
try:
    stdp.setLTDWeight(0.06)
    new_ltd_weight = stdp.getLTDWeight()
    print(f"✓ LTD weight set to 0.06, retrieved: {new_ltd_weight}")
except Exception as e:
    print(f"✗ Failed to set/get LTD weight: {e}")
    sys.exit(1)

# Test 5: Test setTimeConstant
print("\nTest 5: Testing setTimeConstant")
try:
    stdp.setTimeConstant(25.0)
    new_tau = stdp.getTimeConstant()
    print(f"✓ Time constant set to 25.0, retrieved: {new_tau}")
except Exception as e:
    print(f"✗ Failed to set/get time constant: {e}")
    sys.exit(1)

# Test 6: Test configure
print("\nTest 6: Testing configure")
try:
    stdp.configure(0.03, 0.04, 30.0)
    ltp_weight = stdp.getLTPWeight()
    ltd_weight = stdp.getLTDWeight()
    tau = stdp.getTimeConstant()
    print(f"✓ Configured with (0.03, 0.04, 30.0)")
    print(f"✓ Retrieved: LTP={ltp_weight}, LTD={ltd_weight}, tau={tau}")
except Exception as e:
    print(f"✗ Failed to configure STDP: {e}")
    sys.exit(1)

# Test 7: Test createSTDP factory function
print("\nTest 7: Testing createSTDP factory function")
try:
    stdp2 = createSTDP()
    print("✓ createSTDP() factory function works")
    # Test a quick configuration
    stdp2.configure(0.015, 0.017, 18.0)
    print(f"✓ Configured stdp2: LTP={stdp2.getLTPWeight()}, LTD={stdp2.getLTDWeight()}, tau={stdp2.getTimeConstant()}")
except Exception as e:
    print(f"✗ Failed createSTDP factory function: {e}")
    sys.exit(1)

# Test 8: Test Brain with STDP access
print("\nTest 8: Testing Brain with STDP access")
try:
    config = createDefaultConfig()
    brain = createBrain(config)
    print("✓ Brain created successfully")
    
    # Check if Brain has STDP access
    if hasattr(brain, 'getSTDP'):
        stdp_from_brain = brain.getSTDP()
        print("✓ Brain.getSTDP() method exists and returns an STDP object")
        print(f"✓ STDP from brain configured: LTP={stdp_from_brain.getLTPWeight()}, LTD={stdp_from_brain.getLTDWeight()}, tau={stdp_from_brain.getTimeConstant()}")
    else:
        print("✗ Brain.getSTDP() method not found")
        sys.exit(1)
except Exception as e:
    print(f"✗ Failed to test Brain with STDP: {e}")
    sys.exit(1)

# Test 9: Test parameter validation (weights should be clamped)
print("\nTest 9: Testing parameter validation (weight clamping)")
try:
    # Test weight clamping
    stdp.setLTPWeight(2.0)  # Should be clamped to 1.0
    if stdp.getLTPWeight() <= 1.0:
        print("✓ LTP weight correctly clamped to max 1.0")
    else:
        print(f"✗ LTP weight not clamped: {stdp.getLTPWeight()}")
        sys.exit(1)
    
    stdp.setLTDWeight(-0.5)  # Should be clamped to 0.0
    if stdp.getLTDWeight() >= 0.0:
        print("✓ LTD weight correctly clamped to min 0.0")
    else:
        print(f"✗ LTD weight not clamped: {stdp.getLTDWeight()}")
        sys.exit(1)
        
    stdp.setTimeConstant(200.0)  # Should be clamped to 100.0
    if stdp.getTimeConstant() <= 100.0:
        print("✓ Time constant correctly clamped to max 100.0")
    else:
        print(f"✗ Time constant not clamped: {stdp.getTimeConstant()}")
        sys.exit(1)
except Exception as e:
    print(f"✗ Failed parameter validation test: {e}")
    sys.exit(1)

print("\n" + "="*60)
print("All STDP Python binding tests passed successfully!")
print("="*60)