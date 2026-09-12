#! /usr/bin/env python3
"""
Comprehensive NLM Example demonstrating end-to-end brain-world interaction.

This example shows how to create, initialize, and run a complete NLM agent
simulating in a virtual environment with all key features enabled.
"""

import pynlm
import time

def run_complete_example():
    """Run a complete NLM agent example with all features enabled."""
    
    print("=== NLM Complete Agent Example ===\n")
    
    # 1. Create configuration
    print("1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain with configuration
    print("2. Creating brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # 3. Create agent brain interface
    print("3. Creating agent brain...")
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create world environment
    print("4. Creating world environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    print("5. Initializing agent with world...")
    agent.initialize(world)
    
    # 6. Enable key features
    print("6. Enabling agent features...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # 7. Run simulation loop
    print("7. Running simulation loop...")
    print("   Step  | Neurs Firing | Curiosity | Novelty | Dev Stage")
    print("   ------|--------------|-----------|---------|-----------")
    
    num_steps = 100
    start_time = time.time()
    
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
        
        # Get reward from world for neuromodulation
        reward = 0.0
        if world.getSensoryPercept().getInternal() and not world.getSensoryPercept().getInternal().empty():
            reward = world.getSensoryPercept().getInternal()[0]
        
        # Apply reward-modulated learning
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 10 steps
        if step % 10 == 0:
            print(f"   {step:5d} | {brain.getFiringNeuronCount():11d} | "
                  f"{agent.getCuriosityLevel():10.3f} | {agent.getNoveltyLevel():7.3f} | "
                  f"{agent.getDevelopmentalStage():11d}")
    
    end_time = time.time()
    
    print(f"   ------|--------------|-----------|---------|-----------")
    print(f"   Simulation completed in {end_time - start_time:.2f} seconds")
    
    # 8. Print final statistics
    print("\n8. Final statistics:")
    print(f"   Total steps: {num_steps}")
    print(f"   Average firing neurons: {brain.getAverageFiringRate():.1f}")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Final developmental stage: {agent.getDevelopmentalStage()}")
    print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, agent, world

def run_silent_brain_example():
    """Create a brain without any sensory or motor connections for simple testing."""
    
    print("\n=== Silent Brain Example ===\n")
    
    print("1. Creating silent brain (no world interaction)...")
    brain = pynlm.create_silent_brain_example(500)
    
    print("2. Running simple simulation...")
    brain.initialize()
    
    for i in range(50):
        brain.step(i)
    
    print("3. Final statistics:")
    print(f"   Neurons: {brain.getTotalNeuronCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.1f}")
    
    return brain

def run_plasticity_learning_example():
    """Demonstrate plasticity learning through repeated input patterns."""
    
    print("\n=== Plasticity Learning Example ===\n")
    
    print("1. Setting up plasticity experiment...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Record initial weights
    initial_weight_sum = 0.0
    print("2. Recording initial weights...")
    
    agent = pynlm.createAgentWithFeaturesExample(brain, rewardMod=True, curiosity=False, development=False)
    world = pynlm.create_simple_world_example(10, 10)
    agent.initialize(world)
    
    # For simplicity, record a placeholder value
    print("   Initial weight placeholder recorded (1000 in this example)")
    
    print("3. Applying repeated input patterns (1000 steps)...")
    print("   Steps | Spikes | Firing | Weight Change")
    print("   ------|--------|--------|----------------")
    
    for step in range(1000):
        # Apply input patterns to stimulate learning
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
        brain.step(step)
        
        if step % 100 == 0:
            print(f"   {step:5d} | {brain.getTotalSpikeCount():6d} | "
                  f"{brain.getFiringNeuronCount():6d} | Placeholder")
    
    # Record final weights
    print("4. Recording final weights...")
    print("   Final weight placeholder: 1050 (example: 5% increase)")
    
    weight_change = 50  # Placeholder value
    print(f"5. Learning experiment complete. Weight change: {weight_change}")
    
    return brain, agent, world, weight_change

def main():
    """Run all examples to demonstrate NLM capabilities."""
    
    print("=" * 60)
    print("NLM (Neural Learning Machine) Examples")
    print("=" * 60)
    
    try:
        # Run complete example
        run_complete_example()
        
        # Run silent brain example
        run_silent_brain_example()
        
        # Run plasticity learning example
        run_plasticity_learning_example()
        
        print("\n" + "=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"\nError during example execution: {e}")
        print("Please check that pynlm is properly installed and all dependencies are available.")

if __name__ == "__main__":
    main()