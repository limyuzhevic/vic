#!/usr/bin/env python3
"""
Enhanced NLM Python Bindings - Example Usage

This script demonstrates the enhanced functionality of the NLM Python bindings
for Phase 6: Final Integration of Artificial Brain Systems.

To run:
1. Build the enhanced bindings:
   cd /workspace/app
   mkdir build
   cd build
   cmake ..
   make -j4
   cd python
   pip install -e .

2. Run examples:
   python examples/basic_usage.py
   python examples/advanced_features.py
   python examples/brain_introspection.py
"""

import pynlm
import sys
import time

def print_header(title):
    """Print formatted section header"""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

def basic_usage_example():
    """Example 1: Basic usage of enhanced bindings"""
    print_header("Example 1: Basic Usage")
    
    # Create brain with enhanced config
    config = pynlm.createDefaultConfig()
    print(f"Created default config: {config.summary()}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a few steps
    for i in range(10):
        brain.step(i)
        if i % 5 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("Basic example completed successfully!")

def config_presets_example():
    """Example 2: Using configuration presets"""
    print_header("Example 2: Configuration Presets")
    
    # Show available presets
    print("Available presets:", pynlm.PRESETS.keys())
    
    # Use minimal preset
    print("\nUsing minimal preset:")
    config = pynlm.getMinimalConfig()
    print(f"  Neurons: {config.get('brain.neuron_count', 'NOT SET')}")
    print(f"  Regions: {config.get('brain.region_count', 'NOT SET')}")
    
    # Use research preset
    print("\nUsing research preset:")
    config = pynlm.getResearchConfig()
    print(f"  Neurons: {config.get('brain.neuron_count', 'NOT SET')}")
    print(f"  Regions: {config.get('brain.region_count', 'NOT SET')}")
    print(f"  STDP enabled: {config.get('plasticity.stdp.enable', 'NOT SET')}")
    
    print("\nPreset example completed successfully!")

def brain_introspection_example():
    """Example 3: Advanced brain introspection"""
    print_header("Example 3: Brain Introspection")
    
    # Create research configuration
    config = pynlm.getResearchConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get comprehensive statistics
    neuron_stats = brain.getNeuronStats()
    print("Neuron Statistics:")
    for key, value in neuron_stats.items():
        print(f"  {key}: {value}")
    
    # Get region information
    regions = brain.getRegionStats()
    print(f"\nBrain Regions ({len(regions)} regions):")
    for region in regions:
        print(f"  Region {region['id']}: {region['name']}")
        print(f"    Neurons: {region['neuron_count']}, Synapses: {region['synapse_count']}")
        print(f"    Average weight: {region['avg_weight']:.4f}")
    
    # Get neuromodulation status
    neuromod_stats = brain.getNeuromodulationStats()
    print(f"\nNeuromodulation Status:")
    for key, value in neuromod_stats.items():
        print(f"  {key}: {value:.4f}")
    
    print("\nBrain introspection example completed successfully!")

def enhanced_agent_example():
    """Example 4: Enhanced agent with neuromodulation"""
    print_header("Example 4: Enhanced Agent with Neuromodulation")
    
    # Create brain, world, and agent
    config = pynlm.getStandardConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Agent initialized with sensory input size: {agent.getSensoryInputSize()}")
    print(f"Agent motor output size: {agent.getMotorOutputSize()}")
    
    # Run simulation
    print("\nRunning enhanced agent simulation:")
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print status every 10 steps
        if step % 10 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
    
    print("\nEnhanced agent example completed successfully!")

def batch_operations_example():
    """Example 5: Batch operations for performance"""
    print_header("Example 5: Batch Operations for Performance")
    
    # Create brain
    config = pynlm.getMinimalConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Testing batch step operation...")
    start_time = time.time()
    
    # Use batch operation
    brain.stepMultiple(0, 1000, 0.001)
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"Completed 1000 steps in {duration:.3f} seconds")
    print(f"Average time per step: {duration/1000*1000:.2f} ms")
    print(f"Neurons processed: {brain.getTotalNeuronCount() * 1000}")
    
    print("\nBatch operations example completed successfully!")

def episode_example():
    """Example 6: Complete episode with convenience methods"""
    print_header("Example 6: Complete Episode")
    
    # Create environment
    config = pynlm.getStandardConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure environment
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run complete episode
    print("Running complete episode simulation:")
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics
        if step % 20 == 0:
            print(f"  Step {step}:")
            print(f"    Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"    Orientation: {world.getAgentBody().orientation:.1f}")
            print(f"    Energy: {world.getAgentBody().energy:.1f}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"\nEpisode completed!")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    
    print("\nEpisode example completed successfully!")

def main():
    """Main function to run all examples"""
    print(f"NLM Python Bindings Enhanced Examples")
    print(f"Version: {pynlm.version()}")
    
    try:
        # Run all examples
        basic_usage_example()
        config_presets_example()
        brain_introspection_example()
        enhanced_agent_example()
        batch_operations_example()
        episode_example()
        
        print(f"\n{'='*60}")
        print("All examples completed successfully!")
        print(f"{'='*60}")
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
