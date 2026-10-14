#!/usr/bin/env python3
"""
Example scripts for NLM Python bindings.
Run with: python -m pynlm.examples
"""

import pynlm
import argparse
import sys

def main():
    parser = argparse.ArgumentParser(description="NLM Example Scripts")
    parser.add_argument('script', nargs='?', choices=['quick', 'simulation', 'agent'],
                       help='Which example to run')
    
    args = parser.parse_args()
    
    if not args.script:
        print("NLM Python Bindings Examples")
        print("=" * 40)
        print("Run with: python -m pynlm.examples quick")
        print("         python -m pynlm.examples simulation")
        print("         python -m pynlm.examples agent")
        return
    
    try:
        if args.script == 'quick':
            print("Quick Test Example")
            print("=" * 40)
            quick_test()
        elif args.script == 'simulation':
            print("Complete Simulation Example")
            print("=" * 40)
            simulation_example()
        elif args.script == 'agent':
            print("Agent in World Example")
            print("=" * 40)
            agent_example()
    except Exception as e:
        print(f"Error running example: {e}")
        sys.exit(1)

def quick_test():
    """Simple brain simulation example."""
    # Create brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"\nFinal stats:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Currently firing: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

def simulation_example():
    """Complete simulation with brain and world."""
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 25 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Action: {action}")
    
    print(f"\nSimulation complete!")

def agent_example():
    """Simple agent watching a world."""
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run without learning (just watching)
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"\nAgent watching complete!")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")

if __name__ == "__main__":
    main()
