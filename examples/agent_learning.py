"""
Learning Agent Example
=======================

This example demonstrates how to create an agent that learns from environmental
interactions using NLM's brain, world, and agent components. It shows the
complete loop of perception, action, and learning.

## What This Example Shows

1. Creating a complete agent system (brain + world + agent)
2. Running an episode of perception-action-learning cycle
3. Using neuromodulation for learning
4. Monitoring agent behavior and learning progress
5. Episodic memory and development integration

## When to Use This Example

- You want to create an autonomous learning agent
- You need to understand the complete NLM agent architecture
- You're interested in neural learning and adaptation
- You want to see how brains interact with environments
"""

import pynlm

def main():
    """Run the learning agent example"""
    print("=== Learning Agent Example ===")
    print()
    
    # 1. Create and configure the brain
    print("Step 1: Creating and configuring brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain created: {brain.getTotalNeuronCount()} neurons")
    
    # 2. Create and configure the world
    print("Step 2: Creating and configuring world...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    print(f"   World configured: {world.getWidth()}x{world.getHeight()} cells")
    
    # 3. Create the agent brain interface
    print("Step 3: Creating agent interface...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning subsystems
    print("Step 4: Enabling learning subsystems...")
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableCuriosity(True)             # Explore new things
    agent.enableDevelopment(True)           # Brain matures over time
    
    print("   Learning subsystems enabled:")
    print("     - Reward modulation: ENABLED")
    print("     - Curiosity: ENABLED")
    print("     - Development: ENABLED")
    
    # 5. Run the learning episode
    print("Step 5: Running learning episode...")
    print()
    
    for step in range(100):
        # Update world state
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes and decides action
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            print(f"Step {step:3d}: "
                  f"{brain.getFiringNeuronCount():4d} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"novelty: {agent.getNoveltyLevel():.3f}, "
                  f"dev stage: {brain.getDevelopmentalStage()}")
    
    # 6. Print final results
    print()
    print("Step 6: Learning episode complete!")
    print("Final statistics:")
    print(f"  Total steps: 100")
    print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    print()
    print("=== Learning Agent Example Complete ===")

if __name__ == "__main__":
    main()
