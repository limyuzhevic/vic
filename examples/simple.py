#!/usr/bin/env python3
"""
NLM Python API Examples

This script demonstrates the key features of the NLM Python API.
Run with: python examples/simple.py
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

try:
    import pynlm
    print("NLM Python API Examples")
    print("=" * 50)
    print()
    
    print("Example 1: Basic Brain Creation")
    print("-" * 30)
    # Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial spike count: {brain.getTotalSpikeCount()}")
    
    # Run simulation
    for i in range(50):
        brain.step(i)
    
    print(f"After 50 steps - spike count: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print()
    
    print("Example 2: Brain with Custom Configuration")
    print("-" * 30)
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.connection_probability", 0.15)
    
    custom_brain = pynlm.createBrain(config)
    custom_brain.initialize()
    
    print(f"Custom brain with {custom_brain.getTotalNeuronCount()} neurons")
    print(f"Connection probability: {custom_brain.getConfig().getOr<float>('brain.connection_probability', 0.0)}")
    print()
    
    print("Example 3: Agent in World Simulation")
    print("-" * 30)
    # Create complete agent system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation steps
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Simulation completed: {brain.getTotalSpikeCount()} total spikes")
    print(f"Agent curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Agent novelty level: {agent.getNoveltyLevel():.3f}")
    print()
    
    print("Example 4: Brain Statistics and Monitoring")
    print("-" * 30)
    # Get brain statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    if agent.getBrain():
        agent_brain = agent.getBrain()
        print(f"Agent thought trace: {agent.getThoughtTrace()}")
        print(f"Agent learned preference: {agent.getLearnedPreference():.3f}")
    
    print()
    print("All examples completed successfully!")
    
except ImportError as e:
    print(f"Error: Could not import pynlm: {e}")
    print("Please make sure pynlm is installed:")
    print("  pip install pynlm")
    sys.exit(1)
except Exception as e:
    print(f"Error during execution: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)