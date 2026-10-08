#!/usr/bin/env python3
"""
NLM Python Example: Configuration Management
============================================

This example demonstrates the enhanced configuration management features
of the NLM Python API, including templates, validation, and serialization.
"""

import pynlm as nlm
import pynlm.utils as utils
import json

def main():
    print("NLM Configuration Management Example")
    print("=" * 50)
    
    # Create different configuration templates
    print("1. Creating Configuration Templates")
    print("-" * 40)
    
    # Default configuration
    print("\n1.1 Default Configuration:")
    default_config = nlm.create_default_config()
    default_config.set("neuron_count", 100)
    default_config.set("region_count", 2)
    print(f"   Neurons: {default_config.get('neuron_count')}")
    print(f"   Regions: {default_config.get('region_count')}")
    
    # Research configuration
    print("\n1.2 Research Configuration:")
    research_config = utils.ConfigUtils.create_research_config(neuron_count=1000)
    print(f"   Neurons: {research_config.get('neuron_count')}")
    print(f"   Plasticity STDP learning rate: {research_config.get('plasticity.stdp.learning_rate')}")
    print(f"   Neuromodulation dopamine strength: {research_config.get('neuromodulation.dopamine.strength')}")
    
    # Development configuration
    print("\n1.3 Development Configuration:")
    dev_config = utils.ConfigUtils.create_development_config()
    print(f"   Neurons: {dev_config.get('neuron_count')}")
    print(f"   Synaptogenesis enabled: {dev_config.get('development.enable_synaptogenesis')}")
    print(f"   Critical period: {dev_config.get('development.critical_period.enabled')}")
    
    # Validation configuration
    print("\n1.4 Validation Configuration:")
    val_config = utils.ConfigUtils.create_validation_config()
    print(f"   Neurons: {val_config.get('neuron_count')}")
    print(f"   Optimized for testing: True")
    
    # Configuration validation
    print("\n2. Configuration Validation")
    print("-" * 35)
    
    # Validate research config
    print("\n2.1 Validating Research Configuration:")
    research_errors = utils.validate_configuration(research_config)
    if research_errors:
        print(f"   Errors: {research_errors}")
    else:
        print("   Configuration is valid!")
    
    # Validate development config
    print("\n2.2 Validating Development Configuration:")
    dev_errors = utils.validate_configuration(dev_config)
    if dev_errors:
        print(f"   Errors: {dev_errors}")
    else:
        print("   Configuration is valid!")
    
    # Configuration to/from dictionary
    print("\n3. Configuration Serialization")
    print("-" * 35)
    
    # Export to dictionary
    print("\n3.1 Exporting to Dictionary:")
    config_dict = research_config.to_dict()
    print(f"   Configuration keys: {list(config_dict.keys())}")
    print(f"   Neuron count: {config_dict.get('neuron_count', 'N/A')}")
    print(f"   Plasticity rate: {config_dict.get('plasticity.stdp.learning_rate', 'N/A')}")
    
    # Create new config from dictionary
    print("\n3.2 Creating from Dictionary:")
    new_config = nlm.create_default_config()
    new_config.from_dict({
        'neuron_count': 500,
        'region_count': 3,
        'plasticity.stdp.learning_rate': 0.02,
        'neuromodulation.dopamine.strength': 1.5,
        'development.enable_synaptogenesis': True
    })
    
    print(f"   Created config - Neurons: {new_config.get('neuron_count')}")
    print(f"   Plasticity rate: {new_config.get('plasticity.stdp.learning_rate')}")
    print(f"   Synaptogenesis: {new_config.get('development.enable_synaptogenesis')}")
    
    # Configuration comparison
    print("\n4. Configuration Comparison")
    print("-" * 35)
    
    print("\n4.1 Comparing Research and Development Configs:")
    common_keys = set(research_config.get_keys()) & set(dev_config.get_keys())
    
    print("   Common keys and values:")
    for key in sorted(common_keys):
        res_val = research_config.get_or(key, None)
        dev_val = dev_config.get_or(key, None)
        if res_val != dev_val:
            print(f"     {key}:")
            print(f"       Research: {res_val}")
            print(f"       Development: {dev_val}")
    
    # Configuration merge
    print("\n5. Configuration Merge")
    print("-" * 30)
    
    print("\n5.1 Merging Configurations:")
    merged_config = nlm.create_default_config()
    
    # Merge research and development configs
    for key in research_config.get_keys():
        merged_config.set(key, research_config.get_or(key, None))
    
    for key in dev_config.get_keys():
        merged_config.set(key, dev_config.get_or(key, None))
    
    print(f"   Merged config neuron count: {merged_config.get('neuron_count')}")
    print(f"   Merged config synaptogenesis: {merged_config.get('development.enable_synaptogenesis')}")
    print(f"   Merged config critical period: {merged_config.get('development.critical_period.enabled')}")
    
    # Configuration export/import
    print("\n6. Configuration File I/O")
    print("-" * 35)
    
    print("\n6.1 Exporting to JSON:")
    utils.DataUtils.save_to_file(merged_config.to_dict(), "config_export.json")
    print("   Exported to: config_export.json")
    
    print("\n6.2 Importing from JSON:")
    imported_config = nlm.create_default_config()
    imported_data = utils.DataUtils.load_from_file("config_export.json")
    imported_config.from_dict(imported_data)
    
    print(f"   Imported neuron count: {imported_config.get('neuron_count')}")
    print(f"   Imported synaptogenesis: {imported_config.get('development.enable_synaptogenesis')}")
    
    # Configuration validation utility
    print("\n7. Advanced Validation")
    print("-" * 30)
    
    print("\n7.1 Validating Required Configuration:")
    required_keys = ['neuron_count', 'region_count', 'plasticity.stdp.learning_rate']
    try:
        merged_config.validate_all(required_keys)
        print("   Required configuration validation passed!")
    except Exception as e:
        print(f"   Validation failed: {e}")
    
    # Configuration statistics
    print("\n8. Configuration Statistics")
    print("-" * 35)
    
    print("\n8.1 Configuration Summary:")
    print(f"   Merged config summary: {merged_config.summary()}")
    print(f"   Number of keys: {len(merged_config.get_keys())}")
    print(f"   Source types: {set([str(merged_config.get(key, 'N/A')) for key in merged_config.get_keys()])}")
    
    # Configuration comparison for analysis
    print("\n9. Configuration Analysis")
    print("-" * 35)
    
    print("\n9.1 Configuration Type Analysis:")
    config_types = {
        'Research': research_config.get_keys(),
        'Development': dev_config.get_keys(),
        'Merged': merged_config.get_keys()
    }
    
    for config_type, keys in config_types.items():
        print(f"   {config_type} config has {len(keys)} keys:")
        for key in sorted(keys):
            val = merged_config.get_or(key, 'N/A')
            print(f"     {key}: {val}")
    
    # Configuration performance test
    print("\n10. Configuration Performance Test")
    print("-" * 40)
    
    print("\n10.1 Configuration Performance:")
    import time
    
    # Test export performance
    start_time = time.time()
    for _ in range(1000):
        _ = merged_config.to_dict()
    export_time = time.time() - start_time
    print(f"   Export 1000 iterations: {export_time:.3f} seconds")
    
    # Test import performance
    start_time = time.time()
    for _ in range(1000):
        test_config = nlm.create_default_config()
        test_config.from_dict({'neuron_count': 100, 'region_count': 2})
    import_time = time.time() - start_time
    print(f"   Import 1000 iterations: {import_time:.3f} seconds")
    
    # Configuration memory usage
    print("\n11. Configuration Memory Usage")
    print("-" * 40)
    
    print("\n11.1 Memory Usage Estimate:")
    print(f"   Research config keys: {len(research_config.get_keys())}")
    print(f"   Development config keys: {len(dev_config.get_keys())}")
    print(f"   Merged config keys: {len(merged_config.get_keys())}")
    
    # Configuration compatibility check
    print("\n12. Configuration Compatibility")
    print("-" * 40)
    
    print("\n12.1 Checking Configuration Compatibility:")
    
    # Check if configurations can work together
    compatibility_issues = []
    
    # Check neuron count ranges
    for config_name, config in [('Research', research_config), ('Development', dev_config), ('Merged', merged_config)]:
        neuron_count = config.get_or('neuron_count', 0)
        if neuron_count < 10:
            compatibility_issues.append(f"{config_name}: Neuron count too low ({neuron_count})")
        elif neuron_count > 10000:
            compatibility_issues.append(f"{config_name}: Neuron count too high ({neuron_count})")
    
    # Check plasticity settings
    for config_name, config in [('Research', research_config), ('Development', dev_config), ('Merged', merged_config)]:
        stdp_rate = config.get_or('plasticity.stdp.learning_rate', 0)
        hebbian_rate = config.get_or('plasticity.hebbian.learning_rate', 0)
        if stdp_rate <= 0:
            compatibility_issues.append(f"{config_name}: STDP learning rate should be positive")
        if hebbian_rate <= 0:
            compatibility_issues.append(f"{config_name}: Hebbian learning rate should be positive")
    
    if compatibility_issues:
        print("   Compatibility issues found:")
        for issue in compatibility_issues:
            print(f"     - {issue}")
    else:
        print("   All configurations are compatible!")
    
    # Save final configuration
    print("\n13. Final Configuration")
    print("-" * 30)
    
    print("\n13.1 Saving Final Configuration:")
    utils.DataUtils.save_to_file(merged_config.to_dict(), "final_config.json")
    print("   Final configuration saved to: final_config.json")
    
    print("\nConfiguration management example completed successfully!")

if __name__ == "__main__":
    main()
