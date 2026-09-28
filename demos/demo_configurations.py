import pynlm
import time

"""
Demo 6: Configuration Management

This demo showcases different configuration types and validation from the NLM Python API.
"""

def demo_configurations():
    print("=== NLM Configuration Management Demo ===")
    
    # Test different configuration types
    print("\n1. Creating different configuration types...")
    
    config_types = ["default", "brain_only", "high_performance", "developmental"]
    
    for config_type in config_types:
        print(f"\n   Testing configuration: {config_type}")
        
        try:
            # Create advanced configuration
            config = pynlm.create_advanced_config(config_type)
            
            # Create brain with this config
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Get statistics
            stats = pynlm.get_brain_stats(brain)
            
            print(f"     Neurons: {stats['total_neurons']}")
            print(f"     Synapses: {stats['total_synapses']}")
            print(f"     Development stage: {stats['development_stage']}")
            
            # Validate configuration
            warnings = pynlm.validate_config(config)
            if warnings:
                print(f"     Warnings: {len(warnings)}")
            
        except Exception as e:
            print(f"     Error creating {config_type} config: {e}")
    
    # Demonstrate configuration validation
    print("\n2. Demonstrating configuration validation...")
    
    # Create different configurations to test validation
    test_configs = []
    
    # Create default config
    default_config = pynlm.createDefaultConfig()
    test_configs.append(("Default", default_config))
    
    # Create brain-only config
    brain_only_config = pynlm.create_advanced_config("brain_only")
    test_configs.append(("Brain Only", brain_only_config))
    
    # Create high-performance config
    high_perf_config = pynlm.create_advanced_config("high_performance")
    test_configs.append(("High Performance", high_perf_config))
    
    # Create developmental config
    dev_config = pynlm.create_advanced_config("developmental")
    test_configs.append(("Developmental", dev_config))
    
    for name, config in test_configs:
        print(f"\n   Validating {name} configuration:")
        warnings = pynlm.validate_config(config)
        
        if warnings:
            print(f"     Warnings ({len(warnings)}):")
            for warning in warnings:
                print(f"       - {warning}")
        else:
            print("     No warnings - configuration is valid")
    
    # Demonstrate quick test setup
    print("\n3. Demonstrating quick test setup...")
    try:
        config, brain = pynlm.quick_test_setup()
        
        print(f"   Quick setup successful:")
        print(f"     Config neurons: {config.getOr('brain.neuron_count', 1000)}")
        print(f"     Brain neurons: {brain.getTotalNeuronCount()}")
        print(f"     Brain synapses: {brain.getTotalSynapseCount()}")
        
        # Run quick simulation
        for i in range(50):
            brain.step(i)
        
        print(f"     Total spikes after 50 steps: {brain.getTotalSpikeCount()}")
        
    except Exception as e:
        print(f"   Quick setup error: {e}")
    
    print("\n=== Demo Complete ===")
    return True

if __name__ == "__main__":
    demo_configurations()
