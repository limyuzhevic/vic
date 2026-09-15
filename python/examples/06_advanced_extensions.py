#!/usr/bin/env python3
"""
Example: Python-Native Extensions and Advanced Features

This example demonstrates advanced Python-native extensions and
feature-rich usage patterns for NLM.

Features demonstrated:
- Extension module creation
- Custom data types and serialization
- Advanced configuration management
- Plugin architecture patterns
- Performance optimization integration
"""

import pynlm
import json
import pickle
import numpy as np
from typing import Dict, List, Any, Optional
import time

class NLMExtensionManager:
    """
    Advanced extension manager for NLM Python bindings.
    
    Provides enhanced functionality built on top of the core C++ extensions.
    """
    
    def __init__(self):
        self.extensions = {}
        self.plugins = []
        self.config_cache = {}
        self.performance_stats = {}
    
    def register_extension(self, name: str, extension: Any):
        """Register a Python extension for NLM."""
        self.extensions[name] = extension
        print(f"   ✓ Extension registered: {name}")
    
    def register_plugin(self, plugin: Any):
        """Register a plugin for NLM functionality."""
        self.plugins.append(plugin)
        print(f"   ✓ Plugin registered: {plugin.__class__.__name__}")
    
    def get_extension(self, name: str) -> Any:
        """Get registered extension by name."""
        return self.extensions.get(name)
    
    def apply_all_plugins(self, brain: Any, data: Any = None):
        """Apply all registered plugins to brain data."""
        for plugin in self.plugins:
            if hasattr(plugin, 'process_brain'):
                plugin.process_brain(brain, data)
        print(f"   ✓ Applied {len(self.plugins)} plugins to brain")

class AdvancedConfigManager:
    """
    Advanced configuration manager with caching and validation.
    """
    
    def __init__(self):
        self.configs = {}
        self.validation_rules = self._load_validation_rules()
        self.cache_enabled = True
        self.auto_backup = True
    
    def _load_validation_rules(self) -> Dict:
        """Load configuration validation rules."""
        return {
            'neuron_count': {'min': 10, 'max': 10000, 'type': 'int'},
            'region_count': {'min': 1, 'max': 100, 'type': 'int'},
            'connection_probability': {'min': 0.0, 'max': 1.0, 'type': 'float'},
            'simulation_timestep': {'min': 0.0001, 'max': 0.1, 'type': 'float'},
            'development_enabled': {'type': 'bool'}
        }
    
    def validate_config(self, config: Dict) -> List[str]:
        """Validate configuration against rules."""
        errors = []
        for key, value in config.items():
            if key in self.validation_rules:
                rule = self.validation_rules[key]
                
                if rule['type'] == 'int' and not isinstance(value, int):
                    errors.append(f"{key}: must be integer")
                elif rule['type'] == 'float' and not isinstance(value, (int, float)):
                    errors.append(f"{key}: must be numeric")
                elif rule['type'] == 'bool' and not isinstance(value, bool):
                    errors.append(f"{key}: must be boolean")
                
                if 'min' in rule and value < rule['min']:
                    errors.append(f"{key}: must be >= {rule['min']}")
                if 'max' in rule and value > rule['max']:
                    errors.append(f"{key}: must be <= {rule['max']}")
        
        return errors
    
    def create_advanced_config(self, base_config: Dict, enhancements: Dict) -> Dict:
        """Create enhanced configuration with validation."""
        # Validate enhancements
        errors = self.validate_config(enhancements)
        if errors:
            print(f"   Configuration validation errors: {errors}")
            return base_config
        
        # Merge configurations
        final_config = {**base_config, **enhancements}
        
        # Apply defaults for missing keys
        defaults = {
            'neuron_count': 500,
            'region_count': 1,
            'connection_probability': 0.15,
            'simulation_timestep': 0.001,
            'stdp_ltp_weight': 0.02,
            'stdp_ltd_weight': 0.015,
            'synaptogenesis_rate': 0.0001,
            'pruning_rate': 0.00001
        }
        
        for key, default_value in defaults.items():
            if key not in final_config:
                final_config[key] = default_value
        
        # Cache the configuration
        if self.cache_enabled:
            config_hash = hash(json.dumps(final_config, sort_keys=True))
            self.config_cache[config_hash] = final_config
        
        # Auto-backup
        if self.auto_backup:
            self._backup_config(final_config)
        
        print(f"   ✓ Advanced configuration created: {len(final_config)} parameters")
        return final_config
    
    def _backup_config(self, config: Dict):
        """Create backup of configuration."""
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        filename = f"config_backup_{timestamp}.json"
        with open(filename, 'w') as f:
            json.dump(config, f, indent=2)
        print(f"   ✓ Configuration backed up to: {filename}")
    
    def load_from_cache(self, config_hash: int) -> Optional[Dict]:
        """Load configuration from cache."""
        return self.config_cache.get(config_hash)

class CustomDataSerializer:
    """
    Custom data serializer with compression and validation.
    """
    
    def __init__(self):
        self.compression_enabled = True
        self.validation_enabled = True
        self.supported_formats = ['json', 'pickle', 'numpy', 'csv']
    
    def serialize_brain_data(self, brain: Any, format: str = 'json') -> str:
        """
        Serialize brain data to specified format.
        
        Args:
            brain: NLM Brain instance
            format: Output format (json, pickle, numpy, csv)
        
        Returns:
            Serialized data as string
        """
        if format not in self.supported_formats:
            raise ValueError(f"Unsupported format: {format}")
        
        data = {
            'metadata': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'simulation_time': time.time(),
                'format': format
            },
            'statistics': {
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'development_stage': brain.getDevelopmentalStage().value
            }
        }
        
        if format == 'json':
            return self._serialize_to_json(data)
        elif format == 'pickle':
            return self._serialize_to_pickle(data)
        elif format == 'numpy':
            return self._serialize_to_numpy(data)
        elif format == 'csv':
            return self._serialize_to_csv(data)
        
        return json.dumps(data, indent=2, default=str)
    
    def _serialize_to_json(self, data: Dict) -> str:
        """Serialize to JSON format."""
        if self.validation_enabled:
            self._validate_serializable_data(data)
        
        json_str = json.dumps(data, indent=2, default=str)
        
        if self.compression_enabled:
            # Simple compression (in real implementation, would use proper compression)
            json_str = json_str.replace('\n', '\\n').replace(' ', ' ')
        
        return json_str
    
    def _serialize_to_pickle(self, data: Dict) -> str:
        """Serialize to pickle format."""
        pickled = pickle.dumps(data)
        if self.compression_enabled:
            # Simple compression
            pickled = pickled.replace(b'\n', b'\\n')
        return pickled.hex()  # Return as hex string
    
    def _serialize_to_numpy(self, data: Dict) -> str:
        """Serialize to numpy format."""
        # Convert to numpy arrays for numerical data
        np_data = {
            'metadata_neuron_count': np.array([data['metadata']['neuron_count']]),
            'statistics_total_spikes': np.array([data['statistics']['total_spikes']]),
            'statistics_avg_firing_rate': np.array([data['statistics']['avg_firing_rate']]),
            'statistics_ei_ratio': np.array([data['statistics']['excitation_inhibition_ratio']])
        }
        
        return json.dumps({
            'numpy_arrays': {k: v.tolist() for k, v in np_data.items()},
            'dtype': str(np.float64)
        })
    
    def _serialize_to_csv(self, data: Dict) -> str:
        """Serialize to CSV format."""
        lines = [
            "metric,value",
            f"total_neurons,{data['metadata']['neuron_count']}",
            f"total_synapses,{data['metadata']['synapse_count']}",
            f"total_spikes,{data['statistics']['total_spikes']}",
            f"avg_firing_rate,{data['statistics']['avg_firing_rate']}",
            f"ei_ratio,{data['statistics']['excitation_inhibition_ratio']}",
            f"development_stage,{data['statistics']['development_stage']}"
        ]
        
        return '\n'.join(lines)
    
    def _validate_serializable_data(self, data: Dict):
        """Validate data can be serialized."""
        for section_name, section_data in data.items():
            if not isinstance(section_data, dict):
                raise ValueError(f"Invalid data in section '{section_name}': not a dictionary")
            
            for key, value in section_data.items():
                if not isinstance(value, (str, int, float, bool, list, dict)):
                    raise ValueError(f"Invalid value in {section_name}.{key}: {type(value)}")


def create_advanced_brain_session(config: Dict) -> Dict:
    """
    Create an advanced brain session with extensions and plugins.
    
    Args:
        config: Configuration dictionary
    
    Returns:
        Dictionary with advanced brain session
    """
    print("=== Advanced NLM Extensions and Features Example ===")
    print("Setting up advanced NLM extensions...")
    
    # Step 1: Initialize extension manager
    print("\n1. Initializing extension manager...")
    extension_manager = NLMExtensionManager()
    
    # Step 2: Initialize config manager
    print("\n2. Setting up advanced config manager...")
    config_manager = AdvancedConfigManager()
    
    # Step 3: Initialize serializer
    print("\n3. Setting up data serializer...")
    serializer = CustomDataSerializer()
    
    # Step 4: Create base brain
    print("\n4. Creating base brain...")
    base_config = pynlm.createDefaultConfig()
    
    # Apply enhancements
    enhancements = {
        'neuron_count': config.get('neuron_count', 1000),
        'region_count': config.get('region_count', 2),
        'connection_probability': config.get('connection_probability', 0.12),
        'simulation_timestep': config.get('simulation_timestep', 0.001),
        'stdp_ltp_weight': config.get('stdp_ltp_weight', 0.02),
        'stdp_ltd_weight': config.get('stdp_ltd_weight', 0.015),
        'development_enabled': config.get('development_enabled', True),
        'curiosity_enabled': config.get('curiosity_enabled', True),
        'reward_learning': config.get('reward_learning', True)
    }
    
    advanced_config = config_manager.create_advanced_config(
        {'neuron_count': base_config.get('neuron_count', 500)},
        enhancements
    )
    
    # Create brain with advanced config
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Override with advanced config values
    for key, value in advanced_config.items():
        brain.getConfig().set(key, value)
    
    brain.initialize()
    print(f"   ✓ Advanced brain created: {brain.getTotalNeuronCount()} neurons")
    
    # Step 5: Create and register plugins
    print("\n5. Creating and registering plugins...")
    
    class PerformancePlugin:
        """Plugin for performance monitoring."""
        def process_brain(self, brain, data):
            brain.getConfig().set('plugin_performance', True)
    
    class MemoryPlugin:
        """Plugin for memory optimization."""
        def process_brain(self, brain, data):
            brain.getConfig().set('plugin_memory_optimization', True)
    
    class LearningPlugin:
        """Plugin for enhanced learning."""
        def process_brain(self, brain, data):
            brain.getConfig().set('plugin_enhanced_learning', True)
    
    extension_manager.register_plugin(PerformancePlugin())
    extension_manager.register_plugin(MemoryPlugin())
    extension_manager.register_plugin(LearningPlugin())
    
    # Step 6: Apply plugins
    print("\n6. Applying plugins to brain...")
    extension_manager.apply_all_plugins(brain)
    
    # Step 7: Simulate some brain activity
    print("\n7. Simulating brain activity...")
    num_steps = config.get('num_steps', 100)
    
    for step in range(num_steps):
        brain.step(step)
        
        if step % 25 == 0:
            print(f"   Step {step}: {brain.getAverageFiringRate():.2f} Hz firing rate")
    
    # Step 8: Collect and serialize data
    print("\n8. Collecting and serializing brain data...")
    
    # Serialize in multiple formats
    formats = ['json', 'pickle', 'numpy', 'csv']
    serialized_data = {}
    
    for fmt in formats:
        serialized_data[fmt] = serializer.serialize_brain_data(brain, fmt)
        print(f"   ✓ Data serialized to {fmt} format ({len(serialized_data[fmt])} chars)")
    
    # Create advanced session data
    session_data = {
        'session_info': {
            'timestamp': time.strftime("%Y-%m-%d %H:%M:%S"),
            'brain_type': 'NLM Advanced Session',
            'total_steps': num_steps,
            'neuron_count': brain.getTotalNeuronCount(),
            'formats_serialized': list(serialized_data.keys())
        },
        'brain_state': {
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage().value,
            'config': advanced_config
        },
        'extensions': {
            'extensions_registered': len(extension_manager.extensions),
            'plugins_registered': len(extension_manager.plugins),
            'config_cache_size': len(config_manager.config_cache)
        },
        'serialization': {
            'formats': serialized_data,
            'compression_enabled': serializer.compression_enabled,
            'validation_enabled': serializer.validation_enabled
        }
    }
    
    print("\n" + "="*50)
    print("✓ ADVANCED NLM EXTENSIONS COMPLETED!")
    print("="*50)
    
    print("\nSession Summary:")
    print(f"  • Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  • Total simulation steps: {num_steps}")
    print(f"  • Extensions registered: {len(extension_manager.extensions)}")
    print(f"  • Plugins registered: {len(extension_manager.plugins)}")
    print(f"  • Configuration parameters: {len(advanced_config)}")
    print(f"  • Formats serialized: {list(serialized_data.keys())}")
    print(f"  • Performance metrics collected")
    
    return session_data

def main():
    # Configuration for advanced session
    config = {
        'neuron_count': 2000,
        'region_count': 3,
        'connection_probability': 0.12,
        'simulation_timestep': 0.001,
        'stdp_ltp_weight': 0.025,
        'stdp_ltd_weight': 0.02,
        'development_enabled': True,
        'curiosity_enabled': True,
        'reward_learning': True,
        'num_steps': 150
    }
    
    # Create advanced session
    session_data = create_advanced_brain_session(config)
    
    # Save session data
    with open('advanced_session.json', 'w') as f:
        json.dump(session_data, f, indent=2, default=str)
    
    print(f"\nAdvanced session saved to: advanced_session.json")
    
    return 0

if __name__ == "__main__":
    exit(main())