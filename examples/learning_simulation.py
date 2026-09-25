import enhanced_pynlm as nlm

def main():
    print("=== NLM Enhanced API Example: Learning with Memory ===")
    
    # Create brain with memory support
    brain = nlm.Brain()
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Memory systems: Working={brain.getWorkingMemory() is not None}, "
          f"Episodic={brain.getEpisodicMemory() is not None}")
    
    # Run learning simulation with varying input patterns
    print("\nRunning learning simulation with varying inputs...")
    print("Step  | Firing Neurons | Working Memory | Novelty Level")
    print("------|----------------|----------------|--------------")
    
    for step in range(0, 501, 100):
        # Create different input patterns based on step
        if step < 200:
            # Increasing pattern
            sensory_input = [0.1 * ((step + i) % 10) / 10 for i in range(8)]
        elif step < 400:
            # Decreasing pattern  
            sensory_input = [0.1 * (9 - ((step - 200 + i) % 10)) / 10 for i in range(8)]
        else:
            # Random pattern
            import random
            sensory_input = [random.random() for _ in range(8)]
        
        # Run step
        brain.step(step, sensory_input)
        
        # Get statistics
        stats = brain.getStats()
        
        # Display progress every 100 steps
        if step % 100 == 0:
            print(f"{step:5d} | {stats['firing_neurons']:14d} | "
                  f"{stats.get('working_memory_traces', 0):14d} | "
                  f"{stats.get('curiosity_level', 0):11.3f}")
    
    # Final statistics
    final_stats = brain.getStats()
    
    print(f"\nLearning Simulation Results:")
    print(f"  Total steps: 500")
    print(f"  Average firing rate: {final_stats['average_firing_rate']:.2f}")
    print(f"  Total spikes: {final_stats['total_spikes']}")
    print(f"  Development stage: {final_stats['development_stage']}")
    
    # Check memory statistics
    if 'working_memory_traces' in final_stats:
        print(f"  Final working memory traces: {final_stats['working_memory_traces']}")
    if 'episodic_memory_episodes' in final_stats:
        print(f"  Final episodic memory episodes: {final_stats['episodic_memory_episodes']}")
    if 'novelty_level' in final_stats:
        print(f"  Final novelty level: {final_stats['novelty_level']:.3f}")
    
    # Save learning results
    print(f"\nSaving learning simulation...")
    brain.save("learning_simulation_example")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
