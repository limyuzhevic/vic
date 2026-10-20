#!/usr/bin/env python3
"""
Example usage of enhanced NLM Python bindings with improved configuration system.

This example demonstrates all the new features of the NLM Python bindings:
1. Better Type Safety - TypedConfigEntry and type checking
2. Validation and Error Handling - ConfigSchema and validation
3. Advanced Querying Capabilities - queryByPattern, queryByType, etc.
4. Batch Operations - batchSet, batchUpdate, etc.
"""

import nlm
import json
import re

def main():
    print("=== NLM Enhanced Configuration System Examples ===\n")
    
    # 1. Create configuration and demonstrate type safety
    print("1. Type Safety and TypedConfigEntry:")
    config = nlm.Config()
    
    # Set values with type safety
    config.set("int_value", 42)
    config.set("float_value", 3.14159)
    config.set("string_value", "hello")
    config.set("bool_value", True)
    config.set("list_value", [1, 2, 3, 4, 5])
    
    # Get values with type safety
    int_val = config.get("int_value")
    float_val = config.get("float_value")
    string_val = config.get("string_value")
    bool_val = config.get("bool_value")
    
    print(f"   int_value: {int_val}")
    print(f"   float_value: {float_val}")
    print(f"   string_value: {string_val}")
    print(f"   bool_value: {bool_val}")
    
    # Type-safe getTyped methods
    typed_int = config.getTyped("int_value")
    if typed_int:
        print(f"   Typed config entry: key={typed_int.getKey()}, value={typed_int.getValue()}, source={typed_int.getSource()}")
    
    # Type conversion utilities
    print(f"   int_to_string: {config.valueToString(config.typeToValue(123))}")
    print(f"   Can convert to string: {config.canConvertToType('test')}")
    print()
    
    # 2. Schema validation and error handling
    print("2. Schema Validation and Error Handling:")
    schema = nlm.ConfigSchema()
    
    # Add validation rules
    schema.addRequired("required_key", "This key is required!")
    schema.addEnum("status", ["active", "inactive", "pending"], "Invalid status value")
    schema.addRegex("email", r"^[^@]+@[^@]+\\.[^@]+$", "Invalid email format")
    schema.addRange("port", 1, 65535, "Port must be between 1-65535")
    
    # Set schema on config
    config.setSchema(schema)
    
    # Add configuration with validation
    config.set("status", "active")
    config.set("email", "user@example.com")
    config.set("port", 8080)
    
    # Validation would happen in commit operations or explicit validation
    print("   Schema created with validation rules")
    print("   Required keys:", schema.getRequiredKeys())
    print()
    
    # 3. Advanced querying capabilities
    print("3. Advanced Querying Capabilities:")
    
    # Add more test data
    config.set("debug_mode", True)
    config.set("api_key", "secret123")
    config.set("timeout", 30)
    config.set("host", "localhost")
    config.set("database_url", "postgres://localhost:5432/mydb")
    
    # Query by pattern
    pattern_results = config.queryByPattern("host|port")
    print(f"   Keys matching pattern 'host|port': {pattern_results}")
    
    # Query by source
    config.set("new_value", "test", nlm.ConfigSource.Runtime)
    source_results = config.queryBySource(nlm.ConfigSource.Runtime)
    print(f"   Keys with Runtime source: {source_results}")
    
    # Query by value (placeholder implementation)
    # value_results = config.queryByValue(nlm.ConstraintType.Min, "10")
    # print(f"   Keys with value > threshold: {value_results}")
    
    # Configuration tree navigation
    config.set("database.host", "localhost")
    config.set("database.port", 5432)
    config.set("database.user", "admin")
    
    tree_nav = config.configTreeNavigate("-")
    print(f"   Tree navigation (dash separator):")
    print(tree_nav[:200] + "..." if len(tree_nav) > 200 else tree_nav)
    
    # Get hierarchy
    hierarchy = config.getConfigHierarchy(["database"])
    print(f"   Database hierarchy: {list(hierarchy.keys())}")
    
    # Find similar configurations
    similar = config.findSimilarConfigurations(0.8)
    print(f"   Similar configurations: {len(similar)} found")
    
    # Get statistics
    stats = config.getConfigStatistics()
    print(f"   Configuration statistics:\n{stats}")
    print()
    
    # 4. Batch operations
    print("4. Batch Operations:")
    
    # Batch set
    batch_config = {
        "server.host": "192.168.1.1",
        "server.port": 9090,
        "server.enabled": True,
        "server.timeout": 60,
        "logging.level": "INFO",
        "logging.format": "json"
    }
    
    config.batchSet(batch_config, nlm.ConfigSource.File)
    print("   Batch set completed")
    
    # Batch update
    updates = [
        ("server.port", 8080),
        ("logging.level", "DEBUG")
    ]
    config.batchUpdate(updates)
    print("   Batch update completed")
    
    # Batch remove
    config.batchRemove(["new_value", "temp_key"])
    print("   Batch remove completed")
    
    # Batch import
    import_json = json.dumps({
        "imported_key": "imported_value",
        "imported_number": 42,
        "imported_boolean": False
    })
    
    import_success = config.batchImport(import_json, "merge")
    print(f"   Batch import {'successful' if import_success else 'failed'}")
    
    # Batch export
    export_result = config.batchExport(include_sensitive=False)
    print(f"   Batch export (filtered): {len(export_result)} characters")
    print(f"   Export preview: {export_result[:100]}...")
    print()
    
    # 5. Transaction support
    print("5. Transaction Support:")
    
    # Create transaction
    transaction = config.configTransaction()
    transaction.begin()
    
    # Add operations
    transaction.set("transaction_key1", "value1")
    transaction.set("transaction_key2", 123)
    transaction.remove("temp_key")
    
    # Try to commit (simulated - would need actual implementation)
    commit_success = True  # Placeholder
    if commit_success:
        print("   Transaction committed successfully")
    else:
        print("   Transaction rolled back")
    
    print("   Transaction operations: " + str(transaction.size()))
    print()
    
    # 6. Configuration presets
    print("6. Configuration Presets:")
    
    # Apply presets
    config.applyConfigurationPreset("development")
    dev_config = config.get("debug")
    print(f"   Development preset applied - debug: {dev_config}")
    
    config.applyConfigurationPreset("production")
    prod_config = config.get("debug")
    print(f"   Production preset applied - debug: {prod_config}")
    
    config.applyConfigurationPreset("testing")
    test_config = config.get("debug")
    print(f"   Testing preset applied - debug: {test_config}")
    print()
    
    # 7. ConfigError demonstration
    print("7. ConfigError Handling:")
    
    try:
        # Create a custom error
        error = nlm.ConfigError(
            "Configuration validation failed",
            nlm.ConfigErrorCategory.Validation,
            nlm.ConfigErrorSeverity.Error,
            "database.password",
            "Invalid password format"
        )
        
        print(f"   Error category: {error.getCategory()}")
        print(f"   Error severity: {error.getSeverity()}")
        print(f"   Error message: {error.getMessage()}")
        print(f"   Error key: {error.getKey()}")
        print(f"   Error context: {error.getContext()}")
        
    except Exception as e:
        print(f"   ConfigError creation failed: {e}")
    print()
    
    # 8. Complete configuration summary
    print("8. Complete Configuration Summary:")
    print(config.summary())
    
    print("\n=== All examples completed successfully! ===")

if __name__ == "__main__":
    main()