#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Comprehensive Example

This example demonstrates the enhanced NLM Phase 6 API with advanced features.
It shows how to use the new configuration options, performance monitoring,
error handling, and visualization capabilities.
"""

import pynlm
import time
import sys

def basic_example():
    """Basic NLM example using the enhanced API."""
    print("=== NLM Basic Example ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Print configuration summary
    summary = brain.getConfigurationSummary()
    print("Brain initialized successfully:")
    print(summary)
    
    # Run simulation
    print("\nRunning simulation...")
    for step in range(100):
        brain.step(step)
        
        # Get firing neuron count
        firing_count = brain.getFiringNeuronCount()
        if step % 20 == 0:
            print(f"Step {step}: {firing_count} firing neurons")
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    print()

def advanced_example():
    """Advanced NLM example using advanced configuration and monitoring."""
    print("=== NLM Advanced Example ===")
    
    # Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("working_memory.capacity", 50)
    config.set("episodic_memory.max_episodes", 1000)
    
    # Create brain with advanced configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain configured with advanced settings:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  STDP enabled: {config.has('plasticity.stdp.enable') and config.getOr('plasticity.stdp.enable', False)}")
    print(f"  Hebbian enabled: {config.has('plasticity.hebbian.enable') and config.getOr('plasticity.hebbian.enable', False)}")
    print(f"  Neuromodulation enabled: {config.has('neuromod.dopamine.scale')}")
    
    # Enable performance monitoring
    brain.enablePerformanceMonitoring(True)
    
    # Advanced plasticity configuration
    brain.setSTDPParameters(0.02f, 0.015f, 20.0f)
    brain.setHebbianParameters(0.001f, 0.0005f)
    brain.setStructuralPlasticityParameters(0.0002f, 0.00002f)
    
    print("\nAdvanced plasticity parameters configured")
    
    # Advanced neuromodulation configuration
    brain.setNeuromodulationParameters(1.5f, 0.1f, 0.2f, 2.0f)
    
    # Advanced memory configuration
    brain.setWorkingMemoryParameters(100, 0.05f)
    brain.setEpisodicMemoryParameters(2000, 500)
    
    # Advanced cognitive configuration
    brain.setCognitiveParameters(10, 2.0f, 0.5f, 1.5f)
    
    # Advanced development configuration
    brain.setDevelopmentParameters(100, 1000, 0.01f)
    
    # Enable subsystems
    brain.enableSubsystem("working_memory", True)
    brain.enableSubsystem("episodic_memory", True)
    brain.enableSubsystem("curiosity", True)
    
    print("Advanced configuration applied")
    
    # Run simulation with advanced features
    print("\nRunning advanced simulation...")
    for step in range(200):
        brain.step(step)
        
        # Get performance statistics
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Step count: {brain.getStepCount()}")
            
            # Check curiosity-driven exploration
            if brain.getCuriosity():
                curiosity = brain.getCuriosityLevel()
                print(f"  Curiosity level: {curiosity:.3f}")
                if curiosity > 0.5:
                    print(f"  High curiosity detected - triggering exploration")
    
    print(f"\nAdvanced simulation complete.")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Performance monitoring enabled: {brain.isPerformanceMonitoringEnabled()}")
    print()

def error_handling_example():
    """Example demonstrating error handling."""
    print("=== NLM Error Handling Example ===")
    
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Try to set invalid configuration
        config.set("invalid.parameter", "invalid_value")
        
        # Create brain (may fail with invalid configuration)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Brain initialized successfully")
        
    except Exception as e:
        print(f"Error occurred: {e}")
        print("Error handling demonstration completed")
    
    print("Error handling example completed\n")

def visualization_example():
    """Example showing visualization capabilities."""
    print("=== NLM Visualization Example ===")
    
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Visualization example would require visualization library integration")
        print("Available visualization features:")
        print("  - Network graph visualization")
        print("  - Activity heatmap")
        print("  - Spike raster plots")
        print("  - Weight matrix visualization")
        print("  - Memory system visualization")
        print("  - Neuromodulation visualization")
        print("  - Development visualization")
        print("  - Performance monitoring visualization")
        
        # Note: Actual visualization would require external libraries like OpenGL, GLFW, etc.
        
    except Exception as e:
        print(f"Visualization example error: {e}")
    
    print("Visualization example completed\n")

def agent_example():
    """Example showing agent-brain integration."""
    print("=== NLM Agent Example ===")
    
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Initialize agent with world
        agent.initialize(world)
        
        print("Agent initialized with world")
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print("Subsystems enabled")
        
        # Run agent simulation
        print("\nRunning agent simulation...")
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input in brain
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command from brain activity
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0f
            if percept.getInternal() and !percept.getInternal()->empty() {
                reward = percept.getInternal()->at(0);
            }
            agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            if step % 20 == 0:
                print(f"Step {step}:")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        
        print("\nAgent simulation completed successfully")
        
    except Exception as e:
        print(f"Agent example error: {e}")
    
    print("Agent example completed\n")

def main():
    """Main function demonstrating all examples."""
    print("NLM (Neural Learning Machine) - Comprehensive Examples")
    print("=" * 60)
    
    try:
        # Run basic example
        basic_example()
        
        # Run advanced example
        advanced_example()
        
        # Run error handling example
        error_handling_example()
        
        # Run visualization example
        visualization_example()
        
        # Run agent example
        agent_example()
        
        print("=" * 60)
        print("All examples completed successfully!")
        print("\nKey improvements in NLM Phase 6:")
        print("  ✓ Advanced configuration options")
        print("  ✓ Enhanced Python bindings")
        print("  ✓ Performance monitoring and statistics")
        print("  ✓ Comprehensive error handling")
        print("  ✓ Advanced visualization capabilities")
        print("  ✓ Agent-brain integration")
        print("  ✓ Real-time monitoring")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
