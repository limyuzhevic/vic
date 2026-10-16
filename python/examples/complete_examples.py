"""
NLM Python Bindings - Complete Working Examples
=================================================

Complete working examples that demonstrate various NLM features
and their practical applications.
"""

import pynlm
import json
import tempfile
from typing import List, Dict, Any

def complete_example_1_neural_network_simulation():
    """Complete example: Neural network simulation with learning."""
    
    print("=== Complete Example 1: Neural Network Simulation ===\n")
    
    # Initialize simulation
    config = (pynlm.ConfigBuilder()
              .setNeuronCount(2000)
              .setRegionCount(2)
              .setConnectionProbability(0.1)
              .setSTDPParameters(0.01, 0.012, 20.0)
              .setStructuralPlasticity(0.0001, 0.00001)
              .setMemoryCapacity(500, 200)
              .enableNeuromodulation(True)
              .setDopamineLevel(0.5)
              .enableDevelopment(True)
              .setSimulation(0.001, 2000)
              .build())
    
    sim = pynlm.SimulationManager(config)
    
    # Create some synthetic sensory input data
    sensory_inputs = []
    for step in range(100):
        # Create simple vision-like input (width=10, height=10, channels=3)
        input_data = []
        for i in range(300):  # 10 * 10 * 3
            input_data.append((i % 255) / 255.0)
        sensory_inputs.append(input_data)
    
    # Create corresponding actions (just placeholders for this example)
    action_indices = list(range(100))
    
    # Run episode with sensory input
    sim.runEpisode(sensory_inputs, action_indices)
    
    # Get results
    stats = sim.getStatistics()
    print(f"Simulation completed with {len(sensory_inputs)} time steps")
    print(f"Final statistics:")
    print(f"  Neurons: {stats['neuron_count']}")
    print(f"  Synapses: {stats['synapse_count']}")
    print(f"  Active neurons: {stats['active_neurons']}")
    print(f"  Firing neurons: {stats['firing_neurons']}")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"  E/I ratio: {stats['e_i_ratio']:.2f}")
    
    # Save state
    sim.saveState("/tmp/network_simulation.state")
    print(f"State saved to: /tmp/network_simulation.state")
    
    print("\n=== Example Complete ===\n")
    return sim

def complete_example_2_multi_agent_environment():
    """Complete example: Multi-agent environment simulation."""
    
    print("=== Complete Example 2: Multi-Agent Environment ===\n")
    
    # Create multiple agents with different configurations
    agents = []
    
    # Agent 1: Fast and responsive
    config1 = pynlm.getPresetConfig("performance")
    config1->set("max_steps", 1000)
    config1->set("enable_neuromodulation", True)
    
    agent1 = pynlm.AgentManager()
    agent1.configureWorld(50, 50, 10, 10)
    agent1.setAgentPosition(25.0, 25.0)
    
    # Agent 2: Slow but learning
    config2 = pynlm.getPresetConfig("research")
    config2->set("max_steps", 2000)
    config2->set("enable_development", True)
    
    agent2 = pynlm.AgentManager()
    agent2.configureWorld(80, 80, 15, 15)
    agent2.setAgentPosition(40.0, 40.0)
    
    # Create a temporary config file for demonstration
    config_data = {
        "brain": {
            "neuron_count": 3000,
            "region_count": 2,
            "connection_probability": 0.12,
            "simulation_timestep": 0.001
        },
        "plasticity": {
            "stdp": {"enable": True, "learning_rate": 0.001},
            "hebbian": {"enable": True},
            "structural": {"enable": True}
        }
    }
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(config_data, f)
        config_file = f.name
    
    try:
        # Configure agents with shared config
        agent1.setAgentConfig(config_file)
        agent2.setAgentConfig(config_file)
        
        # Run agents in sequence
        print("Running Agent 1 (performance mode)...")
        agent1.runEpisode(500)
        results1 = agent1.getEpisodeResults()
        print(f"  Completed {results1['total_steps']} steps")
        print(f"  Final curiosity: {results1['final_curiosity']:.3f}")
        
        print("\nRunning Agent 2 (research mode)...")
        agent2.runEpisode(1000)
        results2 = agent2.getEpisodeResults()
        print(f"  Completed {results2['total_steps']} steps")
        print(f"  Final novelty: {results2['final_novelty']:.3f}")
        
        # Compare results
        print("\nComparison:")
        print(f"  Agent 1 total steps: {results1['total_steps']}")
        print(f"  Agent 2 total steps: {results2['total_steps']}")
        print(f"  Agent 1 final dopamine: {results1['final_dopamine']:.3f}")
        print(f"  Agent 2 final dopamine: {results2['final_dopamine']:.3f}")
        
        # Get final world states
        state1 = agent1.getWorldState()
        state2 = agent2.getWorldState()
        
        print(f"\nAgent 1 position: ({state1['x']:.1f}, {state1['y']:.1f})")
        print(f"Agent 2 position: ({state2['x']:.1f}, {state2['y']:.1f})")
        
    finally:
        import os
        os.unlink(config_file)
    
    print("\n=== Example Complete ===\n")
    return [agent1, agent2]

def complete_example_3_learning_curves_analysis():
    """Complete example: Analyzing learning curves over time."""
    
    print("=== Complete Example 3: Learning Curves Analysis ===\n")
    
    # Simulate with different parameters
    parameters = [
        {"neurons": 500, "plasticity": 0.001, "name": "Slow Learning"},
        {"neurons": 2000, "plasticity": 0.01, "name": "Medium Learning"},
        {"neurons": 5000, "plasticity": 0.1, "name": "Fast Learning"}
    ]
    
    learning_data = {}
    
    for param in parameters:
        print(f"Running {param['name']} (neurons={param['neurons']}, plasticity={param['plasticity']})...")
        
        config = (pynlm.ConfigBuilder()
                  .setNeuronCount(param['neurons'])
                  .setRegionCount(1)
                  .setConnectionProbability(0.05)
                  .setSTDPParameters(0.01, 0.012, 20.0)
                  .set("plasticity.stdp.learning_rate", param['plasticity'])
                  .setMemoryCapacity(100, 50)
                  .enableNeuromodulation(True)
                  .enableDevelopment(False)
                  .setSimulation(0.001, 500)
                  .build())
        
        sim = pynlm.SimulationManager(config)
        
        # Track learning metrics over time
        spike_counts = []
        firing_rates = []
        
        for step in range(500):
            sim.runSteps(1)
            stats = sim.getStatistics()
            spike_counts.append(stats['total_spikes'])
            firing_rates.append(stats['average_firing_rate'])
        
        learning_data[param['name']] = {
            'spike_counts': spike_counts,
            'firing_rates': firing_rates,
            'final_firing_rate': firing_rates[-1],
            'total_spikes': spike_counts[-1]
        }
        
        print(f"  Final firing rate: {firing_rates[-1]:.2f} Hz")
        print(f"  Total spikes: {spike_counts[-1]}")
    
    # Analyze and compare
    print("\nLearning Analysis:")
    print("-" * 50)
    
    for name, data in learning_data.items():
        print(f"{name}:")
        print(f"  Final firing rate: {data['final_firing_rate']:.2f} Hz")
        print(f"  Total spikes: {data['total_spikes']}")
        print(f"  Average firing rate: {sum(data['firing_rates']) / len(data['firing_rates']):.2f} Hz")
    
    print("\n=== Example Complete ===\n")
    return learning_data

def complete_example_4_neural_similarity_benchmark():
    """Complete example: Neural similarity benchmark with various patterns."""
    
    print("=== Complete Example 4: Neural Similarity Benchmark ===\n")
    
    # Generate different types of neural patterns
    import random
    import numpy as np
    
    patterns = {}
    
    # 1. Random patterns
    patterns['random'] = [random.uniform(0, 1) for _ in range(1000)]
    
    # 2. Gaussian patterns
    mean = 0.5
    std = 0.1
    patterns['gaussian'] = [random.gauss(mean, std) for _ in range(1000)]
    
    # 3. Spiking patterns
    patterns['spiking'] = [0.0 if i % 10 != 0 else 1.0 for i in range(1000)]
    
    # 4. Sigmoid patterns
    patterns['sigmoid'] = [1.0 / (1.0 + np.exp(-2 * (i/1000 - 0.5))) for i in range(1000)]
    
    # 5. Gradient patterns
    patterns['gradient'] = [(i / 999) for i in range(1000)]
    
    # Calculate pairwise similarities
    print("Calculating pairwise similarities:")
    print("-" * 60)
    
    similarities = {}
    pattern_names = list(patterns.keys())
    
    for i, name1 in enumerate(pattern_names):
        similarities[name1] = {}
        for j, name2 in enumerate(pattern_names):
            if i < j:
                similarity = pynlm.neuralSimilarity(patterns[name1], patterns[name2])
                similarities[name1][name2] = similarity
                similarities[name2][name1] = similarity
                print(f"  {name1:10s} vs {name2:10s}: {similarity:.4f}")
    
    # Find most similar patterns
    print("\nMost Similar Pattern Pairs:")
    print("-" * 40)
    
    similar_pairs = []
    for name1 in pattern_names:
        for name2 in pattern_names:
            if name1 < name2:
                sim = similarities[name1][name2]
                similar_pairs.append((sim, name1, name2))
    
    similar_pairs.sort(reverse=True)
    
    for sim, name1, name2 in similar_pairs[:5]:
        print(f"  {name1:10s} ↔ {name2:10s}: {sim:.4f}")
    
    # Compare with original patterns
    print("\nSimilarity with original pattern1:")
    print("-" * 40)
    
    if 'random' in pattern_names:
        base_pattern = patterns['random']
        print(f"  Original pattern (baseline): {similarities['random']['random']:.4f}")
        
        for name in pattern_names:
            if name != 'random':
                sim = similarities['random'][name]
                print(f"  {name:10s}: {sim:.4f}")
    
    print("\n=== Example Complete ===\n")
    return patterns, similarities

def complete_example_5_config_validation():
    """Complete example: Configuration validation and optimization."""
    
    print("=== Complete Example 5: Configuration Validation ===\n")
    
    # Create various configurations to validate
    configs_to_test = []
    
    # 1. Minimal valid config
    config1 = pynlm.createDefaultConfig()
    configs_to_test.append({"name": "default", "config": config1})
    
    # 2. Performance config
    config2 = pynlm.getPresetConfig("performance")
    configs_to_test.append({"name": "performance", "config": config2})
    
    # 3. Research config
    config3 = pynlm.getPresetConfig("research")
    configs_to_test.append({"name": "research", "config": config3})
    
    # 4. Custom config
    config4 = (pynlm.ConfigBuilder()
               .setNeuronCount(10000)
               .setRegionCount(5)
               .setConnectionProbability(0.2)
               .setSTDPParameters(0.005, 0.006, 10.0)
               .setStructuralPlasticity(0.00005, 0.000005)
               .setMemoryCapacity(5000, 2000)
               .enableNeuromodulation(True)
               .enableDevelopment(True)
               .setSimulation(0.002, 10000)
               .build())
    configs_to_test.append({"name": "custom", "config": config4})
    
    # Test each configuration
    results = {}
    
    for test_case in configs_to_test:
        name = test_case['name']
        config = test_case['config']
        
        print(f"Testing {name} configuration...")
        print(f"  Validation result: {pynlm.validateConfig(config)}")
        
        # Try to create brain and initialize
        try:
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Check if valid
            stats = brain.getStatistics()
            results[name] = {
                'valid': True,
                'neurons': stats['neuron_count'],
                'synapses': stats['synapse_count'],
                'active_neurons': stats['active_neurons']
            }
            
            print(f"  Created successfully: {stats['neuron_count']} neurons")
            
        except Exception as e:
            results[name] = {
                'valid': False,
                'error': str(e)
            }
            print(f"  Failed: {str(e)}")
    
    # Summarize results
    print("\nConfiguration Validation Summary:")
    print("-" * 50)
    
    valid_configs = 0
    for name, result in results.items():
        status = "✓ Valid" if result['valid'] else "✗ Invalid"
        print(f"  {name:10s}: {status}")
        
        if result['valid']:
            valid_configs += 1
            print(f"    Neurons: {result['neurons']}")
            print(f"    Synapses: {result['synapses']}")
            print(f"    Active neurons: {result['active_neurons']}")
    
    print(f"\nSummary: {valid_configs}/{len(configs_to_test)} configurations valid")
    
    print("\n=== Example Complete ===\n")
    return results

def run_all_complete_examples():
    """Run all complete examples."""
    print("Running all complete examples...\n")
    
    complete_example_1_neural_network_simulation()
    complete_example_2_multi_agent_environment()
    complete_example_3_learning_curves_analysis()
    complete_example_4_neural_similarity_benchmark()
    complete_example_5_config_validation()
    
    print("All complete examples completed successfully!")

if __name__ == "__main__":
    run_all_complete_examples()