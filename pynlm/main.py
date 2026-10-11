"""
NLM Python Interface

This module provides Python bindings for the NLM (Neural Learning Machine) framework.
NLM is an experimental artificial developmental brain project that implements
spiking neural computation with plasticity, memory, and learning capabilities.

Usage Examples:
=============

Basic Neural Network:
------------------
import nlm

# Create a brain with default configuration
brain = nlm.Brain()
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Check results
print(f'Neurons: {brain.getTotalNeuronCount()}')
print(f'Synapses: {brain.getTotalSynapseCount()}')
print(f'Spikes: {brain.getTotalSpikeCount()}')

Advanced Usage:
--------------
import nlm
from nlm import LogLevel

# Set up logging
nlm.setupLogger()

# Create brain with custom configuration
config = nlm.getDefaultConfig()
brain = nlm.Brain(config)
brain.initialize()

# Inject current into sensory neurons
brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 50.0)

# Get a specific neuron
neuron = brain.getRegion(0).getAllNeurons()[0]
print(f'Neuron membrane potential: {neuron.getMembranePotential()}')

# Create a synapse
synapse = nlm.Synapse(nlm.SynapseId(1), nlm.NeuronId(1), nlm.NeuronId(2))
synapse.setWeight(0.5)

# Run benchmark
results = nlm.run_benchmark(1000, 500)
print(f'Benchmark results: {results}')

# Memory system
brain = nlm.Brain()
brain.initialize()

# Store in working memory
brain.getWorkingMemory().store(nlm.NeuronId(1), 0.8)

# Store episode in episodic memory
episode = nlm.EpisodicMemoryItem()
episode.timestamp = 100
episode.positionX = 1.0
episode.positionY = 2.0
episode.reward = 5.0
brain.getEpisodicMemory().storeEpisode(episode)

# Get similar episodes
results = brain.getEpisodicMemory().retrieveSimilar([0.5, 0.5])
print(f'Found {len(results)} similar episodes')

# Procedural memory
brain = nlm.Brain()
brain.initialize()

# Learn a skill
brain.getProceduralMemory().learnSkill(
    "walk", 
    [nlm.NeuronId(1), nlm.NeuronId(2), nlm.NeuronId(3)]
)

# Get the skill
skill = brain.getProceduralMemory().getSkill("walk")
print(f'Skill learned: {skill.name} (pattern length: {len(skill.neuralPattern)})')

# Associative memory
brain = nlm.Brain()
brain.initialize()

# Create association between two patterns
pattern_a = [0.1, 0.2, 0.3, 0.4]
pattern_b = [0.4, 0.3, 0.2, 0.1]
brain.getAssociativeMemory().associate(pattern_a, pattern_b, 0.8)

# Retrieve associated patterns
results = brain.getAssociativeMemory().retrieve(pattern_a)
print(f'Retrieved {len(results)} associated patterns')

# Neural planner
brain = nlm.Brain()
brain.initialize()
brain.getNeuralPlanner().setGoal("find_target")

plan = brain.getNeuralPlanner().plan()
print(f'Plan: {plan}')

# Visualization interface
brain = nlm.Brain()
brain.initialize()

# Create visualization interface
viz = nlm.VisualizationInterface()
viz.setWindowSize(1024, 768)

# Update visualization
viz.update(brain)
viz.render()

# Wait a bit
import time
time.sleep(1)

# Close visualization
viz.close()

# Tutorial example
nlm.example_tutorial()

# Custom functions can be added here

def main():
    """Main entry point for the NLM Python module."""
    print("NLM Python Module")
    print("=================")
    print("Type 'help' for more information or run example_tutorial() to see usage examples.")
    
    # Run tutorial
    example_tutorial()

if __name__ == "__main__":
    main()
