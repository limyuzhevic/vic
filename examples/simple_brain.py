import enhanced_pynlm as nlm

def main():
    print("=== NLM Enhanced API Example: Simple Brain Simulation ===")
    
    # Create and initialize brain (one-liner!)
    brain = nlm.Brain()
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    print("Running 100 simulation steps...")
    brain.simulate(100)
    
    # Get and display results
    stats = brain.getStats()
    print(f"\nSimulation Results:")
    print(f"  Total neurons: {stats['total_neurons']}")
    print(f"  Total synapses: {stats['total_synapses']}")
    print(f"  Firing neurons: {stats['firing_neurons']}")
    print(f"  Average firing rate: {stats['average_firing_rate']:.2f}")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Development stage: {stats['development_stage']}")
    
    # Save brain state
    print(f"\nSaving brain state...")
    brain.save("simple_brain_example")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
