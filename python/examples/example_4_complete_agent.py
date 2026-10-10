"""
Example 4: Complete Agent with Learning

This example demonstrates a complete agent with all learning features enabled.
This matches the "Complete Agent" example from the documentation.
"""

import pynlm as nlm

def main():
    print("=== Example 4: Complete Agent with Learning ===\n")
    
    # Setup
    print("1. Creating complete agent with all features...")
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    print("2. Enabling all learning features...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run agent
    print("3. Running complete agent simulation (100 steps)...")
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"   Step {step:3d}: "
                  f"Neurons={brain.getFiringNeuronCount():4d}, "
                  f"Rate={brain.getAverageFiringRate():5.2f}")
    
    # Results
    print("4. Simulation complete!")
    print("   Agent simulation complete!")
    
    print("\n=== Example 4 Complete ===\n")

def performance_analysis_example():
    """Example with performance analysis."""
    print("=== Performance Analysis Example ===\n")
    
    # Create optimized brain
    config = nlm.createDefaultConfig()
    config.set('neuron_count', 800)  # Optimized for performance
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features for analysis
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run with timing
    import time
    start_time = time.time()
    
    print("Running performance analysis simulation...")
    for step in range(300):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            print(f"   Step {step:3d}: "
                  f"Neurons={brain.getFiringNeuronCount():4d}, "
                  f"Rate={brain.getAverageFiringRate():5.2f}, "
                  f"Spikes={brain.getTotalSpikeCount():6d}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\n=== Performance Analysis Complete ===")
    print(f"Simulation time: {duration:.2f} seconds")
    print(f"Steps per second: {300/duration:.1f}")
    print(f"Final statistics:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    print("\n=== Performance Analysis Example Complete ===\n")

if __name__ == "__main__":
    main()
    
    # Uncomment to run performance analysis
    # performance_analysis_example()
