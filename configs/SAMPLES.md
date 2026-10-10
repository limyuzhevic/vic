// Sample JSON configuration file
{
  "nlm": {
    "version": "2.0",
    "description": "Neural Learning Machine Configuration",
    
    "system": {
      "random_seed": 42,
      "simulation_timestep": 0.001
    },
    
    "neural": {
      "neuron_count": 500,
      "region_count": 1,
      "connection_probability": 0.15
    }
  }
}

// Sample YAML configuration file
yaml: '2.1'
nlm:
  version: '2.0'
  description: 'Neural Learning Machine Configuration'
  
  system:
    random_seed: 42
    simulation_timestep: 0.001
  
  neural:
    neuron_count: 500
    region_count: 1
    connection_probability: 0.15

// Sample key=value format (for backward compatibility)
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1
