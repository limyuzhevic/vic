#!/usr/bin/env python3
"""
NLM Exploration Script
======================

Script to explore NLM features and demonstrate advanced functionality.
"""

import sys
import os
import time
import argparse

def main():
    parser = argparse.ArgumentParser(description="Explore NLM features")
    parser.add_argument("--mode", type=str, default="basic", 
                       choices=["basic", "agent", "batch", "analysis"],
                       help="Exploration mode")
    parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser.add_argument("--output", type=str, default="exploration.log", help="Output log file")
    
    args = parser.parse_args()
    
    # Add the enhanced module path
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))
    
    try:
        # Import NLM modules
        import pynlm
        print("NLM Python bindings loaded successfully")
        
        # Setup logging
        with open(args.output, 'w') as log_file:
            log_file.write(f"NLM Exploration Log\n")
            log_file.write(f"Mode: {args.mode}\n")
            log_file.write(f"Started at: {time.ctime()}\n")
            log_file.write("-" * 50 + "\n")
        
        if args.mode == "basic":
            # Basic neural activity exploration
            explore_basic(log_file, args.steps)
            
        elif args.mode == "agent":
            # Agent-based exploration
            explore_agent(log_file, args.steps)
            
        elif args.mode == "batch":
            # Batch processing exploration
            explore_batch(log_file, args.steps)
            
        elif args.mode == "analysis":
            # Analysis exploration
            explore_analysis(log_file, args.steps)
        
        print(f"\nExploration completed. Results saved to: {args.output}")
        return 0
        
    except ImportError as e:
        print(f"Error importing NLM: {e}")
        return 1
    except Exception as e:
        print(f"Error during exploration: {e}")
        return 1

def explore_basic(log_file, steps):
    """Explore basic neural activity."""
    print("Running basic neural activity exploration...")
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 500)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 20 == 0:
            log_file.write(f"Basic: Step {step}, Firing={brain.getFiringNeuronCount()}, "
                          f"TotalSpikes={brain.getTotalSpikeCount()}\n")
    
    log_file.write(f"\nBasic exploration completed.\n")

def explore_agent(log_file, steps):
    """Explore agent-based behavior."""
    print("Running agent-based exploration...")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        reward = percept.getInternal()[0] if percept and percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 20 == 0:
            log_file.write(f"Agent: Step {step}, Curiosity={agent.getCuriosityLevel():.3f}, "
                          f"Novelty={agent.getNoveltyLevel():.3f}, "
                          f"Firing={brain.getFiringNeuronCount()}\n")
    
    log_file.write(f"\nAgent exploration completed.\n")

def explore_batch(log_file, steps):
    """Explore batch processing."""
    print("Running batch processing exploration...")
    
    # Create multiple brains
    brains = []
    for i in range(3):
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 500 + i * 200)
        brain = nlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Run batch simulation
    for step in range(steps):
        for i, brain in enumerate(brains):
            brain.step(step)
        
        if step % 20 == 0:
            total_firing = sum(brain.getFiringNeuronCount() for brain in brains)
            log_file.write(f"Batch: Step {step}, TotalFiring={total_firing}, "
                          f"AvgFiring={total_firing/len(brains):.1f}\n")
    
    log_file.write(f"\nBatch exploration completed.\n")

def explore_analysis(log_file, steps):
    """Explore analysis capabilities."""
    print("Running analysis exploration...")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
    
    # Analyze results
    log_file.write(f"Analysis Results:\n")
    log_file.write(f"  Total neurons: {brain.getTotalNeuronCount()}\n")
    log_file.write(f"  Total synapses: {brain.getTotalSynapseCount()}\n")
    log_file.write(f"  Final firing neurons: {brain.getFiringNeuronCount()}\n")
    log_file.write(f"  Average firing rate: {brain.getAverageFiringRate():.2f}\n")
    log_file.write(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}\n")
    log_file.write(f"  Total spikes: {brain.getTotalSpikeCount()}\n")
    
    log_file.write(f"\nAnalysis exploration completed.\n")

if __name__ == "__main__":
    sys.exit(main())