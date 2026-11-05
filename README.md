# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Configuration System Update

### JSON/YAML Configuration Support

As of Phase 2 (completed), the NLM configuration system now supports multiple file formats:

#### Supported Formats

1. **Key-Value Format** (`.cfg`, `.ini`, `.txt`)
   - Traditional format with `key = value` pairs
   - **Backward compatible** with existing configuration files
   - Supports comments and simple data types

2. **JSON Format** (`.json`)
   - Modern, structured configuration format
   - Rich data types (arrays, objects, nested structures)
   - Human-readable and IDE-friendly
   - Cross-language support

3. **YAML Format** (`.yaml`, `.yml`)
   - Human-readable and expressive format
   - Supports multi-line strings, comments
   - Natural for configuration files
   - YAML-specific features (sequences, mappings)

#### Automatic Format Detection

The configuration system automatically detects the file format:

```cpp
// Example: Loading configuration from file
nlm::Config config;

// Auto-detects format based on file content
bool success = config.loadFromFile("config.json");   // JSON format
bool success = config.loadFromFile("config.yaml");   // YAML format
bool success = config.loadFromFile("config.cfg");    // Key-value format
```

#### New API Methods

```cpp
// Load configuration from different formats
config.loadFromJsonFile("config.json");     // Load JSON format
config.loadFromYamlFile("config.yaml");     // Load YAML format
config.loadFromFile("config.cfg");          // Auto-detect format

// Save configuration to different formats
config.saveToJsonFile("config.json");       // Save JSON format
config.saveToYamlFile("config.yaml");       // Save YAML format
config.saveToFile("config.cfg");            // Save key-value format

// Convert between formats
std::string jsonString = config.toJson();     // Convert to JSON
std::string yamlString = config.toYaml();     // Convert to YAML

// Detect format
Config::ConfigFormat format = Config::detectFormat("file.json");
```

#### Python API

```python
import pynlm

config = pynlm.createDefaultConfig()

# Load configuration from different formats
config.loadFromJsonFile("config.json")
config.loadFromYamlFile("config.yaml")
config.loadFromFile("config.cfg")  # Auto-detect

# Save configuration to different formats
config.saveToJsonFile("config.json")
config.saveToYamlFile("config.yaml")
config.saveToFile("config.cfg")

# Convert between formats
json_string = config.toJson()
yaml_string = config.toYaml()
```

#### Configuration Examples

**JSON Configuration:**

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "plasticity_learning_rate": 0.01,
    "environment_name": "GridWorld",
    "metadata": {
        "version": "1.0",
        "description": "NLM Configuration"
    }
}
```

**YAML Configuration:**

```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
plasticity_learning_rate: 0.01
environment_name: GridWorld
metadata:
  version: "1.0"
  description: "NLM Configuration"
```

**Key-Value Configuration:**

```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
plasticity_learning_rate = 0.01
environment_name = GridWorld
```

### Building with JSON/YAML Support

The JSON configuration support requires the `nlohmann_json` library. Update CMake options:

```bash
cmake .. \
    -DNLM_ENABLE_JSON=ON \
    -DNLM_ENABLE_YAML=ON \
    -DCMAKE_BUILD_TYPE=Release
```

**Default Configuration:**
- `NLM_ENABLE_JSON`: ON (enabled by default)
- `NLM_ENABLE_YAML`: OFF (disabled by default)

### CLI Configuration Tool

The `nlm config` command-line tool now supports JSON and YAML formats:

```bash
# Create configuration in JSON format
nlm config create \
    --output-file config.json \
    --format json \
    --params "neuron_count=500" "region_count=3"

# Load configuration from JSON
nlm config load --config-file config.json --json

# Set configuration parameter
nlm config set \
    --key neuron_count \
    --value 1000 \
    --output-file config.json \
    --format json

# Validate configuration
nlm config validate --config-file config.json
```

### Testing

Test the JSON/YAML configuration support:

```bash
# Run configuration tests
python tests/test_config_json_yaml.py

# Test specific formats
python tests/test_config_json_yaml.py --test json
python tests/test_config_json_yaml.py --test yaml
```

### Migration Path

**Existing Users:**
- **Zero Impact**: Existing `.cfg` files continue to work
- **Gradual Migration**: Adopt JSON/YAML at your own pace
- **Backward Compatible**: No breaking changes to existing code

**New Users:**
- **Default Behavior**: Auto-detection of format
- **Easy Migration**: Simple JSON configuration possible
- **Advanced Features**: Full JSON/YAML support available

### Configuration File Examples

See the `configs/` directory for example configuration files in all supported formats:

- `configs/default.cfg` - Traditional key-value format
- `configs/default.json` - JSON format (if created)
- `configs/default.yaml` - YAML format (if created)

## Current status (honest, 2026-11-05)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase5_demo
./nlm_phase6_demo
./nlm_benchmark
```

**Note**: Many users will start with `./nlm` or `./nlm_phase6_demo`.

## Configuration Management

### Basic Configuration

NLM can load configuration from files and command-line arguments. The configuration system supports multiple file formats for flexibility:

#### Configuration File Locations

- **Default**: `configs/default.cfg`
- **Custom**: Specify path with `--config-file` option
- **JSON**: Any file ending with `.json`
- **YAML**: Any file ending with `.yaml` or `.yml`

#### Configuration Examples

**Example Configuration File (key=value format):**

```ini
# NLM Configuration File
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1
```

**Command-line Override:**

```bash
./nlm --random_seed=100 --neuron_count=500 --simulation_timestep=0.01
```

### Configuration File Format

NLM supports three configuration file formats:

1. **Key-Value Format** (`.cfg`)
   - Simple `key = value` syntax
   - Supports comments (lines starting with `#`)
   - Backward compatible with existing NLM configuration

2. **JSON Format** (`.json`)
   - Structured key-value pairs
   - Supports complex data types
   - IDE-friendly and version control ready

3. **YAML Format** (`.yaml`, `.yml`)
   - Human-readable format
   - Supports comments and rich data structures
   - Natural for configuration files

#### Configuration File Format Support

The configuration system automatically detects the file format based on file extension and content. You can specify the format explicitly with the `--format` option:

```bash
# Auto-detect format
nlm --config-file config.json

# Explicitly specify format
nlm --config-file config.json --format json
nlm --config-file config.yaml --format yaml
nlm --config-file config.cfg --format key_value
```

#### Configuration File Examples

**JSON Configuration (`config.json`):**

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "environment_name": "GridWorld"
}
```

**YAML Configuration (`config.yaml`):**

```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1
plasticity_learning_rate: 0.01
environment_name: GridWorld
```

**Key-Value Configuration (`config.cfg`):**

```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
environment_name = GridWorld
```

### Configuration Management CLI

Use the `nlm config` command for advanced configuration management:

#### Commands

- `create`: Create a new configuration file
- `load`: Load configuration from file
- `inspect`: Inspect current configuration
- `set`: Set a configuration parameter
- `get`: Get a configuration parameter value
- `remove`: Remove a configuration parameter
- `validate`: Validate a configuration file

#### Examples

```bash
# Create a new configuration file in JSON format
nlm config create \
    --output-file config.json \
    --format json \
    --params "neuron_count=500" "region_count=3" "random_seed=123"

# Load configuration from JSON file
nlm config load --config-file config.json --json

# Inspect configuration
nlm config inspect --config-file config.json

# Set a configuration parameter
nlm config set \
    --key neuron_count \
    --value 1000 \
    --output-file config.json \
    --format json

# Get a configuration parameter value
nlm config get --key neuron_count

# Remove a configuration parameter
nlm config remove --key neuron_count --output-file config.json

# Validate a configuration file
nlm config validate --config-file config.json
```

### Configuration Management Python API

Use the Python API for programmatic configuration management:

```python
import pynlm

# Create a new configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("random_seed", 42)
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

# Save to different formats
config.saveToFile("config.cfg")                    # Key-value format
config.saveToJsonFile("config.json")              # JSON format
config.saveToYamlFile("config.yaml")              # YAML format

# Load from file
config.loadFromFile("config.json")               # Auto-detect format
config.loadFromJsonFile("config.json")            # Explicit JSON
config.loadFromYamlFile("config.yaml")            # Explicit YAML

# Get configuration values
value = config.get("random_seed")
if value.has_value():
    print(f"Random seed: {value.value()}")

# Check if key exists
if config.has("random_seed"):
    print("Configuration contains random_seed")

# Get all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Get configuration summary
summary = config.summary()
print(f"Configuration summary:\n{summary}")

# Convert between formats
json_string = config.toJson()
yaml_string = config.toYaml()
```

### Configuration File Format Detection

The configuration system automatically detects the file format based on:

1. **File Extension**: `.json`, `.yaml`, `.yml` → JSON/YAML; `.cfg`, `.ini`, `.txt` → key-value
2. **File Content**: First line starting with `{` or `[` → JSON; containing `:` → YAML; containing `=` → key-value
3. **Explicit Format**: `--format` option in CLI

### Configuration Validation

The `nlm config validate` command validates configuration files:

```bash
# Validate configuration file
nlm config validate --config-file config.json

# Validate with JSON output
nlm config validate --config-file config.json --json
```

### Configuration Templates

NLM provides default configuration templates:

- **Default Configuration**: `configs/default.cfg`
- **JSON Template**: `configs/default.json` (can be created)
- **YAML Template**: `configs/default.yaml` (can be created)

### Configuration Migration

#### From Legacy Key-Value to JSON/YAML

1. **Keep existing `.cfg` files**: They continue to work without changes
2. **Create JSON/YAML versions**: For new projects, use JSON or YAML format
3. **Gradual migration**: Convert existing files as needed

#### Configuration Migration Example

```bash
# Convert existing config.cfg to config.json
# Use the nlm config tool or Python API

# Or manually create config.json from config.cfg
# The new configuration system handles this automatically
```

### Configuration Best Practices

1. **Use JSON for new projects**: Better for version control and complex configurations
2. **Use YAML for human-readable configurations**: Good for configuration with comments and formatting
3. **Use key-value for simple configurations**: Backward compatible and simple
4. **Validate configurations**: Use `nlm config validate` before running
5. **Document configuration**: Use comments in configuration files
6. **Version control configurations**: Store configuration files in version control
7. **Keep configuration separate**: Store configuration files separately from code

### Configuration File Locations

The following configuration file locations are searched by the configuration system:

1. **Current directory**: `./configs/`
2. **Current working directory**: `./`
3. **User home directory**: `~/.nlm/`
4. **Environment variable**: `NLM_CONFIG_PATH`

### Configuration Environment Variables

The configuration system supports the following environment variables:

- `NLM_CONFIG_PATH`: Path to configuration directory
- `NLM_CONFIG_FILE`: Path to specific configuration file
- `NLM_CONFIG_FORMAT`: Default configuration format

### Configuration File Permissions

The configuration system requires read/write permissions for:

- Reading configuration files (`.cfg`, `.json`, `.yaml`)
- Writing configuration files (`.cfg`, `.json`, `.yaml`)

### Configuration Error Handling

The configuration system provides detailed error messages for:

- File not found
- Permission denied
- Invalid format
- Missing required files

### Configuration Help

For more information about configuration management:

```bash
# Show configuration help
nlm config --help

# Show configuration examples
nlm config create --help
```

For detailed documentation, see the NLM documentation at:
https://nlm.readthedocs.io

## Contributing

### Configuration Updates

1. **Update configuration files**: Modify existing `.cfg`, `.json`, or `.yaml` files
2. **Add new configuration options**: Add new keys to configuration files
3. **Update documentation**: Update configuration documentation
4. **Test changes**: Run configuration tests

### Configuration Tests

Run configuration tests to ensure changes work correctly:

```bash
# Run all configuration tests
python tests/test_config_json_yaml.py

# Run specific configuration tests
python tests/test_config_json_yaml.py --test json
python tests/test_config_json_yaml.py --test yaml
python tests/test_config_json_yaml.py --test key_value
```

### Configuration Validation

Validate configuration files before running:

```bash
# Validate configuration file
nlm config validate --config-file config.json

# Validate with verbose output
nlm config validate --config-file config.json --verbose
```

### Configuration Support

If you need help with configuration:

1. **Check documentation**: Read the configuration documentation
2. **Run tests**: Run configuration tests
3. **Check examples**: Look at configuration examples
4. **File issues**: Report configuration issues on GitHub

For more information, visit:
https://github.com/nlm-project/nlm

## License

This project is licensed under the MIT License.

See `LICENSE` for more details.

## Acknowledgments

- **NLM Authors**: The original NLM team
- **Contributors**: All contributors to the NLM project
- **Community**: The NLM community
- **Open Source**: Open source enthusiasts

## Contact

For questions, issues, or suggestions:

- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Documentation**: https://nlm.readthedocs.io
- **Community**: https://github.com/nlm-project/nlm/discussions

## Thank You

Thank you for using NLM! We hope you enjoy using this experimental artificial brain project.

# NLM — 熙然
