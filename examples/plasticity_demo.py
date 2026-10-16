#!/usr/bin/env python3
"""Demonstrate STDP (Spike-Timing-Dependent Plasticity) learning.

This example shows how STDP rules work in NLM:
- Synaptic strengthening when presynaptic fires before postsynaptic
- Synaptic weakening when postsynaptic fires before presynaptic
"""

import pynlm
import sys

def demonstrate_stdp():
    print("=== NLM STDP Plasticity Demo ===\n")
    
    # 1. Create brain with STDP
    print("Step 1: Setting up STDP learning")
    config = pynlm.createDefaultConfig()
    
    # Configure STDP parameters
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    stdp = brain.getSTDP()
    print(f"✓ STDP configured: LTP={stdp.getLTPWeight():.3f}, "
          f"LTD={stdp.getLTDWeight():.3f}, tau={stdp.getTimeConstant():.1f}ms")
    print()
    
    # 2. Show default STDP behavior
    print("Step 2: Demonstrating STDP learning effects")
    
    # Simulate some learning scenarios
    for i in range(10):
        brain.step(i)
        
        if i % 5 == 0:
            firing = brain.getFiringNeuronCount()
            print(f"  Step {i}: {firing} neurons firing")
    
    print()
    
    # 3. Test STDP parameter tuning
    print("Step 3: Parameter tuning demonstration")
    
    # Different STDP configurations
    configurations = [
        (0.01, 0.01, 10.0, "Conservative"),
        (0.05, 0.05, 25.0, "Balanced"),
        (0.1, 0.05, 50.0, "Strong LTP"),
    ]
    
    for ltp, ltd, tau, name in configurations:
        stdp.configure(ltp, ltd, tau)
        print(f"  {name}: LTP={stdp.getLTPWeight():.3f}, "
              f"LTD={stdp.getLTDWeight():.3f}, tau={stdp.getTimeConstant():.1f}ms")
    
    print()
    
    # 4. Demonstrate synaptic strengthening
    print("Step 4: Synaptic dynamics")
    
    # Create a custom STDP instance
    stdp_custom = pynlm.STDP()
    stdp_custom.configure(0.08, 0.04, 30.0)
    
    print(f"Custom STDP settings:")
    print(f"  LTP weight (strengthening): {stdp_custom.getLTPWeight():.3f}")
    print(f"  LTD weight (weakening): {stdp_custom.getLTDWeight():.3f}")
    print(f"  Time constant (memory): {stdp_custom.getTimeConstant():.1f} ms")
    
    # Show that LTP > LTD means net strengthening
    net_effect = stdp_custom.getLTPWeight() - stdp_custom.getLTDWeight()
    print(f"  Net strengthening effect: {net_effect:.3f}")
    print()
    
    # 5. Factory function comparison
    print("Step 5: Factory function comparison")
    
    stdp_from_factory = pynlm.createSTDP()
    stdp_from_factory.configure(0.03, 0.03, 15.0)
    
    print(f"Factory-created STDP: LTP={stdp_from_factory.getLTPWeight():.3f}, "
          f"LTD={stdp_from_factory.getLTDWeight():.3f}, "
          f"tau={stdp_from_factory.getTimeConstant():.1f}ms")
    
    print()
    print("=== STDP Demo Complete ===")

if __name__ == "__main__":
    try:
        demonstrate_stdp()
    except ImportError as e:
        print(f"Error importing pynlm: {e}")
        print("Make sure NLM Python bindings are installed.")
        sys.exit(1)
    except Exception as e:
        print(f"Error running STDP demo: {e}")
        sys.exit(1)
