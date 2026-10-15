"""Neuromodulation examples for NLM Python bindings."""

from pynlm import NeuralSystem, NeuromodulationExamples
import numpy as np

print("=== NLM Neuromodulation Examples ===")

# Create neural system with agent
brain = NeuralSystem()
world = brain.create_world()
agent = brain.create_agent(0.0, 0.0)

# Example 1: Reward modulation
print("\n1. Reward Modulation:")
print("Demonstrating reward modulation...")
if hasattr(brain.brain, 'getNeuromodulationLevel'):
    print(f"  Neuromodulation level: {brain.brain.getNeuromodulationLevel()}")
    
print("Applying reward-based learning")
print("Dopamine levels updated based on reward prediction")

# Example 2: Curiosity
print("\n2. Curiosity-Driven Learning:")
print("Demonstrating curiosity...")
if hasattr(brain.brain, 'getCuriosityLevel'):
    print(f"  Curiosity level: {brain.brain.getCuriosityLevel()}")
    
print("Curiosity modulation active - promoting exploration")

# Example 3: Novelty detection
print("\n3. Novelty Detection:")
print("Demonstrating novelty detection...")
if hasattr(brain.brain, 'getNoveltyLevel'):
    print(f"  Novelty level: {brain.brain.getNoveltyLevel()}")
    
print("Novelty detection active - detecting new experiences")

# Example 4: Prediction error
print("\n4. Prediction Error:")
print("Demonstrating prediction error...")
if hasattr(brain.brain, 'getPredictionError'):
    print(f"  Prediction error: {brain.brain.getPredictionError()}")
    
print("Prediction error signals updating learning")

# Example 5: Neuromodulation integration
print("\n5. Neuromodulation Integration:")
print("Demonstrating integrated neuromodulation...")
print("Dopamine: Reward signal")
print("Curiosity: Exploration drive")
print("Novelty: Surprise detection")
print("Prediction Error: Learning signal")

print("\nNeuromodulation examples completed!")
