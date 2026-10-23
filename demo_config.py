#!/usr/bin/env python3
"""
NLM Configuration System Demonstration

This script demonstrates the improved NLM configuration system with
JSON support, validation, and user-friendly tools.
"""

import json
import os
import sys

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "src"))

try:
    from core.Config.Config import Config, ConfigSource
except ImportError as e:
    print(f"Error importing NLM modules: {e}")
    sys.exit(1)

def demo_json_config():
    """Demonstrate JSON configuration file support."""
    print("=" * 60)
    print("DEMONSTRATION: JSON Configuration Support")
    print("=" * 60)
    
    # Create a JSON configuration
    config_data = {
        "brain": {
            "neuron_count": 1000,
            "region_count": 1,
            "connection_probability": 0.1
        },
        "plasticity": {
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_time_constant": 20.0
        },
        "development": {
            "synaptogenesis_rate": 0.0001f,
            "pruning_rate": 0.00001f
        },
        "neuromodulation": {
            "dopamine_baseline": 0.1,
            "curiosity_strength": 0.5
        },
        "simulation": {
            "timestep": 0.001,
            "max_steps": 10000,
            "random_seed": 42
        }
    }
    
    # Write JSON to file
    with open("demo_config.json", "w") as f:
        json.dump(config_data, f, indent=2)
    
    print("✓ Created demo_config.json with JSON structure")
    
    # Load the JSON configuration
    config = Config()
    if config.loadFromFile("demo_config.json"):
        print("✓ Successfully loaded JSON configuration")
        
        # Demonstrate accessing values
        print("\nAccessing configuration values:")
        print(f"  neuron_count: {config.getOr('neuron_count', 0)}")
        print(f"  region_count: {config.getOr('region_count', 0)}")
        print(f"  connection_probability: {config.getOr('connection_probability', 0.0)}")
        print(f"  stdp_ltp_weight: {config.getOr('stdp_ltp_weight', 0.0)}")
        print(f"  simulation_timestep: {config.getOr('simulation_timestep', 0.0)}")
        print(f"  random_seed: {config.getOr('random_seed', 0)}")
        
        # Demonstrate validation
        print("\nDemonstrating validation:")
        config.enableValidation(True)
        
        # Add some validation rules
        config.setRangeValidation("neuron_count", 10, 10000)
        config.setRangeValidation("connection_probability", 0.0, 1.0)
        config.setIntRangeValidation("random_seed", 0, 1000000)
        
        # Test validation
        print(f"  Validating neuron_count (1000): {config.validate('neuron_count', config.getOr('neuron_count', 0))}")
        print(f"  Validating connection_probability (0.1): {config.validate('connection_probability', config.getOr('connection_probability', 0.0))}")
        print(f"  Validating random_seed (42): {config.validate('random_seed', config.getOr('random_seed', 0))}")
        
        # Test invalid value
        print(f"  Validating connection_probability (1.5): {config.validate('connection_probability', 1.5)}")
        
    else:
        print("✗ Failed to load JSON configuration")
    
    # Demonstrate saving to JSON
    print("\nDemonstrating JSON save:")
    config.saveToFile("saved_config.json")
    print("✓ Saved configuration to saved_config.json")
    
    # Show saved JSON
    with open("saved_config.json", "r") as f:
        saved_data = json.load(f)
    
    print("Saved JSON structure:")
    print(json.dumps(saved_data, indent=2))

def demo_legacy_config():
    """Demonstrate legacy configuration file support."""
    print("\n" + "=" * 60)
    print("DEMONSTRATION: Legacy Configuration Support")
    print("=" * 60)
    
    # Create a legacy configuration file
    legacy_config = """# NLM Configuration File
# Legacy format - simple key=value pairs

# Random seed for reproducible experiments
random_seed = 123

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 500

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.15

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Neuromodulation settings
dopamine_baseline = 0.1
reward_discount_factor = 0.99

# Simulation settings
max_simulation_steps = 5000
"""
    
    with open("legacy_config.cfg", "w") as f:
        f.write(legacy_config)
    
    print("✓ Created legacy_config.cfg with legacy format")
    
    # Load the legacy configuration
    config = Config()
    if config.loadFromFile("legacy_config.cfg"):
        print("✓ Successfully loaded legacy configuration")
        
        print("\nAccessing legacy configuration values:")
        print(f"  random_seed: {config.getOr('random_seed', 0)}")
        print(f"  simulation_timestep: {config.getOr('simulation_timestep', 0.0)}")
        print(f"  neuron_count: {config.getOr('neuron_count', 0)}")
        print(f"  region_count: {config.getOr('region_count', 0)}")
        print(f"  connection_probability: {config.getOr('connection_probability', 0.0)}")
        print(f"  plasticity_learning_rate: {config.getOr('plasticity_learning_rate', 0.0)}")
        
    else:
        print("✗ Failed to load legacy configuration")

def demo_validation_features():
    """Demonstrate configuration validation features."""
    print("\n" + "=" * 60)
    print("DEMONSTRATION: Configuration Validation Features")
    print("=" * 60)
    
    config = Config()
    config.enableValidation(True)
    
    # Set up validation rules
    config.setRangeValidation("population_size", 1, 10000)
    config.setRangeValidation("learning_rate", 0.0, 1.0)
    config.setIntRangeValidation("epochs", 1, 1000)
    config.setStringLengthValidation("model_name", 1, 50)
    config.setAllowedValuesValidation("optimizer", {0.0, 0.01, 0.1, 1.0})  # {SGD, Adam, RMSprop, Adagrad}
    
    # Set parameter descriptions
    config.setParameterDescription("population_size", "Number of neurons in each population")
    config.setParameterDescription("learning_rate", "Learning rate for plasticity rules (0.0 to 1.0)")
    config.setParameterDescription("epochs", "Number of training epochs")
    config.setParameterDescription("model_name", "Name identifier for the neural model")
    config.setParameterDescription("optimizer", "Optimization algorithm to use")
    
    # Add some configuration values
    config.set("population_size", 1000)
    config.set("learning_rate", 0.01)
    config.set("epochs", 100)
    config.set("model_name", "LIF_Network_v2")
    config.set("optimizer", 0.01)  # Adam optimizer
    
    print("✓ Configuration with validation rules set up")
    
    # Demonstrate validation
    print("\nTesting validation:")
    print(f"  population_size (1000): {config.validate('population_size', 1000)}")
    print(f"  learning_rate (0.01): {config.validate('learning_rate', 0.01)}")
    print(f"  epochs (100): {config.validate('epochs', 100)}")
    print(f"  model_name (short): {config.validate('model_name', 'test')}")
    print(f"  model_name (too long): {config.validate('model_name', 'x' * 100)}")
    print(f"  optimizer (0.01): {config.validate('optimizer', 0.01)}")
    print(f"  optimizer (invalid): {config.validate('optimizer', 0.5)}")
    
    # Demonstrate full validation
    print(f"\nFull validation check: {config.validateAll()}")
    
    # Show validation summary
    print("\nValidation summary:")
    print(config.summary())

def demo_experiment_config():
    """Demonstrate experiment configuration management."""
    print("\n" + "=" * 60)
    print("DEMONSTRATION: Experiment Configuration Management")
    print("=" * 60)
    
    try:
        from experiments.ExperimentRunner import ExperimentRunner
    except ImportError:
        print("Skipping experiment demo - ExperimentRunner not available")
        return
    
    # Create experiment runner
    runner = ExperimentRunner()
    
    # Create experiments
    exp1 = runner.createExperiment("Connectivity_Test")
    exp2 = runner.createExperiment("Plasticity_Study")
    exp3 = runner.createExperiment("Development_Experiment")
    
    # Apply configuration to experiments
    config = Config()
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    
    # Apply config to experiments
    for exp in [exp1, exp2, exp3]:
        for key in config.getKeys():
            exp.setConfigValue(key, str(config.getOr(key, "")))
    
    print("✓ Created and configured experiments")
    
    # Show experiment information
    print("\nExperiment details:")
    for exp in [exp1, exp2, exp3]:
        print(f"  {exp.getName()} (seed: {exp.getSeed()})")
        print(f"    Config keys: {len(exp.getConfig())}")
        for key, value in exp.getConfig():
            print(f"      {key} = {value}")
    
    # Save experiment results
    print("\nSaving experiment results:")
    if runner.saveResults("demo_experiments.json"):
        print("✓ Successfully saved experiment results to demo_experiments.json")
        
        # Show saved results
        with open("demo_experiments.json", "r") as f:
            experiments_data = json.load(f)
        
        print(f"  Saved {len(experiments_data)} experiments")
    else:
        print("✗ Failed to save experiment results")

def main():
    print("NLM Configuration System Demonstration")
    print("=" * 60)
    
    # Run demonstrations
    demo_json_config()
    demo_legacy_config()
    demo_validation_features()
    demo_experiment_config()
    
    print("\n" + "=" * 60)
    print("DEMONSTRATION COMPLETE")
    print("=" * 60)
    print("\nKey improvements demonstrated:")
    print("  1. JSON configuration file support with validation")
    print("  2. Backward compatibility with legacy key=value format")
    print("  3. Comprehensive configuration validation system")
    print("  4. Parameter descriptions and validation rules")
    print("  5. Experiment configuration management")
    print("  6. Results saving in JSON format")
    print("\nConfiguration files created:")
    print("  - demo_config.json: JSON configuration example")
    print("  - saved_config.json: Validated configuration")
    print("  - legacy_config.cfg: Legacy configuration example")
    print("  - demo_experiments.json: Experiment results")
    
    print("\nTo continue exploring, run the nlm-config command-line tool:")
    print("  python3 nlm-config [command] [options]")

if __name__ == "__main__":
    main()
