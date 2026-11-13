#!/usr/bin/env python3
"""
Example 5: Batch Processing with Different Configurations

This example demonstrates how to run multiple simulations with different
configurations using a batch processing approach.
"""

import pynlm
import time

def run_single_simulation(config_dict, num_steps=50):
    """Run a single simulation with given configuration."""
    print(f"  Running simulation with config: {config_dict}")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    for key, value in config_dict.items():
        config.set(key, value)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Return results
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'dev_stage': brain.getDevelopmentalStage()
    }

def create_batch_processing_example():
    """Create batch processing example."""
    print("=== Batch Processing Example ===")
    
    # Define different configurations to test
    configurations = [
        {"brain.neuron_count": 100, "brain.region_count": 1, "brain.connection_probability": 0.05},
        {"brain.neuron_count": 500, "brain.region_count": 3, "brain.connection_probability": 0.1},
        {"brain.neuron_count": 1000, "brain.region_count": 5, "brain.connection_probability": 0.2},
        {"brain.neuron_count": 2000, "brain.region_count": 8, "brain.connection_probability": 0.15}
    ]
    
    print(f"1. Running {len(configurations)} simulations...")
    print(f"   Each simulation will run for 50 steps.\n")
    
    all_results = []
    
    for i, config in enumerate(configurations):
        print(f"[{i+1}/{len(configurations)}]")
        result = run_single_simulation(config)
        all_results.append((config, result))
        
        print(f"   Results:")
        print(f"     Total spikes: {result['total_spikes']}")
        print(f"     Firing neurons: {result['firing_neurons']}")
        print(f"     Average firing rate: {result['avg_firing_rate']:.2f} Hz")
        print(f"     Developmental stage: {result['dev_stage']}")
        print()
    
    # Analyze results
    print("2. Batch analysis:")
    print(f"   Total simulations: {len(all_results)}")
    
    total_spikes = sum(r['total_spikes'] for _, r in all_results)
    avg_firing_rate = sum(r['avg_firing_rate'] for _, r in all_results) / len(all_results)
    
    print(f"   Average total spikes across all simulations: {total_spikes / len(all_results):.1f}")
    print(f"   Average firing rate across all simulations: {avg_firing_rate:.2f} Hz")
    
    # Find best performing configuration
    best_config = max(all_results, key=lambda x: x[1]['total_spikes'])
    print(f"   Best performing config: {best_config[0]}")
    print(f"   Best total spikes: {best_config[1]['total_spikes']}")
    
    return all_results

if __name__ == "__main__":
    results = create_batch_processing_example()
    print("\n✓ Batch processing example completed successfully!")