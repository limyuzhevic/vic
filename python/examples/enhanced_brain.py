#!/usr/bin/env python3
"""
NLM Enhanced Brain Example

This example demonstrates enhanced brain features including context managers,
batch operations, and advanced APIs.
"""

import pynlm

def main():
    print("=== Enhanced Brain Features Example ===")
    
    # Create brain with experimental config
    print("1. Creating brain with experimental configuration...")
    config = pynlm.create_experimental_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Use context manager for session
    print("2. Running simulation with context manager...")
    with pynlm.simulation_session(brain, num_steps=500, timestep=0.001) as session:
        session.run()
    
    # Use batch operations
    print("3. Running batch operations...")
    brain.runSteps(1000)
    
    # Get advanced statistics
    print("4. Advanced brain features:")
    print(f"   - Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"   - Developmental stage: {brain.getDevelopmentalStage()}")
    
    stats = brain.getNetworkStats()
    print(f"   - Network neurons: {stats['total_neurons']}")
    print(f"   - Network synapses: {stats['total_synapses']}")
    
    print("=== Complete ===")

if __name__ == "__main__":
    main()