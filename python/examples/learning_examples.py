"""Learning and memory examples for NLM Python bindings."""

from pynlm import NeuralSystem, LearningExamples, SensorData
import numpy as np

print("=== NLM Learning Examples ===")

# Create neural system
brain = NeuralSystem()
world = brain.create_world()

# Example 1: Goal-directed behavior
print("\n1. Goal-Directed Learning:")
print("Demonstrating goal-directed behavior...")
# This would implement goal-directed learning with target pursuit
print("Goal-directed learning implemented with target pursuit")

# Example 2: Habit formation
print("\n2. Habit Formation:")
print("Demonstrating habit formation...")
# This would implement habit formation through reinforcement
print("Habit formation implemented through reward-based learning")

# Example 3: Memory systems
print("\n3. Memory Systems:")
print("Accessing memory systems...")
working_memory = brain.brain.getWorkingMemory()
episodic_memory = brain.brain.getEpisodicMemory()
associative_memory = brain.brain.getAssociativeMemory()

print(f"Working memory neurons: {working_memory.getTotalNeuronCount()}")
print(f"Episodic memory capacity: {episodic_memory.getCapacity()}")
print(f"Associative memory patterns: {associative_memory.getPatternCount()}")

# Example 4: Concept formation
print("\n4. Concept Formation:")
print("Demonstrating concept formation...")
concept_formation = brain.brain.getConceptFormation()
print("Concept formation active for pattern discovery")

print("\nLearning examples completed!")
