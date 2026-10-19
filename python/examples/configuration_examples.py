"""
Example: Configuration Examples

This example demonstrates configuration management using the Enhanced NLM module.
"""

from nlm_enhanced import AdvancedConfig
import json
import time

# Example 1: Basic configuration
print("Example 1: Basic Configuration")
print("=" * 40)

config = AdvancedConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

print("Configuration created:")
for key in config.keys():
    value = config[key]
    print(f"  {key}: {value}")

# Example 2: Schema validation
print("\nExample 2: Schema Validation")
print("=" * 40)

config2 = AdvancedConfig()
config2.add_schema("brain.neuron_count", "int", "Number of neurons", [100, 10000])
config2.add_schema("brain.synapse_density", "float", "Synapse density", [0.0, 1.0])
config2.add_schema("plasticity.learning_rate", "float", "Learning rate", [0.0, 1.0])

# Add configuration
config2.set("brain.neuron_count", 500)
config2.set("brain.synapse_density", 0.15)
config2.set("plasticity.learning_rate", 0.01)

# Validate
if config2.validate_schema():
    print("✓ Schema validation passed")
else:
    print("✗ Schema validation failed")
    for error in config2.validate():
        print(f"  {error.key}: {error.message}")

# Example 3: Configuration comparison
print("\nExample 3: Configuration Comparison")
print("=" * 40)

config3a = AdvancedConfig()
config3a.set("brain.neuron_count", 1000)
config3a.set("brain.synapse_density", 0.1)

config3b = AdvancedConfig()
config3b.set("brain.neuron_count", 1500)
config3b.set("brain.synapse_density", 0.15)

# Compare configurations
diff = config3a.diff(config3b)
print("Differences between configurations:")
print(diff)

# Check if different
if config3a.is_different(config3b):
    print("✓ Configurations are different")
else:
    print("✗ Configurations are the same")

# Example 4: Time series configuration
print("\nExample 4: Time Series Configuration")
print("=" * 40)

config4 = AdvancedConfig()
config4.add_time_series("brain.neuron_count", 0.0, "1000")
config4.add_time_series("brain.neuron_count", 1.0, "1100")
config4.add_time_series("brain.neuron_count", 2.0, "1200")
config4.add_time_series("brain.neuron_count", 3.0, "1150")
config4.add_time_series("brain.neuron_count", 4.0, "1300")

print("Time series data:")
for entry in config4.get_time_series("brain.neuron_count"):
    print(f"  Time {entry['timestep']}: {entry['value']}")

# Example 5: Interpolation
print("\nExample 5: Time Series Interpolation")
print("=" * 40)

# Interpolate at time 2.5
interpolated = config4.interpolate_time_series("brain.neuron_count", 2.5)
print(f"Interpolated value at time 2.5: {interpolated}")

# Example 6: Statistics
print("\nExample 6: Configuration Statistics")
print("=" * 40)

stats = config4.get_statistics()
for key, value in stats.items():
    print(f"  {key}: {value}")

# Example 7: Configuration export/import
print("\nExample 7: Configuration Export/Import")
print("=" * 40)

# Export to JSON
json_config = config4.export_to_json()
print(f"Configuration exported to JSON ({len(json_config)} characters)")

# Create new config and import
config4_imported = AdvancedConfig()
config4_imported.import_from_json(json_config)

# Compare
if config4.is_different(config4_imported):
    print("✓ Configuration successfully imported")
else:
    print("✗ Configuration import failed - no differences found")

# Example 8: Configuration recommendations
print("\nExample 8: Configuration Recommendations")
print("=" * 40)

recommendations = config4.get_recommendations()
if recommendations:
    print("Recommendations:")
    for rec in recommendations:
        print(f"  • {rec}")
else:
    print("No recommendations available")

optimization_suggestions = config4.get_optimization_suggestions()
print(f"\nOptimization Suggestions:\n{optimization_suggestions}")

# Example 9: Configuration snapshots
print("\nExample 9: Configuration Snapshots")
print("=" * 40)

# Create snapshot
snapshot = config4.create_snapshot()
print(f"Snapshot created: {snapshot[:50]}...")

# Check if key has changed
has_changed = config4.has_changed("brain.neuron_count", snapshot)
print(f"Key 'brain.neuron_count' has changed since snapshot: {has_changed}")

# Example 10: Batch operations
print("\nExample 10: Batch Operations")
print("=" * 40)

# Set multiple keys
key_values = {
    "brain.neuron_count": "2000",
    "brain.synapse_density": "0.2",
    "plasticity.learning_rate": "0.05"
}

config10 = AdvancedConfig()
config10.set_multiple(key_values)

print("Configuration after batch operations:")
for key in config10.keys():
    value = config10[key]
    print(f"  {key}: {value}")

# Get all as strings
all_as_strings = config10.get_all_as_strings()
print(f"\nAll as strings: {all_as_strings}")

# Get all as doubles
all_as_doubles = config10.get_all_as_doubles()
print(f"All as doubles: {all_as_doubles}")

# Example 11: Pythonic interface
print("\nExample 11: Pythonic Interface")
print("=" * 40)

config11 = AdvancedConfig()
config11["brain.neuron_count"] = "3000"
config11["brain.synapse_density"] = "0.25"

print("Configuration using Pythonic interface:")
print(f"  brain.neuron_count: {config11['brain.neuron_count']}")
print(f"  brain.synapse_density: {config11['brain.synapse_density']}")
print(f"  Has brain.synapse_density: {'brain.synapse_density' in config11}")
print(f"  Number of entries: {len(config11)}")
print(f"  String representation:\n{config11}")

# Example 12: JSON/YAML export
print("\nExample 12: JSON/YAML Export")
print("=" * 40)

# Export to JSON
json_config = config11.export_to_json()
print(f"JSON export ({len(json_config)} characters)")

# Export to YAML
yaml_config = config11.export_to_yaml()
print(f"YAML export ({len(yaml_config)} characters)")

# Example 13: File I/O
print("\nExample 13: File I/O")
print("=" * 40)

# Save to file
success = config11.save_to_file("config.txt", "cfg", "Example configuration file")
if success:
    print("✓ Configuration saved to file")
else:
    print("✗ Configuration save failed")

# Load from file
config11_loaded = AdvancedConfig()
success = config11_loaded.load_from_file("config.txt", "cfg")
if success:
    print("✓ Configuration loaded from file")
    print(f"  Loaded keys: {config11_loaded.keys()}")
else:
    print("✗ Configuration load failed")

# Example 14: Validation
print("\nExample 14: Configuration Validation")
print("=" * 40)

config14 = AdvancedConfig()
config14.add_schema("brain.neuron_count", "int", [100, 10000])
config14.add_schema("brain.synapse_density", "float", [0.0, 1.0])

# Add valid configuration
config14.set("brain.neuron_count", 500)
config14.set("brain.synapse_density", 0.15)

# Validate
validation_errors = config14.validate()
if validation_errors:
    print("Validation errors:")
    for error in validation_errors:
        print(f"  {error.key}: {error.message} ({error.severity})")
else:
    print("✓ Configuration validation passed")

# Example 15: Debug information
print("\nExample 15: Debug Information")
print("=" * 40)

debug_info = config14.get_debug_info()
print(f"Debug information ({len(debug_info)} characters)")

print("\n" + "=" * 50)
print("All configuration examples completed!")
print("=" * 50)