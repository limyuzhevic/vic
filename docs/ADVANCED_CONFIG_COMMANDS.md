# Advanced Configuration Commands - NLM Phase 6 Documentation

## Overview

This document describes the advanced configuration commands and features available in NLM Phase 6, which extend the original configuration system to provide comprehensive file management, validation, conversion, and analysis capabilities.

## Configuration Command Categories

### 1. Config File Management Commands

#### `config list [directory]`
**Description:** List available config files in a directory
**Usage:** 
```bash
config list                    # List files in current directory
config list ./my_configs       # List files in specific directory
```
**Output:** Displays all config files (.cfg, .json, .yaml, .yml) with file sizes and modification timestamps

#### `config validate [file]`
**Description:** Validate config file format (JSON/YAML)
**Usage:**
```bash
config validate configs/neural_network.json
config validate configs/experiment.yaml
```
**Output:** Reports whether the file is valid and can be parsed

#### `config convert [input] [output]`
**Description:** Convert between JSON and YAML formats
**Usage:**
```bash
config convert configs/network.json configs/network.yaml
config convert configs/experiment.yaml configs/experiment.json
```
**Output:** Converts input file to output file in the target format

#### `config merge [files...] [output]`
**Description:** Merge multiple config files
**Usage:**
```bash
config merge configs/simple.cfg configs/advanced.json configs/experiment.yaml configs/combined.cfg
```
**Output:** Creates a merged configuration file with all settings from input files

#### `config diff [file1] [file2]`
**Description:** Show differences between config files
**Usage:**
```bash
config diff configs/before.json configs/after.json
```
**Output:** Detailed comparison showing added, removed, and modified settings

#### `config backup [file]`
**Description:** Create backup of config with timestamp
**Usage:**
```bash
config backup configs/neural_network.json
```
**Output:** Creates timestamped backup file with the original content preserved

### 2. Advanced Config Operations

#### `config extract [file] [keys...]`
**Description:** Extract specific keys/patterns to new config
**Usage:**
```bash
config extract configs/complex.json neuron_count region_count connection_probability
config extract configs/experiment.yaml environment.type environment.dimensions
```
**Output:** Creates a new config file containing only the specified keys

#### `config template [type] [file]`
**Description:** Generate template configs with examples
**Usage:**
```bash
config template neural my_neural_template.cfg
config template simple my_simple_config.cfg
```
**Output:** Creates configuration templates for different use cases (neural, simple, etc.)

#### `config statistics [file]`
**Description:** Show config statistics (types, sources, etc.)
**Usage:**
```bash
config statistics configs/neural_network.json
```
**Output:** Comprehensive statistics about config entries including type distribution and source analysis

#### `config search [file] [pattern]`
**Description:** Search for config keys with patterns
**Usage:**
```bash
config search configs/complex.json "neuron*"
config search configs/experiment.yaml "^environment"
```
**Output:** Finds all keys matching the specified pattern (supports regex)

#### `config migrate [file] [version]`
**Description:** Migrate old config format to new format
**Usage:**
```bash
config migrate configs/old_format.cfg v2.0
```
**Output:** Converts old configuration format to the target version

### 3. Learning and Performance Commands

#### `brain status`
**Description:** Show brain system status and metrics
**Usage:**
```bash
brain status
```
**Output:** Displays comprehensive brain system information including neuron counts, spike statistics, and region information

#### `memory stats`
**Description:** Show memory system statistics
**Usage:**
```bash
memory stats
```
**Output:** Shows working memory, episodic memory, and associative memory statistics

#### `neuron info [region]`
**Description:** Show neuron population information
**Usage:**
```bash
neuron info                    # Region 1 (default)
neuron info 2                  # Region 2
```
**Output:** Displays neuron type distribution, counts, and population information

#### `plasticity report`
**Description:** Show plasticity rule status and parameters
**Usage:**
```bash
plasticity report
```
**Output:** Displays STDP, Hebbian, and structural plasticity rule status and parameters

#### `neuromodulation status`
**Description:** Show neuromodulation system state
**Usage:**
```bash
neuromodulation status
```
**Output:** Shows dopamine, curiosity, novelty, and prediction error system states

### 4. Agent Commands

#### `agent learn [params]`
**Description:** Start learning session with custom parameters
**Usage:**
```bash
agent learn --rate 0.01 --episodes 1000 --mode exploration
agent learn --rate 0.05 --episodes 500 --mode exploitation
```
**Output:** Initiates learning session with specified parameters

#### `agent evaluate [file]`
**Description:** Evaluate agent performance
**Usage:**
```bash
agent evaluate ./results/agent_performance.json
```
**Output:** Evaluates agent performance metrics from saved results

#### `agent save [file] [metadata]`
**Description:** Save agent state with metadata
**Usage:**
```bash
agent save ./checkpoints/agent_state.json "Learning completed successfully"
```
**Output:** Saves agent state and associated metadata for later restoration

#### `agent load [file]`
**Description:** Load saved agent state
**Usage:**
```bash
agent load ./checkpoints/agent_state.json
```
**Output:** Restores agent state from saved file

#### `agent profile`
**Description:** Show agent capabilities and limitations
**Usage:**
```bash
agent profile
```
**Output:** Displays agent's capabilities, supported features, and current limitations

### 5. Environment and World Commands

#### `env create [type] [name] [params]`
**Description:** Create new environment with specified parameters
**Usage:**
```bash
env create simulated neural_world --width 100 --height 100 --depth 10
env create simple grid_world --size 50
```
**Output:** Creates new environment with specified type and parameters

#### `env list`
**Description:** List available environments
**Usage:**
```bash
env list
```
**Output:** Displays all available environment types and descriptions

#### `env info [name]`
**Description:** Show environment details
**Usage:**
```bash
env info neural_world
```
**Output:** Displays detailed information about specified environment

#### `world reset`
**Description:** Reset world state
**Usage:**
```bash
world reset
```
**Output:** Resets world to initial state

#### `world pause [enable]`
**Description:** Pause/unpause world simulation
**Usage:**
```bash
world pause 1    # Pause simulation
world pause 0    # Resume simulation (default)
```
**Output:** Toggles simulation pause state

## Configuration File Examples

### Basic Configuration (`basic.cfg`)
```
# Basic Neural Network Configuration
neuron_count = 500
region_count = 5
connection_probability = 0.12
simulation_timestep = 0.001

stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

working_memory_capacity = 100
e pisodic_memory_max_episodes = 200
```

### JSON Configuration (`experiment.json`)
```json
{
  "name": "Complex Learning Experiment",
  "parameters": {
    "neuron_count": 2000,
    "region_count": 20,
    "connection_probability": 0.18
  },
  "plasticity": {
    "stdp": {
      "ltp_weight": 0.025,
      "ltd_weight": 0.02,
      "tau": 25.0
    }
  }
}
```

### YAML Configuration (`advanced.yaml`)
```yaml
name: Advanced Neural Network
parameters:
  neuron_count: 2000
  region_count: 20
  connection_probability: 0.18

plasticity:
  stdp:
    ltp_weight: 0.025
    ltd_weight: 0.02
    tau: 25.0
  hebbian:
    learning_rate: 0.015

environment:
  type: simulated
  dimensions:
    width: 200
    height: 200
    depth: 20
```

## Usage Examples

### Example 1: Working with Config Files
```bash
# List available config files
config list ./configs

# Validate a JSON config
config validate configs/experiment.json

# Convert JSON to YAML
config convert configs/network.json configs/network.yaml

# Create a backup
config backup configs/neural_network.json

# Extract specific settings
config extract configs/complex.json neuron_count region_count environment.type
```

### Example 2: Analyzing Brain Systems
```bash
# Get brain status
brain status

# Check memory statistics
memory stats

# Show neuron information
neuron info

# Display plasticity status
plasticity report

# Check neuromodulation
neuromodulation status
```

### Example 3: Agent Management
```bash
# Save agent state with metadata
agent save ./checkpoints/agent_state.json "Completed 100 episodes"

# Load agent state
agent load ./checkpoints/agent_state.json

# Start learning session
agent learn --rate 0.01 --episodes 1000

# View agent capabilities
agent profile
```

### Example 4: Environment and World Control
```bash
# List available environments
env list

# Create custom environment
env create simulated my_world --width 100 --height 100 --depth 10

# Get environment info
env info my_world

# Reset world state
world reset

# Pause simulation
world pause 1
```

## Configuration Templating

The `config template` command generates configuration templates for common use cases:

### Neural Templates
- `neural`: Standard neural network configuration
- `deep_neural`: Complex multi-region neural network
- `adaptive`: Configuration for adaptive learning

### Simple Templates
- `simple`: Basic configuration for small networks
- `minimal`: Bare-bones configuration
- `example`: Complete working example

Example usage:
```bash
config template neural neural_template.cfg
config template simple simple_template.cfg
```

## Configuration Statistics

The `config statistics` command provides detailed analysis of configuration files:

### Sample Output
```
Configuration Statistics:
  Total entries: 25
  By type:
    int=8, double=5, string=6, int[]=3, double[]=2, string[]=1
  By source:
    Default=12, File=8, CommandLine=3, Runtime=2
```

### Analysis Types
- **Type Distribution**: Count of different value types in the configuration
- **Source Distribution**: Breakdown of configuration sources
- **File Information**: Size, modification time, and format detection

## Error Handling

All configuration commands include robust error handling:

### Common Error Messages
- `"Error: Config file path required for validate command"`
- `"Error: Unknown config subcommand: [command]"`
- `"✗ Config file is invalid or cannot be parsed!"`
- `"✓ Operation completed successfully!"`

### Validation Rules
- File format validation (JSON, YAML, plain text)
- Required parameter checking
- File existence verification
- Format compatibility checking

## Integration with NLM Systems

The advanced configuration commands integrate seamlessly with all NLM systems:

### Config Commands
- Enhanced `Config` class with new methods
- Backward compatibility maintained
- Integration with existing JSON/YAML parsing
- Support for all existing configuration sources

### Brain System Integration
- `brain status` uses existing brain logging system
- `memory stats` accesses memory system statistics
- `plasticity report` uses existing plasticity rule interfaces
- `neuromodulation status` integrates with neuromodulator systems

### Agent System Integration
- `agent learn` uses agent brain interfaces
- `agent save/load` integrates with checkpoint system
- `agent profile` uses agent capability analysis

### Environment System Integration
- `env create/list/info` uses environment factory pattern
- `world reset/pause/step` integrates with world simulation loop

## File Management Features

### Config File Discovery
- Automatic file extension detection (.cfg, .json, .yaml, .yml)
- Recursive directory scanning
- Format-based file categorization

### Backup and Recovery
- Timestamped backup files
- Automatic file copying
- Error recovery on backup failure

### Format Conversion
- JSON ↔ YAML conversion
- Plain text → structured format parsing
- Format validation before conversion

## Performance Considerations

### Large Configuration Files
- Memory-efficient processing
- Streaming file reading for large files
- Lazy loading of configuration entries

### Batch Operations
- Efficient file merging algorithms
- Parallel processing for multiple operations
- Optimized memory usage for statistics

### Error Recovery
- Graceful handling of corrupted files
- Fallback to default configurations
- Comprehensive logging of operations

## Migration Guide

### From Old Config System to Advanced Commands

#### Phase 1: Basic Commands
1. Start with `config list` to discover available files
2. Use `config validate` to ensure file integrity
3. Practice `config statistics` for file analysis

#### Phase 2: Advanced Operations
1. Learn `config convert` for format switching
2. Use `config merge` for combining configurations
3. Implement `config diff` for version control

#### Phase 3: Full Integration
1. Integrate `brain status` and `memory stats` into monitoring
2. Add `agent save/load` for checkpointing
3. Utilize `world reset/pause` for simulation control

### Example Migration Script
```bash
#!/bin/bash

# Backup existing configs
config backup ./old_configs/*.cfg

# List new configs
config list ./new_configs

# Validate critical configs
config validate ./new_configs/neural_network.json

# Convert formats
config convert ./old_configs/experiment.json ./new_configs/experiment.yaml

# Merge configurations
config merge ./new_configs/experiment.yaml ./new_configs/network.cfg ./merged_config.cfg

# Analyze statistics
config statistics ./merged_config.cfg
```

## Troubleshooting

### Common Issues and Solutions

1. **"Config command not found"**
   - Ensure you're using the correct NLM version
   - Check that the main executable includes advanced commands
   - Verify build includes all config functionality

2. **"Invalid file format"**
   - Check file extension (.json, .yaml, .yml, .cfg)
   - Validate file syntax manually
   - Use `config validate` to identify issues

3. **"File not found"**
   - Verify file path is correct
   - Use absolute paths when in doubt
   - Check directory permissions

4. **"Permission denied"**
   - Check file write permissions
   - Use sudo for system directories
   - Ensure adequate disk space

### Debug Commands
```bash
# Show help for all commands
help

# Show version information
version

# Run default test suite
run 100

# Check for help
config help
brain help
agent help
```

## Future Enhancements

### Planned Features
1. **Config Schema Validation**: JSON Schema and YAML schema support
2. **Remote Config Management**: HTTP/REST API for remote configuration
3. **Config Diff Tools**: Advanced diff with color output and unified format
4. **Configuration Templates**: Community-shared configuration templates
5. **Performance Profiling**: Configuration impact on simulation performance
6. **Configuration Auditing**: Change tracking and history management

### Research Directions
- Machine learning for configuration optimization
- Automated configuration discovery
- Real-time configuration adaptation
- Configuration-based simulation control

## Conclusion

The advanced configuration commands in NLM Phase 6 provide a comprehensive toolkit for managing, analyzing, and optimizing neural network configurations. These commands enable:

- **Efficient file management** with backup, conversion, and validation
- **Powerful analysis** with statistics, search, and diff capabilities
- **Seamless integration** with all NLM systems
- **Robust error handling** for reliable operation
- **Extensible architecture** for future enhancements

These commands make NLM more powerful, user-friendly, and extensible while maintaining full backward compatibility with existing functionality.

## References

- NLM Documentation: https://github.com/NLM/nlm/docs
- Configuration Guidelines: https://github.com/NLM/nlm/configs
- Example Experiments: https://github.com/NLM/nlm/experiments
- Advanced Features: https://github.com/NLM/nlm/advanced-features

## Acknowledgments

This advanced configuration system was developed as part of NLM Phase 6 Final Integration, building upon the foundation of all previous phases and incorporating feedback from the NLM community.
