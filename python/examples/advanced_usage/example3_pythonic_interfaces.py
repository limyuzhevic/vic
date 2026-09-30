# Pythonic Interfaces Example
"""
Example demonstrating all Pythonic interfaces and enhancements.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Pythonic Interfaces Example ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enhanced interfaces
    enhanced_brain = pynlm.EnhancedBrain(brain)
    enhanced_config = pynlm.EnhancedConfig(config)
    
    # Property-based access (Pythonic)
    print("1. Property-based access:")
    print(f"   Neuron count: {enhanced_brain.neuron_count}")
    print(f"   Synapse count: {enhanced_brain.synapse_count}")
    print(f"   Firing count: {enhanced_brain.firing_count}")
    print(f"   Firing rate: {enhanced_brain.firing_rate}")
    print(f"   Is active: {enhanced_brain.is_active}")
    
    # Iterator support
    print("\n2. Iterator support:")
    print(f"   Number of regions: {len(enhanced_brain)}")
    for region_id in enhanced_brain:
        print(f"   - Region {region_id.index}")
    
    # Getitem support
    if enhanced_brain:
        first_region_id = next(iter(enhanced_brain))
        region_stats = enhanced_brain[first_region_id]
        print(f"   First region stats: {region_stats}")
    
    # Key method calls
    print("\n3. Method chaining and fluent interface:")
    # Chain development setup
    enhanced_brain.setDevelopmentStage(pynlm.DevelopmentalStage.Maturation)\
                   .enableAllModulations()\
                   .enableDevelopment(True)\
                   .enableCuriosity(True)
    
    print(f"   Development stage: {enhanced_brain.developmental_stage}")
    print(f"   All modulations enabled: {enhanced_brain.is_reward_modulation_enabled and enhanced_brain.is_development_enabled}")
    
    # Config dictionary-like interface
    print("\n4. Config dictionary-like interface:")
    enhanced_config.set_value("test.key1", "string_value")\
                   .set_value("test.key2", 42)\
                   .set_value("test.key3", 3.14)\
                   .set_value("test.key4", True)
    
    print(f"   Has key 'test.key1': {enhanced_config.has('test.key1')}")
    print(f"   Has key 'test.key2': {enhanced_config.has('test.key2')}")
    
    # Type-specific getters
    print("\n5. Type-specific getters:")
    print(f"   String value: {enhanced_config.get_string('test.key1', 'default')}")
    print(f"   Int value: {enhanced_config.get_int('test.key2', 0)}")
    print(f"   Float value: {enhanced_config.get_float('test.key3', 0.0)}")
    print(f"   Bool value: {enhanced_config.get_bool('test.key4', False)}")
    
    # Getitem with config
    if enhanced_config:
        value = enhanced_config['test.key1']
        print(f"   Config getitem for 'test.key1': {value}")
    
    # Iteration over config
    print("\n6. Iteration over config keys:")
    for key in enhanced_config:
        value = enhanced_config[key]
        print(f"   {key}: {value} ({type(value).__name__})")
    
    print("\n=== Pythonic Interfaces Complete ===")

if __name__ == "__main__":
    main()
