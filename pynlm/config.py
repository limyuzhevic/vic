import pynlm
from pathlib import Path
from typing import List, Dict, Optional, Union
import json
import yaml
import numpy as np
from datetime import datetime
import hashlib

class BrainConfigurator:
    """
    Advanced configuration management for NLM brains.
    
    This class provides sophisticated configuration management including
    preset configurations, parameter sweeps, validation, and serialization.
    """
    
    # Built-in configuration presets
    BUILTIN_PRESETS = {
        'tiny': {
            'name': 'Tiny Brain (Minimal)',
            'description': 'Very small brain for testing and debugging',
            'brain.neuron_count': 50,
            'brain.synapse_density': 0.02,
            'brain.connection_probability': 0.02,
            'random_seed': 42,
            'simulation_timestep': 0.001,
        },
        'small': {
            'name': 'Small Brain (Basic)',
            'description': 'Small brain for basic simulations',
            'brain.neuron_count': 200,
            'brain.synapse_density': 0.05,
            'brain.connection_probability': 0.05,
            'random_seed': 42,
            'simulation_timestep': 0.001,
        },
        'medium': {
            'name': 'Medium Brain (Standard)',
            'description': 'Standard-sized brain for most simulations',
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.1,
            'brain.connection_probability': 0.1,
            'random_seed': 42,
            'simulation_timestep': 0.001,
        },
        'large': {
            'name': 'Large Brain (Advanced)',
            'description': 'Large brain for complex simulations',
            'brain.neuron_count': 5000,
            'brain.synapse_density': 0.05,
            'brain.connection_probability': 0.08,
            'random_seed': 42,
            'simulation_timestep': 0.0005,
        },
        'research': {
            'name': 'Research Brain (High Density)',
            'description': 'High-density brain for research simulations',
            'brain.neuron_count': 10000,
            'brain.synapse_density': 0.2,
            'brain.connection_probability': 0.15,
            'random_seed': 12345,
            'simulation_timestep': 0.0001,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.01,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromodulation.curiosity.enable': True,
            'neuromodulation.novelty.enable': True,
        },
        'debug': {
            'name': 'Debug Configuration',
            'description': 'Debug-friendly configuration with deterministic behavior',
            'brain.neuron_count': 100,
            'brain.synapse_density': 0.05,
            'random_seed': 999,
            'simulation_timestep': 0.001,
            'brain.v_thresh': -45.0,  # Higher threshold for easier spiking
            'brain.v_reset': -60.0,
        }
    }
    
    # Default configuration values
    DEFAULT_CONFIG = {
        'brain.neuron_count': 1000,
        'brain.synapse_density': 0.1,
        'brain.connection_probability': 0.05,
        'brain.initial_weight_mean': 0.5,
        'brain.initial_weight_std': 0.1,
        'brain.v_thresh': -50.0,
        'brain.v_rest': -70.0,
        'brain.v_reset': -75.0,
        'brain.tau_mem': 20.0,
        'brain.tau_ref': 9,  # Phase 6: 9 steps
        'random_seed': 42,
        'simulation_timestep': 0.001,
        'stdp_ltp_weight': 0.01,
        'stdp_ltd_weight': 0.012,
        'stdp_tau': 20.0,
        'plasticity.stdp.enable': True,
        'plasticity.stdp.learning_rate': 0.01,
        'plasticity.hebbian.enable': True,
        'plasticity.structural.enable': True,
        'synaptogenesis_rate': 0.0001,
        'pruning_rate': 0.00001,
        'neuromodulation.curiosity.enable': True,
        'neuromodulation.novelty.enable': True,
        'neuromodulation.dopamine.scale': 1.0,
        'enable_development': True,
        'developmental_stage': 'Initial',
    }
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.load_defaults()
    
    def load_defaults(self):
        """Load default configuration values."""
        for key, value in self.DEFAULT_CONFIG.items():
            self.config.set(key, value)
    
    def apply_preset(self, preset_name: str) -> 'BrainConfigurator':
        """
        Apply a built-in configuration preset.
        
        Args:
            preset_name: Name of preset ('tiny', 'small', 'medium', 'large', 'research', 'debug')
        
        Returns:
            Self for method chaining
        """
        preset_name = preset_name.lower()
        
        if preset_name in self.BUILTIN_PRESETS:
            preset = self.BUILTIN_PRESETS[preset_name]
            for key, value in preset.items():
                if key != 'name' and key != 'description':
                    self.config.set(key, value)
            print(f"Applied preset: {preset['name']}")
            print(f"Description: {preset['description']}")
        else:
            available = ', '.join(f"'{name}'" for name in self.BUILTIN_PRESETS.keys())
            raise ValueError(f"Unknown preset '{preset_name}'. Available presets: {available}")
        
        return self
    
    def update(self, **kwargs):
        """
        Update configuration with key-value pairs.
        
        Args:
            **kwargs: Configuration key-value pairs
        """
        for key, value in kwargs.items():
            self.config.set(key, value)
        return self
    
    def get(self, key: str, default=None):
        """
        Get configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
        
        Returns:
            Configuration value or default
        """
        value = self.config.get(key)
        return value if value is not None else default
    
    def has(self, key: str) -> bool:
        """Check if configuration has key."""
        return self.config.has(key)
    
    def keys(self):
        """Get all configuration keys."""
        return self.config.getKeys()
    
    def to_dict(self) -> Dict:
        """Convert configuration to dictionary."""
        result = {}
        for key in self.keys():
            result[key] = self.get(key)
        return result
    
    def from_dict(self, config_dict: Dict) -> 'BrainConfigurator':
        """
        Update configuration from dictionary.
        
        Args:
            config_dict: Dictionary with configuration key-value pairs
        
        Returns:
            Self for method chaining
        """
        for key, value in config_dict.items():
            self.config.set(key, value)
        return self
    
    def load_from_json(self, filepath: str) -> 'BrainConfigurator':
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to JSON file
        
        Returns:
            Self for method chaining
        """
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        
        return self.from_dict(config_dict)
    
    def save_to_json(self, filepath: str, indent: int = 2) -> str:
        """
        Save configuration to JSON file.
        
        Args:
            filepath: Path to save JSON file
            indent: JSON indentation
        
        Returns:
            Path to saved file
        """
        Path(filepath).parent.mkdir(parents=True, exist_ok=True)
        
        with open(filepath, 'w') as f:
            json.dump(self.to_dict(), f, indent=indent)
        
        return filepath
    
    def load_from_yaml(self, filepath: str) -> 'BrainConfigurator':
        """
        Load configuration from YAML file.
        
        Args:
            filepath: Path to YAML file
        
        Returns:
            Self for method chaining
        """
        try:
            import yaml
            with open(filepath, 'r') as f:
                config_dict = yaml.safe_load(f)
            
            return self.from_dict(config_dict)
        except ImportError:
            raise ImportError("PyYAML is required for YAML support. Install with: pip install pyyaml")
    
    def save_to_yaml(self, filepath: str) -> str:
        """
        Save configuration to YAML file.
        
        Args:
            filepath: Path to save YAML file
        
        Returns:
            Path to saved file
        """
        try:
            import yaml
            Path(filepath).parent.mkdir(parents=True, exist_ok=True)
            
            with open(filepath, 'w') as f:
                yaml.dump(self.to_dict(), f, default_flow_field=False)
            
            return filepath
        except ImportError:
            raise ImportError("PyYAML is required for YAML support. Install with: pip install pyyaml")
    
    def validate(self) -> List[str]:
        """
        Validate configuration.
        
        Returns:
            List of validation error messages (empty if valid)
        """
        errors = []
        
        # Check neuron count is reasonable
        neuron_count = self.get('brain.neuron_count')
        if neuron_count is not None:
            if neuron_count <= 0:
                errors.append("brain.neuron_count must be positive")
            elif neuron_count > 100000:
                errors.append("brain.neuron_count is very large, may cause performance issues")
        
        # Check synapse density is reasonable
        synapse_density = self.get('brain.synapse_density')
        if synapse_density is not None:
            if not (0.0 <= synapse_density <= 1.0):
                errors.append("brain.synapse_density must be between 0.0 and 1.0")
        
        # Check connection probability is reasonable
        conn_prob = self.get('brain.connection_probability')
        if conn_prob is not None:
            if not (0.0 <= conn_prob <= 1.0):
                errors.append("brain.connection_probability must be between 0.0 and 1.0")
        
        return errors
    
    def clone(self) -> 'BrainConfigurator':
        """Create a copy of this configuration."""
        cloned = BrainConfigurator()
        cloned.config = self.config  # Note: This shares the underlying config object
        return cloned
    
    def print_summary(self):
        """Print configuration summary."""
        print("=== Brain Configuration ===")
        print(f"Total settings: {len(self.keys())}")
        
        # Show key settings
        important_keys = [
            'brain.neuron_count',
            'brain.synapse_density',
            'brain.connection_probability',
            'random_seed',
            'simulation_timestep',
            'plasticity.stdp.enable',
            'neuromodulation.curiosity.enable'
        ]
        
        print("\nKey Settings:")
        for key in important_keys:
            if self.has(key):
                value = self.get(key)
                print(f"  {key}: {value}")
        
        # Print validation results
        validation_errors = self.validate()
        if validation_errors:
            print(f"\nValidation Errors ({len(validation_errors)}):")
            for error in validation_errors:
                print(f"  - {error}")
        else:
            print("\n✓ Configuration is valid")

class ParameterSweep:
    """
    Utility for running parameter sweeps across configurations.
    
    This class provides functionality to generate multiple configurations
    with different parameter values for systematic experimentation.
    """
    
    def __init__(self, base_config: BrainConfigurator):
        self.base_config = base_config
        self.parameter_grid = []
    
    def add_parameter(self, param_name: str, values: List):
        """
        Add a parameter to the sweep.
        
        Args:
            param_name: Name of parameter (e.g., 'brain.neuron_count')
            values: List of values to sweep over
        """
        self.parameter_grid.append((param_name, values))
    
    def generate_configurations(self) -> List[BrainConfigurator]:
        """
        Generate all configurations for the parameter sweep.
        
        Returns:
            List of BrainConfigurator objects
        """
        if not self.parameter_grid:
            # Return just the base configuration
            return [self.base_config]
        
        configurations = []
        
        # Generate all combinations
        param_combinations = self._generate_combinations()
        
        for combination in param_combinations:
            config = self.base_config.clone()
            
            # Apply parameter values
            for param_name, value in combination.items():
                config.config.set(param_name, value)
            
            configurations.append(config)
        
        return configurations
    
    def _generate_combinations(self) -> List[Dict]:
        """Generate all parameter combinations."""
        if not self.parameter_grid:
            return [{}]
        
        # Use recursion to generate all combinations
        def generate_combinations(index, current_combination):
            if index == len(self.parameter_grid):
                return [current_combination.copy()]
            
            param_name, values = self.parameter_grid[index]
            combinations = []
            
            for value in values:
                current_combination[param_name] = value
                combinations.extend(generate_combinations(index + 1, current_combination))
            
            return combinations
        
        return generate_combinations(0, {})
    
    def run_sweep_with_results(self, config_processor):
        """
        Run parameter sweep and collect results.
        
        Args:
            config_processor: Function that takes a BrainConfigurator and returns results
        """
        configurations = self.generate_configurations()
        results = []
        
        for i, config in enumerate(configurations):
            print(f"Running configuration {i+1}/{len(configurations)}...")
            
            # Run the processor
            result = config_processor(config)
            result['configuration'] = config.to_dict()
            result['configuration_index'] = i
            
            results.append(result)
        
        return results
    
    def print_sweep_summary(self):
        """Print summary of parameter sweep."""
        print("=== Parameter Sweep Summary ===")
        print(f"Total configurations: {len(self.generate_configurations())}")
        
        for param_name, values in self.parameter_grid:
            print(f"{param_name}: {len(values)} values")

class ExperimentManager:
    """
    Utility for managing and running experiments.
    
    This class provides tools for organizing experiments, tracking results,
    and managing experiment lifecycle.
    """
    
    def __init__(self, experiment_name: str, base_dir: str = "experiments"):
        self.experiment_name = experiment_name
        self.base_dir = Path(base_dir)
        self.base_dir.mkdir(exist_ok=True)
        
        # Create experiment directory
        self.experiment_dir = self.base_dir / experiment_name
        self.experiment_dir.mkdir(exist_ok=True)
        
        # Create subdirectories
        (self.experiment_dir / "configs").mkdir(exist_ok=True)
        (self.experiment_dir / "results").mkdir(exist_ok=True)
        (self.experiment_dir / "checkpoints").mkdir(exist_ok=True)
        
        # Load or create experiment metadata
        self.metadata_file = self.experiment_dir / "metadata.json"
        self.metadata = self._load_metadata()
    
    def _load_metadata(self) -> Dict:
        """Load experiment metadata."""
        if self.metadata_file.exists():
            with open(self.metadata_file, 'r') as f:
                return json.load(f)
        else:
            return {
                'name': self.experiment_name,
                'created': datetime.now().isoformat(),
                'trials': [],
                'parameters': {}
            }
    
    def _save_metadata(self):
        """Save experiment metadata."""
        with open(self.metadata_file, 'w') as f:
            json.dump(self.metadata, f, indent=2)
    
    def add_parameter(self, name: str, value):
        """Add a parameter to the experiment configuration."""
        self.metadata['parameters'][name] = value
        self._save_metadata()
    
    def create_config(self) -> pynlm.Config:
        """Create configuration from experiment parameters."""
        config = pynlm.createDefaultConfig()
        
        for key, value in self.metadata['parameters'].items():
            config.set(key, value)
        
        return config
    
    def run_trial(self, trial_name: str, config: pynlm.Config, 
                  world=None, agent=None, steps: int = 100) -> Dict:
        """
        Run a single trial.
        
        Args:
            trial_name: Name of the trial
            config: Brain configuration
            world: World object (optional)
            agent: Agent object (optional)
            steps: Number of simulation steps
        
        Returns:
            Trial results
        """
        print(f"Running trial: {trial_name}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Track trial start time
        start_time = datetime.now()
        
        # Run simulation
        if world and agent:
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Record trial end time
        end_time = datetime.now()
        duration = (end_time - start_time).total_seconds()
        
        # Collect results
        result = {
            'trial_name': trial_name,
            'start_time': start_time.isoformat(),
            'end_time': end_time.isoformat(),
            'duration_seconds': duration,
            'brain_stats': {
                'neuron_count': brain.getTotalNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'developmental_stage': brain.getDevelopmentalStage(),
            },
            'success': True
        }
        
        # Save brain checkpoint
        checkpoint_file = self.experiment_dir / "checkpoints" / f"{trial_name}_brain.bin"
        brain.save(str(checkpoint_file))
        
        # Add to metadata
        self.metadata['trials'].append(result)
        self._save_metadata()
        
        return result
    
    def list_trials(self) -> List[Dict]:
        """List all trials for this experiment."""
        return self.metadata.get('trials', [])
    
    def get_trial_summary(self, trial_index: int) -> Dict:
        """Get summary for specific trial."""
        trials = self.list_trials()
        if trial_index < len(trials):
            return trials[trial_index]
        return {}
    
    def print_experiment_summary(self):
        """Print experiment summary."""
        print(f"=== Experiment: {self.experiment_name} ===")
        print(f"Base directory: {self.experiment_dir}")
        print(f"Total trials: {len(self.metadata.get('trials', []))}")
        print(f"Parameters: {len(self.metadata.get('parameters', {}))}")
        
        if self.metadata.get('trials'):
            print("\nRecent Trials:")
            for i, trial in enumerate(self.metadata['trials'][-3:]):  # Show last 3 trials
                idx = len(self.metadata['trials']) - 3 + i
                print(f"  {idx+1}. {trial['trial_name']}: "
                      f"{trial['duration_seconds']:.1f}s, "
                      f"{trial['brain_stats']['total_spikes']:,} spikes")

# Convenience functions for common operations
def create_research_config(neuron_count=1000, enable_all_systems=True) -> pynlm.Config:
    """
    Create a research-grade configuration with sensible defaults.
    
    Args:
        neuron_count: Number of neurons (default: 1000)
        enable_all_systems: Whether to enable all neuromodulation and plasticity systems
    
    Returns:
        Configured NLM Config object
    """
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)
    config.set("brain.connection_probability", 0.05)
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    
    if enable_all_systems:
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromodulation.curiosity.enable", True)
        config.set("neuromodulation.novelty.enable", True)
        config.set("neuromodulation.dopamine.scale", 1.0)
        config.set("enable_development", True)
    
    return config

def run_research_brain(num_neurons=1000, num_steps=1000, config=None):
    """
    Run a research brain with standard configuration.
    
    Args:
        num_neurons: Number of neurons
        num_steps: Number of simulation steps
        config: Optional configuration (will be created if not provided)
    
    Returns:
        Tuple of (brain, config)
    """
    if config is None:
        config = create_research_config(num_neurons)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(num_steps):
        brain.step(step)
    
    return brain, config

def create_experiment_suite(base_dir="experiments") -> ExperimentManager:
    """
    Create an experiment manager for systematic research.
    
    Args:
        base_dir: Base directory for experiments
    
    Returns:
        ExperimentManager instance
    """
    return ExperimentManager("default_experiment", base_dir)

# Main execution example
if __name__ == "__main__":
    print("NLM Configuration Manager - Example Usage")
    print("=" * 50)
    
    # Example 1: Use built-in presets
    print("\n1. Using Built-in Presets")
    config1 = BrainConfigurator()
    config1.apply_preset("small")
    config1.print_summary()
    
    # Example 2: Custom configuration
    print("\n2. Custom Configuration")
    config2 = BrainConfigurator()
    config2.update({
        'brain.neuron_count': 2000,
        'brain.synapse_density': 0.15,
        'random_seed': 123,
        'neuromodulation.curiosity.enable': True
    })
    config2.print_summary()
    
    # Example 3: Save and load configuration
    print("\n3. Save/Load Configuration")
    config_file = "test_config.json"
    config2.save_to_json(config_file)
    print(f"Saved configuration to {config_file}")
    
    # Load it back
    config3 = BrainConfigurator()
    config3.load_from_json(config_file)
    print("Loaded configuration and summary:")
    config3.print_summary()
    
    # Example 4: Parameter sweep
    print("\n4. Parameter Sweep")
    base_config = BrainConfigurator()
    base_config.apply_preset("medium")
    
    sweep = ParameterSweep(base_config)
    sweep.add_parameter("brain.neuron_count", [500, 1000, 2000])
    sweep.add_parameter("random_seed", [42, 123, 456])
    
    print(f"Generated {len(sweep.generate_configurations())} configurations")
    sweep.print_sweep_summary()
    
    # Example 5: Experiment management
    print("\n5. Experiment Management")
    experiment = create_experiment_suite("demo_experiment")
    experiment.add_parameter("brain.neuron_count", 1000)
    experiment.add_parameter("random_seed", 42)
    
    # Create a simple brain and run a trial
    config = experiment.create_config()
    brain, _ = run_research_brain(num_neurons=1000, num_steps=100)
    
    trial_result = experiment.run_trial(
        "baseline_test",
        config,
        steps=100
    )
    
    experiment.print_experiment_summary()
    
    print("\n" + "=" * 50)
    print("Configuration Manager examples completed successfully!")
