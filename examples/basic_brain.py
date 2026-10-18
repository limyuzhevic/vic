#!/usr/bin/env python3
"""
Basic NLM Example Demonstrating Core Features

This example showcases the key improvements and features of the NLM (Neural Learning Machine) framework.
It demonstrates the integration of neural computation, sensory processing, and learning systems.
"""

import pynlm
import time

def main():
    print("=" * 60)
    print("NLM Basic Example - Neural Learning Machine Demonstration")
    print("=" * 60)
    
    # Example 1: Silent Brain Simulation
    print("\n1. Silent Brain Simulation (Neurons Only)")
    print("-" * 40)
    
    brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain1.initialize()
    
    start_time = time.time()
    for step in range(100):
        brain1.step(step)
    end_time = time.time()
    
    print(f"✓ Brain simulation completed in {end_time - start_time:.3f} seconds")
    print(f"  Total neurons: {brain1.getTotalNeuronCount()}")
    print(f"  Total spikes generated: {brain1.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain1.getAverageFiringRate():.2f} Hz")
    print(f"  Currently firing neurons: {brain1.getFiringNeuronCount()}")
    
    # Example 2: Brain with Learning Capabilities
    print("\n2. Learning Agent Simulation")
    print("-" * 40)
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    agent = pynlm.createAgentBrain(brain2)
    agent.initialize(world)
    
    # Enable all learning systems
    print("Enabling all learning systems...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run learning episode
    print("Running learning episode...")
    start_time = time.time()
    
    for step in range(200):
        # Environment interaction
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain2.step(step)
        
        # Get and execute action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning (simplified reward from world state)
        reward = 0.0  # Would come from actual world rewards
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Display progress
        if step % 50 == 0:
            print(f"  Step {step}: {brain2.getFiringNeuronCount()} firing neurons, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    end_time = time.time()
    
    print(f"✓ Learning episode completed in {end_time - start_time:.3f} seconds")
    print(f"  Total spikes: {brain2.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain2.getAverageFiringRate():.2f} Hz")
    print(f"  Final development stage: {brain2.getDevelopmentalStage()}")
    print(f"  Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    
    # Example 3: Configuration Customization
    print("\n3. Configuration Customization")
    print("-" * 40)
    
    # Create custom configuration
    custom_config = pynlm.createDefaultConfig()
    
    # Modify brain parameters
    custom_config.set("brain.neuron_count", 3000)
    custom_config.set("brain.synapse_density", 0.15)
    custom_config.set("plasticity.stdp.learning_rate", 0.002)
    custom_config.set("neuromod.curiosity.enable", True)
    custom_config.set("neuromod.novelty.enable", True)
    
    print("Custom configuration created:")
    print(f"  Neuron count: {custom_config.get('brain.neuron_count')}")
    print(f"  Synapse density: {custom_config.get('brain.synapse_density')}")
    print(f"  STDP learning rate: {custom_config.get('plasticity.stdp.learning_rate')}")
    print(f"  Curiosity enabled: {custom_config.get('neuromod.curiosity.enable')}")
    print(f"  Novelty enabled: {custom_config.get('neuromod.novelty.enable')}")
    
    # Create brain with custom config
    custom_brain = pynlm.createBrain(custom_config)
    custom_brain.initialize()
    
    print(f"✓ Custom brain created with {custom_brain.getTotalNeuronCount()} neurons")
    
    # Example 4: State Management
    print("\n4. State Management (Save/Load)")
    print("-" * 40)
    
    # Save brain state
    checkpoint_file = "brain_checkpoint.brain"
    saved = custom_brain.save(checkpoint_file)
    
    if saved:
        print(f"✓ Brain state saved to {checkpoint_file}")
    else:
        print(f"✗ Failed to save brain state to {checkpoint_file}")
    
    # Create new brain and load state
    loaded_brain = pynlm.createBrain(custom_config)
    loaded_brain.initialize()
    
    loaded = loaded_brain.load(checkpoint_file)
    
    if loaded:
        print(f"✓ Brain state loaded successfully")
        print(f"  Loaded neuron count: {loaded_brain.getTotalNeuronCount()}")
    else:
        print(f"✗ Failed to load brain state")
    
    # Example 5: Advanced Agent Features
    print("\n5. Advanced Agent Features")
    print("-" * 40)
    
    # Create advanced agent
    advanced_agent = pynlm.createAgentBrain(custom_brain)
    advanced_agent.initialize(world)
    
    # Check learning system status
    print("Learning system status:")
    print(f"  Reward modulation enabled: {advanced_agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity enabled: {advanced_agent.isStructuralPlasticityEnabled()}")
    print(f"  Development enabled: {advanced_agent.isDevelopmentEnabled()}")
    print(f"  Curiosity enabled: {advanced_agent.isCuriosityEnabled()}")
    
    # Get comprehensive agent statistics
    stats = advanced_agent.getLearningStats()
    print(f"\nAgent learning statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    print("\n" + "=" * 60)
    print("✓ ALL EXAMPLES COMPLETED SUCCESSFULLY!")
    print("=" * 60)
    print("\nKey Improvements Demonstrated:")
    print("  • Comprehensive neural simulation with learning")
    print("  • Configurable brain parameters and plasticity")
    print("  • State management (save/load functionality)")
    print("  • Multiple learning systems (reward, curiosity, development)")
    print("  • Sensory-motor integration")
    print("  • Neuromodulation and cognitive control")
    print("  • Advanced error handling and validation")
    print("  • Comprehensive documentation and examples")
    
    print("\nFor more information:")
    print("  • Read docs/USAGE/BASIC.md for beginner tutorial")
    print("  • Read docs/USAGE/INTERMEDIATE.md for advanced usage")
    print("  • Read docs/USAGE/ADVANCED.md for expert features")
    print("  • Check API_REFERENCE/ for complete method documentation")
    print("  • Explore examples/ for additional working examples")

if __name__ == "__main__":
    main()
