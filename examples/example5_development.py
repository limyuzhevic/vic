#!/usr/bin/env python3
"""
Example 5: Development System

This example demonstrates how the NLM development system works,
including developmental stages, maturation, and learning changes over time.
"""

import pynlm
import numpy as np

def development_system_demo():
    """Demonstrate development system functionality."""
    
    print("=== Example 5: Development System ===")
    
    # Create brain with development
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Development system initialized")
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Create agent for world interaction
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(True)
    
    # Collect development data
    dev_data = {
        'steps': [],
        'stage': [],
        'stage_progress': [],
        'critical_period': [],
        'learning_rate': [],
        'plasticity': []
    }
    
    # Run simulation with development
    print("\nRunning development simulation...")
    for step in range(300):
        # Update world
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step (development updates)
        brain.step(step)
        
        # Get current developmental state
        current_stage = brain.getDevelopmentalStage()
        stage_progress = brain.getStageProgress()
        critical_period = brain.isCriticalPeriodActive()
        
        # Calculate developmental learning rate
        learning_rate = (stage_progress * 0.01) + 0.001
        
        # Get plasticity modulation
        plasticity = brain.getPlasticityLevel()
        
        # Collect data
        dev_data['steps'].append(step)
        dev_data['stage'].append(current_stage)
        dev_data['stage_progress'].append(stage_progress)
        dev_data['critical_period'].append(critical_period)
        dev_data['learning_rate'].append(learning_rate)
        dev_data['plasticity'].append(plasticity)
        
        # Apply reward modulation
        reward = np.random.random() * 0.2
        agent.applyRewardModulation(reward, 0.0)
        
        # Print progress every 60 steps
        if step % 60 == 0:
            print(f"  Step {step}: Stage {current_stage}, Progress {stage_progress:.2f}, "
                  f"Critical: {critical_period}, Learning Rate: {learning_rate:.4f}")
    
    # Analyze development data
    print("\n--- Development Analysis ---")
    
    stage_transitions = sum(1 for i in range(1, len(dev_data['stage'])) 
                            if dev_data['stage'][i] != dev_data['stage'][i-1])
    
    critical_periods = sum(dev_data['critical_period'])
    avg_plasticity = np.mean(dev_data['plasticity'])
    avg_learning_rate = np.mean(dev_data['learning_rate'])
    
    print(f"Stage transitions: {stage_transitions}")
    print(f"Critical period steps: {critical_periods}/{len(dev_data['steps'])}")
    print(f"Average plasticity level: {avg_plasticity:.3f}")
    print(f"Average learning rate: {avg_learning_rate:.4f}")
    
    # Show developmental progression
    print("\n--- Developmental Progression ---")
    for i in range(0, len(dev_data['steps']), 60):
        stage = dev_data['stage'][i]
        progress = dev_data['stage_progress'][i]
        print(f"Step {dev_data['steps'][i]:3d}: Stage {stage:2d}, Progress {progress:.2f}")
    
    # Test development stage transitions
    print("\n--- Development Stage Testing ---")
    print("Initial stage:", brain.getDevelopmentalStage())
    
    # Force next stage
    brain.startNextStage()
    print("After force next stage:", brain.getDevelopmentalStage())
    
    # Test critical period
    print("Critical period status:", brain.isCriticalPeriodActive())
    
    # Modify developmental parameters
    print("\n--- Modifying Development Parameters ---")
    # Note: This would require additional API methods
    print("Development parameters would be modified here")
    
    print("\nDevelopment system demonstration complete!")
    return brain, world, agent

if __name__ == "__main__":
    development_system_demo()