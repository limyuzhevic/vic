#!/usr/bin/env python3
"""
Example 2: Agent in World Simulation
Demonstrates how to create an agent that interacts with a simulated world.
"""

import pynlm
import time

def agent_world_simulation():
    """Run an agent-world simulation."""
    print("=== NLM Agent-World Simulation ===")
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)  # More neurons for richer behavior
    config.set("brain.synapse_density", 0.15)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    print("✓ Agent brain interface created")
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30,
        height=30,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    print("✓ World configured and reset")
    
    # Set agent starting position
    world.setAgentStart(15.0, 15.0)
    print("✓ Agent start position set")
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    print("✓ Learning systems enabled")
    
    # Initialize agent with world
    agent.initialize(world)
    print("✓ Agent initialized with world")
    
    # Add some obstacles and resources to world
    # Wall
    wall = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Wall, radius=0.8)
    world.addObject(wall)
    
    # Resource
    resource = pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Resource, value=2.0)
    world.addObject(resource)
    
    print("✓ World objects added (wall and resource)")
    
    # Simulation loop
    print("\nStarting simulation loop...")
    start_time = time.time()
    
    for step in range(200):
        # Update world
        world.update(0.1)  # 100ms timestep
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation based on agent performance
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        # Update development system
        agent.updateDevelopment(0.1)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            body = world.getAgentBody()
            print(f"Step {step:3d}: Pos({body.x:5.1f}, {body.y:5.1f}) Energy({body.energy:4.1f}) "
                  f"Health({body.health:3.0f}) Reward({reward:4.1f}) "
                  f"Firing({brain.getFiringNeuronCount():3d}) "
                  f"Curiosity({agent.getCuriosityLevel():4.2f}) "
                  f"Novelty({agent.getNoveltyLevel():4.2f})")
        
        # Check if agent died
        if world.getAgentBody().health <= 0:
            print(f"Agent died at step {step}")
            break
    
    end_time = time.time()
    print(f"\n✓ Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Show final results
    body = world.getAgentBody()
    print(f"\nFinal Results:")
    print(f"  Final position: ({body.x:.1f}, {body.y:.1f})")
    print(f"  Final energy: {body.energy:.1f}")
    print(f"  Final health: {body.health}")
    print(f"  Final age: {body.age:.1f}")
    print(f"  Time alive: {body.lastActionTime:.1f}")
    
    # Show brain statistics
    print(f"\nBrain Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Show neuromodulation levels
    print(f"\nNeuromodulation Levels:")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction Error: {agent.getPredictionError():.3f}")
    print(f"  Neuromodulation Level: {agent.getNeuromodulationLevel():.3f}")
    
    # Save simulation results
    brain.save("agent_world_brain.bin")
    print(f"✓ Brain state saved to agent_world_brain.bin")
    
    # Generate simulation summary
    simulation_summary = {
        'duration': end_time - start_time,
        'steps_completed': step + 1,
        'final_position': (body.x, body.y),
        'final_energy': body.energy,
        'final_health': body.health,
        'total_neurons': brain.getTotalNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'reward_received': sum(0.0),  # Would need to track this
    }
    
    print(f"\n=== Simulation Summary ===")
    print(f"  Simulation completed successfully!")
    print(f"  Agent explored and learned in the environment")
    print(f"  View results in agent_world_brain.bin")

if __name__ == "__main__":
    agent_world_simulation()