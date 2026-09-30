# Simplest Brain Example
"""
This is the absolute simplest way to create and use an NLM brain.
This matches Example 1 from easy_usage.md but with enhanced features.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Simplest Brain Example ===")
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print("Your brain has", brain.neuron_count, "neurons!")
    print(f"Total synapses: {brain.synapse_count}")
    print(f"Firing neurons: {brain.firing_count}")
    
    # Example of enhanced features
    metrics = brain.computeMetrics()
    print(f"System metrics: {metrics}")

if __name__ == "__main__":
    main()
