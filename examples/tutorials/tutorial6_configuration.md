# Tutorial 6: Advanced Configuration

This tutorial explores advanced configuration options for NLM, including schema validation, templates, monitoring, and best practices for efficient and effective NLM configuration.

## Learning Objectives

By the end of this tutorial, you will be able to:

- Understand advanced configuration options in NLM
- Implement schema validation for configuration
- Use configuration templates for consistency
- Monitor and manage configuration changes
- Write efficient and effective NLM code
- Identify and troubleshoot common configuration issues

## 1. Introduction to Advanced Configuration

Advanced configuration in NLM goes beyond basic settings to provide:

- **Schema validation** for configuration correctness
- **Template management** for reusable configurations
- **Change monitoring** for configuration tracking
- **Configuration comparison** for version control
- **Export/import capabilities** for configuration sharing

### 1.1 Configuration Components

| Component | Purpose | Benefits |
|-----------|---------|----------|
| **Schema Validation** | Validate configuration structure | Error prevention |
| **Templates** | Reusable configuration patterns | Consistency |
| **Monitoring** | Track configuration changes | Audit trails |
| **Comparison** | Compare configurations | Change tracking |
| **Export/Import** | Share configurations | Collaboration |

## 2. Schema Validation

### 2.1 Configuration Schema

```python
import pynlm

# Create configuration with schema validation
config = pynlm.createDefaultConfig()

# Enable schema validation
config.setValidationEnabled(True)

# Validate configuration against schema
validation_errors = config.validateConfiguration()

if validation_errors:
    print("Configuration validation errors:")
    for error in validation_errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

### 2.2 Custom Configuration Schema

```python
class CustomConfiguration:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.setValidationEnabled(True)
        
        # Define custom schema
        self.custom_schema = {
            "required_keys": ["neuron_count", "region_count", "plasticity_learning_rate"],
            "optional_keys": ["environment_width", "environment_height"],
            "value_ranges": {
                "neuron_count": (100, 10000),
                "region_count": (1, 10),
                "plasticity_learning_rate": (0.001, 0.1)
            }
        }
        
    def validate_custom_config(self, config):
        """Validate custom configuration schema"""
        
        errors = []
        
        # Check required keys
        for key in self.custom_schema["required_keys"]:
            if not config.has(key):
                errors.append(f"Missing required key: {key}")
                
        # Check value ranges
        for key, (min_val, max_val) in self.custom_schema["value_ranges"].items():
            if config.has(key):
                value = config.get(key)
                if value < min_val or value > max_val:
                    errors.append(f"Value for {key} ({value}) outside range [{min_val}, {max_val}]")
                    
        return errors
```

### 2.3 Schema Validation in Action

```python
class SchemaValidationExample:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.setValidationEnabled(True)
        
    def demonstrate_schema_validation(self):
        """Demonstrate schema validation"""
        
        print("Schema Validation Example:")
        print("=" * 40)
        
        # Configure validation
        self.config.setValidationEnabled(True)
        
        # Set some configuration values
        self.config.set("neuron_count", 1500)
        self.config.set("region_count", 3)
        self.config.set("plasticity_learning_rate", 0.02)
        
        # Validate configuration
        validation_errors = self.config.validateConfiguration()
        
        if validation_errors:
            print("Configuration validation errors:")
            for i, error in enumerate(validation_errors, 1):
                print(f"  {i}. {error}")
        else:
            print("✓ Configuration is valid!")
            
        # Test with invalid configuration
        print("\nTesting with invalid configuration:")
        
        # Create invalid config
        invalid_config = pynlm.createDefaultConfig()
        invalid_config.set("neuron_count", 50000)  # Too high
        invalid_config.set("region_count", 15)     # Too high
        invalid_config.setValidationEnabled(True)
        
        invalid_errors = invalid_config.validateConfiguration()
        if invalid_errors:
            print("Validation errors (expected):")
            for i, error in enumerate(invalid_errors, 1):
                print(f"  {i}. {error}")
```

## 3. Configuration Templates

### 3.1 Template Management

```python
class ConfigurationTemplates:
    def __init__(self):
        self.config_templates = {}
        
    def create_template(self, name, description, configuration):
        """Create a new configuration template"""
        template = {
            "name": name,
            "description": description,
            "configuration": configuration,
            "created_at": "2026-10-19",  # Would use actual timestamp
            "version": "1.0"
        }
        
        self.config_templates[name] = template
        print(f"Template '{name}' created successfully")
        
    def load_template(self, name):
        """Load a configuration template"""
        if name not in self.config_templates:
            raise ValueError(f"Template '{name}' not found")
            
        template = self.config_templates[name]
        print(f"Template '{name}' loaded:")
        print(f"  Description: {template['description']}")
        print(f"  Version: {template['version']}")
        
        return template["configuration"]
        
    def list_templates(self):
        """List all available templates"""
        print("Available Templates:")
        for name, template in self.config_templates.items():
            print(f"  - {name}: {template['description']}")
```

### 3.2 Template Example

```python
class TemplateExample:
    def __init__(self):
        self.templates = ConfigurationTemplates()
        
    def setup_templates(self):
        """Set up example templates"""
        
        # Create basic template
        basic_config = pynlm.createDefaultConfig()
        basic_config.set("neuron_count", 1000)
        basic_config.set("region_count", 2)
        self.templates.create_template(
            "basic",
            "Basic NLM configuration with standard settings",
            basic_config
        )
        
        # Create advanced template
        advanced_config = pynlm.createDefaultConfig()
        advanced_config.set("neuron_count", 5000)
        advanced_config.set("region_count", 5)
        advanced_config.set("plasticity_learning_rate", 0.05)
        advanced_config.set("environment_width", 30)
        advanced_config.set("environment_height", 30)
        self.templates.create_template(
            "advanced",
            "Advanced NLM configuration with enhanced capabilities",
            advanced_config
        )
        
        # Create development template
        development_config = pynlm.createDefaultConfig()
        development_config.set("neuron_count", 2000)
        development_config.set("region_count", 3)
        development_config.set("plasticity_learning_rate", 0.1)
        development_config.set("environment_width", 20)
        development_config.set("environment_height", 20)
        self.templates.create_template(
            "development",
            "Development-focused NLM configuration",
            development_config
        )
        
    def use_template(self, template_name):
        """Use a configuration template"""
        
        print(f"\nUsing template: {template_name}")
        
        # Load template
        template_config = self.templates.load_template(template_name)
        
        # Create brain with template configuration
        brain = pynlm.createBrain(template_config)
        brain.initialize()
        
        print(f"Brain created with template configuration")
        print(f"  Neuron count: {brain.getTotalNeuronCount()}")
        
        return brain
```

## 4. Configuration Monitoring

### 4.1 Configuration Monitoring

```python
class ConfigurationMonitor:
    def __init__(self):
        self.change_history = []
        self.monitoring_enabled = True
        
    def monitor_configuration(self, config):
        """Monitor configuration changes"""
        
        if not self.monitoring_enabled:
            return
            
        # Track configuration state
        current_state = self.get_configuration_state(config)
        
        # Check for changes
        if len(self.change_history) == 0:
            # First configuration
            self.change_history.append({
                'timestamp': '2026-10-19',  # Would use actual timestamp
                'event': 'configuration_created',
                'state': current_state
            })
        else:
            # Compare with previous state
            previous_state = self.change_history[-1]['state']
            if self.configuration_changed(previous_state, current_state):
                # Record change
                self.change_history.append({
                    'timestamp': '2026-10-19',  # Would use actual timestamp
                    'event': 'configuration_changed',
                    'state': current_state,
                    'changes': self.get_configuration_changes(previous_state, current_state)
                })
                
    def get_configuration_state(self, config):
        """Get current configuration state"""
        return {
            'timestamp': '2026-10-19',
            'keys': config.getKeys(),
            'values': {key: config.get(key) for key in config.getKeys()}
        }
        
    def configuration_changed(self, previous_state, current_state):
        """Check if configuration has changed"""
        return current_state['values'] != previous_state['values']
        
    def get_configuration_changes(self, previous_state, current_state):
        """Get specific configuration changes"""
        changes = []
        
        # Find added keys
        added_keys = set(current_state['keys']) - set(previous_state['keys'])
        for key in added_keys:
            changes.append({
                'type': 'added',
                'key': key,
                'value': current_state['values'][key]
            })
            
        # Find removed keys
        removed_keys = set(previous_state['keys']) - set(current_state['keys'])
        for key in removed_keys:
            changes.append({
                'type': 'removed',
                'key': key,
                'value': previous_state['values'][key]
            })
            
        # Find changed keys
        common_keys = set(current_state['keys']) & set(previous_state['keys'])
        for key in common_keys:
            if current_state['values'][key] != previous_state['values'][key]:
                changes.append({
                    'type': 'changed',
                    'key': key,
                    'from_value': previous_state['values'][key],
                    'to_value': current_state['values'][key]
                })
                
        return changes
```

### 4.2 Monitoring Example

```python
class MonitoringExample:
    def __init__(self):
        self.monitor = ConfigurationMonitor()
        
    def demonstrate_monitoring(self):
        """Demonstrate configuration monitoring"""
        
        print("Configuration Monitoring Example:")
        print("=" * 40)
        
        # Create and monitor configuration
        config = pynlm.createDefaultConfig()
        
        # Monitor configuration changes
        self.monitor.monitor_configuration(config)
        
        # Modify configuration
        config.set("neuron_count", 1500)
        self.monitor.monitor_configuration(config)
        
        config.set("region_count", 3)
        self.monitor.monitor_configuration(config)
        
        config.set("plasticity_learning_rate", 0.05)
        self.monitor.monitor_configuration(config)
        
        # Display monitoring results
        print(f"\nConfiguration monitoring completed:")
        print(f"  Total events tracked: {len(self.monitor.change_history)}")
        
        for i, event in enumerate(self.monitor.change_history, 1):
            print(f"  Event {i}: {event['event']} at {event['timestamp']}")
            if 'changes' in event:
                for change in event['changes']:
                    print(f"    - {change['type']}: {change['key']}")
```

## 5. Configuration Comparison

### 5.1 Configuration Comparison

```python
class ConfigurationComparison:
    def __init__(self):
        
    def compare_configurations(self, config1, config2, name1="Config 1", name2="Config 2"):
        """Compare two configurations"""
        
        print(f"Configuration Comparison: {name1} vs {name2}")
        print("=" * 50)
        
        # Get configuration keys
        keys1 = config1.getKeys()
        keys2 = config2.getKeys()
        
        # Find common keys
        common_keys = set(keys1) & set(keys2)
        
        print(f"Common keys: {len(common_keys)}")
        
        # Compare common keys
        differences = []
        
        for key in common_keys:
            value1 = config1.get(key)
            value2 = config2.get(key)
            
            if value1 != value2:
                differences.append((key, value1, value2))
                print(f"  {key}:")
                print(f"    {name1}: {value1}")
                print(f"    {name2}: {value2}")
                
        # Find keys only in first config
        only_in_1 = set(keys1) - set(keys2)
        if only_in_1:
            print(f"Keys only in {name1}:")
            for key in only_in_1:
                print(f"  - {key}: {config1.get(key)}")
                
        # Find keys only in second config
        only_in_2 = set(keys2) - set(keys1)
        if only_in_2:
            print(f"Keys only in {name2}:")
            for key in only_in_2:
                print(f"  - {key}: {config2.get(key)}")
                
        return differences
```

### 5.2 Configuration Merging

```python
class ConfigurationMerging:
    def __init__(self):
        
    def merge_configurations(self, config1, config2):
        """Merge two configurations into a new one"""
        
        # Create new configuration
        merged_config = pynlm.createDefaultConfig()
        
        # Get all keys from both configurations
        keys1 = config1.getKeys()
        keys2 = config2.getKeys()
        all_keys = set(keys1) | set(keys2)
        
        # Merge values
        for key in all_keys:
            if key in keys1:
                merged_config.set(key, config1.get(key))
            if key in keys2:
                merged_config.set(key, config2.get(key))
                
        return merged_config
        
    def demonstrate_merging(self):
        """Demonstrate configuration merging"""
        
        print("Configuration Merging Example:")
        print("=" * 40)
        
        # Create two configurations
        config1 = pynlm.createDefaultConfig()
        config1.set("neuron_count", 1000)
        config1.set("region_count", 2)
        
        config2 = pynlm.createDefaultConfig()
        config2.set("plasticity_learning_rate", 0.05)
        config2.set("environment_width", 30)
        
        print("Config 1:")
        for key in config1.getKeys():
            print(f"  {key}: {config1.get(key)}")
            
        print("\nConfig 2:")
        for key in config2.getKeys():
            print(f"  {key}: {config2.get(key)}")
            
        # Merge configurations
        merged = self.merge_configurations(config1, config2)
        
        print(f"\nMerged configuration:")
        for key in merged.getKeys():
            print(f"  {key}: {merged.get(key)}")
```

## 6. Export and Import

### 6.1 Configuration Export

```python
class ConfigurationExport:
    def __init__(self):
        
    def export_configuration(self, config, filename, format="json"):
        """Export configuration to file"""
        
        print(f"Exporting configuration to {filename} ({format})...")
        
        # Prepare configuration data
        config_data = {
            "keys": config.getKeys(),
            "values": {key: config.get(key) for key in config.getKeys()},
            "timestamp": '2026-10-19'  # Would use actual timestamp
        }
        
        # Export to file (simplified implementation)
        if format == "json":
            import json
            with open(filename, 'w') as f:
                json.dump(config_data, f, indent=2)
        elif format == "txt":
            with open(filename, 'w') as f:
                for key, value in config_data["values"].items():
                    f.write(f"{key}={value}\n")
        elif format == "ini":
            with open(filename, 'w') as f:
                f.write("[configuration]\n")
                for key, value in config_data["values"].items():
                    f.write(f"{key}={value}\n")
        
        print(f"Configuration exported to {filename}")
        return True
```

### 6.2 Configuration Import

```python
class ConfigurationImport:
    def __init__(self):
        
    def import_configuration(self, filename, format="json"):
        """Import configuration from file"""
        
        print(f"Importing configuration from {filename} ({format})...")
        
        # Import from file (simplified implementation)
        if format == "json":
            import json
            with open(filename, 'r') as f:
                config_data = json.load(f)
        elif format == "txt":
            with open(filename, 'r') as f:
                config_data = self.parse_txt_file(f)
        elif format == "ini":
            with open(filename, 'r') as f:
                config_data = self.parse_ini_file(f)
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply configuration values
        for key, value in config_data.get("values", {}).items():
            config.set(key, value)
            
        print(f"Configuration imported from {filename}")
        return config
        
    def parse_txt_file(self, file):
        """Parse .txt configuration file"""
        config_data = {"values": {}}
        
        for line in file:
            line = line.strip()
            if line and not line.startswith('#'):
                if '=' in line:
                    key, value = line.split('=', 1)
                    config_data["values"][key.strip()] = self.parse_value(value.strip())
                    
        return config_data
        
    def parse_ini_file(self, file):
        """Parse .ini configuration file"""
        config_data = {"values": {}}
        current_section = None
        
        for line in file:
            line = line.strip()
            if not line or line.startswith('#') or line.startswith(';'):
                continue
                
            if line.startswith('[') and line.endswith(']'):
                current_section = line[1:-1]
            elif '=' in line and current_section == "configuration":
                key, value = line.split('=', 1)
                config_data["values"][key.strip()] = self.parse_value(value.strip())
                
        return config_data
        
    def parse_value(self, value_str):
        """Parse configuration value string"""
        
        # Try to convert to appropriate type
        try:
            return int(value_str)
        except ValueError:
            try:
                return float(value_str)
            except ValueError:
                return value_str
```

### 6.3 Export/Import Example

```python
class ExportImportExample:
    def __init__(self):
        self.exporter = ConfigurationExport()
        self.importer = ConfigurationImport()
        
    def demonstrate_export_import(self):
        """Demonstrate export and import functionality"""
        
        print("Export/Import Example:")
        print("=" * 40)
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1500)
        config.set("region_count", 3)
        config.set("plasticity_learning_rate", 0.05)
        config.set("environment_width", 25)
        config.set("environment_height", 25)
        
        print("Original configuration:")
        for key in config.getKeys():
            print(f"  {key}: {config.get(key)}")
        
        # Export configuration
        export_filename = "exported_config.json"
        self.exporter.export_configuration(config, export_filename, "json")
        
        # Import configuration
        imported_config = self.importer.import_configuration(export_filename, "json")
        
        print(f"\nImported configuration:")
        for key in imported_config.getKeys():
            print(f"  {key}: {imported_config.get(key)}")
            
        # Verify they are the same
        if config.getKeys() == imported_config.getKeys():
            print(f"\n✓ Export/import successful: Configurations match!")
        else:
            print(f"\n✗ Export/import failed: Configurations differ!")
```

## 7. Best Practices

### 7.1 Configuration Best Practices

```python
class ConfigurationBestPractices:
    def __init__(self):
        
    def validate_configuration(self, config):
        """Validate configuration according to best practices"""
        
        errors = []
        
        # Check for required keys
        required_keys = ["neuron_count", "region_count"]
        for key in required_keys:
            if not config.has(key):
                errors.append(f"Missing required key: {key}")
                
        # Check for valid value ranges
        if config.has("neuron_count"):
            neuron_count = config.get("neuron_count")
            if neuron_count < 100 or neuron_count > 10000:
                errors.append("neuron_count should be between 100 and 10000")
                
        if config.has("region_count"):
            region_count = config.get("region_count")
            if region_count < 1 or region_count > 10:
                errors.append("region_count should be between 1 and 10")
                
        # Check for valid data types
        if config.has("plasticity_learning_rate"):
            learning_rate = config.get("plasticity_learning_rate")
            if not isinstance(learning_rate, (int, float)):
                errors.append("plasticity_learning_rate should be a number")
                
        return errors
        
    def optimize_configuration(self, config):
        """Optimize configuration for performance"""
        
        # Optimize for memory usage
        if config.has("neuron_count"):
            neuron_count = config.get("neuron_count")
            if neuron_count > 5000:
                print("Warning: Large neuron count may impact performance")
                print("Consider reducing neuron_count for better performance")
                
        # Optimize for speed
        if config.has("plasticity_learning_rate"):
            learning_rate = config.get("plasticity_learning_rate")
            if learning_rate > 0.05:
                print("Warning: High learning rate may cause instability")
                print("Consider reducing plasticity_learning_rate for stability")
                
        # Optimize for learning
        if not config.has("environment_width") or not config.has("environment_height"):
            config.set("environment_width", 20)
            config.set("environment_height", 20)
            print("Set default environment dimensions")
```

### 7.2 Efficient Configuration Usage

```python
class EfficientConfigurationUsage:
    def __init__(self):
        self.configuration_cache = {}
        
    def load_configuration_efficiently(self, filename):
        """Load configuration efficiently"""
        
        # Check cache first
        if filename in self.configuration_cache:
            print(f"Loading configuration from cache: {filename}")
            return self.configuration_cache[filename]
            
        # Load from file
        print(f"Loading configuration from file: {filename}")
        config = self.importer.import_configuration(filename, "json")
        
        # Cache for future use
        self.configuration_cache[filename] = config
        
        return config
        
    def create_multiple_brains(self, config, count):
        """Create multiple brains with same configuration"""
        
        brains = []
        
        for i in range(count):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
            
        print(f"Created {count} brains with shared configuration")
        return brains
```

## 8. Common Configuration Issues

### 8.1 Configuration Problems

**Problem:** Configuration validation fails due to schema issues.
**Solution:** Ensure configuration keys are valid and values are within expected ranges.

**Problem:** Configuration too large for memory.
**Solution:** Reduce neuron count, simplify configuration, or use incremental loading.

**Problem:** Configuration not serializable.
**Solution:** Ensure all values are JSON-serializable (numbers, strings, lists, dictionaries).

### 8.2 Troubleshooting

```python
class ConfigurationTroubleshooting:
    def __init__(self):
        
    def troubleshoot_configuration_issues(self, config):
        """Troubleshoot common configuration issues"""
        
        issues = []
        
        # Check for invalid configuration
        if not config.getKeys():
            issues.append("Configuration has no keys - creating default configuration")
            # Create default configuration
            config = pynlm.createDefaultConfig()
            
        # Check for invalid values
        if config.has("neuron_count"):
            neuron_count = config.get("neuron_count")
            if neuron_count < 1:
                issues.append("neuron_count is too small - setting to 100")
                config.set("neuron_count", 100)
            elif neuron_count > 100000:
                issues.append("neuron_count is too large - setting to 10000")
                config.set("neuron_count", 10000)
                
        # Check for valid types
        if config.has("plasticity_learning_rate"):
            learning_rate = config.get("plasticity_learning_rate")
            if not isinstance(learning_rate, (int, float)):
                issues.append("plasticity_learning_rate is not a number - setting to 0.01")
                config.set("plasticity_learning_rate", 0.01)
                
        return issues, config
```

## 9. Next Steps

### 9.1 Continue to Reference Documentation

Move on to the **Reference Documentation** for comprehensive API references and detailed configuration options.

### 9.2 Explore Advanced Configuration

Explore the advanced configuration examples:

- **Schema Validation**: `Tutorial 6` shows advanced configuration validation
- **Configuration Templates**: Use templates for consistent configurations
- **Configuration Monitoring**: Track configuration changes over time

### 9.3 Configuration Research

Study the configuration-related documentation:

- [Configuration Guide](https://kilo.ai/docs/configuration)
- [Configuration Schema](https://kilo.ai/docs/configuration-schema)
- [Configuration Best Practices](https://kilo.ai/docs/configuration-best-practices)

## 10. Summary

Advanced configuration in NLM provides powerful tools for:

1. **Schema validation** for configuration correctness
2. **Template management** for consistent configurations
3. **Change monitoring** for configuration tracking
4. **Configuration comparison** for version control
5. **Export/import capabilities** for configuration sharing

These advanced configuration features enable:

- **Reliable configurations** through validation
- **Consistent configurations** through templates
- **Trackable configurations** through monitoring
- **Shareable configurations** through export/import
- **Efficient configurations** through optimization

---

*Tutorial 6 completed successfully! Ready for Reference Documentation.*