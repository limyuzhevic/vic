#!/usr/bin/env python3
"""
NLM Demo 3: Complete Agent

This is the most comprehensive demo showing a complete agent that can learn,
explore, and adapt over time. It enables all major brain systems including
reward modulation, curiosity, and development.

This demonstrates the full capabilities of NLM's integrated brain architecture.
"""

import pynlm
import time

def run_agent_simulation(num_steps=500):
    """Run a complete agent simulation with brain and world."""
    
    print("=== NLM Complete Agent Demo ===")
    print(f"Running simulation for {num_steps} steps...\n")
    
    # 1. Create configuration
    print("1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    print("2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    print("3. Creating agent brain interface...")
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    print("4. Creating and configuring world...")
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # 5. Initialize agent with world
    print("5. Initializing agent with world...")
    agent.initialize(world)
    
    # 6. Enable all learning subsystems
    print("6. Enabling learning subsystems...")
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # 7. Run simulation loop
    print("7. Starting simulation loop...\n")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
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
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal() and not percept.getInternal().empty():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Brain firing: {brain.getFiringNeuronCount()} neurons")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    
    # 8. Print final statistics
    print("\n8. Final simulation statistics:")
    print(f"   Total simulation time: {num_steps * 0.1:.1f} seconds")
    print(f"   Final agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"   Total neurons fired: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"   Development stage: {brain.getDevelopmentalStage()}")
    
    # 9. Run integration verification
    print("\n9. Running integration verification...")
    # For demo purposes, we just verify basic connectivity
    # In a real Phase 6 experiment, this would run the full integration test
    
    # Check basic integration by verifying that major systems are present
    print("   Verifying memory system integration...")
    has_working_memory = brain.getWorkingMemory() is not None
    has_episodic_memory = brain.getEpisodicMemory() is not None
    
    print("   Verifying neuromodulation system integration...")
    has_neuromodulation = brain.getDopamine() is not None
    
    print("   Verifying development system integration...")
    has_development = brain.getDevelopmentSystem() is not None
    
    # Report integration status
    print("   Integration verification complete!")
    print(f"   - Working Memory: {'YES' if has_working_memory else 'NO'}")
    print(f"   - Episodic Memory: {'YES' if has_episodic_memory else 'NO'}")
    print(f"   - Neuromodulation: {'YES' if has_neuromodulation else 'NO'}")
    print(f"   - Development: {'YES' if has_development else 'NO'}")
    
    return brain, agent, world

def main():
    """Main demo function."""
    try:
        brain, agent, world = run_agent_simulation(500)
        
        print("\n=== Complete Agent Demo Complete ===")
        print("\nWhat you learned:")
        print("- How to create a complete agent with brain and world")
        print("- How learning systems (reward, curiosity, development) work together")
        print("- How agents explore and adapt over time")
        print("- The full closed-loop brain-in-world interaction")
        print("\nKey features demonstrated:")
        print("✓ Reward-modulated learning")
        print("✓ Curiosity-driven exploration")
        print("✓ Neural development and maturation")
        print("✓ Structural plasticity (connection growth)")
        print("✓ Memory systems integration")
        print("✓ Neuromodulation coordination")
        
    except ImportError as e:
        print(f"Import error: {e}")
        print("This demo requires the complete NLM installation.")
    except Exception as e:
        print(f"Error during simulation: {e}")

if __name__ == "__main__":
    main()