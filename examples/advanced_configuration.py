"""
Example 4: Advanced Configuration and Batch Processing
Demonstrates advanced configuration options and batch processing capabilities.
"""

import sys
sys.path.insert(0, 'python')

import pynlm
import json
import os
import time

def create_advanced_config(config_file=None):
    """Create an advanced configuration from file or defaults"""
    config = pynlm.create_default_config()
    
    if config_file and os.path.exists(config_file):
        print(f"Loading configuration from: {config_file}")
        # Note: pynlm Config.loadFromFile() expects JSON, but our config is .cfg
        # This is a placeholder for future implementation
        print("Warning: Config file loading would need to be implemented for .cfg files")
    
    # Advanced configuration
    config.set("neuron_count", 2000)
    config.set("region_count", 4)
    config.set("connection_probability", 0.08)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.018)
    config.set("stdp_tau", 25.0)
    config.set("synaptogenesis_rate", 0.0005)
    config.set("pruning_rate", 0.00005)
    config.set("plasticity_learning_rate", 0.005)
    
    # Neuromodulation
    config.set("dopamine_baseline", 0.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Memory settings
    config.set("working_memory_capacity", 200)
    config.set("episodic_memory_max_episodes", 5000)
    
    # Development
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0001)
    
    # Integration intervals
    config.set("replay_interval", 200)
    config.set("consolidation_interval", 2000)
    
    # Checkpointing
    config.set("checkpoint_dir", "./nlm_checkpoints")
    config.set("checkpoint_interval", 500)
    config.set("checkpoint_compression", "balanced")
    
    return config

def batch_brain_simulation(num_brains=3, steps_per_brain=1000):
    """Run multiple brain simulations in batch"""
    print(f"=== NLM Batch Processing with {num_brains} Brains ===\n")
    
    results = []
    
    for brain_idx in range(num_brains):
        print(f"Running Brain {brain_idx + 1}/{num_brains}")
        print("-" * 40)
        
        # Create unique configuration for each brain
        config = create_advanced_config()
        config.set("random_seed", 42 + brain_idx * 1000)  # Different seed per brain
        config.set("neuron_count", 1000 + brain_idx * 200)  # Variable size
        
        # Create and initialize brain
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Create agent and world
        world = pynlm.create_simple_world()
        world.configure(width=10 + brain_idx, height=10 + brain_idx, 
                       visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        agent.enableAllSubsystems(True)
        
        # Run simulation
        start_time = time.time()
        
        for step in range(steps_per_brain):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, step * 0.1)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            
            reward = 0.0
            if world.getSensoryPercept().getInternal() and not world.getSensoryPercept().getInternal().empty():
                reward = world.getSensoryPercept().getInternal()[0]
            
            agent.applyRewardModulation(reward, 0.0)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Collect results
        brain_result = {
            "brain_id": brain_idx + 1,
            "steps": steps_per_brain,
            "duration": duration,
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "e_i_ratio": brain.getExcitationInhibitionRatio(),
            "final_curiosity": agent.getCuriosityLevel(),
            "final_novelty": agent.getNoveltyLevel(),
            "final_development_stage": str(agent.getDevelopmentalStage()),
            "neuromodulation_level": agent.getNeuromodulationLevel(),
            "prediction_error": agent.getPredictionError()
        }
        
        results.append(brain_result)
        
        print(f"  Completed in {duration:.2f} seconds")
        print(f"  Neurons: {brain_result['total_neurons']}, "
              f"Synapses: {brain_result['total_synapses']}")
        print(f"  Avg firing: {brain_result['average_firing_rate']:.3f}, "
              f"Spikes: {brain_result['total_spikes']}")
        print(f"  E/I ratio: {brain_result['e_i_ratio']:.3f}")
        print()
    
    # Summary statistics
    print("=== Batch Processing Summary ===")
    print(f"Total brains processed: {num_brains}")
    print(f"Total simulation time: {sum(r['duration'] for r in results):.2f} seconds")
    print()
    
    # Performance metrics
    total_neurons = sum(r['total_neurons'] for r in results)
    avg_firing = sum(r['average_firing_rate'] for r in results) / num_brains
    avg_ei_ratio = sum(r['e_i_ratio'] for r in results if r['e_i_ratio'] < float('inf')) / num_brains
    
    print("Performance metrics:")
    print(f"  Average neurons per brain: {total_neurons / num_brains:.0f}")
    print(f"  Average firing rate across all brains: {avg_firing:.3f} Hz")
    print(f"  Average E/I ratio: {avg_ei_ratio:.3f}")
    print()
    
    # Save results
    with open("batch_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print("Results saved to: batch_results.json")
    
    return results

def save_configuration(config, filename):
    """Save configuration to file"""
    # Note: This would need proper implementation for .cfg files
    print(f"Configuration save to {filename} would be implemented for .cfg files")
    print("Current config summary:")
    print(config.summary())

if __name__ == "__main__":
    # Example 1: Advanced configuration
    print("Creating advanced configuration...")
    config = create_advanced_config()
    save_configuration(config, "advanced_config.cfg")
    
    print("\n" + "="*60 + "\n")
    
    # Example 2: Batch processing
    results = batch_brain_simulation(num_brains=3, steps_per_brain=500)
    
    print("\nAdvanced configuration and batch processing completed successfully!")
