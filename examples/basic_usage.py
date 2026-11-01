"""
NLM Python Examples - Practical examples and tutorials for NLM usage
"""

# examples/basic_usage.py
"""
Basic NLM Usage Example

This example demonstrates the fundamental usage patterns for NLM Python bindings.
"""

import sys
sys.path.insert(0, '.')

try:
    import nlm
    print("NLM package imported successfully")
except ImportError:
    print("NLM package not available. Make sure to build the Python bindings first.")
    sys.exit(1)

def basic_brain_creation():
    """Example: Create and initialize a brain"""
    print("\n=== Basic Brain Creation ===")
    
    # Create a configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 500, "example")
    config.set("region_count", 2, "example")
    config.set("excitatory_ratio", 0.8, "example")
    
    print(f"Created config with {config.get('neuron_count', 0)} neurons")
    print(f"Config summary: {config.summary()}")
    
    # Create brain
    brain = nlm.createBrain(config)
    print(f"Created brain: {brain}")
    
    # Initialize brain
    success = brain.initialize()
    if success:
        print("Brain initialized successfully")
    else:
        print("Failed to initialize brain")
        return
    
    # Run some simulation steps
    for i in range(10):
        brain.step(i)
        
        # Check some basic stats
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"Step {i}: {neurons} neurons, {synapses} synapses, {firing_rate:.2f} Hz")
    
    # Demonstrate property access
    print(f"\nBrain properties:")
    print(f"  Total neurons: {brain.neuron_count}")
    print(f"  Total synapses: {brain.synapse_count}")
    print(f"  Active neurons: {brain.active_neuron_count}")
    print(f"  Firing rate: {brain.avg_firing_rate:.2f} Hz")
    print(f"  E/I ratio: {brain.e_i_ratio:.3f}")
    print(f"  Total spikes: {brain.total_spikes}")
    
    # Demonstrate context manager
    print(f"\n=== Context Manager Example ===")
    with brain.create_default_config() as new_config:
        new_brain = nlm.createBrain(new_config)
        new_brain.initialize()
        new_brain.step(0)
        print("Context manager example completed (brain automatically reset)")
    
    # Demonstrate run_steps method
    print(f"\n=== Batch Operations Example ===")
    brain.run_steps(5, auto_save=True, checkpoint="batch_example.bin")
    print("Ran 5 steps with auto-save")
    
    # Demonstrate visualization
    print(f"\n=== Visualization Example ===")
    brain.visualize("basic_example.png")
    print("Visualization saved to 'basic_example.png'")
    
    print("\n=== Basic Example Complete ===")

if __name__ == "__main__":
    basic_brain_creation()