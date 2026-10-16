"""
NLM Python Bindings - Advanced Usage Examples
================================================

Advanced examples for experienced users who want to leverage
NLM's full capabilities.
"""

import pynlm
import math

def advanced_example_1_simulation_manager():
    """Example using the SimulationManager for complex simulations."""
    
    print("=== Advanced Example 1: Simulation Manager ===\n")
    
    # Create a simulation manager with custom configuration
    config = (pynlm.ConfigBuilder()
              .setNeuronCount(5000)
              .setRegionCount(3)
              .setConnectionProbability(0.1)
              .setSTDPParameters(0.02, 0.025, 50.0)
              .setStructuralPlasticity(0.0001, 0.00001)
              .setMemoryCapacity(2000, 1000)
              .enableNeuromodulation(True)
              .setDopamineLevel(0.8)
              .setSimulation(0.001, 5000)
              .build())
    
    sim = pynlm.SimulationManager(config)
    
    # Run steps
    sim.runSteps(2000)
    
    # Get detailed statistics
    stats = sim.getStatistics()
    print(f"Simulation completed with {stats['total_steps']} steps")
    print(f"Neurons: {stats['neuron_count']}")
    print(f"Synapses: {stats['synapse_count']}")
    print(f"Active neurons: {stats['active_neurons']}")
    print(f"Firing neurons: {stats['firing_neurons']}")
    print(f"Total spikes: {stats['total_spikes']}")
    print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"E/I ratio: {stats['e_i_ratio']:.2f}")
    
    if stats.get('working_memory_traces'):
        print(f"Working memory traces: {stats['working_memory_traces']}")
    
    if stats.get('episodic_memory_episodes'):
        print(f"Episodic memory episodes: {stats['episodic_memory_episodes']}")
    
    print("\n=== Example Complete ===\n")
    return sim

def advanced_example_2_agent_simulation():
    """Example using AgentManager for agent-based simulation."""
    
    print("=== Advanced Example 2: Agent Manager ===\n")
    
    # Create an agent manager
    agent = pynlm.AgentManager()
    
    # Configure the world
    agent.configureWorld(100, 100, 20, 20)
    agent.setAgentPosition(50.0, 50.0)
    
    # Set custom agent configuration
    import tempfile
    import json
    
    # Create a temporary config file
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
            "structural": {"enable": True, "synaptogenesis_rate": 0.0002}
        }
    }
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(config_data, f)
        config_file = f.name
    
    try:
        agent.setAgentConfig(config_file)
        
        # Run a complete episode
        agent.runEpisode(500)
        
        # Get results
        results = agent.getEpisodeResults()
        print(f"Episode completed: {results['total_steps']} steps")
        print(f"Neuron count: {results['neuron_count']}")
        print(f"Region count: {results['region_count']}")
        print(f"Final dopamine level: {results['final_dopamine']:.3f}")
        print(f"Final curiosity level: {results['final_curiosity']:.3f}")
        print(f"Final novelty level: {results['final_novelty']:.3f}")
        
        # Get world state
        world_state = agent.getWorldState()
        print(f"Agent position: ({world_state['x']:.1f}, {world_state['y']:.1f})")
        print(f"Agent orientation: {world_state['orientation']:.1f}°")
        print(f"Agent energy: {world_state['energy']:.1f}")
        print(f"Agent health: {world_state['health']:.1f}")
        print(f"Agent age: {world_state['age']:.0f} steps")
        
    finally:
        import os
        os.unlink(config_file)
    
    print("\n=== Example Complete ===\n")
    return agent

def advanced_example_3_neural_similarity():
    """Example demonstrating neural similarity calculations."""
    
    print("=== Advanced Example 3: Neural Similarity ===\n")
    
    # Create two neural activity patterns
    import random
    
    pattern1 = [random.uniform(0, 1) for _ in range(1000)]
    pattern2 = [random.uniform(0, 1) for _ in range(1000)]
    
    # Calculate similarity using NLM's neuralSimilarity function
    similarity = pynlm.neuralSimilarity(pattern1, pattern2)
    
    print(f"Pattern 1 length: {len(pattern1)}")
    print(f"Pattern 2 length: {len(pattern2)}")
    print(f"Neural similarity: {similarity:.4f}")
    
    # Create more similar patterns
    pattern3 = [x * 0.9 + random.uniform(-0.1, 0.1) for x in pattern1]
    similarity2 = pynlm.neuralSimilarity(pattern1, pattern3)
    
    print(f"Similar pattern similarity: {similarity2:.4f}")
    
    print("\n=== Example Complete ===\n")
    return similarity

def advanced_example_4_preset_configs():
    """Example using preset configurations."""
    
    print("=== Advanced Example 4: Preset Configurations ===\n")
    
    # Get different preset configurations
    default_config = pynlm.getPresetConfig("default")
    performance_config = pynlm.getPresetConfig("performance")
    research_config = pynlm.getPresetConfig("research")
    
    print("Default config:")
    print(f"  Neurons: {default_config.getOr<int64_t>('neuron_count', 0)}")
    print(f"  Regions: {default_config.getOr<int64_t>('region_count', 0)}")
    print(f"  Connection probability: {default_config.getOr<float>('connection_probability', 0):.3f}")
    print(f"  Timestep: {default_config.getOr<double>('simulation_timestep', 0):.4f}")
    
    print("\nPerformance config:")
    print(f"  Neurons: {performance_config.getOr<int64_t>('neuron_count', 0)}")
    print(f"  Neuromodulation enabled: {performance_config.getOr<bool>('enable_neuromodulation', False)}")
    
    print("\nResearch config:")
    print(f"  Neurons: {research_config.getOr<int64_t>('neuron_count', 0)}")
    print(f"  Regions: {research_config.getOr<int64_t>('region_count', 0)}")
    print(f"  Max steps: {research_config.getOr<int64_t>('max_steps', 0)}")
    print(f"  Neuromodulation enabled: {research_config.getOr<bool>('enable_neuromodulation', False)}")
    print(f"  Development enabled: {research_config.getOr<bool>('enable_development', False)}")
    
    # Validate configurations
    print("\nConfiguration validation:")
    print(f"  Default config valid: {pynlm.validateConfig(default_config)}")
    print(f"  Performance config valid: {pynlm.validateConfig(performance_config)}")
    print(f"  Research config valid: {pynlm.validateConfig(research_config)}")
    
    print("\n=== Example Complete ===\n")

def advanced_example_5_state_management():
    """Example demonstrating state save/load functionality."""
    
    print("=== Advanced Example 5: State Management ===\n")
    
    # Create and run a simulation
    config = pynlm.ConfigBuilder().setNeuronCount(1000).build()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps
    for i in range(100):
        brain.step(i)
    
    # Save brain state
    save_file = "/tmp/brain_checkpoint.nlm"
    brain.save(save_file)
    print(f"Saved brain state to: {save_file}")
    
    # Get statistics before reset
    stats_before = brain.getStatistics()
    print(f"Neurons before reset: {stats_before['neuron_count']}")
    print(f"Total spikes before reset: {stats_before['total_spikes']}")
    
    # Reset and load from saved state
    brain.reset()
    brain.load(save_file)
    
    # Get statistics after reload
    stats_after = brain.getStatistics()
    print(f"Neurons after reload: {stats_after['neuron_count']}")
    print(f"Total spikes after reload: {stats_after['total_spikes']}")
    
    # Clean up
    import os
    if os.path.exists(save_file):
        os.unlink(save_file)
    
    print("\n=== Example Complete ===\n")
    return True

def run_all_advanced_examples():
    """Run all advanced examples."""
    print("Running all advanced examples...\n")
    
    advanced_example_1_simulation_manager()
    advanced_example_2_agent_simulation()
    advanced_example_3_neural_similarity()
    advanced_example_4_preset_configs()
    advanced_example_5_state_management()
    
    print("All advanced examples completed successfully!")

if __name__ == "__main__":
    run_all_advanced_examples()