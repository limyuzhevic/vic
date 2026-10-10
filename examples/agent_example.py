#!/usr/bin/env python3

"""
Example: Complete Agent-World Simulation

This script demonstrates a complete NLM agent simulation with world interaction,
learning, and development.
"""

import pynlm

def run_complete_agent_simulation(num_steps=500):
    """
    Run a complete agent simulation with brain, world, and agent integration.
    
    Args:
        num_steps: Number of simulation steps to run
        
    Returns:
        dict: Simulation results and statistics
    """
    print("=== Complete Agent-World Simulation ===")
    print(f"Running {num_steps} simulation steps...")
    
    # Step 1: Create configuration
    print("1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    
    # Step 2: Create and initialize brain
    print("2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Step 3: Create and configure world
    print("3. Creating and configuring world...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(10.0, 10.0)  # Start in center
    
    # Step 4: Create agent interface
    print("4. Creating agent interface...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Step 5: Enable learning subsystems
    print("5. Enabling learning subsystems...")
    agent.enableRewardModulation(True)   # Learn from rewards
    agent.enableCuriosity(True)          # Drive exploration
    agent.enableDevelopment(True)        # Brain matures over time
    agent.enableStructuralPlasticity(True) # Grow new connections
    
    # Step 6: Run simulation loop
    print(f"6. Running {num_steps} simulation steps...")
    
    results = {
        'steps_completed': 0,
        'total_reward': 0.0,
        'max_curiosity': 0.0,
        'max_novelty': 0.0,
        'total_spikes': 0,
        'final_dev_stage': None,
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Brain thinks and learns
        brain.step(step)
        
        # Get motor command and execute it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        results['total_reward'] += reward
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Update statistics
        results['max_curiosity'] = max(results['max_curiosity'], agent.getCuriosityLevel())
        results['max_novelty'] = max(results['max_novelty'], agent.getNoveltyLevel())
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"   Step {step:3d}: "
                  f"Reward={reward:6.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():5.3f}, "
                  f"Novelty={agent.getNoveltyLevel():5.3f}, "
                  f"Firing={brain.getFiringNeuronCount():3d}")
    
    # Step 7: Collect final statistics
    print("7. Collecting final statistics...")
    results['steps_completed'] = num_steps
    results['total_spikes'] = brain.getTotalSpikeCount()
    results['final_dev_stage'] = brain.getDevelopmentalStage()
    
    # Step 8: Print summary
    print("\n=== Simulation Complete ===")
    print(f"Completed {results['steps_completed']} steps")
    print(f"Total reward received: {results['total_reward']:.3f}")
    print(f"Maximum curiosity: {results['max_curiosity']:.3f}")
    print(f"Maximum novelty: {results['max_novelty']:.3f}")
    print(f"Total spikes generated: {results['total_spikes']}")
    print(f"Final development stage: {results['final_dev_stage']}")
    
    # Print brain statistics
    print(f"\nBrain Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Print agent statistics
    print(f"\nAgent Statistics:")
    print(f"  Current curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Current novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Current prediction error: {agent.getPredictionError():.3f}")
    print(f"  Current neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    
    return results

def main():
    """Main function to run the example."""
    try:
        # Run the simulation
        results = run_complete_agent_simulation(500)
        
        print("\n=== Next Steps ===")
        print("To explore further:")
        print("  - Try different configurations")
        print("  - Experiment with learning parameters")
        print("  - Test different world environments")
        print("  - Run benchmarks to measure performance")
        
    except Exception as e:
        print(f"Error running simulation: {e}")
        print("Make sure you have built the NLM Python bindings:")
        print("  python python/build.py --install")

if __name__ == "__main__":
    main()