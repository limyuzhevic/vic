#!/usr/bin/env python3
"""
Example 3: Development and Learning Simulation
=============================================

This example demonstrates how development stages and learning mechanisms
interact during a simulation.
"""

import pynlm
import json
import os

def development_learning_example():
    """Simulate development and learning over time."""
    print("=== Development and Learning Example ===")
    
    # Create a configuration optimized for development studies
    config = pynlm.createDefaultConfig()
    
    # Set development parameters
    config.set("development.initial_plasticity", 1.0)
    config.set("development.critical_period_start", 100)
    config.set("development.critical_period_end", 500)
    config.set("development.pruning_rate", 0.0001)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    agent.initialize(world)
    
    # Enable development and learning
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    # Track development stages
    developmental_stages = []
    plasticity_rates = []
    firing_rates = []
    
    # Run simulation with developmental tracking
    for step in range(1000):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get motor command and apply
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Record development metrics
        if step % 50 == 0:
            developmental_stages.append({
                "step": step,
                "stage": brain.getDevelopmentalStage().value,
                "plasticity_rate": brain.getSynapticPlasticityRate(),
                "firing_rate": brain.getAverageFiringRate(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel()
            })
            
            # Update development
            agent.updateDevelopment(0.1)
    
    # Save developmental data
    dev_data = {
        "config": {
            "neuron_count": config.get("brain.neuron_count", 1000),
            "plasticity_learning_rate": config.get("plasticity.stdp.learning_rate", 0.001)
        },
        "developmental_stages": developmental_stages
    }
    
    output_dir = os.path.expanduser("~/nlm_dev_data")
    os.makedirs(output_dir, exist_ok=True)
    
    with open(f"{output_dir}/development_simulation.json", 'w') as f:
        json.dump(dev_data, f, indent=2)
    
    print(f"Development simulation completed!")
    print(f"Recorded {len(developmental_stages)} developmental snapshots")
    print(f"Saved to: {output_dir}/development_simulation.json")
    
    # Print summary
    final_stage = developmental_stages[-1]
    print(f"\nDevelopment Summary:")
    print(f"  Final developmental stage: {final_stage['stage']}")
    print(f"  Average firing rate: {sum(d['firing_rate'] for d in developmental_stages) / len(developmental_stages):.3f}")
    print(f"  Final curiosity level: {final_stage['curiosity']:.3f}")
    
    return brain, agent, world, dev_data

if __name__ == "__main__":
    development_learning_example()