#!/usr/bin/env python3
"""
NLM Python Basic Examples

This script contains basic examples for using the enhanced NLM Python bindings.
These examples are designed for beginners and demonstrate fundamental usage patterns.
"""

import pynlm
from datetime import datetime

def example_1_simple_brain():
    """Example 1: Simple brain creation and basic operations"""
    print("=" * 60)
    print("Example 1: Simple Brain Creation")
    print("=" * 60)
    
    # Create a simple brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run some steps
    for i in range(10):
        brain.step(i)
    
    print(f"Total spikes after 10 steps: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Development stage: {brain.getDevelopmentalStage()}")

def example_2_basic_world():
    """Example 2: Basic world simulation"""
    print("\n" + "=" * 60)
    print("Example 2: Basic World Simulation")
    print("=" * 60)
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable basic features
    agent.enableRewardModulation(True)
    
    # Run simulation
    num_steps = 50
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Simulation completed with {num_steps} steps")
    print(f"Final state: Firing neurons: {brain.getFiringNeuronCount()}, "
          f"Total spikes: {brain.getTotalSpikeCount()}")

def example_3_configuration_management():
    """Example 3: Configuration management"""
    print("\n" + "=" * 60)
    print("Example 3: Configuration Management")
    print("=" * 60)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    
    print(f"Created config with neurons: {config.get('brain.neuron_count')}")
    print(f"Synapse density: {config.get('brain.synapse_density')}")
    
    # Save configuration
    config.saveToFile("test_config.json")
    print("✓ Configuration saved to 'test_config.json'")
    
    # Load configuration
    loaded_config = pynlm.createDefaultConfig()
    loaded_config.loadFromFile("test_config.json")
    print(f"✓ Configuration loaded: neurons = {loaded_config.get('brain.neuron_count')}")
    
    # Modify configuration
    loaded_config.set("brain.neuron_count", 1000)
    print(f"✓ Modified neuron count: {loaded_config.get('brain.neuron_count')}")

def example_4_error_handling():
    """Example 4: Error handling"""
    print("\n" + "=" * 60)
    print("Example 4: Error Handling")
    print("=" * 60)
    
    # Test error handling
    print("Testing error handling...")
    
    # Valid configuration should pass validation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    try:
        pynlm.validateConfig(config)
        print("✓ Configuration validation passed")
    except Exception as e:
        print(f"✗ Configuration validation failed: {e}")
    
    # Test with invalid configuration
    try:
        invalid_config = pynlm.createDefaultConfig()
        pynlm.validateConfig(invalid_config)  # Should fail - missing neuron count
        print("✗ Should have failed validation")
    except Exception as e:
        print(f"✓ Invalid configuration correctly rejected: {type(e).__name__}")

def example_5_agent_lifecycle():
    """Example 5: Complete agent lifecycle"""
    print("\n" + "=" * 60)
    print("Example 5: Complete Agent Lifecycle")
    print("=" * 60)
    
    # Create agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Agent initialized with:")
    print(f"  - Reward Modulation: {agent.isRewardModulationEnabled()}")
    print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
    
    # Run complete episode
    print("\nRunning complete episode...")
    for step in range(30):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 10 == 0:
            print(f"  Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
    
    print(f"\nEpisode completed successfully!")
    print(f"Final statistics:")
    print(f"  - Total Spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Firing Neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  - Curiosity Level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty Level: {agent.getNoveltyLevel():.3f}")

def example_6_benchmark():
    """Example 6: Performance benchmark"""
    print("\n" + "=" * 60)
    print("Example 6: Performance Benchmark")
    print("=" * 60)
    
    import time
    
    # Create brain for benchmarking
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Benchmark different step counts
    step_counts = [100, 500, 1000]
    
    for steps in step_counts:
        brain.reset()
        brain.initialize()
        
        start_time = time.time()
        for i in range(steps):
            brain.step(i)
        end_time = time.time()
        
        duration = end_time - start_time
        spikes = brain.getTotalSpikeCount()
        
        print(f"Steps: {steps:4d} | Duration: {duration:.3f}s | "
              f"Steps/sec: {steps/duration:8.1f} | "
              f"Spikes: {spikes:6d}")
    
    print("\n✓ Benchmarking completed")

def example_7_context_manager():
    """Example 7: Context manager usage"""
    print("\n" + "=" * 60)
    print("Example 7: Context Manager")
    print("=" * 60)
    
    # Create brain with context manager
    config = pynlm.createDefaultConfig()
    
    print("Using brain with context manager:")
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        print(f"  Brain created with {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation
        for i in range(20):
            brain.step(i)
        
        print(f"  After 20 steps: {brain.getTotalSpikeCount()} spikes")
    
    print("✓ Context manager completed automatically")
    print("✓ Brain properly cleaned up")

def example_8_statistics():
    """Example 8: Basic statistics"""
    print("\n" + "=" * 60)
    print("Example 8: Basic Statistics")
    print("=" * 60)
    
    # Create multiple brains for statistics
    brains = []
    
    for i in range(3):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200 + i * 100)
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Collect statistics
    total_neurons = [b.getTotalNeuronCount() for b in brains]
    total_synapses = [b.getTotalSynapseCount() for b in brains]
    total_spikes = [b.getTotalSpikeCount() for b in brains]
    
    print("Brain Statistics:")
    print(f"{'Brain':<8} {'Neurons':<10} {'Synapses':<10} {'Spikes':<10} {'Firing':<8}")
    print("-" * 55)
    
    for i, (b, n, s, sp, f) in enumerate(zip(brains, total_neurons, total_synapses, total_spikes, 
                                         [b.getFiringNeuronCount() for b in brains])):
        print(f"{'Brain ' + str(i+1):<8} {n:<10} {s:<10} {sp:<10} {f:<8}")
    
    # Summary statistics
    print(f"\nSummary Statistics:")
    print(f"  Average Neurons: {np.mean(total_neurons):.1f}")
    print(f"  Average Synapses: {np.mean(total_synapses):.1f}")
    print(f"  Average Total Spikes: {np.mean(total_spikes):.1f}")
    print(f"  Average Firing Neurons: {np.mean([b.getFiringNeuronCount() for b in brains]):.1f}")

def main():
    """Run all basic examples"""
    print("NLM Python Basic Examples")
    print("=" * 60)
    print("This script contains basic examples for using NLM Python bindings.")
    print()
    
    # Run all examples
    example_1_simple_brain()
    example_2_basic_world()
    example_3_configuration_management()
    example_4_error_handling()
    example_5_agent_lifecycle()
    example_6_benchmark()
    example_7_context_manager()
    example_8_statistics()
    
    print("\n" + "=" * 60)
    print("All basic examples completed successfully!")
    print("=" * 60)
    print("\nKey features demonstrated:")
    print("1. ✓ Basic brain creation and initialization")
    print("2. ✓ World and agent simulation")
    print("3. ✓ Configuration management")
    print("4. ✓ Error handling and validation")
    print("5. ✓ Complete agent lifecycle")
    print("6. ✓ Performance benchmarking")
    print("7. ✓ Context manager usage")
    print("8. ✓ Basic statistics and analysis")
    print("\nAll basic NLM Python features working correctly!")

if __name__ == "__main__":
    main()