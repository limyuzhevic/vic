#!/usr/bin/env python3
"""
Example 5: Development and Adaptation

This example demonstrates how NLM brains develop over time, including
critical periods, maturation, and aging.
"""

import sys
import os
import json

# Add the parent directory to path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm

def main():
    print("\n=== Example 5: Development and Adaptation ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    
    # Create brain
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Setup
    world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
    world.reset()
    world.setAgentStart(4, 4)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track development over time
    development_data = {
        'steps': [],
        'plasticity': [],
        'stage': [],
        'firing_rate': [],
        'curiosity': []
    }
    
    print("Development timeline:")
    
    # Run extended simulation
    total_steps = 1000
    for step in range(total_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.applyRewardModulation(0.0, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            stage = brain.getDevelopmentalStage()
            stage_str = str(stage).split('.')[-1]  # Get enum value
            
            development_data['steps'].append(step)
            development_data['plasticity'].append(agent.getNeuromodulationLevel())
            development_data['stage'].append(stage_str)
            development_data['firing_rate'].append(brain.getFiringNeuronCount())
            development_data['curiosity'].append(agent.getCuriosityLevel())
            
            print(f"  Step {step}: Stage {stage_str}, "
                  f"Plasticity={agent.getNeuromodulationLevel():.3f}, "
                  f"Firing={brain.getFiringNeuronCount()}")
    
    # Analyze development
    print("\n=== Development Analysis ===")
    print(f"Total developmental steps: {len(development_data['steps'])}")
    
    # Identify developmental stages
    initial_stage = development_data['stage'][0]
    final_stage = development_data['stage'][-1]
    print(f"Initial developmental stage: {initial_stage}")
    print(f"Final developmental stage: {final_stage}")
    
    # Plasticity trajectory
    initial_plasticity = development_data['plasticity'][0]
    final_plasticity = development_data['plasticity'][-1]
    print(f"Initial plasticity: {initial_plasticity:.3f}")
    print(f"Final plasticity: {final_plasticity:.3f}")
    print(f"Plasticity change: {final_plasticity - initial_plasticity:.3f}")
    
    # Create development summary
    development_summary = {
        'initial_stage': initial_stage,
        'final_stage': final_stage,
        'plasticity_change': final_plasticity - initial_plasticity,
        'firing_rate_change': development_data['firing_rate'][-1] - development_data['firing_rate'][0],
        'total_steps': total_steps
    }
    
    with open('development_summary.json', 'w') as f:
        json.dump(development_summary, f, indent=2)
    
    print(f"\nDevelopment summary saved to development_summary.json")
    
    return brain, agent, world, development_data

if __name__ == "__main__":
    main()
