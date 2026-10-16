"""Configuration examples demonstrating ConfigBuilder and EnhancedConfig."""

import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from pynlm.enhanced import (
    ConfigBuilder, 
    EnhancedConfig,
    create_enhanced_config,
    ValidationError,
    create_enhanced_brain
)

print("Configuration Examples - Enhanced NLM API")
print("=" * 50)

# Example 1: Creating config with ConfigBuilder
print("\n1. Creating config with ConfigBuilder:")
builder = ConfigBuilder()

# Method chaining
builder.set_brain(
    neuron_count=1000,
    synapse_density=0.1,
    connection_probability=0.05,
    v_thresh=-50.0
).set_plasticity(
    stdp_enable=True,
    stdp_learning_rate=0.001,
    hebbian_enable=True
).set_neuromod(
    dopamine_scale=1.0,
    curiosity_enable=True,
    novelty_enable=True
)

config_dict = builder.to_dict()
print(f"   Created config with {len(config_dict)} parameters")
for key, value in config_dict.items():
    print(f"     {key}: {value}")

# Example 2: EnhancedConfig with file loading
print("\n2. Creating EnhancedConfig:")
config = create_enhanced_config()
config_summary = config.summary()
print(f"   Config summary:\n{config_summary}")

# Example 3: Validation
print("\n3. Configuration validation:")
valid_builder = ConfigBuilder()
valid_builder.set("brain.neuron_count", 500)
valid_builder.set("plasticity.stdp.enable", True)

try:
    valid_builder.validate()
    print("   Configuration is valid")
except ValidationError as e:
    print(f"   Validation error: {e}")

# Example 4: Config validation error
print("\n4. Config validation error (invalid format):")
invalid_builder = ConfigBuilder()
invalid_builder.set("brain_neuron_count", 500)  # Missing dot notation
try:
    invalid_builder.validate()
    print("   ERROR: Should have raised ValidationError")
except ValidationError as e:
    print(f"   Correctly caught validation error: {e}")

# Example 5: Create brain with enhanced config
print("\n5. Creating brain with enhanced config:")
brain = create_enhanced_brain(config, name="ConfigTestBrain")
brain.initialize()
print(f"   Created enhanced brain: {brain}")
print(f"   Total neurons: {brain.getTotalNeuronCount()}")

# Example 6: Summary and statistics
print("\n6. Configuration statistics:")
print(f"   Brain parameters: {len(config.get_brain_params())}")
print(f"   Plasticity parameters: {len(config.get_plasticity_params())}")
print(f"   Neuromodulation parameters: {len(config.get_neuromod_params())}")

print("\n" + "=" * 50)
print("Configuration examples completed successfully!")
