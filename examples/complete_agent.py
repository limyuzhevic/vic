#!/usr/bin/env python3
"""Example 3: Complete Agent Simulation

This example demonstrates a complete agent simulation where the brain learns
through interaction with the environment. The agent explores, learns from
rewards, and develops over time.
"""

import pynlm

def run_complete_agent_simulation(num_steps=100):
    """Run a complete agent simulation with brain, world, and learning."""
    print(f"=== Complete Agent Simulation ({num_steps} steps) ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)  # Start in center
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run agent simulation
    for step in range(num_steps):
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step (neural computation)
        brain.step(step)
        
        # Decode motor command from brain activity
        action = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update developmental system
        agent.updateDevelopment(0.1)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Neuromodulation Level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Development Stage: {brain.getDevelopmentalStage().name}")
            print(f"  Agent Body Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print("\n=== Simulation Complete ===")
    print(f"Final Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage().name}")

def learn_from_different_situations():
    """Demonstrate learning from different environmental situations."""
    print("\n=== Learning from Different Situations ===")
    
    # Create agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Simulate different scenarios
    scenarios = [
        ("Safe Zone", lambda w: w.addObject(pynlm.WorldObject(3.0, 3.0, pynlm.WorldObjectType.Resource, value=10.0))),
        ("Hazard Zone", lambda w: w.addObject(pynlm.WorldObject(7.0, 7.0, pynlm.WorldObjectType.Hazard, value=-5.0))),
        ("Navigation Aid", lambda w: w.addObject(pynlm.WorldObject(8.0, 2.0, pynlm.WorldObjectType.Marker, value=1.0))),
    ]
    
    for scenario_name, obj_placer in scenarios:
        print(f"\n--- Testing {scenario_name} ---")
        obj_placer(world)
        
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction Error: {agent.getPredictionError():.3f}")

if __name__ == "__main__":
    # Run the main simulation
    run_complete_agent_simulation(100)
    
    # Run additional learning demonstrations
    learn_from_different_situations()