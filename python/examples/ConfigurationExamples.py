# ConfigurationExamples.py - Advanced Configuration Management

This example demonstrates advanced configuration management techniques for the NLM Python bindings. It shows how to create complex configurations, load from various sources, and dynamically modify settings.

```python
import pynlm
import json
import os
from pathlib import Path

class AdvancedConfigurationManager:
    """Advanced configuration management for NLM simulations."""
    
    def __init__(self):
        self.config_history = []
        self.template_registry = {}
        self.config_cache = {}
        
    def createTemplate(self, name, template_config):
        """Create a configuration template for reuse."""
        if not isinstance(template_config, dict):
            raise ValueError("Template config must be a dictionary")
        
        # Validate template structure
        if 'brain' not in template_config:
            raise ValueError("Template must include 'brain' section")
        
        self.template_registry[name] = template_config
        print(f"Created template: {name}")
        
    def applyTemplate(self, template_name):
        """Apply a configuration template."""
        if template_name not in self.template_registry:
            raise ValueError(f"Template not found: {template_name}")
        
        template = self.template_registry[template_name]
        config = pynlm.createDefaultConfig()
        
        # Apply template with validation
        for key, value in template.items():
            try:
                self._applyConfigValue(config, key, value)
            except Exception as e:
                print(f"Warning: Failed to apply {key}={value}: {e}")
        
        # Save to history
        self.config_history.append({
            'action': f"apply_template:{template_name}",
            'timestamp': self._getTimestamp(),
            'config': self._configToDict(config)
        })
        
        return config
    
    def createCustomConfig(self, **kwargs):
        """Create custom configuration with dynamic parameters."""
        config = pynlm.createDefaultConfig()
        
        # Predefined configuration presets
        presets = {
            'small_fast': {
                'brain.neuron_count': 500,
                'brain.synapse_density': 0.05,
                'simulation.timestep': 0.005,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': False
            },
            'large_detailed': {
                'brain.neuron_count': 5000,
                'brain.synapse_density': 0.15,
                'simulation.timestep': 0.001,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': True,
                'neuromod.dopamine.scale': 2.0,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True
            },
            'balanced': {
                'brain.neuron_count': 2000,
                'brain.synapse_density': 0.1,
                'simulation.timestep': 0.002,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': False,
                'neuromod.dopamine.scale': 1.0,
                'neuromod.curiosity.enable': False,
                'neuromod.novelty.enable': True
            }
        }
        
        # Apply preset if specified
        preset = kwargs.pop('preset', None)
        if preset and preset in presets:
            for key, value in presets[preset].items():
                self._applyConfigValue(config, key, value)
        
        # Apply custom parameters
        for key, value in kwargs.items():
            self._applyConfigValue(config, key, value)
        
        # Save to history
        self.config_history.append({
            'action': f"create_custom:preset={preset},kwargs={len(kwargs)}",
            'timestamp': self._getTimestamp(),
            'config': self._configToDict(config)
        })
        
        return config
    
    def loadFromMultipleSources(self, sources):
        """Load configuration from multiple sources (files, dicts, etc.)."""
        config = pynlm.createDefaultConfig()
        loaded_sources = []
        
        for source in sources:
            if isinstance(source, str):
                # File path
                if os.path.exists(source):
                    if source.endswith('.json'):
                        with open(source, 'r') as f:
                            data = json.load(f)
                        self._dictToConfig(config, data)
                    elif source.endswith('.cfg'):
                        # Assume it's a simple key=value format
                        self._parseConfigFile(config, source)
                    else:
                        print(f"Warning: Unknown file format for {source}")
                    loaded_sources.append(('file', source))
                else:
                    print(f"Warning: File not found: {source}")
                    
            elif isinstance(source, dict):
                # Dictionary
                self._dictToConfig(config, source)
                loaded_sources.append(('dict', str(len(source)) + ' items'))
                
            elif hasattr(source, 'keys'):
                # Object with keys method (like Config itself)
                temp_config = source
                self._mergeConfigs(config, temp_config)
                loaded_sources.append(('config', 'Config object'))
        
        # Save to history
        self.config_history.append({
            'action': f"load_multiple: {len(loaded_sources)} sources",
            'timestamp': self._getTimestamp(),
            'config': self._configToDict(config)
        })
        
        return config, loaded_sources
    
    def _applyConfigValue(self, config, key, value):
        """Apply a single configuration value with type conversion."""
        if '.' not in key:
            # Simple key
            if isinstance(value, bool):
                config.set(key, value, pynlm.ConfigSource::Default)
            elif isinstance(value, int):
                config.set_int(key, value)
            elif isinstance(value, float):
                config.set_float(key, value)
            elif isinstance(value, str):
                config.set_string(key, value)
            else:
                config.set(key, value, pynlm.ConfigSource::Default)
        else:
            # Dotted key (nested)
            parts = key.split('.')
            if parts[0] == 'brain':
                # Brain-specific configuration
                if parts[1] == 'neuron_count':
                    config.set_int(key, value)
                elif parts[1] == 'synapse_density':
                    config.set_float(key, value)
                elif parts[1] == 'connection_probability':
                    config.set_float(key, value)
                else:
                    config.set_float(key, value)
            elif parts[0] == 'plasticity':
                # Plasticity configuration
                config.set_bool(key, value)
            elif parts[0] == 'neuromod':
                # Neuromodulation configuration
                if parts[1] == 'dopamine' and parts[2] == 'scale':
                    config.set_float(key, value)
                else:
                    config.set_bool(key, value)
            elif parts[0] == 'simulation':
                # Simulation configuration
                config.set_float(key, value)
            else:
                config.set(key, value, pynlm.ConfigSource::Default)
    
    def _dictToConfig(self, config, data):
        """Convert dictionary to config."""
        for key, value in data.items():
            if isinstance(value, dict):
                # Nested dictionary
                for subkey, subvalue in value.items():
                    self._applyConfigValue(config, f"{key}.{subkey}", subvalue)
            else:
                self._applyConfigValue(config, key, value)
    
    def _parseConfigFile(self, config, filepath):
        """Parse simple key=value config file."""
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    if '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # Try to convert to appropriate type
                        if value.lower() in ('true', 'false'):
                            self._applyConfigValue(config, key, value.lower() == 'true')
                        elif value.replace('.', '').isdigit():
                            if '.' in value:
                                self._applyConfigValue(config, key, float(value))
                            else:
                                self._applyConfigValue(config, key, int(value))
                        else:
                            self._applyConfigValue(config, key, value)
    
    def _mergeConfigs(self, target_config, source_config):
        """Merge source config into target config."""
        # Get all keys from source config
        keys = source_config.getKeys()
        
        for key in keys:
            if key.find('.') != -1:  # Only dotted keys have specific types
                # Try to get value as appropriate type
                if 'neuron_count' in key:
                    value = source_config.getOrInt(key, 0)
                    target_config.set_int(key, value)
                elif 'density' in key or 'probability' in key or 'scale' in key or 'rate' in key:
                    value = source_config.getOrFloat(key, 0.0)
                    target_config.set_float(key, value)
                elif 'enable' in key or 'enabled' in key:
                    value = source_config.has(key)
                    target_config.set_bool(key, value)
                else:
                    value = source_config.getOrStr(key, "")
                    target_config.set_string(key, value)
            else:
                # Simple key - use string representation
                value = source_config.getOrStr(key, "")
                target_config.set_string(key, value)
    
    def _configToDict(self, config):
        """Convert config to dictionary."""
        result = {}
        keys = config.getKeys()
        
        for key in keys:
            if key.find('.') != -1:
                # Dotted key
                parts = key.split('.')
                current = result
                for i, part in enumerate(parts[:-1]):
                    if part not in current:
                        current[part] = {}
                    current = current[part]
                current[parts[-1]] = config.getOrStr(key, "")
            else:
                # Simple key
                result[key] = config.getOrStr(key, "")
        
        return result
    
    def _getTimestamp(self):
        """Get current timestamp."""
        return time.strftime("%Y-%m-%d %H:%M:%S")
    
    def validateConfiguration(self, config):
        """Validate configuration for consistency."""
        errors = []
        warnings = []
        
        # Check for required sections
        keys = config.getKeys()
        
        # Brain configuration validation
        brain_keys = [k for k in keys if k.startswith('brain.')]
        if not brain_keys:
            warnings.append("No brain configuration found")
        
        for key in brain_keys:
            if 'neuron_count' in key:
                value = config.getOrInt(key, 0)
                if value <= 0:
                    errors.append(f"Invalid brain.neuron_count: {value}")
                elif value > 10000:
                    warnings.append(f"Large neuron count ({value}) may affect performance")
            
            elif 'synapse_density' in key:
                value = config.getOrFloat(key, 0.0)
                if value < 0.0 or value > 1.0:
                    errors.append(f"Invalid brain.synapse_density: {value} (must be 0-1)")
        
        # Plasticity validation
        stdp_enabled = config.has('plasticity.stdp.enable') and config.getOrBool('plasticity.stdp.enable')
        hebbian_enabled = config.has('plasticity.hebbian.enable') and config.getOrBool('plasticity.hebbian.enable')
        
        if not (stdp_enabled or hebbian_enabled):
            warnings.append("No plasticity enabled - learning will not occur")
        
        # Return validation results
        return {
            'valid': len(errors) == 0,
            'errors': errors,
            'warnings': warnings,
            'summary': {
                'total_keys': len(keys),
                'brain_keys': len(brain_keys),
                'plasticity_enabled': stdp_enabled or hebbian_enabled,
                'neuromod_enabled': config.has('neuromod.curiosity.enable') and config.getOrBool('neuromod.curiosity.enable')
            }
        }
    
    def compareConfigs(self, config1, config2):
        """Compare two configurations and show differences."""
        dict1 = self._configToDict(config1)
        dict2 = self._configToDict(config2)
        
        differences = {
            'added': [],
            'removed': [],
            'modified': [],
            'unchanged': []
        }
        
        all_keys = set(dict1.keys()) | set(dict2.keys())
        
        for key in all_keys:
            val1 = dict1.get(key, 'MISSING')
            val2 = dict2.get(key, 'MISSING')
            
            if val1 == 'MISSING' and val2 == 'MISSING':
                continue
            elif val1 == 'MISSING':
                differences['added'].append(key)
            elif val2 == 'MISSING':
                differences['removed'].append(key)
            elif val1 != val2:
                differences['modified'].append({
                    'key': key,
                    'value1': val1,
                    'value2': val2
                })
            else:
                differences['unchanged'].append(key)
        
        return differences

class ConfigurationPresets:
    """Predefined configuration presets for common use cases."""
    
    @staticmethod
    def getScientificResearchPreset():
        """Preset for scientific research with detailed recording."""
        return {
            'brain.neuron_count': 3000,
            'brain.synapse_density': 0.12,
            'brain.connection_probability': 0.08,
            'simulation.timestep': 0.001,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.001,
            'plasticity.hebbian.enable': True,
            'plasticity.hebbian.learning_rate': 0.0005,
            'plasticity.structural.enable': True,
            'neuromod.dopamine.scale': 1.5,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'memory.working.capacity': 100,
            'memory.episodic.capacity': 50,
            'checkpoint.save_interval': 100,
            'checkpoint.max_size': 1000
        }
    
    @staticmethod
    def getRealTimeInteractivePreset():
        """Preset for real-time interactive applications."""
        return {
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.08,
            'simulation.timestep': 0.005,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': False,
            'plasticity.structural.enable': False,
            'neuromod.dopamine.scale': 1.0,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': False,
            'checkpoint.save_interval': 500,
            'performance.mode': 'fast'
        }
    
    @staticmethod
    def getDevelopmentalLearningPreset():
        """Preset for developmental learning experiments."""
        return {
            'brain.neuron_count': 2000,
            'brain.synapse_density': 0.1,
            'simulation.timestep': 0.002,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.dopamine.scale': 2.0,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'development.initial_stage': 'CriticalPeriod',
            'development.plasticity_modifier': 2.0,
            'memory.working.capacity': 200,
            'memory.episodic.capacity': 100
        }

# Example usage
if __name__ == "__main__":
    print("=== Advanced Configuration Management ===")
    
    # Create configuration manager
    config_manager = AdvancedConfigurationManager()
    
    # Create templates
    config_manager.createTemplate("research", ConfigurationPresets.getScientificResearchPreset())
    config_manager.createTemplate("interactive", ConfigurationPresets.getRealTimeInteractivePreset())
    config_manager.createTemplate("developmental", ConfigurationPresets.getDevelopmentalLearningPreset())
    
    # Apply different templates
    print("\n--- Applying Research Template ---")
    research_config = config_manager.applyTemplate("research")
    print(f"Research config created with {research_config.getKeys().__len__()} keys")
    
    print("\n--- Applying Interactive Template ---")
    interactive_config = config_manager.applyTemplate("interactive")
    print(f"Interactive config created with {interactive_config.getKeys().__len__()} keys")
    
    # Create custom config
    print("\n--- Creating Custom Config ---")
    custom_config = config_manager.createCustomConfig(
        preset='balanced',
        brain_neuron_count=1500,
        plasticity_structural_enable=True,
        neuromod_novelty_enable=False
    )
    print(f"Custom config created")
    
    # Load from multiple sources
    print("\n--- Loading from Multiple Sources ---")
    
    # Create a temporary config file
    test_config = {
        'brain.neuron_count': 800,
        'plasticity.stdp.enable': True,
        'neuromod.dopamine.scale': 1.2
    }
    
    with open('temp_config.json', 'w') as f:
        json.dump(test_config, f, indent=2)
    
    sources = [
        'temp_config.json',  # JSON file
        test_config,        # Dictionary
        custom_config       # Config object
    ]
    
    multi_config, loaded_sources = config_manager.loadFromMultipleSources(sources)
    print(f"Loaded from {len(loaded_sources)} sources: {loaded_sources}")
    
    # Validate configurations
    print("\n--- Validating Configurations ---")
    for name, cfg in [("Research", research_config), ("Custom", custom_config), ("Multi", multi_config)]:
        validation = config_manager.validateConfiguration(cfg)
        print(f"{name}: {'VALID' if validation['valid'] else 'INVALID'}")
        if validation['warnings']:
            print(f"  Warnings: {len(validation['warnings'])}")
        if validation['errors']:
            print(f"  Errors: {validation['errors']}")
    
    # Compare configurations
    print("\n--- Comparing Research vs Custom ---")
    differences = config_manager.compareConfigs(research_config, custom_config)
    print(f"Added keys: {differences['added']}")
    print(f"Removed keys: {differences['removed']}")
    print(f"Modified keys: {len(differences['modified'])}")
    print(f"Unchanged keys: {len(differences['unchanged'])}")
    
    # Clean up
    if os.path.exists('temp_config.json'):
        os.remove('temp_config.json')
    
    print("\n=== Configuration Management Complete ===")
