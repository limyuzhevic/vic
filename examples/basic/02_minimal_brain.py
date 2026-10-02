#!/usr/bin/env python3
"""
Example 2: Minimal Brain

The absolute minimum example - does everything in 3 lines.
This shows the simplest possible way to create and use an NLM brain.
"""

import pynlm

def main():
    print("NLM Example 2: Minimal Brain")
    print("=" * 40)
    
    # ONE LINE to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # INITIALIZE it
    brain.initialize()
    
    # MAKE it active
    brain.step(0)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons!")
    print("This is the absolute minimum to get a working brain.")
    print("\nMinimal example completed successfully!")

if __name__ == "__main__":
    main()