#!/usr/bin/env python3
import sys
import pynlm

def main():
    print("=== NLM Installation Verification ===")
    
    # Check version
    print(f"NLM Version: {pynlm.__version__}")
    
    # Test basic functionality
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simple simulation
    for step in range(10):
        brain.step(step)
    
    # Check results
    neurons = brain.getTotalNeuronCount()
    spikes = brain.getTotalSpikeCount()
    
    print(f"Test brain created: {neurons} neurons")
    print(f"Test simulation completed: {spikes} spikes")
    
    # Test Advanced API if available
    try:
        from pynlm.advanced_api import AdvancedAPI
        advanced_api = AdvancedAPI(brain)
        print("Advanced API available: Yes")
    except ImportError:
        print("Advanced API available: No")
    
    print("=== Installation Verification Complete ===")
    return 0

if __name__ == "__main__":
    sys.exit(main())
