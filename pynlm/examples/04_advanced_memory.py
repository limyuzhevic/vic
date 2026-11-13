#!/usr/bin/env python3
"""
Example 4: Advanced Memory and Cognitive Features

This example demonstrates the advanced memory and cognitive features of NLM,
including working memory, episodic memory, associative memory, and cognitive functions.
"""

import pynlm
import time

def create_advanced_memory_example():
    """Create an example demonstrating advanced memory features."""
    print("=== Advanced Memory and Cognitive Features Example ===")
    
    # Create brain with configuration
    print("1. Creating brain with configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 4)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get memory systems
    print("2. Accessing memory systems...")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"   Working memory: {working_memory}")
    print(f"   Episodic memory: {episodic_memory}")
    print(f"   Associative memory: {associative_memory}")
    
    # Get cognitive systems
    print("3. Accessing cognitive systems...")
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    print(f"   Neural planner: {planner}")
    print(f"   Concept formation: {concept_formation}")
    print(f"   Attentional selection: {attention}")
    
    # Get neuromodulation systems
    print("4. Accessing neuromodulation systems...")
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    print(f"   Dopamine: {dopamine}")
    print(f"   Curiosity: {curiosity}")
    print(f"   Novelty: {novelty}")
    print(f"   Prediction error: {prediction_error}")
    
    # Run simulation
    print("5. Running simulation...")
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"   Step {step}:")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Working memory traces: {working_memory.getActiveTraces() if working_memory else 'N/A'}")
    
    # Get final statistics
    print("6. Final statistics:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Active neurons: {brain.getActiveNeuronCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain

if __name__ == "__main__":
    brain = create_advanced_memory_example()
    print("\n✓ Advanced memory example completed successfully!")