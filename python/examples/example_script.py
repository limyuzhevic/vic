#!/usr/bin/env python3
"""
NLM Python Bindings - Example Scripts

This directory contains example scripts demonstrating the use of NLM Python bindings.
Each example shows different aspects of the NLM library, from simple to complex usage.

## How to Run Examples

1. Install NLM first:
   ```bash
   pip install -e .
   ```

2. Run examples:
   ```bash
   python simple_brain.py
   python agent_example.py
   python learning_example.py
   ```

## Examples Included

1. **simple_brain.py** - Basic brain creation and simulation
2. **agent_example.py** - Complete agent-in-world simulation
3. **learning_example.py** - Learning and plasticity demonstration
4. **comparison_example.py** - Brain configuration comparison
5. **experiment_example.py** - Experiment framework usage
6. **advanced_example.py** - Advanced features and optimization
"""

import pynlm

# Example 1: Simple Brain
print("=== Example 1: Simple Brain ===")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing neurons: {brain.getFiringNeuronCount()}")

for i in range(50):
    brain.step(i)

print(f"After 50 steps: {brain.getTotalSpikeCount()} total spikes")
print(f"Final firing neurons: {brain.getFiringNeuronCount()}")
print()

# Example 2: Complete Agent Simulation
print("=== Example 2: Complete Agent Simulation ===")

def run_agent_simulation():
    # Create components
    config = pynlm.createLearningConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure and initialize
    brain.initialize()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 50 == 0:
            print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, " +
                  f"Action={action}, " +
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")

run_agent_simulation()
print()

# Example 3: Learning Experiment
print("=== Example 3: Learning Experiment ===")

def run_learning_experiment():
    # Create brain optimized for learning
    brain = pynlm.createBrain(pynlm.createLearningConfig())
    brain.initialize()
    
    # Configure for learning
    pynlm.configureBrainForLearning(brain)
    
    print(f"Brain configured for learning with {brain.getTotalSynapseCount()} synapses")
    
    # Run learning experiment
    for step in range(300):
        # Provide sensory input
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        brain.step(step)
        
        if step % 50 == 0:
            print(f"Learning step {step}: " +
                  f"Spikes={brain.getTotalSpikeCount()}, " +
                  f"Firing={brain.getFiringNeuronCount()}, " +
                  f"E/I ratio={brain.getExcitationInhibitionRatio():.3f}")
    
    print("Learning experiment complete!")

run_learning_experiment()
print()

# Example 4: Brain Comparison
print("=== Example 4: Brain Configuration Comparison ===")

def compare_brains():
    # Create two different configurations
    config1 = pynlm.createLearningConfig()
    config2 = pynlm.createExplorationConfig()
    
    brain1 = pynlm.createBrain(config1)
    brain2 = pynlm.createBrain(config2)
    
    brain1.initialize()
    brain2.initialize()
    
    print(f"Brain 1 (Learning): {brain1.getTotalNeuronCount()} neurons, " +
          f"{brain1.getTotalSynapseCount()} synapses")
    print(f"Brain 2 (Exploration): {brain2.getTotalNeuronCount()} neurons, " +
          f"{brain2.getTotalSynapseCount()} synapses")
    
    # Compare performance
    for step in range(100):
        brain1.step(step)
        brain2.step(step)
        
        if step % 20 == 0:
            print(f"Step {step}: " +
                  f"Brain1 spikes={brain1.getTotalSpikeCount()}, " +
                  f"Brain2 spikes={brain2.getTotalSpikeCount()}")
    
    print("Comparison complete!")

compare_brains()
print()

# Example 5: State Management
print("=== Example 5: Brain State Management ===")

def demonstrate_state_management():
    # Create and save brain state
    brain = pynlm.createBrain(pynlm.createLearningConfig())
    brain.initialize()
    
    # Run some steps
    for i in range(50):
        brain.step(i)
    
    # Save state
    pynlm.saveBrainWithMetadata(brain, "demo_brain.bin", "Demonstration state")
    print("Brain state saved to demo_brain.bin")
    
    # Get statistics
    stats = pynlm.getBrainStats(brain)
    print("Brain statistics:")
    print(stats)
    
    # Try to load (in real usage, you'd load a saved file)
    print("State management demonstration complete!")

demonstrate_state_management()
print()

print("=== All Examples Completed Successfully ===")
print("This script demonstrates:")
print("1. Basic brain creation and simulation")
print("2. Complete agent-in-world simulation")
print("3. Learning and plasticity")
print("4. Brain configuration comparison")
print("5. State management")
print()
print("For more advanced features, see advanced_example.py")