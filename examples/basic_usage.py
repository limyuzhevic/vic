"""
Basic Usage Examples for NLM (熙然) Python Bindings

This file demonstrates the main usage patterns for the NLM Python API.
It's designed to help both beginners and experienced users get started
quickly with the neural learning machine framework.
"""

import pynlm

def example_1_basic_brain():
    """Example 1: Create a basic brain and run simulation."""
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Run a simple simulation
    for step in range(10):
        brain.step(step)
        
    print(f"After 10 steps: {brain.getTotalSpikeCount()} total spikes")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print()

def example_2_brain_with_world():
    """Example 2: Use brain with world and agent."""
    print("=== Example 2: Brain with World and Agent ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Run simulation
    print("Running simulation for 50 steps...")
    for step in range(50):
        world.update(0.1)  # Update world
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    print(f"Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print()

def example_3_save_and_load():
    """Example 3: Save brain state and load it later."""
    print("=== Example 3: Save and Load Brain State ===")
    
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Run some steps to generate activity
    for step in range(100):
        brain1.step(step)
    
    print(f"Brain1 has {brain1.getTotalSpikeCount()} spikes")
    
    # Save brain state
    save_path = "/tmp/nlm_brain_test.bin"
    if brain1.save(save_path):
        print(f"Brain state saved to {save_path}")
    else:
        print("Warning: Failed to save brain state")
    
    # Create a new brain and load the saved state
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    if brain2.load(save_path):
        print(f"Brain state loaded successfully")
        print(f"Brain2 has {brain2.getTotalSpikeCount()} spikes (should match Brain1)")
    else:
        print("Warning: Failed to load brain state")
    
    print()

def example_4_advanced_configuration():
    """Example 4: Use advanced configuration options."""
    print("=== Example 4: Advanced Configuration ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Modify configuration parameters
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.05)
    config.set("brain.initial_weight_mean", 0.6f)
    config.set("brain.initial_weight_std", 0.2f)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.2f)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain with custom configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with custom configuration:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with learning enabled
    print("\nRunning simulation with learning enabled...")
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.05)  # Apply reward learning
        agent.updateDevelopment(0.1)  # Update development
        
        if step % 50 == 0:
            print(f"Step {step}: "
                  f"Stage: {brain.getDevelopmentalStage()}, "
                  f"Learning: {agent.getNeuromodulationLevel():.3f}")
    
    print(f"Final learned preference: {brain.getLearnedPreference():.3f}")
    print()

def example_5_error_handling():
    """Example 5: Error handling and validation."""
    print("=== Example 5: Error Handling and Validation ===")
    
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Validate configuration
        if pynlm.validate_config(config):
            print("Configuration validation passed")
        else:
            print("Configuration validation failed")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try to process None input (should handle gracefully)
        print("Testing error handling...")
        
        # Create world for testing
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("Example completed with proper error handling")
        
    except pynlm.NLMError as e:
        print(f"NLM Error caught: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")
    
    print()

def example_6_performance_monitoring():
    """Example 6: Performance monitoring and statistics."""
    print("=== Example 6: Performance Monitoring ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation while collecting statistics
    steps = 300
    print(f"Running {steps} steps with performance monitoring...")
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Collect comprehensive statistics
    summary = pynlm.get_brain_summary(brain)
    print(f"\nBrain Summary: {summary}")
    
    # Custom statistics
    stats = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
        'working_memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
        'development_stage': brain.getDevelopmentalStage(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'dopamine_level': agent.getNeuromodulationLevel(),
    }
    
    print("\nDetailed Statistics:")
    for key, value in stats.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    
    print()

def main():
    """Run all examples sequentially."""
    print("NLM Python Bindings - Complete Examples")
    print("=" * 50)
    print()
    
    example_1_basic_brain()
    example_2_brain_with_world()
    example_3_save_and_load()
    example_4_advanced_configuration()
    example_5_error_handling()
    example_6_performance_monitoring()
    
    print("All examples completed successfully!")
    print("\nFor more advanced usage, see:")
    print("  - NLM documentation (README.md)")
    print("  - Python API reference")
    print("  - Advanced configuration options")
    print("  - Performance tuning guidelines")

if __name__ == "__main__":
    main()
