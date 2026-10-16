# Example 3: Configuration Templates Demo

import pynlm

print("=== Configuration Templates Demo ===")

# Step 1: Show available templates
print("\n1. Available configuration templates...")

# Create default configuration
print("\n  Default Configuration:")
config = pynlm.createDefaultConfig()
print(f"    Neurons: {config.get('brain.neuron_count', 1000)}")
print(f"    Threshold: {config.get('brain.v_thresh', -55.0)}")
print(f"    Resting potential: {config.get('brain.v_rest', -70.0)}")
print(f"    Learning rate (STDP): {config.get('plasticity.stdp.learning_rate', 0.001)}")

# Create training configuration
print("\n  Training Configuration:")
config = pynlm.createTrainingConfig()
print(f"    Neurons: {config.get('brain.neuron_count', 1000)}")
print(f"    STDP enabled: {config.get('plasticity.stdp.enable', False)}")
print(f"    Curiosity enabled: {config.get('neuromod.curiosity.enable', False)}")
print(f"    Development enabled: {config.get('development.enable', False)}")

# Create simulation configuration
print("\n  Simulation Configuration:")
config = pynlm.createSimulationConfig()
print(f"    Neurons: {config.get('brain.neuron_count', 1000)}")
print(f"    STDP enabled: {config.get('plasticity.stdp.enable', False)}")
print(f"    Curiosity enabled: {config.get('neuromod.curiosity.enable', False)}")
print(f"    Development enabled: {config.get('development.enable', False)}")

# Create research configuration
print("\n  Research Configuration:")
config = pynlm.createResearchConfig()
print(f"    Neurons: {config.get('brain.neuron_count', 1000)}")
print(f"    STDP enabled: {config.get('plasticity.stdp.enable', False)}")
print(f"    Curiosity enabled: {config.get('neuromod.curiosity.enable', False)}")
print(f"    Development enabled: {config.get('development.enable', False)}")
print(f"    Prediction enabled: {config.get('prediction.enable', False)}")
print(f"    Replay enabled: {config.get('replay.enable', False)}")

# Step 2: Demonstrate Pythonic API
print("\n2. Pythonic Configuration API...")

# Method 1: Dictionary-style access
config = pynlm.createConfig()
config["brain.neuron_count"] = 2000
config["brain.v_thresh"] = -60.0
config["plasticity.stdp.learning_rate"] = 0.002

print(f"  Dictionary-style: {config['brain.neuron_count']} neurons")
print(f"  With default: {config.get('nonexistent.key', 'default_value')}")

# Method 2: Fluent interface
config = (pynlm.createConfig()
          .with_neuron_count(2500)
          .with_learning_rate(0.003)
          .with_curiosity(True))

print(f"  Fluent interface: {config.get('brain.neuron_count', 0)} neurons")

# Step 3: Configuration validation
print("\n3. Configuration validation...")

from pynlm import ConfigValidator

validator = ConfigValidator()

# Test valid configuration
valid_config = pynlm.createDefaultConfig()
valid_config["brain.neuron_count"] = 1500

result = validator.validate(valid_config)
print(f"  Valid config: {result['valid']}")
if not result['valid']:
    for error in result['errors']:
        print(f"    Error: {error}")
for warning in result['warnings']:
    print(f"    Warning: {warning}")

# Test invalid configuration
invalid_config = pynlm.createConfig()
invalid_config["brain.neuron_count"] = -1000  # Invalid!

result = validator.validate(invalid_config)
print(f"  Invalid config: {result['valid']}")
if not result['valid']:
    for error in result['errors']:
        print(f"    Error: {error}")

# Step 4: Configuration save/load
print("\n4. Configuration save/load...")

config = pynlm.createDefaultConfig()
config["brain.neuron_count"] = 1200
config["learning.test"] = "example_value"

# Save to file
config.saveToFile("example_config.json")
print(f"  Configuration saved to 'example_config.json'")

# Load from file
loaded_config = pynlm.createConfig()
loaded_config.loadFromFile("example_config.json")
print(f"  Loaded configuration: {loaded_config.get('brain.neuron_count', 0)} neurons")

# Step 5: Template factory
print("\n5. Template factory usage...")

class ConfigFactory:
    """Factory for creating configuration templates"""
    
    @staticmethod
    def create_experiment_config(neuron_count=1000, enable_curiosity=True, enable_development=True):
        """Create configuration for experiments"""
        config = pynlm.createConfig()
        config["brain.neuron_count"] = neuron_count
        config["plasticity.stdp.enable"] = True
        config["plasticity.hebbian.enable"] = True
        config["neuromod.curiosity.enable"] = enable_curiosity
        config["development.enable"] = enable_development
        config["experiment.enabled"] = True
        return config
    
    @staticmethod
    def create_production_config(neuron_count=500, enable_ml=False):
        """Create configuration for production"""
        config = pynlm.createConfig()
        config["brain.neuron_count"] = neuron_count
        config["plasticity.stdp.enable"] = False  # Disable for stability
        config["neuromod.curiosity.enable"] = False
        config["development.enable"] = False
        config["performance.optimization"] = True
        config["ml.integration"] = enable_ml
        return config

# Use factory
experiment_config = ConfigFactory.create_experiment_config(neuron_count=2000, enable_curiosity=True)
production_config = ConfigFactory.create_production_config(neuron_count=300, enable_ml=True)

print(f"  Experiment config: {experiment_config.get('brain.neuron_count')} neurons")
print(f"  Production config: {production_config.get('brain.neuron_count')} neurons")

# Step 6: Configuration comparison
print("\n6. Configuration comparison...")

config1 = pynlm.createDefaultConfig()
config2 = pynlm.createTrainingConfig()

# Compare configurations
print(f"  Config1 neurons: {config1.get('brain.neuron_count')}")
print(f"  Config2 neurons: {config2.get('brain.neuron_count')}")
print(f"  Config2 has more neurons: {config2.get('brain.neuron_count', 0) > config1.get('brain.neuron_count', 0)}")

# Check for differences
differences = []
for key in config1.getKeys():
    if key not in config2.getKeys():
        differences.append(f"{key} in Config1 only")
    elif config1.get(key) != config2.get(key):
        differences.append(f"{key}: Config1={config1.get(key)}, Config2={config2.get(key)}")

for key in config2.getKeys():
    if key not in config1.getKeys():
        differences.append(f"{key} in Config2 only")

if differences:
    print(f"  Differences: {len(differences)} found")
    for diff in differences[:3]:  # Show first 3 differences
        print(f"    {diff}")
else:
    print(f"  Configurations are identical")

# Step 7: Configuration templates as data
print("\n7. Configuration templates as data...")

# Define template structure
templates = {
    "baseline": {
        "brain.neuron_count": 1000,
        "brain.v_thresh": -55.0,
        "plasticity.stdp.enable": True
    },
    "advanced": {
        "brain.neuron_count": 3000,
        "brain.v_thresh": -60.0,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "neuromod.curiosity.enable": True,
        "development.enable": True
    },
    "minimal": {
        "brain.neuron_count": 500,
        "brain.v_thresh": -50.0,
        "plasticity.stdp.enable": False,
        "neuromod.curiosity.enable": False
    }
}

# Apply templates
for name, template in templates.items():
    config = pynlm.createConfig()
    for key, value in template.items():
        config[key] = value
    print(f"  {name} template: {config.get('brain.neuron_count')} neurons")

# Step 8: Advanced template features
print("\n8. Advanced template features...")

# Template with inheritance
base_config = pynlm.createDefaultConfig()
extended_config = base_config.copy()
extended_config["experiment.feature1"] = True
extended_config["experiment.feature2"] = True

print(f"  Base neurons: {base_config.get('brain.neuron_count')}")
print(f"  Extended neurons: {extended_config.get('brain.neuron_count')}")

# Template with parameters
parameterized_template = {
    "neuron_count": 1000,
    "threshold_offset": 0.0,
    "learning_rate": 0.001
}

custom_config = pynlm.createDefaultConfig()
custom_config["brain.neuron_count"] = parameterized_template["neuron_count"] + 500
custom_config["brain.v_thresh"] = -55.0 + parameterized_template["threshold_offset"]
custom_config["plasticity.stdp.learning_rate"] = parameterized_template["learning_rate"]

print(f"  Parameterized config: {custom_config.get('brain.neuron_count')} neurons")

print("\n=== Configuration Templates Demo Complete ===")
