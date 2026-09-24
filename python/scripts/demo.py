#!/usr/bin/env python3
"""
NLM Demo Script

This script provides example usage of the NLM Python API.
Run this script to see NLM in action with various examples.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from pynlm import createBrain, createSimpleWorld, createAgentBrain
from pynlm import createDefaultConfig
import time


def demo_basic_simulation():
    """Demonstrate basic neural simulation."""
    print("=== Basic Neural Simulation Demo ===")
    
    # Create brain with default configuration
    brain = createBrain()
    brain.initialize()
    
    print(f"Brain initialized: {brain}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation for 10 steps
    print("\nRunning simulation...")
    for i in range(10):
        brain.step(i)
        if i % 2 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    print("Simulation complete!\n")


def demo_agent_world():
    """Demonstrate agent in world simulation."""
    print("=== Agent in World Demo ===")
    
    # Setup
    brain = createBrain()
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Agent initialized: {agent}")
    print(f"World configured: {world.getWidth()}x{world.getHeight()} cells")
    
    # Run simulation
    print("\nRunning agent-world simulation...")
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        print(f"Step {step}: Action={action.getType()}, "
              f"Firing neurons={brain.getFiringNeuronCount()}, "
              f"Total spikes={brain.getTotalSpikeCount()}")
        
        # Apply action (in this case, just print what would happen)
        # In a real simulation, this would move the agent in the world
        if action.getType() == "MoveForward":
            print("  -> Moving forward")
        elif action.getType() == "Look":
            print("  -> Looking around")
        elif action.getType() == "Wait":
            print("  -> Waiting")
    
    print("Agent-world simulation complete!\n")


def demo_memory_systems():
    """Demonstrate memory systems integration."""
    print("=== Memory Systems Demo ===")
    
    # Create brain with memory systems
    brain = createBrain()
    brain.initialize()
    
    # Check memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    if working_memory:
        print(f"Working memory: {working_memory}")
        print(f"Active traces: {working_memory.getActiveTraces()}")
    
    if episodic_memory:
        print(f"Episodic memory: {episodic_memory}")
        print(f"Stored episodes: {episodic_memory.getEpisodeCount()}")
    
    # Run a few steps to create some memory
    print("\nRunning memory simulation...")
    for step in range(10):
        brain.step(step)
        if working_memory:
            traces = working_memory.getActiveTraces()
            if traces > 0:
                print(f"Step {step}: {traces} active working memory traces")
    
    print("Memory systems demo complete!\n")


def demo_prediction_system():
    """Demonstrate prediction system."""
    print("=== Prediction System Demo ===")
    
    # Create brain with prediction system
    brain = createBrain()
    brain.initialize()
    
    prediction_system = brain.getPredictionSystem()
    if prediction_system:
        print(f"Prediction system: {prediction_system}")
        
        error = prediction_system.getPredictionError()
        confidence = prediction_system.getConfidence()
        
        print(f"Prediction error: {error:.3f}")
        print(f"Prediction confidence: {confidence:.3f}")
        
        history = prediction_system.getErrorHistory()
        if history:
            print(f"Error history: {history}")
    
    # Run simulation
    print("\nRunning prediction simulation...")
    for step in range(15):
        brain.step(step)
        
        if prediction_system:
            error = prediction_system.getPredictionError()
            confidence = prediction_system.getConfidence()
            print(f"Step {step}: Error={error:.3f}, Confidence={confidence:.3f}")
    
    print("Prediction system demo complete!\n")


def demo_concept_formation():
    """Demonstrate concept formation."""
    print("=== Concept Formation Demo ===")
    
    # Create brain with concept formation
    brain = createBrain()
    brain.initialize()
    
    concept_formation = brain.getConceptFormation()
    if concept_formation:
        print(f"Concept formation: {concept_formation}")
    
    # Run simulation to form concepts
    print("\nRunning concept formation simulation...")
    for step in range(20):
        brain.step(step)
        
        if concept_formation:
            # Check for stable concepts
            for concept_id in range(1, 5):  # Check first 4 concepts
                if concept_formation.isConceptStable(concept_id):
                    prototype = concept_formation.getConceptPrototype(concept_id)
                    stability = concept_formation.getConceptStability(concept_id)
                    print(f"Step {step}: Stable concept {concept_id} "
                          f"(stability={stability:.2f}, "
                          f"prototype_len={len(prototype)})")
                    break
    
    print("Concept formation demo complete!\n")


def demo_neuromodulation():
    """Demonstrate neuromodulation systems."""
    print("=== Neuromodulation Demo ===")
    
    # Create brain with neuromodulation
    brain = createBrain()
    brain.initialize()
    
    # Enable all neuromodulation systems
    agent = createAgentBrain(brain)
    agent.initialize(createSimpleWorld())
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Neuromodulation systems enabled")
    
    # Check neuromodulation levels
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    pred_error = brain.getPredictionErrorSignal()
    
    if dopamine:
        print(f"Dopamine level: {dopamine.getLevel():.3f}")
    if curiosity:
        print(f"Curiosity level: {curiosity.getLevel():.3f}")
    if novelty:
        print(f"Novelty level: {novelty.getLevel():.3f}")
    if pred_error:
        print(f"Prediction error: {pred_error.getLevel():.3f}")
    
    # Run simulation
    print("\nRunning neuromodulation simulation...")
    for step in range(15):
        brain.step(step)
        
        if dopamine:
            print(f"Step {step}: Dopamine={dopamine.getLevel():.3f}, "
                  f"Curiosity={curiosity.getLevel():.3f if curiosity else 0:.3f}")
    
    print("Neuromodulation demo complete!\n")


def main():
    """Main demo function."""
    print("NLM Python API Demo")
    print("==================")
    print()
    
    demos = [
        ("Basic Simulation", demo_basic_simulation),
        ("Agent in World", demo_agent_world),
        ("Memory Systems", demo_memory_systems),
        ("Prediction System", demo_prediction_system),
        ("Concept Formation", demo_concept_formation),
        ("Neuromodulation", demo_neuromodulation),
    ]
    
    for name, demo_func in demos:
        print(f"Running: {name}")
        print("-" * 50)
        try:
            demo_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            print()
    
    print("All demos completed!")
    print("\nFor more information, see the documentation:")
    print("https://github.com/nlm-project/nlm/blob/main/python/nlm_documentation.md")


if __name__ == "__main__":
    main()
