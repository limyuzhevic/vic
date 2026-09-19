"""
Examples of using the NLM Configuration Manager

This file contains practical examples demonstrating the use of the NLM Configuration Manager.
"""

from nlm import (
    ConfigManager, 
    ValidationLevel, 
    ConfigProfile,
    ConfigGroup,
    ConfigSource,
    ConfigEntry,
    ConfigChange
)


def example_basic_config():
    """Example: Basic configuration usage"""
    print("=== Example 1: Basic Configuration ===")
    
    # Create a configuration manager
    config = ConfigManager()
    
    # Set configuration values
    config.set("brain.neuron_count", 1000)
    config.set("world.width", 20)
    config.set("agent.enabled", True)
    
    # Get configuration values
    print(f"Neurons: {config.get('brain.neuron_count')}")
    print(f"World width: {config.get('world.width')}")
    print(f"Agent enabled: {config.get('agent.enabled')}")
    
    # Get all keys
    print(f"All keys: {config.get_keys()}")
    
    # Check if key exists
    print(f"Has 'brain.neuron_count': {config.has('brain.neuron_count')}")
    
    # Save to file
    config.save_to_file("basic_config.json")
    print("Saved to basic_config.json")
    
    # Load from file
    config2 = ConfigManager()
    config2.load_from_file("basic_config.json")
    print(f"Loaded config: {config2.get_all()}")
    
    print()


def example_validation():
    """Example: Configuration validation"""
    print("=== Example 2: Configuration Validation ===")
    
    # Create config with strict validation
    config = ConfigManager(validation_level=ValidationLevel.STRICT)
    
    # Set valid values
    config.set("test.string", "valid string",
              description="A valid string",
              type_hint="string",
              required=True)
    
    config.set("test.number", 42,
              description="A valid number",
              type_hint="integer",
              required=True)
    
    config.set("test.boolean", True,
              description="A valid boolean",
              type_hint="boolean",
              required=True)
    
    # Try to set invalid value (would raise ValueError in strict mode)
    try:
        config.set("test.invalid", "short",  # Too short for minLength
                  validation_rules={"minLength": 5},
                  type_hint="string")
    except ValueError as e:
        print(f"Validation error (expected): {e}")
    
    # Get validation errors
    errors = config.get_validation_errors()
    print(f"Validation errors (should be empty): {errors}")
    
    print()


def example_profiles():
    """Example: Configuration profiles"""
    print("=== Example 3: Configuration Profiles ===")
    
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
    
    # Create production profile
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
    
    # Create config and apply profiles
    config = ConfigManager()
    config.add_profile("development", development)
    config.add_profile("production", production)
    
    # Apply development profile
    config.apply_profile("development")
    print(f"Development neuron count: {config.get('brain.neuron_count')}")
    print(f"Development debug mode: {config.get('agent.debug_mode')}")
    
    # Switch to production profile
    config.apply_profile("production")
    print(f"Production neuron count: {config.get('brain.neuron_count')}")
    print(f"Production debug mode: {config.get('agent.debug_mode')}")
    
    print()


def example_groups():
    """Example: Configuration groups"""
    print("=== Example 4: Configuration Groups ===")
    
    # Create groups
    brain_group = ConfigGroup("brain", "Brain configuration group")
    brain_group.add_entry(
        ConfigEntry("brain.neuron_count", 1000,
                   ConfigSource.DEFAULT,
                   "Number of neurons",
                   "integer",
                   True,
                   groups=["brain"])
    )
    
    brain_group.add_entry(
        ConfigEntry("brain.synapse_density", 0.1,
                   ConfigSource.DEFAULT,
                   "Synapse density",
                   "number",
                   False,
                   groups=["brain"])
    )
    
    world_group = ConfigGroup("world", "World configuration group")
    world_group.add_entry(
        ConfigEntry("world.width", 20,
                   ConfigSource.DEFAULT,
                   "World width",
                   "integer",
                   True,
                   groups=["world"])
    )
    
    # Create config manager and add groups
    config = ConfigManager()
    config.add_group(brain_group)
    config.add_group(world_group)
    
    # Set values
    config.set("brain.neuron_count", 2000)
    config.set("world.width", 30)
    
    # Get configuration by group
    brain_config = config.get_by_group("brain")
    world_config = config.get_by_group("world")
    print(f"Brain config: {brain_config}")
    print(f"World config: {world_config}")
    
    # Get keys by group
    print(f"Brain keys: {config.get_keys('brain')}")
    print(f"World keys: {config.get_keys('world')}")
    
    print()


def example_serialization():
    """Example: Configuration serialization"""
    print("=== Example 5: Configuration Serialization ===")
    
    # Create config with values
    config = ConfigManager()
    config.set("brain.neuron_count", 1000)
    config.set("world.width", 20)
    config.set("agent.enabled", True)
    
    # Save to JSON
    config.save_to_file("serialization_config.json", "json")
    print("Saved to serialization_config.json")
    
    # Load from JSON
    config2 = ConfigManager()
    config2.load_from_file("serialization_config.json", "json")
    print(f"Loaded config: {config2.get_all()}")
    
    # Export to dictionary
    exported = config.export_config(["brain.neuron_count", "world.width"])
    print(f"Exported config: {exported}")
    
    # Load from dictionary
    config3 = ConfigManager()
    config3.import_config(exported)
    print(f"Imported config: {config3.get_all()}")
    
    # Test with string
    json_string = """
    {
        "brain.neuron_count": 1500,
        "world.width": 25
    }
    """
    config4 = ConfigManager()
    config4.load_from_string(json_string, "json")
    print(f"Loaded from string: {config4.get_all()}")
    
    print()


def example_monitoring():
    """Example: Configuration monitoring"""
    print("=== Example 6: Configuration Monitoring ===")
    
    # Create config with monitoring enabled
    config = ConfigManager(enable_monitoring=True, log_changes=True)
    
    # Track changes with a callback
    change_count = 0
    change_log = []
    
    def change_callback(change):
        nonlocal change_count, change_log
        change_count += 1
        change_info = {
            "change_number": change_count,
            "key": change.key,
            "old_value": change.old_value,
            "new_value": change.new_value,
            "source": change.source.value,
            "timestamp": change.timestamp
        }
        change_log.append(change_info)
        print(f"Change #{change_count}: {change.key} = {change.old_value} -> {change.new_value}")
    
    config.add_change_callback(change_callback)
    
    # Set initial values
    config.set("test.value1", "initial")
    config.set("test.value2", 100)
    
    # Change values
    config.set("test.value1", "updated")
    config.set("test.value2", 200)
    
    print(f"Total changes detected: {change_count}")
    print(f"Change log: {change_log}")
    
    # Note: In a real implementation, the monitor would also check for changes
    # in the background and trigger callbacks.
    
    print()


def example_schema_validation():
    """Example: Schema validation"""
    print("=== Example 7: Schema Validation ===")
    
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
                "maximum": 150
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
    
    # Get validation errors
    errors = config.get_validation_errors()
    print(f"Validation errors (should be empty): {errors}")
    
    # Try to set value that violates schema
    try:
        config.set("name", "A")  # Too short
    except ValueError as e:
        print(f"Schema validation error (expected): {e}")
    
    print()


def example_inheritance():
    """Example: Configuration inheritance"""
    print("=== Example 8: Configuration Inheritance ===")
    
    # Create parent config
    parent = ConfigManager()
    parent.set("brain.neuron_count", 1000)
    parent.set("world.width", 20)
    parent.set("agent.enabled", True)
    
    # Create child config
    child = ConfigManager()
    child.set_parent(parent)
    child.inherit_from_parents()
    
    # Child can access parent's values
    print(f"Child neuron count: {child.get('brain.neuron_count')}")  # 1000
    print(f"Child world width: {child.get('world.width')}")        # 20
    print(f"Child agent enabled: {child.get('agent.enabled')}")  # True
    
    # Override parent's value
    child.set("brain.neuron_count", 2000)
    print(f"Child overridden neuron count: {child.get('brain.neuron_count')}")  # 2000
    
    # Check if parent values are still accessible
    print(f"Child still has world width: {child.has('world.width')}")  # True
    print(f"Child world width value: {child.get('world.width')}")        # 20
    
    print()


def example_merging():
    """Example: Configuration merging"""
    print("=== Example 9: Configuration Merging ===")
    
    # Create first config
    config1 = ConfigManager()
    config1.set("brain.neuron_count", 1000)
    config1.set("world.width", 20)
    
    # Create second config
    config2 = ConfigManager()
    config2.set("agent.enabled", True)
    config2.set("plasticity.enabled", True)
    
    # Merge config2 into config1
    config1.merge_config(config2, strategy="overwrite")
    
    print(f"Merged config includes brain.neuron_count: {config1.has('brain.neuron_count')}")
    print(f"Merged config includes world.width: {config1.has('world.width')}")
    print(f"Merged config includes agent.enabled: {config1.has('agent.enabled')}")
    print(f"Merged config includes plasticity.enabled: {config1.has('plasticity.enabled')}")
    
    print(f"All merged config values: {config1.get_all()}")
    
    print()


def example_copy():
    """Example: Configuration copying"""
    print("=== Example 10: Configuration Copying ===")
    
    # Create original config
    original = ConfigManager()
    original.set("brain.neuron_count", 1000)
    original.set("world.width", 20)
    original.set("agent.enabled", True)
    
    # Copy config
    copy = original.copy()
    print(f"Copy neuron count: {copy.get('brain.neuron_count')}")  # 1000
    
    # Modify copy
    copy.set("brain.neuron_count", 2000)
    
    print(f"Original neuron count: {original.get('brain.neuron_count')}")  # 1000
    print(f"Copy neuron count after modification: {copy.get('brain.neuron_count')}")  # 2000
    print(f"Original still has agent.enabled: {original.has('agent.enabled')}")  # True
    
    print()


def main():
    """Run all examples"""
    print("NLM Configuration Manager Examples")
    print("=" * 50)
    print()
    
    example_basic_config()
    example_validation()
    example_profiles()
    example_groups()
    example_serialization()
    example_monitoring()
    example_schema_validation()
    example_inheritance()
    example_merging()
    example_copy()
    
    print("=" * 50)
    print("All examples completed!")
    print()
    print("Note: Some features require additional packages:")
    print("- YAML support requires PyYAML")
    print("- TOML support requires tomli")
    print("- Schema validation requires jsonschema")


if __name__ == "__main__":
    main()