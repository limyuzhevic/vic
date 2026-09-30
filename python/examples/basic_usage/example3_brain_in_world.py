# Brain in a World Example
"""
Complete simulation with a brain, world, and agent.
This matches Example 3 from easy_usage.md with enhanced features.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Brain in a World Example ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        
    print("Watched world for 30 steps")
    print(f"Firing rate: {brain.average_firing_rate}")
    print(f"Total spikes: {brain.total_spikes}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")

if __name__ == "__main__":
    main()
