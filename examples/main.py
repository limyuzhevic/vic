#!/usr/bin/env python3
"""
NLM Python Examples - Comprehensive examples for the NLM (Neural Learning Machine) framework.

This file contains examples demonstrating various aspects of the NLM Python API,
ranging from basic usage to advanced functionality.
"""

import pynlm
import time

def example_basic_brain():
    """Example 1: Basic brain simulation"""
    print("=" * 50)
    print("Example 1: Basic Brain Simulation")
    print("=" * 50)
    
    # Create brain with default configuration
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f" and {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation
    print("\nRunning simulation for 100 steps...")
    start_time = time.time()
    brain.runSimulation(100, log_interval=20)
    end_time = time.time()
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
    print()

def example_advanced_configuration():
    """Example 2: Advanced configuration"""
    print("=" * 50)
    print("Example 2: Advanced Configuration")
    print("=" * 50)
    
    # Create brain with custom parameters
    config = pynlm.createConfigAdvanced(
        neuron_count=2000,
        connection_probability=0.02,
        enable_stdp=True,
        enable_hebbian=True,
        enable_structural=True,
        enable_curiosity=True,
        enable_reward_modulation=True,
        config_file="configs/advanced.cfg"  # Optional config file
    )
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with advanced configuration")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Synapses: {brain.getTotalSynapseCount()}")
    print(f"E/I Ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Get configuration summary
    config_summary = pynlm.getConfigSummary(config)
    print(f"\nConfiguration keys: {list(config_summary.keys())[:5]}...")  # Show first 5 keys
    
    print()

def example_agent_world_simulation():
    """Example 3: Complete agent-world simulation"""
    print("=" * 50)
    print("Example 3: Complete Agent-World Simulation")
    print("=" * 50)
    
    # Setup the complete system
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Running agent-world simulation for 300 steps...")
    print("Step | Firing | Curiosity | Novelty | DevStage | Action")
    print("-" * 70)
    
    for step in range(300):
        # Update world
        world.update(timestep=0.1)
        
        # Get perception and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 50 steps
        if step % 50 == 0:
            dev_stage = brain.getDevelopmentalStage()
            action_str = action.getType().name if hasattr(action.getType(), 'name') else str(action.getType())
            print(f"{step:4d} | {brain.getFiringNeuronCount():6d} | "
                  f"{agent.getCuriosityLevel():.3f}  | {agent.getNoveltyLevel():.3f}  | "
                  f"{int(dev_stage):8d} | {action_str}")
    
    print("\nSimulation completed!")
    print(f"Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print()

def example_memory_system():
    """Example 4: Memory system exploration"""
    print("=" * 50)
    print("Example 4: Memory System Exploration")
    print("=" * 50)
    
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation to generate activity
    for step in range(200):
        brain.step(step)
    
    # Get memory statistics
    memory_stats = pynlm.getMemoryStats(brain)
    print("Memory System Statistics:")
    for key, value in memory_stats.items():
        print(f"  {key}: {value}")
    
    # Analyze spike patterns
    spike_pattern = pynlm.analyzeSpikePattern(brain, window_size=50)
    print(f"\nSpike pattern analysis (first 10 points): {spike_pattern[:10]}")
    print()

def example_development_tracking():
    """Example 5: Development tracking"""
    print("=" * 50)
    print("Example 5: Development Tracking")
    print("=" * 50)
    
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    print("Monitoring development over 500 steps...")
    print("Step | DevStage | Curiosity | Novelty | Neuromod")
    print("-" * 60)
    
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.decodeMotorCommand()  # Let brain control agent
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(0.0, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            dev_stats = pynlm.getDevelopmentStats(brain, agent)
            print(f"{step:4d} | {dev_stats['stage']:8d} | "
                  f"{dev_stats['curiosity_level']:.3f}  | "
                  f"{dev_stats['novelty_level']:.3f}  | "
                  f"{dev_stats['neuromodulation_level']:.3f}")
    
    print("\nDevelopment tracking complete!")
    print()

def example_error_handling():
    """Example 6: Error handling and validation"""
    print("=" * 50)
    print("Example 6: Error Handling and Validation")
    print("=" * 50)
    
    try:
        # Test error handling
        config = pynlm.createDefaultConfig()
        
        # This should work
        print("Creating brain...")
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Brain created successfully")
        
        # Test error scenarios
        print("\nTesting error handling...")
        
        # Test null checks
        if not brain:
            raise pynlm.NLMError("Brain object is null")
        
        # Test configuration validation
        config_summary = pynlm.getConfigSummary(config)
        print(f"✓ Configuration loaded with {len(config_summary)} keys")
        
        # Test statistics access
        stats = pynlm.getBrainStatistics(brain)
        print(f"✓ Brain statistics collected")
        
        print("\nAll error handling tests passed!")
        
    except Exception as e:
        print(f"Error occurred: {e}")
        print("Error handling test completed")
    
    print()

def example_performance_benchmark():
    """Example 7: Performance benchmarking"""
    print("=" * 50)
    print("Example 7: Performance Benchmarking")
    print("=" * 50)
    
    import time
    
    # Test different brain sizes
    brain_sizes = [500, 1000, 2000]
    
    for size in brain_sizes:
        print(f"\nBenchmarking brain with {size} neurons...")
        
        config = pynlm.createConfigAdvanced(neuron_count=size)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time simulation
        start_time = time.time()
        brain.runSimulation(200, log_interval=50)
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_sec = 200 / duration
        
        print(f"  Duration: {duration:.2f}s")
        print(f"  Steps/sec: {steps_per_sec:.1f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print("\nBenchmarking complete!")
    print()

def main():
    """Run all examples"""
    print("NLM Python Examples - Complete Demonstration")
    print("=" * 60)
    print()
    
    examples = [
        example_basic_brain,
        example_advanced_configuration,
        example_agent_world_simulation,
        example_memory_system,
        example_development_tracking,
        example_error_handling,
        example_performance_benchmark,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Example failed: {e}")
            print("Continuing with next example...")
            print()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)
    print("\nFor more detailed documentation, see:")
    print("  - HOW_TO_USE.md (basic usage)")
    print("  - easy_usage.md (beginner guide)")
    print("  - NLM documentation files in docs/ directory")

if __name__ == "__main__":
    main()