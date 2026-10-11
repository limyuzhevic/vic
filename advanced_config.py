#!/usr/bin/env python3
"""
Advanced Python Configuration Utilities for NLM

This module provides enhanced configuration management utilities for NLM,
extending the basic configuration system with advanced features for research
and production environments.
"""

import json
import os
import sys
import pynlm
from typing import Dict, Any, List, Optional, Union
from pathlib import Path


class NLMConfigManager:
    """
    Enhanced configuration manager for NLM with advanced features:
    - Configuration validation
    - Profile-based configurations
    - Configuration templates
    - Configuration comparison
    - Configuration migration
    """
    
    def __init__(self, config_file: Optional[str] = None):
        self.config = pynlm.createDefaultConfig()
        self.config_file = config_file
        self.profiles_dir = "nlm_profiles"
        
        # Create profiles directory if it doesn't exist
        os.makedirs(self.profiles_dir, exist_ok=True)
        
        # Load profiles
        self.profiles = self._load_profiles()
        
    def _load_profiles(self) -> Dict[str, Any]:
        """Load configuration profiles from the profiles directory"""
        profiles = {}
        
        if os.path.exists(self.profiles_dir):
            for filename in os.listdir(self.profiles_dir):
                if filename.endswith('.json'):
                    filepath = os.path.join(self.profiles_dir, filename)
                    try:
                        with open(filepath, 'r') as f:
                            profile_name = filename[:-5]  # Remove .json extension
                            profiles[profile_name] = json.load(f)
                    except Exception as e:
                        print(f"Warning: Failed to load profile {filename}: {e}")
        
        return profiles
    
    def load_from_profile(self, profile_name: str) -> bool:
        """Load configuration from a profile"""
        if profile_name not in self.profiles:
            print(f"Error: Profile '{profile_name}' not found")
            return False
        
        # Clear current config
        self.config.clear()
        
        # Apply profile settings
        profile = self.profiles[profile_name]
        for key, value in profile.items():
            self.config.set(key, value)
        
        return True
    
    def save_to_profile(self, profile_name: str, description: str = "") -> bool:
        """Save current configuration to a profile"""
        # Create profile dict
        profile = {}
        keys = self.config.getKeys()
        
        for key in keys:
            profile[key] = self.config.get(key)
        
        # Add metadata
        profile['_metadata'] = {
            'description': description,
            'created': time.strftime('%Y-%m-%d %H:%M:%S'),
            'version': '1.0'
        }
        
        # Save profile
        filepath = os.path.join(self.profiles_dir, f"{profile_name}.json")
        try:
            with open(filepath, 'w') as f:
                json.dump(profile, f, indent=2)
            
            print(f"Profile '{profile_name}' saved to {filepath}")
            return True
        except Exception as e:
            print(f"Error saving profile: {e}")
            return False
    
    def validate_config(self) -> List[str]:
        """Validate current configuration and return list of errors"""
        errors = []
        
        # Check for basic consistency issues
        neuron_count = self.config.getOr<int64_t>('brain.neuron_count', 1000)
        if neuron_count <= 0:
            errors.append("neuron_count must be positive")
        
        if neuron_count > 100000:
            print("Warning: Very large neuron count may impact performance")
        
        # Check plasticity configuration consistency
        if self.config.getOr<bool>('plasticity.stdp.enable', False):
            if self.config.getOr<float>('plasticity.stdp.learning_rate', 0.001) <= 0:
                errors.append("STDP learning rate must be positive when enabled")
        
        if self.config.getOr<bool>('plasticity.hebbian.enable', False):
            if self.config.getOr<float>('plasticity.hebbian.learning_rate', 0.001) <= 0:
                errors.append("Hebbian learning rate must be positive when enabled")
        
        # Check developmental consistency
        if self.config.getOr<bool>('development.enable', False):
            critical_period = self.config.getOr<float>('development.critical_period_end', 60.0)
            maturation_end = self.config.getOr<float>('development.maturation_end', 300.0)
            if critical_period >= maturation_end:
                errors.append("critical_period_end must be less than maturation_end")
        
        # Check world configuration
        if hasattr(self, 'world_configured') and self.world_configured:
            vision_width = self.config.getOr<int64_t>('world.vision_width', 8)
            vision_height = self.config.getOr<int64_t>('world.vision_height', 8)
            if vision_width <= 0 or vision_height <= 0:
                errors.append("Vision dimensions must be positive")
        
        return errors
    
    def apply_preset(self, preset_name: str) -> bool:
        """Apply a predefined configuration preset"""
        presets = {
            'research': {
                'brain.neuron_count': 2000,
                'brain.synapse_density': 0.05,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'development.enable': True,
                'development.initial_plasticity': 1.0,
                'development.critical_period_end': 60.0,
                'development.maturation_end': 300.0,
                'world.enable': True,
                'world.vision_width': 16,
                'world.vision_height': 16,
            },
            'production': {
                'brain.neuron_count': 500,
                'brain.synapse_density': 0.1,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': False,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': False,
                'development.enable': False,
                'world.enable': True,
                'world.vision_width': 8,
                'world.vision_height': 8,
                'performance.optimization_level': 'medium',
            },
            'learning': {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.08,
                'plasticity.stdp.enable': True,
                'plasticity.stdp.learning_rate': 0.005,
                'plasticity.hebbian.enable': True,
                'plasticity.hebbian.learning_rate': 0.002,
                'plasticity.structural.enable': True,
                'neuromod.dopamine.scale': 2.0,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.prediction_error.enable': True,
                'development.enable': True,
                'development.initial_plasticity': 1.5,
                'world.enable': True,
                'world.vision_width': 12,
                'world.vision_height': 12,
            },
            'simulation': {
                'brain.neuron_count': 100,
                'brain.synapse_density': 0.03,
                'plasticity.stdp.enable': False,
                'plasticity.hebbian.enable': False,
                'plasticity.structural.enable': False,
                'neuromod.curiosity.enable': False,
                'neuromod.novelty.enable': False,
                'development.enable': False,
                'world.enable': True,
                'world.vision_width': 4,
                'world.vision_height': 4,
                'simulation.realtime_factor': 1.0,
                'simulation.checkpoint_interval': 1000,
            },
        }
        
        if preset_name not in presets:
            print(f"Error: Preset '{preset_name}' not found. Available presets: {list(presets.keys())}")
            return False
        
        # Clear and apply preset
        self.config.clear()
        for key, value in presets[preset_name].items():
            self.config.set(key, value)
        
        print(f"Applied preset '{preset_name}'")
        return True
    
    def export_config(self, filepath: str, include_metadata: bool = True) -> bool:
        """Export configuration to JSON file"""
        try:
            config_data = {}
            keys = self.config.getKeys()
            
            for key in keys:
                config_data[key] = self.config.get(key)
            
            if include_metadata:
                config_data['_exported'] = {
                    'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
                    'version': '1.0',
                    'tool': 'NLM Advanced Configuration Utilities'
                }
            
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
            
            print(f"Configuration exported to {filepath}")
            return True
        except Exception as e:
            print(f"Error exporting configuration: {e}")
            return False
    
    def import_config(self, filepath: str) -> bool:
        """Import configuration from JSON file"""
        try:
            with open(filepath, 'r') as f:
                config_data = json.load(f)
            
            # Clear current config
            self.config.clear()
            
            # Import configuration (skip metadata)
            for key, value in config_data.items():
                if key.startswith('_'):
                    continue
                self.config.set(key, value)
            
            print(f"Configuration imported from {filepath}")
            return True
        except Exception as e:
            print(f"Error importing configuration: {e}")
            return False
    
    def diff_configs(self, other_config_manager) -> Dict[str, str]:
        """Compare two configurations and return differences"""
        differences = {}
        
        # Get keys from both configs
        keys1 = set(self.config.getKeys())
        keys2 = set(other_config_manager.config.getKeys())
        
        # Keys in config1 but not in config2
        for key in keys1 - keys2:
            differences[f"Only in config1: {key}"] = str(self.config.get(key))
        
        # Keys in config2 but not in config1
        for key in keys2 - keys1:
            differences[f"Only in config2: {key}"] = str(other_config_manager.config.get(key))
        
        # Different values for same keys
        for key in keys1 & keys2:
            val1 = self.config.get(key)
            val2 = other_config_manager.config.get(key)
            if val1 != val2:
                differences[f"Different value for {key}:"] = f"  Config1: {val1}\n  Config2: {val2}"
        
        return differences
    
    def backup_config(self, backup_dir: str = "nlm_backups") -> bool:
        """Create a backup of current configuration"""
        try:
            os.makedirs(backup_dir, exist_ok=True)
            
            timestamp = time.strftime('%Y%m%d_%H%M%S')
            backup_file = os.path.join(backup_dir, f"config_backup_{timestamp}.json")
            
            return self.export_config(backup_file)
        except Exception as e:
            print(f"Error creating backup: {e}")
            return False


class NLMExperimentManager:
    """
    Advanced experiment management utilities for NLM.
    
    This class helps organize and run complex experiments with:
    - Experiment parameter sweeps
    - Result recording and analysis
    - Multi-condition experiments
    - Statistical analysis
    """
    
    def __init__(self, base_dir: str = "nlm_experiments"):
        self.base_dir = Path(base_dir)
        self.base_dir.mkdir(exist_ok=True)
        
    def run_parameter_sweep(self, param_sweep: Dict[str, List[Any]], 
                          brain_factory_func, experiment_func,
                          experiment_name: str) -> List[Dict[str, Any]]:
        """
        Run an experiment with parameter sweeps.
        
        Args:
            param_sweep: Dictionary mapping parameter names to lists of values
            brain_factory_func: Function that creates a brain with given config
            experiment_func: Function that runs experiment and returns results
            experiment_name: Name for the experiment
        """
        # Create experiment directory
        exp_dir = self.base_dir / experiment_name
        exp_dir.mkdir(exist_ok=True)
        
        # Generate all parameter combinations
        param_names = list(param_sweep.keys())
        param_values = list(param_sweep.values())
        
        from itertools import product
        combinations = list(product(*param_values))
        
        print(f"Running {len(combinations)} combinations for experiment '{experiment_name}'")
        
        results = []
        
        for i, combination in enumerate(combinations):
            # Create config
            config = pynlm.createDefaultConfig()
            
            # Set parameters
            for name, value in zip(param_names, combination):
                config.set(name, value)
            
            # Create brain
            brain = brain_factory_func(config)
            
            # Run experiment
            result = experiment_func(brain)
            
            # Save result
            result_file = exp_dir / f"result_{i:04d}.json"
            with open(result_file, 'w') as f:
                json.dump(result, f, indent=2)
            
            result_summary = {
                'combination': dict(zip(param_names, combination)),
                'result_file': str(result_file),
                'result_summary': result
            }
            results.append(result_summary)
            
            print(f"  Completed {i+1}/{len(combinations)}")
        
        # Create summary report
        summary_file = exp_dir / "experiment_summary.json"
        with open(summary_file, 'w') as f:
            json.dump({
                'experiment_name': experiment_name,
                'parameter_sweep': param_sweep,
                'total_combinations': len(combinations),
                'completed_combinations': len(results),
                'results': results
            }, f, indent=2)
        
        print(f"Parameter sweep completed. Results saved to {exp_dir}")
        return results
    
    def analyze_results(self, experiment_dir: str, analysis_type: str = 'summary') -> Dict[str, Any]:
        """Analyze experiment results"""
        exp_dir = Path(experiment_dir)
        
        # Find all result files
        result_files = list(exp_dir.glob("result_*.json"))
        
        if not result_files:
            print(f"No result files found in {experiment_dir}")
            return {}
        
        # Load results
        results = []
        for result_file in result_files:
            with open(result_file, 'r') as f:
                results.append(json.load(f))
        
        # Analyze based on type
        if analysis_type == 'summary':
            return self._analyze_summary(results)
        elif analysis_type == 'performance':
            return self._analyze_performance(results)
        elif analysis_type == 'learning':
            return self._analyze_learning(results)
        else:
            print(f"Unknown analysis type: {analysis_type}")
            return {}
    
    def _analyze_summary(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Basic analysis of experiment results"""
        if not results:
            return {}
        
        # Extract key metrics from all results
        metrics = {}
        
        for result in results:
            if 'result_summary' in result:
                for key, value in result['result_summary'].items():
                    if key not in metrics:
                        metrics[key] = []
                    metrics[key].append(value)
        
        # Calculate statistics
        analysis = {
            'total_experiments': len(results),
            'parameters_tested': list(results[0]['combination'].keys()) if results else [],
            'metrics': {}
        }
        
        for key, values in metrics.items():
            if isinstance(values[0], (int, float)):
                analysis['metrics'][key] = {
                    'mean': sum(values) / len(values),
                    'min': min(values),
                    'max': max(values),
                    'std': ((sum((x - sum(values)/len(values))**2 for x in values) / len(values))**0.5) if len(values) > 1 else 0
                }
            elif isinstance(values[0], dict):
                # Nested metrics
                nested_metrics = {}
                for val_dict in values:
                    for k, v in val_dict.items():
                        if k not in nested_metrics:
                            nested_metrics[k] = []
                        nested_metrics[k].append(v)
                
                analysis['metrics'][key] = {}
                for nested_key, nested_values in nested_metrics.items():
                    if isinstance(nested_values[0], (int, float)):
                        analysis['metrics'][key][nested_key] = {
                            'mean': sum(nested_values) / len(nested_values),
                            'min': min(nested_values),
                            'max': max(nested_values),
                            'std': ((sum((x - sum(nested_values)/len(nested_values))**2 for x in nested_values) / len(nested_values))**0.5) if len(nested_values) > 1 else 0
                        }
        
        return analysis
    
    def _analyze_performance(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze performance-related results"""
        return self._analyze_summary(results)
    
    def _analyze_learning(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze learning-related results"""
        return self._analyze_summary(results)


class NLMVisualizationManager:
    """
    Advanced visualization utilities for NLM results.
    
    Provides tools for visualizing simulation results, performance metrics,
    and experimental data.
    """
    
    def __init__(self, output_dir: str = "nlm_visualizations"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
    
    def plot_results(self, results: Dict[str, Any], plot_type: str, 
                    output_file: Optional[str] = None) -> bool:
        """Generate visualization of results"""
        # This would require matplotlib or similar library
        # For now, we'll create a basic text-based visualization
        
        if plot_type == 'results_summary':
            return self._plot_results_summary(results, output_file)
        elif plot_type == 'performance_trend':
            return self._plot_performance_trend(results, output_file)
        elif plot_type == 'parameter_impact':
            return self._plot_parameter_impact(results, output_file)
        else:
            print(f"Unsupported plot type: {plot_type}")
            return False
    
    def _plot_results_summary(self, results: Dict[str, Any], output_file: Optional[str] = None) -> bool:
        """Create summary visualization of results"""
        if output_file is None:
            output_file = str(self.output_dir / "results_summary.txt")
        
        try:
            with open(output_file, 'w') as f:
                f.write("NLM Experiment Results Summary\n")
                f.write("=" * 50 + "\n\n")
                
                if 'total_experiments' in results:
                    f.write(f"Total Experiments: {results['total_experiments']}\n")
                
                if 'parameters_tested' in results:
                    f.write(f"Parameters Tested: {', '.join(results['parameters_tested'])}\n")
                
                if 'metrics' in results:
                    f.write("\nMetric Statistics:\n")
                    for metric_name, stats in results['metrics'].items():
                        f.write(f"  {metric_name}:\n")
                        for stat_name, stat_value in stats.items():
                            if isinstance(stat_value, dict):
                                f.write(f"    {stat_name}:\n")
                                for sub_name, sub_value in stat_value.items():
                                    f.write(f"      {sub_name}: {sub_value:.3f}\n")
                            else:
                                f.write(f"    {stat_name}: {stat_value:.3f}\n")
            
            print(f"Results summary saved to {output_file}")
            return True
        except Exception as e:
            print(f"Error creating results summary: {e}")
            return False
    
    def _plot_performance_trend(self, results: Dict[str, Any], output_file: Optional[str] = None) -> bool:
        """Create performance trend visualization"""
        # Similar to results summary for now
        return self._plot_results_summary(results, output_file)
    
    def _plot_parameter_impact(self, results: Dict[str, Any], output_file: Optional[str] = None) -> bool:
        """Create parameter impact visualization"""
        # Similar to results summary for now
        return self._plot_results_summary(results, output_file)


# Utility functions for common tasks
def create_research_configuration():
    """Create a configuration suitable for research experiments"""
    config = pynlm.createDefaultConfig()
    
    # Research-oriented settings
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.05)
    config.set("brain.connection_probability", 0.03)
    
    # Enable all plasticity mechanisms
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Fine-tune plasticity parameters for research
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.0005)
    config.set("plasticity.structural.synapse_creation_rate", 0.00005)
    config.set("plasticity.structural.synapse_pruning_rate", 0.00001)
    
    # Enable all neuromodulation
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    
    # Enable development for age-dependent studies
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.2)
    config.set("development.critical_period_end", 120.0)
    config.set("development.maturation_end", 600.0)
    
    # Enable world interaction for embodied cognition
    config.set("world.enable", True)
    config.set("world.vision_width", 16)
    config.set("world.vision_height", 16)
    config.set("world.enable_vision", True)
    config.set("world.enable_touch", True)
    config.set("world.enable_internal", True)
    
    # Enable memory systems
    config.set("memory.working_memory.enable", True)
    config.set("memory.episodic_memory.enable", True)
    config.set("memory.associative_memory.enable", True)
    
    # Enable prediction systems
    config.set("prediction.enable", True)
    config.set("prediction.temporal_horizon", 100)
    
    return config


def create_production_configuration():
    """Create a configuration suitable for production/stable operation"""
    config = pynlm.createDefaultConfig()
    
    # Stable, reliable settings for production
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("brain.connection_probability", 0.05)
    
    # Core plasticity only
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", False)  # Disabled for stability
    
    # Moderate neuromodulation
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", False)  # Disabled for consistency
    config.set("neuromod.prediction_error.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    
    # Minimal development
    config.set("development.enable", False)  # Fixed developmental stage
    
    # Standard world configuration
    config.set("world.enable", True)
    config.set("world.vision_width", 8)
    config.set("world.vision_height", 8)
    
    # Performance optimizations
    config.set("performance.optimization_level", "high")
    config.set("performance.use_event_queues", True)
    config.set("performance.use_memory_pools", True)
    
    return config


def create_learning_configuration():
    """Create a configuration optimized for learning and adaptation"""
    config = pynlm.createDefaultConfig()
    
    # Larger network for rich learning dynamics
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.08)
    
    # Strong plasticity for rapid learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.003)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synapse_creation_rate", 0.0001)
    
    # Rich neuromodulation for exploration-exploitation
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.acetylcholine.enable", True)  # Additional neuromodulator
    
    # Active development for developmental learning
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.5)
    config.set("development.critical_period_end", 90.0)
    config.set("development.maturation_end", 450.0)
    
    # Full sensory interaction
    config.set("world.enable", True)
    config.set("world.vision_width", 12)
    config.set("world.vision_height", 12)
    config.set("world.enable_vision", True)
    config.set("world.enable_touch", True)
    config.set("world.enable_internal", True)
    
    # Enhanced memory for learning
    config.set("memory.working_memory.enable", True)
    config.set("memory.episodic_memory.enable", True)
    config.set("memory.associative_memory.enable", True)
    
    # Advanced prediction for learning
    config.set("prediction.enable", True)
    config.set("prediction.temporal_horizon", 200)
    config.set("prediction.action_consequence_prediction", True)
    
    return config


def create_baseline_configuration():
    """Create a minimal baseline configuration for testing"""
    config = pynlm.createDefaultConfig()
    
    # Small, fast network for baseline testing
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.03)
    
    # No plasticity for clean baseline
    config.set("plasticity.stdp.enable", False)
    config.set("plasticity.hebbian.enable", False)
    config.set("plasticity.structural.enable", False)
    
    # Minimal neuromodulation
    config.set("neuromod.curiosity.enable", False)
    config.set("neuromod.novelty.enable", False)
    config.set("neuromod.prediction_error.enable", False)
    
    # No development
    config.set("development.enable", False)
    
    # Simple world
    config.set("world.enable", False)  # No world interaction
    
    return config


# Global configuration presets
PRESETS = {
    'research': create_research_configuration,
    'production': create_production_configuration,
    'learning': create_learning_configuration,
    'baseline': create_baseline_configuration,
}


def get_preset(preset_name: str) -> Optional[pynlm.Config]:
    """Get a predefined configuration preset"""
    if preset_name in PRESETS:
        return PRESETS[preset_name]()
    else:
        print(f"Unknown preset: {preset_name}")
        return None


def list_presets() -> List[str]:
    """List available configuration presets"""
    return list(PRESETS.keys())
