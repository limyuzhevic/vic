# NLM Advanced Usage Guide

This guide provides advanced information for experienced NLM users and researchers.

## Advanced Configuration

### Configuration File Format

NLM configuration files use a structured format with sections and key-value pairs:

```ini
[section_name]
  key = value
  another_key = another_value
```

**Available Sections:**

#### Simulation Section
```ini
[simulation]
  simulation_timestep = 0.001        # Time step for simulation (seconds)
  max_steps = 1000                    # Maximum number of simulation steps
  enable_progress_bar = true          # Show progress indicators
  enable_logging = true               # Enable logging to file
  log_level = "INFO"                  # Logging level: DEBUG, INFO, WARN, ERROR
```

#### Brain Section
```ini
[brain]
  neuron_count = 500                 # Number of neurons
  region_count = 1                    # Number of brain regions
  region_0_type = "Cortical"         # Type of first region
  enable_development = false          # Enable developmental processes
```

#### Memory Systems Section
```ini
[memory]
  enable_working_memory = true        # Enable working memory
  enable_episodic_memory = true       # Enable episodic memory
  enable_associative_memory = true    # Enable associative memory
  wm_capacity = 20                    # Working memory capacity
  wm_decay_rate = 0.001               # Decay rate of working memory
  em_encoding_strength = 0.8          # Strength of episodic memory encoding
  em_replay_frequency = 0.1           # Frequency of memory replay
```

#### Neuromodulation Section
```ini
[neuromodulation]
  enable_dopamine = true              # Enable dopamine system
  enable_novelty = true               # Enable novelty detection
  enable_curiosity = true             # Enable curiosity-driven exploration
  enable_prediction_error = true      # Enable prediction error signals
  dopamine_level = 0.5                # Baseline dopamine level
  novelty_threshold = 0.1             # Threshold for novelty detection
  curiosity_strength = 0.3            # Strength of curiosity drive
```

#### Prediction System Section
```ini
[prediction]
  enable_prediction = true            # Enable predictive processing
  prediction_horizon = 20             # Number of steps to predict ahead
  prediction_error_weight = 0.5       # Weight of prediction error in learning
  confidence_threshold = 0.8          # Confidence threshold for action selection
```

#### Cognitive Systems Section
```ini
[cognition]
  enable_attention = true             # Enable attentional selection
  enable_planning = true              # Enable forward planning
  enable_self_model = true            # Enable self-model formation
  enable_social_learning = true       # Enable social learning
  attention_strength = 0.7            # Strength of attentional bias
  planning_horizon = 10               # Planning depth
```

#### Development Section
```ini
[development]
  enable_development = true           # Enable developmental processes
  developmental_stages = 10           # Number of developmental stages
  plasticity_decay_rate = 0.05        # Rate at which plasticity decreases with age
  maturation_threshold = 0.9          # Threshold for reaching maturity
```

#### Plasticity Section
```ini
[plasticity]
  enable_stdp = true                  # Enable spike-timing-dependent plasticity
  enable_hebbian = true               # Enable Hebbian learning
  enable_reward_modulated = true      # Enable reward-modulated plasticity
  stdp_ltp_weight = 0.02               # STDP LTP weight
  stdp_ltd_weight = 0.015              # STDP LTD weight
  stdp_tau = 20.0                      # STDP time constant
  enable_structural_plasticity = true # Enable structural plasticity
  synaptogenesis_rate = 0.0001         # Rate of new synapse formation
  pruning_rate = 0.00001              # Rate of synapse elimination
  reward_modulation_strength = 0.8    # Strength of reward modulation
```

#### Checkpointing Section
```ini
[checkpointing]
  enable_checkpointing = true         # Enable checkpoint saving
  checkpoint_interval = 1000          # Steps between checkpoints
  checkpoint_history = 10             # Number of checkpoints to keep
  enable_replay = true                 # Enable memory replay during sleep
  replay_interval = 100                # Steps between replay events
```

#### Environment Section
```ini
[environment]
  type = "SimpleWorld"                # Type of environment
  complexity = "normal"               # Environment complexity
  enable_rewards = true                # Enable reward system
  reward_range = "(-10, 10)"          # Reward range
  enable_social_agents = false        # Enable social agents
  social_agent_count = 5              # Number of social agents
```

## Performance Optimization

### Memory Optimization

NLM uses custom memory pools for efficiency:

**Configuration for Memory-Optimized Runs:**
```ini
[simulation]
  enable_progress_bar = false
  enable_logging = false

[brain]
  neuron_count = 2000

[plasticity]
  enable_structural_plasticity = false
  pruning_rate = 0.00005  # Lower pruning for memory efficiency

[environment]
  complexity = "simple"
```

### Speed Optimization

For maximum simulation speed:

```ini
[simulation]
  simulation_timestep = 0.002        # Larger time step
  max_steps = 500                      # Fewer steps
  enable_progress_bar = false

[brain]
  neuron_count = 1000                  # Fewer neurons
  region_count = 2                      # Fewer regions

[plasticity]
  stdp_tau = 10.0                       # Shorter STDP window

[environment]
  complexity = "simple"
```

### Precision Optimization

For research with high precision:

```ini
[simulation]
  simulation_timestep = 0.0001       # Very small time step
  max_steps = 5000                     # Many steps
  enable_progress_bar = true

[brain]
  neuron_count = 5000                  # Many neurons
  region_count = 5                      # Many regions

[plasticity]
  stdp_tau = 50.0                       # Longer STDP window
  synaptogenesis_rate = 0.001          # Higher synapse formation

[environment]
  complexity = "complex"
```

## Custom Configuration Creation

### Configuration Builder Script

**File:** `build_config.py`

```python
#!/usr/bin/env python3

"""
NLM Configuration Builder

This script helps create custom NLM configuration files based on research needs.
"""

import argparse
import json
import os
from pathlib import Path

class NLMConfigBuilder:
    def __init__(self):
        self.config = {
            "simulation": {},
            "brain": {},
            "memory": {},
            "neuromodulation": {},
            "prediction": {},
            "cognition": {},
            "development": {},
            "plasticity": {},
            "checkpointing": {},
            "environment": {}
        }
    
    def set_simulation(self, timestep=0.001, max_steps=1000, 
                      enable_progress_bar=True, log_level="INFO"):
        self.config["simulation"] = {
            "simulation_timestep": timestep,
            "max_steps": max_steps,
            "enable_progress_bar": enable_progress_bar,
            "enable_logging": True,
            "log_level": log_level
        }
        return self
    
    def set_brain(self, neuron_count=500, region_count=1,
                 enable_development=False):
        self.config["brain"] = {
            "neuron_count": neuron_count,
            "region_count": region_count,
            "enable_development": enable_development
        }
        return self
    
    def set_memory(self, enable_working_memory=True,
                  enable_episodic_memory=True,
                  wm_capacity=20, em_encoding_strength=0.8):
        self.config["memory"] = {
            "enable_working_memory": enable_working_memory,
            "enable_episodic_memory": enable_episodic_memory,
            "enable_associative_memory": True,
            "wm_capacity": wm_capacity,
            "em_encoding_strength": em_encoding_strength
        }
        return self
    
    def set_neuromodulation(self, enable_dopamine=True,
                           enable_novelty=True,
                           enable_curiosity=True,
                           dopamine_level=0.5):
        self.config["neuromodulation"] = {
            "enable_dopamine": enable_dopamine,
            "enable_novelty": enable_novelty,
            "enable_curiosity": enable_curiosity,
            "dopamine_level": dopamine_level
        }
        return self
    
    def set_plasticity(self, enable_stdp=True, enable_hebbian=True,
                       stdp_ltp_weight=0.02, stdp_ltd_weight=0.015,
                       stdp_tau=20.0):
        self.config["plasticity"] = {
            "enable_stdp": enable_stdp,
            "enable_hebbian": enable_hebbian,
            "stdp_ltp_weight": stdp_ltp_weight,
            "stdp_ltd_weight": stdp_ltd_weight,
            "stdp_tau": stdp_tau
        }
        return self
    
    def set_environment(self, env_type="SimpleWorld", complexity="normal",
                       enable_rewards=True):
        self.config["environment"] = {
            "type": env_type,
            "complexity": complexity,
            "enable_rewards": enable_rewards
        }
        return self
    
    def save(self, filename):
        """Save configuration to file in INI format"""
        Path(filename).parent.mkdir(parents=True, exist_ok=True)
        
        with open(filename, 'w') as f:
            f.write(f"# Generated NLM Configuration\n")
            f.write(f"# Generated: {__import__('datetime').datetime.now()}\n\n")
            
            for section, values in self.config.items():
                # Skip empty sections
                if not values:
                    continue
                    
                f.write(f"[{section}]\n")
                for key, value in values.items():
                    if isinstance(value, float):
                        f.write(f"  {key} = {value}\n")
                    elif isinstance(value, int):
                        f.write(f"  {key} = {value}\n")
                    elif isinstance(value, bool):
                        f.write(f"  {key} = {'true' if value else 'false'}\n")
                    elif isinstance(value, str):
                        f.write(f"  {key} = {value}\n")
                    else:
                        f.write(f"  {key} = {value}\n")
                f.write("\n")
        
        print(f"Configuration saved to: {filename}")
        return self
    
    def save_json(self, filename):
        """Save configuration to JSON file"""
        Path(filename).parent.mkdir(parents=True, exist_ok=True)
        
        with open(filename, 'w') as f:
            json.dump(self.config, f, indent=2)
        
        print(f"Configuration saved to JSON: {filename}")
        return self
    
    def load(self, filename):
        """Load configuration from file"""
        Path(filename).parent.mkdir(parents=True, exist_ok=True)
        
        # Create a default config if file doesn't exist
        if not os.path.exists(filename):
            print(f"Warning: Config file not found. Using defaults.")
            return self
        
        # Parse INI format
        current_section = None
        with open(filename, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                
                if line.startswith('[') and line.endswith(']'):
                    current_section = line[1:-1]
                    continue
                
                if '=' in line and current_section:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Parse value based on type
                    if value.lower() == 'true':
                        parsed_value = True
                    elif value.lower() == 'false':
                        parsed_value = False
                    elif value.replace('.', '', 1).isdigit():
                        if '.' in value:
                            parsed_value = float(value)
                        else:
                            parsed_value = int(value)
                    else:
                        parsed_value = value
                    
                    # Add to config
                    if current_section not in self.config:
                        self.config[current_section] = {}
                    
                    self.config[current_section][key] = parsed_value
        
        print(f"Configuration loaded from: {filename}")
        return self

def main():
    parser = argparse.ArgumentParser(description="Build NLM configuration files")
    
    parser.add_argument("-o", "--output", default="configs/custom.cfg",
                       help="Output configuration file path")
    parser.add_argument("-l", "--load", help="Load existing configuration file")
    parser.add_argument("--neuron-count", type=int, help="Number of neurons")
    parser.add_argument("--region-count", type=int, help="Number of brain regions")
    parser.add_argument("--steps", type=int, help="Maximum simulation steps")
    parser.add_argument("--timestep", type=float, help="Simulation timestep")
    parser.add_argument("--enable-memory", action="store_true",
                       help="Enable memory systems")
    parser.add_argument("--enable-cognition", action="store_true",
                       help="Enable cognitive systems")
    parser.add_argument("--enable-neuromodulation", action="store_true",
                       help="Enable neuromodulation systems")
    parser.add_argument("--complexity", choices=["simple", "normal", "complex"],
                       default="normal", help="Environment complexity")
    parser.add_argument("--format", choices=["ini", "json"], default="ini",
                       help="Output format")
    
    args = parser.parse_args()
    
    builder = NLMConfigBuilder()
    
    if args.load:
        builder.load(args.load)
    
    # Apply command-line overrides
    if args.neuron_count:
        builder.set_brain(neuron_count=args.neuron_count)
    
    if args.region_count:
        # Note: This is a simplified approach - real implementation would be more complex
        pass
    
    if args.steps:
        builder.set_simulation(max_steps=args.steps)
    
    if args.timestep:
        builder.set_simulation(timestep=args.timestep)
    
    if args.enable_memory:
        builder.set_memory()
    
    if args.enable_cognition:
        # Note: This is a simplified approach - real implementation would be more complex
        pass
    
    if args.enable_neuromodulation:
        builder.set_neuromodulation()
    
    builder.set_environment(complexity=args.complexity)
    
    # Save configuration
    if args.format == "json":
        builder.save_json(args.output)
    else:
        builder.save(args.output)

if __name__ == "__main__":
    main()
```

### Configuration Validation Script

**File:** `validate_config.py`

```python
#!/usr/bin/env python3

"""
NLM Configuration Validator

Validates NLM configuration files for consistency and completeness.
"""

import argparse
import sys
from pathlib import Path

class NLMConfigValidator:
    def __init__(self, config_file):
        self.config_file = Path(config_file)
        self.errors = []
        self.warnings = []
        self.config = {}
    
    def parse_config(self):
        """Parse INI configuration file"""
        current_section = None
        
        with open(self.config_file, 'r') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                
                if not line or line.startswith('#'):
                    continue
                
                if line.startswith('[') and line.endswith(']'):
                    current_section = line[1:-1].strip()
                    continue
                
                if '=' in line and current_section:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Store in config
                    if current_section not in self.config:
                        self.config[current_section] = {}
                    
                    # Parse value
                    if value.lower() == 'true':
                        parsed_value = True
                    elif value.lower() == 'false':
                        parsed_value = False
                    elif value.replace('.', '', 1).isdigit():
                        if '.' in value:
                            parsed_value = float(value)
                        else:
                            parsed_value = int(value)
                    else:
                        parsed_value = value
                    
                    self.config[current_section][key] = parsed_value
    
    def validate(self):
        """Validate configuration"""
        self.parse_config()
        
        # Basic validation
        self.validate_simulation()
        self.validate_brain()
        self.validate_plasticity()
        self.validate_memory()
        self.validate_neuromodulation()
        self.validate_environment()
        
        return len(self.errors) == 0
    
    def validate_simulation(self):
        """Validate simulation configuration"""
        sim = self.config.get("simulation", {})
        
        if "simulation_timestep" in sim:
            timestep = sim["simulation_timestep"]
            if timestep <= 0 or timestep > 1.0:
                self.errors.append(
                    "simulation.timestep must be between 0 and 1.0 seconds"
                )
        
        if "max_steps" in sim:
            max_steps = sim["max_steps"]
            if max_steps <= 0:
                self.errors.append("simulation.max_steps must be positive")
    
    def validate_brain(self):
        """Validate brain configuration"""
        brain = self.config.get("brain", {})
        
        if "neuron_count" in brain:
            neuron_count = brain["neuron_count"]
            if neuron_count <= 0:
                self.errors.append("brain.neuron_count must be positive")
            elif neuron_count > 100000:
                self.warnings.append("brain.neuron_count is very large, may cause memory issues")
    
    def validate_plasticity(self):
        """Validate plasticity configuration"""
        plastic = self.config.get("plasticity", {})
        
        if "stdp_ltp_weight" in plastic and "stdp_ltd_weight" in plastic:
            ltp = plastic["stdp_ltp_weight"]
            ltd = plastic["stdp_ltd_weight"]
            
            if ltp <= 0 or ltd <= 0:
                self.errors.append("plasticity.STDP weights must be positive")
    
    def validate_memory(self):
        """Validate memory configuration"""
        mem = self.config.get("memory", {})
        
        if "wm_capacity" in mem:
            capacity = mem["wm_capacity"]
            if capacity <= 0:
                self.errors.append("memory.wm_capacity must be positive")
    
    def validate_neuromodulation(self):
        """Validate neuromodulation configuration"""
        mod = self.config.get("neuromodulation", {})
        
        if "dopamine_level" in mod:
            level = mod["dopamine_level"]
            if level < 0 or level > 1:
                self.warnings.append("neuromodulation.dopamine_level should be between 0 and 1")
    
    def validate_environment(self):
        """Validate environment configuration"""
        env = self.config.get("environment", {})
        
        if "type" in env:
            env_type = env["type"]
            valid_types = ["SimpleWorld", "ComplexWorld", "SocialWorld", "HardWorld"]
            
            if env_type not in valid_types:
                self.errors.append(
                    f"environment.type must be one of: {', '.join(valid_types)}"
                )
    
    def print_report(self):
        """Print validation report"""
        print(f"=== NLM Configuration Validation Report ===")
        print(f"File: {self.config_file}")
        print()
        
        if self.warnings:
            print("Warnings:")
            for warning in self.warnings:
                print(f"  ⚠ {warning}")
            print()
        
        if self.errors:
            print("Errors:")
            for error in self.errors:
                print(f"  ✗ {error}")
            print()
            print("Configuration is INVALID.")
            return False
        
        print("✓ Configuration is VALID")
        return True

def main():
    parser = argparse.ArgumentParser(description="Validate NLM configuration files")
    parser.add_argument("config_file", help="Configuration file to validate")
    
    args = parser.parse_args()
    
    validator = NLMConfigValidator(args.config_file)
    
    if not validator.validate():
        sys.exit(1)

if __name__ == "__main__":
    main()
```

## Advanced CLI Usage

### Scripted Simulations

For batch processing and automation:

```bash
#!/bin/bash

# Run multiple simulations with different configurations
configs=("configs/basic.cfg" "configs/memory.cfg" "configs/integration.cfg")

for config in "${configs[@]}"; do
    echo "Running simulation with $config"
    nlm run --config "$config" --verbose --output "results/$(basename $config .cfg).log"
    
    # Save brain state
    nlm brain save "results/$(basename $config .cfg)_brain.brain"
    
    # Run analysis
    python analyze_results.py "results/$(basename $config .cfg).log"
    
    echo "---"
done
```

### Configuration Comparison

Compare two configurations:

```bash
#!/bin/bash

compare_configs() {
    local config1=$1
    local config2=$2
    
    echo "=== Configuration Comparison ==="
    echo "Config 1: $config1"
    echo "Config 2: $config2"
    echo
    
    # Compare key parameters
    echo "Key Parameters Comparison:"
    echo "Parameter        | Config 1 | Config 2 | Difference"
    echo "------------------|----------|----------|-----------"
    
    # Neurons
    n1=$(grep "neuron_count" "$config1" | head -1 | cut -d'=' -f2 | tr -d ' ')
    n2=$(grep "neuron_count" "$config2" | head -1 | cut -d'=' -f2 | tr -d ' ')
    echo "neuron_count      | $n1      | $n2      | $(($n2 - $n1))"
    
    # Simulation steps
    s1=$(grep "max_steps" "$config1" | head -1 | cut -d'=' -f2 | tr -d ' ')
    s2=$(grep "max_steps" "$config2" | head -1 | cut -d'=' -f2 | tr -d ' ')
    echo "max_steps        | $s1      | $s2      | $(($s2 - $s1))"
    
    # Time step
    t1=$(grep "simulation_timestep" "$config1" | head -1 | cut -d'=' -f2 | tr -d ' ')
    t2=$(grep "simulation_timestep" "$config2" | head -1 | cut -d'=' -f2 | tr -d ' ')
    echo "simulation_timestep| $t1      | $t2      | $(echo "$t2 - $t1" | bc)"
}

# Usage
if [ $# -ne 2 ]; then
    echo "Usage: $0 <config1> <config2>"
    exit 1
fi

compare_configs "$1" "$2"
```

### Performance Profiling

Profile simulation performance:

```bash
#!/bin/bash

profile_simulation() {
    local config_file=$1
    local output_file=$2
    
    echo "=== Performance Profile ==="
    echo "Configuration: $config_file"
    echo "Output: $output_file"
    echo
    
    # Run with timing
    start_time=$(date +%s.%N)
    nlm run --config "$config_file" --output "$output_file"
    end_time=$(date +%s.%N)
    
    duration=$(echo "$end_time - $start_time" | bc)
    echo "Total time: $duration seconds"
    
    # Extract performance metrics from output
    if [ -f "$output_file" ]; then
        echo "\nPerformance metrics from output:"
        grep -E "(neuron_count|synapse_count|spike_count|firing_rate)" "$output_file" | head -10
        
        # Calculate performance metrics
        echo "\nCalculated metrics:"
        spikes=$(grep "Total spikes:" "$output_file" | tail -1 | cut -d':' -f2 | tr -d ' ')
        neurons=$(grep "neuron_count" "$config_file" | tail -1 | cut -d'=' -f2 | tr -d ' ')
        
        if [ "$neurons" -gt 0 ]; then
            firing_rate=$(echo "scale=2; $spikes / ($duration * $neurons)" | bc)
            echo "Average firing rate: $firing_rate spikes/neuron/second"
        fi
    fi
}

# Usage
if [ $# -ne 2 ]; then
    echo "Usage: $0 <config_file> <output_file>"
    exit 1
fi

profile_simulation "$1" "$2"
```

## Cross-Platform Considerations

### Platform-Specific Configuration

#### Linux Configuration
```ini
[simulation]
  enable_progress_bar = true
  progress_bar_type = "unicode"

[environment]
  type = "LinuxWorld"
```

#### Windows Configuration
```ini
[simulation]
  enable_progress_bar = true
  progress_bar_type = "windows"

[environment]
  type = "WindowsWorld"
```

#### macOS Configuration
```ini
[simulation]
  enable_progress_bar = true
  progress_bar_type = "ascii"

[environment]
  type = "MacOSWorld"
```

### File Path Considerations

Handle platform-specific path separators:

```cpp
// Cross-platform path handling
std::string getPlatformPath(const std::string& path) {
    std::string result = path;
    
#ifdef _WIN32
    std::replace(result.begin(), result.end(), '/', '\\');
#else
    std::replace(result.begin(), result.end(), '\\', '/');
#endif
    
    return result;
}
```

## Development and Testing

### Unit Testing Configuration

Test configuration generation and validation:

```bash
#!/bin/bash

# Run configuration tests
test_configs() {
    echo "=== Testing Configuration Generation ==="
    
    # Test builder script
    python build_config.py --help
    
    # Test validator script
    python validate_config.py --help
    
    # Test with sample config
    python build_config.py --output "test_config.cfg"
    python validate_config.py test_config.cfg
    
    rm -f test_config.cfg
}

# Performance testing
perf_test() {
    echo "=== Performance Testing ==="
    
    # Test different configurations
    configs=("configs/basic.cfg" "configs/memory.cfg" "configs/integration.cfg")
    
    for config in "${configs[@]}"; do
        echo "Testing $config..."
        time nlm run --config "$config" --quiet
        echo "Completed"
    done
}

# Stress testing
stress_test() {
    echo "=== Stress Testing ==="
    
    # Create stress test configuration
    python build_config.py \
        --neuron-count 10000 \
        --steps 5000 \
        --output "stress_test.cfg"
    
    echo "Running stress test...
    time nlm run --config stress_test.cfg"
    
    rm -f stress_test.cfg
}
```

## Configuration Migration

### Migrate from Old to New Format

Migrate configuration from older NLM versions:

```python
#!/usr/bin/env python3

"""
NLM Configuration Migration Tool

Migrates configurations from older NLM versions to the current format.
"""

import os
import re
from pathlib import Path

class NLMConfigMigrator:
    def __init__(self, input_file, output_file):
        self.input_file = Path(input_file)
        self.output_file = Path(output_file)
        self.migration_map = self.get_migration_map()
    
    def get_migration_map(self):
        """Mapping of old parameter names to new names"""
        return {
            "neurons": "neuron_count",
            "regions": "region_count",
            "plasticity_std": "stdp_ltp_weight",
            "plasticity_decay": "stdp_ltd_weight",
            "dt": "simulation_timestep",
            "max_steps": "max_steps",
            "enable_memory": "enable_working_memory",
            "enable_cognition": "enable_attention",
            "enable_neuromod": "enable_dopamine",
            "world_type": "environment.type",
            "world_complexity": "environment.complexity"
        }
    
    def migrate_ini(self):
        """Migrate INI format configuration"""
        content = self.input_file.read_text()
        
        # Convert old format to new format
        lines = content.split('\n')
        migrated_lines = []
        
        for line in lines:
            original_line = line
            
            # Check for old parameter names
            for old_key, new_key in self.migration_map.items():
                if old_key in line and '=' in line:
                    # Extract value
                    value = line.split('=', 1)[1].strip()
                    
                    # Determine section
                    if 'neurons' in line or 'regions' in line:
                        section = "[brain]"
                    elif 'plasticity' in line:
                        section = "[plasticity]"
                    elif 'enable_' in line and line.count('=') == 1:
                        section = "[memory]" if 'memory' in line else "[cognition]"
                    elif 'world' in line:
                        section = "[environment]"
                    else:
                        section = "[simulation]"
                    
                    # Add section header
                    if section not in migrated_lines[-5:]:  # Avoid duplicate sections
                        migrated_lines.append(section)
                    
                    # Add migrated parameter
                    migrated_lines.append(f"  {new_key} = {value}")
                    line = f"# Migrated: {original_line}\n"
            
            if not line.startswith('#'):
                migrated_lines.append(line)
        
        return '\n'.join(migrated_lines)
    
    def migrate_json(self):
        """Migrate JSON configuration"""
        import json
        
        with open(self.input_file, 'r') as f:
            config = json.load(f)
        
        # Apply migrations
        migrated_config = {}
        
        if 'brain' in config:
            migrated_config['brain'] = {
                'neuron_count': config['brain'].get('neurons', 500),
                'region_count': config['brain'].get('regions', 1)
            }
        
        if 'plasticity' in config:
            migrated_config['plasticity'] = {
                'stdp_ltp_weight': config['plasticity'].get('plasticity_std', 0.02),
                'stdp_ltd_weight': config['plasticity'].get('plasticity_decay', 0.015)
            }
        
        # Add other sections with defaults
        if 'simulation' not in config:
            migrated_config['simulation'] = {
                'simulation_timestep': config.get('dt', 0.001),
                'max_steps': config.get('max_steps', 1000)
            }
        
        return migrated_config
    
    def save_config(self, content, is_json=False):
        """Save migrated configuration"""
        self.output_file.parent.mkdir(parents=True, exist_ok=True)
        
        if is_json:
            import json
            with open(self.output_file, 'w') as f:
                json.dump(content, f, indent=2)
        else:
            with open(self.output_file, 'w') as f:
                f.write(content)
        
        print(f"Migrated configuration saved to: {self.output_file}")
    
    def migrate(self, target_format='ini'):
        """Perform migration"""
        print(f"Migrating {self.input_file} to {self.output_file}")
        
        if target_format == 'ini':
            content = self.migrate_ini()
            self.save_config(content)
        elif target_format == 'json':
            content = self.migrate_json()
            self.save_config(content, is_json=True)
        else:
            raise ValueError(f"Unsupported target format: {target_format}")

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="Migrate NLM configuration files")
    parser.add_argument("input", help="Input configuration file")
    parser.add_argument("output", help="Output configuration file")
    parser.add_argument("--format", choices=['ini', 'json'], default='ini',
                       help="Output format")
    
    args = parser.parse_args()
    
    migrator = NLMConfigMigrator(args.input, args.output)
    migrator.migrate(args.format)

if __name__ == "__main__":
    main()
```

## Future Enhancements

### Experimental Configuration Features

#### Machine Learning-based Configuration Optimization
```ini
[ml_optimization]
  enable_optimization = true
  optimization_algorithm = "bayesian_optimization"
  objective_function = "simulation_performance"
  parameter_bounds = {"neuron_count": [100, 1000], "timestep": [0.0001, 0.01]}
```

#### Adaptive Configuration
```ini
[adaptive_config]
  enable_adaptation = true
  adaptation_algorithm = "reinforcement_learning"
  adaptation_interval = 1000
  reward_function = "learning_speed + memory_retention"
```

#### Quantum-inspired Configuration
```ini
[quantum_config]
  enable_quantum = true
  superposition_enabled = true
  entanglement_factor = 0.5
  quantum_tunneling = true
```

## Configuration Best Practices

### Do's
- Use meaningful parameter names
- Document configuration decisions
- Validate configuration before use
- Use version control for configuration files
- Create configuration templates for different use cases

### Don'ts
- Don't hardcode configuration in source code
- Don't ignore validation warnings
- Don't use extremely large values without testing
- Don't mix development and production configurations
- Don't share configuration files with sensitive data

## Troubleshooting

### Configuration Validation Issues

If validation fails:

```bash
# Check for specific errors
python validate_config.py configs/problematic.cfg

# Get detailed error information
python validate_config.py configs/problematic.cfg 2>&1 | grep -A5 "Errors:"
```

### Performance Issues

If simulation is slow:

```bash
# Check system resources
free -h
ps aux | grep nlm

# Try performance-optimized configuration
python build_config.py \
    --neuron-count 1000 \
    --steps 500 \
    --output "perf_test.cfg"

# Run with minimal settings
nlm run --config perf_test.cfg --quiet
```

### Compatibility Issues

If configuration doesn't work:

```bash
# Check configuration syntax
python validate_config.py configs/your.cfg

# Compare with sample configuration
python -c "
import configparser
cp = configparser.ConfigParser()
cp.read('configs/default.cfg')
for section in cp.sections():
    print(f'[{section}]')
    for key, value in cp.items(section):
        print(f'  {key} = {value}')
"
```

This guide provides advanced users with the knowledge and tools to effectively use and customize NLM configurations for their specific needs.