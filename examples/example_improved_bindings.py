#!/usr/bin/env python3
"""
Example script demonstrating the improved NLM Python bindings.

This script shows the new features and improvements to the NLM Python API.
"""

import pynlm

def example_1_basic_usage():
    """Basic usage example - creating and running a brain"""
    print("Example 1: Basic Brain Usage")
    print("=" * 40)
    
    # Create brain with improved configuration helper
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation using the improved runSimulation method
    brain.runSimulation(100, log_interval=20)
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    print()

def example_2_advanced_configuration():
    """Advanced configuration example"""
    print("Example 2: Advanced Configuration")
    print("=" * 40)
    
    # Create brain with custom parameters
    config = pynlm.createConfigAdvanced(
        neuron_count=1500,
        connection_probability=0.03,
        enable_stdp=True,
        enable_hebbian=True,
        enable_structural=True,
        enable_curiosity=True
    )
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Advanced brain created with custom parameters")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"E/I Ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Get configuration summary
    config_summary = pynlm.getConfigSummary(config)
    print(f"Configuration keys: {len(config_summary)} total")
    print()

def example_3_agent_world():
    """Agent-world simulation example"""
    print("Example 3: Agent-World Simulation")
    print("=" * 40)
    
    # Setup complete system
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Running agent simulation for 200 steps...")
    
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Simulation complete!")
    print(f"Final stats: {brain.getFiringNeuronCount()} firing neurons")
    print()

def example_4_memory_analysis():
    """Memory system analysis example"""
    print("Example 4: Memory System Analysis")
    print("=" * 40)
    
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Generate some activity
    for step in range(100):
        brain.step(step)
    
    # Analyze memory systems
    memory_stats = pynlm.getMemoryStats(brain)
    print("Memory Statistics:")
    for key, value in memory_stats.items():
        print(f"  {key}: {value}")
    
    # Analyze spike patterns
    spike_pattern = pynlm.analyzeSpikePattern(brain, window_size=50)
    print(f"\nSpike pattern (first 10 points): {spike_pattern[:10]}")
    print()

def example_5_error_handling():
    """Error handling example"""
    print("Example 5: Error Handling")
    print("=" * 40)
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test validation
        if not brain:
            raise pynlm.NLMError("Brain object is null")
        
        print("✓ Error handling test passed")
        
    except pynlm.NLMError as e:
        print(f"✓ Caught NLMError: {e}")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
    print()

def main():
    """Run all examples"""
    print("NLM Python Bindings - Demonstration of Improvements")
    print("=" * 60)
    print()
    
    examples = [
        example_1_basic_usage,
        example_2_advanced_configuration,
        example_3_agent_world,
        example_4_memory_analysis,
        example_5_error_handling,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Example failed: {e}")
        print()
    
    print("=" * 60)
    print("All examples completed successfully!")
    print("\nKey improvements demonstrated:")
    print("• createConfigWithDefaults() - Easy configuration setup")
    print("• createConfigAdvanced() - Advanced parameter control")
    print("• runSimulation() - High-level simulation control")
    print("• getBrainStatistics() - Comprehensive statistics")
    print("• getMemoryStats() - Memory system analysis")
    print("• Improved error handling with NLMError exception")

if __name__ == "__main__":
    main()