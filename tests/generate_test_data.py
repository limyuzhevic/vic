#!/usr/bin/env python3
"""
Generate test data for NLM tests.
This script creates test data files that are used by the test suite.
"""

import numpy as np
import json
import os
import sys
from pathlib import Path

def generate_neuron_data():
    """Generate synthetic neuron data for testing."""
    neurons = []
    for i in range(100):
        neuron = {
            "id": i,
            "type": np.random.choice(["Sensory", "Internal", "Motor"]),
            "membrane_potential": np.random.uniform(-70, -50),
            "threshold": np.random.uniform(-55, -40),
            "refractory_period": np.random.randint(1, 10),
            "conductance": np.random.uniform(0.0, 1.0),
            "capacitance": np.random.uniform(0.1, 1.0)
        }
        neurons.append(neuron)
    
    return neurons

def generate_synapse_data():
    """Generate synthetic synapse data for testing."""
    synapses = []
    for i in range(200):
        synapse = {
            "id": i,
            "pre_id": np.random.randint(0, 100),
            "post_id": np.random.randint(0, 100),
            "weight": np.random.uniform(0.0, 1.0),
            "delay": np.random.randint(1, 5),
            "plasticity_enabled": True,
            "current_weight_change": 0.0
        }
        synapses.append(synapse)
    
    return synapses

def generate_spike_data():
    """Generate synthetic spike data for testing."""
    spikes = []
    for i in range(50):
        for j in range(np.random.randint(0, 20)):
            spike = {
                "neuron_id": np.random.randint(0, 100),
                "time_step": np.random.randint(0, 1000),
                "amplitude": np.random.uniform(0.5, 2.0),
                "type": np.random.choice(["Excitatory", "Inhibitory"])
            }
            spikes.append(spike)
    
    return spikes

def save_test_data(neurons, synapses, spikes):
    """Save generated test data to JSON files."""
    test_data_dir = Path(__file__).parent / "test_data"
    test_data_dir.mkdir(exist_ok=True)
    
    with open(test_data_dir / "neurons.json", 'w') as f:
        json.dump(neurons, f, indent=2)
    
    with open(test_data_dir / "synapses.json", 'w') as f:
        json.dump(synapses, f, indent=2)
    
    with open(test_data_dir / "spikes.json", 'w') as f:
        json.dump(spikes, f, indent=2)
    
    print(f"Generated test data in {test_data_dir}")

def create_config_from_template(template_file, output_file, replacements):
    """Create configuration file from template with replacements."""
    with open(template_file, 'r') as f:
        content = f.read()
    
    for key, value in replacements.items():
        placeholder = f"@{key}@"
        content = content.replace(placeholder, str(value))
    
    with open(output_file, 'w') as f:
        f.write(content)
    
    print(f"Created configuration file: {output_file}")

def main():
    """Main function to generate test data."""
    print("Generating test data for NLM...")
    
    # Generate synthetic data
    neurons = generate_neuron_data()
    synapses = generate_synapse_data()
    spikes = generate_spike_data()
    
    # Save test data
    save_test_data(neurons, synapses, spikes)
    
    # Create test configuration
    test_config_dir = Path(__file__).parent
    template_file = test_config_dir / "config.ini.in"
    output_file = test_config_dir / "config.ini"
    
    # Default replacements
    replacements = {
        "TEST_RANDOM_SEED": 42,
        "TEST_TIMESTEP": 0.001,
        "TEST_NEURON_COUNT": 100,
        "TEST_REGION_COUNT": 2,
        "TEST_CONNECTION_PROBABILITY": 0.1,
        "TEST_PLASTICITY_LEARNING_RATE": 0.01,
        "TEST_STDP_LTP_WEIGHT": 0.01,
        "TEST_STDP_LTD_WEIGHT": 0.012,
        "TEST_STDP_TAU": 20.0,
        "TEST_SYNAPTOGENESIS_RATE": 0.001,
        "TEST_PRUNING_RATE": 0.0001,
        "TEST_DOPAMINE_BASELINE": 0.1,
        "TEST_REWARD_DISCOUNT_FACTOR": 0.99,
        "TEST_ENVIRONMENT_NAME": "GridWorld",
        "TEST_ENVIRONMENT_WIDTH": 10,
        "TEST_ENVIRONMENT_HEIGHT": 10,
        "TEST_LOG_LEVEL": "INFO",
        "TEST_LOG_TO_FILE": "false",
        "TEST_LOG_FILENAME": "nlm_test.log",
        "TEST_MAX_STEPS": 10000,
        "TEST_TIME_LIMIT": 0.0,
        "TEST_VISUALIZATION_ENABLED": "false",
        "TEST_VISUALIZATION_UPDATE_RATE": 30.0
    }
    
    # Allow command-line overrides
    if len(sys.argv) > 1:
        # Parse command-line arguments for replacements
        for i in range(0, len(sys.argv), 2):
            if i + 1 < len(sys.argv) and sys.argv[i].startswith("--"):
                key = sys.argv[i][2:]
                value = sys.argv[i + 1]
                replacements[key] = value
    
    # Create configuration file
    if template_file.exists():
        create_config_from_template(template_file, output_file, replacements)
    else:
        print(f"Warning: Template file {template_file} not found")
    
    print("Test data generation completed successfully!")
    return 0

if __name__ == "__main__":
    sys.exit(main())