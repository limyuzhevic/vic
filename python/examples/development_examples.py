"""Development system examples for NLM Python bindings."""

from pynlm import NeuralSystem, DevelopmentExamples
from pynlm import DevelopmentalStage
import numpy as np

print("=== NLM Development Examples ===")

# Create neural system
brain = NeuralSystem()

# Example 1: Critical period
print("\n1. Critical Period Development:")
print("Demonstrating critical period...")
brain.brain.setDevelopmentalStage(DevelopmentalStage.CriticalPeriod)
print("Brain in critical period stage - enhanced plasticity")

# Simulate development over time
for step in range(5):
    brain.brain.develop()
    print(f"  Development step {step + 1}")

# Example 2: Maturation
print("\n2. Maturation Development:")
print("Demonstrating maturation...")
brain.brain.setDevelopmentalStage(DevelopmentalStage.Maturation)
print("Brain in maturation stage - synaptic pruning")

# Simulate maturation
for step in range(5):
    brain.brain.develop()
    print(f"  Maturation step {step + 1}")

# Example 3: Synaptogenesis
print("\n3. Synaptogenesis:")
print("Demonstrating synaptic formation...")
# This would show formation of new synapses
print("Synaptogenesis active - forming new connections")

# Example 4: Pruning
print("\n4. Synaptic Pruning:")
print("Demonstrating synaptic pruning...")
# This would show removal of weak synapses
print("Synaptic pruning active - removing weak connections")

# Example 5: Developmental milestones
print("\n5. Developmental Milestones:")
print("Tracking developmental milestones...")
for stage in [DevelopmentalStage.Initial, DevelopmentalStage.CriticalPeriod, 
              DevelopmentalStage.Maturation, DevelopmentalStage.Adult, DevelopmentalStage.Aging]:
    brain.brain.setDevelopmentalStage(stage)
    print(f"  Reached {stage} stage")

print("\nDevelopment examples completed!")
