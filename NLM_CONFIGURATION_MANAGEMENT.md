# NLM Configuration Management Improvements

## Executive Summary

This document details comprehensive improvements to the NLM configuration management system, transforming it from a simple key=value parser into a sophisticated, multi-format configuration system with advanced features.

## 1. Configuration System Architecture

### 1.1 Configuration Format Support

The enhanced configuration system supports multiple formats with unified API:

```python
# Configuration format hierarchy
config = pynlm.createAdvancedConfig()

# Automatic format detection
config.loadFromFile("config.json")      # JSON format
config.loadFromFile("config.yaml")      # YAML format  
config.loadFromFile("config.cfg")       # Key=value format (backward compatibility)
config.loadFromFile("config.toml")      # TOML format (future)
config.loadFromFile("config.ini")       # INI format (future)
```

### 1.2 Configuration Schema Support

```python
# Advanced schema validation
from nlm.config.schema import ConfigSchema

class BrainSchema(ConfigSchema):
    """Configuration schema for brain parameters"""
    schema = {
        "brain.neuron_count": {"type": "int", "min": 1, "max": 100000},
        "brain.synapse_density": {"type": "float", "min": 0.0, "max": 1.0},
        "brain.v_thresh": {"type": "float", "min": -100.0, "max": -10.0},
        "brain.v_rest": {"type": "float", "min": -100.0, "max": 0.0},
        "brain.tau_mem": {"type": "float", "min": 1.0, "max": 1000.0}
    }

class PlasticitySchema(ConfigSchema):
    """Configuration schema for plasticity parameters"""
    schema = {
        "plasticity.stdp.enable": {"type": "bool"},
        "plasticity.stdp.learning_rate": {"type": "float", "min": 0.0, "max": 1.0},
        "plasticity.stdp.tau_plus": {"type": "float", "min": 1.0, "max": 1000.0},
        "plasticity.stdp.tau_minus": {"type": "float", "min": 1.0, "max": 1000.0}
    }
```

### 1.3 Configuration Class Hierarchy

```python
# Configuration class hierarchy
class NLMConfig:
    """Base configuration class"""
    
    class ConfigSection:
        """Configuration section for organizing parameters"""
        
        class Brain(ConfigSection):
            NEURON_COUNT = "brain.neuron_count"
            SYNAPSE_DENSITY = "brain.synapse_density"
            V_THRESH = "brain.v_thresh"
            V_REST = "brain.v_rest"
            V_RESET = "brain.v_reset"
            TAU_MEM = "brain.tau_mem"
            TAU_REF = "brain.tau_ref"
            
        class Plasticity(ConfigSection):
            STDP_ENABLE = "plasticity.stdp.enable"
            STDP_LEARNING_RATE = "plasticity.stdp.learning_rate"
            STDP_TAU_PLUS = "plasticity.stdp.tau_plus"
            STDP_TAU_MINUS = "plasticity.stdp.tau_minus"
            HEBBIAN_ENABLE = "plasticity.hebbian.enable"
            HEBBIAN_LEARNING_RATE = "plasticity.hebbian.learning_rate"
            STRUCTURAL_ENABLE = "plasticity.structural.enable"
            
        class Neuromodulation(ConfigSection):
            DOPAMINE_SCALE = "neuromod.dopamine.scale"
            CURIOUSITY_ENABLE = "neuromod.curiosity.enable"
            NOVELTY_ENABLE = "neuromod.novelty.enable"
            
        class Memory(ConfigSection):
            WORKING_MEMORY_CAPACITY = "memory.working_memory.capacity"
            EPISODIC_MEMORY_DURATION = "memory.episodic.duration"
            ASSOCIATIVE_THRESHOLD = "memory.associative.threshold"
            
        class Development(ConfigSection):
            STAGE = "development.stage"
            PLASTICITY_RATE = "development.plasticity_rate"
            MATURATION_SPEED = "development.maturation_speed"
            
        class Prediction(ConfigSection):
            PREDICTION_ACCURACY = "prediction.accuracy"
            ERROR_WEIGHT = "prediction.error_weight"
            CONFIDENCE_THRESHOLD = "prediction.confidence_threshold"
    
    # Convenience properties
    brain = ConfigSection.Brain
    plasticity = ConfigSection.Plasticity
    neuromodulation = ConfigSection.Neuromodulation
    memory = ConfigSection.Memory
    development = ConfigSection.Development
    prediction = ConfigSection.Prediction
```

## 2. Enhanced Configuration Loading

### 2.1 Automatic Format Detection

```python
class AdvancedConfigLoader:
    """Advanced configuration loader with format detection and validation"""
    
    @staticmethod
    def detect_file_format(filepath):
        """Automatically detect configuration file format"""
        if filepath.endswith('.json'):
            return 'json'
        elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
            return 'yaml'
        elif filepath.endswith('.toml'):
            return 'toml'
        elif filepath.endswith('.ini') or filepath.endswith('.cfg'):
            return 'key_value'
        else:
            # Content-based detection
            with open(filepath, 'r') as f:
                content = f.read(1024)
                
            # Try to parse as JSON
            try:
                json.loads(content)
                return 'json'
            except:
                pass
            
            # Try to parse as YAML (basic check)
            if '{' in content and '}' in content and ':' in content:
                return 'key_value'
            
            return 'unknown'
    
    @staticmethod
    def load_config_with_validation(filepath, schema=None):
        """Load configuration with automatic format detection and validation"""
        format = AdvancedConfigLoader.detect_file_format(filepath)
        
        if format == 'json':
            return AdvancedConfigLoader._load_json_config(filepath, schema)
        elif format == 'yaml':
            return AdvancedConfigLoader._load_yaml_config(filepath, schema)
        elif format == 'key_value':
            return AdvancedConfigLoader._load_key_value_config(filepath, schema)
        else:
            raise ValueError(f"Unsupported configuration format: {format}")
    
    @staticmethod
    def _load_json_config(filepath, schema):
        """Load JSON configuration"""
        import json
        
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        config = pynlm.createAdvancedConfig()
        
        # Convert flat JSON to nested structure
        for key, value in config_data.items():
            # Support both flat and nested JSON
            if isinstance(value, dict):
                for subkey, subvalue in value.items():
                    config.set(f"{key}.{subkey}", subvalue)
            else:
                config.set(key, value)
        
        if schema:
            config.validate(schema)
        
        return config
    
    @staticmethod
    def _load_yaml_config(filepath, schema):
        """Load YAML configuration"""
        try:
            import yaml
            
            with open(filepath, 'r') as f:
                config_data = yaml.safe_load(f)
            
            config = pynlm.createAdvancedConfig()
            
            # Convert YAML to configuration
            AdvancedConfigLoader._flatten_dict(config_data, config)
            
            if schema:
                config.validate(schema)
            
            return config
            
        except ImportError:
            raise ImportError("PyYAML is required for YAML configuration support")
    
    @staticmethod
    def _load_key_value_config(filepath, schema):
        """Load key=value configuration (backward compatibility)"""
        config = pynlm.createAdvancedConfig()
        
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    if '=' in line:
                        key, value = line.split('=', 1)
                        config.set(key.strip(), value.strip())
        
        if schema:
            config.validate(schema)
        
        return config
    
    @staticmethod
    def _flatten_dict(nested_dict, config):
        """Flatten nested dictionary to configuration keys"""
        for key, value in nested_dict.items():
            if isinstance(value, dict):
                # Recursively flatten nested dictionaries
                AdvancedConfigLoader._flatten_dict(value, config)
            else:
                # Convert to string if needed
                if isinstance(value, (bool, int, float)):
                    config.set(key, value)
                else:
                    config.set(key, str(value))
```

### 2.2 Configuration Validation

```python
class ConfigValidator:
    """Configuration validation with comprehensive checking"""
    
    @staticmethod
    def validate_config(config, schema=None):
        """Validate configuration against schema and rules"""
        errors = []
        
        # Basic validation
        errors.extend(ConfigValidator._validate_basic_types(config))
        
        # Schema validation
        if schema:
            errors.extend(ConfigValidator._validate_against_schema(config, schema))
        
        # Business rule validation
        errors.extend(ConfigValidator._validate_business_rules(config))
        
        # Inter-parameter validation
        errors.extend(ConfigValidator._validate_inter_parameter(config))
        
        return errors
    
    @staticmethod
    def _validate_basic_types(config):
        """Validate basic type constraints"""
        errors = []
        
        # Check neuron count constraints
        neuron_count = config.getOr("brain.neuron_count", 0)
        if neuron_count <= 0:
            errors.append("brain.neuron_count must be positive")
        elif neuron_count > 1000000:
            errors.append("brain.neuron_count cannot exceed 1,000,000")
        
        # Check synapse density
        synapse_density = config.getOr("brain.synapse_density", 0.0)
        if synapse_density < 0.0 or synapse_density > 1.0:
            errors.append("brain.synapse_density must be between 0.0 and 1.0")
        
        # Check voltage parameters
        v_thresh = config.getOr("brain.v_thresh", -50.0)
        v_rest = config.getOr("brain.v_rest", -70.0)
        v_reset = config.getOr("brain.v_reset", -75.0)
        
        if v_thresh >= v_rest:
            errors.append("brain.v_thresh must be less than brain.v_rest")
        
        if v_reset > v_thresh:
            errors.append("brain.v_reset cannot be greater than brain.v_thresh")
        
        # Check time constants
        tau_mem = config.getOr("brain.tau_mem", 20.0)
        tau_ref = config.getOr("brain.tau_ref", 2.0)
        
        if tau_mem <= tau_ref:
            errors.append("brain.tau_mem must be greater than brain.tau_ref")
        
        return errors
    
    @staticmethod
    def _validate_business_rules(config):
        """Validate business rules and constraints"""
        errors = []
        
        # Check plasticity consistency
        if config.getOr("plasticity.stdp.enable", True):
            stdp_learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
            if stdp_learning_rate <= 0.0:
                errors.append("plasticity.stdp.learning_rate must be positive when enabled")
        
        # Check neuromodulation consistency
        if config.getOr("neuromodulation.dopamine.scale", 1.0) < 0.0:
            errors.append("neuromodulation.dopamine.scale cannot be negative")
        
        # Check development stage consistency
        stage = config.getOr("development.stage", "adult")
        allowed_stages = ["initial", "critical_period", "maturation", "adult", "aging"]
        if stage not in allowed_stages:
            errors.append(f"development.stage must be one of: {', '.join(allowed_stages)}")
        
        return errors
    
    @staticmethod
    def _validate_inter_parameter(config):
        """Validate inter-parameter relationships"""
        errors = []
        
        # Check if plasticity is disabled when development is advanced
        development_stage = config.getOr("development.stage", "adult")
        plasticity_enabled = config.getOr("plasticity.stdp.enable", True)
        
        if development_stage == "aging" and plasticity_enabled:
            # In aging stage, plasticity should be reduced
            stdp_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
            if stdp_rate > 0.0001:
                errors.append(
                    "plasticity.stdp.learning_rate should be reduced in 'aging' stage. "
                    "Consider setting to 0.0001 or lower."
                )
        
        return errors
```

## 3. Configuration Management Utilities

### 3.1 Configuration Templates

```python
class ConfigTemplates:
    """Predefined configuration templates for common use cases"""
    
    @staticmethod
    def create_research_template(neurons=5000, plasticity="full"):
        """Create research configuration template"""
        config = pynlm.createAdvancedConfig()
        
        config.set("brain.neuron_count", neurons)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromodulation.dopamine.scale", 1.0)
        config.set("neuromodulation.curiosity.enable", True)
        config.set("neuromodulation.novelty.enable", True)
        
        if plasticity == "reduced":
            config.set("plasticity.stdp.learning_rate", 0.0001)
            config.set("plasticity.structural.enable", False)
        elif plasticity == "none":
            config.set("plasticity.stdp.enable", False)
            config.set("plasticity.hebbian.enable", False)
            config.set("plasticity.structural.enable", False)
        
        return config
    
    @staticmethod
    def create_production_template(neurons=1000, performance="optimized"):
        """Create production configuration template"""
        config = pynlm.createAdvancedConfig()
        
        config.set("brain.neuron_count", neurons)
        config.set("brain.synapse_density", 0.05)  # Lower density for performance
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0005)  # Slower learning
        config.set("plasticity.hebbian.enable", False)  # Disable for stability
        config.set("plasticity.structural.enable", False)  # Disable for performance
        config.set("neuromodulation.dopamine.scale", 0.5)  # Reduced modulation
        config.set("neuromodulation.curiosity.enable", False)  # Disable exploration
        config.set("neuromodulation.novelty.enable", False)  # Disable novelty
        
        if performance == "high":
            config.set("brain.tau_mem", 10.0)  # Faster dynamics
            config.set("brain.tau_ref", 1.0)  # Shorter refractory
        
        return config
    
    @staticmethod
    def create_development_template(stages="complete"):
        """Create development-focused configuration template"""
        config = pynlm.createAdvancedConfig()
        
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.002)  # Higher learning rate
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromodulation.dopamine.scale", 2.0)
        config.set("neuromodulation.curiosity.enable", True)
        config.set("neuromodulation.novelty.enable", True)
        
        if stages == "complete":
            config.set("development.stage", "critical_period")
            config.set("development.plasticity_rate", 2.0)  # Enhanced plasticity
            config.set("development.maturation_speed", 0.5)  # Slower maturation
        elif stages == "adult":
            config.set("development.stage", "adult")
            config.set("development.plasticity_rate", 1.0)
            config.set("development.maturation_speed", 1.0)
        
        return config
    
    @staticmethod
    def create_curiosity_template(neurons=3000, exploration="high"):
        """Create curiosity-driven exploration template"""
        config = pynlm.createAdvancedConfig()
        
        config.set("brain.neuron_count", neurons)
        config.set("brain.synapse_density", 0.2)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromodulation.dopamine.scale", 1.5)
        config.set("neuromodulation.curiosity.enable", True)
        config.set("neuromodulation.novelty.enable", True)
        
        if exploration == "high":
            config.set("neuromodulation.curiosity.threshold", 0.1)
            config.set("neuromodulation.curiosity.strength", 2.0)
        elif exploration == "medium":
            config.set("neuromodulation.curiosity.threshold", 0.3)
            config.set("neuromodulation.curiosity.strength", 1.0)
        elif exploration == "low":
            config.set("neuromodulation.curiosity.threshold", 0.5)
            config.set("neuromodulation.curiosity.strength", 0.5)
        
        return config
```

### 3.2 Configuration Comparison and Analysis

```python
class ConfigAnalyzer:
    """Configuration analysis and comparison tools"""
    
    @staticmethod
    def compare_configs(config1, config2, section=None):
        """Compare two configurations"""
        differences = {}
        
        # Get all keys from both configurations
        keys1 = config1.getKeys()
        keys2 = config2.getKeys()
        all_keys = set(keys1) | set(keys2)
        
        for key in all_keys:
            value1 = config1.get(key) if config1.has(key) else None
            value2 = config2.get(key) if config2.has(key) else None
            
            if value1 != value2:
                differences[key] = {
                    'config1_value': value1,
                    'config2_value': value2,
                    'changed': True
                }
        
        # Filter by section if specified
        if section:
            differences = {k: v for k, v in differences.items() if k.startswith(section + '.')}
        
        return differences
    
    @staticmethod
    def analyze_config(config):
        """Analyze configuration for potential issues"""
        analysis = {
            'total_parameters': len(config.getKeys()),
            'sections': {},
            'potential_issues': [],
            'recommendations': []
        }
        
        # Analyze by sections
        sections = ['brain', 'plasticity', 'neuromodulation', 'memory', 'development', 'prediction']
        
        for section in sections:
            section_keys = [k for k in config.getKeys() if k.startswith(section + '.')]
            analysis['sections'][section] = len(section_keys)
            
            # Check for empty sections
            if len(section_keys) == 0:
                analysis['potential_issues'].append(
                    f"Section '{section}' has no parameters configured"
                )
        
        # Check for potential issues
        if config.getOr("brain.neuron_count", 0) > 10000:
            analysis['potential_issues'].append(
                "Large neuron count (>10,000) may impact performance"
            )
        
        if config.getOr("brain.synapse_density", 0.0) > 0.5:
            analysis['potential_issues'].append(
                "High synapse density (>0.5) may impact performance"
            )
        
        # Add recommendations
        if config.getOr("plasticity.stdp.learning_rate", 0.0) == 0.0:
            analysis['recommendations'].append(
                "STDP plasticity is disabled. Consider enabling for better learning."
            )
        
        if config.getOr("neuromodulation.curiosity.enable", False) == False:
            analysis['recommendations'].append(
                "Curiosity-driven exploration is disabled. Consider enabling for better exploration."
            )
        
        return analysis
```

### 3.3 Configuration Migration Tools

```python
class ConfigMigration:
    """Configuration migration and upgrade tools"""
    
    @staticmethod
    def migrate_from_old_format(old_config_path, new_config_path=None):
        """Migrate from old key=value format to new format"""
        old_config = pynlm.createDefaultConfig()
        old_config.loadFromFile(old_config_path)
        
        new_config = pynlm.createAdvancedConfig()
        
        # Migrate all parameters
        for key in old_config.getKeys():
            new_config.set(key, old_config.get(key))
        
        # Save in new format
        if new_config_path:
            new_config.saveToFile(new_config_path)
        
        return new_config
    
    @staticmethod
    def migrate_from_json(old_json_path, new_config_path=None):
        """Migrate from JSON to advanced configuration"""
        import json
        
        with open(old_json_path, 'r') as f:
            json_data = json.load(f)
        
        new_config = pynlm.createAdvancedConfig()
        
        # Flatten JSON and migrate
        ConfigMigration._flatten_dict_to_config(json_data, new_config)
        
        # Save in new format
        if new_config_path:
            new_config.saveToFile(new_config_path)
        
        return new_config
    
    @staticmethod
    def _flatten_dict_to_config(nested_dict, config):
        """Flatten nested dictionary to configuration"""
        for key, value in nested_dict.items():
            if isinstance(value, dict):
                # Recursively flatten
                ConfigMigration._flatten_dict_to_config(value, config)
            else:
                config.set(key, value)
    
    @staticmethod
    def upgrade_config(old_version, new_version, config_path):
        """Upgrade configuration between versions"""
        config = pynlm.loadFromFile(config_path)
        
        # Version-specific migration logic
        if old_version == "1.0" and new_version == "2.0":
            # Migrate from 1.0 to 2.0
            ConfigMigration._migrate_1_to_2(config)
        
        # Save upgraded configuration
        config.saveToFile(config_path)
        
        return config
    
    @staticmethod
    def _migrate_1_to_2(config):
        """Migrate configuration from version 1.0 to 2.0"""
        # Update parameter names
        name_mapping = {
            "neuron_count": "brain.neuron_count",
            "synapse_density": "brain.synapse_density",
            "learning_rate": "plasticity.stdp.learning_rate",
            "enable_hebbian": "plasticity.hebbian.enable",
            "enable_structural": "plasticity.structural.enable",
            "dopamine_scale": "neuromod.dopamine.scale",
            "enable_curiosity": "neuromod.curiosity.enable",
            "enable_novelty": "neuromod.novelty.enable"
        }
        
        for old_name, new_name in name_mapping.items():
            if config.has(old_name):
                config.set(new_name, config.get(old_name))
                config.remove(old_name)  # Remove old parameter
        
        # Add new defaults
        config.set("plasticity.stdp.tau_plus", 20.0)
        config.set("plasticity.stdp.tau_minus", 20.0)
        config.set("development.stage", "adult")
```

## 4. Advanced Configuration Features

### 4.1 Configuration Profiles

```python
class ConfigProfiles:
    """Configuration profile management"""
    
    def __init__(self):
        self.profiles = {}
    
    def create_profile(self, name, config):
        """Create a named configuration profile"""
        self.profiles[name] = config
    
    def get_profile(self, name):
        """Get a configuration profile by name"""
        if name not in self.profiles:
            raise KeyError(f"Profile '{name}' not found")
        return self.profiles[name]
    
    def delete_profile(self, name):
        """Delete a configuration profile"""
        if name in self.profiles:
            del self.profiles[name]
    
    def list_profiles(self):
        """List all configuration profiles"""
        return list(self.profiles.keys())
    
    def export_profile(self, name, filepath):
        """Export a profile to file"""
        config = self.get_profile(name)
        config.saveToFile(filepath)
    
    def import_profile(self, name, filepath):
        """Import a profile from file"""
        config = pynlm.loadFromFile(filepath)
        self.create_profile(name, config)
        return config
```

### 4.2 Configuration Versioning

```python
class ConfigVersioning:
    """Configuration versioning and history tracking"""
    
    def __init__(self):
        self.version_history = {}
        self.current_version = 1
    
    def create_version(self, config, description=""):
        """Create a new version of configuration"""
        version = self.current_version
        self.version_history[version] = {
            'config': config,
            'description': description,
            'timestamp': time.time()
        }
        
        self.current_version += 1
        return version
    
    def get_version(self, version):
        """Get configuration by version"""
        if version not in self.version_history:
            raise KeyError(f"Version {version} not found")
        return self.version_history[version]['config'].copy()
    
    def restore_version(self, version):
        """Restore configuration to a specific version"""
        config = self.get_version(version)
        # Could update current config or return new config
        return config
    
    def compare_versions(self, version1, version2):
        """Compare two versions"""
        config1 = self.get_version(version1)
        config2 = self.get_version(version2)
        
        return ConfigAnalyzer.compare_configs(config1, config2)
    
    def get_version_history(self):
        """Get version history"""
        return {
            'current_version': self.current_version,
            'total_versions': len(self.version_history),
            'versions': {
                str(v): {
                    'description': info['description'],
                    'timestamp': info['timestamp']
                }
                for v, info in self.version_history.items()
            }
        }
```

### 4.3 Configuration Macros

```python
class ConfigMacros:
    """Configuration macro system for parameterized configurations"""
    
    def __init__(self):
        self.macros = {}
    
    def define_macro(self, name, template):
        """Define a new configuration macro"""
        self.macros[name] = template
    
    def expand_macro(self, name, parameters=None):
        """Expand a configuration macro with parameters"""
        if name not in self.macros:
            raise KeyError(f"Macro '{name}' not found")
        
        template = self.macros[name]
        
        if parameters is None:
            parameters = {}
        
        config = pynlm.createAdvancedConfig()
        
        # Expand template
        ConfigMacros._expand_template(template, config, parameters)
        
        return config
    
    def _expand_template(self, template, config, parameters):
        """Expand template with parameters"""
        for line in template:
            if line.startswith('#'):
                # Comment line, skip
                continue
            
            if '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Replace macro parameters
                for param_name, param_value in parameters.items():
                    placeholder = "{" + param_name + "}"
                    if placeholder in value:
                        value = value.replace(placeholder, str(param_value))
                    
                    if placeholder in key:
                        key = key.replace(placeholder, str(param_value))
                
                config.set(key, value)
    
    def list_macros(self):
        """List all defined macros"""
        return list(self.macros.keys())
```

## 5. Configuration Command Line Interface

### 5.1 Command Line Configuration Commands

```bash
# Configuration command examples
nlm --config --help                    # Show configuration help
nlm --create-config-template template.json      # Create configuration template
nlm --validate-config config.json                # Validate configuration
nlm --analyze-config config.json                # Analyze configuration
nlm --compare-configs config1.json config2.json  # Compare configurations
nlm --export-profile --name "my_profile" --output profile.json  # Export profile
nlm --import-profile profile.json --name "imported"              # Import profile
nlm --list-profiles                                               # List all profiles
nlm --version-history --show                                     # Show version history
nlm --migrate-config old_config.cfg --output new_config.json     # Migrate configuration
nlm --expand-macro large_neural --neurons=10000 --plasticity=full  # Expand macro

# Advanced configuration operations
nlm --bulk-config-update --input config_updates.json          # Bulk update
nlm --template-config --template research --neurons=5000      # Template configuration
nlm --profile-analysis --config config.json                   # Profile analysis
nlm --macro-management --list                                 # List macros
nlm --version-management --create "Initial configuration"      # Create version
```

### 5.2 Configuration Script Language

```python
class ConfigScript:
    """Configuration script language for batch operations"""
    
    def __init__(self):
        self.commands = []
    
    def load_script(self, script_path):
        """Load configuration script from file"""
        with open(script_path, 'r') as f:
            script_content = f.read()
        
        # Parse script (simplified)
        lines = script_content.split('\n')
        for line in lines:
            line = line.strip()
            if line and not line.startswith('#'):
                self.commands.append(line)
    
    def execute(self, config):
        """Execute configuration script"""
        for command in self.commands:
            self._execute_command(config, command)
        
        return config
    
    def _execute_command(self, config, command):
        """Execute a single configuration command"""
        if command.startswith('set '):
            # Set parameter
            parts = command[4:].split('=', 1)
            if len(parts) == 2:
                key, value = parts[0].strip(), parts[1].strip()
                config.set(key, value)
        
        elif command.startswith('load '):
            # Load configuration
            filepath = command[5:].strip()
            loaded_config = pynlm.loadFromFile(filepath)
            config.merge(loaded_config)
        
        elif command.startswith('save '):
            # Save configuration
            filepath = command[5:].strip()
            config.saveToFile(filepath)
        
        elif command.startswith('profile '):
            # Create profile
            parts = command[7:].split(' ', 1)
            if len(parts) == 2:
                name, description = parts[0].strip(), parts[1].strip()
                ConfigProfiles.create_profile(name, config)
        
        elif command.startswith('macro '):
            # Define macro
            parts = command[6:].split('=', 1)
            if len(parts) == 2:
                name, template = parts[0].strip(), parts[1].strip()
                ConfigMacros.define_macro(name, template.split('\n'))
    
    def save_script(self, script_path):
        """Save configuration script to file"""
        with open(script_path, 'w') as f:
            for command in self.commands:
                f.write(command + '\n')
```

## 6. Configuration Security and Access Control

### 6.1 Configuration Access Control

```python
class ConfigSecurity:
    """Configuration access control and security"""
    
    def __init__(self):
        self.access_control = {}
        self.encryption_keys = {}
    
    def add_user_access(self, username, permissions):
        """Add user access permissions"""
        self.access_control[username] = permissions
    
    def check_access(self, username, operation, resource):
        """Check if user has access to operation on resource"""
        if username not in self.access_control:
            return False
        
        permissions = self.access_control[username]
        
        # Check operation permission
        if 'operations' in permissions:
            if operation not in permissions['operations']:
                return False
        
        # Check resource permission
        if 'resources' in permissions:
            if resource not in permissions['resources']:
                return False
        
        return True
    
    def set_encryption(self, resource, key):
        """Set encryption key for resource"""
        self.encryption_keys[resource] = key
    
    def encrypt_config(self, config, resource):
        """Encrypt configuration"""
        if resource not in self.encryption_keys:
            raise ValueError(f"No encryption key for resource: {resource}")
        
        # Implementation depends on encryption method
        # For now, just return the config (encryption would be added)
        return config
    
    def decrypt_config(self, config_data, resource):
        """Decrypt configuration data"""
        if resource not in self.encryption_keys:
            raise ValueError(f"No encryption key for resource: {resource}")
        
        # Implementation depends on encryption method
        # For now, just return the config data
        return config_data
```

### 6.2 Configuration Audit Trail

```python
class ConfigAudit:
    """Configuration audit trail and logging"""
    
    def __init__(self):
        self.audit_log = []
    
    def log_operation(self, username, operation, resource, details):
        """Log configuration operation"""
        audit_entry = {
            'timestamp': time.time(),
            'username': username,
            'operation': operation,
            'resource': resource,
            'details': details,
            'success': True
        }
        
        self.audit_log.append(audit_entry)
    
    def get_audit_log(self, start_time=None, end_time=None, username=None):
        """Get audit log with optional filters"""
        filtered_log = self.audit_log
        
        if start_time:
            filtered_log = [entry for entry in filtered_log if entry['timestamp'] >= start_time]
        
        if end_time:
            filtered_log = [entry for entry in filtered_log if entry['timestamp'] <= end_time]
        
        if username:
            filtered_log = [entry for entry in filtered_log if entry['username'] == username]
        
        return filtered_log
    
    def export_audit_log(self, filepath, start_time=None, end_time=None):
        """Export audit log to file"""
        filtered_log = self.get_audit_log(start_time, end_time)
        
        with open(filepath, 'w') as f:
            for entry in filtered_log:
                f.write(f"{entry['timestamp']}, {entry['username']}, {entry['operation']}, "
                       f"{entry['resource']}, {entry['details']}, {entry['success']}\n")
```

## 7. Configuration Testing and Validation

### 7.1 Configuration Test Framework

```python
class ConfigTestFramework:
    """Configuration testing framework"""
    
    def __init__(self):
        self.test_cases = []
    
    def add_test_case(self, name, config, expected_valid=True, expected_errors=None):
        """Add test case"""
        self.test_cases.append({
            'name': name,
            'config': config,
            'expected_valid': expected_valid,
            'expected_errors': expected_errors or []
        })
    
    def run_tests(self):
        """Run all test cases"""
        results = []
        
        for test_case in self.test_cases:
            try:
                errors = ConfigValidator.validate_config(test_case['config'])
                
                is_valid = len(errors) == 0
                
                if is_valid == test_case['expected_valid']:
                    # Test passed
                    result = 'PASSED'
                else:
                    # Test failed
                    result = 'FAILED'
                
                results.append({
                    'name': test_case['name'],
                    'result': result,
                    'errors': errors,
                    'expected_valid': test_case['expected_valid'],
                    'expected_errors': test_case['expected_errors']
                })
                
            except Exception as e:
                # Test crashed
                results.append({
                    'name': test_case['name'],
                    'result': 'CRASHED',
                    'error': str(e),
                    'expected_valid': test_case['expected_valid']
                })
        
        return results
    
    def generate_test_report(self, results):
        """Generate test report"""
        total_tests = len(results)
        passed_tests = len([r for r in results if r['result'] == 'PASSED'])
        failed_tests = len([r for r in results if r['result'] == 'FAILED'])
        crashed_tests = len([r for r in results if r['result'] == 'CRASHED'])
        
        report = {
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': failed_tests,
            'crashed_tests': crashed_tests,
            'success_rate': passed_tests / total_tests if total_tests > 0 else 0,
            'results': results
        }
        
        return report
```

## 8. Integration with Advanced User Commands

### 8.1 Configuration-Aware Command Processing

```python
class AdvancedCommandProcessor:
    """Advanced command processor with configuration awareness"""
    
    def __init__(self, config):
        self.config = config
    
    def process_command(self, command):
        """Process advanced command"""
        if command.startswith('--config'):
            return self._process_config_command(command)
        elif command.startswith('--template'):
            return self._process_template_command(command)
        elif command.startswith('--profile'):
            return self._process_profile_command(command)
        elif command.startswith('--macro'):
            return self._process_macro_command(command)
        else:
            return False  # Not a configuration command
    
    def _process_config_command(self, command):
        """Process configuration command"""
        if '--validate-config' in command:
            self._validate_config_command(command)
            return True
        elif '--analyze-config' in command:
            self._analyze_config_command(command)
            return True
        elif '--compare-configs' in command:
            self._compare_configs_command(command)
            return True
        elif '--export-profile' in command:
            self._export_profile_command(command)
            return True
        elif '--import-profile' in command:
            self._import_profile_command(command)
            return True
        
        return False
    
    def _validate_config_command(self, command):
        """Validate configuration command"""
        # Extract config file path from command
        parts = command.split()
        for i, part in enumerate(parts):
            if part == '--validate-config' and i + 1 < len(parts):
                config_path = parts[i + 1]
                # Validate configuration
                errors = ConfigValidator.validate_config(config_path)
                if errors:
                    print("Configuration validation failed:")
                    for error in errors:
                        print(f"  - {error}")
                else:
                    print("Configuration validation passed")
                break
    
    def _analyze_config_command(self, command):
        """Analyze configuration command"""
        # Extract config file path from command
        parts = command.split()
        for i, part in enumerate(parts):
            if part == '--analyze-config' and i + 1 < len(parts):
                config_path = parts[i + 1]
                # Load and analyze configuration
                config = pynlm.loadFromFile(config_path)
                analysis = ConfigAnalyzer.analyze_config(config)
                
                print("Configuration Analysis:")
                print(f"  Total parameters: {analysis['total_parameters']}")
                print(f"  Sections: {analysis['sections']}")
                
                if analysis['potential_issues']:
                    print("  Potential Issues:")
                    for issue in analysis['potential_issues']:
                        print(f"    - {issue}")
                
                if analysis['recommendations']:
                    print("  Recommendations:")
                    for recommendation in analysis['recommendations']:
                        print(f"    - {recommendation}")
                
                break
    
    def _compare_configs_command(self, command):
        """Compare configurations command"""
        # Extract config files from command
        parts = command.split()
        config_files = [part for part in parts if part.endswith('.json') or part.endswith('.yaml')]
        
        if len(config_files) >= 2:
            config1 = pynlm.loadFromFile(config_files[0])
            config2 = pynlm.loadFromFile(config_files[1])
            
            differences = ConfigAnalyzer.compare_configs(config1, config2)
            
            print(f"Configuration Comparison between {config_files[0]} and {config_files[1]}:")
            if differences:
                print("  Differences:")
                for key, diff in differences.items():
                    print(f"    {key}:")
                    print(f"      Config1: {diff['config1_value']}")
                    print(f"      Config2: {diff['config2_value']}")
            else:
                print("  No differences found")
    
    def _export_profile_command(self, command):
        """Export profile command"""
        # Extract profile name and output file
        parts = command.split()
        name = None
        output_file = None
        
        for i, part in enumerate(parts):
            if part == '--name' and i + 1 < len(parts):
                name = parts[i + 1]
            elif part == '--output' and i + 1 < len(parts):
                output_file = parts[i + 1]
        
        if name and output_file:
            profile = ConfigProfiles.get_profile(name)
            profile.saveToFile(output_file)
            print(f"Profile '{name}' exported to {output_file}")
    
    def _import_profile_command(self, command):
        """Import profile command"""
        # Extract profile name and input file
        parts = command.split()
        name = None
        input_file = None
        
        for i, part in enumerate(parts):
            if part == '--name' and i + 1 < len(parts):
                name = parts[i + 1]
            elif part == '--input' and i + 1 < len(parts):
                input_file = parts[i + 1]
        
        if name and input_file:
            profile = pynlm.loadFromFile(input_file)
            ConfigProfiles.create_profile(name, profile)
            print(f"Profile '{name}' imported from {input_file}")
    
    def _process_template_command(self, command):
        """Process template command"""
        if '--template' in command:
            # Extract template name and parameters
            parts = command.split()
            template = None
            parameters = {}
            
            for i, part in enumerate(parts):
                if part == '--template' and i + 1 < len(parts):
                    template = parts[i + 1]
                elif part.startswith('--') and i + 1 < len(parts):
                    param_name = part[2:]  # Remove '--'
                    param_value = parts[i + 1]
                    parameters[param_name] = param_value
            
            if template:
                expanded_config = ConfigTemplates.expand_macro(template, parameters)
                print(f"Template '{template}' expanded with parameters: {parameters}")
                print(f"Expanded configuration saved")
    
    def _process_profile_command(self, command):
        """Process profile command"""
        if '--list-profiles' in command:
            profiles = ConfigProfiles.list_profiles()
            print("Available profiles:")
            for profile in profiles:
                print(f"  - {profile}")
    
    def _process_macro_command(self, command):
        """Process macro command"""
        if '--macro-management' in command:
            macros = ConfigMacros.list_macros()
            print("Available macros:")
            for macro in macros:
                print(f"  - {macro}")
```

## Implementation Summary

The enhanced configuration management system provides:

1. **Multi-format Support**: JSON, YAML, key=value, and future format support
2. **Schema Validation**: Comprehensive schema-based configuration validation
3. **Business Rule Validation**: Context-aware business rule checking
4. **Template System**: Parameterized configuration templates
5. **Profile Management**: Named configuration profiles with persistence
6. **Version Control**: Configuration versioning and history tracking
7. **Macro System**: Parameterized macro expansion for complex configurations
8. **Migration Tools**: Seamless migration from older configuration formats
9. **Security Features**: Access control and encryption support
10. **Audit Trail**: Comprehensive logging and auditing capabilities

These improvements transform the NLM configuration system from a basic key=value parser into a sophisticated, enterprise-grade configuration management system that supports complex, large-scale neural network deployments.
