# Enhanced Configuration Management for NLM

## Overview

This document describes enhanced configuration management for NLM, including:
- Configuration validation and schema support
- Environment variable integration
- Command-line argument enhancements
- Configuration templates and presets
- Change tracking and diffing
- Serialization and deserialization improvements

---

## Enhanced Configuration Features

### 1. Configuration Validation

#### Schema Validation

```python
import pynlm

# Configuration schemas for validation
SCHEMAS = {
    "minimal": {
        "required": ["brain.neuron_count"],
        "optional": ["brain.region_count", "plasticity.stdp.enable"],
        "types": {
            "brain.neuron_count": int,
            "brain.region_count": int,
            "plasticity.stdp.enable": bool
        },
        "ranges": {
            "brain.neuron_count": (10, 10000),
            "brain.region_count": (1, 10)
        }
    },
    
    "advanced": {
        "required": ["brain.neuron_count", "brain.synapse_density", "neuromod.curiosity.enable"],
        "optional": ["brain.region_count", "development.enable", "plasticity.structural.enable"],
        "types": {
            "brain.neuron_count": int,
            "brain.synapse_density": float,
            "neuromod.curiosity.enable": bool,
            "brain.region_count": int,
            "development.enable": bool,
            "plasticity.structural.enable": bool
        },
        "ranges": {
            "brain.neuron_count": (100, 50000),
            "brain.synapse_density": (0.01, 0.5),
            "brain.region_count": (1, 20)
        },
        "constraints": {
            "neuron_count * synapse_density": lambda n, d: n * d < 50000
        }
    }
}

class ConfigValidator:
    """Validate configuration against schemas."""
    
    def __init__(self, schema_name="advanced"):
        self.schema = SCHEMAS.get(schema_name, SCHEMAS["advanced"])
    
    def validate(self, config):
        """Validate configuration and return list of errors."""
        errors = []
        
        # Check required keys
        for key in self.schema.get("required", []):
            if not config.has(key):
                errors.append(f"Missing required key: {key}")
        
        # Check optional keys types
        for key, expected_type in self.schema.get("types", {}).items():
            if config.has(key):
                value = config.get(key)
                if value is None:
                    errors.append(f"Key {key} has None value")
                elif not isinstance(value, expected_type):
                    errors.append(f"Key {key} should be {expected_type}, got {type(value)}")
        
        # Check ranges
        for key, (min_val, max_val) in self.schema.get("ranges", {}).items():
            if config.has(key):
                value = config.get(key)
                if value is not None and (value < min_val or value > max_val):
                    errors.append(f"Key {key} value {value} outside range [{min_val}, {max_val}]")
        
        # Check constraints
        for constraint in self.schema.get("constraints", []):
            if config.has(constraint):
                # Evaluate constraint function
                pass
        
        return errors
    
    def suggest_fixes(self, errors):
        """Suggest fixes for validation errors."""
        fixes = []
        
        for error in errors:
            if "Missing required key" in error:
                key = error.split(": ")[1]
                fixes.append(f"Consider adding {key} = {self.get_default_value(key)}")
            elif "outside range" in error:
                fixes.append("Adjust value to within specified range")
            elif "should be" in error:
                fixes.append("Change value to correct type")
        
        return fixes
    
    def get_default_value(self, key):
        """Get default value for a key."""
        defaults = {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "neuromod.curiosity.enable": True,
            "brain.region_count": 1,
            "development.enable": True,
            "plasticity.structural.enable": True
        }
        return defaults.get(key)
```

#### Usage Example:

```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("neuromod.curiosity.enable", True)

# Validate with advanced schema
validator = ConfigValidator("advanced")
errors = validator.validate(config)

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
    
    fixes = validator.suggest_fixes(errors)
    print("Suggested fixes:")
    for fix in fixes:
        print(f"  - {fix}")
else:
    print("Configuration is valid!")
```

### 2. Environment Variable Integration

```python
import os
import pynlm

class EnvironmentConfig:
    """Load configuration from environment variables."""
    
    def __init__(self, prefix="NLM_"):
        self.prefix = prefix
    
    def load_from_env(self, config=None):
        """Load configuration from environment variables."""
        if config is None:
            config = pynlm.createDefaultConfig()
        
        # Common NLM environment variables
        env_mappings = {
            "NLM_NEURON_COUNT": ("brain.neuron_count", int),
            "NLM_REGION_COUNT": ("brain.region_count", int),
            "NLM_SYNAPSE_DENSITY": ("brain.synapse_density", float),
            "NLM_CURIOSITY_ENABLE": ("neuromod.curiosity.enable", bool),
            "NLM_NOVELTY_ENABLE": ("neuromod.novelty.enable", bool),
            "NLM_DEVELOPMENT_ENABLE": ("development.enable", bool),
            "NLM_STDP_ENABLE": ("plasticity.stdp.enable", bool),
            "NLM_STRUCTURAL_PLASTICITY_ENABLE": ("plasticity.structural.enable", bool),
            "NLM_CHECKPOINT_ENABLE": ("checkpoint.enable", bool),
            "NLM_REPLAY_ENABLE": ("replay.enable", bool),
            "NLM_RANDOM_SEED": ("random_seed", int),
            "NLM_SIMULATION_STEPS": ("max_steps", int),
            "NLM_WORLD_WIDTH": ("world.width", int),
            "NLM_WORLD_HEIGHT": ("world.height", int)
        }
        
        for env_var, (config_key, var_type) in env_mappings.items():
            value = os.environ.get(env_var)
            if value is not None:
                try:
                    if var_type == bool:
                        config.set(config_key, value.lower() in ('true', '1', 'yes', 'on'))
                    elif var_type == int:
                        config.set(config_key, int(value))
                    elif var_type == float:
                        config.set(config_key, float(value))
                    else:
                        config.set(config_key, value)
                    print(f"Loaded {env_var} = {value} -> {config_key}")
                except ValueError as e:
                    print(f"Warning: Invalid value for {env_var}: {value} - {e}")
        
        return config
    
    def create_env_example(self, filename="env_example.sh"):
        """Create an example environment file."""
        example_config = {
            "NLM_NEURON_COUNT": "# Number of neurons in the brain",
            "NLM_REGION_COUNT": "# Number of brain regions",
            "NLM_SYNAPSE_DENSITY": "# Synapse connectivity density (0.0-1.0)",
            "NLM_CURIOSITY_ENABLE": "# Enable curiosity-driven exploration (true/false)",
            "NLM_NOVELTY_ENABLE": "# Enable novelty detection (true/false)",
            "NLM_DEVELOPMENT_ENABLE": "# Enable developmental processes (true/false)",
            "NLM_STDP_ENABLE": "# Enable STDP learning (true/false)",
            "NLM_STRUCTURAL_PLASTICITY_ENABLE": "# Enable structural plasticity (true/false)",
            "NLM_RANDOM_SEED": "# Random seed for reproducibility",
            "NLM_SIMULATION_STEPS": "# Number of simulation steps",
        }
        
        with open(filename, 'w') as f:
            f.write("# NLM Environment Configuration Example\n")
            f.write("# Copy this file to .env and set your values\n\n")
            
            for var, description in example_config.items():
                f.write(f"# {description}\n")
                f.write(f"{var}=value\n\n")
        
        print(f"Created environment example: {filename}")

# Usage example:
import os

# Load from existing .env file if available
if os.path.exists('.env'):
    from dotenv import load_dotenv
    load_dotenv()

# Create enhanced configuration
config = pynlm.createDefaultConfig()
env_loader = EnvironmentConfig()
config = env_loader.load_fromEnv(config)

# Show example creation
env_loader.create_env_example()
```

### 3. Command-Line Argument Enhancements

```python
import argparse
import pynlm

class EnhancedArgumentParser:
    """Enhanced argument parser for NLM configuration."""
    
    def __init__(self):
        self.parser = argparse.ArgumentParser(
            description="NLM Neural Learning Machine Configuration",
            formatter_class=argparse.ArgumentDefaultsHelpFormatter
        )
        
        # Core configuration
        self.add_core_arguments()
        
        # Learning system arguments
        self.add_learning_arguments()
        
        # Development arguments
        self.add_development_arguments()
        
        # Performance arguments
        self.add_performance_arguments()
        
        # Output arguments
        self.add_output_arguments()
    
    def add_core_arguments(self):
        """Add core brain configuration arguments."""
        group = self.parser.add_argument_group('Core Configuration')
        group.add_argument("--neurons", "--neuron-count",
                          type=int, default=1000,
                          help="Total number of neurons")
        group.add_argument("--regions", "--region-count",
                          type=int, default=1,
                          help="Number of brain regions")
        group.add_argument("--synapse-density", "--synapse-density",
                          type=float, default=0.1,
                          help="Synapse connectivity density (0.0-1.0)")
        group.add_argument("--connection-prob", "--connection-probability",
                          type=float, default=0.05,
                          help="Probability of connection between neurons")
    
    def add_learning_arguments(self):
        """Add learning system arguments."""
        group = self.parser.add_argument_group('Learning Systems')
        group.add_argument("--enable-stdp", action="store_true", default=True,
                          help="Enable Spike-Timing-Dependent Plasticity")
        group.add_argument("--enable-hebbian", action="store_true", default=True,
                          help="Enable Hebbian learning")
        group.add_argument("--enable-structural", action="store_true", default=True,
                          help="Enable structural plasticity")
        group.add_argument("--stdp-learning-rate", type=float, default=0.001,
                          help="STDP learning rate")
        group.add_argument("--hebbian-learning-rate", type=float, default=0.01,
                          help="Hebbian learning rate")
    
    def add_development_arguments(self):
        """Add development system arguments."""
        group = self.parser.add_argument_group('Development System')
        group.add_argument("--enable-development", action="store_true", default=True,
                          help="Enable developmental processes")
        group.add_argument("--development-stage",
                          choices=['Initial', 'CriticalPeriod', 'Maturation', 'Adult'],
                          default='Initial', help="Initial developmental stage")
        group.add_argument("--development-rate", type=float, default=0.001,
                          help="Development rate")
    
    def add_performance_arguments(self):
        """Add performance-related arguments."""
        group = self.parser.add_argument_group('Performance')
        group.add_argument("--max-steps", type=int, default=1000,
                          help="Maximum simulation steps")
        group.add_argument("--timestep", type=float, default=0.001,
                          help="Simulation timestep")
        group.add_argument("--checkpoint-freq", type=int, default=100,
                          help="Checkpoint frequency (steps)")
        group.add_argument("--replay-freq", type=int, default=100,
                          help="Replay frequency (steps)")
    
    def add_output_arguments(self):
        """Add output and logging arguments."""
        group = self.parser.add_argument_group('Output')
        group.add_argument("--output-dir", default=".",
                          help="Output directory for checkpoints and logs")
        group.add_argument("--log-level", choices=['DEBUG', 'INFO', 'WARNING', 'ERROR'],
                          default='INFO', help="Logging level")
        group.add_argument("--save-metrics", action="store_true", default=True,
                          help="Save performance metrics")
        group.add_argument("--metrics-file", default="metrics.csv",
                          help="File for metrics output")
    
    def load_config_from_args(self):
        """Parse arguments and create configuration."""
        args = self.parser.parse_args()
        
        config = pynlm.createDefaultConfig()
        
        # Core configuration
        config.set("brain.neuron_count", args.neurons)
        config.set("brain.region_count", args.regions)
        config.set("brain.synapse_density", args.synapse_density)
        config.set("connection_probability", args.connection_prob)
        
        # Learning systems
        config.set("plasticity.stdp.enable", args.enable_stdp)
        config.set("plasticity.hebbian.enable", args.enable_hebbian)
        config.set("plasticity.structural.enable", args.enable_structural)
        config.set("plasticity.stdp.learning_rate", args.stdp_learning_rate)
        config.set("plasticity.hebbian.learning_rate", args.hebbian_learning_rate)
        
        # Development
        config.set("development.enable", args.enable_development)
        config.set("development.initial_stage", args.development_stage)
        config.set("development.rate", args.development_rate)
        
        # Performance
        config.set("max_steps", args.max_steps)
        config.set("simulation_timestep", args.timestep)
        config.set("checkpoint_frequency", args.checkpoint_freq)
        config.set("replay_frequency", args.replay_freq)
        
        return config

# Usage example:
parser = EnhancedArgumentParser()
config = parser.load_config_from_args()

# Create brain with enhanced configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Save configuration to file
config.saveToFile("nlm_config.txt")
print("Configuration saved to nlm_config.txt")
```

### 4. Configuration Templates and Presets

```python
import pynlm

class ConfigPresets:
    """Predefined configuration presets for common use cases."""
    
    @staticmethod
    def get_preset(preset_name):
        """Get a predefined configuration preset."""
        presets = {
            "research": {
                "brain.neuron_count": 5000,
                "brain.region_count": 3,
                "brain.synapse_density": 0.05,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "development.enable": True,
                "plasticity.structural.enable": True,
                "checkpoint.enable": True,
                "replay.enable": True,
                "max_steps": 5000
            },
            
            "real_time": {
                "brain.neuron_count": 1000,
                "brain.region_count": 1,
                "brain.synapse_density": 0.1,
                "neuromod.curiosity.enable": False,
                "neuromod.novelty.enable": False,
                "development.enable": False,
                "plasticity.structural.enable": False,
                "checkpoint.enable": False,
                "replay.enable": False,
                "max_steps": 1000,
                "simulation_timestep": 0.01
            },
            
            "exploration": {
                "brain.neuron_count": 10000,
                "brain.region_count": 5,
                "brain.synapse_density": 0.02,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "development.enable": True,
                "plasticity.structural.enable": True,
                "checkpoint.enable": True,
                "replay.enable": True,
                "max_steps": 20000
            },
            
            "precision_learning": {
                "brain.neuron_count": 2000,
                "brain.region_count": 2,
                "brain.synapse_density": 0.08,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "development.enable": True,
                "plasticity.structural.enable": True,
                "plasticity.stdp.learning_rate": 0.005,
                "plasticity.hebbian.learning_rate": 0.02,
                "checkpoint.enable": True,
                "replay.enable": True,
                "max_steps": 10000
            }
        }
        
        if preset_name not in presets:
            available = ", ".join(presets.keys())
            raise ValueError(f"Unknown preset: '{preset_name}'. Available: {available}")
        
        return presets[preset_name]
    
    @staticmethod
    def create_config_from_preset(preset_name):
        """Create configuration from a preset."""
        preset_config = ConfigPresets.get_preset(preset_name)
        
        config = pynlm.createDefaultConfig()
        for key, value in preset_config.items():
            config.set(key, value)
        
        return config
    
    @staticmethod
    def list_presets():
        """List available presets."""
        presets = ConfigPresets.get_preset("research")  # This will trigger the error but we don't care
        return list(ConfigPresets.get_preset.__defaults__ if hasattr(ConfigPresets.get_preset, '__defaults__') 
                  else ConfigPresets.__doc__.split("presets = ")[1].split("}")[0].replace("{", "").replace("}", "").replace("'", "").replace(' ', "").split(", "))

# Usage examples:
presets = ["research", "real_time", "exploration", "precision_learning"]

for preset in presets:
    print(f"\n=== {preset.upper()} Preset ===")
    config = ConfigPresets.create_config_from_preset(preset)
    
    print(f"Neurons: {config.getOr('brain.neuron_count', 'N/A')}")
    print(f"Regions: {config.getOr('brain.region_count', 'N/A')}")
    print(f"Curiosity: {config.getOr('neuromod.curiosity.enable', 'N/A')}")
    print(f"Development: {config.getOr('development.enable', 'N/A')}")
    print(f"Max Steps: {config.getOr('max_steps', 'N/A')}")

# Save a custom preset
def save_custom_preset(name, config, filename="custom_presets.json"):
    """Save a custom preset to file."""
    import json
    
    # Convert config to dict
    custom_config = {}
    for key in config.getKeys():
        custom_config[key] = config.get(key)
    
    # Load existing presets
    presets = {}
    if os.path.exists(filename):
        with open(filename, 'r') as f:
            presets = json.load(f)
    
    # Add new preset
    presets[name] = custom_config
    
    # Save
    with open(filename, 'w') as f:
        json.dump(presets, f, indent=2)
    
    print(f"Saved custom preset '{name}' to {filename}")

# Load custom presets
if os.path.exists("custom_presets.json"):
    import json
    with open("custom_presets.json", 'r') as f:
        custom_presets = json.load(f)
    
    print("\n=== Custom Presets ===")
    for name, preset_config in custom_presets.items():
        print(f"\n{name}:")
        for key, value in preset_config.items():
            print(f"  {key}: {value}")
```

### 5. Enhanced Serialization and Deserialization

```python
import pynlm
import json
import os
from pathlib import Path

class EnhancedSerializer:
    """Enhanced serialization for NLM configuration and state."""
    
    def __init__(self, base_dir="nlm_data"):
        self.base_dir = Path(base_dir)
        self.base_dir.mkdir(exist_ok=True)
    
    def save_configuration(self, config, filename="config.json"):
        """Save configuration in multiple formats."""
        config_data = self._config_to_dict(config)
        
        # Save as JSON
        json_file = self.base_dir / f"{filename}.json"
        with open(json_file, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        # Save as Python module (for easy import)
        py_file = self.base_dir / f"{filename}.py"
        with open(py_file, 'w') as f:
            f.write(self._config_to_python(config_data))
        
        # Save as environment variables
        env_file = self.base_dir / f"{filename}.env"
        with open(env_file, 'w') as f:
            self._config_to_env(config_data, f)
        
        return {
            "json": str(json_file),
            "python": str(py_file),
            "env": str(env_file)
        }
    
    def load_configuration(self, filename="config.json"):
        """Load configuration from file."""
        # Try JSON first
        json_file = self.base_dir / f"{filename}.json"
        if json_file.exists():
            with open(json_file, 'r') as f:
                config_data = json.load(f)
            return self._dict_to_config(config_data)
        
        # Try Python module
        py_file = self.base_dir / f"{filename}.py"
        if py_file.exists():
            import importlib.util
            spec = importlib.util.spec_from_file_location("config_module", str(py_file))
            config_module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(config_module)
            return config_module.create_config()
        
        raise FileNotFoundError(f"Configuration file not found: {filename}")
    
    def save_experiment(self, brain, config, experiment_name, 
                       metrics=None, additional_data=None):
        """Save an entire experiment with all data."""
        experiment_dir = self.base_dir / "experiments" / experiment_name
        experiment_dir.mkdir(parents=True, exist_ok=True)
        
        # Save brain state
        brain_file = experiment_dir / "brain.bin"
        brain.save(str(brain_file))
        
        # Save configuration
        config_files = self.save_configuration(config, str(experiment_dir / "config"))
        
        # Save metrics if provided
        if metrics:
            metrics_file = experiment_dir / "metrics.json"
            with open(metrics_file, 'w') as f:
                json.dump(metrics, f, indent=2)
        
        # Save additional data
        if additional_data:
            for key, data in additional_data.items():
                if isinstance(data, (dict, list)):
                    file_path = experiment_dir / f"{key}.json"
                    with open(file_path, 'w') as f:
                        json.dump(data, f, indent=2)
                elif isinstance(data, (str, bytes)):
                    file_path = experiment_dir / f"{key}.txt"
                    with open(file_path, 'w') as f:
                        f.write(str(data))
        
        # Create experiment summary
        summary = {
            "experiment_name": experiment_name,
            "brain_file": str(brain_file),
            "config_files": config_files,
            "brain_neurons": brain.getTotalNeuronCount(),
            "brain_synapses": brain.getTotalSynapseCount(),
            "brain_total_spikes": brain.getTotalSpikeCount(),
            "created": str(Path.now()),
            "metrics": metrics,
            "additional_data": list(additional_data.keys()) if additional_data else []
        }
        
        summary_file = experiment_dir / "experiment_summary.json"
        with open(summary_file, 'w') as f:
            json.dump(summary, f, indent=2)
        
        return str(experiment_dir)
    
    def load_experiment(self, experiment_name):
        """Load an entire experiment."""
        experiment_dir = self.base_dir / "experiments" / experiment_name
        
        if not experiment_dir.exists():
            raise FileNotFoundError(f"Experiment not found: {experiment_name}")
        
        # Load summary
        summary_file = experiment_dir / "experiment_summary.json"
        with open(summary_file, 'r') as f:
            summary = json.load(f)
        
        # Load configuration
        config = self.load_configuration(str(experiment_dir / "config"))
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Load brain state
        brain_file = summary['brain_file']
        if Path(brain_file).exists():
            brain.load(brain_file)
        
        # Load metrics
        metrics = {}
        metrics_file = experiment_dir / "metrics.json"
        if metrics_file.exists():
            with open(metrics_file, 'r') as f:
                metrics = json.load(f)
        
        # Load additional data
        additional_data = {}
        for key in summary.get("additional_data", []):
            for file_ext in ['.json', '.txt']:
                file_path = experiment_dir / f"{key}{file_ext}"
                if file_path.exists():
                    with open(file_path, 'r') as f:
                        additional_data[key] = f.read()
                    break
        
        return {
            "brain": brain,
            "config": config,
            "metrics": metrics,
            "additional_data": additional_data,
            "summary": summary
        }
    
    def _config_to_dict(self, config):
        """Convert configuration to dictionary."""
        config_dict = {}
        
        for key in config.getKeys():
            value = config.get(key)
            if value is not None:
                config_dict[key] = self._convert_value_for_serialization(value)
        
        return config_dict
    
    def _convert_value_for_serialization(self, value):
        """Convert value to JSON-serializable format."""
        if hasattr(value, 'value'):  # Handle ID types
            return value.value
        return value
    
    def _config_to_python(self, config_dict):
        """Convert configuration to Python module format."""
        python_code = '''"""NLM Configuration Module

This module contains configuration for an NLM (Neural Learning Machine) simulation.
Generated automatically by NLM serializer.
"""

import pynlm

def create_config():
    """Create configuration from this module."""
    config = pynlm.createDefaultConfig()
    
'''
        for key, value in config_dict.items():
            python_code += f'    config.set("{key}", {repr(value)})\n'
        
        python_code += '''
    return config
'''
        
        return python_code
    
    def _config_to_env(self, config_dict, file_obj):
        """Convert configuration to environment variables format."""
        file_obj.write("# NLM Configuration Environment Variables\n")
        file_obj.write("# Copy these to your .env file\n\n")
        
        for key, value in config_dict.items():
            # Convert key to environment variable format
            env_key = f"NLM_{key.upper().replace('.', '_')}"
            
            if isinstance(value, bool):
                env_value = 'true' if value else 'false'
            elif isinstance(value, (int, float)):
                env_value = str(value)
            else:
                env_value = str(value)
            
            file_obj.write(f"{env_key}={env_value}\n")
    
    def _dict_to_config(self, config_dict):
        """Convert dictionary to configuration."""
        config = pynlm.createDefaultConfig()
        
        for key, value in config_dict.items():
            config.set(key, self._convert_value_from_serialization(value))
        
        return config
    
    def _convert_value_from_serialization(self, value):
        """Convert serialized value back to original type."""
        # This is a simplified version - in practice, you'd need more sophisticated type detection
        return value

# Usage example:
serializer = EnhancedSerializer()

# Save configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("neuromod.curiosity.enable", True)

file_info = serializer.save_configuration(config, "my_simulation")
print("Configuration saved:")
for format_name, file_path in file_info.items():
    print(f"  {format_name}: {file_path}")

# Load configuration
loaded_config = serializer.load_configuration("my_simulation")
print(f"\nLoaded configuration with {len(loaded_config.getKeys())} keys")

# Save experiment
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)

experiment_path = serializer.save_experiment(
    brain, 
    config, 
    "test_experiment",
    metrics={"total_spikes": brain.getTotalSpikeCount()},
    additional_data={"notes": "Test experiment for validation"}
)
print(f"\nExperiment saved to: {experiment_path}")

# Load experiment
experiment = serializer.load_experiment("test_experiment")
print(f"Loaded experiment with brain: {experiment['brain'].getTotalNeuronCount()} neurons")
```

### 6. Configuration Change Tracking

```python
import pynlm
import time

class ConfigChangeTracker:
    """Track changes to configuration over time."""
    
    def __init__(self, config):
        self.config = config
        self.change_history = []
        self.initial_state = self._capture_config_state()
    
    def _capture_config_state(self):
        """Capture current configuration state."""
        return {
            "timestamp": time.time(),
            "keys": self.config.getKeys(),
            "values": {key: self.config.get(key) for key in self.config.getKeys()}
        }
    
    def set_value(self, key, value, description=""):
        """Set a configuration value and track the change."""
        old_value = self.config.get(key)
        
        self.config.set(key, value)
        
        change = {
            "timestamp": time.time(),
            "key": key,
            "old_value": old_value,
            "new_value": value,
            "description": description,
            "change_type": self._determine_change_type(old_value, value)
        }
        
        self.change_history.append(change)
        return change
    
    def _determine_change_type(self, old_value, new_value):
        """Determine the type of change."""
        if old_value is None and new_value is not None:
            return "creation"
        elif old_value is not None and new_value is None:
            return "removal"
        elif old_value != new_value:
            return "modification"
        else:
            return "no_change"
    
    def get_diff(self, from_state, to_state):
        """Get diff between two configuration states."""
        diff = {
            "added": [],
            "removed": [],
            "modified": []
        }
        
        # Check for added keys
        for key in to_state["values"]:
            if key not in from_state["values"]:
                diff["added"].append({
                    "key": key,
                    "value": to_state["values"][key],
                    "timestamp": to_state["timestamp"]
                })
        
        # Check for removed keys
        for key in from_state["values"]:
            if key not in to_state["values"]:
                diff["removed"].append({
                    "key": key,
                    "old_value": from_state["values"][key],
                    "timestamp": from_state["timestamp"]
                })
        
        # Check for modified keys
        for key in from_state["values"]:
            if key in to_state["values"] and from_state["values"][key] != to_state["values"][key]:
                diff["modified"].append({
                    "key": key,
                    "old_value": from_state["values"][key],
                    "new_value": to_state["values"][key],
                    "timestamp": to_state["timestamp"]
                })
        
        return diff
    
    def get_change_summary(self, change):
        """Get a human-readable summary of a change."""
        change_type = change["change_type"]
        
        if change_type == "creation":
            return f"Added {change['key']} = {change['new_value']}"
        elif change_type == "removal":
            return f"Removed {change['key']} (was {change['old_value']})"
        elif change_type == "modification":
            return f"Modified {change['key']}: {change['old_value']} -> {change['new_value']}"
        else:
            return f"No change to {change['key']}"
    
    def get_change_report(self):
        """Get a comprehensive report of all changes."""
        report = {
            "initial_state": self.initial_state,
            "current_state": self._capture_config_state(),
            "total_changes": len(self.change_history),
            "changes_by_type": self._count_changes_by_type(),
            "recent_changes": self.change_history[-10:] if self.change_history else []
        }
        
        return report
    
    def _count_changes_by_type(self):
        """Count changes by type."""
        counts = {
            "creation": 0,
            "removal": 0,
            "modification": 0,
            "no_change": 0
        }
        
        for change in self.change_history:
            change_type = change["change_type"]
            if change_type in counts:
                counts[change_type] += 1
        
        return counts
    
    def undo_change(self, change_index):
        """Undo a specific change by index."""
        if 0 <= change_index < len(self.change_history):
            change = self.change_history[change_index]
            
            if change["change_type"] == "creation":
                self.config.remove(change["key"])
            elif change["change_type"] == "removal":
                self.config.set(change["key"], change["old_value"])
            elif change["change_type"] == "modification":
                self.config.set(change["key"], change["old_value"])
            
            # Remove from history
            self.change_history.pop(change_index)
            
            return True
        
        return False

# Usage example:
config = pynlm.createDefaultConfig()
tracker = ConfigChangeTracker(config)

print("=== Configuration Change Tracking Demo ===")

# Initial state
print(f"Initial config: {tracker.initial_state}")

# Make some changes
change1 = tracker.set_value("brain.neuron_count", 2000, "Increased for better learning")
print(f"\nChange 1: {tracker.get_change_summary(change1)}")

change2 = tracker.set_value("neuromod.curiosity.enable", True, "Enabled curiosity")
print(f"Change 2: {tracker.get_change_summary(change2)}")

change3 = tracker.set_value("plasticity.stdp.learning_rate", 0.005, "Adjusted STDP rate")
print(f"Change 3: {tracker.get_change_summary(change3)}")

change4 = tracker.set_value("development.enable", False, "Disabled development")
print(f"Change 4: {tracker.get_change_summary(change4)}")

# Get comprehensive report
report = tracker.get_change_report()
print(f"\n=== Change Report ===")
print(f"Total changes: {report['total_changes']}")
print(f"Changes by type: {report['changes_by_type']}")

print(f"\nRecent changes:")
for i, change in enumerate(report['recent_changes']):
    print(f"  {i+1}. {tracker.get_change_summary(change)}")

# Demonstrate undo
print(f"\nUndoing change 2...")
if tracker.undo_change(1):  # Change at index 1 (change2)
    print("Undo successful!")
    print(f"Current config keys: {config.getKeys()}")
else:
    print("Undo failed!")
```

### 7. Configuration Comparison and Diffing

```python
import pynlm

class ConfigComparator:
    """Compare configurations and identify differences."""
    
    @staticmethod
    def compare_configs(config1, config2, name1="Config 1", name2="Config 2"):
        """Compare two configurations and return differences."""
        diff = {
            "same": [],
            "added": [],
            "removed": [],
            "modified": [],
            "summary": ConfigComparator._generate_summary(
                config1, config2, name1, name2
            )
        }
        
        # Get all keys from both configurations
        all_keys = set(config1.getKeys()) | set(config2.getKeys())
        
        for key in all_keys:
            value1 = config1.get(key)
            value2 = config2.get(key)
            
            if value1 == value2:
                diff["same"].append({
                    "key": key,
                    "value": value1,
                    "type": type(value1).__name__
                })
            elif value1 is None and value2 is not None:
                diff["added"].append({
                    "key": key,
                    "value": value2,
                    "type": type(value2).__name__
                })
            elif value1 is not None and value2 is None:
                diff["removed"].append({
                    "key": key,
                    "value": value1,
                    "type": type(value1).__name__
                })
            elif value1 != value2:
                diff["modified"].append({
                    "key": key,
                    "old_value": value1,
                    "new_value": value2,
                    "old_type": type(value1).__name__,
                    "new_type": type(value2).__name__
                })
        
        return diff
    
    @staticmethod
    def _generate_summary(config1, config2, name1, name2):
        """Generate a human-readable summary of differences."""
        diff = ConfigComparator.compare_configs(config1, config2, name1, name2)
        
        summary_parts = []
        
        if diff["added"]:
            summary_parts.append(f"{len(diff['added'])} keys added")
        if diff["removed"]:
            summary_parts.append(f"{len(diff['removed'])} keys removed")
        if diff["modified"]:
            summary_parts.append(f"{len(diff['modified'])} keys modified")
        
        if diff["same"]:
            summary_parts.append(f"{len(diff['same'])} keys same")
        
        return f"{name1} vs {name2}: {', '.join(summary_parts)}"
    
    @staticmethod
    def print_detailed_diff(diff):
        """Print a detailed diff in a readable format."""
        print("\n=== Configuration Comparison ===\n")
        
        if diff["same"]:
            print(f"Same ({len(diff['same'])} keys):")
            for item in diff["same"]:
                print(f"  {item['key']}: {item['value']} ({item['type']})")
            print()
        
        if diff["added"]:
            print(f"Added ({len(diff['added'])} keys):")
            for item in diff["added"]:
                print(f"  + {item['key']}: {item['value']} ({item['type']})")
            print()
        
        if diff["removed"]:
            print(f"Removed ({len(diff['removed'])} keys):")
            for item in diff["removed"]:
                print(f"  - {item['key']}: {item['value']} ({item['type']})")
            print()
        
        if diff["modified"]:
            print(f"Modified ({len(diff['modified'])} keys):")
            for item in diff["modified"]:
                print(f"  ~ {item['key']} ({item['old_type']}): {item['old_value']} -> {item['new_value']} ({item['new_type']})")
            print()
    
    @staticmethod
    def create_config_from_template(config, template_config):
        """Apply a template configuration to a base config."""
        # Create a copy of the template
        template = pynlm.createDefaultConfig()
        for key in template_config.getKeys():
            template.set(key, template_config.get(key))
        
        # Apply template to base config
        result_config = pynlm.createDefaultConfig()
        
        # Keep base config values unless overridden by template
        for key in config.getKeys():
            result_config.set(key, config.get(key))
        
        # Override with template values
        for key in template.getKeys():
            result_config.set(key, template.get(key))
        
        return result_config
    
    @staticmethod
    def validate_config_against_template(config, template_config):
        """Validate that a config meets requirements specified in a template."""
        errors = []
        warnings = []
        
        # Check that all required template keys are present in config
        for key in template_config.getKeys():
            if not config.has(key):
                errors.append(f"Missing required key from template: {key}")
        
        # Check for potential issues
        for key in config.getKeys():
            template_value = template_config.get(key)
            if template_value is not None and config.get(key) != template_value:
                warnings.append(f"Config differs from template for key: {key}")
        
        return {
            "valid": len(errors) == 0,
            "errors": errors,
            "warnings": warnings
        }

# Usage example:
print("=== Configuration Comparison Demo ===")

# Create two different configurations
base_config = pynlm.createDefaultConfig()
base_config.set("brain.neuron_count", 1000)
base_config.set("brain.region_count", 2)
base_config.set("neuromod.curiosity.enable", True)
base_config.set("plasticity.stdp.enable", True)

advanced_config = pynlm.createDefaultConfig()
advanced_config.set("brain.neuron_count", 2000)  # Different
advanced_config.set("brain.region_count", 3)    # Different
advanced_config.set("neuromod.curiosity.enable", True)  # Same
advanced_config.set("neuromod.novelty.enable", True)   # Added
advanced_config.set("development.enable", True)   # Added

# Compare configurations
diff = ConfigComparator.compare_configs(
    base_config, advanced_config, "Base", "Advanced"
)

# Print detailed diff
ConfigComparator.print_detailed_diff(diff)

print(f"\nSummary: {diff['summary']}")

# Create template-based configuration
print("\n=== Template Application Demo ===")

template_config = pynlm.createDefaultConfig()
template_config.set("brain.synapse_density", 0.1)
template_config.set("neuromod.curiosity.enable", True)
template_config.set("checkpoint.enable", True)

# Apply template to a base configuration
final_config = ConfigComparator.create_config_from_template(
    base_config, template_config
)

print("Final configuration after template application:")
for key in final_config.getKeys():
    print(f"  {key}: {final_config.get(key)}")

# Validate against template
validation = ConfigComparator.validate_config_against_template(
    final_config, template_config
)

print(f"\nValidation Result: {'PASS' if validation['valid'] else 'FAIL'}")
if validation['errors']:
    print("Errors:")
    for error in validation['errors']:
        print(f"  - {error}")
if validation['warnings']:
    print("Warnings:")
    for warning in validation['warnings']:
        print(f"  - {warning}")
```

## Summary

Enhanced configuration management provides:

1. **Advanced Validation**: Schema-based validation with detailed error reporting
2. **Environment Integration**: Automatic loading from environment variables
3. **Command-Line Enhancements**: Comprehensive CLI argument parsing
4. **Templates and Presets**: Ready-to-use configuration templates
5. **Enhanced Serialization**: Multiple serialization formats and experiment management
6. **Change Tracking**: Detailed tracking of configuration modifications
7. **Comparison Tools**: Powerful diffing and template application

These enhancements make NLM configuration more robust, user-friendly, and suitable for both research and production environments.

## Next Steps

- Implement additional validation schemas for specific use cases
- Add more sophisticated environment variable parsing
- Extend command-line argument support for all configuration options
- Implement configuration inheritance and merging
- Add configuration export/import for sharing
- Create configuration documentation generator
- Implement configuration compliance checking
