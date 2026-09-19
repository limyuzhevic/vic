"""
Test script for NLM Configuration Manager

Demonstrates the features and capabilities of the ConfigManager class.
"""

from nlm import (
    ConfigManager, 
    ValidationLevel, 
    ConfigProfile,
    ConfigGroup,
    ConfigSource
)


def test_basic_config():
    """Test basic configuration functionality"""
    print("=== Test 1: Basic Configuration ===")
    
    config = ConfigManager()
    
    # Set some configuration values
    config.set("brain.neuron_count", 1000, groups=["brain"])
    config.set("world.width", 20, groups=["world"])
    config.set("agent.enabled", True, groups=["agent"])
    
    # Get values
    print(f"Neuron count: {config.get('brain.neuron_count')}")
    print(f"World width: {config.get('world.width')}")
    
    # Check existence
    print(f"Has brain.neuron_count: {config.has('brain.neuron_count')}")
    print(f"Has world.height: {config.has('world.height')}")
    
    # Get all keys
    print(f"All keys: {config.get_keys()}")
    print(f"Keys in 'brain' group: {config.get_keys('brain')}")
    
    # Get by group
    brain_config = config.get_by_group("brain")
    print(f"Brain config: {brain_config}")
    
    print()


def test_validation():
    """Test configuration validation"""
    print("=== Test 2: Configuration Validation ===")
    
    # Create config with strict validation
    config = ConfigManager(validation_level=ValidationLevel.STRICT)
    
    # Set valid values
    config.set("test.string", "valid", 
              description="A valid string",
              type_hint="string")
    
    config.set("test.number", 42,
              description="A valid number",
              type_hint="integer")
    
    config.set("test.boolean", True,
              description="A valid boolean",
              type_hint="boolean")
    
    # Get validation errors (should be none)
    errors = config.get_validation_errors()
    print(f"Validation errors (should be empty): {errors}")
    
    # Try to set invalid value (this would raise an error in strict mode)
    print("Note: Setting invalid values would raise ValueError in strict mode")
    print()


def test_profiles():
    """Test configuration profiles"""
    print("=== Test 3: Configuration Profiles ===")
    
    # Create a production profile
    production = ConfigProfile("production", "Production configuration")
    production.add_default("brain.neuron_count", 5000,
                         description="Production neuron count",
                         type_hint="integer",
                         groups=["brain"])
    production.add_default("world.width", 50,
                         description="Production world width",
                         type_hint="integer",
                         groups=["world"])
    production.add_override("agent.debug_mode", False)
    production.with_validation(ValidationLevel.STRICT)
    
    # Create development profile
    development = ConfigProfile("development", "Development configuration")
    development.add_default("brain.neuron_count", 100,
                          description="Development neuron count",
                          type_hint="integer",
                          groups=["brain"])
    development.add_default("world.width", 20,
                          description="Development world width",
                          type_hint="integer",
                          groups=["world"])
    development.add_override("agent.debug_mode", True)
    development.with_validation(ValidationLevel.BASIC)
    
    # Create config manager and apply profiles
    config = ConfigManager()
    
    # Apply production profile
    config.add_profile("production", production)
    config.apply_profile("production")
    
    print(f"Production neuron count: {config.get('brain.neuron_count')}")
    print(f"Production world width: {config.get('world.width')}")
    
    # Switch to development profile
    config.apply_profile("development")
    
    print(f"Development neuron count: {config.get('brain.neuron_count')}")
    print(f"Development world width: {config.get('world.width')}")
    
    print()


def test_groups():
    """Test configuration groups"""
    print("=== Test 4: Configuration Groups ===")
    
    # Create groups
    brain_group = ConfigGroup("brain", "Brain configuration group")
    brain_group.add_entry(ConfigEntry("brain.neuron_count", 1000,
                                    ConfigSource.DEFAULT,
                                    "Number of neurons",
                                    "integer",
                                    True,  # required
                                    groups=["brain"]))
    
    brain_group.add_entry(ConfigEntry("brain.synapse_density", 0.1,
                                    ConfigSource.DEFAULT,
                                    "Synapse density",
                                    "number",
                                    False,
                                    groups=["brain"]))
    
    world_group = ConfigGroup("world", "World configuration group")
    world_group.add_entry(ConfigEntry("world.width", 20,
                                    ConfigSource.DEFAULT,
                                    "World width",
                                    "integer",
                                    True,
                                    groups=["world"]))
    
    # Create config manager and add groups
    config = ConfigManager()
    config.add_group(brain_group)
    config.add_group(world_group)
    
    print(f"Brain group entries: {list(config.get_group('brain').entries.keys())}")
    print(f"World group entries: {list(config.get_group('world').entries.keys())}")
    
    # Set values
    config.set("brain.neuron_count", 2000)
    config.set("world.width", 30)
    
    print(f"Config with groups - brain entries: {config.get_by_group('brain')}")
    print(f"Config with groups - world entries: {config.get_by_group('world')}")
    
    print()


def test_serialization():
    """Test configuration serialization"""
    print("=== Test 5: Configuration Serialization ===")
    
    config = ConfigManager()
    
    # Set some configuration values
    config.set("brain.neuron_count", 1000)
    config.set("world.width", 20)
    config.set("agent.enabled", True)
    
    # Save to JSON
    config.save_to_file("test_config.json", "json")
    print("Saved to test_config.json")
    
    # Load from JSON
    config2 = ConfigManager()
    config2.load_from_file("test_config.json", "json")
    print(f"Loaded config: {config2.get_all()}")
    
    # Test JSON string
    json_string = config.export_config()
    print(f"Exported to JSON string: {json_string}")
    
    # Load from JSON string
    config3 = ConfigManager()
    config3.load_from_string(json.dumps(json_string), "json")
    print(f"Loaded from JSON string: {config3.get_all()}")
    
    print()


def test_monitoring():
    """Test configuration monitoring"""
    print("=== Test 6: Configuration Monitoring ===")
    
    # Create config with monitoring enabled
    config = ConfigManager(enable_monitoring=True, log_changes=True)
    
    # Track changes with a callback
    change_count = 0
    
    def change_callback(change):
        nonlocal change_count
        change_count += 1
        print(f"Change #{change_count}: {change.key} = {change.old_value} -> {change.new_value}")
    
    config.add_change_callback(change_callback)
    
    # Set initial values
    config.set("test.value1", "initial")
    config.set("test.value2", 100)
    
    # Change values
    config.set("test.value1", "updated")
    config.set("test.value2", 200)
    
    print(f"Total changes detected: {change_count}")
    
    # Note: In a real implementation, the monitor would also check for changes
    # in the background and trigger callbacks.
    
    print()


def test_schema_validation():
    """Test schema validation"""
    print("=== Test 7: Schema Validation ===")
    
    # Define a schema
    schema = {
        "type": "object",
        "properties": {
            "name": {
                "type": "string",
                "minLength": 3,
                "maxLength": 50,
                "required": True
            },
            "age": {
                "type": "integer",
                "minimum": 0,
                "maximum": 150,
                "required": False
            }
        },
        "additionalProperties": False
    }
    
    # Create config with schema validation
    config = ConfigManager(validation_level=ValidationLevel.SCHEMA)
    config.set_schema("user", schema)
    
    # Set valid values
    config.set("name", "Test User")
    config.set("age", 25)
    
    # Get validation errors (should be empty for valid schema)
    errors = config.get_validation_errors()
    print(f"Validation errors with valid schema: {errors}")
    
    print("Note: Schema validation requires jsonschema package")
    print()


def test_copy_and_merge():
    """Test configuration copying and merging"""
    print("=== Test 8: Copy and Merge ===")
    
    # Create original config
    config1 = ConfigManager()
    config1.set("brain.neuron_count", 1000)
    config1.set("world.width", 20)
    
    # Copy config
    config2 = config1.copy()
    print(f"Copied config neuron count: {config2.get('brain.neuron_count')}")
    
    # Merge another config
    config3 = ConfigManager()
    config3.set("brain.synapse_density", 0.2)
    config3.set("agent.enabled", True)
    
    config1.merge_config(config3, strategy="overwrite")
    
    print(f"Merged config includes synapse density: {config1.has('brain.synapse_density')}")
    print(f"Merged config includes agent.enabled: {config1.has('agent.enabled')}")
    
    print()


def test_export_import():
    """Test configuration export and import"""
    print("=== Test 9: Export and Import ===")
    
    config = ConfigManager()
    
    # Set some configuration values
    config.set("brain.neuron_count", 1000)
    config.set("world.width", 20)
    config.set("agent.enabled", True)
    
    # Export to dictionary
    exported = config.export_config(["brain.neuron_count", "world.width"])
    print(f"Exported config: {exported}")
    
    # Import from dictionary
    config2 = ConfigManager()
    config2.import_config(exported)
    print(f"Imported config: {config2.get_all()}")
    
    print()


def main():
    """Run all tests"""
    print("NLM Configuration Manager Tests")
    print("=" * 50)
    print()
    
    test_basic_config()
    test_validation()
    test_profiles()
    test_groups()
    test_serialization()
    test_monitoring()
    test_schema_validation()
    test_copy_and_merge()
    test_export_import()
    
    print("=" * 50)
    print("All tests completed!")
    print()
    print("Note: Some features require additional packages:")
    print("- YAML support requires PyYAML")
    print("- TOML support requires tomli")
    print("- Schema validation requires jsonschema")


if __name__ == "__main__":
    main()