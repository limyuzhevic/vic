"""
Basic NLM simulation example.

This example demonstrates fundamental NLM simulation concepts:
- Brain creation and initialization
- Simulation steps
- Statistics collection
- State saving and loading
"""

import pynlm

def basic_simulation():
    """Run a basic brain simulation."""
    print("=== Basic NLM Simulation ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    print(f"Created config: {config.summary()}")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # Get final statistics
    print(f"\nFinal statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage().name}")
    
    # Save brain state
    brain.save("basic_brain.pkl")
    print(f"Brain saved to 'basic_brain.pkl'")
    
    return brain

def custom_config_example():
    """Example with custom configuration."""
    print("\n=== Custom Configuration Example ===")
    
    from pynlm import NLMConfigBuilder
    
    # Create custom configuration
    config = (NLMConfigBuilder()
             .set_neural_parameters(neuron_count=2000, synapse_density=0.15)
             .set_plasticity(stdp_enabled=True, hebbian_enabled=True, stdp_rate=0.002)
             .set_neuromodulation(dopamine_scale=1.5, curiosity_enabled=True)
             .set_development(developmental_enabled=True)
             .get_config())
    
    print(f"Custom config: {config.summary()}")
    
    # Run simulation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(50):
        brain.step(step)
    
    print(f"Custom brain simulation complete:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain

if __name__ == "__main__":
    # Run examples
    brain1 = basic_simulation()
    brain2 = custom_config_example()
    
    print("\n=== All examples completed successfully! ===")
