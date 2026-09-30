# Silent Brain Example
"""
Example showing a brain simulation with minimal interaction.
This matches Example 2 from easy_usage.md with enhanced features.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Silent Brain Example ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(10):
        brain.step(i)
    
    print("Silent brain test done!")
    print(f"Final spikes: {brain.total_spikes}")
    print(f"Average firing rate: {brain.average_firing_rate}")

if __name__ == "__main__":
    main()
