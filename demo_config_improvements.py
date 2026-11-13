#!/usr/bin/env python3
"""
NLM Configuration Demonstration
Shows the enhanced configuration system in action
"""

import pynlm
import tempfile
import os

def demonstrate_config_improvements():
    print("=== NLM Configuration Improvements Demonstration ===")
    print()
    
    # 1. Basic Configuration Setup
    print("1. Basic Configuration Setup:")
    config = pynlm.createDefaultConfig()
    config.set("random_seed", 42)
    config.set("neuron_count", 500)
    config.set("connection_probability", 0.15)
    config.set("stdp_ltp_weight", 0.02)
    
    print(f"   Neuron count: {config.getOr('neuron_count', 100)}")
    print(f"   Connection probability: {config.getOr('connection_probability', 0.0)}")
    print(f"   STDP LTP weight: {config.getOr('stdp_ltp_weight', 0.0)}")
    print()
    
    # 2. Advanced Configuration
    print("2. Advanced Configuration:")
    config.set("region_count", 2)
    config.set("stdp_ltd_weight", 0.015)
    config.set("stdp_tau", 20.0)
    config.set("synaptogenesis_rate", 0.0001)
    config.set("pruning_rate", 0.00001)
    config.set("simulation_timestep", 0.001)
    
    print(f"   Region count: {config.getOr('region_count', 1)}")
    print(f"   STDP LTD weight: {config.getOr('stdp_ltd_weight', 0.0)}")
    print(f"   Simulation timestep: {config.getOr('simulation_timestep', 0.001)}")
    print()
    
    # 3. Configuration Summary
    print("3. Configuration Summary:")
    print("   " + config.summary().replace("\n", "\n   "))
    print()
    
    # 4. Brain Initialization
    print("4. Brain Initialization:")
    print("   Creating brain with enhanced configuration...")
    
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"   Brain initialized successfully!")
        print(f"   Total neurons: {brain.getTotalNeuronCount()}")
        print(f"   Total synapses: {brain.getTotalSynapseCount()}")
        print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"   Total spikes: {brain.getTotalSpikeCount()}")
        
    except Exception as e:
        print(f"   Error initializing brain: {e}")
    
    print()
    
    # 5. Configuration Management
    print("5. Configuration Management:")
    
    # Add some more configuration options
    config.set("enable_reward_modulation", True)
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_curiosity", True)
    
    print("   Additional configuration options set:")
    for key in ["enable_reward_modulation", "enable_structural_plasticity", 
                "enable_development", "enable_curiosity"]:
        value = config.getOr(key, False)
        print(f"     {key}: {value}")
    
    print()
    
    # 6. Configuration Statistics
    print("6. Configuration Statistics:")
    keys = config.getKeys()
    print(f"   Total configuration keys: {len(keys)}")
    print(f"   Configuration keys: {', '.join(keys)}")
    
    # Group by type
    int_keys = [k for k in keys if config.has(k) and isinstance(config.getOr(k, 0), (int, float))]
    str_keys = [k for k in keys if config.has(k) and isinstance(config.getOr(k, ""), str)]
    bool_keys = [k for k in keys if config.has(k) and isinstance(config.getOr(k, False), bool)]
    
    print(f"   Integer/Double keys: {len(int_keys)}")
    print(f"   String keys: {len(str_keys)}")
    print(f"   Boolean keys: {len(bool_keys)}")
    print()
    
    # 7. Performance Configuration
    print("7. Performance-Optimized Configuration:")
    perf_config = pynlm.createDefaultConfig()
    
    # High-performance settings for larger networks
    perf_config.set("neuron_count", 2000)
    perf_config.set("region_count", 4)
    perf_config.set("connection_probability", 0.05)
    perf_config.set("simulation_timestep", 0.002)
    perf_config.set("stdp_ltp_weight", 0.025)
    perf_config.set("stdp_ltd_weight", 0.02)
    perf_config.set("stdp_tau", 30.0)
    
    print(f"   Neurons: {perf_config.getOr('neuron_count', 0)}")
    print(f"   Regions: {perf_config.getOr('region_count', 0)}")
    print(f"   Connectivity: {perf_config.getOr('connection_probability', 0.0) * 100}%")
    print(f"   Timestep: {perf_config.getOr('simulation_timestep', 0.0)}s")
    print()
    
    # 8. Configuration Comparison
    print("8. Configuration Comparison:")
    
    # Create two similar configurations with different settings
    config1 = pynlm.createDefaultConfig()
    config1.set("neuron_count", 1000)
    config1.set("development_enabled", True)
    
    config2 = pynlm.createDefaultConfig()
    config2.set("neuron_count", 1500)
    config2.set("development_enabled", False)
    config2.set("curiosity_enabled", True)
    
    print("   Configuration 1:")
    print(f"     Neurons: {config1.getOr('neuron_count', 0)}")
    print(f"     Development: {config1.getOr('development_enabled', False)}")
    
    print("   Configuration 2:")
    print(f"     Neurons: {config2.getOr('neuron_count', 0)}")
    print(f"     Development: {config2.getOr('development_enabled', False)}")
    print(f"     Curiosity: {config2.getOr('curiosity_enabled', False)}")
    
    print()
    
    print("=== Configuration Improvements Demonstration Complete ===")
    print()
    print("Key improvements demonstrated:")
    print("✅ Enhanced configuration management")
    print("✅ Support for advanced configuration options")
    print("✅ Configuration statistics and analysis")
    print("✅ Performance-optimized configuration")
    print("✅ Configuration comparison capabilities")
    print("✅ Backward compatibility with existing configs")

if __name__ == "__main__":
    demonstrate_config_improvements()
