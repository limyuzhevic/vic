#!/usr/bin/env python3
"""
Example 3: Brain Statistics and Analysis
Demonstrates brain analysis using new convenience methods and statistical functions
"""
import pynlm
import json

def brain_statistics_analysis():
    """Demonstrate comprehensive brain statistics analysis"""
    print("=== Example 3: Brain Statistics and Analysis ===\n")
    
    # 1. Create brain with specific configuration
    print("1. Creating brain with analysis-focused configuration:")
    config = pynlm.createDefaultConfig()
    
    # Configure for interesting analysis
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.12)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   Configuration loaded successfully")
    
    print()
    
    # 2. Initial brain state analysis
    print("2. Initial brain state analysis:")
    
    # Use new getBrainStats() method for comprehensive statistics
    initial_stats = brain.getBrainStats()
    print("   Initial brain statistics:")
    for key, value in initial_stats.items():
        if isinstance(value, float):
            print(f"     {key}: {value:.4f}")
        else:
            print(f"     {key}: {value}")
    
    # Analyze neuron types using new convenience method
    print("\n   Neuron type analysis:")
    neuron_types = [
        ("Excitatory", pynlm.NeuronType.Excitatory),
        ("Inhibitory", pynlm.NeuronType.Inhibitory),
        ("Sensory", pynlm.NeuronType.Sensory),
        ("Motor", pynlm.NeuronType.Motor),
        ("Modulatory", pynlm.NeuronType.Modulatory),
        ("Internal", pynlm.NeuronType.Internal)
    ]
    
    for type_name, type_enum in neuron_types:
        count = brain.getNeuronCountByType(type_enum)
        percentage = (count / brain.getTotalNeuronCount()) * 100
        print(f"     {type_name}: {count} neurons ({percentage:.1f}%)")
    
    print()
    
    # 3. Dynamic analysis during simulation
    print("3. Dynamic analysis during simulation:")
    
    simulation_data = []
    
    for step in range(10):
        brain.step(step)
        
        # Get stats at each step using new convenience methods
        step_stats = brain.getBrainStats()
        neuron_by_type = {
            "excitatory": brain.getNeuronCountByType(pynlm.NeuronType.Excitatory),
            "inhibitory": brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory),
            "sensory": brain.getNeuronCountByType(pynlm.NeuronType.Sensory),
            "motor": brain.getNeuronCountByType(pynlm.NeuronType.Motor)
        }
        
        current_step = {
            "step": step,
            "total_spikes": step_stats["total_spikes"],
            "firing_neurons": step_stats["firing_neurons"],
            "active_neurons": step_stats["active_neurons"],
            "firing_rate": step_stats["firing_rate"],
            "e_to_i_ratio": step_stats["e_to_i_ratio"],
            "neuron_by_type": neuron_by_type,
            "developmental_stage": step_stats["developmental_stage"]
        }
        
        simulation_data.append(current_step)
        
        print(f"   Step {step}: {step_stats['firing_rate']:.4f} firing rate, "
              f"{step_stats['firing_neurons']} firing neurons, "
              f"E/I ratio={step_stats['e_to_i_ratio']:.3f}")
    
    print()
    
    # 4. Statistical summary
    print("4. Statistical summary:")
    
    total_spikes = sum(d["total_spikes"] for d in simulation_data)
    max_firing_rate = max(d["firing_rate"] for d in simulation_data)
    avg_firing_rate = sum(d["firing_rate"] for d in simulation_data) / len(simulation_data)
    max_e_i_ratio = max(d["e_to_i_ratio"] for d in simulation_data)
    min_e_i_ratio = min(d["e_to_i_ratio"] for d in simulation_data)
    
    print(f"   Total spikes over simulation: {total_spikes}")
    print(f"   Average firing rate: {avg_firing_rate:.4f}")
    print(f"   Maximum firing rate: {max_firing_rate:.4f}")
    print(f"   E/I ratio range: {min_e_i_ratio:.3f} - {max_e_i_ratio:.3f}")
    
    # Final brain state
    final_stats = brain.getBrainStats()
    print(f"\n   Final developmental stage: {final_stats['developmental_stage']}")
    print(f"   Final total spikes: {final_stats['total_spikes']}")
    
    print()
    
    # 5. Save analysis results
    print("5. Saving analysis results:")
    
    analysis_results = {
        "initial_stats": initial_stats,
        "simulation_data": simulation_data,
        "summary": {
            "total_spikes": total_spikes,
            "avg_firing_rate": avg_firing_rate,
            "max_firing_rate": max_firing_rate,
            "e_i_ratio_range": [min_e_i_ratio, max_e_i_ratio],
            "final_stage": final_stats["developmental_stage"]
        }
    }
    
    try:
        with open("brain_analysis.json", "w") as f:
            json.dump(analysis_results, f, indent=2, default=str)
        print(f"   ✓ Analysis results saved to brain_analysis.json")
    except Exception as e:
        print(f"   ✗ Error saving analysis: {e}")
    
    print("\n=== Brain statistics analysis example complete ===\n")

if __name__ == "__main__":
    brain_statistics_analysis()