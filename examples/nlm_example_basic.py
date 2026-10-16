#!/usr/bin/env python3
"""
NLM Basic Usage Example
=======================

This example demonstrates the basic usage of NLM (Neural Learning Machine).
It's designed to be simple and clear for beginners.

To run this example:
1. Build NLM with: cmake --build build && make -j4
2. Install Python bindings: pip install pynlm
3. Run: python nlm_example_basic.py
"""

import pynlm
import sys
import time

def basic_neural_simulation():
    """Example 1: Simple neural simulation"""
    print("=== Example 1: Simple Neural Simulation ===")
    
    # Create a brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    print("\nRunning simulation (100 steps)...")
    start_time = time.time()
    
    for step in range(100):
        brain.step(step)
        
        # Print progress every 25 steps
        if (step + 1) % 25 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            print(f"  Step {step + 1}: {firing} firing neurons, {rate:.2f} Hz")
    
    end_time = time.time()
    
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

def agent_world_simulation():
    """Example 2: Agent in a world"""
    print("\n=== Example 2: Agent in a World ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Created agent with world: {world.getWidth()}x{world.getHeight()}")
    print(f"World vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Run agent simulation
    print("\nRunning agent simulation (100 steps)...")
    
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 25 steps
        if (step + 1) % 25 == 0:
            firing = brain.getFiringNeuronCount()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"  Step {step + 1}: {firing} firing, curiosity={curiosity:.2f}, novelty={novelty:.2f}")
    
    print(f"\nAgent simulation completed!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.2f}")

def complete_workflow_example():
    """Example 3: Complete workflow with all features"""
    print("\n=== Example 3: Complete Workflow ===")
    
    def run_complete_simulation(num_steps=200):
        """Complete simulation with all NLM features"""
        
        # 1. Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.15)
        
        # 2. Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 3. Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        
        # 4. Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        # 5. Initialize agent with world
        agent.initialize(world)
        
        # 6. Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print(f"Starting complete simulation ({num_steps} steps)...")
        print(f"Features enabled: Reward Modulation, Structural Plasticity, Development, Curiosity")
        
        # 7. Run simulation loop
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print statistics every 50 steps
            if step % 50 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                      f"dev stage={brain.getDevelopmentalStage()}, "
                      f"curiosity={agent.getCuriosityLevel():.2f}")
        
        return brain, agent, world
    
    # Run the simulation
    brain, agent, world = run_complete_simulation(200)
    
    print(f"\nComplete workflow results:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.2f}")
    print(f"  Final novelty: {agent.getNoveltyLevel():.2f}")

def configuration_examples():
    """Example 4: Configuration management"""
    print("\n=== Example 4: Configuration Management ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    print("Default configuration:")
    print(f"  Neuron count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"  Region count: {config.getOr('brain.region_count', 2)}")
    print(f"  Connection probability: {config.getOr('brain.connection_probability', 0.1)}")
    print(f"  Simulation timestep: {config.getOr('simulation.timestep', 0.001)}")
    
    # Modify configuration
    config.set("brain.neuron_count", 2000, pynlm.ConfigSource.CommandLine)
    config.set("brain.synapse_density", 0.2, pynlm.ConfigSource.CommandLine)
    config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.CommandLine)
    
    print("\nModified configuration:")
    print(f"  Neuron count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"  Synapse density: {config.getOr('brain.synapse_density', 0.1)}")
    print(f"  STDP enabled: {config.getOr('plasticity.stdp.enable', False)}")
    
    # Create brain with modified config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"\nBrain created with custom config: {brain.getTotalNeuronCount()} neurons")
    
    # Run a few steps to verify
    for i in range(10):
        brain.step(i)
    
    print(f"Total spikes after 10 steps: {brain.getTotalSpikeCount()}")

def error_handling_example():
    """Example 5: Error handling and troubleshooting"""
    print("\n=== Example 5: Error Handling and Troubleshooting ===")
    
    try:
        # Try to create a brain with invalid configuration
        config = pynlm.createDefaultConfig()
        # Note: NLM doesn't validate during creation, only during initialization
        # So we can't easily test invalid config here
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Created brain with default config (should work)")
        
    except Exception as e:
        print(f"Caught exception: {e}")
    
    # Proper configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100, pynlm.ConfigSource.CommandLine)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("\nProper configuration works:")
    print(f"  Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test the importance of initialize()
    try:
        brain2 = pynlm.createBrain(config)
        # Don't call initialize()
        # Note: In real usage, calling step() without initialize() would likely cause issues
        # but we can't demonstrate this easily in Python bindings
        print("Note: In real NLM usage, brain.step() without brain.initialize() would cause issues")
        print(f"  Brain has {brain2.getTotalNeuronCount()} neurons")
    except Exception as e:
        print(f"Caught error: {e}")

def main():
    """Main function to run all examples"""
    print("NLM Python Bindings Examples")
    print("=" * 40)
    print()
    
    # Check if pynlm module is properly loaded
    try:
        version = getattr(pynlm, '__version__', 'unknown')
        print(f"NLM Python bindings version: {version}")
    except AttributeError:
        print("ERROR: pynlm module not properly loaded")
        print("Please ensure Python bindings are compiled and installed")
        sys.exit(1)
    
    print()
    print("Running examples...")
    
    # Run all examples
    basic_neural_simulation()
    agent_world_simulation()
    complete_workflow_example()
    configuration_examples()
    error_handling_example()
    
    print("\n" + "=" * 40)
    print("All examples completed successfully!")
    print("\nNext steps:")
    print("1. Read HOW_TO_USE.md for detailed documentation")
    print("2. Explore more complex examples in the examples directory")
    print("3. Try running NLM from command line: nlm_cli run")
    print("4. Use nlm_cli config show for configuration management")
    
    return 0

if __name__ == "__main__":
    exit(main())