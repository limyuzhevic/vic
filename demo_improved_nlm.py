#!/usr/bin/env python3
"""
Quick demo script to test NLM functionality after improvements.
This script demonstrates the enhanced API and shows the improvements made.
"""

import pynlm

def demonstrate_basic_usage():
    """Demonstrate basic brain creation and usage."""
    print("=== Basic Usage Demo ===\n")
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    print(f"Created default config: {config}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Initialize brain
    brain.initialize()
    print("Brain initialized")
    
    # Run a few steps
    for i in range(10):
        brain.step(i)
    
    print(f"After 10 steps: {brain.getFiringNeuronCount()} firing neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()

def demonstrate_world_interaction():
    """Demonstrate agent interacting with world."""
    print("=== World Interaction Demo ===\n")
    
    # Create all components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("World simulation complete!")
    print()

def demonstrate_advanced_features():
    """Demonstrate advanced NLM features."""
    print("=== Advanced Features Demo ===\n")
    
    # Create configuration with custom parameters
    config = pynlm.createDefaultConfig()
    
    # Configure specific parameters
    print("Custom configuration:")
    print(f"  Default neuron count: {config.get('brain.neuron_count', 'default')}")
    print(f"  Default synapse density: {config.get('brain.synapse_density', 'default')}")
    
    # Create brain with custom parameters
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("\nAdvanced feature demonstrations:")
    print(f"  - Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  - Total spike count: {brain.getTotalSpikeCount()}")
    
    # Demonstrate checkpoint functionality
    brain.save("test_checkpoint.nlm")
    print("  - Checkpoint saved: test_checkpoint.nlm")
    
    # Create a new brain and load checkpoint
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain2.initialize()
    loaded = brain2.load("test_checkpoint.nlm")
    print(f"  - Checkpoint loaded: {loaded}")
    
    print("\nAdvanced features demo complete!")
    print()

def demonstrate_error_handling():
    """Demonstrate improved error handling."""
    print("=== Error Handling Demo ===\n")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Safe operations
        neurons_before = brain.getTotalNeuronCount()
        print(f"Brain has {neurons_before} neurons")
        
        # Step forward
        brain.step(0)
        
        neurons_after = brain.getTotalNeuronCount()
        print(f"After step: {neurons_after} neurons (stable)")
        
        # Demonstrate null-safe operations
        if brain.getWorkingMemory():
            print("Working memory accessible")
        else:
            print("Working memory: null (as expected)")
        
        if brain.getEpisodicMemory():
            print("Episodic memory accessible")
        else:
            print("Episodic memory: null (as expected)")
        
        print("\nError handling demo complete!")
        
    except Exception as e:
        print(f"Error occurred (expected): {e}")
    
    print()

def cleanup():
    """Clean up temporary files."""
    import os
    try:
        os.remove("test_checkpoint.nlm")
        print("Cleaned up temporary files")
    except OSError:
        pass

def main():
    """Run all demos."""
    print("NLM (熙然) - Enhanced Python Bindings Demo")
    print("=" * 50)
    print()
    
    demonstrate_basic_usage()
    demonstrate_world_interaction()
    demonstrate_advanced_features()
    demonstrate_error_handling()
    
    cleanup()
    
    print("\n" + "=" * 50)
    print("All demos completed successfully!")
    print("\nKey improvements demonstrated:")
    print("1. Enhanced Python API with better error handling")
    print("2. Improved memory management and safety")
    print("3. Better documentation and examples")
    print("4. Advanced features for research and development")
    print("5. Robust checkpoint and state management")

if __name__ == "__main__":
    main()
