# Advanced Configuration Examples for NLM Phase 2
# This file contains example configurations for different use cases

## 1. Basic Neural Network Configuration
# File: basic_neural.cfg
# Description: Simple configuration for basic neural network simulation

neuron_count = 500
region_count = 5
connection_probability = 0.12
simulation_timestep = 0.001

stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

working_memory_capacity = 100
e pisodic_memory_max_episodes = 200

## 2. Complex Learning Configuration
# File: learning_experiment.json
# Description: Advanced configuration for learning experiments

{
  "name": "Complex Learning Experiment",
  "parameters": {
    "neuron_count": 2000,
    "region_count": 20,
    "connection_probability": 0.18
  },
  "plasticity": {
    "stdp": {
      "ltp_weight": 0.025,
      "ltd_weight": 0.02,
      "tau": 25.0
    },
    "hebbian": {
      "learning_rate": 0.015
    }
  },
  "neuromodulation": {
    "reward": {
      "baseline_dopamine": 0.15,
      "prediction_gain": 3.0
    }
  },
  "memory": {
    "working_memory": {
      "capacity": 500,
      "decay_rate": 0.95
    }
  },
  "experiment": {
    "type": "advanced_learning",
    "duration": 5000,
    "evaluation_metric": "synaptic_strength_change"
  }
}

## 3. Performance Testing Configuration
# File: performance_test.yaml
# Description: Configuration optimized for performance testing

name: Performance Test Configuration
parameters:
  neuron_count: 5000
  region_count: 50
  connection_probability: 0.2
simulation_timestep: 0.0005

plasticity:
  stdp:
    ltp_weight: 0.03
    ltd_weight: 0.025
    tau: 30.0
  hebbian:
    learning_rate: 0.02

environment:
  type: simulated
  dimensions:
    width: 200
    height: 200
    depth: 20
  rewards:
    step_penalty: -0.001
    goal_reward: 1000.0

## 4. Developmental Configuration
# File: developmental.cfg
# Description: Configuration for developmental processes

neuron_count = 1000
region_count = 10

# Development parameters
development_enabled = true
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
maturation_time = 100.0

# Neuromodulation for development
curiosity_enabled = true
novelty_detection = true

# Memory for developmental tracking
episodic_memory_max_episodes = 5000
working_memory_capacity = 200

## 5. Migration Example
# File: old_format.cfg
# Description: Example of an old configuration format (to be migrated)

# Old format: simple key=value pairs
neuron_count = 100
region_count = 2
stdp_weight = 0.01
hebbian_weight = 0.005

# This would be migrated to the new format with proper structure and nesting
