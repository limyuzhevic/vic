#!/usr/bin/env python3
"""
Example 2: Configuration Setup - Advanced Configuration Management

This example demonstrates advanced configuration management features
of the enhanced NLM Python bindings, including:

- Loading configuration from multiple sources (dictionaries, files, command line)
- Using preset configurations
- Parameter validation and optimization
- Configuration chaining for fluent interface
- Configuration validation and error handling

Key concepts demonstrated:
- Enhanced configuration loading with validation
- Preset configuration system
- Parameter optimization for different use cases
- Error handling and validation
- Configuration persistence and sharing
"""

import sys
import os
import json
import tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm_enhanced as nlm
except ImportError:
    print("Enhanced NLM bindings not available. Using mock for demonstration.")
    # Mock implementation for demonstration
    class MockConfig:
        def __init__(self):
            self.data = {}
        
        def load_from_dict(self, d):
            self.data = d
            return self
        
        def load_from_file(self, filepath):
            with open(filepath, 'r') as f:
                self.data = json.load(f)
            return self
        
        def get_int(self, key, default=None):
            return self.data.get(key, default)
        
        def get_double(self, key, default=None):
            return self.data.get(key, default)
        
        def get_bool(self, key, default=None):
            return self.data.get(key, default)
        
        def set_int(self, key, value):
            self.data[key] = value
            return self
        
        def set_double(self, key, value):
            self.data[key] = value
            return self
        
        def set_bool(self, key, value):
            self.data[key] = value
            return self
        
        def get_keys(self):
            return list(self.data.keys())
        
        def to_dict(self):
            return self.data.copy()
        
        def summary(self):
            return f"Config with {len(self.data)} keys"
    
    # Mock preset function
    def get_preset_config(name):
        presets = {
            'toy_brain': {'neuron_count': 100, 'simulation_timestep': 0.01},
            'research_brain': {'neuron_count': 10000, 'simulation_timestep': 0.001},
            'production_brain': {'neuron_count': 5000, 'simulation_timestep': 0.001}
        }
        config = MockConfig()
        if name in presets:
            config.load_from_dict(presets[name])
        return config
    
    # Create mock nlm module
    nlm = type('MockNLM', (), {
        'Config': MockConfig,
        'get_preset_config': get_preset_config
    })()

def demonstrate_basic_configuration():
    """Demonstrate basic configuration loading."""
    print("1. Basic Configuration Loading")
    print("-" * 40)
    
    # Method 1: Load from Python dictionary
    print("Method 1: Load from Python dictionary")
    config = (nlm.Config()
              .with_neuron_count(500)
              .with_simulation_timestep(0.001)
              .with_stdp_params(0.02, 0.015)
              .set_bool('enable_curiosity', True))
    
    print(f"   ✓ Configuration loaded: {config.get_int('neuron_count')} neurons")
    print(f"   ✓ Simulation timestep: {config.get_double('simulation_timestep')}s")
    print(f"   ✓ Curiosity enabled: {config.get_bool('enable_curiosity')}")
    print()
    
    # Method 2: Load from file
    print("Method 2: Load from JSON file")
    # Create a temporary config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_data = {
            'neuron_count': 1000,
            'simulation_timestep': 0.001,
            'random_seed': 42,
            'enable_reward_modulation': True,
            'enable_structural_plasticity': True
        }
        json.dump(config_data, f)
        config_file = f.name
    
    try:
        file_config = nlm.Config()
        file_config.load_from_file(config_file)
        print(f"   ✓ Configuration loaded from file")
        print(f"   ✓ Keys: {', '.join(file_config.get_keys())}")
    finally:
        os.unlink(config_file)  # Clean up
    
    print()

def demonstrate_preset_configurations():
    """Demonstrate preset configuration system."""
    print("2. Preset Configuration System")
    print("-" * 40)
    
    # Show available presets
    print("Available preset configurations:")
    presets = ['toy_brain', 'research_brain', 'production_brain', 
               'fast_simulation', 'high_precision']
    
    for preset_name in presets:
        try:
            preset_config = nlm.get_preset_config(preset_name)
            neuron_count = preset_config.get_int('neuron_count')
            timestep = preset_config.get_double('simulation_timestep')
            print(f"   • {preset_name:20s}: {neuron_count:6d} neurons, {timestep:.6f}s timestep")
        except Exception:
            print(f"   • {preset_name:20s}: Not available")
    
    print()
    
    # Demonstrate using a preset
    print("Using research_brain preset:")
    research_config = nlm.get_preset_config('research_brain')
    print(f"   ✓ Neuron count: {research_config.get_int('neuron_count')}")
    print(f"   ✓ Simulation timestep: {research_config.get_double('simulation_timestep')}")
    print()

def demonstrate_configuration_chaining():
    """Demonstrate configuration chaining and fluent interface."""
    print("3. Configuration Chaining (Fluent Interface)")
    print("-" * 40)
    
    # Example 1: Multi-step configuration
    print("Example 1: Multi-step configuration")
    config1 = (nlm.Config()
               .with_neuron_count(2000)
               .with_simulation_timestep(0.0005)
               .with_stdp_params(0.03, 0.025)
               .with_plasticity(1.5)
               .set_bool('enable_development', True)
               .set_bool('enable_curiosity', True))
    
    print(f"   ✓ Created config with {config1.get_int('neuron_count')} neurons")
    print(f"   ✓ STDP params: LTP={config1.get_double('stdp_ltp_weight'):.3f}, "
          f"LTD={config1.get_double('stdp_ltd_weight'):.3f}")
    print(f"   ✓ Development enabled: {config1.get_bool('enable_development')}")
    print(f"   ✓ Curiosity enabled: {config1.get_bool('enable_curiosity')}")
    print()
    
    # Example 2: Configuration merging
    print("Example 2: Configuration merging")
    base_config = nlm.Config().with_neuron_count(1000)
    optimized_config = (base_config
                       .with_simulation_timestep(0.001)
                       .with_stdp_params(0.05, 0.04)
                       .set_bool('enable_optimization', True))
    
    print(f"   ✓ Base: {base_config.get_int('neuron_count')} neurons")
    print(f"   ✓ Optimized: {optimized_config.get_int('neuron_count')} neurons")
    print()

def demonstrate_configuration_validation():
    """Demonstrate configuration validation."""
    print("4. Configuration Validation")
    print("-" * 40)
    
    # Create a configuration with potential issues
    print("Creating configuration with validation checks...")
    
    config = nlm.Config()
    config.load_from_dict({
        'neuron_count': 500,
        'simulation_timestep': 0.001,
        'stdp_ltp_weight': 0.02,
        'stdp_ltd_weight': 0.015,
        'enable_curiosity': True,
        'enable_development': True
    })
    
    # Demonstrate validation checks
    print("Validation checks:")
    
    # Check required parameters
    required_params = ['neuron_count', 'simulation_timestep']
    for param in required_params:
        if config.get_int(param) is not None:
            print(f"   ✓ {param}: {config.get_int(param)} (valid)")
        else:
            print(f"   ✗ {param}: Missing (invalid)")
    
    # Check parameter ranges
    print("\nParameter range checks:")
    if config.get_int('neuron_count', 0) > 0:
        print(f"   ✓ neuron_count: {config.get_int('neuron_count')} (reasonable)")
    else:
        print(f"   ✗ neuron_count: {config.get_int('neuron_count')} (invalid)")
    
    if config.get_double('simulation_timestep', 0.0) > 0:
        print(f"   ✓ simulation_timestep: {config.get_double('simulation_timestep')} (valid)")
    else:
        print(f"   ✗ simulation_timestep: {config.get_double('simulation_timestep')} (invalid)")
    
    # Check parameter combinations
    print("\nParameter combination checks:")
    if config.get_bool('enable_curiosity') and config.get_bool('enable_development'):
        print("   ✓ Curiosity and development both enabled (compatible)")
    
    if config.get_double('stdp_ltp_weight', 0.0) > config.get_double('stdp_ltd_weight', 0.0):
        print("   ✓ LTP weight > LTD weight (normal for STDP)")
    else:
        print("   ⚠ LTP weight <= LTD weight (may affect learning)")
    
    print()

def demonstrate_configuration_export():
    """Demonstrate configuration export and sharing."""
    print("5. Configuration Export and Sharing")
    print("-" * 40)
    
    # Create a complex configuration
    config = (nlm.Config()
              .with_neuron_count(3000)
              .with_simulation_timestep(0.0005)
              .with_stdp_params(0.04, 0.03)
              .with_plasticity(1.3)
              .set_bool('enable_reward_modulation', True)
              .set_bool('enable_structural_plasticity', True)
              .set_bool('enable_curiosity', True)
              .set_bool('enable_development', True))
    
    # Export to dictionary
    config_dict = config.to_dict()
    print("Configuration exported to dictionary:")
    for key, value in config_dict.items():
        print(f"   {key}: {value}")
    
    # Show JSON serialization
    print("\nJSON serialization example:")
    json_str = json.dumps(config_dict, indent=2)
    print(json_str)
    
    # Demonstrate loading back
    print("\nLoading configuration from dictionary:")
    loaded_config = nlm.Config().load_from_dict(config_dict)
    print(f"   ✓ Successfully loaded {len(loaded_config.get_keys())} parameters")
    
    print()

def demonstrate_configuration_troubleshooting():
    """Demonstrate configuration troubleshooting."""
    print("6. Configuration Troubleshooting")
    print("-" * 40)
    
    print("Common configuration issues and solutions:")
    
    # Issue 1: Missing parameters
    print("\nIssue 1: Missing required parameters")
    print("   Problem: Configuration doesn't have 'neuron_count'")
    config = nlm.Config().with_simulation_timestep(0.001)
    if config.get_int('neuron_count') is None:
        print("   Solution: Add neuron_count parameter")
        config.with_neuron_count(500)
    print(f"   ✓ Fixed: neuron_count = {config.get_int('neuron_count')}")
    
    # Issue 2: Invalid parameter types
    print("\nIssue 2: Invalid parameter types")
    print("   Problem: simulation_timestep should be positive number")
    config2 = nlm.Config().set_double('simulation_timestep', -0.001)
    if config2.get_double('simulation_timestep', 0.0) <= 0:
        print("   Solution: Use positive timestep")
        config2.set_double('simulation_timestep', 0.001)
    print(f"   ✓ Fixed: simulation_timestep = {config2.get_double('simulation_timestep')}")
    
    # Issue 3: Parameter conflicts
    print("\nIssue 3: Parameter conflicts")
    print("   Problem: Too many neurons with small timestep (performance issue)")
    config3 = nlm.Config().with_neuron_count(10000).with_simulation_timestep(0.0001)
    total_operations = config3.get_int('neuron_count') / config3.get_double('simulation_timestep')
    if total_operations > 1000000:  # Performance threshold
        print(f"   ⚠ Performance warning: ~{total_operations:,.0f} operations per second")
        print("   Solution: Reduce neurons or increase timestep")
        config3.with_neuron_count(5000).with_simulation_timestep(0.001)
    print(f"   ✓ Optimized: {config3.get_int('neuron_count')} neurons, "
          f"{config3.get_double('simulation_timestep')}s timestep")
    
    print()

def main():
    """Run the configuration setup example."""
    print("=" * 70)
    print("NLM Configuration Setup - Advanced Configuration Management")
    print("=" * 70)
    print()
    
    demonstrate_basic_configuration()
    demonstrate_preset_configurations()
    demonstrate_configuration_chaining()
    demonstrate_configuration_validation()
    demonstrate_configuration_export()
    demonstrate_configuration_troubleshooting()
    
    print("=" * 70)
    print("Configuration Setup Complete!")
    print("=" * 70)
    print()
    print("Key takeaways:")
    print("• Enhanced Config class supports multiple input formats")
    print("• Preset configurations provide quick setup for common scenarios")
    print("• Configuration chaining enables fluent, readable setup")
    print("• Validation checks help identify potential issues")
    print("• Configuration export facilitates sharing and persistence")
    print("• Troubleshooting guidance helps resolve common issues")
    print()
    print("Next steps:")
    print("• Create custom preset configurations for your use cases")
    print("• Implement configuration validation in your applications")
    print("• Use configuration export to share settings with team members")
    print("• Add configuration-specific error handling for robust applications")

if __name__ == "__main__":
    main()
