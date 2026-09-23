#!/usr/bin/env python3
"""
NLM Advanced Example

This script demonstrates advanced NLM features including neural memory,
attention mechanisms, and performance optimization.
"""

import sys
sys.path.insert(0, "../src")

from core.Config.Config import Config
from brain.Brain import Brain
from brain.NeuronType import NeuronType
from memory.Memory import WorkingMemory, EpisodicMemory
from memory.NeuralWorkingMemory import NeuralWorkingMemory
from memory.NeuralEpisodicMemory import NeuralEpisodicMemory
from memory.NeuralAssociativeMemory import NeuralAssociativeMemory
from motor.Action import ActionType


def main():
    print("=== NLM Advanced Example ===\n")
    
    # Create configuration
    config = Config()
    config.load_from_file("configs/advanced_config.yaml")
    
    print(f"Configuration loaded:")
    print(f"  Max steps: {config.get_or('max_steps', 5000)}")
    print(f"  Time step: {config.get_or('time_step', 0.001)}")
    print(f"  Use neural memory: {config.get_or('use_neural_memory', True)}")
    
    # Create brain with advanced settings
    brain = Brain()
    brain.set_max_steps(config.get_or("max_steps", 5000))
    brain.set_time_step(config.get_or("time_step", 0.001))
    brain.set_leak_conductance(config.get_or("leak_conductance", 10.0))
    brain.set_threshold(config.get_or("neuron_threshold", -55.0))
    
    # Create neural regions with different neuron types
    brain.create_region("sensory_excitatory", 8, NeuronType.EXCITATORY)
    brain.create_region("sensory_inhibitory", 4, NeuronType.INHIBITORY)
    brain.create_region("motor_excitatory", 6, NeuronType.EXCITATORY)
    brain.create_region("motor_inhibitory", 3, NeuronType.INHIBITORY)
    brain.create_region("associative", 15, NeuronType.EXCITATORY)
    brain.create_region("cognitive", 10, NeuronType.EXCITATORY)
    brain.create_region("internal", 12, NeuronType.EXCITATORY)
    
    print(f"\nCreated {len(brain.get_all_regions())} neural regions")
    
    # Setup memory systems
    print(f"\nSetting up memory systems...")
    
    # Basic memory systems
    brain.create_working_memory(capacity=config.get_or("working_memory_capacity", 500))
    brain.create_episodic_memory(max_episodes=config.get_or("episodic_memory_capacity", 5000))
    brain.create_semantic_memory()
    brain.create_procedural_memory()
    brain.create_associative_memory()
    
    # Neural memory systems (if configured)
    use_neural_memory = config.get_or("use_neural_memory", True)
    if use_neural_memory:
        print("  Initializing neural memory systems...")
        
        # Initialize brain reference for neural memory
        brain_ref = brain
        
        # Create neural working memory (using neural mechanisms)
        neural_working_mem = NeuralWorkingMemory()
        neural_working_mem.initialize(brain_ref)
        
        # Create neural episodic memory (with neural encoding)
        neural_episodic_mem = NeuralEpisodicMemory()
        neural_episodic_mem.initialize(brain_ref)
        
        # Create neural associative memory (with pattern completion)
        neural_assoc_mem = NeuralAssociativeMemory()
        neural_assoc_mem.initialize(brain_ref)
        
        print("  Neural memory systems initialized")
    
    # Setup connections with different weights and types
    print(f"\nSetting up synaptic connections...")
    
    regions = brain.get_all_regions()
    total_synapses = 0
    
    for region_name, region in regions.items():
        neurons = region.get_all_neurons()
        if len(neurons) < 2:
            continue
            
        for i, source_neuron in enumerate(neurons):
            # Connect to neurons in other regions
            for other_region_name, other_region in regions.items():
                if region_name == other_region_name:
                    # Recurrent connections within region
                    if i > 0 and other_region_name in ["associative", "cognitive", "internal"]:
                        target_neuron = neurons[(i + 1) % len(neurons)]
                        weight = config.get_or("recurrent_weight", 0.2)
                        brain.create_synapse(source_neuron, target_neuron, weight)
                        total_synapses += 1
                else:
                    # Connections between regions
                    target_neurons = other_region.get_all_neurons()
                    if target_neurons:
                        # Connection strength based on region types
                        weight = config.get_or("inter_region_weight", 0.5)
                        if "inhibitory" in region_name:
                            weight *= 0.8
                        elif "sensory" in region_name:
                            weight *= 1.2
                        
                        # Add multiple connections
                        for _ in range(min(3, len(target_neurons))):
                            target_neuron = target_neurons[total_synapses % len(target_neurons)]
                            brain.create_synapse(source_neuron, target_neuron, weight)
                            total_synapses += 1
    
    print(f"  Total synaptic connections: {len(brain.get_all_synapses())}")
    
    # Store initial experience
    print(f"\nStoring initial experience...")
    
    initial_episode = EpisodicMemoryItem(
        timestamp=0,
        neurons=[neuron.get_id() for neuron in brain.get_all_neurons() if neuron.get_type() == NeuronType.EXCITATORY][:5],
        values=[0.8, 0.6, 0.9, 0.7, 0.5],
        metadata="Initial network configuration",
        action=ActionType.WAIT,
        reward=1.0
    )
    
    brain.get_episodic_memory().storeEpisode(initial_episode)
    print("  Initial experience stored")
    
    # Store semantic facts
    print(f"\nStoring semantic facts...")
    semantic_mem = brain.get_semantic_memory()
    
    semantic_facts = [
        ("population_burst", "High-frequency synchronized neural activity"),
        ("synaptic_plasticity", "Ability of synapses to strengthen or weaken over time"),
        ("neural_integration", "Combining multiple inputs to produce a single output"),
        ("working_memory_capacity", "Limited amount of information that can be held temporarily"),
        ("episodic_memory", "Memory of specific events and experiences")
    ]
    
    for key, value in semantic_facts:
        semantic_mem.store_fact(key, value)
    
    print(f"  Stored {len(semantic_facts)} semantic facts")
    
    # Store procedural skills
    print(f"\nStoring procedural skills...")
    procedural_mem = brain.get_procedural_memory()
    
    # Get all excitatory neurons for skill patterns
    all_neurons = brain.get_all_neurons()
    excitatory_neurons = [neuron for neuron in all_neurons if neuron.get_type() == NeuronType.EXCITATORY]
    
    if len(excitatory_neurons) >= 3:
        # Store skills using different neuron patterns
        procedural_mem.learn_skill("perception", 
                                   [n.get_id() for n in excitatory_neurons[:3]])
        procedural_mem.learn_skill("action", 
                                   [n.get_id() for n in excitatory_neurons[3:6]])
        procedural_mem.learn_skill("integration", 
                                   [n.get_id() for n in excitatory_neurons[6:9]])
        
        print(f"  Stored {len(procedural_mem.get_all_skills())} procedural skills")
    
    # Run simulation with enhanced monitoring
    print(f"\nRunning advanced simulation for {brain.get_max_steps()} steps...")
    
    spike_history = []
    memory_usage_history = []
    
    for step in range(brain.get_max_steps()):
        # Create dynamic input patterns
        import math
        time_factor = step / (brain.get_max_steps() / 10)
        
        # Different input patterns for different phases
        if step < brain.get_max_steps() * 0.3:
            # Learning phase - gradual increase
            input_strength = min(1.0, time_factor / 3)
        elif step < brain.get_max_steps() * 0.7:
            # Stable phase - oscillating input
            input_strength = 0.5 * (1 + math.sin(2 * math.pi * (step - brain.get_max_steps() * 0.3) / 200)) / 2
        else:
            # Testing phase - random exploration
            import random
            input_strength = random.random()
        
        # Apply input to multiple regions
        for region_name in ["sensory_excitatory", "motor_excitatory", "associative"]:
            if region_name in regions:
                neurons = regions[region_name].get_all_neurons()
                for neuron in neurons:
                    neuron.injectCurrent(input_strength * 3.0)
        
        # Step brain
        brain.step()
        
        # Record statistics
        stats = brain.get_statistics()
        spike_history.append(stats.average_firing_rate)
        
        mem_stats = brain.get_memory_statistics()
        memory_usage_history.append(
            mem_stats.working_memory_usage / mem_stats.working_memory_capacity
        )
        
        # Print progress
        if step % 1000 == 0:
            print(f"  Step {step}/{brain.get_max_steps()}: "
                  f"{stats.average_firing_rate:.2f} Hz, "
                  f"{mem_stats.working_memory_usage}/{mem_stats.working_memory_capacity} "
                  f"working memory used")
    
    # Analyze results
    print(f"\nSimulation Analysis:")
    
    import statistics
    
    # Spike rate statistics
    avg_spike_rate = statistics.mean(spike_history)
    max_spike_rate = max(spike_history)
    min_spike_rate = min(spike_history)
    
    # Memory usage statistics
    avg_memory_usage = statistics.mean(memory_usage_history)
    peak_memory_usage = max(memory_usage_history)
    
    # Calculate derived metrics
    total_steps = brain.get_max_steps()
    activity_phase = "oscillating" if max_spike_rate > 20 and min_spike_rate < 10 else "stable"
    
    print(f"  Spike Rate Statistics:")
    print(f"    Average: {avg_spike_rate:.2f} Hz")
    print(f"    Maximum: {max_spike_rate:.2f} Hz")
    print(f"    Minimum: {min_spike_rate:.2f} Hz")
    print(f"    Standard deviation: {statistics.stdev(spike_history):.2f} Hz")
    
    print(f"  Memory Usage Statistics:")
    print(f"    Average utilization: {avg_memory_usage*100:.1f}%")
    print(f"    Peak utilization: {peak_memory_usage*100:.1f}%")
    
    print(f"  System Behavior:")
    print(f"    Activity phase: {activity_phase}")
    print(f"    Neural stability: {'high' if statistics.stdev(spike_history) < 10 else 'moderate'}")
    print(f"    Memory efficiency: {'good' if avg_memory_usage > 0.5 else 'needs improvement'}")
    
    # Save detailed results
    print(f"\nSaving detailed results...")
    
    results_file = "advanced_example_results.txt"
    
    with open(results_file, "w") as f:
        f.write("NLM Advanced Example Results\n")
        f.write("=" * 50 + "\n\n")
        
        f.write(f"Simulation Parameters:\n")
        f.write(f"  Max steps: {brain.get_max_steps()}\n")
        f.write(f"  Time step: {brain.get_time_step():.4f} ms\n")
        f.write(f"  Leak conductance: {brain.get_leak_conductance():.1f} nS\n")
        f.write(f"  Neuron threshold: {brain.get_threshold():.1f} mV\n\n")
        
        f.write(f"Network Architecture:\n")
        for region_name, region in regions.items():
            neurons = region.get_all_neurons()
            f.write(f"  {region_name}: {len(neurons)} neurons\n")
        f.write(f"  Total synapses: {len(brain.get_all_synapses())}\n\n")
        
        f.write(f"Memory Statistics:\n")
        mem_stats = brain.get_memory_statistics()
        f.write(f"  Working memory: {mem_stats.working_memory_usage}/{mem_stats.working_memory_capacity}\n")
        f.write(f"  Episodic memory: {mem_stats.episodic_memory_episodes} episodes\n")
        f.write(f"  Semantic memory: {len(brain.get_semantic_memory().get_all_facts())} facts\n")
        f.write(f"  Procedural memory: {len(brain.get_procedural_memory().get_all_skills())} skills\n")
        f.write(f"  Associative memory: {mem_stats.associative_memory_associations} associations\n\n")
        
        f.write(f"Performance Metrics:\n")
        final_stats = brain.get_statistics()
        f.write(f"  Total simulation time: {final_stats.simulation_time:.2f} ms\n")
        f.write(f"  Total spikes: {final_stats.total_spikes}\n")
        f.write(f"  Average firing rate: {final_stats.average_firing_rate:.2f} Hz\n")
        f.write(f"  Max firing rate: {final_stats.max_firing_rate:.2f} Hz\n")
        f.write(f"  Min firing rate: {final_stats.min_firing_rate:.2f} Hz\n\n")
        
        f.write(f"Activity Analysis:\n")
        f.write(f"  Spike rate mean: {avg_spike_rate:.2f} Hz\n")
        f.write(f"  Spike rate std: {statistics.stdev(spike_history):.2f} Hz\n")
        f.write(f"  Memory utilization mean: {avg_memory_usage*100:.1f}%\n")
        f.write(f"  Memory utilization max: {peak_memory_usage*100:.1f}%\n\n")
        
        f.write(f"Learning Outcomes:\n")
        f.write(f"  Pattern recognition: {'detected' if len(brain.get_semantic_memory().get_all_facts()) > 3 else 'limited'}\n")
        f.write(f"  Skill acquisition: {'achieved' if len(brain.get_procedural_memory().get_all_skills()) > 2 else 'partial'}\n")
        f.write(f"  Memory consolidation: {'active' if mem_stats.episodic_memory_episodes > 100 else 'limited'}\n")
        f.write(f"  Association formation: {'established' if mem_stats.associative_memory_associations > 10 else 'developing'}\n")
    
    print(f"  Results saved to {results_file}")
    
    # Create visualization data
    print(f"\nCreating visualization data...")
    
    viz_file = "advanced_example_viz_data.json"
    
    viz_data = {
        "simulation_parameters": {
            "max_steps": brain.get_max_steps(),
            "time_step": brain.get_time_step(),
            "leak_conductance": brain.get_leak_conductance(),
            "threshold": brain.get_threshold()
        },
        "network_architecture": {
            "regions": list(regions.keys()),
            "total_neurons": sum(len(region.get_all_neurons()) for region in regions.values()),
            "total_synapses": len(brain.get_all_synapses())
        },
        "activity_metrics": {
            "average_firing_rate": avg_spike_rate,
            "max_firing_rate": max_spike_rate,
            "min_firing_rate": min_spike_rate,
            "spike_rate_std": statistics.stdev(spike_history) if len(spike_history) > 1 else 0,
            "average_memory_usage": avg_memory_usage,
            "peak_memory_usage": peak_memory_usage
        },
        "memory_system": {
            "working_memory_usage": mem_stats.working_memory_usage,
            "working_memory_capacity": mem_stats.working_memory_capacity,
            "episodic_memory_episodes": mem_stats.episodic_memory_episodes,
            "semantic_memory_facts": len(brain.get_semantic_memory().get_all_facts()),
            "procedural_memory_skills": len(brain.get_procedural_memory().get_all_skills()),
            "associative_memory_associations": mem_stats.associative_memory_associations
        }
    }
    
    import json
    with open(viz_file, "w") as f:
        json.dump(viz_data, f, indent=2)
    
    print(f"  Visualization data saved to {viz_file}")
    
    print(f"\n=== Advanced Example Complete ===")

if __name__ == "__main__":
    main()