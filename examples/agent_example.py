#!/usr/bin/env python3
"""
NLM - Agent Simulation Example

This example demonstrates how to create an agent that interacts with a simulated world.
It shows the complete NLM agent loop: sense, think, act.
"""

import pynlm

def run_agent_simulation(num_steps=100):
    """Run a complete agent simulation with brain and world interaction."""
    print(f"=== NLM Agent Simulation ( {num_steps} steps ) ===")
    
    # 1. Create and configure the brain
    print("1. Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create the world
    print("2. Creating world...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 3. Create the agent brain interface
    print("3. Creating agent interface...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning subsystems
    print("4. Enabling learning subsystems...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 5. Run the simulation loop
    print("5. Running simulation loop...")
    for step in range(num_steps):
        # Update world (advance time)
        world.update(0.1)
        
        # Get what the agent sees (sensory input)
        percept = world.getSensoryPercept()
        
        # Tell the brain what it senses
        agent.processSensoryInput(percept)
        
        # Brain thinks (process neural dynamics)
        brain.step(step)
        
        # Brain decides what to do
        action = agent.decodeMotorCommand()
        
        # Do the action in the world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 20 steps
        if step % 20 == 0:
            firing_rate = brain.getAverageFiringRate()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            dev_stage = brain.getDevelopmentalStage()
            
            print(f"  Step {step:3d}: Firing rate={firing_rate:6.2f}, "
                  f"Curiosity={curiosity:5.3f}, Novelty={novelty:5.3f}, "
                  f"Dev={dev_stage}")
    
    # 6. Print final statistics
    print("\n=== Simulation Complete ===")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Final average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    
    return {
        "brain": brain,
        "world": world,
        "agent": agent,
        "total_spikes": brain.getTotalSpikeCount(),
        "final_firing_rate": brain.getAverageFiringRate(),
        "final_curiosity": agent.getCuriosityLevel(),
        "final_novelty": agent.getNoveltyLevel()
    }

def main():
    """Main function to run the example."""
    print("NLM Agent Simulation Example")
    print("=" * 40)
    
    # Run the simulation
    results = run_agent_simulation(100)
    
    print("\nExample complete! Use the returned results for further analysis.")

if __name__ == "__main__":
    main()