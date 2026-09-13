#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <cmath>
#include <random>
#include <chrono>
#include <iomanip>
#include <set>

#include "nlm/Constants.hpp"
#include "nlm/CLI.hpp"

namespace nlm {

/**
 * Example configuration file template for NLM users
 */
class ExampleConfig {
public:
    static std::string getDefaultConfig() {
        return R"(#!/usr/bin/env cfg
# NLM (熙然) - Neural Learning Machine Configuration Example
# 
# This is an example configuration file for NLM Phase 2 neural computation experiments.
# Copy this file to your project and modify as needed for your experiments.
#
# Configuration options and their valid values:

# --- Basic Simulation Parameters ---
[random_section]
# Basic simulation settings
random_seed: 42                           # Random number generator seed (64-bit integer)
simulation_timestep: 0.001               # Simulation time step in seconds (0.0001 - 0.1)
neuron_count: 500                        # Number of neurons (10 - 100000)
region_count: 1                          # Number of neural regions (1 - 100)
connection_probability: 0.15             # Probability of connection between neurons (0.0 - 1.0)
max_simulation_steps: 10000              # Maximum simulation steps (optional)

# --- Neuron Parameters ---
[neuron_section]
# Neuron type: sensory, internal, motor, excitatory, inhibitory, modulatory
neuron_type: internal                    # Default neuron type
leak_conductance: 10.0                   # Membrane leak conductance (nS)
threshold: -55.0                        # Firing threshold (mV)
resting_potential: -70.0                # Resting potential (mV)
reset_potential: -70.0                  # Post-spike reset potential (mV)
refractory_period: 5                    # Refractory period (simulation steps)
adaptation_rate: 0.01                   # Spike-frequency adaptation rate
max_membrane_potential: 50.0            # Safety limit (mV)
min_membrane_potential: -100.0          # Safety limit (mV)

# --- Synapse Parameters ---
synapse_section
weight: 0.5                             # Default synaptic weight (0.0 - 10.0)
delay: 2                                # Synaptic delay (steps)
conductance: 0.5                        # Synaptic conductance (nS)
max_weight: 10.0                        # Maximum synaptic weight
min_weight: 0.0                          # Minimum synaptic weight
max_delay: 100                          # Maximum synaptic delay

# --- Plasticity Parameters ---
[plasticity_section]
stdp_ltp_weight: 0.02                   # STDP LTP learning rate
stdp_ltd_weight: 0.015                  # STDP LTD learning rate
stdp_tau: 20.0                          # STDP time constant (ms)
stdp_trace_decay: 0.9                   # STDP trace decay rate (0.0 - 1.0)
hebbian_rate: 0.01                      # Hebbian learning rate
reward_modulation_rate: 0.1             # Reward-modulated learning rate

# --- Structural Plasticity ---
development_section
enable_synaptogenesis: true            # Enable synapse formation
synaptogenesis_rate: 0.0001             # Synapse formation rate (per step per neuron)
enable_pruning: true                    # Enable synapse elimination
pruning_rate: 0.00001                   # Synapse elimination rate (per step per neuron)
enable_maturation: true                 # Enable synapse maturation
maturation_rate: 0.001                  # Synapse maturation rate
enable_critical_period: true            # Enable critical period modulation
critical_period_start: 1.0              # Critical period start time (seconds)
critical_period_end: 10.0               # Critical period end time (seconds)

# --- Memory System Parameters ---
memory_section
enable_working_memory: true             # Enable working memory system
working_memory_size: 100                # Working memory capacity (number of traces)
enable_episodic_memory: true            # Enable episodic memory system
episodic_memory_capacity: 1000          # Maximum episodes to store
episodic_memory_recall_probability: 0.8 # Probability of successful recall
pattern_completion_threshold: 0.7       # Threshold for pattern completion

# --- Neuromodulation Parameters ---
neuromodulation_section
enable_dopamine: true                  # Enable dopamine system
dopamine_release_rate: 0.1              # Dopamine release rate
enable_curiosity: true                  # Enable curiosity system
curiosity_rate: 0.05                    # Curiosity generation rate
enable_novelty: true                    # Enable novelty detection
novelty_threshold: 0.5                  # Novelty detection threshold
enable_prediction_error: true           # Enable prediction error system
prediction_error_rate: 0.01             # Prediction error rate

# --- Experiment Configuration ---
experiment_section
experiment_type: plasticity               # Type of experiment (connectivity, plasticity, stdp, all)
steps: 1000                             # Number of simulation steps
input_current: 30.0                     # External input current (0.0 - 1000.0)
weight_change_threshold: 0.01           # Threshold for detecting weight changes
learning_detection_threshold: 0.001     # Threshold for detecting learning
enable_plotting: false                  # Enable experimental plotting
output_directory: ./results            # Directory for output files

# --- Development Parameters ---
development_phase: adult                 # Development stage (initial, critical_period, maturation, adult)
plasticity_modulation: true             # Enable plasticity modulation by development

# --- Logging Parameters ---
log_section
log_level: info                         # Log level (trace, debug, info, warning, error, critical)
log_file:                                # Log file path (empty for console only)
log_format: "%timestamp %level %source: %message"  # Log message format
enable_performance_logging: false       # Enable performance metrics logging

# --- Output and Analysis ---
output_section
enable_checkpointing: false             # Enable checkpoint save/load
checkpoint_path: ./checkpoint.bin      # Checkpoint file path
enable_replay: false                    # Enable memory replay
save_plots: false                       # Save experimental plots
plot_format: png                         # Plot format (png, pdf, svg)

# --- Advanced Options ---
advanced_section
verbose: false                          # Enable verbose output
quiet: false                            # Suppress non-error output
enable_caching: true                    # Enable caching for better performance
max_memory_usage: 512                   # Maximum memory usage (MB)
enable_multithreading: false            # Enable multithreaded processing
num_threads: 4                          # Number of threads (if multithreading enabled)

# --- Example Usage ---
# To use this configuration:
# 1. Save as my_config.cfg
# 2. Run: ./nlm_phase2 --config my_config.cfg
# 3. Or include in your C++ code:
#    config.loadFromFile("my_config.cfg");
#
# For advanced usage, use command line options:
# ./nlm_phase2 --help                    # Show help
# ./nlm_phase2 --experiment plasticity --steps 5000 --input_current 50.0
#
# Configuration options can also be overridden with command line arguments.
";
    }
    
    static std::string getINIExample() {
        return R"([nlm_config]
# Basic simulation settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Neuron parameters
neuron_type = internal
leak_conductance = 10.0
threshold = -55.0
resting_potential = -70.0
refractory_period = 5

# Synapse parameters
weight = 0.5
delay = 2

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Development parameters
enable_synaptogenesis = true
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory parameters
working_memory_size = 100
episodic_memory_capacity = 1000

# Neuromodulation parameters
dopamine_release_rate = 0.1
curiosity_rate = 0.05
";
    }
    
    static std::string getJSONExample() {
        return R"({
    "nlm_config": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15,
        
        "neuron": {
            "type": "internal",
            "leak_conductance": 10.0,
            "threshold": -55.0,
            "resting_potential": -70.0,
            "refractory_period": 5
        },
        
        "synapse": {
            "weight": 0.5,
            "delay": 2
        },
        
        "plasticity": {
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "hebbian_rate": 0.01
        },
        
        "development": {
            "enable_synaptogenesis": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        },
        
        "memory": {
            "working_memory_size": 100,
            "episodic_memory_capacity": 1000
        },
        
        "neuromodulation": {
            "dopamine_release_rate": 0.1,
            "curiosity_rate": 0.05
        },
        
        "experiment": {
            "type": "plasticity",
            "steps": 1000,
            "input_current": 30.0
        }
    }
}
";
    }
};

} // namespace nlm
