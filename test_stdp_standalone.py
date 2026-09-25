#!/usr/bin/env python3
"""
Standalone STDP implementation test in Python
This tests the STDP logic without requiring C++ compilation
"""

import math
import numpy as np

class MockSynapse:
    def __init__(self):
        self.weight = 0.5
        self.efficacy = 1.0
        self.eligibility_trace = 0.0
    
    def get_weight(self):
        return self.weight
    
    def set_weight(self, w):
        self.weight = w
    
    def get_efficacy(self):
        return self.efficacy
    
    def get_eligibility_trace(self):
        return self.eligibility_trace
    
    def set_eligibility_trace(self, trace):
        self.eligibility_trace = trace
    
    def decay_eligibility_trace(self, decay_rate):
        self.eligibility_trace *= (1.0 - decay_rate)

class MockSTDP:
    def __init__(self):
        self.ltp_weight = 0.01
        self.ltd_weight = 0.012
        self.tau_plus = 20.0
        self.tau_minus = 30.0
        self.min_weight = -1.0
        self.max_weight = 1.0
        self.eligibility_decay = 0.05
    
    def update(self, synapse, pre_spikes, post_spikes, dt):
        if not pre_spikes or not post_spikes:
            return
        
        total_delta = 0.0
        
        for pre_time in pre_spikes:
            for post_time in post_spikes:
                delta_t = post_time - pre_time  # Δt in ms
                
                if delta_t > 0:
                    # Pre before post: POTENTIATION
                    delta = self.ltp_weight * math.exp(-delta_t / self.tau_plus)
                    total_delta += delta
                elif delta_t < 0:
                    # Post before pre: DEPRESSION
                    delta = -self.ltd_weight * math.exp(delta_t / self.tau_minus)
                    total_delta += delta
        
        if abs(total_delta) > 1e-6:
            # Scale by synaptic efficacy
            total_delta *= synapse.get_efficacy()
            
            # Apply weight change with bounds
            new_weight = synapse.get_weight() + total_delta
            new_weight = max(self.min_weight, min(self.max_weight, new_weight))
            synapse.set_weight(new_weight)
            
            # Update eligibility trace
            current_trace = synapse.get_eligibility_trace()
            synapse.set_eligibility_trace(current_trace + total_delta)
            
            # Apply trace decay
            synapse.decay_eligibility_trace(self.eligibility_decay)
    
    def get_ltp_weight(self):
        return self.ltp_weight
    
    def get_ltd_weight(self):
        return self.ltd_weight
    
    def get_time_constant(self):
        return (self.tau_plus + self.tau_minus) * 0.5

def test_creation():
    stdp = MockSTDP()
    assert stdp.get_ltp_weight() == 0.01
    assert stdp.get_ltd_weight() == 0.012
    assert stdp.get_time_constant() == 25.0

def test_potentiation():
    stdp = MockSTDP()
    synapse = MockSynapse()
    
    initial_weight = synapse.get_weight()
    
    # Pre fires before post (positive dt) - should potentiate
    pre_spikes = [0.0, 10.0, 20.0]  # ms
    post_spikes = [5.0, 15.0, 25.0]  # ms (10ms after pre)
    
    stdp.update(synapse, pre_spikes, post_spikes, 0.001)
    
    final_weight = synapse.get_weight()
    
    # Weight should have increased due to pre-before-post
    assert final_weight > initial_weight, f"Weight should increase, got {final_weight} from {initial_weight}"

def test_depression():
    stdp = MockSTDP()
    synapse = MockSynapse()
    
    initial_weight = synapse.get_weight()
    
    # Post fires before pre (negative dt) - should depress
    pre_spikes = [10.0, 20.0, 30.0]
    post_spikes = [5.0, 15.0, 25.0]  # 5ms before pre
    
    stdp.update(synapse, pre_spikes, post_spikes, 0.001)
    
    final_weight = synapse.get_weight()
    
    # Weight should have decreased due to post-before-pre
    assert final_weight < initial_weight, f"Weight should decrease, got {final_weight} from {initial_weight}"

def test_empty_spikes():
    stdp = MockSTDP()
    synapse = MockSynapse()
    
    initial_weight = synapse.get_weight()
    
    # Empty pre spikes - should not change
    pre_spikes = []
    post_spikes = [1.0, 2.0]
    
    stdp.update(synapse, pre_spikes, post_spikes, 0.001)
    assert synapse.get_weight() == initial_weight, f"Weight should not change with empty pre-spikes"
    
    # Empty post spikes - should not change
    pre_spikes = [1.0, 2.0]
    post_spikes = []
    stdp.update(synapse, pre_spikes, post_spikes, 0.001)
    assert synapse.get_weight() == initial_weight, f"Weight should not change with empty post-spikes"

def test_weight_bounds():
    stdp = MockSTDP()
    synapse = MockSynapse()
    synapse.set_weight(0.99)  # Near max
    
    # Very strong potentiation
    pre_spikes = [0.0, 1.0, 2.0, 3.0, 4.0]
    post_spikes = [1.0, 2.0, 3.0, 4.0, 5.0]
    
    stdp.update(synapse, pre_spikes, post_spikes, 0.001)
    
    # Weight should be clamped to max (1.0)
    assert synapse.get_weight() <= 1.0, f"Weight should be clamped to 1.0, got {synapse.get_weight()}"

def test_biological_parameters():
    """Test that the biological parameters are reasonable"""
    stdp = MockSTDP()
    
    # Check that LTP and LTD weights are in reasonable range
    assert 0.001 <= stdp.get_ltp_weight() <= 0.1, f"LTP weight {stdp.get_ltp_weight()} should be in range [0.001, 0.1]"
    assert 0.001 <= stdp.get_ltd_weight() <= 0.1, f"LTD weight {stdp.get_ltd_weight()} should be in range [0.001, 0.1]"
    
    # Check that time constants are in biologically realistic range
    assert 10.0 <= stdp.tau_plus <= 100.0, f"tau_plus {stdp.tau_plus} should be in range [10, 100]"
    assert 10.0 <= stdp.tau_minus <= 100.0, f"tau_minus {stdp.tau_minus} should be in range [10, 100]"
    
    # Check that weight bounds are reasonable
    assert -10.0 <= stdp.min_weight <= 0.0, f"min_weight {stdp.min_weight} should be in range [-10, 0]"
    assert 0.0 <= stdp.max_weight <= 10.0, f"max_weight {stdp.max_weight} should be in range [0, 10]"

def test_asymmetric_window():
    """Test that the STDP window is asymmetric (LTP for pre-before-post, LTD for post-before-pre)"""
    stdp = MockSTDP()
    synapse1 = MockSynapse()
    synapse2 = MockSynapse()
    
    # Set up with same initial weight
    synapse1.set_weight(0.5)
    synapse2.set_weight(0.5)
    
    # Pre before post (positive dt)
    pre_spikes = [0.0]
    post_spikes = [10.0]  # 10ms after pre
    stdp.update(synapse1, pre_spikes, post_spikes, 0.001)
    
    # Post before pre (negative dt)
    pre_spikes = [0.0]
    post_spikes = [-10.0]  # 10ms before pre
    stdp.update(synapse2, pre_spikes, post_spikes, 0.001)
    
    # LTP should be positive (weight increase)
    ltp_effect = synapse1.get_weight() - 0.5
    assert ltp_effect > 0, f"LTP should be positive, got {ltp_effect}"
    
    # LTD should be negative (weight decrease)
    ltd_effect = synapse2.get_weight() - 0.5
    assert ltd_effect < 0, f"LTD should be negative, got {ltd_effect}"

def test_eligibility_trace():
    """Test that eligibility traces are updated correctly"""
    stdp = MockSTDP()
    synapse = MockSynapse()
    
    # Initially zero
    assert synapse.get_eligibility_trace() == 0.0
    
    # Create some spike activity
    pre_spikes = [0.0]
    post_spikes = [10.0]
    
    stdp.update(synapse, pre_spikes, post_spikes, 0.001)
    
    # After update, eligibility trace should be non-zero
    # (it accumulates weight changes)
    assert synapse.get_eligibility_trace() != 0.0

def main():
    print("=== Running Standalone STDP Implementation Tests ===")
    
    tests = [
        ("Creation", test_creation),
        ("Potentiation", test_potentiation),
        ("Depression", test_depression),
        ("Empty Spikes", test_empty_spikes),
        ("Weight Bounds", test_weight_bounds),
        ("Biological Parameters", test_biological_parameters),
        ("Asymmetric Window", test_asymmetric_window),
        ("Eligibility Trace", test_eligibility_trace),
    ]
    
    failed_tests = []
    
    for test_name, test_func in tests:
        try:
            test_func()
            print(f"✓ {test_name} PASSED")
        except AssertionError as e:
            print(f"✗ {test_name} FAILED: {e}")
            failed_tests.append(test_name)
        except Exception as e:
            print(f"✗ {test_name} FAILED with exception: {e}")
            failed_tests.append(test_name)
    
    print()
    if failed_tests:
        print(f"=== {len(failed_tests)} Tests FAILED ===")
        print("Failed tests:", ", ".join(failed_tests))
        return 1
    else:
        print("=== All Tests PASSED ===")
        return 0

if __name__ == "__main__":
    exit(main())