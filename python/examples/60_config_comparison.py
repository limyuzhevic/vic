"""NLM Configuration Comparison - Comparing different configuration types"""
import pynlm

def compare_configurations():
    """Compare different configuration presets."""
    print("=== Configuration Comparison Demo ===\n")
    
    # Test all configuration types
    configs = [
        ("Default", pynlm.createDefaultConfig()),
        ("Minimal", pynlm.createMinimalConfig()),
        ("Research", pynlm.createResearchConfig())
    ]
    
    brains = []
    
    for name, config in configs:
        print(f"{name} Configuration:")
        print(f"  - Neuron count: {config.getOr('neuron_count', 0)}")
        print(f"  - Region count: {config.getOr('region_count', 0)}")
        print(f"  - Connection probability: {config.getOr('connection_probability', 0.0)}")
        print(f"  - STDP enabled: {config.getOr('plasticity.stdp.enable', False)}")
        print(f"  - Hebbian enabled: {config.getOr('plasticity.hebbian.enable', False)}")
        print(f"  - Dopamine scale: {config.getOr('neuromod.dopamine.scale', 0.0)}")
        print(f"  - Curiosity enabled: {config.getOr('neuromod.curiosity.enable', False)}")
        print(f"  - Novelty enabled: {config.getOr('neuromod.novelty.enable', False)}")
        print(f"  - Development enabled: {config.getOr('development.enable', False)}")
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        brain.step(0)
        
        brains.append((name, brain))
        
        print(f"  - Total spikes after 1 step: {brain.total_spike_count}")
        print()
    
    # Compare results
    print("=== Comparison Summary ===")
    for name, brain in brains:
        print(f"{name} Config:")
        print(f"  - Neurons: {brain.neuron_count:,}")
        print(f"  - Synapses: {brain.synapse_count:,}")
        print(f"  - Firing neurons: {brain.firing_neuron_count}")
        print(f"  - Average firing rate: {brain.average_firing_rate:.2f} Hz")
        print()
    
    # Recommendations
    print("=== Recommendations ===")
    print("Default Config: Good for general purpose simulations")
    print("Minimal Config: Best for quick testing and debugging")
    print("Research Config: Best for advanced experiments and research")

cmpare_configurations()
