#!/usr/bin/env python3
"""
Example 3: Complete Agent

This example demonstrates a complete agent that can:
- Perceive its environment
- Make decisions
- Take actions
- Learn from rewards
"""

import sys
import pynlm

def main():
    print("=" * 50)
    print("Example 3: Complete Agent")
    print("=" * 50)
    
    try:
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        print("Agent configuration:")
        print("  - Reward modulation: ENABLED")
        print("  - Curiosity: ENABLED")
        print("  - Structural plasticity: ENABLED (by default)")
        print()
        
        # Run agent
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Show progress every 20 steps
            if step % 20 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                      f"action: {action}")
        
        print("✓ Agent simulation complete!")
        print(f"  - Final firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Development stage: {brain.getDevelopmentalStage()}")
        print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
        print(f"  - Curiosity level: {agent.getCuriosityLevel():.2f}")
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return 1
    
    print("=" * 50)
    return 0
if __name__ == "__main__":
    sys.exit(main())