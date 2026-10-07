# NLM Configuration Examples
# 
# This file demonstrates the different configuration file formats supported by NLM
# and shows the improved configuration system features.

---
# Simple Configuration (Legacy Format - Still Supported)
# This is the original key=value format, still works for backward compatibility

brain.neuron_count = 1000
brain.region_count = 2
brain.connection_probability = 0.1
brain.initial_weight_mean = 0.5
brain.v_thresh = -50.0
brain.v_rest = -70.0

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development Configuration
development.enable = true
development.initial_plasticity = 1.0
development.maturation_rate = 0.001

# Memory Configuration
memory.working_memory.capacity = 100
memory.episodic_memory.max_episodes = 1000
memory.associative.enable = true

# Debug & Logging
logging.level = "INFO"
logging.file = "nlm_debug.log"

---
# JSON Configuration (New Format - Recommended)
# This is the new JSON format, supports complex nested structures and better type safety

{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "initial_weight_mean": 0.5,
    "v_thresh": -50.0,
    "v_rest": -70.0
  },
  
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true
    },
    "structural": {
      "enable": true
    }
  },
  
  "neuromod": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true
    },
    "novelty": {
      "enable": true
    }
  },
  
  "development": {
    "enable": true,
    "initial_plasticity": 1.0,
    "maturation_rate": 0.001
  },
  
  "memory": {
    "working_memory": {
      "capacity": 100
    },
    "episodic_memory": {
      "max_episodes": 1000
    },
    "associative": {
      "enable": true
    }
  },
  
  "logging": {
    "level": "INFO",
    "file": "nlm_debug.log"
  }
}

---
# Command Line Configuration Examples
# These show how to override configuration values using command line arguments

# Override neuron count
./nlm --brain.neuron_count=500 --brain.region_count=1

# Multiple overrides
./nlm --brain.neuron_count=2000 --plasticity.stdp.learning_rate=0.01 --neuromod.curiosity.enable=false

# Short format
./nlm -brain.neuron_count 1500 -plasticity.stdp.enable false -neuromod.curiosity.enable true

---
# Python Configuration Examples
# These show how to use the configuration system with Python

import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Modify configuration
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.curiosity.enable", False)

# Create brain with custom configuration
brain2 = pynlm.createBrain(config)
brain2.initialize()

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Use configuration in simulation
for step in range(1000):
    brain2.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain2.getFiringNeuronCount()} neurons firing")

---
# Advanced Configuration Features
# These examples show advanced configuration capabilities

# Configuration with arrays (for multiple values)
brain.v_thresh_values = [-50.0, -52.0, -48.0]
brain.v_rest_values = [-70.0, -72.0, -68.0]

# Enable multiple plasticity mechanisms simultaneously
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Advanced neuromodulation settings
neuromod.dopamine.scale = 2.0
neuromod.dopamine.noise_level = 0.1
neuromod.curiosity.baseline = 0.5
neuromod.curiosity.sensitivity = 1.2

# Development stage configuration
development.stages = [
  {"name": "initial", "plasticity_rate": 1.0, "duration": 100},
  {"name": "critical_period", "plasticity_rate": 2.0, "duration": 500},
  {"name": "maturation", "plasticity_rate": 0.5, "duration": 1000},
  {"name": "adult", "plasticity_rate": 0.1, "duration": 0}
]

# Memory system configuration
memory.working_memory.algorithm = "traces"
memory.episodic_memory.encoding_scheme = "spike_patterns"
memory.associative.similarity_metric = "correlation"
memory.procedural.learning_rate = 0.001
memory.semantic.topics = 50

---
# Configuration Validation Examples
# These examples show how to validate configuration settings

# Validate configuration before use
if config.has("brain.neuron_count"):
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 10 || neuron_count > 100000:
        print("ERROR: Invalid neuron count")
        exit(1)

# Check for required configuration
required_keys = [
    "brain.neuron_count",
    "brain.connection_probability",
    "plasticity.stdp.enable"
]

for key in required_keys:
    if not config.has(key):
        print(f"ERROR: Missing required configuration: {key}")
        exit(1)

# Configuration summary
print("\n=== Configuration Summary ===")
print(config.summary())

---
# Performance Configuration
# These examples show performance-related configuration options

# For fast, lightweight simulation
brain.neuron_count = 100
brain.region_count = 1
brain.connection_probability = 0.01
plasticity.stdp.enable = false  # Disable plasticity for speed
neuromod.enable = false          # Disable neuromodulation
memory.enable = false            # Disable memory systems

# For detailed, high-fidelity simulation
brain.neuron_count = 10000
brain.region_count = 10
brain.connection_probability = 0.1
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true
plasticity.structural.enable = true
neuromod.enable = true
memory.enable = true

# For specific research experiments
# Experimental configuration with specific parameters for research
brain.neuron_count = 500
brain.region_count = 2
plasticity.stdp.tau_plus = 50.0   # Long-term potentiation
plasticity.stdp.tau_minus = 100.0 # Long-term depression
neuromod.curiosity.sensitivity = 2.0  # High curiosity for exploration
development.enable = true         # Enable development for lifespan learning