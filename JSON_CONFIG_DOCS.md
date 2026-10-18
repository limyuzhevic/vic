# JSON Configuration Support Documentation

## Overview

The NLM configuration system now supports both traditional key=value format (.cfg files) and JSON format (.json files). This document explains the new JSON configuration capabilities.

## JSON Configuration File Format

JSON configuration files use the standard JSON syntax with the following value types supported:

- **Strings**: `"value"`
- **Numbers**: `42`, `3.14`, `-5`
- **Booleans**: `true`, `false`
- **Arrays**: `[1, 2, 3]`, `["a", "b", "c"]`, `[1.1, 2.2, 3.3]`
- **Objects**: `{"key": "value", "number": 42}`

### Example JSON Configuration (`configs/default.json`)

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    
    "log_level": "INFO",
    "log_to_file": false,
    "log_filename": "nlm.log",
    
    "max_simulation_steps": 10000,
    "simulation_time_limit": 0.0,
    
    "visualization_enabled": false,
    "visualization_update_rate": 30.0,
    
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10
}
```

## Command Line Configuration

### Traditional Configuration
```bash
./nlm --config=configs/default.cfg
./nlm --config=file.json --key1=value1 --key2=value2
```

### JSON Configuration (Recommended)
```bash
./nlm --json-config=configs/default.json
./nlm --json-config=file.json
```

## Configuration Value Types

### Complex JSON Values

JSON supports nested structures through arrays and objects:

**Arrays:**
```json
"int_array": [1, 2, 3, 4, 5],
"float_array": [1.1, 2.2, 3.3],
"string_array": ["a", "b", "c"]
```

**Objects (Maps):**
```json
"complex_object": {
    "nested_key": "nested_value",
    "nested_number": 999
}
```

## Configuration Schema Validation

The configuration system includes schema validation for better type checking:

```cpp
std::vector<nlm::ConfigSchema> schema;
schema.emplace_back("name", "string", "Application name", true);
schema.emplace_back("version", "number", "Application version", true);
schema.emplace_back("enabled", "boolean", "Feature enabled", true);
```

Schema validation ensures:
- Required fields are present
- Type compatibility between JSON values and C++ types
- Better error messages for configuration issues

## Backward Compatibility

The configuration system maintains full backward compatibility:

1. **Traditional .cfg files**: Continue to work exactly as before
   - `key = value`
   - `# comment` for comments
   - Support for quoted strings

2. **Migration Path**: 
   - Create new `.json` files with complex configurations
   - Gradually migrate existing `.cfg` files to JSON format
   - Both formats can coexist in the same project

## Error Handling

### JSON Parsing Errors
- Malformed JSON: Clear error messages with parse location
- Type mismatches: Helpful warnings about expected types
- Missing files: Graceful fallback to default configuration

### Command Line Errors
- Invalid file paths: Clear error messages
- Missing required files: Warning messages but continue with defaults

## Configuration Class API

### New Methods

| Method | Description |
|--------|-------------|
| `loadFromJSONFile()` | Load configuration from JSON file specifically |
| `saveToJSONFile()` | Save configuration to JSON file specifically |
| `isJSONFile()` | Detect if file is JSON based on extension |
| `configValueToJSON()` | Convert internal ConfigValue to JSON |
| `jsonToConfigValue()` | Convert JSON to internal ConfigValue |
| `validateValueType()` | Validate schema type against ConfigValue |
| `validateSchema()` | Validate entire configuration against schema |

### Enhanced ConfigValue

The `ConfigValue` variant now supports:
- `std::map<std::string, ConfigValue>` - JSON objects
- `std::vector<ConfigValue>` - JSON arrays (nested)

## Usage Examples

### 1. Basic JSON Configuration
```bash
# Load JSON configuration
./nlm --json-config=configs/my_config.json

# Load traditional configuration (backward compatible)
./nlm --config=configs/old_config.cfg
```

### 2. Mixed Configuration
```bash
# Load JSON base config, then override with command line
./nlm --json-config=configs/default.json --random_seed=123 --neuron_count=1000
```

### 3. Schema Validation in Code
```cpp
nlm::Config config;
config.loadFromJSONFile("config.json");

std::vector<nlm::ConfigSchema> schema = {
    {"random_seed", "number", "Random seed", true},
    {"neuron_count", "number", "Number of neurons", true},
    {"environment_name", "string", "Environment name", true}
};

if (!config.validateSchema(schema)) {
    // Handle validation errors
}
```

## Best Practices

### 1. Use JSON for Complex Configurations
- Use JSON for configurations with nested structures
- Use traditional format for simple key=value pairs

### 2. Validate Your Configurations
- Always validate configuration schemas in production
- Use different schemas for different environments (dev, test, prod)

### 3. Maintain Backward Compatibility
- Keep existing .cfg files working
- Provide migration paths for gradual adoption

### 4. Error Handling
- Always handle file not found errors gracefully
- Provide clear error messages for malformed JSON

## Testing

Run the JSON configuration tests:
```bash
./nlm_test  # Includes JSON configuration tests
```

Or run the specific JSON test program:
```bash
./test_json_config
```

## Future Enhancements

- YAML configuration support (planned)
- More advanced schema validation
- Configuration inheritance
- Environment-specific configuration loading

## License

See LICENSE file for full license information.
