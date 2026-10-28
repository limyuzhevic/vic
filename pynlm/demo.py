# NLM Demo Module
# High-level examples for getting started with NLM

import numpy as np
from pynlm import (
    create_brain, create_default_config, create_simple_world, 
    create_agent_brain, get_system_info
)

def main():
    """Run NLM demos."""
    print("=== NLM Demos ===\n")
    
    print("1. Basic Brain Demo")
    print("-" * 30)
    
    # Create and initialize brain
    brain = create_brain()
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    steps = 50
    print(f"\nRunning {steps} simulation steps...")
    for i in range(steps):
        brain.step(i)
        if i % 10 == 0:
            print(f"  Step {i}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"\nFinal Statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    print("\n2. Complete Agent Demo")
    print("-" * 30)
    
    # Run complete simulation
    config = {
        "neuron_count": 500,
        "world_width": 10,
        "world_height": 10,
        "enable_reward_modulation": True,
        "enable_curiosity": True,
        "enable_development": True,
    }
    
    results = run_complete_simulation(config, steps=200, verbose=True)
    
    print(f"\nSimulation Results:")
    print(f"  Duration: {results['duration']:.2f} seconds")
    print(f"  Final firing rate: {results['final_firing_rate']:.2f} Hz")
    print(f"  Development stage: {results['development_stage']}")
    print(f"  Neuromodulation: {results['final_neuromodulation']:.3f}")
    
    print("\n=== Demos Complete ===")

if __name__ == "__main__":
    main()