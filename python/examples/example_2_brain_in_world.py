"""
Example 2: Brain in a World

This example demonstrates brain-world interaction, where a brain explores
a virtual environment. This matches the "Brain in a Simple World" example
from the documentation.
"""

import pynlm as nlm

def main():
    print("=== Example 2: Brain in a World ===\n")
    
    # Step 1: Create everything
    print("1. Setting up brain...")
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    print("2. Creating world...")
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    print("3. Creating agent...")
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Step 2: Run for 50 steps
    print("4. Running simulation (50 steps)...")
    for step in range(50):
        world.update(0.1)  # Update world
        percept = world.getSensoryPercept()  # What does the agent see?
        agent.processSensoryInput(percept)  # Brain sees it
        brain.step(step)  # Brain thinks
        action = agent.decodeMotorCommand()  # Brain decides action
        world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    # Step 3: Get results
    print("5. Displaying results...")
    print(f"   Simulation finished!")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    print("\n=== Example 2 Complete ===\n")

def enhanced_example():
    """Enhanced version with learning features."""
    print("=== Enhanced Brain in World Example ===\n")
    
    # Create everything with learning capabilities
    print("1. Setting up brain with learning capabilities...")
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print("2. Creating world...")
    world = nlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    print("3. Creating agent...")
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    print("4. Enabling learning features...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run enhanced simulation
    print("5. Running enhanced simulation (100 steps)...")
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get results
    print("6. Displaying enhanced results...")
    print(f"   Enhanced simulation complete!")
    print(f"   Learning features enabled: Reward modulation, Structural plasticity, Development, Curiosity")
    print(f"   Final statistics:")
    print(f"     - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"     - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"     - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"     - Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    print("\n=== Enhanced Example Complete ===\n")

if __name__ == "__main__":
    main()
    
    # Uncomment to run enhanced version
    # enhanced_example()
