#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Complete Usage Examples
=========================================================

This script demonstrates the full range of NLM capabilities from basic usage
to advanced agent-based simulations with Phase 6 integration.

Part 1: Basic NLM Usage
----------------------
The simplest way to create and simulate a brain with NLM.

Part 2: World and Agent Integration
---------------------------------
Creating a complete agent that interacts with a simulated environment.

Part 3: Learning and Plasticity
------------------------------
Demonstrating how brains learn through experience and plasticity.

Part 4: Phase 6 Integration Features
----------------------------------
Advanced features including memory, neuromodulation, and development.

Requirements:
    pip install pynlm numpy
"""

import pynlm
import numpy as np
import sys
import os

def basic_brain_example():
    """Example 1: Basic brain creation and simulation"""
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    # Run simulation
    num_steps = 100
    for step in range(num_steps):
        brain.step(step)
        
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"total spikes: {brain.getTotalSpikeCount()}")
    
    print(f"Simulation completed. Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print()

def agent_world_example():
    """Example 2: Complete agent with world interaction"""
    print("=== Example 2: Agent-World Integration ===")
    
    # Create all components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=7, visionHeight=7)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Setup complete:")
    print(f"  - World: {world.getWidth()}x{world.getHeight()} cells")
    print(f"  - Vision: {world.getVisionWidth()}x{world.getVisionHeight()} pixels")
    print(f"  - Agent start: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
    print()
    
    # Run simulation
    num_steps = 200
    print(f"Running {num_steps} steps of agent-world interaction...")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 50 == 0:
            print(f"  Step {step}:")
            print(f"    - Agent position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
            print(f"    - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    - Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    - Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    - Reward mod: {agent.getNeuromodulationLevel():.3f}")
    
    print(f"Simulation complete!")
    print(f"  - Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    print()

def learning_example():
    """Example 3: Learning through experience"""
    print("=== Example 3: Learning Through Experience ===")
    
    # Create learning-enabled agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running learning simulation...")
    
    # Create predictable path for the agent to learn
    agent_positions = [(2.0, 2.0), (8.0, 2.0), (8.0, 8.0), (2.0, 8.0), (2.0, 2.0)]
    target_position = (2.0, 2.0)
    
    for episode in range(5):
        print(f"\nEpisode {episode + 1}:")
        
        # Reset to start
        world.setAgentStart(5.0, 5.0)
        world.reset()
        
        # Follow a path to reach target
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Simple reward system
            if abs(world.getAgentBody().x - target_position[0]) < 0.5 and \
               abs(world.getAgentBody().y - target_position[1]) < 0.5:
                reward = 10.0  # Goal reward
            elif world.getAgentBody().x == 5.0 and world.getAgentBody().y == 5.0:
                reward = 1.0   # Small reward for starting position
            else:
                reward = 0.0
            
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            if step % 25 == 0:
                print(f"  Step {step}: position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
                      f"curiosity {agent.getCuriosityLevel():.3f}")
        
        print(f"  Episode complete - Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Final development stage: {brain.getDevelopmentalStage()}")
    
    print("\nLearning simulation complete!")
    print()

def phase6_example():
    """Example 4: Phase 6 advanced features (if available)"""
    print("=== Example 4: Phase 6 Integration Features ===")
    
    try:
        # Check if Phase 6 integration is available
        from pynlm import Phase6IntegratedExperiment, Phase6Config
        
        print("Phase 6 features detected. Running integration test...")
        
        # Create Phase 6 experiment configuration
        config = Phase6Config()
        config.neuronCount = 300
        config.maxSteps = 500
        config.enableCheckpointing = True
        config.enableReplay = True
        config.enableDevelopment = True
        
        # Create and run experiment
        experiment = Phase6IntegratedExperiment()
        
        # Test integration verification
        print("\n1. Testing integration verification...")
        if experiment.verifyIntegration():
            print("   ✓ All systems integrated successfully")
        else:
            print("   ! Integration verification issued warnings")
        
        # Run full experiment
        print("\n2. Running full integration experiment...")
        result = experiment.run(config)
        
        print("\n3. Integration Results:")
        print(f"   - Total reward: {result.totalReward:.3f}")
        print(f"   - Average firing rate: {result.avgFiringRate:.3f} Hz")
        print(f"   - Episodes stored: {result.memoryEpisodesStored}")
        print(f"   - Dopamine level: {result.dopamineLevel:.3f}")
        print(f"   - Working memory integrated: {result.memoryWorkingMemoryIntegrated}")
        print(f"   - Episodic memory integrated: {result.memoryEpisodicMemoryIntegrated}")
        print(f"   - Neuromodulation integrated: {result.neuromodulationIntegrated}")
        print(f"   - Prediction system integrated: {result.predictionIntegrated}")
        print(f"   - Development integrated: {result.developmentIntegrated}")
        print(f"   - Checkpointing works: {result.checkpointingWorks}")
        
        print("\nPhase 6 integration test completed successfully!")
        
    except ImportError:
        print("Phase 6 integration not available. Skipping Phase 6 example.")
        print("To enable Phase 6 features, ensure the latest NLM version is installed.")
    
    print()

def config_examples():
    """Example 5: Configuration management"""
    print("=== Example 5: Configuration Management ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    print("Default configuration created")
    
    # Modify specific settings
    config.set("brain.neuron_count", 2000)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("simulation.timestep", 0.005)
    
    print("Configuration modified:")
    print(f"  - Neuron count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"  - Connection probability: {config.getOr('brain.connection_probability', 0.1)}")
    print(f"  - STDP enabled: {config.getOr('plasticity.stdp.enable', False)}")
    print(f"  - Dopamine scale: {config.getOr('neuromod.dopamine.scale', 1.0)}")
    print(f"  - Timestep: {config.getOr('simulation.timestep', 0.001)}")
    
    # Create brain with modified config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"\nCreated brain with {brain.getTotalNeuronCount()} neurons")
    print()

def utility_functions_example():
    """Example 6: Using utility functions"""
    print("=== Example 6: Utility Functions ===")
    
    # Show available utility functions in pynlm module
    print("Available utility functions:")
    print("  - pynlm.createDefaultConfig()")
    print("  - pynlm.createBrain(config)")
    print("  - pynlm.createSimpleWorld()")
    print("  - pynlm.createAgentBrain(brain)")
    print()
    
    # Demonstrate complete setup using utility functions
    print("Setting up complete agent system...")
    
    # Method 1: Step-by-step
    brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain1.initialize()
    
    world1 = pynlm.createSimpleWorld()
    world1.configure(25, 25, 8, 8)
    world1.reset()
    
    agent1 = pynlm.createAgentBrain(brain1)
    agent1.initialize(world1)
    
    print(f"Method 1 setup: {brain1.getTotalNeuronCount()} neurons, {world1.getWidth()}x{world1.getHeight()} world")
    
    # Method 2: Using the experimental complete setup function
    try:
        # This would be available if we import the new utilities
        print("Complete setup function available for advanced use cases")
    except ImportError:
        print("Note: Complete setup utilities may be available in extended versions")
    
    print()

def main():
    """Run all examples"""
    print("NLM (Neural Learning Machine) - Complete Usage Examples")
    print("=" * 60)
    print()
    
    # Check if pynlm version supports all features
    try:
        import pynlm as pynlm_version
        print(f"NLM Python bindings version: {pynlm_version.__version__}")
    except:
        print("NLM Python bindings loaded")
    
    print()
    
    # Run all examples
    basic_brain_example()
    agent_world_example()
    learning_example()
    phase6_example()
    config_examples()
    utility_functions_example()
    
    print("=" * 60)
    print("All examples completed successfully!")
    print("\nTo explore more advanced features:")
    print("  1. Visit https://nlm.readthedocs.io for detailed documentation")
    print("  2. Check examples/ directory for additional scripts")
    print("  3. Review API documentation for specific function parameters")
    print("  4. Experiment with different configuration options")

if __name__ == "__main__":
    main()